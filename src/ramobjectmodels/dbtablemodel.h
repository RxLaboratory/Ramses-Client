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
    void setFilterKey(QString key);
    void addFilterValue(QString value);

    // Call it (at least) once to do the initial loading.
    // Does not reload if it's already been loaded, call reload() instead.
    // Calling it the first time the model is needed, as late as possible, may improve performance.
    void load();

private slots:
    void insertObject(QString uuid, QString table);
    void removeObject(QString uuid, QString table);
    void reload();
    void saveOrder(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);

private:
    // Initial loading
    // Change: must be explicitly called
    // to allow setting filters before
    // It is now the public method load()
    // void construct();
    bool m_isLoaded =false;

    // Parameters
    QString m_filterKey;
    QStringList m_filterValues;
};

bool objSorter(const QStringList a, const QStringList b);

#endif // DBTABLEMODEL_H
