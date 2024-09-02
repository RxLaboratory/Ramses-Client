#include "ramschedulerowheaderview.h"

#include "qevent.h"
#include "qpainter.h"
#include "qpainterpath.h"
#include "ramses.h"

RamScheduleRowHeaderView::RamScheduleRowHeaderView(QWidget *parent):
    QHeaderView(Qt::Vertical, parent)
{
    m_abyss = QBrush(QColor(28,28,28));
    m_dark = QBrush(QColor(51,51,51));
    m_editIcon = DuIcon(":/icons/edit").pixmap(QSize(12,12));
}

void RamScheduleRowHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    if (!painter) return;
    painter->setRenderHint(QPainter::Antialiasing);

    bool editor = canEdit();

    // The background
    painter->fillRect(rect.adjusted(0,1,0,-1), m_dark);

    // Get the object
    auto scheduleRow = getScheduleRow(logicalIndex);
    if (!scheduleRow)
        return;

    // The icon
    DuIcon icon(scheduleRow->iconName());
    QRect iconRect( rect.left() + 5, rect.center().y() - 6, 12,12);
    painter->drawPixmap( iconRect, icon.pixmap(12,12));

    // The text
    QString name = scheduleRow->name();
    int w = rect.width() - 30;
    if (editor) w -= 30; // Remove the edit button size
    QRect txtRect( rect.left() + 25, rect.top(), w, rect.height());
    QPen txtPen( scheduleRow->color() );
    painter->setPen(txtPen);
    painter->drawText(txtRect, Qt::AlignCenter, name);

    // The edit button
    if (editor)
    {
        const QRect ebrect = editButtonRect( logicalIndex );
        //xpos -= 22;
        drawButton(painter, ebrect.adjusted(4,4,-4,-4), m_editIcon, m_editButtonHover == logicalIndex);
    }
}

QSize RamScheduleRowHeaderView::sectionSizeFromContents(int logicalIndex) const
{
    auto scheduleRow = getScheduleRow(logicalIndex);
    if (!scheduleRow)
        return QSize();

    // Icon + margins & spacing
    int w = 40;

    // Add text size
    QFontMetrics fm = this->fontMetrics();
    w += fm.horizontalAdvance( scheduleRow->name() );

    if (canEdit())
        w += 30;

    return QSize(w, 20);
}

void RamScheduleRowHeaderView::mousePressEvent(QMouseEvent *event)
{
    const QPoint pos = event->pos();
    const int sectionIndex = this->logicalIndexAt( pos );

    const QRect ebrect = editButtonRect( sectionIndex );

    if (ebrect.contains(pos) && canEdit())
    {
        m_editButtonPressed = sectionIndex;
        event->accept();
        return;
    }

    QHeaderView::mousePressEvent(event);
}

void RamScheduleRowHeaderView::mouseReleaseEvent(QMouseEvent *event)
{
    const QPoint pos = event->pos();
    const int sectionIndex = this->logicalIndexAt( pos );

    const QRect ebrect = editButtonRect( sectionIndex );

    if (m_editButtonPressed == sectionIndex)
    {
        if (ebrect.contains(pos)) {
            // Get the row
            RamScheduleRow *row = getRow(sectionIndex);
            if (row && canEdit()) row->edit();
        }
        m_editButtonPressed = -1;
        event->accept();
        return;
    }

    QHeaderView::mouseReleaseEvent(event);
}

void RamScheduleRowHeaderView::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint pos = event->pos();
    const int sectionIndex = this->logicalIndexAt( pos );

    const QRect ebrect = editButtonRect( sectionIndex );

    if (ebrect.contains(pos) && canEdit())
    {
        m_editButtonHover = sectionIndex;
        this->update();
    }
    else if (m_editButtonHover == sectionIndex)
    {
        m_editButtonHover = -1;
        this->update();
    }

    QHeaderView::mouseMoveEvent(event);
}

bool RamScheduleRowHeaderView::canEdit() const
{
    return Ramses::instance()->isProjectAdmin();
}

RamScheduleRow *RamScheduleRowHeaderView::getRow(int section) const
{
    QString uuid = this->model()->headerData( section, Qt::Vertical, RamObject::UUID).toString();
    if (uuid == "") return nullptr;
    return RamScheduleRow::get( uuid );
}

QRect RamScheduleRowHeaderView::editButtonRect(int section) const
{
    int sectionHeight = sectionSize( section );

    int x = this->size().width() - 26;
    int y = sectionViewportPosition( section ) + sectionHeight/2 - 10;

    return QRect(x,y,20,20);
}

RamScheduleRow *RamScheduleRowHeaderView::getScheduleRow(int section) const
{
    QString uuid = this->model()->headerData(section, Qt::Vertical, RamAbstractObject::UUID).toString();
    if (uuid == "")
        return nullptr;
    return RamScheduleRow::get(uuid);
}

void RamScheduleRowHeaderView::drawButton(QPainter *painter, const QRect &rect, const QPixmap &icon, bool hover, bool checked) const
{
    if (hover || checked)
    {
        QPainterPath path;
        path.addRoundedRect(rect.adjusted(-5, -5, 5, 5), 3, 3);
        if (hover && checked) painter->fillPath(path, m_abyss);
        painter->fillPath(path, m_abyss);
    }
    painter->drawPixmap( rect, icon );
}
