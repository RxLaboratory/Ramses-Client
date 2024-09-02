#ifndef OBJECTEDITWIDGET_H
#define OBJECTEDITWIDGET_H

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QShowEvent>
#include <QRegExpValidator>

#include "duqf-widgets/duqftextedit.h"
#include "qtabwidget.h"
#include "ramobject.h"
#include "duqf-widgets/duscrollarea.h"

/**
 * @brief The ObjectEditWidget class is the base class of all editors for RamObjects (Shots, steps, etc)
 * It can be used either in the UI, or in a DockWidget
 */
class ObjectEditWidget : public DuScrollArea
{
    Q_OBJECT

public:
    explicit ObjectEditWidget(QWidget *parent = nullptr);

    RamObject *object() const;
    void hideName(bool hide = true);
    void hideShortName(bool hide = true);

public slots:
    void setObject(RamObject *object);
    void lockShortName(bool lock = true);

protected slots:
    void setShortName();
    void setName();
    void setComment();

    void objectChanged(RamObject *o = nullptr);

    void checkPath();

    void setFilesTabVisible(bool visible);

protected:
    virtual void reInit(RamObject *o) = 0;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

    QStringList m_dontRename;

    QWidget *ui_attributesWidget;
    QTabWidget *ui_tabWidget;
    QVBoxLayout *ui_mainLayout;
    QGridLayout *ui_mainFormLayout;
    QLineEdit *ui_shortNameEdit;
    QLineEdit *ui_nameEdit;
    QLabel *ui_nameLabel;
    QLabel *ui_shortNameLabel;
    QLabel *ui_commentLabel;
    DuQFTextEdit *ui_commentEdit;
    QToolButton *ui_lockShortNameButton;
    QVBoxLayout *ui_fileLayout;
    QWidget *ui_fileWidget;
    QIcon ui_fileIcon;

    bool m_reinit = false;

private slots:
    void objectRemoved(RamObject *o);
    void unlockShortName();

private:
    void setupUi();
    void connectEvents();

    bool m_nameHidden = false;

    RamObject *m_object;
};

#endif // OBJECTEDITWIDGET_H
