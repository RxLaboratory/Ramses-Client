#include "duqfconnectionmanager.h"

DuQFConnectionManager::DuQFConnectionManager(QObject *parent) : QObject(parent)
{

}

void DuQFConnectionManager::addConnection(DuQFConnection *connection)
{
    connect(connection, &DuQFConnection::removed, this, &DuQFConnectionManager::removeConnection);
    m_connections << connection;
    emit newConnection(connection);
}

DuQFConnection * DuQFConnectionManager::addConnection(DuQFSlot *from, DuQFSlot *to)
{
    // If input and output are the same
    if (to == from) return nullptr;

    // If one slot is missing
    if (!to || !from) return nullptr;

    // Won't connect two inputs or two outputs together
    if (to->slotType() && to->slotType() == from->slotType()) return nullptr;

    // Check which one is the input
    DuQFSlot *input;
    DuQFSlot *output;
    if (to->slotType() == DuQFSlot::Input) {
        input = to;
        output = from;
    }
    else {
        input = from;
        output = to;
    }

    // Check if a connection already exists with the same input/output
    foreach(DuQFConnection *c, m_connections)
    {
        if (input == c->input() && output == c->output()) return c;
        if (input == c->input() && output->isSingleConnection()) return nullptr;
        if (output  == c->output() && output->isSingleConnection()) return nullptr;
    }

    DuQFConnection *co =  new DuQFConnection(output, input, this);
    addConnection( co );
    return co;
}

QSet<DuQFConnection *> DuQFConnectionManager::connections() const
{
    return m_connections;
}

void DuQFConnectionManager::removeConnection()
{
    DuQFConnection *c = (DuQFConnection*)sender();
    emit connectionRemoved(c);
    m_connections.remove(c);
}
