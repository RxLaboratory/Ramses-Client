#ifndef RAMUSERWIDGET_H
#define RAMUSERWIDGET_H

#include "ramobjectwidget.h"
#include "usereditwidget.h"
#include "ramses.h"

/**
 * @brief The RamUserWidget class is a small widget used to show a User anywhere in the UI
 */
class RamUserWidget : public RamObjectWidget
{
public:
    RamUserWidget(RamUser *user, QWidget *parent = nullptr);
    RamUser *user() const;

private slots:
    void userChanged();

private:
    RamUser *_user;
};

#endif // RAMUSERWIDGET_H
