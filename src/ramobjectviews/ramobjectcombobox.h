#ifndef RAMOBJECTCOMBOBOX_H
#define RAMOBJECTCOMBOBOX_H

#include "duqf-widgets/ducombobox.h"
#include "ramobjectsortfilterproxymodel.h"

class RamObjectComboBox : public DuComboBox
{
    Q_OBJECT
public:
    RamObjectComboBox(QWidget *parent = nullptr);

    void setObjectModel(QAbstractItemModel *model);
    void setObjectModel(QAbstractItemModel *model, QString FilterListName);

    void setSortMode(RamObject::DataRole mode);

    RamObject *currentObject();
    QString currentUuid();
    void setObject(QString uuid);
    void setObject(RamObject *obj);
    RamObject *objectAt(int i);
    QString uuidAt(int i);
    QModelIndex modelIndex(int i);

    void beginReset();
    void endReset();

    void showPopup() override;
    void hidePopup() override;

signals:
    void popupShown();
    void popupHidden();

    void currentObjectChanged(RamObject*);
    void currentUuidChanged(QString);
    void objectActivated(RamObject*);
    void uuidActivated(QString);

private slots:
    void currentObjectIndexChanged(int i);
    void objectIndexActivated(int i );

private:
    void setupUi();
    void connectEvents();

    QString m_resettingUuid;
    bool m_resetting = false;

    RamObjectSortFilterProxyModel *m_objects;

};

#endif // RAMOBJECTCOMBOBOX_H
