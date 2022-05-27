#include "ramscheduletable.h"

#include "ramses.h"

RamScheduleTable::RamScheduleTable(QObject *parent) : QAbstractTableModel(parent)
{
    m_startDate = QDate::currentDate().addDays(-5);
    m_endDate = QDate::currentDate();
    connectEvents();
}

void RamScheduleTable::setList(RamObjectList *userList, RamObjectList *comments)
{
    beginResetModel();

    if (m_users) disconnect(m_users, nullptr, this, nullptr);

    m_users = userList;
    m_comments = comments;

    if (m_users)
    {
        connect( m_users, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(insertUser(QModelIndex,int,int)));
        connect( m_users, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(removeUser(QModelIndex,int,int)));
        connect( m_users, SIGNAL(modelReset()), this, SLOT(resetUsers()));
    }

    endResetModel();
}

int RamScheduleTable::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if (!m_users) return 0;

    return m_startDate.daysTo( m_endDate ) + 1;
}

int RamScheduleTable::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    int c = 0;

    if (!m_users && !m_comments) return c;

    // Two rows per user: AM and PM
    if (m_users) c = m_users->count()*2;
    // If there are comments, it's on an additionnal row
    if (m_comments) c++;

    return c;
}

QVariant RamScheduleTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!m_users) return QVariant();

    if (orientation == Qt::Vertical)
    {
        // The comments
        if (section == 0 && m_comments) {
            if ( role == Qt::DisplayRole )
                return "NOTES";
            if ( role == Qt::ToolTipRole )
                return QString("Use this line to add comments for specific dates.");
            return QAbstractTableModel::headerData(section, orientation, role);
        }

        if (m_comments) section--;
        RamObject *usrObj = m_users->at(section / 2);
        RamUser *user = qobject_cast<RamUser*>(usrObj);

        QString ampm = " | AM";
        if (section % 2 == 1) ampm = " | PM";

        if ( role == Qt::DisplayRole )
            return QString(usrObj->shortName() % ampm);

        if ( role == Qt::UserRole)
            return reinterpret_cast<quintptr>( usrObj );

        if ( role == Qt::UserRole + 1)
            return section % 2 == 1; // is pm

        if ( role == Qt::UserRole + 2)
            return usrObj->uuid();

        if ( role == Qt::ToolTipRole )
            return QString(usrObj->shortName() % " | " % usrObj->name() % ampm);

        if ( role == Qt::StatusTipRole )
            return QString(usrObj->shortName() % " | " % usrObj->name() % ampm);

        if ( role == Qt::ForegroundRole && user )
            return QBrush( user->color() );
    }
    else
    {
        QDate date = m_startDate.addDays(section);

        if ( role == Qt::DisplayRole )
        {
            return QString(date.toString("MMMM\ndddd\nyyyy-MM-dd"));
        }

        if (role == Qt::ForegroundRole)
        {
            if (date == QDate::currentDate())
                return QBrush(QColor(213,136,241));

            RamProject *proj = Ramses::instance()->currentProject();
            if (proj)
            {
                if (date == proj->deadline()) return QBrush(QColor(249,105,105));
            }

            if (date.weekNumber() % 2 == 1)
                return QBrush(QColor(130,130,130));
            else
                return QBrush(QColor(170,170,170));

        }

        if (role == Qt::UserRole)
            return date;
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant RamScheduleTable::data(const QModelIndex &index, int role) const
{
    if (!m_users) return QVariant();

    int row = index.row();
    int col = index.column();

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QDateTime date = QDateTime( m_startDate.addDays(col) );
#else
    QDateTime date = QDate( m_startDate.addDays(col) ).startOfDay();
#endif


    if (row == 0 && m_comments)
    {
        for(int i = 0; i < m_comments->count(); i++)
        {
            RamObject *cObj = m_comments->at(i);
            RamScheduleComment *c = qobject_cast<RamScheduleComment*>( cObj );
            if (c->date() == date)
            {
                if ( role == Qt::DisplayRole )
                    return c->comment();

                if ( role == Qt::ToolTipRole || role == Qt::StatusTipRole )
                    return QString(date.toString("yyyy-MM-dd") % "\n" % c->comment() );

                if ( role == Qt::BackgroundRole )
                    return c->color();

                if (role == Qt::UserRole)
                    return reinterpret_cast<quintptr>( c );

                if (role == Qt::UserRole + 3) // isComment
                    return true;

                return QVariant();
            }
        }

        if (role == Qt::UserRole + 3) // isComment
            return true;

        if (role == Qt::BackgroundRole )
        {
            if (date.date().weekNumber() % 2 == 1)
                return QColor(51,51,51);
            else
                return QColor(42,42,42);
        }


        return QVariant();
    }

    RamObject *usrObj = m_users->at((row-1) / 2);
    RamUser *user = qobject_cast<RamUser*>( usrObj );
    if (!user) return QVariant();
    RamObjectList *schedule = user->schedule();

    QString ampm = "am";
    if ( headerData(row, Qt::Vertical, Qt::UserRole +1).toBool() )
    {
        ampm = "pm";
        date.setTime(QTime(12,0));
    }

    if (role == Qt::UserRole +1 )
        return date;

    RamProject *currentProject = Ramses::instance()->currentProject();

    for (int i = 0; i < schedule->count(); i++)
    {
        RamScheduleEntry *entry = qobject_cast<RamScheduleEntry*>( schedule->at(i) );
        if (!entry) continue;
        RamStep *entryStep = entry->step();
        if (!entryStep) continue;
        RamProject *entryProj = entryStep->project();
        if (!entryProj) continue;
        // For current project only
        if (!entryProj->is(currentProject)) continue;

        if (entry->date() == date)
        {
            if ( role == Qt::DisplayRole )
                return entry->step()->shortName();

            if ( role == Qt::ToolTipRole )
                return QString(date.toString("yyyy-MM-dd") %
                        " " % ampm %
                        "\n" % entry->step()->name() %
                        "\n" % user->name() );

            if ( role == Qt::StatusTipRole )
                return QString(date.toString("yyyy-MM-dd") %
                               " " % ampm %
                               " | " % entry->step()->shortName() %
                               " | " % user->shortName() );

            if ( role == Qt::BackgroundRole )
                return entry->step()->color();

            if (role == Qt::UserRole)
                return reinterpret_cast<quintptr>( entry );

            if (role == Qt::UserRole + 2)
                return entry->comment();

            if (role == Qt::UserRole + 3) // isComment
                return false;

            if (role == Qt::EditRole)
                return reinterpret_cast<quintptr>( entry->step() );            
        }
    }

    if (role == Qt::EditRole)
        return 0;

    if (role == Qt::BackgroundRole )
    {
        if (date.date().weekNumber() % 2 == 1)
            return QBrush(QColor(51,51,51));
        else
            return QBrush(QColor(42,42,42));
    }

    return QVariant();
}

void RamScheduleTable::insertUser(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!m_users) return;

    //We're inserting new rows
    beginInsertRows(QModelIndex(), first*2, last*2+1);
    // Finished!
    endInsertRows();
}

