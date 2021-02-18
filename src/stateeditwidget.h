#ifndef STATEEDITWIDGET_H
#define STATEEDITWIDGET_H

#include <QColorDialog>

#include "ui_stateeditwidget.h"
#include "ramses.h"
#include "duqf-widgets/duqfspinbox.h"

class StateEditWidget : public QWidget, private Ui::StateEditWidget
{
    Q_OBJECT

public:
    explicit StateEditWidget(QWidget *parent = nullptr);

    RamState *state() const;
    void setState(RamState *state);

private slots:
    void update();
    void revert();
    bool checkInput();
    void stateDestroyed(QObject *o);
    void dbiLog(QString m, LogUtils::LogType t);
    void updateColorEditStyle();
    void selectColor();

private:
    DuQFSpinBox *completionSpinBox;
    RamState *_state;
    QMetaObject::Connection _currentStateConnection;

};

#endif // STATEEDITWIDGET_H
