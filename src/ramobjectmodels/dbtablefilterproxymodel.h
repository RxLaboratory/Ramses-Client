#ifndef DBTABLEFILTERPROXYMODEL_H
#define DBTABLEFILTERPROXYMODEL_H

#include "dbtablemodel.h"
#include "ramabstractdatamodel.h"

class DBTableFilterProxyModel : public QSortFilterProxyModel, RamAbstractDataModel
{
public:
    explicit DBTableFilterProxyModel(DBTableModel *model, QObject *parent = nullptr);

    // === Filter Methods ===

    /**
     * @brief addFilterValue Adds a value to be accepted with the filter key
     * @param value The value
     */
    void addFilterValue(QString key, QString value);
    void addFilterValues(QString key, QStringList values);

    /**
     * @brief load Initial loading of the table
     * Call it (at least) once to do the initial loading.
     * Calling it the first time the model is actually needed, as late as possible, may improve performance.
     */
    void load();

    // Support move rows
    virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

    // === Data Access ===

    // Number of objects
    virtual int count() const override;

    // A Uuid
    virtual QString getUuid(int row) const override;
    // An object by row
    virtual RamObject *get(int row) const override;

    // An object by its shortname, or name
    virtual RamObject *search(QString searchString) const override;
    // Objects by their lookup key
    virtual QSet<RamObject *> lookUp(QString lookUpValue) const override;
    // This is faster if you know the object doesn't need to be filtered
    QSet<RamObject *> lookUpNoFilter(QString lookUpValue) const;

    // All the uuids
    virtual QVector<QString> toVector() const override;
    virtual QStringList toStringList() const override;

    // Check if contains
    virtual bool contains(QString uuid) const override;

    // The type of objects
    virtual RamObject::ObjectType type() const override;

    virtual int uuidRow(QString uuid) const override;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    // === Utils ===

    bool checkFilters(RamObject *obj) const;
    bool filterAcceptsObject(RamObject *obj) const;
    bool filterAcceptsUuid(QString uuid) const;

    // === Data ===

    DBTableModel *m_tableModel;

    // === Filters ===

    QHash<QString, QStringList> m_filters;
};

#endif // DBTABLEFILTERPROXYMODEL_H
