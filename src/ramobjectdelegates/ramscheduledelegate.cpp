#include "ramscheduledelegate.h"

#include "mainwindow.h"
#include "ramses.h"
#include "ramscheduleentry.h"
#include "ramstatustablemodel.h"
#include "duqf-utils/guiutils.h"
#include "schedulecommenteditwidget.h"
#include "scheduleentryeditwidget.h"

RamScheduleDelegate::RamScheduleDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    m_abyss = QColor(28,28,28);
    m_dark = QColor(51,51,51);
    m_medium = QColor(109,109,109);
    m_lessLight = QColor(157,157,157);
    m_light = QColor(227,227,227);
    m_textFont = qApp->font();
    m_textFont.setPixelSize( 12 );
    m_detailsFont = m_textFont;
    m_detailsFont.setItalic(true);
    m_padding = 10;
}

void RamScheduleDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Base Settings
    const QRect rect = option.rect;
    painter->setRenderHint(QPainter::Antialiasing);

    // PAINT BG

    // bg
    const QRect bgRect = rect.adjusted(m_padding/2,2,-m_padding/2,-2);

    // Select the bg Color
    QBrush bgBrush = index.data(Qt::BackgroundRole).value<QBrush>();

    // State
    QColor bgColor = bgBrush.color();
    if (option.state & QStyle::State_Selected) bgColor = bgColor.darker();
    else if (option.state & QStyle::State_MouseOver) bgColor = bgColor.lighter();

    // before today -> a bit darker
    QDate date = index.data(RamObject::Date).value<QDate>();
    if (date < QDate::currentDate()) bgColor = bgColor.darker(175);

    bgBrush.setColor(bgColor);

    QPainterPath path;
    path.addRoundedRect(bgRect, 5, 5);
    painter->fillPath(path, bgBrush);

    // Text color
    QColor textColor;
    if (bgColor.lightness() > 80) textColor = m_abyss;
    else textColor = m_lessLight;
    QPen textPen(textColor);

    // Too Small !
    if (bgRect.height() < 26 )
    {
        drawMore(painter, bgRect, textPen);
        return;
    }

    // Get the entry
    if (index.data(RamObject::IsComment).toBool()) // comment
    {
        QString comment = index.data(Qt::DisplayRole).toString();
        QRect commentRect(
            bgRect.left() + m_padding,
            bgRect.top() + m_padding,
            bgRect.width() - m_padding,
            bgRect.height() - m_padding
            );
        painter->setPen(textPen);
        int textHeight = drawMarkdown(painter, commentRect, comment);
        if (commentRect.height() < textHeight) drawMore(painter, bgRect, textPen);

        return;
    }

    // Border if this is a deadline for some tasks
    const QSet<RamStatus*> tasks = getDueTasks(index);
    if (!tasks.isEmpty()) {
        QPen p(QColor(227,227,227));
        p.setWidth(2);
        painter->setPen(p);
        painter->drawPath(path);
    }

    // icon
    const QRect iconRect( bgRect.left() + m_padding, bgRect.top() +7 , 12, 12 );
    // text
    const QRect textRect( iconRect.right() + 5, iconRect.top()-5, bgRect.width() - 37, iconRect.height()+5 );

    // icon
    QPixmap icon = index.data(Qt::DecorationRole).value<QPixmap>();
    if (!icon.isNull())
    {
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
    }

    // Title
    painter->setPen( textPen );
    painter->setFont( m_textFont );
    painter->drawText( textRect, Qt::AlignCenter | Qt::AlignHCenter, index.data(Qt::DisplayRole).toString());

    // Comment
    if (bgRect.height() > 35)
    {
        QRect commentRect( iconRect.left(), bgRect.top() + 30, bgRect.width() - m_padding*2, bgRect.height() - 30);
        QString comment = index.data(RamObject::Comment).toString();
        // Add the due tasks
        if (!tasks.isEmpty()) {
            comment += "\n\n#### Due tasks\n";
            for (const auto task: tasks) {
                comment += "\n- " + task->shortName();
            }
        }
        int textHeight = drawMarkdown(painter, commentRect, comment);
        if (commentRect.height() < textHeight) drawMore(painter, bgRect, textPen);
    }
//*/
}

QSize RamScheduleDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    //return QSize(100,40);

    QString uuid = index.data(RamObject::UUID).toString();
    if (uuid == "") return QSize(0,40);

    QSize s;
    if (m_details) s = index.data(RamObject::DetailedSizeHint).toSize();
    else s = index.data(RamObject::SizeHint).toSize();

    // In schedules, the height is a bit bigger
    if (s.height() < 40) s.setHeight(40);

    return s;
}

