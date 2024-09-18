#ifndef FRAMERATEWIDGET_H
#define FRAMERATEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

#include "duwidgets/ducombobox.h"
#include "duwidgets/autoselectdoublespinbox.h"

class FramerateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FramerateWidget(QWidget *parent = nullptr);

    double framerate() const;
    void setFramerate(double fr);

signals:
    // programatically
    void framerateChanged(double);
    // user interaction only
    void framerateEdited(double);

private slots:
    void setPreset(int p);
    void selectPreset(double fr);

private:
    void setupUi();
    void connectEvents();

    DuComboBox *presetsBox;
    AutoSelectDoubleSpinBox *framerateBox;

};

#endif // FRAMERATEWIDGET_H
