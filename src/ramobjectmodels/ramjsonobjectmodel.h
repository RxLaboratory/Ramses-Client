#ifndef RAMJSONOBJECTMODEL_H
#define RAMJSONOBJECTMODEL_H

#include <QAbstractTableModel>
#include <QJsonObject>

class RamJsonObjectModel : public QAbstractListModel
{
public:
    explicit RamJsonObjectModel(QObject *parent = nullptr);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent)
        return _objects.count();
    }

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // Direct Access

    QVector<QJsonObject> objects() const { return _objects; };
    QJsonObject object(int row) const {
        Q_ASSERT(row >= 0 && row < _objects.count());
        return _objects.at(row);
    };

private:
    QVector<QJsonObject> _objects;
};

#endif // RAMJSONOBJECTMODEL_H
