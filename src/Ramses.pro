QT       += core gui \
            sql \
            network \
            svg

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

INCLUDEPATH += rameditwidgets \
    settingswidgets \
    ramdisplaywidgets \
    smallwidgets \
    ramobjects

SOURCES += \
    duqf-widgets/duqfsearchedit.cpp \
    ramdisplaywidgets/ramapplicationwidget.cpp \
    ramdisplaywidgets/ramassetgroupwidget.cpp \
    ramdisplaywidgets/ramfiletypewidget.cpp \
    ramdisplaywidgets/ramprojectwidget.cpp \
    ramdisplaywidgets/ramsequencewidget.cpp \
    ramdisplaywidgets/ramstatewidget.cpp \
    ramdisplaywidgets/ramstepwidget.cpp \
    ramdisplaywidgets/ramuserwidget.cpp \
    rameditwidgets/applicationeditwidget.cpp \
    rameditwidgets/applicationlistmanagerwidget.cpp \
    rameditwidgets/asseteditwidget.cpp \
    rameditwidgets/assetgroupeditwidget.cpp \
    rameditwidgets/assetgroupsmanagerwidget.cpp \
    rameditwidgets/assetsmanagerwidget.cpp \
    daemon.cpp \
    rameditwidgets/filetypelistmanagerwidget.cpp \
    rameditwidgets/objectlisteditwidget.cpp \
    rameditwidgets/objectlistmanagerwidget.cpp \
    rameditwidgets/projectlistmanagerwidget.cpp \
    rameditwidgets/shotlistmanagerwidget.cpp \
    rameditwidgets/statelistmanagerwidget.cpp \
    rameditwidgets/templateassetgrouplistmanagerwidget.cpp \
    rameditwidgets/templatesteplistmanagerwidget.cpp \
    rameditwidgets/userlistmanagerwidget.cpp \
    ramobjects/ramobjectuberlist.cpp \
    ramobjects/ramstatelist.cpp \
    settingswidgets/daemonsettingswidget.cpp \
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
    duqf-utils/guiutils.cpp \
    duqf-utils/language-utils.cpp \
    duqf-utils/utils.cpp \
    duqf-widgets/aboutdialog.cpp \
    duqf-widgets/appearancesettingswidget.cpp \
    duqf-widgets/autoselectdoublespinbox.cpp \
    duqf-widgets/autoselectlineedit.cpp \
    duqf-widgets/autoselectspinbox.cpp \
    duqf-widgets/duqfdocktitle.cpp \
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
    rameditwidgets/filetypeeditwidget.cpp \
    smallwidgets/frameratewidget.cpp \
    rameditwidgets/listmanagerwidget.cpp \
    settingswidgets/localsettingswidget.cpp \
    loginpage.cpp \
    main.cpp \
    mainwindow.cpp \
    rameditwidgets/objectdockwidget.cpp \
    rameditwidgets/objecteditwidget.cpp \
    rameditwidgets/objectlistwidget.cpp \
    rameditwidgets/pipeeditwidget.cpp \
    pipeline-editor/objectnode.cpp \
    pipeline-editor/pipelinewidget.cpp \
    pipeline-editor/stepnode.cpp \
    rameditwidgets/projecteditwidget.cpp \
    smallwidgets/projectselectorwidget.cpp \
    ramobjects/ramapplication.cpp \
    ramobjects/ramasset.cpp \
    ramobjects/ramassetgroup.cpp \
    ramdisplaywidgets/ramassetwidget.cpp \
    ramobjects/ramfiletype.cpp \
    ramobjects/ramitem.cpp \
    ramobjects/ramobject.cpp \
    ramobjects/ramobjectlist.cpp \
    ramdisplaywidgets/ramobjectlistwidget.cpp \
    ramdisplaywidgets/ramobjectwidget.cpp \
    ramobjects/rampipe.cpp \
    ramobjects/ramproject.cpp \
    ramobjects/ramsequence.cpp \
    ramobjects/ramses.cpp \
    ramobjects/ramshot.cpp \
    ramdisplaywidgets/ramshotwidget.cpp \
    ramobjects/ramstate.cpp \
    ramobjects/ramstatus.cpp \
    ramobjects/ramstatushistory.cpp \
    ramdisplaywidgets/ramstatuswidget.cpp \
    ramobjects/ramstep.cpp \
    ramobjects/ramuser.cpp \
    ramobjects/ramuuid.cpp \
    smallwidgets/resolutionwidget.cpp \
    rameditwidgets/sequenceeditwidget.cpp \
    rameditwidgets/sequencesmanagerwidget.cpp \
    settingswidgets/serversettingswidget.cpp \
    rameditwidgets/shoteditwidget.cpp \
    rameditwidgets/simpleobjectlist.cpp \
    smallwidgets/statebox.cpp \
    rameditwidgets/stateeditwidget.cpp \
    rameditwidgets/statuseditwidget.cpp \
    rameditwidgets/statushistorywidget.cpp \
    rameditwidgets/stepeditwidget.cpp \
    rameditwidgets/stepsmanagerwidget.cpp \
    rameditwidgets/templateassetgroupeditwidget.cpp \
    rameditwidgets/templatestepeditwidget.cpp \
    rameditwidgets/usereditwidget.cpp \
    userprofilepage.cpp

