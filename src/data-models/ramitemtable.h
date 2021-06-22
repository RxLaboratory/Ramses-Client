#ifndef RAMITEMTABLE_H
#define RAMITEMTABLE_H

#include "ramobjectlist.h"
#include "ramstepstatushistory.h"
#include "ramitem.h"

class RamItemTable : public RamObjectList
{
    Q_OBJECT
public:
    explicit RamItemTable(RamStep::Type productionType, RamObjectList *steps, QObject *parent = nullptr);
    RamItemTable(RamStep::Type productionType, RamObjectList *steps, QString shortName, QString name = "", QObject *parent = nullptr);

    const RamStep::Type &productionType() const;

    // MODEL REIMPLEMENTATION
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // MODEL EDITING REIMPLEMENTATION
    virtual void insertObject(int i, RamObject *obj) override; // Insert Row

private slots:
    // TODO header data on step insert and change
    void insertStep(const QModelIndex &parent, int first, int last);
    void removeStep(const QModelIndex &parent, int first, int last);
    void statusChanged(RamItem *item, RamStep *step);

private:
    RamObjectList *m_steps;
    RamStep::Type m_productionType;

    // Connect submodels and relay events
    void connectEvents();

    // Utils
    void connectItem(RamItem *item);
    int stepCol(RamStep *step);
};

#endif // RAMITEMTABLE_H
