#include "duqfconnection.h"

DuQFConnection::DuQFConnection(DuQFSlot *output, DuQFSlot *input, DuQFConnector *connector, QObject *parent) : QObject(parent)
{
    m_output = output;
    m_input = input;
    m_connector = connector;

    DuQFNode *outputParent = (DuQFNode*)output->parentItem();
    DuQFNode *inputParent = (DuQFNode*)input->parentItem();

    outputParent->addChildNode(inputParent);
    inputParent->addParentNode(outputParent);

    connect(outputParent, &QGraphicsObject::xChanged, this, &DuQFConnection::outputMoved);
    connect(outputParent, &QGraphicsObject::yChanged, this, &DuQFConnection::outputMoved);
    connect(inputParent, &QGraphicsObject::xChanged, this, &DuQFConnection::inputMoved);
    connect(inputParent, &QGraphicsObject::yChanged, this, &DuQFConnection::inputMoved);
    connect(output, &DuQFSlot::removed, this, &DuQFConnection::remove);
    connect(input, &DuQFSlot::removed, this, &DuQFConnection::remove);
    connect(connector, &DuQFConnector::removed, this, &DuQFConnection::remove);
}

DuQFSlot *DuQFConnection::input() const
{
    return m_input;
}

void DuQFConnection::setInput(DuQFSlot *input)
{
    m_input = input;
}

DuQFSlot *DuQFConnection::output() const
{
    return m_output;
}

void DuQFConnection::setOutput(DuQFSlot *output)
{
    m_output = output;
}

void DuQFConnection::remove()
{
    if (m_removing) return;
    m_removing = true;
    m_connector->remove();

    DuQFNode *outputParent = (DuQFNode*)m_output->parentItem();
    DuQFNode *inputParent = (DuQFNode*)m_input->parentItem();

    outputParent->removeChildNode(inputParent);
    inputParent->removeParentNode(outputParent);

    emit removed();
    deleteLater();
}

void DuQFConnection::outputMoved()
{
    m_connector->setFrom( m_output->scenePos() );
}

void DuQFConnection::inputMoved()
{
    m_connector->setTo( m_input->scenePos() );
}


