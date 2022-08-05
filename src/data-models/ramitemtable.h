#ifndef RAMITEMTABLE_H
#define RAMITEMTABLE_H

#include "ramobjectlist.h"
#include "ramitem.h"

/**
 * @brief The RamItemTable class is the model used to associate shots/assets with their status.
 * It keeps the latest status for each step associated to the corresponding shot/asset
 */
class RamItemTable : public RamObjectList<RamItem*>
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamItemTable *getObject(QString uuid, bool constructNew = false);

    // METHODS //

    RamItemTable(QString shortName, QString name, RamObjectList<RamStep *> *steps, QObject *parent = nullptr, bool isVirtual = false);

    // MODEL REIMPLEMENTATION
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // MODEL EDITING REIMPLEMENTATION
    virtual void insertObject(int i, RamItem *item) override; // Insert Row

protected:
    RamItemTable(QString uuid, QObject *parent = nullptr);

private slots:
    void insertStep(const QModelIndex &parent, int first, int last);
    void removeStep(const QModelIndex &parent, int first, int last);
    void statusChanged(RamItem *item, RamStep *step);

private:

    // METHODS //

    void construct();
    // Connect submodels and relay events
    void connectEvents();
    // Utils
    void connectItem(RamItem *item);
    RamStep *stepAt(int col) const;
    int stepCol(RamStep *step) const;

    // ATTRIBUTES //

    RamObjectList<RamStep*> *m_steps;
};

#endif // RAMITEMTABLE_H
