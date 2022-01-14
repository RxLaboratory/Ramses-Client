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

    m_editIcon = QIcon(":/icons/edit").pixmap(QSize(12,12));
    m_historyIcon = QIcon(":/icons/list").pixmap(QSize(12,12));
    m_folderIcon = QIcon(":/icons/reveal-folder-s").pixmap(QSize(12,12));
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
    const QRect iconRect( bgRect.left() + m_padding, bgRect.top() +7 , 12, 12 );

    // Select the bg Color
    QColor bgColor = m_dark;
    QColor textColor = m_lessLight;
    QColor detailsColor = m_medium;

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
    else if (ramType == RamObject::Status)
    {
        RamStatus *status = qobject_cast<RamStatus*>( obj );
        RamState *noState = Ramses::instance()->noState();
        if (noState->is(status->state()))
        {
            bgColor = QColor(0,0,0,0);
        }
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
        QColor stepColor = step->color();
        if (stepColor.lightness() < 150) stepColor.setHsl( stepColor.hue(), stepColor.saturation(), 150);
        textPen.setColor( stepColor );
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
        RamState *noState = Ramses::instance()->noState();
        if (noState->is(status->state()))
        {
            textPen.setColor(m_dark);
        }
        else
        {
            textPen.setColor(m_medium);
        }
        break;
    }
    case RamObject::State:
    {
        RamState *state = qobject_cast<RamState*>( obj );
        if (m_comboBox) title = state->shortName();
        break;
    }
    case RamObject::User:
    {
        RamUser *user = qobject_cast<RamUser*>( obj );
        QColor userColor = user->color();
        if (userColor.lightness() < 150) userColor.setHsl( userColor.hue(), userColor.saturation(), 150);
        textPen.setColor( userColor );
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
        if (canEdit(index))
        {
            const QRect editButtonRect( xpos, bgRect.top() +7, 12, 12 );
            xpos -= 22;
            drawButton(painter, editButtonRect, m_editIcon, m_editButtonHover);
        }

        // Draw History button
        // History button
        if ( ramType == RamObject::Status )
        {
            QRect historyButtonRect( xpos, bgRect.top() +7, 12, 12);
            xpos -= 22;
            drawButton(painter, historyButtonRect, m_historyIcon, m_historyButtonHover);
        }


        // Draw Folder button
        // Folder button

        if ( obj->path() != "" )
        {
             const QRect folderButtonRect( xpos, bgRect.top() +7, 12, 12 );
             drawButton(painter, folderButtonRect, m_folderIcon, m_folderButtonHover);
        }
    }

    QString details;
    QString subDetails;
    QString previewImagePath;
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
        if (user->role() == RamUser::Admin)
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
        if (titleRect.bottom() + 5 < bgRect.bottom())
        {
            details = "Duration: " %
                            QString::number(shot->duration(), 'f', 2) %
                            " s | " %
                            QString::number(shot->duration() * shot->project()->framerate(), 'f', 2) %
                            " f";
            // List assigned assets
            if (shot->assets()->count() > 0)
            {
                QMap<QString,QStringList> assets;
                for (int i = 0; i < shot->assets()->count(); i++)
                {
                    RamAsset *asset = qobject_cast<RamAsset*>( shot->assets()->at(i) );
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
            }


        }
        // Preview
        if (titleRect.bottom() + 25 < bgRect.bottom())
            previewImagePath = shot->previewImagePath();
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

        // Preview
        if (titleRect.bottom() + 25 < bgRect.bottom())
            previewImagePath = asset->previewImagePath();
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

        // Draw a colored rect for the assigned user
        RamUser *user = status->assignedUser();
        if (user)
        {
            QRect userRect(titleRect.left() - 15, titleRect.top() + 3, 10, 10);
            QPainterPath path;
            path.addRect(userRect);
            painter->fillPath(path, QBrush(user->color()));
        }

        // If no state, nothing else to draw
        if (Ramses::instance()->noState()->is(status->state())) break;

        // Draw a progress bar
        QColor statusColor = status->state()->color();
        if (!m_completionRatio) statusColor = QColor(150,150,150);
        QBrush statusBrush(statusColor.darker(300));
        int statusWidth = bgRect.width() - m_padding;
        QRect statusRect( bgRect.left() + 5, titleRect.bottom() + 5, statusWidth, 6 );

        // Values to be reused
        float estimation = 0;
        float timeSpentDays = 0;
        int completionWidth = 0;
        bool useAutoEstimation = true;

        // details
        if (statusRect.bottom() + 5 < bgRect.bottom() && (m_timeTracking || m_completionRatio))
        {
            // Draw status

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
                if (statusRect.right() > bgRect.right() - 5) statusRect.setRight( bgRect.right() - 5);
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

        }
        if (statusRect.bottom() + 5 < bgRect.bottom())
        {
            // details
            RamUser *assignedUser = status->assignedUser();
            if (assignedUser) details = "Assigned to: " %
                    assignedUser->name() %
                    "\nDifficulty: ";
            else details = "Not assigned\nDifficulty: ";

            switch( status->difficulty() )
            {
            case RamStatus::VeryEasy: { details += "Very easy"; break; }
            case RamStatus::Easy: { details += "Easy"; break; }
            case RamStatus::Medium: { details += "Medium"; break; }
            case RamStatus::Hard: { details += "Hard"; break; }
            case RamStatus::VeryHard: { details += "Very hard"; break; }
            }

            qint64 timeSpentSecs = status->timeSpent();
            // Convert to hours
            int timeSpent = timeSpentSecs / 3600;
            // Estimation or goal
            if (timeSpent > 0)
            {
                details = details %
                    "\nTime spent: " %
                    QString::number(timeSpent) %
                    " hours (" %
                    QString::number(timeSpentDays, 'f', 0) %
                    " days) / " %
                    QString::number(estimation, 'f', 1) %
                    " days ";
                if (useAutoEstimation) details = details % "(estimated)";
                else details = details % "(goal)";
            }
            else if (useAutoEstimation) details = details %
                    "\nEstimation: " %
                    QString::number(estimation, 'f', 1) %
                    " days";
            else details = details %
                    "\nGoal: " %
                    QString::number(estimation, 'f', 1) %
                    " days";

            if (status->isPublished()) details = details % "\n► Published";

            detailsRect.moveTop(statusRect.bottom() + 5);
            detailsRect.setHeight( bgRect.bottom() - statusRect.bottom() - 10);
        }
        // subdetails
        if (statusRect.bottom() + 25 < bgRect.bottom())
        {
            //subdetails
            QString dateFormat = "yyyy-MM-dd hh:mm:ss";
            RamUser *user = Ramses::instance()->currentUser();
            if (user)
            {
                QSettings *uSettings = user->settings();
                dateFormat = uSettings->value("ramses/dateFormat", dateFormat).toString();
            }
            subDetails = "Modified on: " %
                    status->date().toString(dateFormat) %
                    "\nBy: " %
                    status->user()->name();

            previewImagePath = status->previewImagePath();
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
        if (detailsRect.bottom() + 5 > bgRect.bottom()) drawMore(painter, bgRect, commentPen);
    }

    // Draw Comment
    QRect commentRect( iconRect.left() + 5, detailsRect.bottom() + 5, bgRect.width() - 30, bgRect.bottom() - detailsRect.bottom() - 5);
    if (detailsRect.bottom() + 20 < bgRect.bottom() && obj->comment() != "")
    {
        painter->setPen( commentPen );
        painter->setFont(m_textFont);
        painter->drawText( commentRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, obj->comment(), &commentRect);
        if (commentRect.bottom() + 5 > bgRect.bottom()) drawMore(painter, bgRect, commentPen);
    }
    else commentRect.setHeight(0);

    // Draw image preview
    QRect imageRect( iconRect.left() + 5, commentRect.bottom() + 5, bgRect.width() - 30, bgRect.bottom() - commentRect.bottom() - 5);
    if (commentRect.bottom() + 20 < bgRect.bottom() && previewImagePath != "")
    {
        QPixmap pix(previewImagePath);
        float pixRatio = pix.width() / float(pix.height());
        // Adjust image rect height to fit ratio
        float rectRatio = imageRect.width() / float(imageRect.height());
        if (rectRatio < pixRatio)
            imageRect.setHeight( imageRect.width() / pixRatio );
        else
            imageRect.setWidth( imageRect.height() * pixRatio );

        painter->drawPixmap( imageRect, QPixmap(previewImagePath));
    }
    else imageRect.setHeight(0);

    // Draw subdetails
    QRect subDetailsRect( iconRect.left() + 5, imageRect.bottom() + 5, bgRect.width() - 30, bgRect.bottom() - imageRect.bottom() - 5);
    if (commentRect.bottom() + 20 < bgRect.bottom() && subDetails != "")
    {
        painter->setPen( detailsPen );
        painter->setFont( m_detailsFont );
        painter->drawText( subDetailsRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, subDetails, &subDetailsRect);
        if (subDetailsRect.bottom() + 5 > bgRect.bottom()) drawMore(painter, bgRect, commentPen);
    }
//*/
}

