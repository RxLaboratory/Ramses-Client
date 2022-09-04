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

# Try to speed up build with precompiled headers
PRECOMPILED_HEADER = pch/pch.h
CONFIG += precompile_header

INCLUDEPATH += rameditwidgets \
    settingswidgets \
    smallwidgets \
    ramobjects \
    item-tables \
    data-views \
    pages \
    data-model \
    ramdatainterface

SOURCES += \
    data-models/dbtablemodel.cpp \
    data-models/ramitemfiltermodel.cpp \
    data-models/ramitemtablelistproxy.cpp \
    data-models/ramobjectfilterlist.cpp \
    data-models/ramobjectfiltermodel.cpp \
    data-models/ramobjectmodel.cpp \
    data-models/ramprojectfiltermodel.cpp \
    data-models/ramschedulefilter.cpp \
    data-models/ramscheduletable.cpp \
    data-models/ramstatisticstable.cpp \
    data-models/timelineproxy.cpp \
    data-views/ramobjectlistmenu.cpp \
    data-views/ramobjectlistview.cpp \
    data-views/ramscheduledelegate.cpp \
    data-views/ramscheduletablewidget.cpp \
    data-views/ramstatisticsdelegate.cpp \
    data-views/ramstatisticstablewidget.cpp \
    data-views/ramstepheaderview.cpp \
    data-views/timelinedelegate.cpp \
    data-views/timelineview.cpp \
    docks/consolewidget.cpp \
    docks/filemanagerwidget.cpp \
    duqf-utils/qtransposeproxymodel.cpp \
    duqf-utils/simplecrypt.cpp \
    duqf-widgets/duqfautosizetoolbutton.cpp \
    duqf-widgets/duqfcolorselector.cpp \
    duqf-widgets/duqfelidedlabel.cpp \
    duqf-widgets/duqffilelist.cpp \
    duqf-widgets/duqfsearchedit.cpp \
    data-models/ramitemtable.cpp \
    data-models/ramobjectlist.cpp \
    data-models/ramstatelist.cpp \
    data-models/ramstepstatushistory.cpp \
    data-views/ramobjectdelegate.cpp \
    data-views/ramobjectlistcombobox.cpp \
    duqf-widgets/duqfservercombobox.cpp \
    duqf-widgets/duqfsslcheckbox.cpp \
    duqf-widgets/duqftextedit.cpp \
    duqf-widgets/duqftitlebar.cpp \
    duqf-widgets/duqftoolbarspacer.cpp \
    duqf-widgets/duqfupdatedialog.cpp \
    duqf-widgets/duqfupdatesettingswidget.cpp \
    pages/projectpage.cpp \
    ramdatainterface/datacrypto.cpp \
    ramdatainterface/dbsync.cpp \
    ramdatainterface/localdatainterface.cpp \
    ramdatainterface/logindialog.cpp \
    ramdatainterface/ramserverinterface.cpp \
    rameditwidgets/applicationeditwidget.cpp \
    rameditwidgets/applicationlistmanagerwidget.cpp \
    rameditwidgets/asseteditwidget.cpp \
    rameditwidgets/assetgroupeditwidget.cpp \
    processmanager.cpp \
    progressbar.cpp \
    progresspage.cpp \
    daemon.cpp \
    rameditwidgets/assetgrouplistmanagerwidget.cpp \
    rameditwidgets/assetlistmanagerwidget.cpp \
    rameditwidgets/databasecreatewidget.cpp \
    rameditwidgets/databaseeditwidget.cpp \
    rameditwidgets/filetypelistmanagerwidget.cpp \
    rameditwidgets/objectlisteditwidget.cpp \
    rameditwidgets/objectlistmanagerwidget.cpp \
    rameditwidgets/pipefileeditwidget.cpp \
    rameditwidgets/pipefilelistmanagerwidget.cpp \
    rameditwidgets/projectlistmanagerwidget.cpp \
    rameditwidgets/schedulemanagerwidget.cpp \
    rameditwidgets/sequencelistmanagerwidget.cpp \
    ramobjects/ramabstractitem.cpp \
    smallwidgets/servereditwidget.cpp \
    rameditwidgets/shotlistmanagerwidget.cpp \
    rameditwidgets/statelistmanagerwidget.cpp \
    docks/statisticswidget.cpp \
    rameditwidgets/steplistmanagerwidget.cpp \
    rameditwidgets/stepstatushistorywidget.cpp \
    rameditwidgets/templateassetgrouplistmanagerwidget.cpp \
    rameditwidgets/templatesteplistmanagerwidget.cpp \
    docks/timelinewidget.cpp \
    rameditwidgets/userlistmanagerwidget.cpp \
    ramfilemetadatamanager.cpp \
    ramnamemanager.cpp \
    ramobjects/ramabstractobject.cpp \
    ramobjects/ramassetgroup.cpp \
    ramobjects/rampipefile.cpp \
    ramobjects/ramschedulecomment.cpp \
    ramobjects/ramscheduleentry.cpp \
    ramobjects/ramsequence.cpp \
    ramobjects/ramtemplateassetgroup.cpp \
    ramobjects/ramtemplatestep.cpp \
    ramobjects/ramworkingfolder.cpp \
    settingswidgets/daemonsettingswidget.cpp \
    ramdatainterface/dbinterface.cpp \
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
    smallwidgets/statebox.cpp \
    rameditwidgets/stateeditwidget.cpp \
    rameditwidgets/statuseditwidget.cpp \
    rameditwidgets/statushistorywidget.cpp \
    rameditwidgets/stepeditwidget.cpp \
    rameditwidgets/templateassetgroupeditwidget.cpp \
    rameditwidgets/templatestepeditwidget.cpp \
    rameditwidgets/usereditwidget.cpp \
    timelinemanager.cpp \
    userprofilepage.cpp \
    rameditwidgets/itemtablemanagerwidget.cpp \
    pipeline-editor/objectnode.cpp \
    pipeline-editor/pipelinewidget.cpp \
    pipeline-editor/stepnode.cpp

