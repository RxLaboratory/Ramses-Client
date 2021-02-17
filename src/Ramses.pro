QT       += core gui \
            sql \
            network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Ramses
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dbinterface.cpp \
    dbisuspender.cpp \
    duqf-app/app-style.cpp \
    duqf-app/app-utils.cpp \
    duqf-utils/language-utils.cpp \
    duqf-utils/utils.cpp \
    duqf-widgets/aboutdialog.cpp \
    duqf-widgets/appearancesettingswidget.cpp \
    duqf-widgets/duqfdoubleslider.cpp \
    duqf-widgets/duqfdoublespinbox.cpp \
    duqf-widgets/duqfslider.cpp \
    duqf-widgets/duqfspinbox.cpp \
    duqf-widgets/settingswidget.cpp \
    duqf-widgets/toolbarspacer.cpp \
    listmanagerwidget.cpp \
    loginpage.cpp \
    main.cpp \
    mainwindow.cpp \
    projecteditwidget.cpp \
    projectsmanagerwidget.cpp \
    ramobject.cpp \
    ramproject.cpp \
    ramses.cpp \
    ramstate.cpp \
    ramstep.cpp \
    ramuser.cpp \
    ramuuid.cpp \
    serversettingswidget.cpp \
    stateeditwidget.cpp \
    statesmanagerwidget.cpp \
    stepeditwidget.cpp \
    templatestepsmanagerwidget.cpp \
    usereditwidget.cpp \
    userprofilepage.cpp \
    usersmanagerwidget.cpp

HEADERS += \
    dbinterface.h \
    dbisuspender.h \
    duqf-app/app-style.h \
    duqf-app/app-utils.h \
    duqf-app/app-version.h \
    duqf-utils/language-utils.h \
    duqf-utils/utils.h \
    duqf-widgets/aboutdialog.h \
    duqf-widgets/appearancesettingswidget.h \
    duqf-widgets/duqfdoubleslider.h \
    duqf-widgets/duqfdoublespinbox.h \
    duqf-widgets/duqfslider.h \
    duqf-widgets/duqfspinbox.h \
    duqf-widgets/settingswidget.h \
    duqf-widgets/toolbarspacer.h \
    listmanagerwidget.h \
    loginpage.h \
    mainwindow.h \
    projecteditwidget.h \
    projectsmanagerwidget.h \
    ramobject.h \
    ramproject.h \
    ramses.h \
    ramstate.h \
    ramstep.h \
    ramuser.h \
    ramuuid.h \
    serversettingswidget.h \
    stateeditwidget.h \
    statesmanagerwidget.h \
    stepeditwidget.h \
    templatestepsmanagerwidget.h \
    usereditwidget.h \
    userprofilepage.h \
    usersmanagerwidget.h

FORMS += \
    duqf-widgets/aboutdialog.ui \
    duqf-widgets/appearancesettingswidget.ui \
    duqf-widgets/toolbarspacer.ui \
    listmanagerwidget.ui \
    loginpage.ui \
    mainwindow.ui \
    projecteditwidget.ui \
    serversettingswidget.ui \
    stateeditwidget.ui \
    stepeditwidget.ui \
    usereditwidget.ui \
    userprofilepage.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# OS Specific configurations
win* {
    # Add version and other metadata
    DISTFILES += app.rc
    RC_FILE = app.rc
    !build_pass:touch($$RC_FILE, DuF/version.h)
    # Enable console output
    CONFIG += console
    msvc: LIBS += -luser32
} else:unix {
    # Fix issue with c++ version used to compile Qt in some distros (Ubuntu) with Qt <= 5.12.
    # Need to check the version of c++ used with distros providing Qt > 5.12
    equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 13)
    {
        QMAKE_CXXFLAGS += "-fno-sized-deallocation"
        # Ignore annoying errors on older versions of Qt
        QMAKE_CXXFLAGS += -Wdeprecated-copy
    }
} else:macx {
    # Just in case...
}

RESOURCES += \
    duqf_resources.qrc \
    resources.qrc
