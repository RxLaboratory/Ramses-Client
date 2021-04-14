#ifndef SEQUENCEEDITWIDGET_H
#define SEQUENCEEDITWIDGET_H

#include "objecteditwidget.h"
#include "ramses.h"
#include "simpleobjectlist.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"

class SequenceEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit SequenceEditWidget(QWidget *parent = nullptr);
    explicit SequenceEditWidget(RamSequence *s = nullptr, QWidget *parent = nullptr);

    RamSequence *sequence() const;
    void setSequence(RamSequence *sequence);

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    void sequenceChanged(RamObject *o);
    void newShot(RamShot *shot);
    void shotRemoved(RamShot *shot);
    void addShot();
    void removeShot(RamObject *o);

private:
    RamSequence *_sequence;

    void setupUi();
    void connectEvents();

    DuQFFolderDisplayWidget *folderWidget;
    SimpleObjectList *shotsList;
};

#endif // SEQUENCEEDITWIDGET_H
