#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include <QStackedWidget>

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    void addPage(QWidget *ui, QString title, QIcon icon=QIcon());

public slots:
    void setCurrentIndex(int index);

private slots:
    void mainList_currentRowChanged(int currentRow);

private:
    void setupUi();
    QListWidget *mainList;
    QStackedWidget *mainStackWidget;
};

#endif // SETTINGSWIDGET_H
