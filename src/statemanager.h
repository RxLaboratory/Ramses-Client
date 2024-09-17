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
    State previousState() const { return m_previousState; }
    bool isDBBusy() const;

    QString title() const { return m_title; }
    void setTitle(const QString &newTitle);

    QString text() const {  return m_text; }
    void setText(const QString &newText);

signals:
    void stateChanged(StateManager::State);
    void titleChanged(QString title);
    void textChanged(QString text);

public slots:
    void quit(bool sync = true);
    void restart(bool sync = true, const QString &dbFile = "");
    void forceQuit();
    void setState(StateManager::State newState);
    void setTempState(StateManager::State tempState);
    void keepTempState();

protected:
    static StateManager *_instance;

private:
    /**
     * @brief This is a singleton, private constructor
     * @param parent
     */
    StateManager(QObject *parent = nullptr);

    QString m_title = "";
    QString m_text = "";

    State m_state = Idle;
    State m_previousState = Idle;
    State m_tempState = Unknown;
    DuApplication *m_app;
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged FINAL)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged FINAL)
};

class StateChanger
{
public:
    StateChanger(StateManager::State s) {
        StateManager::i()->setTempState(s);
    }
    ~StateChanger() {
        resetState();
    }

    void resetState() {
        StateManager::i()->setTempState(StateManager::Unknown);
    }
    void freezeState() {
        StateManager::i()->keepTempState();
    }
};

#endif // STATEMANAGER_H
