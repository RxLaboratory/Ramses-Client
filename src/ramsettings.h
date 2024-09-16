#ifndef RAMSETTINGS_H
#define RAMSETTINGS_H

#include "duapp/dusettings.h"

namespace RamSettings
{
    enum SettingKey {
        DaemonPort = DuSettings::Other + 1,
        RecentDatabases = DuSettings::Other + 2,
        Login = DuSettings::Other + 3,
    };

    void registerRamsesSettings();
};

#endif // RAMSETTINGS_H
