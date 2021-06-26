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
    m_detailsFont = m_textFont;
    m_detailsFont.setItalic(true);
    m_padding = 10;
}

void RamObjectDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Reinterpret the int to a pointer
    quintptr iptr = index.data(Qt::UserRole).toULongLong();
    RamObject *obj = reinterpret_cast<RamObject*>(iptr);

    if (iptr == 0)
    {
        obj = new RamObject("", index.data(Qt::DisplayRole).toString());
    }


    RamObject::ObjectType ramType = obj->objectType();

    // Base Settings
    const QRect rect = option.rect;
    painter->setRenderHint(QPainter::Antialiasing);

    // Layout (Rects)
    // Constant rects
    // bg
    const QRect bgRect = rect.adjusted(m_padding,2,-m_padding,-2);
    // icon
    const QRect iconRect( bgRect.left() + 10, bgRect.top() +7 , 12, 12 );

    // Select the bg Color
    QColor bgColor = m_dark;
    QColor textColor = m_lessLight;
    QColor detailsColor = m_medium;

    // State
    if (option.state & QStyle::State_Selected)
    {
        bgColor = bgColor.darker();
    }
    else if (option.state & QStyle::State_MouseOver)
    {
        bgColor = bgColor.lighter();
        detailsColor = detailsColor.darker();
    }

    QBrush bgBrush(bgColor);
    QPen textPen(textColor);
    QPen commentPen(textColor);
    QPen detailsPen(detailsColor);

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
    QString title = obj->name();
    switch(ramType)
    {
    case RamObject::AssetGroup:
    {
        RamAssetGroup *ag = qobject_cast<RamAssetGroup*>( obj );
        // Title
        if (ag->isTemplate())
            title = title % " [Template]";
        break;
    }
    case RamObject::Step:
    {
        RamStep *step = qobject_cast<RamStep*>( obj );
        if (step->isTemplate())
            title = title % " [Template]";
        break;
    }
    case RamObject::FileType:
    {
        RamFileType *fileType = qobject_cast<RamFileType*>( obj );
        title = title % " (." % fileType->shortName() + ")";
        break;
    }
    case RamObject::PipeFile:
    {
        RamPipeFile *pf = qobject_cast<RamPipeFile*>( obj );
        RamFileType *ft = pf->fileType();
        title = pf->shortName();
        if (ft) title = title % "." % ft->shortName();
        break;
    }
    case RamObject::Status:
    {
        RamStatus *status = qobject_cast<RamStatus*>( obj );
        title = status->item()->shortName() %
                " | " %
                status->step()->shortName() %
                " | " %
                status->state()->shortName() %
                " (v" %
                QString::number(status->version()) %
                ")";
        textPen.setColor(m_medium);
        break;
    }
    case RamObject::State:
    {
        RamState *state = qobject_cast<RamState*>( obj );
        if (m_comboBox) title = state->shortName();
        break;
    }
    default:
    {
        title = obj->name();
    }
    }

    // Draw title
    QRect titleRect( bgRect.left() + 27, bgRect.top() + 5, bgRect.width() - 32, bgRect.height() - 10 );
    painter->setPen( textPen );
    painter->setFont( m_textFont );
    painter->drawText( titleRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, title, &titleRect);

    // Draw buttons
    if (!m_comboBox)
    {
        int xpos = bgRect.right() - 20;

        // Draw editbutton
        // Edit button
        if (canEdit() || index.column() > 0)
        {
            const QRect editButtonRect( xpos, bgRect.top() +7, 12, 12 );
            xpos -= 22;
            drawButton(painter, editButtonRect, ":/icons/edit", m_editButtonHover);
        }

        // Draw History button
        // History button
        if ( ramType == RamObject::Status )
        {
            QRect historyButtonRect( xpos, bgRect.top() +7, 12, 12);
            xpos -= 22;
            drawButton(painter, historyButtonRect, ":/icons/list", m_historyButtonHover);
        }


        // Draw Folder button
        // Folder button

        if ( obj->path() != "" )
        {
             const QRect folderButtonRect( xpos, bgRect.top() +7, 12, 12 );
             drawButton(painter, folderButtonRect, ":/icons/reveal-folder-s", m_folderButtonHover);
        }
    }


    QString details;
    QRect detailsRect( iconRect.left() + 5, titleRect.bottom() + 3, bgRect.width() - iconRect.width() -5, bgRect.height() - titleRect.height() - 15 );

    // Type Specific Drawing
    switch(ramType)
    {
    case RamObject::User:
    {
        RamUser *user = qobject_cast<RamUser*>( obj );

        // icon
        QString icon = ":/icons/user";
        details = "Standard user";
        if (user->role() == RamUser::AdminFolder)
        {
            icon = ":/icons/admin";
            details = "Administrator";
        }
        else if (user->role() == RamUser::ProjectAdmin)
        {
            icon = ":/icons/project-admin";
            details = "Project administrator";
        }
        else if (user->role() == RamUser::Lead)
        {
            icon = ":/icons/lead";
            details = "Lead";
        }
        painter->drawPixmap( iconRect, QIcon(icon).pixmap(QSize(12,12)));
        break;
    }
    case RamObject::Project:
    {
        RamProject *project = qobject_cast<RamProject*>( obj );
        // icon
        painter->drawPixmap( iconRect, QIcon(":/icons/project").pixmap(QSize(12,12)));
        // details
        details = QString::number(project->width()) %
                " x " %
                QString::number(project->height()) %
                " (" %
                QString::number(project->aspectRatio(),'f',2) %
                ":1)" %
                " @ " %
                QString::number(project->framerate(), 'f', 2) %
                "fps";
        break;
    }
    case RamObject::Step:
    {
        RamStep *step = qobject_cast<RamStep*>( obj );
        // icon
        QString icon = ":/icons/asset";
        details = "Asset production";
        if (step->type() == RamStep::PreProduction)
        {
            icon = ":/icons/project";
            details = "Pre-production";
        }
        else if (step->type() == RamStep::ShotProduction)
        {
            icon = ":/icons/shot";
            details = "Shot production";
        }
        else if (step->type() == RamStep::PostProduction)
        {
            icon = ":/icons/film";
            details = "Post-production";
        }
        painter->drawPixmap( iconRect, QIcon(icon).pixmap(QSize(12,12)));
        break;
    }
    case RamObject::AssetGroup:
    {
        RamAssetGroup *ag = qobject_cast<RamAssetGroup*>( obj );
        // icon
        painter->drawPixmap( iconRect, QIcon(":/icons/asset-group").pixmap(QSize(12,12)));
        if (!ag->isTemplate()) details = "Contains "  % QString::number(ag->assetCount()) % " assets";
        else detailsRect.setHeight(0);
        break;
    }
    case RamObject::FileType:
    {
        RamFileType *fileType = qobject_cast<RamFileType*>( obj );        // icon
        painter->drawPixmap( iconRect, QIcon(":/icons/file").pixmap(QSize(12,12)));
        details = "Extensions: " % fileType->extensions().join(", ");
        break;
    }
    case RamObject::Application:
    {
        //RamApplication *app = qobject_cast<RamApplication*>( obj );
        // icon
        painter->drawPixmap( iconRect, QIcon(":/icons/application").pixmap(QSize(12,12)));
        detailsRect.setHeight(0);
        break;
    }
    case RamObject::PipeFile:
    {
        RamPipeFile *pf = qobject_cast<RamPipeFile*>( obj );
        RamFileType *ft = pf->fileType();
        // icon
        painter->drawPixmap( iconRect, QIcon(":/icons/connection").pixmap(QSize(12,12)));
        if(ft) details = "File type: " % ft->name();
        else detailsRect.setHeight(0);
        break;
    }
    case RamObject::Sequence:
    {
        RamSequence *seq = qobject_cast<RamSequence*>( obj );
        // icon
        painter->drawPixmap( iconRect, QIcon(":/icons/sequence").pixmap(QSize(12,12)));
        QTime duration(0, 0, seq->duration());
        details = "Contains: " %
                QString::number(seq->shotCount()) %
                " shots\n" %
                "Duration: " %
                duration.toString("mm 'mn' ss 's'");
        break;
    }
    case RamObject::Shot:
    {
        RamShot *shot = qobject_cast<RamShot*>( obj );
        // icon
        painter->drawPixmap( iconRect, QIcon(":/icons/shot").pixmap(QSize(12,12)));
        details = "Duration: " %
                            QString::number(shot->duration(), 'f', 2) %
                            " s | " %
                            QString::number(shot->duration() * shot->project()->framerate(), 'f', 2) %
                            " f";
        break;
    }
    case RamObject::Asset:
    {
        RamAsset *asset = qobject_cast<RamAsset*>( obj );
        // icon
        painter->drawPixmap( iconRect, QIcon(":/icons/asset").pixmap(QSize(12,12)));
        details = asset->assetGroup()->name() %
                            "\n" %
                            asset->tags().join(", ");
        break;
    }
    case RamObject::State:
    {
        RamState *state = qobject_cast<RamState*>( obj );
        // icon
        painter->drawPixmap( iconRect, QIcon(":/icons/state").pixmap(QSize(12,12)));
        // Draw a progress bar
        QColor statusColor = state->color();
        QBrush statusBrush(statusColor.darker());
        QPainterPath path;
        QRect statusRect( bgRect.left() + 5, titleRect.bottom() + 5, bgRect.width() - 10, 5 );
        if (statusRect.bottom() + 5 < bgRect.bottom())
        {
            path.addRoundedRect(statusRect, 5, 5);
            painter->fillPath(path, statusBrush);
            statusBrush.setColor(statusColor);
            statusRect.adjust(0,0, -statusRect.width() * (1-(state->completionRatio() / 100.0)), 0);
            QPainterPath completionPath;
            completionPath.addRoundedRect(statusRect, 5, 5);
            painter->fillPath(completionPath, statusBrush);

            //details
            details = "Completion ratio: " % QString::number(state->completionRatio()) % "%";
            detailsRect.moveTop(statusRect.bottom() + 5);
            detailsRect.setHeight( bgRect.bottom() - statusRect.bottom() - 10);
        }

        break;
    }
    case RamObject::Status:
    {
        RamStatus *status = qobject_cast<RamStatus*>( obj );
        // Draw a progress bar
        QColor statusColor = status->state()->color();
        QBrush statusBrush(statusColor.darker());
        QPainterPath path;
        QRect statusRect( bgRect.left() + 5, titleRect.bottom() + 5, bgRect.width() - 10, 5 );
        if (statusRect.bottom() + 5 < bgRect.bottom())
        {
            path.addRoundedRect(statusRect, 5, 5);
            painter->fillPath(path, statusBrush);
            statusBrush.setColor(statusColor);
            statusRect.adjust(0,0, -statusRect.width() * (1-(status->completionRatio() / 100.0)), 0);
            QPainterPath completionPath;
            completionPath.addRoundedRect(statusRect, 5, 5);
            painter->fillPath(completionPath, statusBrush);

            //details
            QString dateFormat = "yyyy-MM-dd hh:mm:ss";
            RamUser *user = Ramses::instance()->currentUser();
            if (user)
            {
                QSettings *uSettings = user->userSettings();
                dateFormat = uSettings->value("ramses/dateFormat", dateFormat).toString();
            }
            details = status->date().toString(dateFormat) %
                    " | " %
                    status->user()->name();

            RamUser *assignedUser = status->assignedUser();
            if (assignedUser) details = details %
                    "\nAssigned to: " %
                    assignedUser->name();
            else details = details %
                    "\nNot assigned";

            if (status->isPublished()) details = details % "\n→ Published";

            detailsRect.moveTop(statusRect.bottom() + 5);
            detailsRect.setHeight( bgRect.bottom() - statusRect.bottom() - 10);
        }

        break;
    }
    default:
        detailsRect.setHeight(0);
    }

    // Draw details
    if (detailsRect.height() > 15 )
    {
        painter->setPen( detailsPen );
        painter->setFont( m_detailsFont );
        painter->drawText( detailsRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, details, &detailsRect);
        if (detailsRect.bottom() - 5 > bgRect.bottom()) drawMore(painter, bgRect, commentPen);
    }

    // Draw Comment
    if (detailsRect.bottom() + 20 < bgRect.bottom() && obj->comment() != "")
    {
        QRect commentRect( iconRect.left() + 5, detailsRect.bottom() + 5, bgRect.width() - 30, bgRect.bottom() - detailsRect.bottom() - 5);
        painter->setPen( commentPen );
        painter->setFont(m_textFont);
        painter->drawText( commentRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, obj->comment(), &commentRect);
        if (commentRect.bottom() - 5 > bgRect.bottom()) drawMore(painter, bgRect, commentPen);
    }

}

