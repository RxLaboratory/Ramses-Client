#include "ramobjectdelegate.h"

#include "ramstate.h"
#include "ramtemplateassetgroup.h"

#include "ramfiletype.h"
#include "ramitem.h"
#include "ramsequence.h"
#include "ramshot.h"
#include "ramstatus.h"
#include "rampipefile.h"
#include "ramses.h"
#include "duqf-app/app-style.h"

PaintParameters RamObjectDelegate::getPaintParameters(const QStyleOptionViewItem &option, RamObject *obj) const
{
    // Default
    PaintParameters params;

    // Layout
    params.bgRect = option.rect.adjusted(m_padding,2,-m_padding,-2);
    params.iconRect = QRect(
                params.bgRect.left() + m_padding,
                params.bgRect.top() + 7,
                12, 12
                );
    params.titleRect = QRect(
                params.bgRect.left() + 27,
                params.bgRect.top() + 5,
                params.bgRect.width() - 32,
                params.bgRect.height() - 10
                );

    // Colors
    if (obj)
    {
        switch (obj->objectType())
        {
        case RamObject::Status:
        {
            RamStatus *status = RamStatus::c(obj);
            if (status->isNoState())
            {
                params.bgColor = QColor(0,0,0,0);
                params.textColor = m_dark;
                params.detailsColor = m_dark;
            }
            break;
        }
        default:
        {
            params.bgColor = m_dark;
            params.textColor = m_lessLight;
            params.detailsColor = m_medium;
        }
        }
    }
    else
    {
        params.bgColor = m_dark;
        params.textColor = m_lessLight;
        params.detailsColor = m_medium;
    }

    if (option.state & QStyle::State_MouseOver)
    {
        params.bgColor = params.bgColor.lighter(120);
        params.detailsColor = params.detailsColor.lighter(120);
        params.textColor = params.textColor.lighter(120);
    }
    // Selected
    if (option.state & QStyle::State_Selected)
    {
        params.bgColor = params.bgColor.darker();
        params.textColor = params.textColor.lighter(150);
    }

    return params;
}

void RamObjectDelegate::paintBG(QPainter *painter, PaintParameters *params) const
{
    painter->setRenderHint(QPainter::Antialiasing);

    // Background
    QPainterPath path;
    path.addRoundedRect(params->bgRect, 5, 5);
    painter->fillPath(path, QBrush(params->bgColor));

    // Too Small !
    if (params->bgRect.height() < 26 )
    {
        drawMore(painter, params->bgRect, QPen(params->textColor));
    }
}

void RamObjectDelegate::paintTitle(RamObject *obj, QPainter *painter, PaintParameters *params) const
{
    // Title
    QString title;
    QColor color;

    switch (obj->objectType())
    {
    case RamObject::Status:
    {
        RamStatus *status = RamStatus::c(obj);
        title = status->item()->shortName() %
            " | " %
            status->step()->shortName() %
            " | " %
            status->state()->shortName() %
            " (v" %
            QString::number(status->version()) %
            ")";
        color = obj->color();
        break;
    }
    case RamObject::TemplateAssetGroup:
    {
        title = obj->name() % " [Template]";
        color = obj->color();
        break;
    }
    case RamObject::TemplateStep:
    {
        title = obj->name() % " [Template]";
        color = obj->color();
        break;
    }
    case RamObject::FileType:
    {
        title = obj->name() % " (." % obj->shortName() + ")";
        color = obj->color();
        break;
    }
    case RamObject::State:
    {

        if (m_comboBox)
        {
            title = obj->shortName();
            color = QColor(150,150,150);
        }
        else
        {
            title = obj->name();
            color = obj->color();
        }
        break;
    }
    case RamObject::PipeFile:
    {
        title = obj->shortName();
        RamPipeFile *pf = RamPipeFile::c(obj);
        RamFileType *ft = pf->fileType();
        if (ft) title = title % "." % ft->shortName();
        color = obj->color();
        break;
    }
    case RamObject::Shot:
    {
        RamShot *shot = RamShot::c(obj);
        title = shot->name();
        color = shot->sequence()->color();
        break;
    }
    default:
    {
        title = obj->name();
        color = obj->color();
    }
    }

    // Draw title
    if (color.lightness() < 150) color.setHsl( color.hue(), color.saturation(), 150);
    params->textColor = color;
    paintTitle(title, painter, params);
}

