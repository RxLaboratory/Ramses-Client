#include "ramscheduledelegate.h"

#include "ramobjectlistcombobox.h"
#include "ramses.h"
#include "ramschedulecomment.h"
#include "ramscheduleentry.h"
#include "duqf-utils/guiutils.h"
#include "duqf-app/app-style.h"

RamScheduleDelegate::RamScheduleDelegate(QObject *parent) : QStyledItemDelegate(parent)
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

    m_shotProdIcon = QIcon(":/icons/shot").pixmap(QSize(12,12));
    m_assetProdIcon = QIcon(":/icons/asset").pixmap(QSize(12,12));
    m_preProdIcon = QIcon(":/icons/project").pixmap(QSize(12,12));
    m_postProdIcon = QIcon(":/icons/film").pixmap(QSize(12,12));
}

void RamScheduleDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
     // Base Settings
    const QRect rect = option.rect;
    painter->setRenderHint(QPainter::Antialiasing);

    // bg
    const QRect bgRect = rect.adjusted(m_padding/2,2,-m_padding/2,-2);
    // icon
    const QRect iconRect( bgRect.left() + m_padding, bgRect.top() +7 , 12, 12 );
    // text
    const QRect textRect( iconRect.right() + 5, iconRect.top()-5, bgRect.width() - 37, iconRect.height()+5 );

    // Select the bg Color
    QColor bgColor = index.data(Qt::BackgroundRole).value<QColor>();

    // State
    if (option.state & QStyle::State_Selected) bgColor = bgColor.darker();
    else if (option.state & QStyle::State_MouseOver) bgColor = bgColor.lighter();

    // before today -> a bit darker
    QDate date = index.data(Qt::UserRole + 1).value<QDate>();
    if (date < QDate::currentDate()) bgColor = bgColor.darker(175);

    // Text color
    QColor textColor;
    if (bgColor.lightness() > 80) textColor = m_abyss;
    else textColor = m_lessLight;

    QBrush bgBrush(bgColor);
    QPen textPen(textColor);

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

    // Get the entry
    quintptr iptr = index.data(Qt::UserRole).toULongLong();

    if (index.data(Qt::UserRole + 3).toBool()) // comment
    {
        QString text = index.data(Qt::DisplayRole).toString();
        QStringList lines = text.split("\n");

        // Title
        painter->setPen( textPen );
        QFont titleFont = m_textFont;
        titleFont.setBold(true);
        painter->setFont( titleFont );
        painter->drawText( textRect.adjusted(-12,0,0,0), Qt::AlignCenter | Qt::AlignHCenter, lines.takeFirst());

        // Other lines
        // Comment
        if (bgRect.height() > 35 && lines.count() > 0)
        {
            QRect commentRect( iconRect.left(), bgRect.top() + 35, bgRect.width() - m_padding*2, bgRect.height() - 35);
            painter->setFont( m_textFont );
            painter->drawText( commentRect, Qt::AlignLeft | Qt::AlignTop, lines.join("\n"), &commentRect);
            if (commentRect.bottom() > bgRect.bottom() - 5) drawMore(painter, bgRect, textPen);
        }

        return;
    }

    if (iptr == 0) return;
    RamScheduleEntry *entry = reinterpret_cast<RamScheduleEntry*>( iptr );
    RamStep *step = entry->step();
    if (!step) return;

    // icon
    QPixmap icon;
    switch( step->type())
    {
    case RamStep::PreProduction: icon = m_preProdIcon; break;
    case RamStep::ShotProduction: icon = m_shotProdIcon; break;
    case RamStep::AssetProduction: icon = m_assetProdIcon; break;
    case RamStep::PostProduction: icon = m_postProdIcon; break;
    default: icon = m_assetProdIcon;
    }

    // icon color
    QColor iconColor;
    if (bgColor.lightness() > 80) iconColor = m_dark;
    else iconColor = m_medium;
    QImage iconImage(12,12, QImage::Format_ARGB32);
    iconImage.fill( iconColor );
    QPainter iconPainter(&iconImage);
    iconPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    iconPainter.drawPixmap( QRect(0,0,12,12), icon );
    painter->drawImage(iconRect, iconImage);

    // Title
    if (index.data(Qt::UserRole) != 0)
    {
        painter->setPen( textPen );
        painter->setFont( m_textFont );
        painter->drawText( textRect, Qt::AlignCenter | Qt::AlignHCenter, index.data(Qt::DisplayRole).toString());
    }

    // Comment
    if (bgRect.height() > 35)
    {
        QRect commentRect( iconRect.left(), bgRect.top() + 30, bgRect.width() - m_padding*2, bgRect.height() - 35);
        painter->drawText( commentRect, Qt::AlignLeft | Qt::AlignTop, index.data(Qt::UserRole +2).toString(), &commentRect);
        if (commentRect.bottom() > bgRect.bottom() - 5) drawMore(painter, bgRect, textPen);
    }
