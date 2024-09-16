QT       += core gui \
            sql \
            network \
            svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += svgwidgets

# On linux, it's more standard to use lower case names
unix:!macx { TARGET = ramses }
else { TARGET = Ramses }

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

INCLUDEPATH += \
    dumodules

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
    docks/consoledock.cpp \
    dumodules/dugraph/dugraph.cpp \
    docks/settingsdock.cpp \
    dumodules/duapp/app-utils.cpp \
    dumodules/duapp/ducli.cpp \
    dumodules/duapp/dulogger.cpp \
    dumodules/duapp/dusettings.cpp \
    dumodules/duapp/dustyle.cpp \
    dumodules/duapp/duui.cpp \
    dumodules/duutils/ducolorutils.cpp \
    dumodules/duutils/dusystemutils.cpp \
    dumodules/duutils/stringutils.cpp \
    dumodules/duwidgets/duaction.cpp \
    dumodules/duwidgets/ducolorselector.cpp \
    dumodules/duwidgets/ducombobox.cpp \
    dumodules/duwidgets/duconsolewidget.cpp \
    dumodules/duwidgets/dudocktitlewidget.cpp \
    dumodules/duwidgets/dudockwidget.cpp \
    dumodules/duwidgets/dufolderselectorwidget.cpp \
    dumodules/duwidgets/duicon.cpp \
    dumodules/duwidgets/dulabel.cpp \
    dumodules/duwidgets/dulineedit.cpp \
    dumodules/duwidgets/dulisteditview.cpp \
    dumodules/duwidgets/dulistview.cpp \
    dumodules/duwidgets/dulistwidget.cpp \
    dumodules/duwidgets/dulogindialog.cpp \
    dumodules/duwidgets/dumainwindow.cpp \
    dumodules/duwidgets/dumenu.cpp \
    dumodules/duwidgets/dumenuview.cpp \
    dumodules/duwidgets/dupassworddialog.cpp \
    dumodules/duwidgets/durichtextedit.cpp \
    dumodules/duwidgets/duscrollarea.cpp \
    dumodules/duwidgets/duscrollbar.cpp \
    dumodules/duwidgets/dushadow.cpp \
    dumodules/duwidgets/duspinbox.cpp \
    dumodules/duwidgets/dusvgiconengine.cpp \
    dumodules/duwidgets/dutableview.cpp \
    dumodules/duwidgets/dutabwidget.cpp \
    dumodules/duwidgets/dutextedit.cpp \
    dumodules/duwidgets/dutoolbarspacer.cpp \
    dumodules/duwidgets/dutreewidget.cpp \
    dumodules/duwidgets/duwaiticonwidget.cpp \
    dumodules/duwidgets/settingswidget.cpp \
    pages/homepage.cpp \
    pages/landingpage.cpp \
    progressmanager.cpp \
    ramdatainterface/ramserverclient.cpp \
    rameditwidgets/objectupdateblocker.cpp \
    rameditwidgets/ramjsonobjecteditwidget.cpp \
    rameditwidgets/ramjsonstepeditwidget.cpp \
    rameditwidgets/ramjsonusereditwidget.cpp \
    rameditwidgets/ramobjectpropertieswidget.cpp \
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
    ramobjectmodels/ramjsonobjectmodel.cpp \
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
    docks/filemanagerwidget.cpp \
    dumodules/duutils/qtransposeproxymodel.cpp \
    dumodules/duutils/simplecrypt.cpp \
    dumodules/duwidgets/duqfautosizetoolbutton.cpp \
    dumodules/duwidgets/duqfelidedlabel.cpp \
    dumodules/duwidgets/duqffilelist.cpp \
    dumodules/duwidgets/duqfsearchedit.cpp \
    dumodules/duwidgets/duqfservercombobox.cpp \
    dumodules/duwidgets/duqfsslcheckbox.cpp \
    dumodules/duwidgets/duqftitlebar.cpp \
    dumodules/duwidgets/duqfupdatedialog.cpp \
    ramdatainterface/datacrypto.cpp \
    ramdatainterface/localdatainterface.cpp \
    rameditwidgets/applicationeditwidget.cpp \
    rammanagerwidgets/applicationmanagerwidget.cpp \
    rameditwidgets/asseteditwidget.cpp \
    rameditwidgets/assetgroupeditwidget.cpp \
    daemon.cpp \
    rammanagerwidgets/assetgroupmanagerwidget.cpp \
    rammanagerwidgets/assetmanagerwidget.cpp \
    rammanagerwidgets/filetypemanagerwidget.cpp \
    rameditwidgets/pipefileeditwidget.cpp \
    rammanagerwidgets/pipefilemanagerwidget.cpp \
    rammanagerwidgets/projectmanagerwidget.cpp \
    rammanagerwidgets/schedulemanagerwidget.cpp \
    ramobjects/ramabstractitem.cpp \
    ramobjectviews/ramobjectview.cpp \
    ramsettings.cpp \
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
    dumodules/dunodeview/duqfconnection.cpp \
    dumodules/dunodeview/duqfconnectionmanager.cpp \
    dumodules/dunodeview/duqfconnector.cpp \
    dumodules/dunodeview/duqfgrid.cpp \
    dumodules/dunodeview/duqfnode.cpp \
    dumodules/dunodeview/duqfnodescene.cpp \
    dumodules/dunodeview/duqfnodeview.cpp \
    dumodules/dunodeview/duqfslot.cpp \
    dumodules/duutils/guiutils.cpp \
    dumodules/duutils/language-utils.cpp \
    dumodules/duutils/utils.cpp \
    dumodules/duwidgets/aboutdialog.cpp \
    dumodules/duwidgets/autoselectdoublespinbox.cpp \
    dumodules/duwidgets/autoselectlineedit.cpp \
    dumodules/duwidgets/autoselectspinbox.cpp \
    dumodules/duwidgets/duqfdoubleslider.cpp \
    dumodules/duwidgets/duqfdoublespinbox.cpp \
    dumodules/duwidgets/duqffolderdisplaywidget.cpp \
    dumodules/duwidgets/duqflistwidget.cpp \
    dumodules/duwidgets/duqfslider.cpp \
    dumodules/duwidgets/duqfspinbox.cpp \
    rameditwidgets/filetypeeditwidget.cpp \
    smallwidgets/frameratewidget.cpp \
    main.cpp \
    mainwindow.cpp \
    rameditwidgets/objecteditwidget.cpp \
    rameditwidgets/pipeeditwidget.cpp \
    rameditwidgets/projecteditwidget.cpp \
    smallwidgets/progresswidget.cpp \
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
    rameditwidgets/templateassetgroupeditwidget.cpp \
    rameditwidgets/templatestepeditwidget.cpp \
    rameditwidgets/usereditwidget.cpp \
    statemanager.cpp \
    timelinemanager.cpp \
    rammanagerwidgets/itemmanagerwidget.cpp \
    pipeline-editor/objectnode.cpp \
    pipeline-editor/pipelinewidget.cpp \
    pipeline-editor/stepnode.cpp \
    wizards/jointeamprojectwizard.cpp \
    wizards/loginwizardpage.cpp \
    wizards/projectlistwizardpage.cpp \
    wizards/projectwizard.cpp \
    wizards/ramdatabasepathswizardpage.cpp \
    wizards/ramobjectpropertieswizardpage.cpp \
    wizards/ramuserswizardpage.cpp

