#include "ramsettings.h"

void RamSettings::registerRamsesSettings() {
    DuSettings::i()->registerSettings({
        { DaemonPort, { "ramses/daemonPort", 18185 }},
        { RecentDatabases, { "database/recent", "" }},
    });
}
