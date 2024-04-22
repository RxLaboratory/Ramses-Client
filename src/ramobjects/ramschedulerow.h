#ifndef RAMSCHEDULEROW_H
#define RAMSCHEDULEROW_H

#include "ramobject.h"
#include "ramproject.h"

/**
 * @brief The RamScheduleRow class represents a row in a project schedule.
 * It can be associated to a specific user.
 */
class RamScheduleRow : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    /**
     * @brief Gets an existing Schedule Row.
     * @param uuid The UUID of the row to get.
     * @return The unique instance of the row.
     */
    static RamScheduleRow *get(QString uuid);
    /**
     * @brief
     * Convenience shortcut to cast a RamObject to a Schedule Row.
     * @param o
     * The Object to be casted
     * @param fast
     * When true, performs a `reinterpret_cast`,
     * otherwise performs a `qobject_cast`
     * @return
     * The Schedule Row object.
     */
    static RamScheduleRow *c(RamObject *o, bool fast = true);

    // CONSTRUCTORS //

    /**
     * @brief RamScheduleRow
     * Creates a new row in the schedule of the given project.
     * @param shortName
     * The ID of the row.
     * @param name
     * The name of the row.
     * @param project
     * The project containing this row.
     */
    explicit RamScheduleRow(const QString &shortName, const QString &name, RamProject *project);

    // METHODS //

    /**
     * @brief project
     * The project this row belongs to.
     * @return
     */
    RamProject *project() const;

    /**
     * @brief user
     * The user associated with this row.
     * Can be nullptr.
     * @return
     * The user or nullptr if the row isn't associated with any specific user.
     */
    RamUser *user() const;
    /**
     * @brief setUser
     * Associates a user to this row.
     * @param user
     * The user to associate;
     * use nullptr to dissociate all users.
     */
    void setUser(RamObject *user);

    /**
     * @brief iconName
     * The name of the icon for the row.
     * This is overriden to return a user icon if a user is set.
     * @return
     * The icon file name.
     */
    virtual QString iconName() const override;

    /**
     * @brief name
     * The name of the row.
     * Overriden to include the user name if any.
     * @return
     * The name of the row.
     */
    virtual QString name() const override;

    /**
     * @brief color
     * The color of the row.
     * Overriden to return the user color if any.
     * @return
     * The color of the row.
     */
    virtual QColor color() const override;

public slots:
    /**
     * @brief edit
     * Creates the widget to edit this Schedule Row.
     * @param show
     * Whether to show the widget immediately after it's been created.
     */
    virtual void edit(bool show = true) override;

protected:

    // CONSTRUCTORS & inheritance //

    /**
     * @brief Constructs a Schedule Row instance from existing data.
     * @param uuid The UUID of the row
     */
    RamScheduleRow(const QString &uuid);

    /**
     * @brief The file system path for this object.
     * @return Always an empty string: schedule rows don't have associated folders.
     */
    virtual QString folderPath() const override { return ""; };

    // LUTs and cache //

    /**
     * @brief Schedule Rows Lookup Table by UUID
     */
    static QHash<QString, RamScheduleRow*> m_existingObjects;

    // UI //

    static QFrame *ui_editWidget;

private:
    void construct();
};

#endif // RAMSCHEDULEROW_H
