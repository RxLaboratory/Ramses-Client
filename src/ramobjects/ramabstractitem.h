#ifndef RAMABSTRACTITEM_H
#define RAMABSTRACTITEM_H

#include "ramstep.h"


class RamState;

/**
 * @brief The RamItem class is the base class for RamShot and RamAsset
 * It handles the status history of the Shot or the Item
 */
class RamAbstractItem : public RamObject
{
    Q_OBJECT
public:
    // OTHER //

    RamAbstractItem(QString shortName, QString name, ObjectType type, RamProject *project );

    RamProject *project() const;
    virtual RamStep::Type productionType() const = 0;

protected:
    RamAbstractItem(QString uuid, ObjectType type);

private:
    void construct();
};

#endif // RAMABSTRACTITEM_H
