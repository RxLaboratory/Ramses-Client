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
    RamObject::ObjectType ramType = obj->objectType();

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
    // Edit button
    const QRect editButtonRect( bgRect.right() - 20, bgRect.top() +7, 12, 12 );

    // Select the bg Color (which is different for ramstates)
    QColor bgColor = m_dark;
    QColor textColor = m_lessLight;
    QColor detailsColor = m_medium;
    if (ramType == RamObject::State)
    {
        RamState *state = qobject_cast<RamState*>( obj );
        bgColor = state->color();
        if (bgColor.lightness() > 80) textColor = m_dark;
    }

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

    // Type Specific Drawing
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
    case RamObject::Project:
    {
        RamProject *project = qobject_cast<RamProject*>( obj );
        // icon
        painter->drawPixmap( iconRect, QIcon(":/icons/project").pixmap(QSize(12,12)));
        // details
        if (bgRect.height() >= 46 )
        {
            QString details =
                    QString::number(project->width()) %
                    " x " %
                    QString::number(project->height()) %
                    " (" %
                    QString::number(project->aspectRatio(),'f',2) %
                    ":1)" %
                    " @ " %
                    QString::number(project->framerate(), 'f', 2) %
                    "fps";
            painter->setPen( detailsPen );
            painter->setFont( m_detailsFont );
            painter->drawText( detailsRect, details, m_detailsOption);
        }
        break;
    }
    case RamObject::Step:
    {
        RamStep *step = qobject_cast<RamStep*>( obj );
        // Title
        if (step->isTemplate())
            title = title % " [Template]";
        // icon
        QString icon = ":/icons/asset";
        QString details = "Asset production";
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
        if (bgRect.height() >= 46 )
        {
            painter->setPen( detailsPen );
            painter->setFont( m_detailsFont );
            painter->drawText( detailsRect, details, m_detailsOption);
        }
        break;
    }
    case RamObject::AssetGroup:
    {
        RamAssetGroup *ag = qobject_cast<RamAssetGroup*>( obj );
        // Title
        if (ag->isTemplate())
            title = title % " [Template]";
        // icon
        painter->drawPixmap( iconRect, QIcon(":/icons/asset-group").pixmap(QSize(12,12)));
        break;
    }
    case RamObject::State:
    {
        RamState *state = qobject_cast<RamState*>( obj );
        // icon
        painter->drawPixmap( iconRect, QIcon(":/icons/state").pixmap(QSize(12,12)));
        if (bgRect.height() >= 46 )
        {
            // details
            QString details = "Completion ratio: " % QString::number(state->completionRatio()) % "%";
            painter->setPen( detailsPen );
            painter->setFont( m_detailsFont );
            painter->drawText( detailsRect, details, m_detailsOption);
        }
        break;
    }
    case RamObject::FileType:
    {
        RamFileType *fileType = qobject_cast<RamFileType*>( obj );
        title = title % " (." % fileType->shortName() + ")";
        // icon
        painter->drawPixmap( iconRect, QIcon(":/icons/file").pixmap(QSize(12,12)));
        if (bgRect.height() >= 46 )
        {
            // details
            QString details = "Extensions: " % fileType->extensions().join(", ");
            painter->setPen( detailsPen );
            painter->setFont( m_detailsFont );
            painter->drawText( detailsRect, details, m_detailsOption);
        }
        break;
    }
    case RamObject::Application:
    {
        //RamApplication *app = qobject_cast<RamApplication*>( obj );
        // icon
        painter->drawPixmap( iconRect, QIcon(":/icons/application").pixmap(QSize(12,12)));
        break;
    }
    case RamObject::PipeFile:
    {
        RamPipeFile *pf = qobject_cast<RamPipeFile*>( obj );
        RamFileType *ft = pf->fileType();
        title = pf->shortName();
        if (ft) title = title % "." % ft->shortName();
        // icon
        painter->drawPixmap( iconRect, QIcon(":/icons/connection").pixmap(QSize(12,12)));
        if (bgRect.height() >= 46 )
        {
            if (ft)
            {
                // details
                QString details = "File type: " % ft->name();
                painter->setPen( detailsPen );
                painter->setFont( m_detailsFont );
                painter->drawText( detailsRect, details, m_detailsOption);
            }
        }
        break;
    }
    default:
        painter->drawPixmap( iconRect, QIcon(":/icons/asset").pixmap(QSize(12,12)));
    }

    // Draw title
    painter->setPen( textPen );
    painter->setFont(m_textFont);
    painter->drawText( titleRect, title, m_textOption);

    // Draw editbutton
    if (canEdit())
    {
        if (m_editButtonHover)
        {
            QPainterPath path;
            path.addRoundedRect(editButtonRect.adjusted(-5, -5, 5, 5), 3, 3);
            painter->fillPath(path, QBrush(m_dark));
        }
        painter->drawPixmap( editButtonRect, QIcon(":/icons/edit").pixmap(QSize(12,12)));
    }
}

QSize RamObjectDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    return QSize(32,30);
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
    const QRect bgRect = option.rect.adjusted(m_padding,2,-m_padding,-2);


    // Edit Button
    const QRect editButtonRect( bgRect.right() - 22, bgRect.top() +7, 22, 22 );


    switch ( event->type() )
    {
    case QEvent::MouseButtonPress:
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );
        if (canEdit() && editButtonRect.contains(e->pos()))
        {
            m_editButtonPressed = true;
            return true;
        }
        break;
    }
    case QEvent::MouseMove:
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );
        if (canEdit() && editButtonRect.contains(e->pos()))
        {
            m_editButtonHover = true;
            return true;
        }
        else if (m_editButtonHover)
        {
            m_editButtonHover = false;
            return true;
        }
        break;
    }
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );
        if (canEdit() && m_editButtonPressed)
        {
            if (editButtonRect.contains(e->pos()))
            {
                // The object
                quintptr iptr = index.data(Qt::UserRole).toULongLong();
                RamObject *o = reinterpret_cast<RamObject*>( iptr );
                emit editObject(o);
            }
            m_editButtonPressed = false;
            return true;
        }
        break;
    }
    default:
        return QStyledItemDelegate::editorEvent( event, model, option, index );
    }

    return QStyledItemDelegate::editorEvent( event, model, option, index );

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
