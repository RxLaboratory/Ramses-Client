#include "duqfconnectionmanager.h"

DuQFConnectionManager::DuQFConnectionManager(QObject *parent) : QObject(parent)
{

}

void DuQFConnectionManager::addConnection(DuQFConnection *connection)
{
    connect(connection, &DuQFConnection::removed, this, &DuQFConnectionManager::removeConnection);
    m_connections << connection;
}

bool DuQFConnectionManager::addConnection(DuQFSlot *output, DuQFSlot *input, DuQFConnector *connector)
{
    // If one slot is missing
    if (!input || !output)
    {
        delete connector;
        return false;
    }

    // Won't connect two inputs or two outputs together
    if (input->slotType() && input->slotType() == output->slotType())
    {
        delete connector;
        return false;
    }

    // Check if a connection already exists with the same input/output
    foreach(DuQFConnection *c, m_connections)
    {
        bool ok = true;
        if (input == c->input() && output == c->output()) ok = false;
        if (input == c->input() && input->isSingleConnection()) ok =  false;
        if (output  == c->output() && output->isSingleConnection()) ok = false;
        if (!ok)
        {
            delete connector;
            return false;
        }
    }

    addConnection( new DuQFConnection(output, input, connector, this) );
    return true;
}

QList<DuQFConnection *> DuQFConnectionManager::connections() const
{
    return m_connections;
}

void DuQFConnectionManager::removeConnection()
{
    DuQFConnection *c = (DuQFConnection*)sender();
    m_connections.removeAll(c);
}