void RamObjectDelegate::paintTitle(QString title, QPainter *painter, PaintParameters *params) const
{
    // Draw title
    painter->setPen( QPen(params->textColor) );
    painter->setFont( m_textFont );
    QRect result;
    painter->drawText( params->titleRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, title, &result);
    params->titleRect = result;
    params->detailsRect = QRect(
                params->iconRect.left() + 5,
                params->titleRect.bottom() + 3,
                params->bgRect.width() - params->iconRect.width() -5,
                params->bgRect.height() - params->titleRect.height() - 15
                );
}

void RamObjectDelegate::paintButtons(RamObject *obj, QPainter *painter, PaintParameters *params, const QModelIndex &index) const
{
    if(m_comboBox) return;

    int xpos = params->bgRect.right() - 20;

    // Status is specific
    if (obj->objectType() == RamObject::Status)
    {
        QRect historyButtonRect( xpos, params->bgRect.top() +7, 12, 12);
        xpos -= 22;

        RamStatus *status = RamStatus::c(obj);
        bool statusNo = status->isNoState();

        if (statusNo)
            drawButton(painter, historyButtonRect, m_historyDarkIcon, m_historyButtonHover == index);
        else
            drawButton(painter, historyButtonRect, m_historyIcon, m_historyButtonHover == index);

        // Draw Folder button
        // Folder button
        if ( obj->path() != "" )
        {
            const QRect folderButtonRect( xpos, params->bgRect.top() +7, 12, 12 );
            if (statusNo)
                drawButton(painter, folderButtonRect, m_folderDarkIcon, m_folderButtonHover == index);
            else
                drawButton(painter, folderButtonRect, m_folderIcon, m_folderButtonHover == index);
        }

        return;
    }

    // Draw Folder button
    if ( obj->path() != "" )
    {
        const QRect folderButtonRect( xpos, params->bgRect.top() +7, 12, 12 );
        xpos -= 22;
        drawButton(painter, folderButtonRect, m_folderIcon, m_folderButtonHover == index);
    }
}

void RamObjectDelegate::paintDetails(QString details, QPainter *painter, PaintParameters *params) const
{
    if (params->detailsRect.height() <= 15) return;

    painter->setPen( QPen(params->detailsColor) );
    painter->setFont( m_detailsFont );
    QRect result;
    painter->drawText( params->detailsRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, details, &result);
    params->detailsRect = result;
    if (params->detailsRect.bottom() + 5 > params->bgRect.bottom()) drawMore(painter, params->bgRect, QPen(params->textColor));
}