HEADERS += \
    docks/consoledock.h \
    dumodules/dugraph/dugraph.h \
    config.h \
    docks/settingsdock.h \
    dumodules/duapp/app-utils.h \
    dumodules/duapp/ducli.h \
    dumodules/duapp/dulogger.h \
    dumodules/duapp/dusettings.h \
    dumodules/duapp/dustyle.h \
    dumodules/duapp/duui.h \
    dumodules/duutils/colorutils.h \
    dumodules/duutils/ducolorutils.h \
    dumodules/duutils/dusystemutils.h \
    dumodules/duutils/stringutils.h \
    dumodules/duwidgets/duaction.h \
    dumodules/duwidgets/ducolorselector.h \
    dumodules/duwidgets/ducombobox.h \
    dumodules/duwidgets/duconsolewidget.h \
    dumodules/duwidgets/dudocktitlewidget.h \
    dumodules/duwidgets/dudockwidget.h \
    dumodules/duwidgets/dufolderselectorwidget.h \
    dumodules/duwidgets/duicon.h \
    dumodules/duwidgets/dulabel.h \
    dumodules/duwidgets/dulineedit.h \
    dumodules/duwidgets/dulisteditview.h \
    dumodules/duwidgets/dulistview.h \
    dumodules/duwidgets/dulistwidget.h \
    dumodules/duwidgets/dulogindialog.h \
    dumodules/duwidgets/dumainwindow.h \
    dumodules/duwidgets/dumenu.h \
    dumodules/duwidgets/dumenuview.h \
    dumodules/duwidgets/dupassworddialog.h \
    dumodules/duwidgets/durichtextedit.h \
    dumodules/duwidgets/duscrollarea.h \
    dumodules/duwidgets/duscrollbar.h \
    dumodules/duwidgets/dushadow.h \
    dumodules/duwidgets/duspinbox.h \
    dumodules/duwidgets/dusvgiconengine.h \
    dumodules/duwidgets/dutableview.h \
    dumodules/duwidgets/dutabwidget.h \
    dumodules/duwidgets/dutextedit.h \
    dumodules/duwidgets/dutoolbarspacer.h \
    dumodules/duwidgets/dutreewidget.h \
    dumodules/duwidgets/duwaiticonwidget.h \
    dumodules/duwidgets/settingswidget.h \
    enums.h \
    pages/homepage.h \
    pages/landingpage.h \
    progressmanager.h \
    ramdatainterface/datastruct.h \
    ramdatainterface/ramserverclient.h \
    rameditwidgets/objectupdateblocker.h \
    rameditwidgets/ramjsonobjecteditwidget.h \
    rameditwidgets/ramjsonstepeditwidget.h \
    rameditwidgets/ramjsonusereditwidget.h \
    rameditwidgets/ramobjectpropertieswidget.h \
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
    ramobjectmodels/ramjsonobjectmodel.h \
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
    docks/filemanagerwidget.h \
    dumodules/duapp/app-config.h \
    dumodules/duutils/qtransposeproxymodel.h \
    dumodules/duutils/simplecrypt.h \
    dumodules/duwidgets/duqfautosizetoolbutton.h \
    dumodules/duwidgets/duqfelidedlabel.h \
    dumodules/duwidgets/duqffilelist.h \
    dumodules/duwidgets/duqfsearchedit.h \
    dumodules/duwidgets/duqfservercombobox.h \
    dumodules/duwidgets/duqfsslcheckbox.h \
    dumodules/duwidgets/duqftitlebar.h \
    dumodules/duwidgets/duqfupdatedialog.h \
    ramdatainterface/localdatainterface.h \
    rameditwidgets/applicationeditwidget.h \
    rammanagerwidgets/applicationmanagerwidget.h \
    rameditwidgets/asseteditwidget.h \
    rameditwidgets/assetgroupeditwidget.h \
    daemon.h \
    rammanagerwidgets/assetgroupmanagerwidget.h \
    rammanagerwidgets/assetmanagerwidget.h \
    rammanagerwidgets/filetypemanagerwidget.h \
    rameditwidgets/pipefileeditwidget.h \
    rammanagerwidgets/pipefilemanagerwidget.h \
    rammanagerwidgets/projectmanagerwidget.h \
    rammanagerwidgets/schedulemanagerwidget.h \
    ramobjects/ramabstractitem.h \
    ramobjectviews/ramobjectview.h \
    ramsettings.h \
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
    dumodules/duapp/app-version.h \
    dumodules/dunodeview/duqfconnection.h \
    dumodules/dunodeview/duqfconnectionmanager.h \
    dumodules/dunodeview/duqfconnector.h \
    dumodules/dunodeview/duqfgrid.h \
    dumodules/dunodeview/duqfnode.h \
    dumodules/dunodeview/duqfnodescene.h \
    dumodules/dunodeview/duqfnodeview.h \
    dumodules/dunodeview/duqfslot.h \
    dumodules/duutils/guiutils.h \
    dumodules/duutils/language-utils.h \
    dumodules/duutils/utils.h \
    dumodules/duwidgets/aboutdialog.h \
    dumodules/duwidgets/autoselectdoublespinbox.h \
    dumodules/duwidgets/autoselectlineedit.h \
    dumodules/duwidgets/autoselectspinbox.h \
    dumodules/duwidgets/duqfdoubleslider.h \
    dumodules/duwidgets/duqfdoublespinbox.h \
    dumodules/duwidgets/duqffolderdisplaywidget.h \
    dumodules/duwidgets/duqflistwidget.h \
    dumodules/duwidgets/duqfslider.h \
    dumodules/duwidgets/duqfspinbox.h \
    rameditwidgets/filetypeeditwidget.h \
    smallwidgets/frameratewidget.h \
    mainwindow.h \
    rameditwidgets/objecteditwidget.h \
    rameditwidgets/pipeeditwidget.h \
    rameditwidgets/projecteditwidget.h \
    smallwidgets/progresswidget.h \
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
    rameditwidgets/templateassetgroupeditwidget.h \
    rameditwidgets/templatestepeditwidget.h \
    rameditwidgets/usereditwidget.h \
    statemanager.h \
    timelinemanager.h \
    rammanagerwidgets/itemmanagerwidget.h \
    pipeline-editor/objectnode.h \
    pipeline-editor/pipelinewidget.h \
    pipeline-editor/stepnode.h \
    wizards/jointeamprojectwizard.h \
    wizards/loginwizardpage.h \
    wizards/projectlistwizardpage.h \
    wizards/projectwizard.h \
    wizards/ramdatabasepathswizardpage.h \
    wizards/ramobjectpropertieswizardpage.h \
    wizards/ramuserswizardpage.h

FORMS += \
    dumodules/duwidgets/aboutdialog.ui \
    dumodules/duwidgets/duqffolderselectorwidget.ui \
    mainwindow.ui \
    pipeline-editor/pipelinewidget.ui \
    smallwidgets/shotscreationdialog.ui

RESOURCES += \
    resources/duresources.qrc \
    resources/resources.qrc

include($$PWD/../include/QGoodWindow/QGoodWindow/QGoodWindow.pri)
include($$PWD/../include/QGoodWindow/QGoodCentralWidget/QGoodCentralWidget.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# OS Specific configurations
win* {
    # Add version and other metadata
    DISTFILES += app.rc
    RC_FILE = app.rc
    !build_pass:touch($$RC_FILE, dumodules/duapp/app-version.h)
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
