#ifndef DBTABLEMODEL_H
#define DBTABLEMODEL_H

#include "ramobjectmodel.h"

/**
 * @brief The DBTableModel class handles a list of objects taken from a complete table in the DB
 */
class DBTableModel: public RamObjectModel
{
    Q_OBJECT
public:
    DBTableModel(RamAbstractObject::ObjectType type, QObject *parent = nullptr);

    // Parameters to be set before load()
    /**
     * @brief setFilterKey Sets the key to be used in the data to filter the objects loaded in this table
     * @param key The key, as used in the JSON data
     */
    void setFilterKey(QString key);
    /**
     * @brief addFilterValue Adds a value to be accepted with the filter key
     * @param value The value
     */
    void addFilterValue(QString value);

    /**
     * @brief load Initial loading of the table
     * Call it (at least) once to do the initial loading.
     * Calling it the first time the model is actually needed, as late as possible, may improve performance.
     */
    void load();

private slots:
    // Inserts a new object
    void insertObject(QString uuid, QString table);
    // Removes an object
    void removeObject(QString uuid, QString table);
    // Clear and reload the data
    void reload();
    // Save the order of the objects when the rows have been moved
    void saveOrder(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);

private:
    // This flag is used to check if the table has already been loaded.
    bool m_isLoaded =false;

    // Parameters
    QString m_filterKey;
    QStringList m_filterValues;
};

bool objSorter(const QStringList a, const QStringList b);

#endif // DBTABLEMODEL_H
