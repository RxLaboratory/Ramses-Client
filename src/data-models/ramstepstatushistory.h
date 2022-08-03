#ifndef RAMSTATUSHISTORY_H
#define RAMSTATUSHISTORY_H

#include "data-models/ramobjectlist.h"

class RamStatus;
class RamStep;
class RamItem;

/**
 * @brief The RamStepStatusHistory class is a list of RamStatus for a specific step and item
 * It is just a simple heritance of RamObjectList used to sort the RamStatus according to their date,
 * and it also keeps a pointer to the corresponding item and step.
 * Furthermore, it implements and 'edit' method, similar to RamObject::edit() which shows a dock widget listing all the RamStatus
 */
class RamStepStatusHistory : public RamObjectList<RamStatus*>
{
    Q_OBJECT
public:
    RamStepStatusHistory(RamStep *step, RamItem *item);
    RamItem *item() const;
    RamStep *step() const;

    // MODEL REIMPLEMENTATION
    virtual void sort(int column = 0, Qt::SortOrder order = Qt::AscendingOrder) override;

public slots:
    void edit(bool show = true);

private:
    RamStep *m_step;
    RamItem *m_item;

    QFrame *ui_editWidget = nullptr;
};

bool statusSorter(RamObject *a, RamObject *b);

#endif // RAMSTATUSHISTORY_H