void RamObjectDelegate::paintDetails(RamObject *obj, QPainter *painter, PaintParameters *params) const
{
    if (obj->objectType() == RamObject::Status)
    {
        // If no state, nothing else to draw
        if (RamStatus::c(obj)->isNoState()) return;
    }

    painter->drawPixmap( params->iconRect, QIcon(obj->icon()).pixmap(QSize(12,12)));

    // State and status have a progress bar
    if (obj->objectType() == RamObject::State)
    {
        RamState *state = RamState::c(obj);
        // Draw a progress bar
        QColor statusColor = state->color();
        QBrush statusBrush(statusColor.darker());
        QPainterPath path;
        QRect statusRect( params->bgRect.left() + 5, params->titleRect.bottom() + 5, params->bgRect.width() - 10, 5 );
        if (statusRect.bottom() + 5 < params->bgRect.bottom())
        {
            path.addRoundedRect(statusRect, 5, 5);
            painter->fillPath(path, statusBrush);
            statusBrush.setColor(statusColor);
            statusRect.adjust(0,0, -statusRect.width() * (1-(state->completionRatio() / 100.0)), 0);
            QPainterPath completionPath;
            completionPath.addRoundedRect(statusRect, 5, 5);
            painter->fillPath(completionPath, statusBrush);

            //details
            params->detailsRect.moveTop(statusRect.bottom() + 5);
            params->detailsRect.setHeight( params->bgRect.bottom() - statusRect.bottom() - 10);
        }
    }
    else if (obj->objectType() == RamObject::Status)
    {
        RamStatus *status = RamStatus::c(obj);
        // Draw a progress bar
        QColor statusColor = status->state()->color();
        if (!m_completionRatio) statusColor = QColor(150,150,150);
        QBrush statusBrush(statusColor.darker(300));
        int statusWidth = params->bgRect.width() - m_padding;
        QRect statusRect( params->bgRect.left() + 5, params->titleRect.bottom() + 5, statusWidth, 6 );

        // Values to be reused
        float estimation = 0;
        float timeSpentDays = 0;
        int completionWidth = 0;
        bool useAutoEstimation = true;

        if (statusRect.bottom() + 5 < params->bgRect.bottom() && (m_timeTracking || m_completionRatio))
        {

            // Draw a colored rect for the assigned user
            RamUser *user = status->assignedUser();
            if (user)
            {
                QRect userRect(params->titleRect.left() - 16, params->titleRect.top() + 4, 8, 8);
                QPainterPath path;
                path.addEllipse(userRect);
                painter->fillPath(path, QBrush(user->color()));
            }

            // Status BG
            QPainterPath path;
            path.addRoundedRect(statusRect, 5, 5);
            painter->fillPath(path, statusBrush);

            // Adjust color according to lateness
            float latenessRatio = status->latenessRatio();
            // Ratio
            useAutoEstimation = status->useAutoEstimation();
            if (useAutoEstimation) estimation = status->estimation();
            else estimation = status->goal();
            timeSpentDays = RamStatus::hoursToDays( status->timeSpent()/3600 );
            float ratio = 0;
            if (estimation > 0) ratio = timeSpentDays / estimation;

            QColor timeColor = statusColor;

            //If we're late, draw the timebar first
            if (latenessRatio > 1 && m_timeTracking)
            {


                if ( latenessRatio < 1.2 )
                {
                    int red = std::min( timeColor.red() + 50, 255 );
                    int green = std::min( timeColor.green() + 50, 255 );
                    int blue = std::max( timeColor.blue() -50, 0);
                    timeColor.setRed( red );
                    timeColor.setGreen( green );
                    timeColor.setBlue( blue );
                    timeColor = timeColor.darker(200);
                }
                // Very late, orange
                else if ( latenessRatio < 1.4 )
                {
                    int red = std::min( timeColor.red() + 150, 255 );
                    int green = std::min( timeColor.green() + 25, 255 );
                    int blue = std::max( timeColor.blue() - 100, 0);
                    timeColor.setRed( red );
                    timeColor.setGreen( green );
                    timeColor.setBlue( blue );
                    timeColor = timeColor.darker(200);
                }
                // Extreme, red
                else
                {
                    int red = std::min( timeColor.red() + 200, 255 );
                    int green = std::max( timeColor.green() - 150, 0 );
                    int blue = std::max( timeColor.blue() - 150, 0);
                    timeColor.setRed( red );
                    timeColor.setGreen( green );
                    timeColor.setBlue( blue );
                    timeColor = timeColor.darker(200);
                }
                statusBrush.setColor( timeColor );

                statusRect.setWidth( statusWidth * ratio );
                if (statusRect.right() > params->bgRect.right() - 5) statusRect.setRight( params->bgRect.right() - 5);
                QPainterPath timePath;
                timePath.addRoundedRect(statusRect, 3, 3);
                painter->fillPath(timePath, statusBrush);
            }

            if (m_completionRatio)
            {
                // Now draw the completion bar
                statusBrush.setColor( statusColor );

                completionWidth = statusWidth * ( status->completionRatio() / 100.0 );
                statusRect.setWidth(completionWidth);
                QPainterPath completionPath;
                completionPath.addRoundedRect(statusRect, 5, 5);
                painter->fillPath(completionPath, statusBrush);
            }

            // And draw the Time bar if we're early
            if (latenessRatio <= 1 && m_timeTracking)
            {
                // Adjust color according to lateness
                statusBrush.setColor( timeColor.darker(130) );

                statusRect.setWidth( statusWidth * ratio );
                QPainterPath timePath;
                timePath.addRoundedRect(statusRect, 3, 3);
                painter->fillPath(timePath, statusBrush);
            }

            params->detailsRect.moveTop(statusRect.bottom() + 5);
            params->detailsRect.setHeight( params->bgRect.bottom() - statusRect.bottom() - 10);

            paintDetails(obj->details(), painter, params);
        }
        return;
    }

    paintDetails(obj->details(), painter, params);
}

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
    m_detailsFont = m_textFont;
    m_detailsFont.setItalic(true);
    m_padding = 10;

    m_editIcon = QIcon(":/icons/edit").pixmap(QSize(12,12));
    m_historyIcon = QIcon(":/icons/list").pixmap(QSize(12,12));
    m_folderIcon = QIcon(":/icons/reveal-folder-s").pixmap(QSize(12,12));
    m_historyDarkIcon = m_historyIcon;
    m_folderDarkIcon = m_folderIcon;

    QPixmap darkMap = QPixmap(12,12);
    darkMap.fill(m_dark);

    QPainter historyPainter(&m_historyDarkIcon);
    historyPainter.setRenderHint(QPainter::Antialiasing);
    historyPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    historyPainter.drawPixmap(0,0,darkMap);

    QPainter folderPainter(&m_folderDarkIcon);
    folderPainter.setRenderHint(QPainter::Antialiasing);
    folderPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    folderPainter.drawPixmap(0,0,darkMap);
}

