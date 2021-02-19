#ifndef DBISUSPENDER_H
#define DBISUSPENDER_H

#include "dbinterface.h"

class DBISuspender
{
public:
    /**
     * @brief Helper class to suspend the DBInterface. It will be unsuspended by the destructor
     */
    DBISuspender(bool release = false);
    ~DBISuspender();
    void release();
    void suspend();
private:
    bool _wasSuspended;
};

#endif // DBISUSPENDER_H
