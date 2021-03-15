#ifndef DUQFCONNECTION_H
#define DUQFCONNECTION_H

#include <QObject>

#include "duqfslot.h"
#include "duqfconnector.h"

/**
 * @brief The DuQFConnection class represents a connexion between two slots and transmits the move signals to the DuQFConnector
 */
class DuQFConnection : public QObject
{
    Q_OBJECT
public:
    explicit DuQFConnection(DuQFSlot *output, DuQFSlot *input, DuQFConnector *connector, QObject *parent = nullptr);

    DuQFSlot *input() const;
    void setInput(DuQFSlot *input);

    DuQFSlot *output() const;
    void setOutput(DuQFSlot *output);

signals:

private slots:
    void outputMoved();
    void inputMoved();

private:
    DuQFSlot *m_input;
    DuQFSlot *m_output;
    DuQFConnector *m_connector;
};

#endif // DUQFCONNECTIONMANAGER_H
