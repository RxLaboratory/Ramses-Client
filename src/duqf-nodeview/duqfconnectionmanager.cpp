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

DuQFConnection * DuQFConnectionManager::addConnection(DuQFSlot *output, DuQFSlot *input, DuQFConnector *connector)
{
    // If input and output are the same
    if (input == output)
    {
        connector->remove();
        return nullptr;
    }

    // If one slot is missing
    if (!input || !output)
    {
        connector->remove();
        return nullptr;
    }

    // Won't connect two inputs or two outputs together
    if (input->slotType() && input->slotType() == output->slotType())
    {
        connector->remove();
        return nullptr;
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
            connector->remove();
            return nullptr;
        }
    }

    DuQFConnection *co =  new DuQFConnection(output, input, connector, this);
    addConnection( co );
    return co;
}

QList<DuQFConnection *> DuQFConnectionManager::connections() const
{
    return m_connections;
}

void DuQFConnectionManager::removeConnection()
{
    DuQFConnection *c = (DuQFConnection*)sender();
    emit connectionRemoved(c);
    m_connections.removeAll(c);
}
