#ifndef RAMSEQUENCE_H
#define RAMSEQUENCE_H

#include "ramobject.h"
#include "data-models/ramobjectfiltermodel.h"

class RamProject;

class RamSequence : public RamObject
{
    Q_OBJECT
public:
    explicit RamSequence(QString shortName, RamProject *project, QString name,  QString uuid = "");
    ~RamSequence();

    int shotCount() const;
    double duration() const;

    RamProject *project() const;

    static RamSequence *sequence(QString uuid);

    const QColor &color() const;
    void setColor(const QColor &newColor);

public slots:
    void update() override;
    virtual void edit(bool show = true) override;
    virtual void removeFromDB() override;

private:
    RamProject *m_project;
    RamObjectFilterModel *m_shots;
    QColor m_color;
};

#endif // RAMSEQUENCE_H
