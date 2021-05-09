#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QObject>

class ProcessManager : public QObject
{
    Q_OBJECT
public:
    static ProcessManager *instance();
    void setText(const QString &t);
    void setTitle(const QString &t);
    void setProgress(const int &p);
    void setMaximum(const int &m);
    void freeze(const bool &f=true);
    void increment();
    void addToMaximum(const int &m);
    void reInit();
    void finish();
    void start();

signals:
    void progress(int);
    void text(QString);
    void title(QString);
    void maximum(int);
    void freezeUI(bool);
    void finished();
    void started();

protected:
    static ProcessManager *_instance;

private:
    explicit ProcessManager(QObject *parent = nullptr);

    int m_val = 0;
    int m_maximum = 0;

};

#endif // PROCESSMANAGER_H
