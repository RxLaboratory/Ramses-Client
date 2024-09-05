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
    RamJsonObjectEditWidget(QWidget *parent = nullptr);
    virtual QJsonObject data() const;
    virtual void setData(const QJsonObject &obj);

public slots:
    void emitDataChanged();

signals:
    void dataChanged(const QJsonObject &);

protected:
    DuTabWidget *ui_tabWidget;
    RamObjectPropertiesWidget *ui_propertiesWidget;
    DuRichTextEdit *ui_customSettingsEdit;

    void setRamObjectData(const QJsonObject &obj);
    QJsonObject ramObjectData() const;

private:
    void setupUi();
    void connectEvents();
};

#endif // RAMJSONOBJECTEDITWIDGET_H
