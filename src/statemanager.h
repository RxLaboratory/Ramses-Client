#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <QObject>

#include "duapp/app-utils.h"

class StateManager : public QObject
{
    Q_OBJECT

public:

    enum State {
        Idle = 0,
        Opening = 1,
        WritingDataBase = 2,
        Connecting = 3,
        LoadingDataBase = 4,
        Closing = 5,
        Syncing = 6,
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
    void quit(bool sync = true);
    void restart(bool sync = true, const QString &dbFile = "");
    void forceQuit();
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

class StateHandler
{
public:
    StateHandler(StateManager::State s) {
        freezeState();
        StateManager::i()->setState(s);
    }
    ~StateHandler() {
        if (!_r)
            resetState();
    }

    void resetState() {
        StateManager::i()->setState(_p);
        _r = true;
    }

    void freezeState() {
        _p = StateManager::i()->state();
    }

private:
    StateManager::State _p;
    bool _r = false;
};

#endif // STATEMANAGER_H
