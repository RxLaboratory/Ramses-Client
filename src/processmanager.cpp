#include "processmanager.h"

ProcessManager* ProcessManager::_instance = nullptr;

ProcessManager::ProcessManager(QObject *parent) : QObject(parent)
{

}

ProcessManager *ProcessManager::instance()
{
    if (!_instance) _instance = new ProcessManager();
    return _instance;
}

void ProcessManager::setText(const QString &t)
{
    emit text(t);
}

void ProcessManager::setTitle(const QString &t)
{
    emit title(t);
}

void ProcessManager::setProgress(const int &p)
{
    m_val = p;
    emit progress(p);
}

void ProcessManager::setMaximum(const int &m)
{
    m_maximum = m;
    emit maximum(m);
}

void ProcessManager::freeze(const bool &f)
{
    setText("");
    setTitle("");
    setProgress(0);
    setMaximum(0);
    emit freezeUI(f);
}

void ProcessManager::increment()
{
    setProgress(m_val + 1);
}

void ProcessManager::addToMaximum(const int &m)
{
    setMaximum(m_maximum + m);
}