HEADERS += \
    duqf-widgets/duqfsearchedit.h \
    ramdisplaywidgets/ramapplicationwidget.h \
    ramdisplaywidgets/ramassetgroupwidget.h \
    ramdisplaywidgets/ramfiletypewidget.h \
    ramdisplaywidgets/ramprojectwidget.h \
    ramdisplaywidgets/ramsequencewidget.h \
    ramdisplaywidgets/ramstatewidget.h \
    ramdisplaywidgets/ramstepwidget.h \
    ramdisplaywidgets/ramuserwidget.h \
    rameditwidgets/applicationeditwidget.h \
    rameditwidgets/applicationlistmanagerwidget.h \
    rameditwidgets/asseteditwidget.h \
    rameditwidgets/assetgroupeditwidget.h \
    rameditwidgets/assetgroupsmanagerwidget.h \
    rameditwidgets/assetsmanagerwidget.h \
    daemon.h \
    rameditwidgets/filetypelistmanagerwidget.h \
    rameditwidgets/objectlisteditwidget.h \
    rameditwidgets/objectlistmanagerwidget.h \
    rameditwidgets/projectlistmanagerwidget.h \
    rameditwidgets/shotlistmanagerwidget.h \
    rameditwidgets/statelistmanagerwidget.h \
    rameditwidgets/templateassetgrouplistmanagerwidget.h \
    rameditwidgets/templatesteplistmanagerwidget.h \
    rameditwidgets/userlistmanagerwidget.h \
    ramobjects/ramobjectuberlist.h \
    ramobjects/ramstatelist.h \
    settingswidgets/daemonsettingswidget.h \
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
    duqf-utils/guiutils.h \
    duqf-utils/language-utils.h \
    duqf-utils/utils.h \
    duqf-widgets/aboutdialog.h \
    duqf-widgets/appearancesettingswidget.h \
    duqf-widgets/autoselectdoublespinbox.h \
    duqf-widgets/autoselectlineedit.h \
    duqf-widgets/autoselectspinbox.h \
    duqf-widgets/duqfdocktitle.h \
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
    rameditwidgets/filetypeeditwidget.h \
    smallwidgets/frameratewidget.h \
    rameditwidgets/listmanagerwidget.h \
    settingswidgets/localsettingswidget.h \
    loginpage.h \
    mainwindow.h \
    rameditwidgets/objectdockwidget.h \
    rameditwidgets/objecteditwidget.h \
    rameditwidgets/objectlistwidget.h \
    rameditwidgets/pipeeditwidget.h \
    pipeline-editor/objectnode.h \
    pipeline-editor/pipelinewidget.h \
    pipeline-editor/stepnode.h \
    rameditwidgets/projecteditwidget.h \
    smallwidgets/projectselectorwidget.h \
    ramobjects/ramapplication.h \
    ramobjects/ramasset.h \
    ramobjects/ramassetgroup.h \
    ramdisplaywidgets/ramassetwidget.h \
    ramobjects/ramfiletype.h \
    ramobjects/ramitem.h \
    ramobjects/ramobject.h \
    ramobjects/ramobjectlist.h \
    ramdisplaywidgets/ramobjectlistwidget.h \
    ramdisplaywidgets/ramobjectwidget.h \
    ramobjects/rampipe.h \
    ramobjects/ramproject.h \
    ramobjects/ramsequence.h \
    ramobjects/ramses.h \
    ramobjects/ramshot.h \
    ramdisplaywidgets/ramshotwidget.h \
    ramobjects/ramstate.h \
    ramobjects/ramstatus.h \
    ramobjects/ramstatushistory.h \
    ramdisplaywidgets/ramstatuswidget.h \
    ramobjects/ramstep.h \
    ramobjects/ramuser.h \
    ramobjects/ramuuid.h \
    smallwidgets/resolutionwidget.h \
    rameditwidgets/sequenceeditwidget.h \
    rameditwidgets/sequencesmanagerwidget.h \
    settingswidgets/serversettingswidget.h \
    rameditwidgets/shoteditwidget.h \
    rameditwidgets/simpleobjectlist.h \
    smallwidgets/statebox.h \
    rameditwidgets/stateeditwidget.h \
    rameditwidgets/statuseditwidget.h \
    rameditwidgets/statushistorywidget.h \
    rameditwidgets/stepeditwidget.h \
    rameditwidgets/stepsmanagerwidget.h \
    rameditwidgets/templateassetgroupeditwidget.h \
    rameditwidgets/templatestepeditwidget.h \
    rameditwidgets/usereditwidget.h \
    userprofilepage.h

FORMS += \
    rameditwidgets/asseteditwidget.ui \
    rameditwidgets/assetgroupeditwidget.ui \
    settingswidgets/daemonsettingswidget.ui \
    duqf-widgets/aboutdialog.ui \
    duqf-widgets/appearancesettingswidget.ui \
    duqf-widgets/duqffolderselectorwidget.ui \
    duqf-widgets/toolbarspacer.ui \
    rameditwidgets/listmanagerwidget.ui \
    settingswidgets/localsettingswidget.ui \
    loginpage.ui \
    mainwindow.ui \
    pipeline-editor/pipelinewidget.ui \
    rameditwidgets/projecteditwidget.ui \
    settingswidgets/serversettingswidget.ui \
    rameditwidgets/stepeditwidget.ui \
    rameditwidgets/templateassetgroupeditwidget.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# OS Specific configurations
win* {
    # Add version and other metadata
    DISTFILES += app.rc
    RC_FILE = app.rc
    !build_pass:touch($$RC_FILE, duqf-app/app-version.h)
    # Enable console output
    # CONFIG += console
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
