#ifndef RAMTEMPLATESTEP_H
#define RAMTEMPLATESTEP_H

#include "ramobject.h"

#include "data-models/ramobjectlist.h"
#include "ramapplication.h"

class RamTemplateStep : public RamObject
{
    Q_OBJECT
public:

    // ENUMS //

    enum Type{ PreProduction,
               AssetProduction,
               ShotProduction,
               PostProduction,
               All };
    Q_ENUM(Type)

    enum EstimationMethod { EstimatePerShot = 0,
                            EstimatePerSecond = 1 };
    Q_ENUM(EstimationMethod)

    // STATIC METHODS //

    /**
     * @brief stepTypeName gets the name of a type, as used in the Database and API classes
     * @param type
     * @return
     */
    static const QString stepTypeName(Type type);
    static Type stepTypeFromName(QString typeName);

    static RamTemplateStep *getObject(QString uuid, bool constructNew = false);

    // OTHER METHODS //

    explicit RamTemplateStep(QString shortName, QString name);

    RamObjectList<RamApplication*> *applications() const;

    Type type() const;
    void setType(const Type &type);
    void setType(QString type);

    QColor color() const;
    void setColor(const QColor &newColor);

    QString publishSettings() const;
    void setPublishSettings(const QString &newPublishSettings);

    EstimationMethod estimationMethod() const;
    void setEstimationMethod(const EstimationMethod &newEstimationMethod);

    float estimationVeryEasy() const;
    void setEstimationVeryEasy(float newEstimationVeryEasy);

    float estimationEasy() const;
    void setEstimationEasy(float newEstimationEasy);

    float estimationMedium() const;
    void setEstimationMedium(float newEstimationMedium);

    float estimationHard() const;
    void setEstimationHard(float newEstimationHard);

    float estimationVeryHard() const;
    void setEstimationVeryHard(float newEstimationVeryHard);

public slots:
    virtual void edit(bool show = true) override;

protected:

    // STATIC //

    /**
     * @brief m_stepTypeMeta is used to return information from the type,
     * like the corresponding string
     */
    static const QMetaEnum m_stepTypeMeta;

    // METHODS //

    RamTemplateStep(QString uuid);

private:
    void construct();

    RamObjectList<RamApplication*> *m_applications;
};

#endif // RAMTEMPLATESTEP_H