bool RamScheduleDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    // Return asap if we don't manage the event
    QEvent::Type type = event->type();
    if (type != QEvent::MouseButtonPress && type != QEvent::MouseButtonRelease)
        return QStyledItemDelegate::editorEvent( event, model, option, index );

    RamUser *u = Ramses::instance()->currentUser();
    if (!u) return QStyledItemDelegate::editorEvent( event, model, option, index );
    if (u->role() < RamUser::Lead) return QStyledItemDelegate::editorEvent( event, model, option, index );

    if (type == QEvent::MouseButtonPress)
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );
        if (e->button() != Qt::LeftButton) return QStyledItemDelegate::editorEvent( event, model, option, index );
        if (e->modifiers() != Qt::NoModifier) return QStyledItemDelegate::editorEvent( event, model, option, index );
        m_indexPressed = index;
        return QStyledItemDelegate::editorEvent( event, model, option, index );
    }

    if (type == QEvent::MouseButtonRelease)
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );

        if (e->button() != Qt::LeftButton)
            return QStyledItemDelegate::editorEvent( event, model, option, index );

        if (m_indexPressed == index && m_indexPressed.isValid())
        {
            if (m_currentEditor)
                m_currentEditor->deleteLater();

            RamProject *proj = Ramses::instance()->currentProject();
            if (proj)
            {
                auto mw = qobject_cast<MainWindow*>(GuiUtils::appMainWindow());
                QDateTime date = getDate(m_indexPressed);

                if (index.data(RamObject::IsComment).toBool()) {
                    RamScheduleComment *comment = getComment(m_indexPressed);
                    m_currentEditor = new ScheduleCommentEditWidget(proj, date, comment);
                    mw->setPropertiesDockWidget(m_currentEditor, tr("Schedule Note") /* TODO ICON */);
                }
                else {
                    // Get the user and date
                    RamUser *user = getUser(m_indexPressed);
                    if (!user)
                        return false;
                    RamScheduleEntry *entry = getEntry(m_indexPressed);

                    // Create and show an editor
                    m_currentEditor = new ScheduleEntryEditWidget(proj, user, date, entry);
                    mw->setPropertiesDockWidget(m_currentEditor, tr("Schedule Entry") /* TODO ICON */);
                }
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

RamUser *RamScheduleDelegate::getUser(QModelIndex index) const
{
    RamScheduleEntry *entry = getEntry(index);
    if (entry) return entry->user();

    QString userUuid = index.model()->headerData( index.row(), Qt::Vertical, RamObject::UUID ).toString();
    if (userUuid != "") return RamUser::get( userUuid );
    return nullptr;
}

QDateTime RamScheduleDelegate::getDate(QModelIndex index) const
{
    RamScheduleEntry *entry = getEntry(index);
    if (entry) return entry->date();

    // Get Date
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QDateTime date = QDateTime( index.model()->headerData( index.column(), Qt::Horizontal, RamObject::Date).toDate() );
#else
    QDateTime date = index.model()->headerData( index.column(), Qt::Horizontal, RamObject::Date).toDate().startOfDay();
#endif
    if (  index.model()->headerData( index.row(), Qt::Vertical, RamObject::IsPM ).toBool() )
        date.setTime(QTime(12,0));

    return date;
}

RamScheduleEntry *RamScheduleDelegate::getEntry(QModelIndex index) const
{
    // Note row, return
    if (index.row() == 0)
        return nullptr;

    QString entryUuid = index.data(RamObject::UUID).toString();
    if (entryUuid != "") return RamScheduleEntry::get( entryUuid );
    return nullptr;
}

RamScheduleComment *RamScheduleDelegate::getComment(QModelIndex index) const
{
    // Not first row, return
    if (index.row() != 0)
        return nullptr;

    QString uuid = index.data(RamObject::UUID).toString();
    if (uuid != "") return RamScheduleComment::get( uuid );
    return nullptr;
}

QSet<RamStatus *> RamScheduleDelegate::getDueTasks(QModelIndex index) const
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return QSet<RamStatus *>();

    RamUser *user = getUser(index);
    if (!user) return QSet<RamStatus *>();

    QDate date = getDate(index).date();

    QSet<RamStatus*> tasks = proj->assetStatus()->getStatus(date, user->uuid());
    tasks += proj->shotStatus()->getStatus(date, user->uuid());

    // Clean up
    QMutableSetIterator<RamStatus*> i(tasks);
    while (i.hasNext()) {
        i.next();
        if (i.value()->completionRatio() >= 100)
            tasks.remove(i.value());
    }

    return tasks;
}

int RamScheduleDelegate::drawMarkdown(QPainter *painter, QRect rect, const QString &md) const
{
    painter->save();
    painter->setFont( m_textFont );

    QTextDocument td;
    td.setIndentWidth(20);
    td.setMarkdown(md);
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
