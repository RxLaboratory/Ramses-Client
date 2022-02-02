#include "ramstatisticstable.h"

RamStatisticsTable::RamStatisticsTable(QObject *parent) : QAbstractTableModel(parent)
{
    connectEvents();
}

int RamStatisticsTable::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

int RamStatisticsTable::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (!m_project) return 0;

    return m_project->steps()->count();
}

QVariant RamStatisticsTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!m_project) return QVariant();

    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole) return "Completion";

        return QVariant();
    }

    RamStep *step = qobject_cast<RamStep*>( m_project->steps()->at(section ));

    if (role == Qt::DisplayRole) return step->shortName();

    if (role == Qt::ForegroundRole)
    {
        QColor stepColor = step->color();
        if (stepColor.lightness() < 150) stepColor.setHsl( stepColor.hue(), stepColor.saturation(), 150);
        return QBrush(stepColor);
    }

    return QVariant();
}

QVariant RamStatisticsTable::data(const QModelIndex &index, int role) const
{
    int row = index.row();

    RamStep *step = qobject_cast<RamStep*>( m_project->steps()->at( row ));


    if (role == Qt::DisplayRole)
    {
        QString text;
        if (step->type() == RamStep::ShotProduction || step->type() == RamStep::AssetProduction)
        {
            int completion = step->completionRatio();
            float estimation = step->estimation();
            float daysSpent = completion * estimation / 100.0;
            float needed = estimation - daysSpent;
            float futureUnassigned = needed - step->futureAssignedDays();
            text = "Completion: " %
                    QString::number( completion ) % " % (" %
                    QString::number( daysSpent, 'f', 1 ) % " / " % QString::number(estimation, 'f', 1) % " days)";

            if (completion > 99.9)
            {
                text = text % "\n\nFinished!\n\n";
            }
            else if (futureUnassigned > 0.4)
            {
                text = text % "\n\nRemaining: " % QString::number( needed, 'f', 1  ) % " days\nMissing: " % QString::number( futureUnassigned, 'f', 1  ) % " days\n";
            }
            else if ( futureUnassigned < -0.4 )
            {
                text = text % "\n\nThere are " % QString::number( -futureUnassigned, 'f', 1  ) % " extra days.\n\n";
            }
            else
            {
                text = text % "\n\nSchedule seems OK\n\n";
            }

            text = text % "\nAssigned: " % QString::number( step->assignedDays(), 'f', 1) % " days";

            if (step->unassignedDays() > 0) text = text %
                                                "\nMissing: " %
                                                QString::number( step->unassignedDays(), 'f', 1) %
                                                " days";
            else if (step->unassignedDays() < 0) text = text %
                                                    "\nExtra: " %
                                                    QString::number( -step->unassignedDays(), 'f', 1) %
                                                    " days";
        }
        else
        {
            text = " Assigned: " % QString::number( step->assignedDays(), 'f', 1) % " days";
        }

        return text;
    }
    if (role == Qt::ForegroundRole)
    {
        float latenessRatio = step->latenessRatio();
        QColor timeColor;
        if ( latenessRatio < 1.0 ) timeColor = QColor(157,157,157);
        else if ( latenessRatio < 1.1 ) timeColor = QColor(191,177,72);
        else if ( latenessRatio < 1.2 ) timeColor = QColor(186,100,50);
        else if ( latenessRatio < 1.3 ) timeColor = QColor(191,148,61);
        else if ( latenessRatio < 1.4 ) timeColor = QColor(213,98,44);
        else if ( latenessRatio < 1.5 ) timeColor = QColor(216,62,31);
        else if ( latenessRatio < 1.6 ) timeColor = QColor(230,31,17);
        else if ( latenessRatio < 1.7 ) timeColor = QColor(244,2,2);
        else timeColor = QColor(255,0,0);
    }

    if (role == Qt::ToolTipRole) return QString( step->name() %
                                                 "\nCompletion: " %
                                                 QString::number( step->completionRatio(), 'f', 0) %
                                                 " %\nLateness: " %
                                                 QString::number( (step->latenessRatio() -1) * 100, 'f', 0) %
                                                 " %");

    if (role == Qt::StatusTipRole) return QString( step->shortName() % " | " % step->name() %
                                                   " | Completion: " %
                                                   QString::number( step->completionRatio()) %
                                                   " % | Lateness: " %
                                                   QString::number( (step->latenessRatio() -1) * 100, 'f', 0) %
                                                   " %");

    if (role == Qt::UserRole) return step->completionRatio();
    if (role == Qt::UserRole +1) return step->latenessRatio();
    if (role == Qt::UserRole +2) return step->estimation();
    if (role == Qt::UserRole +3) return step->timeSpent();
    if (role == Qt::UserRole +4) return step->assignedDays();
    if (role == Qt::UserRole +5) return step->unassignedDays();

    if (role == Qt::UserRole +6) return step->type();

    return QVariant();
}

void RamStatisticsTable::changeProject(RamProject *project)
{
    beginResetModel();

    m_project = project;

    endResetModel();

    if (!project) return;

    connect( m_project, SIGNAL(estimationComputed(RamProject*)),this,SLOT(estimationComputed()));
    connect( project->steps(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(insertStep(QModelIndex,int,int)));
    connect( project->steps(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(removeStep(QModelIndex,int,int)));
}

void RamStatisticsTable::removeStep(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    // We're removing rows
    beginRemoveRows(QModelIndex(), first, last);
    endRemoveRows();
}

void RamStatisticsTable::estimationComputed()
{
    emit dataChanged(createIndex(0,0), createIndex(rowCount()-1, 0));
}

void RamStatisticsTable::insertStep(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    //We're inserting new rows
    beginInsertRows(QModelIndex(), first, last);
    // Finished!
    endInsertRows();
}

void RamStatisticsTable::connectEvents()
{
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(changeProject(RamProject*)));
}
