#ifndef RAMOBJECTPROPERTIESWIDGET_H
#define RAMOBJECTPROPERTIESWIDGET_H

#include <QWidget>
#include <QBoxLayout>
#include <QFormLayout>
#include <QJsonObject>

#include "duwidgets/ducolorselector.h"
#include "duwidgets/dulineedit.h"
#include "duwidgets/durichtextedit.h"

class RamObjectPropertiesWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString shortName READ shortName WRITE setShortName NOTIFY shortNameChanged)
    Q_PROPERTY(QString comment READ comment WRITE setComment NOTIFY commentChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    explicit RamObjectPropertiesWidget(QWidget *parent = nullptr);

    QString name() const { return ui_nameEdit->text(); }
    QString shortName() const { return ui_shortNameEdit->text(); }
    QString comment() const { return ui_commentEdit->toMarkdown(); }
    QColor color() const { return ui_colorSelector->color(); }

    bool isComplete() const { return name() != "" && shortName() != ""; }

    QBoxLayout *mainLayout() const { return ui_mainLayout; }
    QFormLayout *attributesLayout() const { return ui_attributesLayout; }

public slots:
    void setName(const QString &n) { ui_nameEdit->setText(n); }
    void setShortName(const QString &n) { ui_shortNameEdit->setText(n); }
    void setComment(const QString &c) { ui_commentEdit->setMarkdown(c); }
    void setColor(const QColor &c) { ui_colorSelector->setColor(c); }

    void showIdHelp() const;

signals:
    void edited();
    void nameChanged(const QString &);
    void shortNameChanged(const QString &);
    void commentChanged(const QString &);
    void colorChanged(const QColor &);

protected:
    void setupUi();
    void connectEvents();

    QBoxLayout *ui_mainLayout;
    QFormLayout *ui_attributesLayout;

    DuLineEdit *ui_shortNameEdit;
    DuLineEdit *ui_nameEdit;
    DuRichTextEdit *ui_commentEdit;
    DuColorSelector *ui_colorSelector;

    QToolButton *ui_idHelpButton;
};

#endif // RAMOBJECTPROPERTIESWIDGET_H
