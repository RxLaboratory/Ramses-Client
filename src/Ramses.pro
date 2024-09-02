QT       += core gui \
            sql \
            network \
            svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# On linux, it's more standard to use lower case names
unix:!macx { TARGET = ramses }
else { TARGET = DuME }

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
    smallwidgets \
    ramobjects \
    item-tables \
    data-views \
    pages \
    ramdatainterface \
    ramobjectdelegates \
    ramobjectmodels \
    ramobjectviews \
    rammanagerwidgets

SOURCES += \
    DuGraph/dugraph.cpp \
    docks/settingsdock.cpp \
    duqf-app/app-utils.cpp \
    duqf-app/ducli.cpp \
    duqf-app/dulogger.cpp \
    duqf-app/dusettings.cpp \
    duqf-app/dusettingsmanager.cpp \
    duqf-app/dustyle.cpp \
    duqf-app/duui.cpp \
    duqf-utils/ducolorutils.cpp \
    duqf-utils/stringutils.cpp \
    duqf-widgets/duaction.cpp \
    duqf-widgets/ducolorselector.cpp \
    duqf-widgets/ducombobox.cpp \
    duqf-widgets/dudocktitlewidget.cpp \
    duqf-widgets/dudockwidget.cpp \
    duqf-widgets/duicon.cpp \
    duqf-widgets/dulabel.cpp \
    duqf-widgets/dulineedit.cpp \
    duqf-widgets/dulistwidget.cpp \
    duqf-widgets/dumainwindow.cpp \
    duqf-widgets/dumenu.cpp \
    duqf-widgets/duscrollarea.cpp \
    duqf-widgets/duscrollbar.cpp \
    duqf-widgets/dushadow.cpp \
    duqf-widgets/duspinbox.cpp \
    duqf-widgets/dusplashscreen.cpp \
    duqf-widgets/dusvgiconengine.cpp \
    duqf-widgets/dutableview.cpp \
    duqf-widgets/dutabwidget.cpp \
    duqf-widgets/dutextedit.cpp \
    duqf-widgets/dutoolbarspacer.cpp \
    duqf-widgets/dutreewidget.cpp \
    duqf-widgets/settingswidget.cpp \
    progressmanager.cpp \
    rameditwidgets/objectupdateblocker.cpp \
    rameditwidgets/scheduleentryeditwidget.cpp \
    rameditwidgets/scheduleroweditwidget.cpp \
    rammanagerwidgets/dbmanagerwidget.cpp \
    rammanagerwidgets/objectlistwidget.cpp \
    rammanagerwidgets/objectmanagerwidget.cpp \
    rammanagerwidgets/sequencemanagerwidget.cpp \
    rammanagerwidgets/shotmanagerwidget.cpp \
    rammanagerwidgets/statemanagerwidget.cpp \
    rammanagerwidgets/stepmanagerwidget.cpp \
    rammanagerwidgets/usermanagerwidget.cpp \
    ramobjectdelegates/ramobjectdelegate.cpp \
    ramobjectdelegates/ramschedulerowheaderview.cpp \
    ramobjectmodels/dbtablemodel.cpp \
    ramobjectmodels/ramabstractdatamodel.cpp \
    ramobjectmodels/ramabstractobjectmodel.cpp \
    ramobjectmodels/ramitemsortfilterproxymodel.cpp \
    ramobjectmodels/ramfilterlistproxymodel.cpp \
    ramobjectmodels/ramobjectmodel.cpp \
    ramobjectmodels/ramobjectsortfilterproxymodel.cpp \
    ramobjectmodels/ramscheduleentrymodel.cpp \
    ramobjectmodels/ramstatustablemodel.cpp \
    ramobjectmodels/statisticsmodel.cpp \
    ramobjectmodels/timelineproxy.cpp \
    ramobjectmodels/ramschedulefilterproxymodel.cpp \
    ramobjectmodels/ramscheduletablemodel.cpp \
    ramobjects/ramscheduleentry.cpp \
    ramobjects/ramschedulerow.cpp \
    ramobjectviews/ramitemview.cpp \
    ramobjectviews/ramobjectcombobox.cpp \
    ramobjectviews/ramobjectmenu.cpp \
    ramobjectdelegates/ramscheduledelegate.cpp \
    ramobjectviews/ramscheduletableview.cpp \
    ramobjectdelegates/statisticsdelegate.cpp \
    ramobjectdelegates/ramstepheaderview.cpp \
    ramobjectdelegates/timelinedelegate.cpp \
    ramobjectviews/statisticsview.cpp \
    ramobjectviews/timelineview.cpp \
    docks/consolewidget.cpp \
    docks/filemanagerwidget.cpp \
    duqf-utils/qtransposeproxymodel.cpp \
    duqf-utils/simplecrypt.cpp \
    duqf-widgets/duqfautosizetoolbutton.cpp \
    duqf-widgets/duqfcolorselector.cpp \
    duqf-widgets/duqfelidedlabel.cpp \
    duqf-widgets/duqffilelist.cpp \
    duqf-widgets/duqfsearchedit.cpp \
    duqf-widgets/duqfservercombobox.cpp \
    duqf-widgets/duqfsslcheckbox.cpp \
    duqf-widgets/duqftextedit.cpp \
    duqf-widgets/duqftitlebar.cpp \
    duqf-widgets/duqfupdatedialog.cpp \
    ramdatainterface/datacrypto.cpp \
    ramdatainterface/localdatainterface.cpp \
    ramdatainterface/logindialog.cpp \
    ramdatainterface/ramserverinterface.cpp \
    rameditwidgets/applicationeditwidget.cpp \
    rammanagerwidgets/applicationmanagerwidget.cpp \
    rameditwidgets/asseteditwidget.cpp \
    rameditwidgets/assetgroupeditwidget.cpp \
    progressbar.cpp \
    progresspage.cpp \
    daemon.cpp \
    rammanagerwidgets/assetgroupmanagerwidget.cpp \
    rammanagerwidgets/assetmanagerwidget.cpp \
    rameditwidgets/databasecreatewidget.cpp \
    rameditwidgets/databaseeditwidget.cpp \
    rammanagerwidgets/filetypemanagerwidget.cpp \
    rameditwidgets/pipefileeditwidget.cpp \
    rammanagerwidgets/pipefilemanagerwidget.cpp \
    rammanagerwidgets/projectmanagerwidget.cpp \
    rammanagerwidgets/schedulemanagerwidget.cpp \
    ramobjects/ramabstractitem.cpp \
    ramobjectviews/ramobjectview.cpp \
    smallwidgets/ramstatebox.cpp \
    smallwidgets/scheduleentrycreationdialog.cpp \
    smallwidgets/servereditwidget.cpp \
    docks/statisticswidget.cpp \
    rameditwidgets/stepstatushistorywidget.cpp \
    rammanagerwidgets/templateassetgroupmanagerwidget.cpp \
    rammanagerwidgets/templatestepmanagerwidget.cpp \
    docks/timelinewidget.cpp \
    ramfilemetadatamanager.cpp \
    ramnamemanager.cpp \
    ramobjects/ramabstractobject.cpp \
    ramobjects/ramassetgroup.cpp \
    ramobjects/rampipefile.cpp \
    ramobjects/ramsequence.cpp \
    ramobjects/ramtemplateassetgroup.cpp \
    ramobjects/ramtemplatestep.cpp \
    ramobjects/ramworkingfolder.cpp \
    ramdatainterface/dbinterface.cpp \
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
    duqf-widgets/autoselectdoublespinbox.cpp \
    duqf-widgets/autoselectlineedit.cpp \
    duqf-widgets/autoselectspinbox.cpp \
    duqf-widgets/duqfdoubleslider.cpp \
    duqf-widgets/duqfdoublespinbox.cpp \
    duqf-widgets/duqffolderdisplaywidget.cpp \
    duqf-widgets/duqffolderselectorwidget.cpp \
    duqf-widgets/duqflistwidget.cpp \
    duqf-widgets/duqfloggingtextedit.cpp \
    duqf-widgets/duqflogtoolbutton.cpp \
    duqf-widgets/duqfslider.cpp \
    duqf-widgets/duqfspinbox.cpp \
    rameditwidgets/filetypeeditwidget.cpp \
    smallwidgets/frameratewidget.cpp \
    pages/loginpage.cpp \
    main.cpp \
    mainwindow.cpp \
    rameditwidgets/objecteditwidget.cpp \
    rameditwidgets/pipeeditwidget.cpp \
    rameditwidgets/projecteditwidget.cpp \
    smallwidgets/progresswidget.cpp \
    smallwidgets/projectselectorwidget.cpp \
    ramobjects/ramapplication.cpp \
    ramobjects/ramasset.cpp \
    ramobjects/ramfiletype.cpp \
    ramobjects/ramobject.cpp \
    ramobjects/rampipe.cpp \
    ramobjects/ramproject.cpp \
    ramobjects/ramses.cpp \
    ramobjects/ramshot.cpp \
    ramobjects/ramstate.cpp \
    ramobjects/ramstatus.cpp \
    ramobjects/ramstep.cpp \
    ramobjects/ramuser.cpp \
    ramobjects/ramuuid.cpp \
    smallwidgets/resolutionwidget.cpp \
    rameditwidgets/sequenceeditwidget.cpp \
    rameditwidgets/shoteditwidget.cpp \
    smallwidgets/shotscreationdialog.cpp \
    rameditwidgets/stateeditwidget.cpp \
    rameditwidgets/statuseditwidget.cpp \
    rameditwidgets/statushistorywidget.cpp \
    rameditwidgets/stepeditwidget.cpp \
    rameditwidgets/templateassetgroupeditwidget.cpp \
    rameditwidgets/templatestepeditwidget.cpp \
    rameditwidgets/usereditwidget.cpp \
    statemanager.cpp \
    timelinemanager.cpp \
    rammanagerwidgets/itemmanagerwidget.cpp \
    pipeline-editor/objectnode.cpp \
    pipeline-editor/pipelinewidget.cpp \
    pipeline-editor/stepnode.cpp

