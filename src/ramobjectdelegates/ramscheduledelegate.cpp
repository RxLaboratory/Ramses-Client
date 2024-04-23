#include "ramscheduledelegate.h"

#include "mainwindow.h"
#include "ramses.h"
#include "ramscheduleentry.h"
#include "ramstatustablemodel.h"
#include "duqf-utils/guiutils.h"

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
    QRect bgRect = rect.adjusted(0,1,-3,-1);

    // Get entries
    const QVariantList ptrList = index.data(RamObject::Pointer).toList();

    QVector<RamScheduleEntry*> entries;
    for (const auto &i: ptrList) {
        qintptr iptr = i.toULongLong();
        entries << reinterpret_cast<RamScheduleEntry*>(iptr);
    }

    // Empty cell
    if (entries.isEmpty()) {
        QBrush b = index.data(Qt::BackgroundRole).value<QBrush>();
        QColor c = adjustBackgroundColor( b.color(), index, option );
        painter->fillRect(bgRect, QBrush(c));
        return;
    }

    // Sub rects for each entry
    int h = bgRect.height() / entries.count();
    bgRect.setHeight( h );

    for (auto entry: qAsConst(entries)) {

        QColor bgColor = adjustBackgroundColor( entry->color(), index, option );
        painter->fillRect(bgRect, QBrush(bgColor));

        // Border if this is a deadline for some tasks
        const QSet<RamStatus*> tasks = getDueTasks(index);
        if (!tasks.isEmpty()) {
            QPen p(QColor(227,227,227));
            p.setWidth(2);
            painter->setPen(p);
            painter->drawRect(bgRect);
        }

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
            if (commentRect.height() < textHeight && comment != "") drawMore(painter, bgRect, textPen);
        }

        // Next
        bgRect.moveTop(bgRect.top() + h);
    }
}

QSize RamScheduleDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    // Get entries
    const QVariantList ptrList = index.data(RamObject::Pointer).toList();

    // None
    if (ptrList.isEmpty())
        return QSize(40,40);

    int h = 0;
    int w = 40;

    for (const auto &i: ptrList) {
        auto entry = reinterpret_cast<RamScheduleEntry*>(i.toULongLong());
        QSize s;
        if (m_details)
            s = entry->roleData(RamObject::DetailedSizeHint).toSize();
        else
            s = entry->roleData(RamObject::SizeHint).toSize();

        h += s.height();
        w = std::max(w, s.width());
    }

    return QSize(w,h);
}

bool RamScheduleDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    /*
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
                QDate date = getDate(m_indexPressed);

                // Get the user and date
                RamUser *user = getUser(m_indexPressed);
                if (!user)
                    return false;
                RamScheduleEntry *entry = getEntry(m_indexPressed);

                // Create and show an editor
                //m_currentEditor = new ScheduleEntryEditWidget(proj, user, date, entry);
                //mw->setPropertiesDockWidget(m_currentEditor, tr("Schedule Entry")); // TODO Icon
            }
        }
        return true;
    }*/

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
    QString rowUuid = index.model()->headerData(index.row(), Qt::Vertical, RamObject::UUID).toString();
    if (rowUuid == "") return nullptr;
    auto scheduleRow = RamScheduleRow::get(rowUuid);
    return scheduleRow->user();
}

QDate RamScheduleDelegate::getDate(QModelIndex index) const
{
    // Get Date
    return index.data( RamObject::Date ).toDate();
}

QSet<RamStatus *> RamScheduleDelegate::getDueTasks(QModelIndex index) const
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return QSet<RamStatus *>();

    RamUser *user = getUser(index);
    if (!user) return QSet<RamStatus *>();

    QDate date = getDate(index);

    QSet<RamStatus*> tasks = proj->assetStatus()->getStatus(date, user->uuid());
    tasks += proj->shotStatus()->getStatus(date, user->uuid());

    // Clean up
    QMutableSetIterator<RamStatus*> i(tasks);
    while (i.hasNext()) {
        i.next();
        if (i.value()->completionRatio() >= 100)
            tasks.remove(i.value());
        if (!i.value()->item())
            tasks.remove(i.value());
        if (!i.value()->step())
            tasks.remove(i.value());
    }

    return tasks;
}

QColor RamScheduleDelegate::adjustBackgroundColor(const QColor &color, const QModelIndex &index, const QStyleOptionViewItem &option) const
{
    QColor c = color;
    if (option.state & QStyle::State_Selected) c = c.darker();
    else if (option.state & QStyle::State_MouseOver) c = c.lighter();
    // before today -> a bit darker
    QDate date = index.data(RamObject::Date).value<QDate>();
    if (date < QDate::currentDate()) c = c.darker(175);

    return c;
}

int RamScheduleDelegate::drawMarkdown(QPainter *painter, QRect rect, const QString &md) const
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
