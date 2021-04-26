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
    void setObject(RamObject *obj) Q_DECL_OVERRIDE;

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    void stateChanged(RamObject *o);
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
