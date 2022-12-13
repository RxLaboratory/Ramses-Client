#include "statemanager.h"

StateManager *StateManager::_instance = nullptr;

StateManager *StateManager::i()
{
    if (!_instance) _instance = new StateManager();
    return _instance;
}

StateManager::State StateManager::state() const
{
    return m_state;
}

bool StateManager::isDBBusy() const
{
    return (m_state == LoadingDataBase ||
            m_state == WritingDataBase );
}

void StateManager::setState(State newState)
{
    if (newState == m_state) return;
    m_state = newState;
    emit stateChanged(m_state);
}

StateManager::StateManager(QObject *parent)
    : QObject{parent}
{

}
