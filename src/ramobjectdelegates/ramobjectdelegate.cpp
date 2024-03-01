#include "ramobjectdelegate.h"

#include <QTextFrame>

#include "duqf-widgets/duicon.h"
#include "duqf-utils/colorutils.h"
#include "duqf-utils/utils.h"

PaintParameters RamObjectDelegate::getPaintParameters(const QStyleOptionViewItem &option, const QModelIndex &index) const
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
    params.textColor = m_lessLight;
    params.detailsColor = m_medium;
    params.bgColor = m_transparent;
    bool mustBeDark = false;

    RamObject::ObjectType type = static_cast<RamObject::ObjectType>(
                index.data(RamObject::Type).toInt()
                );
    if (index.data(RamObject::Disabled).toBool())
    {
        mustBeDark = true;
        params.textColor = m_dark;
        params.detailsColor = m_dark;
    }
    else if (type == RamObject::State)
    {
        params.textColor = index.data(Qt::ForegroundRole).value<QBrush>().color();
        params.detailsColor = m_medium;
        if (m_comboBox) params.textColor = QColor(150,150,150);
    }
    else
    {
        params.textColor = index.data(Qt::ForegroundRole).value<QBrush>().color();
    }


    if (params.textColor.lightness() < 150 && !mustBeDark) params.textColor.setHsl( params.textColor.hue(), params.textColor.saturation(), 150);

    if (option.state & QStyle::State_MouseOver)
    {
        params.bgColor = params.bgColor.lighter(120);
        params.bgColor.setAlpha(255);
        params.detailsColor = params.detailsColor.lighter(120);
        params.textColor = params.textColor.lighter(120);
    }
    // Selected
    if (option.state & QStyle::State_Selected)
    {
        params.bgColor = params.bgColor.darker();
        params.bgColor.setAlpha(255);
    }

    return params;
}

void RamObjectDelegate::paintBG(const QModelIndex &index, QPainter *painter, PaintParameters *params) const
{
    painter->setRenderHint(QPainter::Antialiasing);

    // Background
    QPainterPath path;
    path.addRoundedRect(params->bgRect, 5, 5);
    painter->fillPath(path, QBrush(params->bgColor));

    qreal priority = index.data(RamAbstractObject::Priority).toFloat();

    if (priority > 0.5) {
        QPen p(priorityColor(priority));
        p.setWidth(1);
        painter->setPen(p);
        painter->drawPath(path);
    }

    // Too Small !
    if (params->bgRect.height() < 26 )
    {
        drawMore(painter, params->bgRect, QPen(params->textColor));
    }
}

void RamObjectDelegate::paintTitle(const QModelIndex &index, QPainter *painter, PaintParameters *params) const
{
    // Draw title
    if (index.data(RamObject::Disabled).toBool()) painter->setPen(params->textColor);
    else painter->setPen( index.data(Qt::ForegroundRole).value<QBrush>().color() );

    painter->setFont( m_textFont );
    QRect result;
    painter->drawText(
                params->titleRect,
                Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                index.data(Qt::DisplayRole).toString(),
                &result);
    params->titleRect = result;
    params->detailsRect = QRect(
                params->iconRect.right() + 5,
                params->titleRect.bottom() + 5,
                params->bgRect.width() - params->iconRect.width() - 35,
                params->bgRect.height() - params->titleRect.height() - 15
                );
}

void RamObjectDelegate::paintPublished(const QModelIndex &index, QPainter *painter, PaintParameters *params) const
{
    if (!index.data(RamAbstractObject::Published).toBool()) return;

    QRect r(
            params->iconRect.right() + m_padding,
            params->bgRect.top() + 7,
            12, 12
            );
    painter->drawPixmap( r, m_icons[":/icons/state-l"] );
    params->titleRect.moveLeft( r.right() + m_padding );
}

