#ifndef RAMOBJECTCOMBOBOX_H
#define RAMOBJECTCOMBOBOX_H

#include "ramobjectmodel.h"
#include <QComboBox>

class RamObjectComboBox : public QComboBox
{
    Q_OBJECT
public:
    RamObjectComboBox(RamObject::ObjectType type, QWidget *parent = nullptr);
    RamObjectComboBox(RamObject::ObjectType type, QString filterName, QWidget *parent = nullptr);

    void setList(RamObjectModel *list);

    RamObject *currentObject();
    QString currentUuid();
    void setObject(QString uuid);
    void setObject(RamObject *obj);
    RamObject *objectAt(int i);
    QString uuidAt(int i);

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

    bool m_isFilterBox = false;
    QString m_filterName;
    RamObject::ObjectType  m_type;

    QString m_resettingUuid;
    bool m_resetting = false;
};

#endif // RAMOBJECTCOMBOBOX_H
