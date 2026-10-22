#include "InspectionProcess.h"
#include "application/inspection/InspectionWireCodec.h"
#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QLocalServer>
#include <QLocalSocket>
#include <QProcess>
#include <QUuid>
#include <QtEndian>
#include <algorithm>

namespace {
constexpr quint32 MaximumPacket = 64 * 1024 * 1024;
void checkBudget(const QElapsedTimer& age, int timeout,
    const std::shared_ptr<std::atomic_bool>& cancelled)
{
    if (cancelled && cancelled->load()) throw std::runtime_error("检测任务已取消");
    if (timeout >= 0 && age.elapsed() >= timeout) throw std::runtime_error("独立检测进程响应超时");
}
QByteArray readBytes(QLocalSocket& socket, int count, const QElapsedTimer& age,
    int timeout, const std::shared_ptr<std::atomic_bool>& cancelled)
{
    QByteArray bytes;
    while (bytes.size() < count) {
        checkBudget(age, timeout, cancelled);
        if (socket.bytesAvailable()) bytes += socket.read(count - bytes.size());
        else if (socket.state() != QLocalSocket::ConnectedState)
            throw std::runtime_error("检测进程连接已断开");
        else socket.waitForReadyRead(25);
    }
    return bytes;
}
QJsonObject readPacket(QLocalSocket& socket, const QElapsedTimer& age, int timeout,
    const std::shared_ptr<std::atomic_bool>& cancelled = {})
{
    const auto header = readBytes(socket, 4, age, timeout, cancelled);
    const auto size = qFromBigEndian<quint32>(reinterpret_cast<const uchar*>(header.constData()));
    if (!size || size > MaximumPacket) throw std::runtime_error("检测进程消息超过上限");
    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(readBytes(socket, int(size), age, timeout, cancelled), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject())
        throw std::runtime_error("检测进程返回格式无效");
    return doc.object();
}
void writePacket(QLocalSocket& socket, const QJsonObject& value, const QElapsedTimer& age,
    int timeout, const std::shared_ptr<std::atomic_bool>& cancelled = {})
{
    const auto body = QJsonDocument(value).toJson(QJsonDocument::Compact);
    if (body.isEmpty() || quint64(body.size()) > MaximumPacket)
        throw std::runtime_error("检测进程消息超过上限");
    QByteArray packet(4, '\0');
    qToBigEndian<quint32>(quint32(body.size()), reinterpret_cast<uchar*>(packet.data()));
    packet += body;
    if (socket.write(packet) != packet.size()) throw std::runtime_error("无法提交检测进程消息");
    while (socket.bytesToWrite()) {
        checkBudget(age, timeout, cancelled);
        if (socket.state() != QLocalSocket::ConnectedState) throw std::runtime_error("检测进程连接已断开");
        socket.waitForBytesWritten(25);
    }
}
}

struct InspectionProcess::Impl {
    QString program;
    int timeoutMs;
    std::unique_ptr<QProcess> process;
    std::unique_ptr<QLocalSocket> socket;
    void stop()
    {
        if (socket) { socket->abort(); socket.reset(); }
        if (!process) return;
        if (process->state() != QProcess::NotRunning) {
            process->kill();
            // Do not enter QProcess's unbounded destructor wait if a kernel task is unkillable.
            if (!process->waitForFinished(500)) {
                (void)process.release();
                return;
            }
        }
        process.reset();
    }
    ~Impl() { stop(); }
    void start(const std::shared_ptr<std::atomic_bool>& cancelled)
    {
        if (process && process->state() == QProcess::Running
            && socket && socket->state() == QLocalSocket::ConnectedState) return;
        stop();
        QLocalServer listener;
        listener.setSocketOptions(QLocalServer::UserAccessOption);
        // macOS sockaddr_un has a small path limit, including the per-user temp directory.
        const auto token = QUuid::createUuid().toString(QUuid::WithoutBraces).remove('-').left(16);
        const auto name = QDir::tempPath() + "/hi-" + token;
        if (!listener.listen(name)) throw std::runtime_error(
            (QStringLiteral("无法建立本地检测进程通道：") + listener.errorString()).toStdString());
        process = std::make_unique<QProcess>();
        process->setStandardOutputFile(QProcess::nullDevice());
        process->setStandardErrorFile(QProcess::nullDevice());
        process->start(program, {"--inspection-worker", name});
        if (!process->waitForStarted(2000)) throw std::runtime_error("无法启动独立检测进程");
        QElapsedTimer age;
        age.start();
        while (!listener.hasPendingConnections()) {
            checkBudget(age, 3000, cancelled);
            if (process->state() == QProcess::NotRunning) throw std::runtime_error("独立检测进程提前退出");
            listener.waitForNewConnection(25);
        }
        socket.reset(listener.nextPendingConnection());
        socket->setParent(nullptr);
        socket->setReadBufferSize(MaximumPacket + 4);
    }
};

InspectionProcess::InspectionProcess(QString program, int timeoutMs) : _impl(std::make_unique<Impl>())
{
    _impl->program = program.isEmpty() ? QCoreApplication::applicationFilePath() : program;
    _impl->timeoutMs = qMax(1, timeoutMs);
}
InspectionProcess::~InspectionProcess() = default;
QJsonObject InspectionProcess::execute(const InspectionRequest& request)
{
    try {
        _impl->start(request.cancelled);
        QElapsedTimer age;
        age.start();
        writePacket(*_impl->socket, InspectionWireCodec::encode(request), age, _impl->timeoutMs, request.cancelled);
        auto result = readPacket(*_impl->socket, age, _impl->timeoutMs, request.cancelled);
        if (result["inspectionId"].toString() != request.trace.inspectionId)
            throw std::runtime_error("检测进程响应编号不匹配");
        return result;
    } catch (const std::exception& error) {
        _impl->stop();
        return {{"scheme", request.scheme}, {"inspectionId", request.trace.inspectionId},
            {"status", "ERROR"}, {"recoveryRequired", true}, {"error", QString::fromUtf8(error.what())}};
    }
}
QJsonObject runIsolatedInspection(const InspectionRequest& request)
{
    bool valid = false;
    const auto value = qEnvironmentVariableIntValue("HANS_INSPECTION_TIMEOUT_MS", &valid);
    const int timeout = valid && value >= 1 && value <= 600000 ? value : 60000;
    thread_local InspectionProcess process({}, timeout);
    return process.execute(request);
}
int inspectionWorkerMain(int argc, char** argv,
    std::function<QJsonObject(const InspectionRequest&)> executor)
{
    QCoreApplication app(argc, argv);
    if (argc != 3) return 2;
    QLocalSocket socket;
    socket.setReadBufferSize(MaximumPacket + 4);
    socket.connectToServer(QString::fromLocal8Bit(argv[2]));
    if (!socket.waitForConnected(3000)) return 3;
    while (socket.state() == QLocalSocket::ConnectedState) {
        try {
            QElapsedTimer age;
            age.start();
            const auto request = InspectionWireCodec::decode(readPacket(socket, age, -1));
            auto result = executor(request);
            result["inspectionId"] = request.trace.inspectionId;
            writePacket(socket, result, age, -1);
        } catch (const std::exception&) { return 4; }
    }
    return 0;
}
