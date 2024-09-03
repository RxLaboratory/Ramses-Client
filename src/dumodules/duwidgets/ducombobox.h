#ifndef DUCOMBOBOX_H
#define DUCOMBOBOX_H

#include <QComboBox>

class DuComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit DuComboBox(QWidget *parent = nullptr);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void setShrinkable(bool big);

    int setCurrentData(QVariant data);

signals:
    // Emitted programatically too
    void dataChanged(QVariant);
    // Emitted only on user interaction
    void dataActivated(QVariant);

private:
    bool _bigHeight;

};

#endif // DUCOMBOBOX_H
