#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include "duqf-app/duapplication.h"
#include <QObject>

class StateManager : public QObject
{
    Q_OBJECT

public:

    enum State {
        Idle = 0,
        WritingDataBase = 1,
        Connecting = 2,
        LoadingDataBase = 3,
    };

    /**
     * @brief instance returns the unique instance of StateManager.
     * @return the instance.
     */
    static StateManager *i();

    State state() const;
    bool isDBBusy() const;

signals:
    void stateChanged(StateManager::State);

public slots:
    void logout(QString reopenFile = "");
    void quit(bool sync = true);
    void restart(bool sync = true);
    void open(QString filePath);
    void setState(StateManager::State newState);

protected:
    static StateManager *_instance;

private:
    /**
     * @brief This is a singleton, private constructor
     * @param parent
     */
    StateManager(QObject *parent = nullptr);

    State m_state = Idle;
    State m_previousState = Idle;
    DuApplication *m_app;
};

#endif // STATEMANAGER_H
