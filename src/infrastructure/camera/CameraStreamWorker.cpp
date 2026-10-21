#include "infrastructure/camera/CameraStreamWorker.h"

#include "infrastructure/camera/CameraController.h"

#include <opencv2/imgproc.hpp>

#include <QDebug>

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/eventfd.h>
#include <linux/videodev2.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <array>
#include <algorithm>
#include <vector>

namespace {

constexpr unsigned int kBufferCount = 4;

struct MmapPlane {
    void* address { MAP_FAILED };
    size_t length { 0 };
};

struct MmapBuffer {
    std::array<MmapPlane, VIDEO_MAX_PLANES> planes;
    unsigned int planeCount { 0 };
};

QString fourccName(__u32 pixelFormat)
{
    const char chars[] = { static_cast<char>(pixelFormat & 0xff),
        static_cast<char>((pixelFormat >> 8) & 0xff), static_cast<char>((pixelFormat >> 16) & 0xff),
        static_cast<char>((pixelFormat >> 24) & 0xff), '\0' };
    return QString::fromLatin1(chars);
}

bool xioctl(int fd, unsigned long request, void* argument)
{
    int result;
    do {
        result = ioctl(fd, request, argument);
    } while (result == -1 && errno == EINTR);
    return result == 0;
}

bool queueBuffer(int fd, v4l2_buf_type type, bool multiPlanar, unsigned int index, unsigned int planeCount)
{
    v4l2_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.type = type;
    buffer.memory = V4L2_MEMORY_MMAP;
    buffer.index = index;
    std::array<v4l2_plane, VIDEO_MAX_PLANES> planes;
    memset(planes.data(), 0, sizeof(planes));
    if (multiPlanar) {
        buffer.length = planeCount;
        buffer.m.planes = planes.data();
    }
    return xioctl(fd, VIDIOC_QBUF, &buffer);
}

void releaseBuffers(
    int fd, std::array<MmapBuffer, kBufferCount>& buffers, v4l2_buf_type type, bool multiPlanar)
{
    if (fd < 0)
        return;

    for (auto& buffer : buffers) {
        for (unsigned int plane = 0; plane < buffer.planeCount; ++plane) {
            if (buffer.planes[plane].address != MAP_FAILED) {
                munmap(buffer.planes[plane].address, buffer.planes[plane].length);
                buffer.planes[plane].address = MAP_FAILED;
                buffer.planes[plane].length = 0;
            }
        }
        buffer.planeCount = 0;
    }

    if (multiPlanar) {
        v4l2_requestbuffers request;
        memset(&request, 0, sizeof(request));
        request.type = type;
        request.memory = V4L2_MEMORY_MMAP;
        request.count = 0;
        xioctl(fd, VIDIOC_REQBUFS, &request);
    }
}

} // namespace

CameraStreamWorker::CameraStreamWorker(QObject* parent)
    : QThread(parent)
{
}

CameraStreamWorker::~CameraStreamWorker()
{
    stop();
}

void CameraStreamWorker::setDevice(const QString& devicePath)
{
    _devicePath = devicePath;
}

void CameraStreamWorker::setResolution(int width, int height)
{
    _width = width;
    _height = height;
}

void CameraStreamWorker::startCapture()
{
    bool expected = false;
    if (!_running.compare_exchange_strong(expected, true))
        return;
    start();
}

void CameraStreamWorker::stop()
{
    _running = false;
    const int wakeFd = _wakeFd.load();
    if (wakeFd >= 0) {
        const uint64_t signal = 1;
        ssize_t result;
        do {
            result = write(wakeFd, &signal, sizeof(signal));
        } while (result < 0 && errno == EINTR);
    }
    // eventfd 会唤醒 poll()，无需等待摄像头帧或固定超时。
    wait();
}

