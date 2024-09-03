#include "dusettings.h"

#include <QtDebug>

DuSettings *DuSettings::_instance = nullptr;
DuSettings *DuSettings::i()
{
    if (!DuSettings::_instance) DuSettings::_instance = new DuSettings();
    return DuSettings::_instance;
}

QVariant DuSettings::get(int key, const QVariant &defaultOverride, const QString &subKey) const
{
    auto setting = _settings.value(key);

    QVariant dval;
    if (!defaultOverride.isValid())
        dval = setting.defaultValue;
    else
        dval = defaultOverride;

    return _s.value(setting.qKey + subKey, dval);
}

QVector<QHash<QString,QVariant>> DuSettings::getArray(int key, const QVariant &defaultOverride, const QString &subKey)
{
    auto setting = _settings.value(key);

    QVariant dval;
    if (!defaultOverride.isValid())
        dval = setting.defaultValue;
    else
        dval = defaultOverride;

    QVector<QHash<QString,QVariant>> values;

    int size = _s.beginReadArray(setting.qKey + subKey);
    qDebug() << "================";
    qDebug() << setting.qKey + subKey;
    qDebug() << size;
    for (int i = 0; i < size; i++)
    {
        _s.setArrayIndex(i);
        const QStringList keys = _s.allKeys();
        qDebug() << keys;
        QHash<QString, QVariant> vals;
        for (const auto &k: keys) {
            vals.insert(
                k,
                _s.value(k, dval)
                );
        }
        values << vals;
    }
    _s.endArray();

    return values;
}

void DuSettings::set(int key, const QVariant &value, const QString &subKey, bool doNotEmit)
{
    if (!_settings.contains(key))
        return;

    // Get the current value to check if it's actually changed
    auto current = get(key,QVariant(),subKey);
    if (current == value)
        return;

    // Save
    auto setting = _settings.value(key);
    _s.setValue(setting.qKey + subKey, value);

    // Emit
    if (!doNotEmit)
        emit settingChanged(key, value);
}

void DuSettings::setArray(int key, const QVector<QHash<QString, QVariant>> &values, const QString &subKey, bool doNotEmit)
{
    if (!_settings.contains(key))
        return;

    auto setting = _settings.value(key);

    _s.beginWriteArray(setting.qKey + subKey);
    int i = 0;
    for (const auto &value: values) {
        _s.setArrayIndex(i);
        QHashIterator<QString,QVariant> it(value);
        while(it.hasNext()) {
            it.next();
            _s.setValue(it.key(), it.value());
        }
        i++;
    }
    _s.endArray();

    // Emit
    if (!doNotEmit)
        emit settingChanged(key, QVariant());
}

void DuSettings::remove(int key, const QString &subKey, bool doNotEmit)
{
    if (!_settings.contains(key))
        return;

    auto setting = _settings.value(key);
    _s.remove(setting.qKey + subKey);

    if (!doNotEmit)
        emit settingChanged(key, setting.defaultValue);
}

QVariant DuSettings::getDefault(int key) const
{
    if (!_settings.contains(key))
        return QVariant();

    return _settings.value(key).defaultValue;
}

void DuSettings::setDefault(int key, const QVariant &value, bool doNotEmit)
{
    Q_ASSERT(_settings.contains(key));

    auto s = _settings.take(key);
    s.defaultValue = value;
    _settings.insert(key, s);

    if (!doNotEmit)
        emit defaultChanged(key, value);
}

void DuSettings::registerSettings(const QHash<int, DuSetting> &settings)
{
    _settings.insert(settings);
}

DuSettings::DuSettings(QObject *parent)
    : QObject{parent}
{
#if REINIT_SETTINGS
    _s.clear();
    qDebug() << "Settings cleared";
#endif
}