HEADERS += \
    DuGraph/dugraph.h \
    config.h \
    docks/settingsdock.h \
    duqf-app/app-utils.h \
    duqf-app/ducli.h \
    duqf-app/dulogger.h \
    duqf-app/dusettings.h \
    duqf-app/dusettingsmanager.h \
    duqf-app/dustyle.h \
    duqf-app/duui.h \
    duqf-utils/colorutils.h \
    duqf-utils/ducolorutils.h \
    duqf-utils/stringutils.h \
    duqf-widgets/duaction.h \
    duqf-widgets/ducolorselector.h \
    duqf-widgets/ducombobox.h \
    duqf-widgets/dudocktitlewidget.h \
    duqf-widgets/dudockwidget.h \
    duqf-widgets/duicon.h \
    duqf-widgets/dulabel.h \
    duqf-widgets/dulineedit.h \
    duqf-widgets/dulistwidget.h \
    duqf-widgets/dumainwindow.h \
    duqf-widgets/dumenu.h \
    duqf-widgets/duscrollarea.h \
    duqf-widgets/duscrollbar.h \
    duqf-widgets/dushadow.h \
    duqf-widgets/duspinbox.h \
    duqf-widgets/dusplashscreen.h \
    duqf-widgets/dusvgiconengine.h \
    duqf-widgets/dutableview.h \
    duqf-widgets/dutabwidget.h \
    duqf-widgets/dutextedit.h \
    duqf-widgets/dutoolbarspacer.h \
    duqf-widgets/dutreewidget.h \
    duqf-widgets/settingswidget.h \
    enums.h \
    progressmanager.h \
    ramdatainterface/datastruct.h \
    rameditwidgets/objectupdateblocker.h \
    rameditwidgets/scheduleentryeditwidget.h \
    rameditwidgets/scheduleroweditwidget.h \
    rammanagerwidgets/dbmanagerwidget.h \
    rammanagerwidgets/objectlistwidget.h \
    rammanagerwidgets/objectmanagerwidget.h \
    rammanagerwidgets/sequencemanagerwidget.h \
    rammanagerwidgets/shotmanagerwidget.h \
    rammanagerwidgets/statemanagerwidget.h \
    rammanagerwidgets/stepmanagerwidget.h \
    ramobjectdelegates/ramobjectdelegate.h \
    ramobjectdelegates/ramschedulerowheaderview.h \
    ramobjectmodels/dbtablemodel.h \
    ramobjectmodels/ramabstractdatamodel.h \
    ramobjectmodels/ramabstractobjectmodel.h \
    ramobjectmodels/ramitemsortfilterproxymodel.h \
    ramobjectmodels/ramfilterlistproxymodel.h \
    ramobjectmodels/ramobjectmodel.h \
    ramobjectmodels/ramobjectsortfilterproxymodel.h \
    ramobjectmodels/ramscheduleentrymodel.h \
    ramobjectmodels/ramstatustablemodel.h \
    ramobjectmodels/statisticsmodel.h \
    ramobjectmodels/timelineproxy.h \
    ramobjectmodels/ramschedulefilterproxymodel.h \
    ramobjectmodels/ramscheduletablemodel.h \
    ramobjects/ramscheduleentry.h \
    ramobjects/ramschedulerow.h \
    ramobjectviews/ramitemview.h \
    ramobjectviews/ramobjectcombobox.h \
    ramobjectviews/ramobjectmenu.h \
    ramobjectdelegates/ramscheduledelegate.h \
    ramobjectviews/ramscheduletableview.h \
    ramobjectdelegates/statisticsdelegate.h \
    ramobjectdelegates/ramstepheaderview.h \
    ramobjectdelegates/timelinedelegate.h \
    ramobjectviews/statisticsview.h \
    ramobjectviews/timelineview.h \
    ramdatainterface/datacrypto.h \
    ramdatainterface/dbistructures.h \
    docks/consolewidget.h \
    docks/filemanagerwidget.h \
    duqf-app/app-config.h \
    duqf-utils/qtransposeproxymodel.h \
    duqf-utils/simplecrypt.h \
    duqf-widgets/duqfautosizetoolbutton.h \
    duqf-widgets/duqfcolorselector.h \
    duqf-widgets/duqfelidedlabel.h \
    duqf-widgets/duqffilelist.h \
    duqf-widgets/duqfsearchedit.h \
    duqf-widgets/duqfservercombobox.h \
    duqf-widgets/duqfsslcheckbox.h \
    duqf-widgets/duqftextedit.h \
    duqf-widgets/duqftitlebar.h \
    duqf-widgets/duqfupdatedialog.h \
    progressbar.h \
    progresspage.h \
    ramdatainterface/localdatainterface.h \
    ramdatainterface/logindialog.h \
    ramdatainterface/ramserverinterface.h \
    rameditwidgets/applicationeditwidget.h \
    rammanagerwidgets/applicationmanagerwidget.h \
    rameditwidgets/asseteditwidget.h \
    rameditwidgets/assetgroupeditwidget.h \
    daemon.h \
    rammanagerwidgets/assetgroupmanagerwidget.h \
    rammanagerwidgets/assetmanagerwidget.h \
    rameditwidgets/databasecreatewidget.h \
    rameditwidgets/databaseeditwidget.h \
    rammanagerwidgets/filetypemanagerwidget.h \
    rameditwidgets/pipefileeditwidget.h \
    rammanagerwidgets/pipefilemanagerwidget.h \
    rammanagerwidgets/projectmanagerwidget.h \
    rammanagerwidgets/schedulemanagerwidget.h \
    ramobjects/ramabstractitem.h \
    ramobjectviews/ramobjectview.h \
    smallwidgets/ramstatebox.h \
    smallwidgets/scheduleentrycreationdialog.h \
    smallwidgets/servereditwidget.h \
    docks/statisticswidget.h \
    rameditwidgets/stepstatushistorywidget.h \
    rammanagerwidgets/templateassetgroupmanagerwidget.h \
    rammanagerwidgets/templatestepmanagerwidget.h \
    docks/timelinewidget.h \
    rammanagerwidgets/usermanagerwidget.h \
    ramfilemetadatamanager.h \
    ramnamemanager.h \
    ramobjects/ramabstractobject.h \
    ramobjects/ramassetgroup.h \
    ramobjects/rampipefile.h \
    ramobjects/ramsequence.h \
    ramobjects/ramtemplateassetgroup.h \
    ramobjects/ramtemplatestep.h \
    ramobjects/ramworkingfolder.h \
    ramdatainterface/dbinterface.h \
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
    duqf-widgets/autoselectdoublespinbox.h \
    duqf-widgets/autoselectlineedit.h \
    duqf-widgets/autoselectspinbox.h \
    duqf-widgets/duqfdoubleslider.h \
    duqf-widgets/duqfdoublespinbox.h \
    duqf-widgets/duqffolderdisplaywidget.h \
    duqf-widgets/duqffolderselectorwidget.h \
    duqf-widgets/duqflistwidget.h \
    duqf-widgets/duqfloggingtextedit.h \
    duqf-widgets/duqflogtoolbutton.h \
    duqf-widgets/duqfslider.h \
    duqf-widgets/duqfspinbox.h \
    rameditwidgets/filetypeeditwidget.h \
    smallwidgets/frameratewidget.h \
    pages/loginpage.h \
    mainwindow.h \
    rameditwidgets/objecteditwidget.h \
    rameditwidgets/pipeeditwidget.h \
    rameditwidgets/projecteditwidget.h \
    smallwidgets/progresswidget.h \
    smallwidgets/projectselectorwidget.h \
    ramobjects/ramapplication.h \
    ramobjects/ramasset.h \
    ramobjects/ramfiletype.h \
    ramobjects/ramobject.h \
    ramobjects/rampipe.h \
    ramobjects/ramproject.h \
    ramobjects/ramses.h \
    ramobjects/ramshot.h \
    ramobjects/ramstate.h \
    ramobjects/ramstatus.h \
    ramobjects/ramstep.h \
    ramobjects/ramuser.h \
    ramobjects/ramuuid.h \
    smallwidgets/resolutionwidget.h \
    rameditwidgets/sequenceeditwidget.h \
    rameditwidgets/shoteditwidget.h \
    smallwidgets/shotscreationdialog.h \
    rameditwidgets/stateeditwidget.h \
    rameditwidgets/statuseditwidget.h \
    rameditwidgets/statushistorywidget.h \
    rameditwidgets/stepeditwidget.h \
    rameditwidgets/templateassetgroupeditwidget.h \
    rameditwidgets/templatestepeditwidget.h \
    rameditwidgets/usereditwidget.h \
    statemanager.h \
    timelinemanager.h \
    rammanagerwidgets/itemmanagerwidget.h \
    pipeline-editor/objectnode.h \
    pipeline-editor/pipelinewidget.h \
    pipeline-editor/stepnode.h

