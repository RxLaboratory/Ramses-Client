#ifndef ENUMS_H
#define ENUMS_H

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

#endif // ENUMS_H
