#include "application/communication/SchemeCommunicationRuntime.h"
#include "contracts/CommunicationModule.h"
#include <QCoreApplication>
SchemeCommunicationRuntime::SchemeCommunicationRuntime(QObject* parent)
    : QObject(parent)
{
    for (const auto& m : communicationModules())
        if (m.observe)
            m.observe(
                this, [this] { emit statusChanged(); },
                [this, id = m.id](const QString& message) {
                    _errors[id] = message;
                    emit statusChanged();
                });
}
SchemeCommunicationRuntime& SchemeCommunicationRuntime::instance()
{
    static auto* runtime = new SchemeCommunicationRuntime(QCoreApplication::instance());
    return *runtime;
}
bool SchemeCommunicationRuntime::apply(const SchemeRuntimeSettings& settings, QString* error)
{
    if (error)
        error->clear();
    const auto invalid = validateCommunicationSettings(
        settings.communication, settings.outputChannels, settings.modbusTriggerEnabled, true);
    if (!invalid.isEmpty()) {
        if (error)
            *error = invalid;
        return false;
    }
    const auto next = normalizedCommunicationSettings(settings.communication);
    const auto channels = activeCommunicationChannels(settings.outputChannels, settings.modbusTriggerEnabled);
    // Stop changed endpoints first so swapping listener ports is valid.
    for (const auto& m : communicationModules())
        if (!channels.contains(m.id) || configuration(m.id) != next[m.id].toObject())
            m.stop();
    _configuration = next;
    _channels = channels;
    _errors.clear();
    QStringList failures;
    for (const auto& m : communicationModules())
        if (channels.contains(m.id)) {
            QString failure;
            if (!m.start(next[m.id].toObject(), &failure)) {
                _errors[m.id] = failure;
                failures.append(m.name + "：" + failure);
            }
        }
    if (error)
        *error = failures.join(QStringLiteral("；"));
    emit statusChanged();
    return failures.isEmpty();
}
void SchemeCommunicationRuntime::stopAll()
{
    SchemeRuntimeSettings s;
    s.outputChannels.clear();
    apply(s);
}
bool SchemeCommunicationRuntime::usesChannel(const QString& id) const
{
    return _channels.contains(id);
}
QJsonObject SchemeCommunicationRuntime::configuration(const QString& id) const
{
    return _configuration[id].toObject();
}
QString SchemeCommunicationRuntime::status(const QString& id) const
{
    if (!usesChannel(id))
        return "未使用";
    if (!_errors.value(id).isEmpty())
        return "通信异常：" + _errors.value(id);
    const auto* m = communicationModule(id);
    return m ? m->status() : QStringLiteral("未知通信方式");
}
QString SchemeCommunicationRuntime::summary() const
{
    if (_channels.isEmpty())
        return "通信：未使用";
    QStringList states;
    for (const auto& id : _channels)
        states << status(id);
    return "通信：" + states.join(" / ");
}
