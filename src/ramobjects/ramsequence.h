#ifndef RAMSEQUENCE_H
#define RAMSEQUENCE_H

#include "ramobject.h"
#include "ramproject.h"

class RamSequence : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamSequence *get(QString uuid);
    static RamSequence *c(RamObject *o);

    // METHODS //

    RamSequence(QString shortName, QString name, RamProject *project);

    int shotCount() const;
    double duration() const;

    bool overrideResolution() const;
    void setOverrideResolution(bool override);

    bool overrideFramerate() const;
    void setOverrideFramerate(bool override);

    qreal framerate() const;
    void setFramerate(const qreal &newFramerate);

    int width() const;
    void setWidth(const int width);

    int height() const;
    void setHeight(const int height);

    qreal aspectRatio() const;
    qreal pixelAspectRatio() const;
    void setPixelAspectRatio(const qreal &aspectRatio);

    RamProject *project() const;

    virtual QString details() const override;

    virtual QVariant roleData(int role) const override;

    DBTableModel *shots() const;


public slots:
    virtual void edit(bool show = true) override;

protected:
    static QHash<QString, RamSequence*> m_existingObjects;
    RamSequence(QString uuid);
    virtual QString folderPath() const override { return ""; };

    static QFrame *ui_editWidget;

private:
    void construct();

    DBTableModel *m_shots;
};

#endif // RAMSEQUENCE_H
