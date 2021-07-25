#ifndef OBJECTEDITWIDGET_H
#define OBJECTEDITWIDGET_H

#include <QScrollArea>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QTextEdit>
#include <QShowEvent>

#include "dbinterface.h"
#include "ramobject.h"

/**
 * @brief The ObjectEditWidget class is the base class of all editors for RamObjects (Shots, steps, etc)
 * It can be used either in the UI, or in a DockWidget
 */
class ObjectEditWidget : public QScrollArea
{
    Q_OBJECT

public:
    explicit ObjectEditWidget(QWidget *parent = nullptr);
    explicit ObjectEditWidget(RamObject *o, QWidget *parent = nullptr);

    RamObject *object() const;
    void hideName(bool hide = true);

    void monitorDbQuery(QString queryName);

public slots:
    virtual void setObject(RamObject *object);

protected slots:
    virtual void update();
    virtual bool checkInput();
    void objectChanged(RamObject *o);

protected:
    void showEvent(QShowEvent *event) override;

    QList<QMetaObject::Connection> _objectConnections;
    bool updating = false;
    QStringList m_dontRename;

    QVBoxLayout *ui_mainLayout;
    QGridLayout *ui_mainFormLayout;
    QLineEdit *ui_shortNameEdit;
    QLineEdit *ui_nameEdit;
    QLabel *ui_nameLabel;
    QLabel *ui_shortNameLabel;
    QLabel *ui_commentLabel;
    QTextEdit *ui_commentEdit;

    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void objectRemoved(RamObject *o);
    void dbiDataReceived(QJsonObject data);

private:
    void setupUi();


    RamObject *m_object;
    void connectEvents();

    QStringList m_dbQueries;
    bool m_modified = true;
};

#endif // OBJECTEDITWIDGET_H
