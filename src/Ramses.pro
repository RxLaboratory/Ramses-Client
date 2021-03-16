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
    asseteditwidget.cpp \
    assetgroupeditwidget.cpp \
    assetgroupsmanagerwidget.cpp \
    assetsmanagerwidget.cpp \
    daemon.cpp \
    daemonsettingswidget.cpp \
    dbinterface.cpp \
    dbisuspender.cpp \
    duqf-app/app-style.cpp \
    duqf-app/app-utils.cpp \
    duqf-nodeview/duqfconnection.cpp \
    duqf-nodeview/duqfconnectionmanager.cpp \
    duqf-nodeview/duqfconnector.cpp \
    duqf-nodeview/duqfgrid.cpp \
    duqf-nodeview/duqfnode.cpp \
    duqf-nodeview/duqfnodescene.cpp \
    duqf-nodeview/duqfnodeview.cpp \
    duqf-nodeview/duqfslot.cpp \
    duqf-utils/duqflogger.cpp \
    duqf-utils/language-utils.cpp \
    duqf-utils/utils.cpp \
    duqf-widgets/aboutdialog.cpp \
    duqf-widgets/appearancesettingswidget.cpp \
    duqf-widgets/duqfdoubleslider.cpp \
    duqf-widgets/duqfdoublespinbox.cpp \
    duqf-widgets/duqffolderdisplaywidget.cpp \
    duqf-widgets/duqffolderselectorwidget.cpp \
    duqf-widgets/duqflistwidget.cpp \
    duqf-widgets/duqfloggingtextedit.cpp \
    duqf-widgets/duqflogtoolbutton.cpp \
    duqf-widgets/duqfslider.cpp \
    duqf-widgets/duqfspinbox.cpp \
    duqf-widgets/settingswidget.cpp \
    duqf-widgets/titlebar.cpp \
    duqf-widgets/toolbarspacer.cpp \
    filetypeeditwidget.cpp \
    filetypesmanagerwidget.cpp \
    listmanagerwidget.cpp \
    localsettingswidget.cpp \
    loginpage.cpp \
    main.cpp \
    mainwindow.cpp \
    pipeline-editor/pipelinewidget.cpp \
    pipeline-editor/ramobjectnode.cpp \
    projecteditwidget.cpp \
    projectselectorwidget.cpp \
    projectsmanagerwidget.cpp \
    ramapplication.cpp \
    ramasset.cpp \
    ramassetgroup.cpp \
    ramassetwidget.cpp \
    ramfiletype.cpp \
    ramobject.cpp \
    ramproject.cpp \
    ramsequence.cpp \
    ramses.cpp \
    ramstate.cpp \
    ramstep.cpp \
    ramuser.cpp \
    ramuuid.cpp \
    sequenceeditwidget.cpp \
    sequencesmanagerwidget.cpp \
    serversettingswidget.cpp \
    stateeditwidget.cpp \
    statesmanagerwidget.cpp \
    stepsmanagerwidget.cpp \
    stepeditwidget.cpp \
    templateassetgroupeditwidget.cpp \
    templateassetgroupsmanagerwidget.cpp \
    templatestepeditwidget.cpp \
    templatestepsmanagerwidget.cpp \
    usereditwidget.cpp \
    userprofilepage.cpp \
    usersmanagerwidget.cpp

HEADERS += \
    asseteditwidget.h \
    assetgroupeditwidget.h \
    assetgroupsmanagerwidget.h \
    assetsmanagerwidget.h \
    daemon.h \
    daemonsettingswidget.h \
    dbinterface.h \
    dbisuspender.h \
    duqf-app/app-style.h \
    duqf-app/app-utils.h \
    duqf-app/app-version.h \
    duqf-nodeview/duqfconnection.h \
    duqf-nodeview/duqfconnectionmanager.h \
    duqf-nodeview/duqfconnector.h \
    duqf-nodeview/duqfgrid.h \
    duqf-nodeview/duqfnode.h \
    duqf-nodeview/duqfnodescene.h \
    duqf-nodeview/duqfnodeview.h \
    duqf-nodeview/duqfslot.h \
    duqf-utils/duqflogger.h \
    duqf-utils/language-utils.h \
    duqf-utils/utils.h \
    duqf-widgets/aboutdialog.h \
    duqf-widgets/appearancesettingswidget.h \
    duqf-widgets/duqfdoubleslider.h \
    duqf-widgets/duqfdoublespinbox.h \
    duqf-widgets/duqffolderdisplaywidget.h \
    duqf-widgets/duqffolderselectorwidget.h \
    duqf-widgets/duqflistwidget.h \
    duqf-widgets/duqfloggingtextedit.h \
    duqf-widgets/duqflogtoolbutton.h \
    duqf-widgets/duqfslider.h \
    duqf-widgets/duqfspinbox.h \
    duqf-widgets/settingswidget.h \
    duqf-widgets/titlebar.h \
    duqf-widgets/toolbarspacer.h \
    filetypeeditwidget.h \
    filetypesmanagerwidget.h \
    listmanagerwidget.h \
    localsettingswidget.h \
    loginpage.h \
    mainwindow.h \
    pipeline-editor/pipelinewidget.h \
    pipeline-editor/ramobjectnode.h \
    projecteditwidget.h \
    projectselectorwidget.h \
    projectsmanagerwidget.h \
    ramapplication.h \
    ramasset.h \
    ramassetgroup.h \
    ramassetwidget.h \
    ramfiletype.h \
    ramobject.h \
    ramproject.h \
    ramsequence.h \
    ramses.h \
    ramstate.h \
    ramstep.h \
    ramuser.h \
    ramuuid.h \
    sequenceeditwidget.h \
    sequencesmanagerwidget.h \
    serversettingswidget.h \
    stateeditwidget.h \
    statesmanagerwidget.h \
    stepsmanagerwidget.h \
    stepeditwidget.h \
    templateassetgroupeditwidget.h \
    templateassetgroupsmanagerwidget.h \
    templatestepeditwidget.h \
    templatestepsmanagerwidget.h \
    usereditwidget.h \
    userprofilepage.h \
    usersmanagerwidget.h

FORMS += \
    asseteditwidget.ui \
    assetgroupeditwidget.ui \
    daemonsettingswidget.ui \
    duqf-widgets/aboutdialog.ui \
    duqf-widgets/appearancesettingswidget.ui \
    duqf-widgets/duqffolderselectorwidget.ui \
    duqf-widgets/toolbarspacer.ui \
    filetypeeditwidget.ui \
    listmanagerwidget.ui \
    localsettingswidget.ui \
    loginpage.ui \
    mainwindow.ui \
    pipeline-editor/pipelinewidget.ui \
    projecteditwidget.ui \
    ramassetwidget.ui \
    sequenceeditwidget.ui \
    serversettingswidget.ui \
    stateeditwidget.ui \
    stepeditwidget.ui \
    templateassetgroupeditwidget.ui \
    templatestepeditwidget.ui \
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
