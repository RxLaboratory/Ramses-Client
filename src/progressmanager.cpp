#include "progressmanager.h"

#include <QApplication>

ProgressManager* ProgressManager::_instance = nullptr;

ProgressManager::ProgressManager(QObject *parent) : QObject(parent)
{

}

ProgressManager *ProgressManager::i()
{
    if (!_instance) _instance = new ProgressManager();
    return _instance;
}

void ProgressManager::setText(const QString &t)
{
    if (t == m_text)
        return;
    m_text = t;
    emit textChanged(t);
}

void ProgressManager::setTitle(const QString &t)
{
    if (m_title == t)
        return;
    m_title = t;
    emit titleChanged(t);
}

void ProgressManager::reInit()
{
    setText("");
    setTitle("");
}

void ProgressManager::finish()
{
    reInit();
    emit finished();
}

void ProgressManager::start()
{
    reInit();
    emit started();
}
