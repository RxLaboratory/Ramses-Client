#ifndef RAMITEMFILTERMODEL_H
#define RAMITEMFILTERMODEL_H

#include "ramobjectfiltermodel.h"
#include "ramitem.h"

/**
 * @brief The RamItemFilterModel class is used to filters items according to current state, step or assigned user.
 * It also sorts the items according to: their completion ratio, their estimation, their time spent, their difficulty, their name or their ID (or default)
 */
class RamItemFilterModel : public RamObjectFilterModel
{
    Q_OBJECT
public:
    enum SortMode {
        Default = 1,
        ShortName = 2,
        Name = 3,
        Difficulty = 4,
        TimeSpent = 5,
        Estimation = 6,
        Completion = 7
    };

    explicit RamItemFilterModel(QObject *parent = nullptr);

    void freeze();
    void unFreeze();

    void useFilters(bool use = true);

    void hideUser(RamUser *u);
    void showUser(RamUser *u);
    void clearUsers();
    void showUnassigned(bool show);

    void hideState(RamState *s);
    void showState(RamState *s);
    void clearStates();

    void setStepType(RamStep::Type t);
    void hideStep(RamStep *s);
    void showStep(RamStep *s);
    void showAllSteps();

    SortMode sortMode() const;
    void setSortMode(SortMode newSortMode);

public slots:
    void resort(int col, Qt::SortOrder order = Qt::AscendingOrder);
    void unsort();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool filterAcceptsColumn(int sourceRow, const QModelIndex &sourceParent) const override;

    QList<RamState*> m_states;
    QList<RamUser*> m_users;
    QList<RamStep*> m_hiddenSteps;
    bool m_showUnassigned = true;
    RamStep::Type m_stepType = RamStep::All;

private:
    /**
     * @brief step Get a step given a source column
     * @param column The column of the step in the source model
     * @return The RamStep* or nullptr if the step is filtered
     */
    RamStep *step( int column ) const;
    bool m_userFilters = false;

    bool m_frozen = false;

    SortMode m_sortMode = Default;
};

#endif // RAMITEMFILTERMODEL_H
