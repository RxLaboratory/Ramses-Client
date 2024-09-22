#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <QObject>

#include "duapp/app-utils.h"

class StateManager : public QObject
{
    Q_OBJECT

public:

    enum State {
        Unknown = -1,
        Idle = 0,
        Opening = 1,
        Connecting = 2,
        Closing = 3,
        Syncing = 4,
    };

    /**
     * @brief instance returns the unique instance of StateManager.
     * @return the instance.
     */
    static StateManager *i();

    State state() const;
    State previousState() const { return m_previousState; }

    QString title() const { return m_title; }
    void setTitle(const QString &newTitle);

    QString text() const {  return m_text; }
    void setText(const QString &newText);

    void autoDisconnect(QMetaObject::Connection co) { _singleStateConnexion = co; }

signals:
    void stateChanged(StateManager::State);
    void titleChanged(QString title);
    void textChanged(QString text);

public slots:
    void quit(bool sync = true, int delay = 0);
    void restart(bool sync = true, const QString &dbFile = "");
    void forceQuit();
    void setState(StateManager::State newState);
    void setState(StateManager::State newState, const QString &title);
    void setTempState(StateManager::State tempState);
    void keepTempState();

    // Helpers
    void setIdle() { setState(Idle, tr("Ready.")); };

protected:
    static StateManager *_instance;

private:
    /**
     * @brief This is a singleton, private constructor
     * @param parent
     */
    StateManager(QObject *parent = nullptr);

    void emitStateChanged();

    QString m_title = "";
    QString m_text = "";

    State m_state = Idle;
    State m_previousState = Idle;
    State m_tempState = Unknown;
    DuApplication *m_app;
    QMetaObject::Connection _singleStateConnexion;
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged FINAL)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged FINAL)
};



class StateChanger
{
public:
    StateChanger(StateManager::State s): _s(s) {
        _p = StateManager::i()->state();
        StateManager::i()->setTempState(_s);
    }
    ~StateChanger() {
        resetState();
    }

    StateManager::State previousState() const {
        return _p;
    }

    void resetState() {
        StateManager::i()->setTempState(StateManager::Unknown);
    }
    void freezeState(const QString &title = "") {
        StateManager::i()->setState(_s, title);
        resetState();
    }
    void freezeState(QMetaObject::Connection co, const QString &title = "") {
        freezeState(title);
        StateManager::i()->autoDisconnect(co);
    }
    void freezeForIdle(QMetaObject::Connection co, const QString &title = "") {
        if (_p != StateManager::Idle)
            return;
        freezeState(co, title);
    }

private:
    StateManager::State _s;
    StateManager::State _p;
};

#endif // STATEMANAGER_H
