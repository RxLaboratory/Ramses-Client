#include "dustyle.h"

#include <QPainter>
#include <QStyleOption>

#include "duapp/dusettings.h"

void DuStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (!painter->isActive()) return;
    switch (element) {
    case QStyle::CE_RubberBand:
    {
        painter->save();
        QColor c = DuSettings::i()->get(DuSettings::UI_FocusColor).value<QColor>();
        QColor bgc = c;
        bgc.setAlpha(85);
        QBrush b(bgc);
        QPen p(c);
        p.setStyle(Qt::DashLine);
        int w = 2;
        p.setWidth(w);
        painter->setPen(p);
        painter->setBrush(b);
        QRect r = option->rect.marginsRemoved(QMargins(w/2,w/2,w/2,w/2));
        painter->drawRoundedRect(r, 7, 7);
        painter->restore();
        break;
    }
    default:
        QProxyStyle::drawControl(element, option, painter, widget);
    }
}

int DuStyle::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
    switch(hint) {
    case QStyle::SH_RubberBand_Mask:
        return 0;
    case QStyle::SH_EtchDisabledText:
        return 0;
    default:
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
}
