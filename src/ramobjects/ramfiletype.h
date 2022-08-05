#ifndef RAMFILETYPE_H
#define RAMFILETYPE_H

#include "ramobject.h"

class RamFileType : public RamObject
{
    Q_OBJECT
public:

    // STATIC //

    static RamFileType *getObject(QString uuid, bool constructNew = false);

    // OTHER //

    RamFileType(QString shortName, QString name);

    void setExtensions(QString extensions);
    void setExtensions(QStringList extensions);
    QStringList extensions() const;

    bool previewable() const;
    void setPreviewable(bool previewable);

    bool check(QString filePath) const;

    virtual QString details() const override;

public slots:
    virtual void edit(bool show = true) override;

protected:
    RamFileType(QString uuid);

private:
    void construct();

};

#endif // RAMFILETYPE_H
