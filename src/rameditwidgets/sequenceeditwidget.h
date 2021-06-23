#ifndef SEQUENCEEDITWIDGET_H
#define SEQUENCEEDITWIDGET_H

#include "objecteditwidget.h"
#include "ramses.h"
#include "objectlisteditwidget.h"

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

public slots:
    void setObject(RamObject *obj) override;

protected slots:
    void update() override;

private slots:
    void createShot();

private:
    RamSequence *m_sequence;

    void setupUi();
    void connectEvents();

    ObjectListEditWidget *ui_shotsList;
};

#endif // SEQUENCEEDITWIDGET_H
