#ifndef DATABASEEDITWIDGET_H
#define DATABASEEDITWIDGET_H

#include <QScrollArea>

#include "servereditwidget.h"

class DatabaseEditWidget : public QScrollArea
{
    Q_OBJECT
public:
    DatabaseEditWidget(QWidget *parent = nullptr);

    const QString &dbFile() const;
    void setDbFile(const QString &newDbFile);

signals:
    void applied();

private slots:
    void setSync(bool s);
    void apply();
    void reset();

private:
    void setupUi();
    void connectEvents();

    QString m_dbFile;

    QCheckBox *ui_syncBox;
    ServerEditWidget *ui_serverEdit;
    QPushButton *ui_resetButton;
    QPushButton *ui_applyButton;
};

#endif // DATABASEEDITWIDGET_H