// Default

void RamObjectDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Base
    PaintParameters params = getPaintParameters(option);

    // BG
    paintBG(painter, &params);

    // no more room, finished
    if (params.bgRect.height() < 26 ) return;

    RamObject *obj = RamObjectList::at(index);

    // Title
    paintTitle(obj, painter, &params);

    // Draw buttons
    paintButtons(obj, painter, &params, index);


    // Draw Comment
    QRect commentRect( params.iconRect.left() + 5, params.detailsRect.bottom() + 5, params.bgRect.width() - 30, params.bgRect.bottom() - params.detailsRect.bottom() - 5);
    QPen commentPen(params.textColor);
    if (params.detailsRect.bottom() + 20 < params.bgRect.bottom() && obj->comment() != "")
    {
        painter->setPen( commentPen );
        painter->setFont(m_textFont);
        painter->drawText( commentRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, obj->comment(), &commentRect);
        if (commentRect.bottom() + 5 > params.bgRect.bottom()) drawMore(painter, params.bgRect, commentPen);
    }
    else commentRect.setHeight(0);

    // Draw image preview
    QString previewImagePath = obj->previewImagePath();
    QRect imageRect( params.iconRect.left() + 5, commentRect.bottom() + 5, params.bgRect.width() - 30, params.bgRect.bottom() - commentRect.bottom() - 5);
    if (commentRect.bottom() + 20 < params.bgRect.bottom() && previewImagePath != "")
    {
        QPixmap pix(previewImagePath);
        float pixRatio = pix.width() / float(pix.height());
        // Adjust image rect height to fit ratio
        float rectRatio = imageRect.width() / float(imageRect.height());
        if (rectRatio < pixRatio)
            imageRect.setHeight( imageRect.width() / pixRatio );
        else
            imageRect.setWidth( imageRect.height() * pixRatio );

        painter->drawPixmap( imageRect, pix);
    }
    else imageRect.setHeight(0);

    // Draw subdetails
    QRect subDetailsRect( params.iconRect.left() + 5, imageRect.bottom() + 5, params.bgRect.width() - 30, params.bgRect.bottom() - imageRect.bottom() - 5);
    QString subDetails = obj->subDetails();
    if (commentRect.bottom() + 20 < params.bgRect.bottom() && subDetails != "")
    {
        painter->setPen( params.detailsColor );
        painter->setFont( m_detailsFont );
        painter->drawText( subDetailsRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, subDetails, &subDetailsRect);
        if (subDetailsRect.bottom() + 5 > params.bgRect.bottom()) drawMore(painter, params.bgRect, commentPen);
    }
//*/
}

QSize RamObjectDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    RamObject *obj = RamObjectList::at(index);

    if (!obj) return QSize(200, 30);

    RamObject::ObjectType ramType = obj->objectType();

    int height = 30;
    int width = 200;

    if (m_details)
    {
        if (ramType == RamObject::Status || ramType == RamObject::Shot || ramType == RamObject::Asset) height = 300;
        else height = 60;
    }

    if ((ramType == RamObject::State || ramType == RamObject::Status) && (m_timeTracking || m_completionRatio))
    {
        height += 12;
        width = 300;
    }

    return QSize(width,height);//*/
}

void RamObjectDelegate::setEditable(bool editable)
{
    m_editable = editable;
}

void RamObjectDelegate::setEditRole(RamUser::UserRole role)
{
    m_editRole = role;
}