void CameraStreamWorker::run()
{
    struct RunningReset {
        std::atomic<bool>& running;
        ~RunningReset()
        {
            running = false;
        }
    } reset { _running };

    const int wakeFd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (wakeFd < 0) {
        Q_EMIT streamError(
            QStringLiteral("创建摄像头停止事件失败: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        return;
    }
    _wakeFd = wakeFd;
    struct WakeFdReset {
        std::atomic<int>& sharedFd;
        int fd;
        ~WakeFdReset()
        {
            int expected = fd;
            sharedFd.compare_exchange_strong(expected, -1);
            close(fd);
        }
    } wakeFdReset { _wakeFd, wakeFd };

    if (!_running.load())
        return;

#ifdef HAVE_ROCKIT_MPI
    // VI/VPSS的创建由CameraStreamService在Qt主线程完成，和参考cam_ctl的
    // 初始化线程一致；后台线程只负责阻塞取帧，避免在AIQ主线程之外创建VI。
    auto& camera = CameraController::instance();
    if (!camera.isQtFrameStreamRunning()) {
        Q_EMIT streamError(QStringLiteral("AIQ/VI/VPSS预览链路未启动"));
        return;
    }

    int vpssConsecutiveFailures = 0;
    while (_running.load()) {
        cv::Mat frame;
        if (camera.readQtPreviewFrame(frame, 500)) {
            vpssConsecutiveFailures = 0;
            Q_EMIT frameReady(frame);
            msleep(50);
            continue;
        }

        if (_running.load() && ++vpssConsecutiveFailures >= 30) {
            Q_EMIT streamError(QStringLiteral("VPSS预览连续取帧超时"));
            break;
        }
    }
    return;
#endif

    if (_devicePath.isEmpty()) {
        Q_EMIT streamError(QStringLiteral("未指定摄像头设备"));
        return;
    }

    const int fd = open(_devicePath.toLocal8Bit().constData(), O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) {
        Q_EMIT streamError(QStringLiteral("无法打开摄像头设备: %1 (%2)")
                .arg(_devicePath, QString::fromLocal8Bit(strerror(errno))));
        return;
    }

    v4l2_capability capability;
    memset(&capability, 0, sizeof(capability));
    if (!xioctl(fd, VIDIOC_QUERYCAP, &capability)) {
        Q_EMIT streamError(
            QStringLiteral("读取摄像头能力失败: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        close(fd);
        return;
    }

    const unsigned int deviceCaps
        = capability.device_caps != 0 ? capability.device_caps : capability.capabilities;
    const bool multiPlanar = (deviceCaps & V4L2_CAP_VIDEO_CAPTURE_MPLANE) != 0;
    const bool singlePlanar = (deviceCaps & V4L2_CAP_VIDEO_CAPTURE) != 0;
    if (!multiPlanar && !singlePlanar) {
        Q_EMIT streamError(QStringLiteral("设备不支持视频采集: %1").arg(_devicePath));
        close(fd);
        return;
    }

    const v4l2_buf_type bufferType
        = multiPlanar ? V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE : V4L2_BUF_TYPE_VIDEO_CAPTURE;

    v4l2_format format;
    memset(&format, 0, sizeof(format));
    format.type = bufferType;
    if (multiPlanar) {
        format.fmt.pix_mp.width = _width > 0 ? _width : 1280;
        format.fmt.pix_mp.height = _height > 0 ? _height : 1024;
        format.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_NV12;
        format.fmt.pix_mp.field = V4L2_FIELD_NONE;
    } else {
        format.fmt.pix.width = _width > 0 ? _width : 1280;
        format.fmt.pix.height = _height > 0 ? _height : 1024;
        format.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
        format.fmt.pix.field = V4L2_FIELD_NONE;
    }

    if (!xioctl(fd, VIDIOC_S_FMT, &format)) {
        Q_EMIT streamError(
            QStringLiteral("设置摄像头NV12格式失败: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        close(fd);
        return;
    }

    const unsigned int width = multiPlanar ? format.fmt.pix_mp.width : format.fmt.pix.width;
    const unsigned int height = multiPlanar ? format.fmt.pix_mp.height : format.fmt.pix.height;
    const unsigned int planeCount = multiPlanar ? format.fmt.pix_mp.num_planes : 1;
    if (width == 0 || height == 0 || planeCount == 0 || planeCount > VIDEO_MAX_PLANES) {
        Q_EMIT streamError(QStringLiteral("摄像头返回了无效的NV12格式"));
        close(fd);
        return;
    }

    std::array<MmapBuffer, kBufferCount> buffers;
    v4l2_requestbuffers request;
    memset(&request, 0, sizeof(request));
    request.type = bufferType;
    request.memory = V4L2_MEMORY_MMAP;
    request.count = kBufferCount;
    if (!xioctl(fd, VIDIOC_REQBUFS, &request) || request.count < 2) {
        Q_EMIT streamError(
            QStringLiteral("申请摄像头MMAP缓冲失败: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        close(fd);
        return;
    }

    const unsigned int bufferCount = std::min(request.count, kBufferCount);
    bool setupOk = true;
    for (unsigned int index = 0; index < bufferCount && setupOk; ++index) {
        v4l2_buffer buffer;
        memset(&buffer, 0, sizeof(buffer));
        buffer.type = bufferType;
        buffer.memory = V4L2_MEMORY_MMAP;
        buffer.index = index;

        std::array<v4l2_plane, VIDEO_MAX_PLANES> planes;
        memset(planes.data(), 0, sizeof(planes));
        if (multiPlanar) {
            buffer.length = VIDEO_MAX_PLANES;
            buffer.m.planes = planes.data();
        }

        if (!xioctl(fd, VIDIOC_QUERYBUF, &buffer)) {
            setupOk = false;
            break;
        }

        const unsigned int count = multiPlanar ? buffer.length : 1;
        buffers[index].planeCount = count;
        for (unsigned int plane = 0; plane < count; ++plane) {
            const size_t length = multiPlanar ? planes[plane].length : buffer.length;
            const off_t offset = multiPlanar ? static_cast<off_t>(planes[plane].m.mem_offset)
                                             : static_cast<off_t>(buffer.m.offset);
            void* address = mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
            if (address == MAP_FAILED) {
                setupOk = false;
                break;
            }
            buffers[index].planes[plane].address = address;
            buffers[index].planes[plane].length = length;
        }
    }

    if (!setupOk) {
        Q_EMIT streamError(
            QStringLiteral("映射摄像头MMAP缓冲失败: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        releaseBuffers(fd, buffers, bufferType, multiPlanar);
        close(fd);
        return;
    }

    for (unsigned int index = 0; index < bufferCount; ++index) {
        if (!queueBuffer(fd, bufferType, multiPlanar, index, buffers[index].planeCount)) {
            setupOk = false;
            break;
        }
    }

    v4l2_buf_type streamType = bufferType;
    if (setupOk && !xioctl(fd, VIDIOC_STREAMON, &streamType))
        setupOk = false;

    if (!setupOk) {
        Q_EMIT streamError(
            QStringLiteral("启动摄像头视频流失败: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        xioctl(fd, VIDIOC_STREAMOFF, &streamType);
        releaseBuffers(fd, buffers, bufferType, multiPlanar);
        close(fd);
        return;
    }

    int consecutiveFailures = 0;
    const unsigned int yStride = multiPlanar
        ? (format.fmt.pix_mp.plane_fmt[0].bytesperline != 0 ? format.fmt.pix_mp.plane_fmt[0].bytesperline
                                                            : width)
        : (format.fmt.pix.bytesperline != 0 ? format.fmt.pix.bytesperline : width);
    const unsigned int uvStride = multiPlanar && planeCount > 1
        ? (format.fmt.pix_mp.plane_fmt[1].bytesperline != 0 ? format.fmt.pix_mp.plane_fmt[1].bytesperline
                                                            : width)
        : yStride;

    while (_running.load()) {
        pollfd pollDescriptors[2] { };
        pollDescriptors[0].fd = fd;
        pollDescriptors[0].events = POLLIN;
        pollDescriptors[1].fd = wakeFd;
        pollDescriptors[1].events = POLLIN;
        const int pollResult = poll(pollDescriptors, 2, 500);
        if (!_running.load())
            break;
        if (pollResult < 0 && errno == EINTR)
            continue;
        if (pollResult <= 0 || !(pollDescriptors[0].revents & POLLIN)) {
            if (++consecutiveFailures >= 30) {
                Q_EMIT streamError(QStringLiteral("摄像头等待帧超时: %1").arg(_devicePath));
                break;
            }
            continue;
        }

        v4l2_buffer buffer;
        memset(&buffer, 0, sizeof(buffer));
        buffer.type = bufferType;
        buffer.memory = V4L2_MEMORY_MMAP;
        std::array<v4l2_plane, VIDEO_MAX_PLANES> planes;
        memset(planes.data(), 0, sizeof(planes));
        if (multiPlanar) {
            buffer.length = planeCount;
            buffer.m.planes = planes.data();
        }

        if (!xioctl(fd, VIDIOC_DQBUF, &buffer)) {
            if (errno == EAGAIN)
                continue;
            if (++consecutiveFailures >= 30) {
                Q_EMIT streamError(
                    QStringLiteral("读取摄像头帧失败: %1").arg(QString::fromLocal8Bit(strerror(errno))));
                break;
            }
            continue;
        }
        consecutiveFailures = 0;

        const unsigned int index = buffer.index;
        if (index >= bufferCount) {
            Q_EMIT streamError(QStringLiteral("摄像头返回了无效缓冲索引"));
            break;
        }

        cv::Mat yuv(static_cast<int>(height * 3 / 2), static_cast<int>(width), CV_8UC1);
        if (multiPlanar && planeCount > 1) {
            const size_t yBytes = std::min<size_t>(width, yStride);
            const size_t uvBytes = std::min<size_t>(width, uvStride);
            const auto& yPlane = buffers[index].planes[0];
            const auto& uvPlane = buffers[index].planes[1];
            for (unsigned int row = 0; row < height; ++row) {
                memcpy(yuv.ptr(static_cast<int>(row)),
                    static_cast<const unsigned char*>(yPlane.address) + row * yStride, yBytes);
            }
            for (unsigned int row = 0; row < height / 2; ++row) {
                memcpy(yuv.ptr(static_cast<int>(height + row)),
                    static_cast<const unsigned char*>(uvPlane.address) + row * uvStride, uvBytes);
            }
        } else {
            const auto& plane = buffers[index].planes[0];
            const size_t bytesPerRow = std::min<size_t>(width, yStride);
            for (unsigned int row = 0; row < height + height / 2; ++row) {
                memcpy(yuv.ptr(static_cast<int>(row)),
                    static_cast<const unsigned char*>(plane.address) + row * yStride, bytesPerRow);
            }
        }

        cv::Mat bgrFrame;
        cv::cvtColor(yuv, bgrFrame, cv::COLOR_YUV2BGR_NV12);
        Q_EMIT frameReady(bgrFrame.clone());

        if (!queueBuffer(fd, bufferType, multiPlanar, index, planeCount)) {
            Q_EMIT streamError(
                QStringLiteral("重新入队摄像头缓冲失败: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            break;
        }

        // 预览画面每帧都要在UI线程转换QPixmap;限制投递频率,避免高帧率相机
        // 把Qt事件队列压满导致触摸和导航看起来失去响应。设备仍持续采集,
        // 这里只降低显示刷新频率,不改变底层采集格式和ISP逻辑。
        msleep(50);
    }

    xioctl(fd, VIDIOC_STREAMOFF, &streamType);
    releaseBuffers(fd, buffers, bufferType, multiPlanar);
    close(fd);
}