HEADERS += \
    config.h \
    data-models/dbtablemodel.h \
    data-models/ramitemfiltermodel.h \
    data-models/ramitemtable.h \
    data-models/ramitemtablelistproxy.h \
    data-models/ramobjectfilterlist.h \
    data-models/ramobjectfiltermodel.h \
    data-models/ramobjectlist.h \
    data-models/ramobjectmodel.h \
    data-models/ramprojectfiltermodel.h \
    data-models/ramschedulefilter.h \
    data-models/ramscheduletable.h \
    data-models/ramstatelist.h \
    data-models/ramstatisticstable.h \
    data-models/ramstepstatushistory.h \
    data-models/timelineproxy.h \
    data-views/ramobjectdelegate.h \
    data-views/ramobjectlistmenu.h \
    data-views/ramobjectlistview.h \
    data-views/ramscheduledelegate.h \
    data-views/ramscheduletablewidget.h \
    data-views/ramstatisticsdelegate.h \
    data-views/ramstatisticstablewidget.h \
    data-views/ramstepheaderview.h \
    data-views/timelinedelegate.h \
    data-views/timelineview.h \
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
    duqf-widgets/duqftoolbarspacer.h \
    duqf-widgets/duqfupdatedialog.h \
    duqf-widgets/duqfupdatesettingswidget.h \
    processmanager.h \
    progressbar.h \
    progresspage.h \
    data-views/ramobjectlistcombobox.h \
    pages/projectpage.h \
    ramdatainterface/dbsync.h \
    ramdatainterface/localdatainterface.h \
    ramdatainterface/logindialog.h \
    ramdatainterface/ramserverinterface.h \
    rameditwidgets/applicationeditwidget.h \
    rameditwidgets/applicationlistmanagerwidget.h \
    rameditwidgets/asseteditwidget.h \
    rameditwidgets/assetgroupeditwidget.h \
    daemon.h \
    rameditwidgets/assetgrouplistmanagerwidget.h \
    rameditwidgets/assetlistmanagerwidget.h \
    rameditwidgets/databasecreatewidget.h \
    rameditwidgets/databaseeditwidget.h \
    rameditwidgets/filetypelistmanagerwidget.h \
    rameditwidgets/objectlisteditwidget.h \
    rameditwidgets/objectlistmanagerwidget.h \
    rameditwidgets/pipefileeditwidget.h \
    rameditwidgets/pipefilelistmanagerwidget.h \
    rameditwidgets/projectlistmanagerwidget.h \
    rameditwidgets/schedulemanagerwidget.h \
    rameditwidgets/sequencelistmanagerwidget.h \
    ramobjects/ramabstractitem.h \
    smallwidgets/servereditwidget.h \
    rameditwidgets/shotlistmanagerwidget.h \
    rameditwidgets/statelistmanagerwidget.h \
    docks/statisticswidget.h \
    rameditwidgets/steplistmanagerwidget.h \
    rameditwidgets/stepstatushistorywidget.h \
    rameditwidgets/templateassetgrouplistmanagerwidget.h \
    rameditwidgets/templatesteplistmanagerwidget.h \
    docks/timelinewidget.h \
    rameditwidgets/userlistmanagerwidget.h \
    ramfilemetadatamanager.h \
    ramnamemanager.h \
    ramobjects/ramabstractobject.h \
    ramobjects/ramassetgroup.h \
    ramobjects/rampipefile.h \
    ramobjects/ramschedulecomment.h \
    ramobjects/ramscheduleentry.h \
    ramobjects/ramsequence.h \
    ramobjects/ramtemplateassetgroup.h \
    ramobjects/ramtemplatestep.h \
    ramobjects/ramworkingfolder.h \
    settingswidgets/daemonsettingswidget.h \
    ramdatainterface/dbinterface.h \
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
    smallwidgets/statebox.h \
    rameditwidgets/stateeditwidget.h \
    rameditwidgets/statuseditwidget.h \
    rameditwidgets/statushistorywidget.h \
    rameditwidgets/stepeditwidget.h \
    rameditwidgets/templateassetgroupeditwidget.h \
    rameditwidgets/templatestepeditwidget.h \
    rameditwidgets/usereditwidget.h \
    timelinemanager.h \
    userprofilepage.h \
    rameditwidgets/itemtablemanagerwidget.h \
    pipeline-editor/objectnode.h \
    pipeline-editor/pipelinewidget.h \
    pipeline-editor/stepnode.h

FORMS += \
    settingswidgets/daemonsettingswidget.ui \
    duqf-widgets/aboutdialog.ui \
    duqf-widgets/appearancesettingswidget.ui \
    duqf-widgets/duqffolderselectorwidget.ui \
    pages/loginpage.ui \
    mainwindow.ui \
    pipeline-editor/pipelinewidget.ui \
    smallwidgets/shotscreationdialog.ui

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
