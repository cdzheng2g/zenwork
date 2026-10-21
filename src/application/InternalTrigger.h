#pragma once

#include <QObject>

class QTimer;

class InternalTrigger final : public QObject {
    Q_OBJECT
public:
    explicit InternalTrigger(QObject* parent = nullptr);
    void start(int intervalMs);
    void stop();
    bool isRunning() const;
    bool tryBeginExecution();
    void finishExecution();
    quint64 skippedTickCount() const
    {
        return _skippedTicks;
    }

Q_SIGNALS:
    void triggered();
    void skipped(quint64 total);

private:
    QTimer* _timer;
    bool _busy { false };
    quint64 _skippedTicks { 0 };
};
