#ifndef SEQUENCEEDITWIDGET_H
#define SEQUENCEEDITWIDGET_H

#include "objecteditwidget.h"
#include "ramses.h"
#include "simpleobjectlist.h"

/**
 * @brief The SequenceEditWidget class is used to edit Sequences and can be shown either in the main UI or in the Dock
 */
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
    void newShot(RamObject *obj);
    void shotRemoved(RamObject *shot);
    void addShot();
    void removeShot(RamObject *o);

private:
    RamSequence *_sequence;

    void setupUi();
    void connectEvents();

    SimpleObjectList *shotsList;

    bool _creatingShot = false;
};

#endif // SEQUENCEEDITWIDGET_H
