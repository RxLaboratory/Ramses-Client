#ifndef RAMSCHEDULEENTRY_H
#define RAMSCHEDULEENTRY_H

#include "ramobject.h"
#include "ramschedulerow.h"

class RamScheduleEntry : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    /**
     * @brief Gets an existing Schedule Entry.
     * @param uuid The UUID of the entry to get.
     * @return The unique instance of the entry.
     */
    static RamScheduleEntry *get(QString uuid);
    /**
     * @brief
     * Convenience shortcut to cast a RamObject to a Schedule Entry.
     * @param o
     * The Object to be casted
     * @param fast
     * When true, performs a `reinterpret_cast`,
     * otherwise performs a `qobject_cast`
     * @return
     * The Schedule Entry object.
     */
    static RamScheduleEntry *c(RamObject *o, bool fast = true);

    /**
     * @brief validateData
     * Validates the data to make sure the entry is valid.
     * This method is used by the data interface
     * before loading entries.
     * @param data
     * @return
     */
    static bool validateData(const QString &data);

    // CONSTRUCTORS //

    /**
     * @brief RamScheduleEntry
     * Constructs a new Schedule Entry.
     * @param name
     * The name (title) of the entry.
     * @param date
     * The date of the entry.
     * @param row
     * The containing row of the entry.
     */
    explicit RamScheduleEntry(const QString &name, const QDate &date, RamScheduleRow *row);

    // METHODS //

    /**
     * @brief date
     * The date of the entry.
     * @return
     */
    QDate date() const;
    /**
     * @brief setDate
     * Moves the entry to a new date.
     * @param date
     * The new date.
     */
    void setDate(const QDate &date);

    /**
     * @brief row
     * The row containing this entry.
     * @return
     */
    RamScheduleRow *row() const;
    /**
     * @brief setRow
     * Moves this entry to another newRow.
     * @param newRow
     * The newRow where to move the entry.
     */
    void setRow(RamScheduleRow *newRow);

    /**
     * @brief step
     * A Step associated to the schedule entry
     * @return
     * Can be nullptr
     */
    RamStep *step() const;
    /**
     * @brief setStep
     * Changes the step associated to the entry
     * @param newStep
     * The new step.
     * Set to nullptr to dissociate all steps from this entry.
     */
    void setStep(RamObject *newStep);

    /**
     * @brief iconName
     * The name of the icon for the entry.
     * This is overriden to return the step icon if a step is set.
     * @return
     * The icon file name.
     */
    virtual QString iconName() const override;

    /**
     * @brief color
     * The color of the entry.
     * This is overriden to return the step color if a step is set.
     * @return
     * The color of the entry.
     */
    virtual QColor color() const override;

    /**
     * @brief roleData
     * Gets some data for this entry.
     * @param role
     * The role to get
     * @return
     * The data
     */
    virtual QVariant roleData(int role) const override;

protected:

    // CONSTRUCTORS & inheritance //

    /**
     * @brief Constructs a Schedule Entry instance from existing data.
     * @param uuid The UUID of the entry
     */
    RamScheduleEntry(const QString &uuid);

    /**
     * @brief The file system path for this object.
     * @return Always an empty string: schedule entries don't have associated folders.
     */
    virtual QString folderPath() const override { return ""; };


    // LUTs and cache //

    /**
     * @brief Schedule Entries Lookup Table by UUID
     */
    static QHash<QString, RamScheduleEntry*> m_existingObjects;

private:
    void construct();
};

#endif // RAMSCHEDULEENTRY_H
