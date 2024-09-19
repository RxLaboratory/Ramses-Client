#ifndef RAMJSONOBJECTEDITWIDGET_H
#define RAMJSONOBJECTEDITWIDGET_H

#include <QBoxLayout>
#include <QFormLayout>
#include <QJsonObject>


#include "duwidgets/duscrollarea.h"
#include "duwidgets/dutabwidget.h"
#include "duwidgets/durichtextedit.h"
#include "ramobjectpropertieswidget.h"

class RamJsonObjectEditWidget : public DuScrollArea
{
    Q_OBJECT
public:
    RamJsonObjectEditWidget(const QString &uuid = "", QWidget *parent = nullptr);
    virtual QJsonObject data() const;
    virtual void setData(const QJsonObject &obj);
    bool exists() const { return _uuid != ""; }
    QString uuid() const { return _uuid; }

public slots:
    void emitEdited();

signals:
    /**
     * @brief dataChanged Emitted every time the data is changed
     */
    void dataChanged(const QJsonObject &);
    /**
     * @brief dataEdited emitted only if the user changed the data,
     * but not programatically
     */
    void dataEdited(const QJsonObject &);
    /**
     * @brief folderOpenRequested The user wants to create/open the corresponding folder
     */
    void folderOpenRequested();

protected:
    DuTabWidget *ui_tabWidget;
    RamObjectPropertiesWidget *ui_propertiesWidget;
    DuRichTextEdit *ui_customSettingsEdit;

    void setRamObjectData(const QJsonObject &obj);
    QJsonObject ramObjectData() const;

private:
    void setupUi();
    void connectEvents();


    QString _uuid;
};

#endif // RAMJSONOBJECTEDITWIDGET_H
