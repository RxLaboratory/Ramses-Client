#ifndef DUQFCONNECTIONMANAGER_H
#define DUQFCONNECTIONMANAGER_H

#include <QObject>

#include "duqf-nodeview/duqfnode.h"
#include "duqf-nodeview/duqfconnection.h"

class DuQFConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit DuQFConnectionManager(DuQFConnector *output, DuQFConnector *input, DuQFConnection *connection, QObject *parent = nullptr);

signals:

private slots:
    void outputMoved();
    void inputMoved();

private:
    DuQFConnector *m_input;
    DuQFConnector *m_output;
    DuQFConnection *m_connection;
};

#endif // DUQFCONNECTIONMANAGER_H
