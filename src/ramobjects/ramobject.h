#ifndef RAMOBJECT_H
#define RAMOBJECT_H

#include <QObject>
#include <QMap>
#include <QSettings>
#include <QStringBuilder>
#include <QMetaEnum>

#include "ramabstractobject.h"

class ObjectDockWidget;
class ObjectEditWidget;

class RamObject : public QObject, public RamAbstractObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    /**
     * @brief object Gets the existing object
     * @param uuid The object uuid
     * @param constructNew When true, will construct a new object if it's not already available
     * @return the object or nullptr if it doesn't exist yet
     */
    static RamObject *getObject(QString uuid, bool constructNew = false);

    // METHODS //

    /**
     * @brief RamObject constructs a new object and adds it in the database
     * @param shortName
     * @param name
     */
    RamObject(QString shortName, QString name, ObjectType type, QObject *parent = nullptr, bool isVirtual = false);

    // reimplemented to disconnect signals
    virtual void remove() override;

    virtual QString filterUuid() const { return QString(); };

public slots:
    virtual void edit(bool s = true) { Q_UNUSED(s) };

signals:
    void dataChanged(RamObject *, QJsonObject);
    void removed(RamObject *);
    void restored(RamObject *);

protected:
    // METHODS //

    /**
     * @brief RamObject constructs an object using its UUID and type. The UUID must exist in the database.
     * @param uuid
     * @param parent
     */
    RamObject(QString uuid, ObjectType type, QObject *parent = nullptr);

    void emitDataChanged(QJsonObject data) override;
    void emitRemoved() override;
    void emitRestored() override;

    /**
     * @brief setEditWidget sets the widget used to edit this object in the UI
     * @param w
     */
    void setEditWidget(ObjectEditWidget *w );
    /**
     * @brief showEdit shows the edit widget
     * @param title
     */
    void showEdit(QString title = "");

    // ATTRIBUTES //

    QString m_icon = ":/icons/asset";
    UserRole m_editRole = Admin;
    bool m_editable = true;

    QFrame *ui_editWidget = nullptr;

private:
    void construct(QObject *parent = nullptr);
    ObjectDockWidget *m_dockWidget = nullptr;
};

#endif // RAMOBJECT_H
