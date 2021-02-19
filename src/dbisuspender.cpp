#include "dbisuspender.h"

DBISuspender::DBISuspender(bool release)
{
    _wasSuspended = DBInterface::instance()->isSuspended();
    if (release) DBInterface::instance()->suspend(false);
    DBInterface::instance()->suspend(true);
}

DBISuspender::~DBISuspender()
{
    DBInterface::instance()->suspend(_wasSuspended);
}

void DBISuspender::release()
{
    DBInterface::instance()->suspend(false);
}

void DBISuspender::suspend()
{
    DBInterface::instance()->suspend(true);
}