void RamScheduleTable::removeUser(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!m_users) return;

    // We're removing rows
    beginRemoveRows(QModelIndex(), first*2, last*2+1);
    endRemoveRows();
}

void RamScheduleTable::resetUsers()
{
    beginResetModel();
    endResetModel();
}

void RamScheduleTable::setEndDate(const QDate &newEndDate)
{
    if (newEndDate < m_startDate) return;

    if (newEndDate > m_endDate)
    {
        beginInsertColumns(QModelIndex(), columnCount(), columnCount() + m_endDate.daysTo(newEndDate) -1);
        m_endDate = newEndDate;
        endInsertColumns();
    }
    else if (newEndDate < m_endDate)
    {
        beginRemoveColumns(QModelIndex(), columnCount() - newEndDate.daysTo(m_endDate) , columnCount() -1);
        m_endDate = newEndDate;
        endRemoveColumns();
    }
}

void RamScheduleTable::setStartDate(const QDate &newStartDate)
{
    if (newStartDate > m_endDate) return;

    if (newStartDate < m_startDate)
    {
        beginInsertColumns(QModelIndex(), 0, newStartDate.daysTo(m_startDate)-1);
        m_startDate = newStartDate;
        endInsertColumns();
    }
    else if (newStartDate > m_startDate)
    {
        beginRemoveColumns(QModelIndex(), 0, m_startDate.daysTo(newStartDate) -1);
        m_startDate = newStartDate;
        endRemoveColumns();
    }
}

void RamScheduleTable::connectEvents()
{

}
