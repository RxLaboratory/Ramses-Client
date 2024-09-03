#include "ramscheduledelegate.h"

#include "mainwindow.h"
#include "qabstracttextdocumentlayout.h"
#include "ramses.h"
#include "ramscheduleentry.h"
#include "ramstatustablemodel.h"
#include "duqf-utils/guiutils.h"
#include "ramstep.h"

RamScheduleDelegate::RamScheduleDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    m_abyss = QColor(28,28,28);
    m_dark = QColor(51,51,51);
    m_medium = QColor(109,109,109);
    m_lessLight = QColor(157,157,157);
    m_light = QColor(227,227,227);
    m_textFont = qApp->font();
    m_textFont.setPixelSize( 12 );
}

void RamScheduleDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Base Settings
    const QRect rect = option.rect;
    painter->setRenderHint(QPainter::Antialiasing);

    // PAINT BG

    // bg
    QRect adjustedRect = rect.adjusted(0,1,-3,-1);
    QRect bgRect = adjustedRect;

    // Get entries
    QVector<RamScheduleEntry*> entries = RamScheduleEntry::get(index);

    // Get Date
    QDate date = index.model()->headerData(index.column(), Qt::Horizontal, RamObject::Date).toDate();

    // Empty cell background
    if (entries.isEmpty()) {
        QColor c = m_dark;

        // Alternate week colors
        if (date.weekNumber() % 2 == 0)
            c = c.darker(120);

        c = adjustBackgroundColor( c, index, option );
        painter->fillRect(bgRect, QBrush(c));
    }
    else {
        // Sub rects for each entry
        qreal r = qreal(option.rect.height()) / qreal(sizeHint(option, index).height());

        for (auto entry: qAsConst(entries)) {

            bgRect.setHeight(
                entrySize(entry).height() * r
                );
            // Round the last rect
            if (bgRect.bottom() >= option.rect.bottom()-2)
                bgRect.setBottom(option.rect.bottom()-1);

            QColor bgColor = adjustBackgroundColor( entry->color(), index, option );
            if (entry == m_pressedEntry)
                bgColor = bgColor.darker(150);
            painter->fillRect(bgRect, QBrush(bgColor));

            // Text color
            QColor textColor;
            if (bgColor.lightness() > 80) textColor = m_abyss;
            else textColor = m_lessLight;
            QPen textPen(textColor);

            // Too Small !
            if (bgRect.height() < 26 )
            {
                drawMore(painter, bgRect, textPen);
                continue;
            }

            // icon
            QString iconName = entry->iconName();
            if (iconName != "") {
                QRect iconRect( bgRect.left() + 10, bgRect.center().y() - 6 , 12, 12 );
                if (m_details)
                    iconRect.moveTop(bgRect.top() + 10); // stick on top
                DuIcon icon(iconName);
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
                    iconPainter.drawPixmap( QRect(0,0,12,12), icon.pixmap(12,12) );
                    painter->drawImage(iconRect, iconImage);
                }
            }

            // Title
            // text
            QRect textRect( bgRect.left() + 22, bgRect.top(), bgRect.width() - 22, bgRect.height() );
            if (m_details)
                textRect.setHeight(30); // stick on top
            painter->setPen( textPen );
            painter->setFont( m_textFont );
            painter->drawText( textRect, Qt::AlignCenter, entry->name());

            // Details
            if (bgRect.height() > 35 && m_details)
            {
                QRect detailsRect( bgRect.left() + 22, textRect.bottom() + 10, bgRect.width() - 32, bgRect.height() - textRect.height() - 10);
                QString details = entry->details();

                int textHeight = drawMarkdown(painter, detailsRect, details);

                if (detailsRect.height() < textHeight && details != "")
                    drawMore(painter, bgRect, textPen);
            }

            // Next
            bgRect.moveTop(bgRect.top() + bgRect.height());
        }
    }

    // Due Tasks
    if (m_details) {
        const QSet<RamStatus*> tasks = getDueTasks(index);
        if (!tasks.isEmpty()) {
            QPen p(QColor(227,227,227));
            p.setWidth(2);
            painter->setPen(p);
            painter->drawRect(adjustedRect);

            QString tasksTxt = getTasksHTML(tasks);

            bgRect.setBottom(adjustedRect.bottom());
            bgRect.setLeft(bgRect.left() + 10);

            QPen textPen(m_lessLight);
            painter->setPen(textPen);

            int textHeight = drawHtml(painter, bgRect, tasksTxt);
            if (textHeight > bgRect.height())
                drawMore(painter, bgRect, textPen);
        }
    }
}

QSize RamScheduleDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    // Get entries
    const QVector<RamScheduleEntry*> entries = RamScheduleEntry::get(index);

    // None
    if (entries.isEmpty())
        return QSize(40,40);

    int height = 0;
    int width = 40;

    for (const auto entry: entries) {
        QSize s = entrySize(entry);

        width = std::max(width, s.width());
        height += s.height();
    }

    if (m_details) {
        const QSet<RamStatus *> tasks = getDueTasks(index);
        if (!tasks.isEmpty()) {

            QString tasksTxt = getTasksHTML(tasks);

            auto tasksDoc = textDocument(m_textFont);
            tasksDoc->setHtml(tasksTxt);

            QSize s = tasksDoc->size().toSize();
            height += s.height() + 10;
            width = std::max(width, s.width() + 40);
        }
    }

    return QSize(width,height);
}

