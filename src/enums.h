#ifndef ENUMS_H
#define ENUMS_H

#include "qobject.h"

enum ColorVariant {
    NormalColor,
    DarkerColor,
    LighterColor
};

enum LogType {
    DataLog = -1,
    DebugLog = 0,
    InformationLog = 1,
    WarningLog = 2,
    CriticalLog = 3,
    FatalLog = 4
};
Q_DECLARE_METATYPE(LogType);

#endif // ENUMS_H
