
#ifndef DBTABLEMODEL_H
#define DBTABLEMODEL_H

#include "ramabstractobjectmodel.h"

/**
 * @brief The DBTableModel class handles a list of objects taken from a complete table in the DB
 */
class DBTableModel: public RamAbstractObjectModel
{
    Q_OBJECT
public:

    // === CONSTRUCTOR ===

    DBTableModel(RamAbstractObject::ObjectType type, bool projectTable, bool sorted = false, QObject *parent = nullptr);

    // === Filter Methods ===

    /**
     * @brief addFilterValue Adds a value to be accepted with the filter key
     * @param value The value
     */
    void addFilterValue(QString key, QString value);
    void addFilterValues(QString key, QStringList values);

    /**
     * @brief setRejectInvalidData
     * If set to true, objects with invalid data
     * can't be inserted in this table.
     * @param r
     */
    void setRejectInvalidData(bool r = true);

    /**
     * @brief load Initial loading of the table
     * Call it (at least) once to do the initial loading.
     * Calling it the first time the model is actually needed, as late as possible, may improve performance.
     */
    void load();

    // === QAbstractTableModel Reimplementation ===

    // Data access
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    // Support move rows
    virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

protected:
    QStringList m_uniqueDataKeys;

protected slots:
    // Inserts a single object
    void insertObject(const QString &uuid, const QString &data, const QString &modificationDate, const QString &table);
    // Removes a single object
    void removeObject(QString uuid, QString table);
    // Clear and reload the data
    void reload();
    // Checks and changes the data
    void changeData(const QString &uuid, const QString &data, const QString &modificationDate, const QString &table = "");

private:

    // === METHODS ===

    // Edit structure
    void insertObjects(int row, const QVector<QStringList> &data, const QString &table = "", bool silent = false);
    void removeObjects(QStringList uuids, const QString &table = "");

    // Gets the order from the data
    int getOrder(QString data);

    // Checks the filters
    bool checkFilters(QString data) const;
    bool validate(const QString &data, const QString &table);
    // Checks if this obj may be inserted
    // Returns a uuid to be removed from
    // the table after inserting the object
    // Or an empty string if there's nothing to remove
    QString checkUnique(const QString &uuid, const QString &data, const QString &modifiedDate);

    // Save the order in the db
    void saveOrder() const;

    void moveObject(int from, int to);

    // === Filters ===

    QHash<QString, QStringList> m_filters;
    bool m_rejectInvalidData = true;

    // === ATTRIBUTES ===

    // This flag is used to check if the table has already been loaded.
    bool m_isLoaded = false;
    bool m_isProjectTable = false;
    bool m_userOrder = false;
};

bool objSorter(const QStringList a, const QStringList b);

#endif // DBTABLEMODEL_H
