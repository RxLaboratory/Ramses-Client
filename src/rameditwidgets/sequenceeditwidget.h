#ifndef SEQUENCEEDITWIDGET_H
#define SEQUENCEEDITWIDGET_H

#include "objecteditwidget.h"
#include "objectlistwidget.h"
#include "duqf-widgets/duqfcolorselector.h"
#include "ramsequence.h"
#include "resolutionwidget.h"
#include "frameratewidget.h"

/**
 * @brief The SequenceEditWidget class is used to edit Sequences and can be shown either in the main UI or in the Dock
 */
class SequenceEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit SequenceEditWidget(QWidget *parent = nullptr);
    explicit SequenceEditWidget(RamSequence *s, QWidget *parent = nullptr);

    RamSequence *sequence() const;

protected:
    virtual void reInit(RamObject *o) override;

private slots:
    void setColor(QColor c);
    void setOverrideResolution(bool r);
    void setOverrideFramerate(bool r);
    void setResolution(int w, int h);
    void setFramerate(qreal f);
    void createShot();

private:
    RamSequence *m_sequence;

    void setupUi();
    void connectEvents();

    DuQFColorSelector *ui_colorSelector;
    QCheckBox *ui_resolutionBox;
    QCheckBox *ui_framerateBox;
    ResolutionWidget *ui_resolutionWidget;
    FramerateWidget *ui_framerateWidget;

    ObjectListWidget *ui_shotsList;
};

#endif // SEQUENCEEDITWIDGET_H
