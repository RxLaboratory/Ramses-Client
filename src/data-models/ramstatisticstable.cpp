#include "ramstatisticstable.h"

RamStatisticsTable::RamStatisticsTable(QObject *parent) : QAbstractTableModel(parent)
{
    connectEvents();
}

int RamStatisticsTable::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int RamStatisticsTable::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (!m_project) return 0;

    return m_project->steps()->count() + 1;
}

QVariant RamStatisticsTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!m_project) return QVariant();

    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole) return "Completion";

        return QVariant();
    }

    if (section == 0)
    {
        if (role == Qt::DisplayRole) return QString("Project\n" % m_project->shortName());

        return QVariant();
    }

    RamStep *step = qobject_cast<RamStep*>( m_project->steps()->at(section -1 ));

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
    int col = index.column();
    int row = index.row();

    if (row == 0)
    {
        m_project->computeEstimation();
        if (col == 0)
        {
            if (role == Qt::DisplayRole) return QString("Completion: " %
                                                        QString::number( m_project->completionRatio(), 'f', 0) %
                                                        "%\nLateness: " %
                                                        QString::number( (m_project->latenessRatio() -1) * 100, 'f', 0) %
                                                        "%");
            if (role == Qt::ForegroundRole)
            {
                float latenessRatio = m_project->latenessRatio();
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

            if (role == Qt::ToolTipRole) return QString( m_project->name() %
                                                         "\nCompletion: " %
                                                         QString::number( m_project->completionRatio()) %
                                                         "%\nLateness: " %
                                                         QString::number( (m_project->latenessRatio() -1 )* 100, 'f', 0) );

            if (role == Qt::StatusTipRole) return QString( m_project->shortName() % " | " % m_project->name() %
                                                           " | Completion: " %
                                                           QString::number( m_project->completionRatio()) %
                                                           " | Lateness: " %
                                                           QString::number( (m_project->latenessRatio() -1) * 100, 'f', 0) %
                                                           "%");

            if (role == Qt::UserRole) return m_project->completionRatio();
            if (role == Qt::UserRole +1) return m_project->latenessRatio();

            return QVariant();
        }

        if (col == 1)
        {
            // TODO compute in RamSteps / RamProject the ration between assigned half-days vs estimation
            return QVariant();
        }

        return QVariant();
    }

    RamStep *step = qobject_cast<RamStep*>( m_project->steps()->at( row -1 ));

    if (col == 0)
    {
        step->computeEstimation();

        if (role == Qt::DisplayRole) return QString("Completion: " %
                                                    QString::number( step->completionRatio(), 'f', 0) %
                                                    "%\nLateness: " %
                                                    QString::number( (step->latenessRatio() -1) * 100, 'f', 0) %
                                                    "%");
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
                                                     QString::number( step->completionRatio()) %
                                                     "%\nLateness: " %
                                                     QString::number( (step->latenessRatio() -1) * 100, 'f', 0) );

        if (role == Qt::StatusTipRole) return QString( step->shortName() % " | " % step->name() %
                                                       " | Completion: " %
                                                       QString::number( step->completionRatio()) %
                                                       " | Lateness: " %
                                                       QString::number( (step->latenessRatio() -1) * 100, 'f', 0) %
                                                       "%");

        if (role == Qt::UserRole) return step->completionRatio();
        if (role == Qt::UserRole +1) return step->latenessRatio();

        return QVariant();
    }

    if (col == 1)
    {
        // TODO compute in RamSteps / RamProject the ration between assigned half-days vs estimation
        return QVariant();
    }

    return QVariant();
}

void RamStatisticsTable::changeProject(RamProject *project)
{
    beginResetModel();

    m_project = project;

    endResetModel();

    if (!project) return;

    connect( project->steps(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(insertStep(QModelIndex,int,int)));
    connect( project->steps(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(removeStep(QModelIndex,int,int)));
}

void RamStatisticsTable::removeStep(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    // We're removing rows
    beginRemoveRows(QModelIndex(), first+1, last+1);
    endRemoveRows();
}

void RamStatisticsTable::insertStep(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    //We're inserting new rows
    beginInsertRows(QModelIndex(), first+1, last+1);
    // Finished!
    endInsertRows();
}

void RamStatisticsTable::connectEvents()
{
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(changeProject(RamProject*)));
}
