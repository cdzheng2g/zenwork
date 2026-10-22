#include "CameraRecovery.h"
#include <algorithm>
CameraRecovery::CameraRecovery(Start start, Stop stop, QObject* parent, int delay, int retries)
    : QObject(parent), _start(std::move(start)), _stop(std::move(stop)),
      _delay(qMax(1, delay)), _maximumRetries(qMax(0, retries))
{
    _retry.setSingleShot(true);
    connect(&_retry, &QTimer::timeout, this, &CameraRecovery::attempt);
}
void CameraRecovery::setState(State state)
{
    if (_state == state) return;
    _state = state;
    emit stateChanged(state == State::Running);
}
void CameraRecovery::setDemand(bool needed)
{
    if (_needed == needed) return;
    _needed = needed;
    if (needed) { _attempts = 0; attempt(); }
    else {
        _retry.stop();
        _stop();
        _attempts = 0;
        setState(State::Stopped);
    }
}
void CameraRecovery::attempt()
{
    if (!_needed) return;
    ++_attempts;
    setState(State::Starting);
    if (!_start()) schedule(QStringLiteral("相机采集启动失败"));
}
void CameraRecovery::schedule(const QString& reason)
{
    emit errorOccurred(reason);
    if (!_needed) { setState(State::Stopped); return; }
    if (_attempts > _maximumRetries) { setState(State::Failed); return; }
    setState(State::Recovering);
    _retry.start(_delay * (1 << qMin(3, qMax(0, _attempts - 1))));
}
void CameraRecovery::workerStopped(const QString& reason)
{
    if (_attempts == 0) _attempts = 1;
    if (_needed) schedule(reason.isEmpty() ? QStringLiteral("相机采集意外结束") : reason);
}
void CameraRecovery::frameReceived()
{
    if (!_needed) return;
    _attempts = 0;
    _retry.stop();
    setState(State::Running);
}
void CameraRecovery::retry()
{
    if (!_needed || _state != State::Failed) return;
    _attempts = 0;
    attempt();
}
