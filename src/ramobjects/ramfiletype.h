#ifndef RAMFILETYPE_H
#define RAMFILETYPE_H

#include "ramobject.h"
#include "objectdockwidget.h"

class RamFileType : public RamObject
{
    Q_OBJECT
public:
    RamFileType(QString shortName, QString name = "", QStringList extensions = QStringList(), QString uuid = "");
    RamFileType(QString shortName, QString name, QString extensions, QString uuid = "");
    ~RamFileType();

    void setExtensions(QString extensions);
    void setExtensions(QStringList extensions);
    QStringList extensions() const;

    bool isPreviewable() const;
    void setPreviewable(bool previewable);

    bool check(QString filePath) const;

    void update() override;

    static RamFileType *fileType(QString uuid);

public slots:
    virtual void edit(bool show = true) override;

private:
    QStringList m_extensions;
    bool m_previewable = false;
};

#endif // RAMFILETYPE_H
