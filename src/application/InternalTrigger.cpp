#include "InternalTrigger.h"
#include <QTimer>
InternalTrigger::InternalTrigger(QObject* parent)
    : QObject(parent)
    , _timer(new QTimer(this))
{
    connect(_timer, &QTimer::timeout, this, [this] {
        if (tryBeginExecution())
            emit triggered();
        else
            emit skipped(_skippedTicks);
    });
}
void InternalTrigger::start(int intervalMs)
{
    _skippedTicks = 0;
    _timer->start(qMax(1, intervalMs));
}
void InternalTrigger::stop()
{
    _timer->stop();
}
bool InternalTrigger::isRunning() const
{
    return _timer->isActive();
}
bool InternalTrigger::tryBeginExecution()
{
    if (_busy) {
        ++_skippedTicks;
        return false;
    }
    _busy = true;
    return true;
}
void InternalTrigger::finishExecution()
{
    _busy = false;
}