bool RamScheduleDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    // Return asap if we don't manage the event
    QEvent::Type type = event->type();
    if (type != QEvent::MouseButtonPress && type != QEvent::MouseButtonRelease)
        return QStyledItemDelegate::editorEvent( event, model, option, index );

    // Check if the current user can edit the entry
    RamUser *u = Ramses::instance()->currentUser();
    if (!u)
        return QStyledItemDelegate::editorEvent( event, model, option, index );
    if (u->role() < RamUser::Lead)
        return QStyledItemDelegate::editorEvent( event, model, option, index );

    // On press, keep the pressed entry
    if (type == QEvent::MouseButtonPress)
    {
        m_pressedEntry = nullptr;
        QMouseEvent *e = static_cast< QMouseEvent * >( event );

        if (e->button() != Qt::LeftButton)
            return QStyledItemDelegate::editorEvent( event, model, option, index );
        if (e->modifiers() != Qt::NoModifier)
            return QStyledItemDelegate::editorEvent( event, model, option, index );

        m_pressedEntry = mouseEventEntry(e, index, option);

        return QStyledItemDelegate::editorEvent( event, model, option, index );
    }

    // Show the editor
    if (type == QEvent::MouseButtonRelease && m_pressedEntry)
    {
        QMouseEvent *e = static_cast< QMouseEvent * >( event );

        if (e->button() != Qt::LeftButton)
            return QStyledItemDelegate::editorEvent( event, model, option, index );

        if (m_pressedEntry == mouseEventEntry(e, index, option))
        {
            // Edit entry
            m_pressedEntry->edit();
        }

        m_pressedEntry = nullptr;
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

RamScheduleEntry *RamScheduleDelegate::mouseEventEntry(QMouseEvent *e, const QModelIndex &index, const QStyleOptionViewItem &option) const
{
    const QVector<RamScheduleEntry*> entries = RamScheduleEntry::get(index);
    int count = entries.count();
    if (count == 0)
        return nullptr;

    // Check which entry was clicked
    int y = e->pos().y() - option.rect.top();
    qreal r = qreal(option.rect.height()) / qreal(sizeHint(option, index).height());
    int h = 0;
    int i = 0;

    for (auto entry: entries) {
        h += entrySize(entry).height() * r;
        if (y < h)
            break;
        ++i;
    }

    if (i >= 0 && i < count)
        return entries.at(i);

    return nullptr;
}

QSize RamScheduleDelegate::entrySize(RamScheduleEntry *entry) const
{
    // At Least a title
    QFontMetrics titleFM(m_textFont);
    int h = 30;
    int w = 40 + titleFM.horizontalAdvance(
                entry->name()
                );

    QString details = entry->details();
    if (m_details && details != "") {
        auto detailsDoc = textDocument( m_textFont);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
        detailsDoc->setPlainText(details);
#else
        detailsDoc->setMarkdown(details);
#endif
        w = std::max(w, int(detailsDoc->size().width() + 40));
        h += detailsDoc->size().height() + 20;
    }

    return QSize(w, h);
}

QString RamScheduleDelegate::getTasksHTML(QSet<RamStatus *> tasks) const
{
    QString tasksTxt = "<h4>Due tasks</h4><ul>";
    for (const auto task: tasks) {
        QColor c = task->step()->color();
        tasksTxt += QString("<li style=\"color: %1\">%2</li>").arg(c.name(), task->shortName());
    }
    tasksTxt += "</ul>";
    return tasksTxt;
}

int RamScheduleDelegate::drawMarkdown(QPainter *painter, QRect rect, const QString &md) const
{
    painter->save();

    auto td = textDocument(m_textFont);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    td->setPlainText(md);
#else
    td->setMarkdown(md);
#endif
    td->setTextWidth(rect.width());

    return drawTextDocument(painter, rect, td);
}

int RamScheduleDelegate::drawHtml(QPainter *painter, QRect rect, const QString &md) const
{
    painter->save();

    auto td = textDocument(m_textFont);
    td->setHtml(md);

    return drawTextDocument(painter, rect, td);
}

int RamScheduleDelegate::drawTextDocument(QPainter *painter, const QRect rect, QTextDocument *td) const
{
    QRect clipRect(rect);
    clipRect.moveTo(0,0);
    painter->translate(rect.topLeft());

    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette.setColor(QPalette::Text, painter->pen().color());
    painter->setClipRect(clipRect);
    ctx.clip = clipRect;
    td->documentLayout()->draw(painter, ctx);

    painter->restore();

    int h = td->size().height();
    td->deleteLater();

    return h;
}

QTextDocument *RamScheduleDelegate::textDocument(const QFont &defaultFont) const
{
    auto td = new QTextDocument();
    td->setIndentWidth(20);
    td->setDefaultFont(defaultFont);
    return td;
}
