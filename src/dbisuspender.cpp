#include "dbisuspender.h"

DBISuspender::DBISuspender()
{
    DBInterface::instance()->suspend(true);
}

DBISuspender::~DBISuspender()
{
    DBInterface::instance()->suspend(false);
}