void RamObjectDelegate::paintButtons(QPainter *painter, PaintParameters *params, const QModelIndex &index) const
{
    if(m_comboBox) return;

    RamObject::ObjectType type = static_cast<RamObject::ObjectType>(
                index.data(RamObject::Type).toInt()
                );

    int xpos = params->bgRect.right() - 20;

    // Status is specific
    if (type == RamObject::Status)
    {
        //QRect historyButtonRect( xpos, params->bgRect.top() +7, 12, 12);
        //xpos -= 22;

        bool statusNo = index.data(RamObject::Disabled).toBool();

        //if (statusNo)
        //    drawButton(painter, historyButtonRect, m_historyDarkIcon, m_historyButtonHover == index);
        //else
        //    drawButton(painter, historyButtonRect, m_historyIcon, m_historyButtonHover == index);

        // Draw Folder button
        // Folder button
        if ( index.data(RamObject::Path).toString() != "" )
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
    if ( index.data(RamObject::Path).toString() != "" )
    {
        const QRect folderButtonRect( xpos, params->bgRect.top() +7, 12, 12 );
        xpos -= 22;
        drawButton(painter, folderButtonRect, m_folderIcon, m_folderButtonHover == index);
    }
}

void RamObjectDelegate::paintDetails(const QModelIndex &index, QPainter *painter, PaintParameters *params) const
{
    if (params->detailsRect.height() <= 15) return;

    painter->setPen( QPen(params->detailsColor) );

    QRect rect(params->detailsRect);
    int textHeight = drawMarkdown(painter, rect, index.data(RamObject::Details).toString());
    if (rect.height() < textHeight) drawMore(painter, params->bgRect, QPen(m_lessLight));
    else params->detailsRect.setHeight(textHeight);
}

void RamObjectDelegate::paintProgress(const QModelIndex &index, QPainter *painter, PaintParameters *params) const
{
    RamObject::ObjectType type = static_cast<RamObject::ObjectType>( index.data(RamObject::Type).toInt() );

    // State and status have a progress bar
    if (type == RamObject::State)
    {
        // Draw a progress bar
        QColor statusColor = index.data(RamObject::ProgressColor).value<QColor>();
        QBrush statusBrush(statusColor.darker());
        QPainterPath path;
        QRect statusRect( params->bgRect.left() + 5, params->titleRect.bottom() + 5, params->bgRect.width() - 10, 5 );
        if (statusRect.bottom() + 5 < params->bgRect.bottom())
        {
            path.addRoundedRect(statusRect, 5, 5);
            painter->fillPath(path, statusBrush);
            statusBrush.setColor(statusColor);
            statusRect.adjust(0,0, -statusRect.width() * (1-( index.data(RamObject::Completion).toInt() / 100.0)), 0);
            QPainterPath completionPath;
            completionPath.addRoundedRect(statusRect, 5, 5);
            painter->fillPath(completionPath, statusBrush);

            //details
            params->detailsRect.moveTop(statusRect.bottom() + 5);
            params->detailsRect.setHeight( params->bgRect.bottom() - statusRect.bottom() - 10);
        }
    }
    else if (type == RamObject::Status)
    {
        // Draw a progress bar
        QColor statusColor = index.data(RamObject::ProgressColor).value<QColor>();
        if (!m_completionRatio) statusColor = QColor(150,150,150);
        QBrush statusBrush(statusColor.darker(300));
        int statusWidth = params->bgRect.width() - m_padding;
        QRect statusRect( params->bgRect.left() + 5, params->titleRect.bottom() + 5, statusWidth, 6 );

        // Values to be reused
        int completionWidth = 0;

        if (statusRect.bottom() + 5 < params->bgRect.bottom() && (m_timeTracking || m_completionRatio))
        {
            // Status BG
            QPainterPath path;
            path.addRoundedRect(statusRect, 5, 5);
            painter->fillPath(path, statusBrush);

            if (m_completionRatio)
            {
                // Now draw the completion bar
                statusBrush.setColor( statusColor );

                completionWidth = statusWidth * ( index.data(RamObject::Completion).toInt() / 100.0 );
                statusRect.setWidth(completionWidth);
                QPainterPath completionPath;
                completionPath.addRoundedRect(statusRect, 5, 5);
                painter->fillPath(completionPath, statusBrush);
            }

            params->detailsRect.moveTop(statusRect.bottom() + 5);
            params->detailsRect.setHeight( params->bgRect.bottom() - statusRect.bottom() - 10);
        }
        return;
    }
}

QColor RamObjectDelegate::priorityColor(qreal priority)
{
    if (priority < .75) {
        qreal v = Interpolations::linear(priority, .5, .75, 0, 1);
        QColor c(250, 237, 95);
        c.setAlpha(v*256);
        return c;
    }
    else if (priority < 1) {
        qreal v = Interpolations::linear(priority, .75, 1, 0, 1);

        return ColorUtils::interpolateRGB(
            QColor(250, 237, 95),
            QColor(250, 160, 36),
            v );
    }
    else if (priority < 2) {
        qreal v = Interpolations::linear(priority, 1, 2, 0, 1);

        return ColorUtils::interpolateRGB(
            QColor(250, 160, 36),
            QColor(250, 20, 20),
            v );
    }
    else {
        qreal v = Interpolations::linear(priority, 2, 3, 0, 1);

        return ColorUtils::interpolateRGB(
            QColor(250, 20, 20),
            QColor(250, 20, 230),
            v );
    }
    return QColor();
}

RamObjectDelegate::RamObjectDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    m_abyss = QColor(28,28,28);
    m_dark = QColor(51,51,51);
    m_medium = QColor(109,109,109);
    m_lessLight = QColor(227,227,227);
    m_light = QColor(227,227,227);
    m_transparent = QColor(51,51,51);
    m_transparent.setAlpha(0);
    m_textFont = qApp->font();
    m_textFont.setPixelSize( 12 );
    m_padding = 10;

    m_editIcon = DuIcon(":/icons/edit").pixmap(QSize(12,12));
    m_historyIcon = DuIcon(":/icons/list").pixmap(QSize(12,12));
    m_folderIcon = DuIcon(":/icons/folder").pixmap(QSize(12,12));
    m_historyDarkIcon = m_historyIcon;
    m_folderDarkIcon = m_folderIcon;

    // The icons
    m_icons[":/icons/asset"] = DuIcon(":/icons/asset").pixmap(QSize(12,12));
    m_icons[":/icons/application"] = DuIcon(":/icons/application").pixmap(QSize(12,12));
    m_icons[":/icons/asset-group"] = DuIcon(":/icons/asset-group").pixmap(QSize(12,12));
    m_icons[":/icons/file"] = DuIcon(":/icons/file").pixmap(QSize(12,12));
    m_icons[":/icons/connection"] = DuIcon(":/icons/connection").pixmap(QSize(12,12));
    m_icons[":/icons/project"] = DuIcon(":/icons/project").pixmap(QSize(12,12));
    m_icons[":/icons/calendar"] = DuIcon(":/icons/calendar").pixmap(QSize(12,12));
    m_icons[":/icons/sequence"] = DuIcon(":/icons/sequence").pixmap(QSize(12,12));
    m_icons[":/icons/shot"] = DuIcon(":/icons/shot").pixmap(QSize(12,12));
    m_icons[":/icons/state-l"] = DuIcon(":/icons/state-l").pixmap(QSize(12,12));
    m_icons[":/icons/status"] = DuIcon(":/icons/status").pixmap(QSize(12,12));
    m_icons[":/icons/film"] = DuIcon(":/icons/film").pixmap(QSize(12,12));
    m_icons[":/icons/step"] = DuIcon(":/icons/step").pixmap(QSize(12,12));
    m_icons[":/icons/admin"] = DuIcon(":/icons/admin").pixmap(QSize(12,12));
    m_icons[":/icons/project-admin"] = DuIcon(":/icons/project-admin").pixmap(QSize(12,12));
    m_icons[":/icons/lead"] = DuIcon(":/DuIcon/lead").pixmap(QSize(12,12));
    m_icons[":/icons/user"] = DuIcon(":/icons/user").pixmap(QSize(12,12));

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
    QString uuid = index.data(RamObject::UUID).toString();
    if (uuid == "") return QStyledItemDelegate::paint(painter, option, index);

    bool disabled = index.data(RamObject::Disabled).toBool();

    // Base
    PaintParameters params = getPaintParameters(option, index);

    // BG
    paintBG(index, painter, &params);

    // no more room, finished
    if (params.bgRect.height() < 26 ) return;

    // Icon
    if (!index.data(RamObject::Disabled).toBool())
    {
        QPixmap icon = index.data(Qt::DecorationRole).value<QPixmap>();
        QColor labelColor = index.data(RamObject::LabelColor).value<QColor>();
        if (labelColor.isValid())
        {
            // Color the icon
            QImage iconImage(12,12, QImage::Format_ARGB32);
            iconImage.fill( labelColor );
            QPainter iconPainter(&iconImage);
            iconPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            iconPainter.drawPixmap( QRect(0,0,12,12), icon );
            painter->drawImage(params.iconRect, iconImage);
        }
        else painter->drawPixmap( params.iconRect, icon );
    }

    // Published
    paintPublished(index, painter, &params);

    // Title
    paintTitle(index, painter, &params);

    // Draw buttons
    paintButtons(painter, &params, index);

    // Don' draw any detail when NO
    if (disabled) return;

    // State and Status have a progress bar
    paintProgress(index, painter, &params);

    // Draw details
    paintDetails(index, painter, &params);

    // Draw Comment
    QRect commentRect(params.detailsRect.left(), params.detailsRect.bottom() + 3, params.bgRect.width() - 30, params.bgRect.height() - params.detailsRect.height() - params.titleRect.height() - 15);
    QString comment = index.data(RamObject::Comment).toString();
    if (commentRect.height() > 10 && comment != "")
    {
        painter->setPen( QPen(m_lessLight) );
        int textHeight = drawMarkdown(painter, commentRect, comment);
        if (commentRect.height() < textHeight) drawMore(painter, params.bgRect, QPen(m_lessLight));
        else commentRect.setHeight(textHeight);
    }
    else commentRect.setHeight(0);

    // Draw image preview
    QString previewImagePath = index.data(RamObject::PreviewImagePath).toString();
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
    /*QRect subDetailsRect( params.iconRect.left() + 5, imageRect.bottom() + 5, params.bgRect.width() - 30, params.bgRect.bottom() - imageRect.bottom() - 5);
    QString subDetails = index.data(RamObject::SubDetails).toString();
    if (commentRect.bottom() + 20 < params.bgRect.bottom() && subDetails != "")
    {
        painter->setPen( params.detailsColor );
        painter->setFont( m_detailsFont );
        painter->drawText( subDetailsRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, subDetails, &subDetailsRect);
        if (subDetailsRect.bottom() + 5 > params.bgRect.bottom()) drawMore(painter, params.bgRect, commentPen);
    }*/
//*/
}