QSize RamObjectDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    // Reinterpret the int to a pointer
    quintptr iptr = index.data(Qt::UserRole).toULongLong();

    if (iptr == 0) return QSize(200, 30);

    RamObject *obj = reinterpret_cast<RamObject*>(iptr);
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

    bool edit = canEdit(index);
    bool history = o->objectType() == RamObject::Status;
    bool folder = o->path() != "";

    if (!edit && !history && !folder)
        return QStyledItemDelegate::editorEvent( event, model, option, index );

    int xpos = bgRect.right() - 22;

    const QRect editButtonRect = QRect( xpos, bgRect.top()+7, 20, 20 );
    if (edit) xpos -= 22;

    const QRect historyButtonRect( xpos, bgRect.top() +7, 20, 20 );
    if (history) xpos -= 22;

    const QRect folderButtonRect( xpos, bgRect.top() +7, 20, 20 );


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

        m_cellPressed = true;
        break;
    }
    case QEvent::MouseMove:
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );

        if (editButtonRect.contains(e->pos()) && edit)
            return m_editButtonHover = true;
        else if (m_editButtonHover)
            return  !(m_editButtonHover = false);

        if (historyButtonRect.contains(e->pos()) && history)
            return m_historyButtonHover = true;
        else if (m_historyButtonHover)
            return !(m_historyButtonHover = false);

        if (folderButtonRect.contains(e->pos()) && folder)
            return m_folderButtonHover = true;
        else if (m_folderButtonHover)
            return !(m_folderButtonHover = false);

        if (bgRect.contains(e->pos()))
            return m_cellHover = true;
        else if (m_cellHover)
            return !(m_cellHover = false);

        return QStyledItemDelegate::editorEvent( event, model, option, index );
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

        if (bgRect.contains(e->pos()))
        {
            emit editObject(o);
        }
        m_cellPressed = false;

        return true;
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

bool RamObjectDelegate::canEdit(const QModelIndex &index) const
{
    if (!m_editable) return false;

    RamUser *u = Ramses::instance()->currentUser();
    if (!u) return false;

    quintptr iptr = index.data(Qt::UserRole).toULongLong();
    if (iptr == 0) return false;
    RamObject *o = reinterpret_cast<RamObject*>( iptr );
    if (o->objectType() == RamObject::Status)
    {
        RamStatus *status = qobject_cast<RamStatus*>( o );
        if (!status) return false;
        if (status->assignedUser()) if(status->assignedUser()->is(u)) return true;
        if (u->role() > RamUser::Standard) return true;
        return false;
    }

    return u->role() >= m_editRole;
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
