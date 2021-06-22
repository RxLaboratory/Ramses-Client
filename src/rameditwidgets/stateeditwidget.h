#ifndef STATEEDITWIDGET_H
#define STATEEDITWIDGET_H

#include <QColorDialog>

#include "ramses.h"
#include "objecteditwidget.h"
#include "duqf-widgets/duqfspinbox.h"

class StateEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit StateEditWidget(RamState *state, QWidget *parent = nullptr);
    explicit StateEditWidget(QWidget *parent = nullptr);

    RamState *state() const;

public slots:
    void setObject(RamObject *obj) override;

protected slots:
    void update() override;

private slots:
    void updateColorEditStyle();
    void selectColor();

private:
    RamState *_state;

    void setupUi();
    void connectEvents();

    DuQFSpinBox *completionSpinBox;
    QLineEdit *colorEdit;
    QToolButton *colorButton;
    QMetaObject::Connection _currentStateConnection;

};

#endif // STATEEDITWIDGET_H
