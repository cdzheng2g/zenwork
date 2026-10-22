#include "ArchiveQueue.h"
#include <QJsonDocument>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <deque>
#include <exception>

struct ArchiveState {
    struct Item { QByteArray line; QString id, sequence; bool notify; };
    QMutex mutex;
    QWaitCondition wake;
    std::deque<Item> queue;
    ArchiveQueue::Writer writer;
    bool accepting {true}, busy {false};
    int maximumItems;
    qint64 maximumBytes, bytes {0};
};
ArchiveThread::ArchiveThread(std::shared_ptr<ArchiveState> state) : _state(std::move(state)) {}
void ArchiveThread::run()
{
    for (;;) {
        ArchiveState::Item item;
        {
            QMutexLocker lock(&_state->mutex);
            while (_state->queue.empty() && _state->accepting) _state->wake.wait(&_state->mutex);
            if (_state->queue.empty()) return;
            item = std::move(_state->queue.front());
            _state->queue.pop_front();
            _state->busy = true;
        }
        QString error;
        try { error = _state->writer(item.line); }
        catch (const std::exception& e) { error = QString::fromUtf8(e.what()); }
        catch (...) { error = QStringLiteral("结果保存异常"); }
        {
            QMutexLocker lock(&_state->mutex);
            _state->bytes -= item.line.size();
            _state->busy = false;
        }
        if (item.notify) emit saved(item.id, item.sequence, error, monotonicNowNs());
    }
}
ArchiveQueue::ArchiveQueue(Writer writer, QObject* parent, int items, qint64 bytes)
    : QObject(parent), _state(std::make_shared<ArchiveState>())
{
    qRegisterMetaType<DeliveryEvent>();
    _state->writer = std::move(writer);
    _state->maximumItems = qMax(1, items);
    _state->maximumBytes = qMax<qint64>(1, bytes);
    _thread = new ArchiveThread(_state);
    connect(_thread, &ArchiveThread::saved, this,
        [this](const QString& id, const QString& sequence, const QString& error, qint64 at) {
            emit completed({id, sequence, "archive", error.isEmpty() ? DeliveryState::Confirmed
                : DeliveryState::Failed, error, at});
        });
    _thread->start();
}
ArchiveQueue::~ArchiveQueue() { shutdown(); }
bool ArchiveQueue::enqueue(const QJsonObject& value, QString* error, bool notify)
{
    if (error) error->clear();
    const auto line = QJsonDocument(value).toJson(QJsonDocument::Compact) + '\n';
    QMutexLocker lock(&_state->mutex);
    if (!_state->accepting || int(_state->queue.size()) + int(_state->busy) >= _state->maximumItems
        || line.size() > _state->maximumBytes - _state->bytes) {
        if (error) *error = QStringLiteral("存档队列已停止或达到容量上限，记录未提交");
        return false;
    }
    _state->bytes += line.size();
    _state->queue.push_back({line, value["inspectionId"].toString(), value["sequenceText"].toString(), notify});
    _state->wake.wakeOne();
    return true;
}
int ArchiveQueue::pending() const
{
    QMutexLocker lock(&_state->mutex);
    return int(_state->queue.size()) + int(_state->busy);
}
bool ArchiveQueue::shutdown(int maximumWaitMs)
{
    if (!_thread) return pending() == 0;
    {
        QMutexLocker lock(&_state->mutex);
        _state->accepting = false;
        _state->wake.wakeOne();
    }
    const bool drained = _thread->wait(qMax(0, maximumWaitMs));
    if (drained) delete _thread;
    else {
        // State and writer belong to the thread; the GUI can exit without deleting a running QThread.
        connect(_thread, &QThread::finished, _thread, &QObject::deleteLater);
        qWarning("Archive queue did not drain before shutdown; pending records may be lost on exit");
    }
    _thread = nullptr;
    return drained;
}