FORMS += \
    duqf-widgets/aboutdialog.ui \
    duqf-widgets/duqffolderselectorwidget.ui \
    pages/loginpage.ui \
    mainwindow.ui \
    pipeline-editor/pipelinewidget.ui \
    smallwidgets/shotscreationdialog.ui

include($$PWD/../include/QGoodWindow/QGoodWindow/QGoodWindow.pri)
include($$PWD/../include/QGoodWindow/QGoodCentralWidget/QGoodCentralWidget.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# OS Specific configurations
win* {

    # ==== FRAMELESS WINDOW ====
    # Only include / compile these files on Windows
    SOURCES += \
        OSUtils/windows/qwinwidget.cpp \
        OSUtils/windows/winnativewindow.cpp \

    HEADERS += \
        OSUtils/windows/qwinwidget.h \
        OSUtils/windows/winnativewindow.h \

    LIBS += -L"C:\Program Files\Microsoft SDKs\Windows\v7.1\Lib" \
        -ldwmapi \
        -lgdi32
    # ========

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
    }
    # Ignore annoying errors on older versions of Qt
    QMAKE_CXXFLAGS += -Wdeprecated \
        -Wdeprecated-copy
} else:macx {
    # icon
    ICON = resources/icons/ramses.icns
    CONFIG += sdk_no_version_check
}

RESOURCES += \
    duqf_resources.qrc \
    resources.qrc