QSize RamObjectDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    // Reinterpret the int to a pointer
    quintptr iptr = index.data(Qt::UserRole).toULongLong();

    if (iptr == 0) return QSize(200, 30);

    RamObject *obj = reinterpret_cast<RamObject*>(iptr);
    RamObject::ObjectType ramType = obj->objectType();

    if (ramType == RamObject::State || ramType == RamObject::Status) return QSize(300, 42);

    return QSize(200,30);
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

    bool edit = canEdit();
    bool history = o->objectType() == RamObject::Status;
    bool folder = o->path() != "";

    if (!edit && !history && !folder)
        return QStyledItemDelegate::editorEvent( event, model, option, index );

    int xpos = bgRect.right() - 22;

    const QRect editButtonRect = QRect( xpos, bgRect.top()+7, 22, 22 );
    if (edit) xpos -= 22;

    const QRect historyButtonRect( xpos, bgRect.top() +7, 22, 22 );
    if (history) xpos -= 22;

    const QRect folderButtonRect( xpos, bgRect.top() +7, 22, 22 );


    switch ( event->type() )
    {
    case QEvent::MouseButtonPress:
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );
        if (editButtonRect.contains(e->pos()) && edit)
        {
            m_editButtonPressed = true;
            return true;
        }

        if ( historyButtonRect.contains(e->pos()) && history)
        {
            m_historyButtonPressed = true;
            return true;
        }

        if (folderButtonRect.contains(e->pos()) && folder)
        {
            m_folderButtonPressed = true;
            return true;
        }
        break;
    }
    case QEvent::MouseMove:
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );

        if (editButtonRect.contains(e->pos()) && edit)
            m_editButtonHover = true;
        else if (m_editButtonHover)
            m_editButtonHover = false;

        if (historyButtonRect.contains(e->pos()) && history)
            m_historyButtonHover = true;
        else if (m_historyButtonHover)
            m_historyButtonHover = false;

        if (folderButtonRect.contains(e->pos()) && folder)
            m_folderButtonHover = true;
        else if (m_folderButtonHover)
            m_folderButtonHover = false;

        return true;
    }
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );

        if (m_editButtonPressed)
        {
            if (editButtonRect.contains(e->pos()))
            {
                emit editObject(o);
            }
            m_editButtonPressed = false;
            return true;
        }

        if (m_historyButtonPressed)
        {
            if (historyButtonRect.contains(e->pos()))
            {
                emit historyObject(o);
            }
            m_historyButtonPressed = false;
            return true;
        }

        if (m_folderButtonPressed)
        {
            if (folderButtonRect.contains(e->pos()))
            {
                emit folderObject(o);
            }
            m_folderButtonPressed = false;
            return true;
        }

        break;
    }
    default:
        return QStyledItemDelegate::editorEvent( event, model, option, index );
    }

    return QStyledItemDelegate::editorEvent( event, model, option, index );

}

void RamObjectDelegate::setComboBoxMode(bool comboBoxMode)
{
    m_comboBox = comboBoxMode;
}

bool RamObjectDelegate::canEdit() const
{
    RamUser *u = Ramses::instance()->currentUser();
    return m_editable && u->role() >= m_editRole;
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

void RamObjectDelegate::drawButton(QPainter *painter, QRect rect, QString iconPath, bool hover) const
{
    if (hover)
    {
        QPainterPath path;
        path.addRoundedRect(rect.adjusted(-5, -5, 5, 5), 3, 3);
        painter->fillPath(path, QBrush(m_dark));
    }
    painter->drawPixmap( rect, QIcon(iconPath).pixmap(QSize(12,12)));
}
