#include "dusettings.h"

DuSettings *DuSettings::_instance = nullptr;
DuSettings *DuSettings::i()
{
    if (!DuSettings::_instance) DuSettings::_instance = new DuSettings();
    return DuSettings::_instance;
}

QVariant DuSettings::get(int key, const QVariant &defaultOverride, const QString &subKey) const
{
    if (!_settings.contains(key))
        return QVariant();

    auto setting = _settings.value(key);
    QVariant dval;
    if (defaultOverride.isNull())
        dval = setting.defaultValue;
    else
        dval = defaultOverride;

    return _s.value(setting.qKey + subKey, dval);
}

void DuSettings::set(int key, const QVariant &value, const QString &subKey, bool doNotEmit)
{
    if (!_settings.contains(key))
        return;

    // Get the current value to check if it's actually changed
    auto current = get(key);
    if (current == value)
        return;

    // Save
    auto setting = _settings.value(key);
    _s.setValue(setting.qKey + subKey, value);

    // Emit
    if (!doNotEmit)
        emit settingChanged(key, value);
}

void DuSettings::registerSettings(const QHash<int, DuSetting> &settings)
{
    _settings.insert(settings);
}

DuSettings::DuSettings(QObject *parent)
    : QObject{parent}
{
#if REINIT_SETTINGS
    m_settings.clear();
    qDebug() << "Settings cleared";
#endif
}
