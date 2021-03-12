#include "duqfconnectionmanager.h"

DuQFConnectionManager::DuQFConnectionManager(DuQFConnector *output, DuQFConnector *input, DuQFConnection *connection, QObject *parent) : QObject(parent)
{
    m_output = output;
    m_input = input;
    m_connection = connection;

    QGraphicsObject *outputParent = (QGraphicsObject*)output->parentItem();
    QGraphicsObject *inputParent = (QGraphicsObject*)input->parentItem();

    connect(outputParent, &QGraphicsObject::xChanged, this, &DuQFConnectionManager::outputMoved);
    connect(outputParent, &QGraphicsObject::yChanged, this, &DuQFConnectionManager::outputMoved);
    connect(inputParent, &QGraphicsObject::xChanged, this, &DuQFConnectionManager::inputMoved);
    connect(inputParent, &QGraphicsObject::yChanged, this, &DuQFConnectionManager::inputMoved);
}

void DuQFConnectionManager::outputMoved()
{
    m_connection->setFrom( m_output->scenePos() );
}

void DuQFConnectionManager::inputMoved()
{
    m_connection->setTo( m_input->scenePos() );
}
