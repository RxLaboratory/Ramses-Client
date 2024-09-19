#ifndef RESOLUTIONWIDGET_H
#define RESOLUTIONWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QVBoxLayout>

#include "duwidgets/duspinbox.h"
#include "duwidgets/ducombobox.h"

class ResolutionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ResolutionWidget(QWidget *parent = nullptr);

    int getWidth() const;
    int getHeight() const;

    void setWidth(int w);
    void setHeight(int h);

signals:
    // emited programatically
    void resolutionChanged(int,int);
    // emited only on user interaction
    void resolutionEdited(int,int);

private slots:
    void setPreset(int p);
    void selectPreset();

private:
    void setupUi();
    void connectEvents();

    DuComboBox *presetsBox;
    DuSpinBox *widthBox;
    DuSpinBox *heightBox;
};

#endif // RESOLUTIONWIDGET_H
