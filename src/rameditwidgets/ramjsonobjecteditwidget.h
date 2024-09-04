#ifndef RAMJSONOBJECTEDITWIDGET_H
#define RAMJSONOBJECTEDITWIDGET_H

#include <QBoxLayout>
#include <QFormLayout>

#include "duwidgets/ducolorselector.h"
#include "duwidgets/dulineedit.h"
#include "duwidgets/duscrollarea.h"
#include "duwidgets/dutabwidget.h"
#include "duwidgets/durichtextedit.h"

class RamJsonObjectEditWidget : public DuScrollArea
{
    Q_OBJECT
public:
    RamJsonObjectEditWidget(QWidget *parent = nullptr);
    virtual QJsonObject data() const;
    virtual void setData(const QJsonObject &obj);

signals:
    void dataChanged(const QJsonObject &);

protected:
    DuTabWidget *ui_tabWidget;
    QBoxLayout *ui_mainLayout;
    QFormLayout *ui_attributesLayout;
    DuLineEdit *ui_shortNameEdit;
    DuLineEdit *ui_nameEdit;
    DuRichTextEdit *ui_commentEdit;
    DuColorSelector *ui_colorSelector;

    void setRamObjectData(const QJsonObject &obj);
    QJsonObject ramObjectData() const;

private:
    void setupUi();
    void connectEvents();
};

#endif // RAMJSONOBJECTEDITWIDGET_H