bool RamObjectDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    // Return asap if we don't manage the event
    QEvent::Type type = event->type();
    if (type != QEvent::QEvent::MouseButtonPress && type != QEvent::MouseButtonRelease && type != QEvent::MouseMove)
        return QStyledItemDelegate::editorEvent( event, model, option, index );


    const QRect bgRect = option.rect.adjusted(m_padding,2,-m_padding,-2);

    // The object
    quintptr iptr = index.data(Qt::UserRole).toULongLong();
    if (iptr == 0) return false;
    RamObject *o = reinterpret_cast<RamObject*>( iptr );

    bool isStatus = o->objectType() == RamObject::Status;
    bool folder = o->path() != "";

    int xpos = bgRect.right() - 22;

    const QRect historyButtonRect( xpos, bgRect.top() +7, 20, 20 );
    if (isStatus) xpos -= 22;

    const QRect folderButtonRect( xpos, bgRect.top() +7, 20, 20 );

    switch ( event->type() )
    {
    case QEvent::MouseButtonPress:
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );

        if (e->button() != Qt::LeftButton) return QStyledItemDelegate::editorEvent( event, model, option, index );

        if ( historyButtonRect.contains(e->pos()) && isStatus)
        {
            m_historyButtonPressed = true;
            return true;
        }

        if (folderButtonRect.contains(e->pos()) && folder)
        {
            m_folderButtonPressed = true;
            return true;
        }

        if (e->modifiers().testFlag(Qt::NoModifier)) m_cellPressed = true;

        break;
    }
    case QEvent::MouseMove:
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );

        if (historyButtonRect.contains(e->pos()) && isStatus)
        {
            m_historyButtonHover = index;
            return true;
        }
        else if (m_historyButtonHover.isValid())
        {
            m_historyButtonHover = QModelIndex();
            return true;
        }

        if (folderButtonRect.contains(e->pos()) && folder)
        {
            m_folderButtonHover = index;
            return true;
        }
        else if (m_folderButtonHover.isValid())
        {
            m_folderButtonHover = QModelIndex();
            return true;
        }

        return QStyledItemDelegate::editorEvent( event, model, option, index );
    }
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );

        if (m_historyButtonPressed)
        {
            if (historyButtonRect.contains(e->pos()) && isStatus)
            {
                // get the step history
                RamStatus *status = qobject_cast<RamStatus*>(o);
                RamStep *step = status->step();
                RamItem *item = status->item();
                item->statusHistory(step)->edit();
            }
            m_historyButtonPressed = false;
            m_historyButtonHover = QModelIndex();
            return true;
        }

        if (m_folderButtonPressed)
        {
            if (folderButtonRect.contains(e->pos()))
            {
                o->revealFolder();
            }
            m_folderButtonPressed = false;
            m_folderButtonHover = QModelIndex();
            return true;
        }

        if (m_cellPressed)
        {
            if (bgRect.contains(e->pos()) && e->modifiers().testFlag(Qt::NoModifier) && o)
            {
                // Check if it's a status
                if (isStatus)
                {
                    RamStatus *status = qobject_cast<RamStatus*>( o );

                    // If it's not the current user, create a new one
                    RamUser *currentUser = Ramses::instance()->currentUser();
                    if(!status->user()->is(currentUser))
                        status = RamStatus::copy( status, currentUser );

                    status->edit();
                }
                else
                    o->edit();
            }
            m_cellPressed = false;
            emit edited(o);
            return true;
        }

        break;
    }
    default:
        return QStyledItemDelegate::editorEvent( event, model, option, index );
    }

    return QStyledItemDelegate::editorEvent( event, model, option, index );

}

void RamObjectDelegate::setCompletionRatio(bool newCompletionRatio)
{
    m_completionRatio = newCompletionRatio;
}

void RamObjectDelegate::showDetails(bool s)
{
    m_details = s;
}

void RamObjectDelegate::setTimeTracking(bool newTimeTracking)
{
    m_timeTracking = newTimeTracking;
}

void RamObjectDelegate::setComboBoxMode(bool comboBoxMode)
{
    m_comboBox = comboBoxMode;
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

void RamObjectDelegate::drawButton(QPainter *painter, QRect rect, QPixmap icon, bool hover) const
{
    if (hover)
    {
        QPainterPath path;
        path.addRoundedRect(rect.adjusted(-5, -5, 5, 5), 3, 3);
        painter->fillPath(path, QBrush(m_dark));
    }
    painter->drawPixmap( rect, icon );
}
