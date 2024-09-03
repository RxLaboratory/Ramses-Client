#ifndef RAMSETTINGS_H
#define RAMSETTINGS_H

#include "duapp/dusettings.h"

namespace RamSettings
{
    enum SettingKey {
        DaemonPort = DuSettings::Other + 1,
        RecentProjects = DuSettings::Other +2,
    };

    void registerRamsesSettings();
};

#endif // RAMSETTINGS_H
