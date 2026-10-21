#ifndef CAMERASTREAMLEASE_H
#define CAMERASTREAMLEASE_H

// 管理共享采集流的消费者数量。返回值仅表示调用方是否需要执行一次
// 状态转换：acquire() 的 true 表示应启动采集，release() 的 true 表示应停止。
class CameraStreamLease {
public:
    bool acquire();
    bool release();
    int consumerCount() const;

private:
    int _consumerCount { 0 };
};

#endif // CAMERASTREAMLEASE_H
