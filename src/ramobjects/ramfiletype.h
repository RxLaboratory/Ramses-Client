#ifndef RAMFILETYPE_H
#define RAMFILETYPE_H

#include "ramobject.h"

class RamFileType : public RamObject
{
    Q_OBJECT
public:
    RamFileType(QString shortName, QString name = "", QStringList extensions = QStringList(), QString uuid = "");
    RamFileType(QString shortName, QString name, QString extensions, QString uuid = "");
    ~RamFileType();

    void setExtensions(QString extensions);
    QStringList extensions() const;

    bool isPreviewable() const;
    void setPreviewable(bool previewable);

    void update();

    static RamFileType *fileType(QString uuid);

private:
    QStringList _extensions;
    bool _previewable = false;
};

#endif // RAMFILETYPE_H
