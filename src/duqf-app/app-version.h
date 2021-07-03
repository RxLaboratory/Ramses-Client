#ifndef APPVERSION_H
#define APPVERSION_H

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_BUILD 1
#define VERSION_SUFFIX "dev"

#define STRINGIFY_VERSION(A, B, C) CONCAT(A, B, C )
#define CONCAT(A, B, C ) STRINGIFY( A##.##B##.##C )
#define STRINGIFY(A) #A

#ifdef QT_DEBUG
#define STR_VERSION STRINGIFY_VERSION(VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD) "-" VERSION_SUFFIX "_development build"
#else
#define STR_VERSION STRINGIFY_VERSION(VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD) "-" VERSION_SUFFIX
#endif

#define VER_FILEVERSION VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,0
#define STR_FILEVERSION STR_VERSION

// Keep the product version as fixed
#define VER_PRODUCTVERSION VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,0
#define STR_PRODUCTVERSION STR_VERSION

#define STR_COMPANYNAME "Rx Laboratory"
#define STR_FILEDESCRIPTION "Ramses"
#define STR_INTERNALNAME "Ramses"
#define STR_LEGALCOPYRIGHT "Copyright (c) 2021 Rx Laboratory, Nicolas Dufresne and contributors"
#define STR_LEGALTRADEMARKS1 "All Rights Reserved"
#define STR_ORIGINALFILENAME "Ramses.exe"
#define STR_PRODUCTNAME "Ramses - Rx Asset Management System"
#define STR_PRODUCTDESCRIPTION "Client application for the Rx Asset and production management system for motion pictures production."

#define STR_COMPANYDOMAIN "rainboxlab.org"

#define URL_CHAT "http://chat.rxlab.info"
#define URL_FORUM ""
#define URL_DOC "http://ramses.rxlab.guide"
#define URL_BUGREPORT "https://github.com/Rainbox-dev/Ramses-Client/issues"
#define URL_DONATION "https://donate.rxlab.info"

#define APP_ICON ":/icons/app"
#define SPLASH_IMAGE ":/images/splash"

#define USE_SYSTRAY true
#define SYSTRAY_ICON_LIGHT ":/icons/tray-light"
#define SYSTRAY_ICON_COLOR ":/icons/tray-color"
#define SYSTRAY_ICON_DARK ":/icons/tray-dark"

#endif // APPVERSION_H
