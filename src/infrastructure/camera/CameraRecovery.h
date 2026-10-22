#pragma once
#include <QObject>
#include <QTimer>
#include <functional>

class CameraRecovery final : public QObject {
    Q_OBJECT
public:
    enum class State { Stopped, Starting, Running, Recovering, Failed };
    using Start = std::function<bool()>;
    using Stop = std::function<void()>;
    CameraRecovery(Start start, Stop stop, QObject* parent = nullptr,
        int retryDelayMs = 1000, int maximumRetries = 3);
    void setDemand(bool needed);
    void workerStopped(const QString& reason);
    void frameReceived();
    void retry();
    State state() const { return _state; }
signals:
    void stateChanged(bool ready);
    void errorOccurred(const QString& reason);
private:
    void attempt();
    void schedule(const QString& reason);
    void setState(State state);
    Start _start;
    Stop _stop;
    QTimer _retry;
    State _state { State::Stopped };
    bool _needed {false};
    int _attempts {0}, _delay, _maximumRetries;
};
