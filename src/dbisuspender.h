#ifndef DBISUSPENDER_H
#define DBISUSPENDER_H

#include "dbinterface.h"

class DBISuspender
{
public:
    /**
     * @brief Helper class to suspend the DBInterface. It will be unsuspended by the destructor
     */
    DBISuspender();
    ~DBISuspender();
};

#endif // DBISUSPENDER_H
