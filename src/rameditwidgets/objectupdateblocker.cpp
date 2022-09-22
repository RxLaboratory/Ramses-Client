#include "objectupdateblocker.h"

ObjectUpdateBlocker::ObjectUpdateBlocker(RamAbstractObject *o)
{
    if (o)
    {
        m_wasVirtual = o->isVirtualObject();
        o->setVirtualObject(true);
        m_obj = o;
    }
}

ObjectUpdateBlocker::~ObjectUpdateBlocker()
{
    if (m_obj) m_obj->setVirtualObject(m_wasVirtual);
}
