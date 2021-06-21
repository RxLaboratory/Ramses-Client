#include "ramobjectdelegate.h"

RamObjectDelegate::RamObjectDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    m_bgBrush = QBrush( DuUI::getColor("dark-grey") );
    m_selectedBgBrush = QBrush( DuUI::getColor("abyss-grey") );
    m_hoverBgBrush = QBrush( DuUI::getColor("medium-grey") );
    m_textPen.setColor( DuUI::getColor("less-light-grey") );
    m_hoverTextPen.setColor( DuUI::getColor("light-grey") );
    m_textFont = qApp->font();
    m_textFont.setPixelSize( DuUI::getSize("font", "size-medium"));
    m_textOption.setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    m_padding = 10;
}

void RamObjectDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Reinterpret the int to a pointer
    quintptr iptr = index.data(Qt::UserRole).toULongLong();
    RamObject *obj = reinterpret_cast<RamObject*>(iptr);

    const QRect rect = option.rect;

    // Background
    QPainterPath path;
    const QRect bgRect = rect.adjusted(m_padding,2,-m_padding,-2);
    path.addRoundedRect(bgRect, 5, 5);
    painter->setRenderHint(QPainter::Antialiasing);

    if (option.state & QStyle::State_Selected)
        painter->fillPath(path, m_selectedBgBrush);
    else if (option.state & QStyle::State_MouseOver)
        painter->fillPath(path, m_hoverBgBrush);
    else
        painter->fillPath(path, m_bgBrush);

    // Icon
    const QRect iconRect( rect.left() + m_padding + 10, rect.top() + (rect.height() - 12) / 2, 12, 12 );
    painter->drawPixmap( iconRect, QIcon(":/icons/step").pixmap(QSize(12,12)));

    // Text
    painter->setPen(m_textPen);
    painter->setFont(m_textFont);
    const QRect titleRect( rect.left() + m_padding + 27, rect.top(), rect.width() - 2*m_padding - 32, rect.height());
    painter->drawText( titleRect, obj->name(), m_textOption);

}

QSize RamObjectDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(32,30);
}
