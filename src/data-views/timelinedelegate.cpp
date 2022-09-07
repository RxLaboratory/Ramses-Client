#include "timelinedelegate.h"

#include "ramasset.h"
#include "ramassetgroup.h"
#include "ramproject.h"
#include "ramsequence.h"
#include "ramshot.h"

TimelineDelegate::TimelineDelegate(QObject *parent)
    : RamObjectDelegateOld{parent}
{

}

void TimelineDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    RamShot *shot = RamShot::c( RamObjectList::at(index) );
    if (!shot) return RamObjectDelegateOld::paint(painter, option, index);

    // Base Settings
    QRect rect = option.rect.adjusted(0,0,-3,0);
    painter->setRenderHint(QPainter::Antialiasing);

    // Select the bg Color
    QColor bgColor;
    RamSequence *seq = shot->sequence();
    if (seq) bgColor = seq->color();
    else bgColor = m_dark;
    QColor textColor = m_lessLight;
    QColor detailsColor = m_medium;

    // Adjust according to bg
    if (bgColor.lightness() > 100)
    {
        textColor = m_abyss;
        detailsColor = m_dark;
    }

    // State mouseover
    if (option.state & QStyle::State_MouseOver)
    {
        bgColor = bgColor.lighter(120);
        detailsColor = detailsColor.lighter(120);
        textColor = textColor.lighter(120);
    }

    // Selected
    if (option.state & QStyle::State_Selected)
    {
        bgColor = bgColor.darker();
        textColor = textColor.lighter(150);
    }

    QBrush bgBrush(bgColor);

    // Background
    QPainterPath path;
    path.addRect(rect);
    painter->fillPath(path, bgBrush);

    // Draw either the preview, either the text
    QString previewImagePath = shot->previewImagePath();
    if (previewImagePath != "")
    {
        QRect imageRect = rect;
        QPixmap pix(previewImagePath);
        float pixRatio = pix.width() / float(pix.height());
        // Adjust image rect height to fit ratio
        float rectRatio = rect.width() / float(rect.height());
        if (rectRatio < pixRatio)
            imageRect.setHeight( rect.width() / pixRatio );
        else
            imageRect.setWidth( rect.height() * pixRatio );
        // Center image
        imageRect.moveCenter(rect.center());

        painter->drawPixmap( imageRect, QPixmap(previewImagePath));
    }
    else
    {
        QPen textPen(textColor);
        QPen commentPen(textColor);
        QPen detailsPen(detailsColor);

        QRect titleRect( rect.left() + 5, rect.top() + 5, rect.width() - 32, rect.height() - 10 );
        painter->setPen( textPen );
        painter->setFont( m_textFont );
        painter->drawText( titleRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, shot->name(), &titleRect);

        // Get details
        if (titleRect.bottom() + 5 < rect.bottom())
        {
            QString details = "Duration: " %
                            QString::number(shot->duration(), 'f', 2) %
                            " s | " %
                            QString::number(shot->duration() * shot->project()->framerate(), 'f', 2) %
                            " f";
            // List assigned assets
            QMap<QString,QStringList> assets;
            for (int i = 0; i < shot->assets()->rowCount(); i++)
            {
                RamAsset *asset = shot->assetAt(i);
                QString agName = asset->assetGroup()->name();
                QStringList ag = assets.value( agName );
                ag << asset->shortName();
                assets[ agName ] = ag;
            }
            QMapIterator<QString,QStringList> i(assets);
            while(i.hasNext())
            {
                i.next();
                details = details % "\n" % i.key() % " ► " % i.value().join(", ");
            }

            // Draw details
            QRect detailsRect( titleRect.left() + 5, titleRect.bottom() + 3, rect.width() -5, rect.height() - titleRect.height() - 15 );
            if (detailsRect.height() > 15 )
            {
                painter->setPen( detailsPen );
                painter->setFont( m_detailsFont );
                painter->drawText( detailsRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, details, &detailsRect);
            }

            // Draw Comment
            QRect commentRect( titleRect.left() + 5, detailsRect.bottom() + 5, rect.width() - 5, rect.bottom() - detailsRect.bottom() - 5);
            if (detailsRect.bottom() + 20 < rect.bottom() && shot->comment() != "")
            {
                painter->setPen( commentPen );
                painter->setFont(m_textFont);
                painter->drawText( commentRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, shot->comment(), &commentRect);
            }
            else commentRect.setHeight(0);
        }
    }

    // Draw Folder button
    // Folder button
    if ( shot->path() != "" )
    {
         const QRect folderButtonRect( rect.right() - 20, rect.top() +7, 12, 12 );
         drawButton(painter, folderButtonRect, m_folderIcon, m_folderButtonHover == index);
    }
}

QSize TimelineDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    // Reinterpret the int to a pointer
    quintptr iptr = index.data(Qt::UserRole).toULongLong();

    if (iptr == 0) return QSize(250, 30);

    RamShot *shot = reinterpret_cast<RamShot*>(iptr);
    if (!shot) return QSize(250, 30);

    return QSize(shot->duration()*50, 30);
}
