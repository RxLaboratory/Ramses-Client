#include "duaction.h"

#include <QApplication>

DuAction::DuAction(QWidget *parent)
    : QAction(parent)
{
    init(parent);
}

DuAction::DuAction(const QString &text, QWidget *parent):
    QAction(text, parent)
{
    init(parent);
}

DuAction::DuAction(const QIcon &icon, const QString &text, QWidget *parent):
    QAction(icon, text, parent)
{
    init(parent);
}

void DuAction::setIcon(DuSVGIconEngine *engine)
{
    setIcon(DuIcon(engine));
}

void DuAction::setIcon(const QString &iconPath)
{
    setIcon(DuIcon(iconPath));
}

void DuAction::setIcon(const DuIcon &icon)
{
    QAction::setIcon(icon);
}

void DuAction::setToolTip(const QString &toolTip)
{
    m_tooltip = toolTip;
    updateToolTip();
}

void DuAction::setShortcut(const QKeySequence &shortcut)
{
    QAction::setShortcut(shortcut);
    updateToolTip();
}

void DuAction::setShortcuts(const QList<QKeySequence> &sc)
{
    QAction::setShortcuts(sc);
    updateToolTip();
}

void DuAction::init(QWidget *parent)
{
    this->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    parent->addAction(this);
}

void DuAction::updateToolTip()
{
    QStringList scsList;

    QList<QKeySequence> scs = shortcuts();
    foreach(QKeySequence ks, scs) {
        scsList << ks.toString(QKeySequence::NativeText);
    }
    QString sc = scsList.join("<br>");

    if (sc == "") {
        QAction::setToolTip(m_tooltip);
        return;
    }

    QColor shortcutTextColor = QApplication::palette().color(QPalette::ToolTipText);
    QString shortCutTextColorName;
    if (shortcutTextColor.value() == 0) {
        shortCutTextColorName = "gray";  // special handling for black because lighter() does not work there [QTBUG-9343].
    } else {
        int factor = (shortcutTextColor.value() < 128) ? 150 : 50;
        shortCutTextColorName = shortcutTextColor.lighter(factor).name();
    }

    QAction::setToolTip(
        QString( "%1<br><code style='color:%2; font-size:small'>%3</code>"
                ).arg(m_tooltip, shortCutTextColorName, sc)
        );
}

