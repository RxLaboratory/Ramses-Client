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
    void quit(bool sync = true);
    void restart(bool sync = true, const QString &project = "");
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
