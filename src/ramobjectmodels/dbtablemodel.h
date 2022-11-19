#ifndef DBTABLEMODEL_H
#define DBTABLEMODEL_H

#include "ramabstractobjectmodel.h"

#include "ramobject.h"

/**
 * @brief The DBTableModel class handles a list of objects taken from a complete table in the DB
 */
class DBTableModel: public RamAbstractObjectModel
{
    Q_OBJECT
public:

    // === CONSTRUCTOR ===

    DBTableModel(RamAbstractObject::ObjectType type, QObject *parent = nullptr);

    // === Parameters to be set before load() ===

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
     * @brief setLookUpKey Sets a key of the JSON Data to be used for fast look ups
     * The default is to use the shortName
     * @param newLookUpKey The key
     */
    void setLookUpKey(const QString &newLookUpKey);

    // === Other Public Methods ===

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

    // Convenience methods
    // An object by its shortname, or name
    virtual RamObject *search(QString searchString) const override;
    // Objects by its lookup key
    QList<RamObject *> lookUp(QString lookUpValue);

    // Clear the model
    virtual void clear() override;

private slots:
    // Inserts a single object
    void insertObject(QString uuid, QString data, QString table);
    // Removes a single object
    void removeObject(QString uuid, QString table);
    // Clear and reload the data
    void reload();
    // Checks and changes the data
    void changeData(QString uuid, QString data);

private:

    // === METHODS ===

    // Edit structure
    void insertObjects(int row, QVector<QStringList> data, QString table);
    void removeObjects(QStringList uuids, QString table = "");

    // Utils
    // Checks if this object uuid belongs to this model
    virtual bool checkFilters(QString data) const;
    // Gets the order from the data
    int getOrder(QString data);
    // Gets the lookUp key value from the data
    QString getLookUpValue(QString data);

    // === ATTRIBUTES ===

    // Data
    // Fast LookUp table
    QMultiHash<QString, QString> m_lookUpTable;

    // This flag is used to check if the table has already been loaded.
    bool m_isLoaded = false;

    // Parameters
    QString m_filterKey;
    QStringList m_filterValues;
    QString m_lookUpKey = "shortName";
};

bool objSorter(const QStringList a, const QStringList b);

#endif // DBTABLEMODEL_H
