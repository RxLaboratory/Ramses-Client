#ifndef RAMITEMTABLE_H
#define RAMITEMTABLE_H

#include "ramobjectlist.h"
#include "ramitem.h"

/**
 * @brief The RamItemTable class is the model used to associate shots/assets with their status.
 * It keeps the latest status for each step associated to the corresponding shot/asset
 */
class RamItemTable : public RamObjectList
{
    Q_OBJECT
public:
    static RamItemTable *get(QString uuid);
    static RamItemTable *c(RamObjectList *o);

    // METHODS //

    RamItemTable(QString shortName, QString name, ObjectType type, QObject *parent = nullptr, DataListMode mode = ListObject);
    RamItemTable(QString uuid, QObject *parent = nullptr);

    // MODEL REIMPLEMENTATION
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;


private slots:
    void removeItem(const QModelIndex &parent, int first, int last);
    void statusChanged(RamItem *item, RamStep *step);

    // Used to update the list of steps
    // when the first item is inserted.
    void inserted(const QModelIndex &parent, int first, int last);

private:

    // METHODS //
    void connectEvents();
    void updateStepList() const;
    // Utils
    RamStep *stepAt(int col) const;
    int stepCol(RamStep *step) const;
};

#endif // RAMITEMTABLE_H
