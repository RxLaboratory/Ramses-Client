#include "ramobjectdelegate.h"

RamObjectDelegate::RamObjectDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    m_abyss = DuUI::getColor("abyss-grey");
    m_dark = DuUI::getColor("dark-grey");
    m_medium = DuUI::getColor("medium-grey");
    m_lessLight = DuUI::getColor("less-light-grey");
    m_light = DuUI::getColor("light-grey");
    m_textFont = qApp->font();
    m_textFont.setPixelSize( DuUI::getSize("font", "size-medium"));
    m_textOption.setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    m_detailsFont = m_textFont;
    m_detailsFont.setItalic(true);
    m_detailsOption.setAlignment( Qt::AlignLeft | Qt::AlignTop );
    m_padding = 10;
}

void RamObjectDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Reinterpret the int to a pointer
    quintptr iptr = index.data(Qt::UserRole).toULongLong();
    RamObject *obj = reinterpret_cast<RamObject*>(iptr);

    // Base Settings
    const QRect rect = option.rect;
    painter->setRenderHint(QPainter::Antialiasing);

    // Layout (Rects)
    // bg
    const QRect bgRect = rect.adjusted(m_padding,2,-m_padding,-2);
    // icon
    const QRect iconRect( bgRect.left() + 10, bgRect.top() +7 , 12, 12 );
    // title
    const QRect titleRect( bgRect.left() + 27, bgRect.top(), bgRect.width() - 32, 26 );
    // Details
    const QRect detailsRect( iconRect.left() + 5, titleRect.bottom() + 3, iconRect.width() + titleRect.width() - 5, 60 );

    // State
    QBrush bgBrush(m_dark);
    QPen textPen(m_lessLight);
    QPen detailsPen(m_medium);
    if (option.state & QStyle::State_Selected)
    {
        bgBrush.setColor(m_abyss);
    }
    else if (option.state & QStyle::State_MouseOver)
    {
        bgBrush.setColor(m_medium);
        detailsPen.setColor(m_dark);
    }

    // Background
    QPainterPath path;
    path.addRoundedRect(bgRect, 5, 5);
    painter->fillPath(path, bgBrush);

    // Too Small !
    if (bgRect.height() < 26 )
    {
        drawMore(painter, bgRect, textPen);
        return;
    }

    // Title
    painter->setPen( textPen );
    painter->setFont(m_textFont);
    painter->drawText( titleRect, obj->name(), m_textOption);

    // Type Specific Drawing
    RamObject::ObjectType ramType = obj->objectType();
    switch(ramType)
    {
    case RamObject::User:
    {
        RamUser *user = qobject_cast<RamUser*>( obj );

        // icon
        QString icon = ":/icons/user";
        QString roleStr = "Standard user";
        if (user->role() == RamUser::Admin)
        {
            icon = ":/icons/admin";
            roleStr = "Administrator";
        }
        else if (user->role() == RamUser::ProjectAdmin)
        {
            icon = ":/icons/project-admin";
            roleStr = "Project administrator";
        }
        else if (user->role() == RamUser::Lead)
        {
            icon = ":/icons/lead";
            roleStr = "Lead";
        }
        painter->drawPixmap( iconRect, QIcon(icon).pixmap(QSize(12,12)));

        // role
        if (bgRect.height() >= 46 )
        {
            painter->setPen( detailsPen );
            painter->setFont( m_detailsFont );
            painter->drawText( detailsRect, roleStr, m_detailsOption);
        }

        break;
    }
    default:
        painter->drawPixmap( iconRect, QIcon(":/icons/asset").pixmap(QSize(12,12)));
    }



}

QSize RamObjectDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(32,30);
}

void RamObjectDelegate::drawMore(QPainter *painter, QRect rect, QPen pen) const
{
    painter->save();
    painter->setPen( pen );
    painter->setFont(m_textFont);
    painter->drawText(
                QRect( rect.left(), rect.top(), rect.width() - 5, rect.height() - 5),
                "...",
                QTextOption(Qt::AlignRight | Qt::AlignBottom));
    painter->restore();
}