//*/
}

QSize RamScheduleDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    // Get the entry
    quintptr iptr = index.data(Qt::UserRole).toULongLong();
    if (iptr == 0) return QSize(75, 10);

    if (index.data(Qt::UserRole + 3).toBool()) {
        RamScheduleComment *comment = reinterpret_cast<RamScheduleComment*>( iptr );
        if (comment) {
            if (comment->comment().indexOf("\n") >= 0 && m_details) return QSize(200, 75);
        }
        return QSize(75, 10);
    }


    RamScheduleEntry *entry = reinterpret_cast<RamScheduleEntry*>( iptr );
    if (!entry) return QSize(75, 10);
    RamStep *step = entry->step();
    if (!step) return QSize(75, 10);

    if (m_details && entry->comment() != "") return QSize(200, 75);
    return QSize(75, 25);
}

bool RamScheduleDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    // Return asap if we don't manage the event
    QEvent::Type type = event->type();
    if (type != QEvent::MouseButtonPress && type != QEvent::MouseButtonRelease && type != QEvent::MouseMove)
        return QStyledItemDelegate::editorEvent( event, model, option, index );

    RamUser *u = Ramses::instance()->currentUser();
    if (!u) return QStyledItemDelegate::editorEvent( event, model, option, index );
    if (u->role() < RamUser::Lead) return QStyledItemDelegate::editorEvent( event, model, option, index );


    if (type == QEvent::MouseButtonPress)
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );
        if (e->button() != Qt::LeftButton) return false;
        if (e->modifiers() != Qt::NoModifier) return QStyledItemDelegate::editorEvent( event, model, option, index );
        m_indexPressed = index;
        return true;
    }

    if (type == QEvent::MouseButtonRelease)
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );
        if (e->button() != Qt::LeftButton) return QStyledItemDelegate::editorEvent( event, model, option, index );

        if (m_indexPressed == index && m_indexPressed.isValid() && !index.data(Qt::UserRole+3).toBool())
        {
            RamProject *proj = Ramses::instance()->currentProject();
            if (proj)
            {
                QMainWindow *mw = GuiUtils::appMainWindow();
                RamObjectListComboBox *editor = new RamObjectListComboBox( true, mw );
                editor->setList(proj->steps());
                QRect rect = option.rect.adjusted(m_padding,2,-m_padding,-2);
                editor->setGeometry( rect );
                editor->move( mw->mapFromGlobal( e->globalPos() )  - e->pos() + QPoint(rect.left(), rect.top() ) );
                // Get current
                RamStep *step = reinterpret_cast<RamStep*>( index.data(Qt::EditRole).toULongLong() );
                editor->setObject(step);
                editor->show();
                editor->showPopup();
                connect(editor, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(setEntry(RamObject*)));
                connect(editor, SIGNAL(popupHidden()), editor, SLOT(deleteLater()));
            }
        }
        return true;
    }

    return QStyledItemDelegate::editorEvent( event, model, option, index );
}

void RamScheduleDelegate::showDetails(bool s)
{
    m_details = s;
}

void RamScheduleDelegate::setEntry(RamStep *step)
{
    if (!m_indexPressed.isValid()) return;

    // Get current entry if any
    RamScheduleEntry *entry = reinterpret_cast<RamScheduleEntry*>( m_indexPressed.data(Qt::UserRole).toULongLong() );
    if (!entry && step)
    {
        // Get user
        quintptr iptr = m_indexPressed.model()->headerData( m_indexPressed.row(), Qt::Vertical, Qt::UserRole ).toULongLong();
        RamUser *user = reinterpret_cast<RamUser*>( iptr );
        if (!user) return;
        // Get Date
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
        QDateTime date = QDateTime( m_indexPressed.model()->headerData( m_indexPressed.column(), Qt::Horizontal, Qt::UserRole).toDate() );
#else
        QDateTime date = m_indexPressed.model()->headerData( m_indexPressed.column(), Qt::Horizontal, Qt::UserRole).toDate().startOfDay();
#endif
        if (  m_indexPressed.model()->headerData( m_indexPressed.row(), Qt::Vertical, Qt::UserRole+1 ).toBool() )
            date.setTime(QTime(12,0));

        entry = new RamScheduleEntry( user, date );
        entry->setStep(step);
        user->schedule()->append(entry);
    }
    else if (entry)
    {
        if (step)
            entry->setStep( step );
        else
            entry->remove();
    }
}

bool RamScheduleDelegate::details() const
{
    return m_details;
}

void RamScheduleDelegate::drawMore(QPainter *painter, QRect rect, QPen pen) const
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
