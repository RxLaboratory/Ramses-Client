#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QSplitter>
#include <QShowEvent>
#include <QHideEvent>

#include "duwidgets/dulistwidget.h"

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    void addPage(QWidget *ui, QString title, QIcon icon=QIcon());
public slots:
    void setCurrentIndex(int index);
signals:
    void reinitRequested();
    void closeRequested();
protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
private slots:
    void mainList_currentRowChanged(int currentRow);
private:
    void setupUi();
    QSplitter *m_splitter;
    DuListWidget *m_mainList;
    QStackedWidget *m_mainStackWidget;
};

#endif // SETTINGSWIDGET_H
