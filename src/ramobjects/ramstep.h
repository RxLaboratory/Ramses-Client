#ifndef RAMSTEP_H
#define RAMSTEP_H

#include <QDesktopServices>

#include "ramobject.h"
#include "ramuser.h"
#include "ramapplication.h"
#include "data-models/ramobjectlist.h"

class RamProject;

class RamStep : public RamObject
{
    Q_OBJECT
public:
    enum Type{ PreProduction, AssetProduction, ShotProduction, PostProduction };
    Q_ENUM(Type)

    // Template (no project set)
    explicit RamStep(QString shortName, QString name = "", QString uuid = "");
    // Actual step
    explicit RamStep(QString shortName, QString name, RamProject *project,  QString uuid = "");
    ~RamStep();
    void init();

    bool isTemplate() const;
    RamStep *createFromTemplate(RamProject *project);
    RamStep *createFromTemplate(QString projectUuid);

    Type type() const;
    void setType(const Type &type);
    void setType(QString type);

    RamProject *project() const;
    void setProject( RamProject *project );

    RamObjectList *users() const;
    RamObjectList *applications() const;

    void openFile(QString filePath) const;
    QStringList publishedTemplates() const;
    QString templateFile(QString templateFileName) const;

    QList<RamObject *> inputFileTypes();
    QList<RamObject *> outputFileTypes();

    static RamStep *step(QString uuid);

public slots:
    void update() override;
    virtual void edit(bool show = true) override;

protected:
    virtual QString folderPath() const override;

private slots:
    void userAssigned(const QModelIndex &parent, int first, int last);
    void userUnassigned(const QModelIndex &parent, int first, int last);
    void applicationAssigned(const QModelIndex &parent, int first, int last);
    void applicationUnassigned(const QModelIndex &parent, int first, int last);

private:
    bool m_template;
    Type m_type;
    RamProject *m_project;
    RamObjectList *m_users;
    RamObjectList *m_applications;
};

#endif // RAMSTEP_H
