#ifndef DUCOLORSELECTOR_H
#define DUCOLORSELECTOR_H

#include "duwidgets/dulineedit.h"
#include <QWidget>

class DuColorSelector : public QWidget
{
    Q_OBJECT
public:
    explicit DuColorSelector(QWidget *parent = nullptr);

    const QColor &color() const;
    void setColor(const QColor &newColor);

signals:
    // Emited programatically
    void colorChanged(QColor);
    // Only on user interaction
    // If and only if the value has actually changed
    void colorEdited(QColor);

private slots:
    void updateColorEditStyle();
    void selectColor();

private:
    void setupUi();
    void connectEvents();

    QColor m_color;

    DuLineEdit *ui_colorEdit;
    QToolButton *ui_colorButton;
};


#endif // DUCOLORSELECTOR_H
