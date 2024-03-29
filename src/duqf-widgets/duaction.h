#ifndef DUACTION_H
#define DUACTION_H

#include "duqf-widgets/duicon.h"
#include "duqf-widgets/dusvgiconengine.h"
#include <QAction>

class DuAction : public QAction
{
public:
    explicit DuAction(QWidget *parent);
    explicit DuAction(const QString &text, QWidget *parent);
    explicit DuAction(const QIcon &icon, const QString &text, QWidget *parent);

    void setIcon(DuSVGIconEngine *engine);
    void setIcon(const QString &iconPath);
    void setIcon(const DuIcon &icon);

    void setToolTip(const QString &toolTip);
    void setShortcut(const QKeySequence &shortcut);
    void setShortcuts(const QList<QKeySequence> &sc);

private:
    void init(QWidget *parent);
    void updateToolTip();

    QString m_tooltip;
};

#endif // DUACTION_H
