#include "ramstatisticsdelegate.h"

#include "ramstatus.h"

RamStatisticsDelegate::RamStatisticsDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    m_abyss = DuUI::getColor("abyss-grey");
    m_dark = DuUI::getColor("dark-grey");
    m_medium = DuUI::getColor("medium-grey");
    m_lessLight = DuUI::getColor("less-light-grey");
    m_light = DuUI::getColor("light-grey");
    m_textFont = qApp->font();
    m_textFont.setPixelSize( DuUI::getSize("font", "size-medium"));
    m_detailsFont = m_textFont;
    m_detailsFont.setItalic(true);
    m_padding = 10;

}

void RamStatisticsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    int col = index.column();

    if (col != 0) return QStyledItemDelegate::paint(painter, option, index);

    // Base Settings
    const QRect rect = option.rect;
    painter->setRenderHint(QPainter::Antialiasing);

    // bg
    const QRect bgRect = rect.adjusted(m_padding,2,-m_padding,-2);

    // Select the bg Color
    QColor bgColor = m_dark;
    QColor textColor = m_lessLight;
    QColor detailsColor = m_medium;

    if (option.state & QStyle::State_MouseOver)
    {
        bgColor = bgColor.lighter();
        detailsColor = detailsColor.darker();
    }

    QBrush bgBrush(bgColor);
    QPen textPen(textColor);
    QPen detailsPen(detailsColor);

    // Background
    QPainterPath path;
    path.addRoundedRect(bgRect, 5, 5);
    painter->fillPath(path, bgBrush);

    // Completion bar
    QBrush statusBrush( m_abyss );
    int statusWidth = bgRect.width() - m_padding ;
    QRect statusRect( bgRect.left() + m_padding / 2, bgRect.top() + m_padding, statusWidth, 5 );
    QPainterPath statusPath;
    statusPath.addRoundedRect(statusRect, 5, 5);
    painter->fillPath(statusPath, statusBrush);

    if (m_timeTracking || m_completionRatio)
    {

        float latenessRatio = index.data(Qt::UserRole +1).toFloat();
        float estimation = index.data(Qt::UserRole + 2).toFloat();
        qint64 timeSpent = index.data(Qt::UserRole + 3).toLongLong();

        // Draw a timebar first
        if (m_timeTracking && latenessRatio > 0 && estimation > 0)
        {
            QColor timeColor;
            if ( latenessRatio < 1.1 ) timeColor = QColor(32,62,32);
            else if ( latenessRatio < 1.2 ) timeColor = QColor(76,30,0);
            else if ( latenessRatio < 1.3 ) timeColor = QColor(93,31,0);
            else if ( latenessRatio < 1.4 ) timeColor = QColor(118,27,0);
            else if ( latenessRatio < 1.5 ) timeColor = QColor(140,23,0);
            else if ( latenessRatio < 1.6 ) timeColor = QColor(168,14,0);
            else if ( latenessRatio < 1.7 ) timeColor = QColor(188,0,0);
            else if ( latenessRatio < 1.8 ) timeColor = QColor(214,0,0);
            else if ( latenessRatio < 1.9 ) timeColor = QColor(236,0,0);
            else timeColor = QColor(255,0,0);
            statusBrush.setColor( timeColor );

            float timeSpentDays = RamStatus::hoursToDays( timeSpent/3600 );
            float ratio = timeSpentDays / estimation;

            statusRect.setWidth( statusWidth * ratio );
            if (statusRect.right() > bgRect.right() - 10) statusRect.setRight( bgRect.right() - 10);
            QPainterPath timePath;
            timePath.addRoundedRect(statusRect, 3, 3);
            painter->fillPath(timePath, statusBrush);
        }

        if (m_completionRatio)
        {
            // Set a color according to the completion
            float completionRatio = index.data(Qt::UserRole ).toInt() / 100.0;
            QColor completionColor;
            if (completionRatio < 0.12) completionColor = QColor( 197, 0, 0);
            else if (completionRatio < 0.25) completionColor = QColor( 197, 98, 17);
            else if (completionRatio < 0.5) completionColor = QColor( 197, 179, 40);
            else if (completionRatio < 0.75) completionColor = QColor( 128, 197, 37);
            else if (completionRatio < 0.88) completionColor = QColor( 100, 172, 69);
            else if (completionRatio < 0.98) completionColor = QColor( 55, 172, 23);
            else completionColor = QColor( 6, 116, 24);

            // Draw completion ratio
            statusBrush.setColor( completionColor );
            statusRect.setWidth(statusWidth * completionRatio);
            QPainterPath completionPath;
            completionPath.addRoundedRect(statusRect, 5, 5);
            painter->fillPath(completionPath, statusBrush);
        }

    }

    // Too Small !
    if (bgRect.height() < 35 ) return;

    // Details
    QRect detailsRect(statusRect.left(), statusRect.bottom() + 5, statusWidth, bgRect.height() - 30);
    painter->setPen(textPen);
    painter->setFont(m_detailsFont);
    painter->drawText(detailsRect, Qt::AlignLeft | Qt::AlignTop, index.data(Qt::DisplayRole).toString(), &detailsRect);
    if (detailsRect.bottom() > bgRect.bottom() - 5)
    {
        drawMore(painter, bgRect, textPen);
    }
}

QSize RamStatisticsDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 0) return QSize(200,105);
    return QStyledItemDelegate::sizeHint(option, index);
}

void RamStatisticsDelegate::drawMore(QPainter *painter, QRect rect, QPen pen) const
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
