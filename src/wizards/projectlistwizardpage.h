#ifndef PROJECTLISTWIZARDPAGE_H
#define PROJECTLISTWIZARDPAGE_H

#include <QWizardPage>

#include "duwidgets/ducombobox.h"

class ProjectListWizardPage : public QWizardPage
{
    Q_OBJECT
public:

    ProjectListWizardPage(QWidget *parent = nullptr);

    //virtual bool isComplete() const override;
    virtual void initializePage() override;

    QString shortName() const
    {
        return _shortName;
    }

    QString name() const
    {
        return _name;
    }

    QString uuid() const
    {
        return ui_projectsBox->currentData().toString();
    }
    void setUuid(const QString &newUuid);

signals:
    void shortNameChanged(QString shortName);
    void nameChanged(QString name);
    void uuidChanged(QString uuid);

private:
    void setupUi();
    void connectEvents();

    DuComboBox *ui_projectsBox;

    QHash<QString,QStringList> _projects;

    QString _shortName;
    QString _name;
    Q_PROPERTY(QString shortName READ shortName NOTIFY shortNameChanged FINAL)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged FINAL)
};

#endif // PROJECTLISTWIZARDPAGE_H
