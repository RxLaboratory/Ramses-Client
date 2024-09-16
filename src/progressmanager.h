#ifndef PROGRESSMANAGER_H
#define PROGRESSMANAGER_H

#include <QObject>


class ProgressManager : public QObject
{
    Q_OBJECT
public:
    static ProgressManager *i();
    void setText(const QString &t);
    void setTitle(const QString &t);

    QString text() const { return m_text; }
    QString title() const { return m_title; }

    void reInit();
    void finish();
    void start();

signals:
    void textChanged(QString);
    void titleChanged(QString);
    void finished();
    void started();

protected:
    static ProgressManager *_instance;

private:
    explicit ProgressManager(QObject *parent = nullptr);

    QString m_title = "";
    QString m_text = "";
};

#endif // PROGRESSMANAGER_H