QSize RamObjectDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    QSize s;
    if (m_details) s = index.data(RamObject::DetailedSizeHint).toSize();
    else s = index.data(RamObject::SizeHint).toSize();

    RamObject::ObjectType type = static_cast<RamObject::ObjectType>( index.data(RamObject::Type).toInt() );

    // Add room for the progressbar
    if ((type == RamObject::State || type == RamObject::Status) && (m_timeTracking || m_completionRatio))
    {
        s.setHeight( s.height() + 12);
    }

    if (s.isEmpty()) s= QSize(200,30);
    return s;
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

    RamObject::ObjectType ramtype = static_cast<RamObject::ObjectType>( index.data(RamObject::Type).toInt() );
    // Get the object to edit it
    QString uuid = index.data(RamObject::UUID).toString();
    if (uuid == "") return QStyledItemDelegate::editorEvent( event, model, option, index );
    RamObject *o = RamObject::get(uuid, ramtype);
    if (!o) return QStyledItemDelegate::editorEvent( event, model, option, index );

    //bool isStatus = ramtype == RamObject::Status;
    bool folder = index.data(RamObject::Path).toString() != "";

    int xpos = bgRect.right() - 22;

    //const QRect historyButtonRect( xpos, bgRect.top() +7, 20, 20 );
    //if (isStatus) xpos -= 22;

    const QRect folderButtonRect( xpos, bgRect.top() +7, 20, 20 );

    switch ( event->type() )
    {
    case QEvent::MouseButtonPress:
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );

        if (e->button() != Qt::LeftButton) return QStyledItemDelegate::editorEvent( event, model, option, index );

        //if ( historyButtonRect.contains(e->pos()) && isStatus)
        //{
        //    m_historyButtonPressed = true;
        //    return true;
        //}

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

        //if (historyButtonRect.contains(e->pos()) && isStatus)
        //{
        //    m_historyButtonHover = index;
        //    return true;
        //}
        /*else*/ if (m_historyButtonHover.isValid())
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
            /*if (historyButtonRect.contains(e->pos()) && isStatus)
            {
                // get the step history
                RamStatus *status = qobject_cast<RamStatus*>(o);
                RamStep *step = status->step();
                RamAbstractItem *item = status->item();
                item->editHistory(step);
            }*/
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
                /*if (isStatus)
                {
                    RamStatus *status = RamStatus::c( o );

                    // If it's not the current user, create a new one
                    RamUser *currentUser = Ramses::instance()->currentUser();
                    if(!status->user()->is(currentUser))
                        status = RamStatus::copy( status, currentUser );

                    status->edit();
                }
                else
                {*/
                    // Get the object to edit it
                    o->edit();
                //}
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

int RamObjectDelegate::drawMarkdown(QPainter *painter, QRect rect, const QString &md) const
{
    painter->save();
    painter->setFont( m_textFont );

    QTextDocument td;
    td.setIndentWidth(20);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    td.setPlainText(md);
#else
    td.setMarkdown(md);
#endif
    td.setTextWidth(rect.width());

    QRect clipRect(rect);
    clipRect.moveTo(0,0);
    painter->translate(rect.topLeft());

    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette.setColor(QPalette::Text, painter->pen().color());
    painter->setClipRect(clipRect);
    ctx.clip = clipRect;
    td.documentLayout()->draw(painter, ctx);

    painter->restore();

    return td.size().height();
}
