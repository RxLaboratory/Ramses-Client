#include "processmanager.h"

ProcessManager* ProcessManager::_instance = nullptr;

ProcessManager::ProcessManager(QObject *parent) : QObject(parent)
{

}

bool ProcessManager::isBusy() const
{
    return m_busy;
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
    reInit();
    if (f) emit started();
    else emit finished();
    emit freezeUI(f);
}

void ProcessManager::increment()
{
    m_busy = true;
    setProgress(m_val + 1);
}

void ProcessManager::addToMaximum(const int &m)
{
    setMaximum(m_maximum + m);
}

void ProcessManager::reInit()
{
    setText("");
    setTitle("");
    setProgress(0);
    setMaximum(0);
    m_busy = false;
}

void ProcessManager::finish()
{
    m_busy = false;
    emit finished();
}

void ProcessManager::start()
{
    reInit();
    m_busy = true;
    emit started();
}
