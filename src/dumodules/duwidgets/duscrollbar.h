#ifndef DUSCROLLBAR_H
#define DUSCROLLBAR_H

#include <QScrollBar>
#include <QVariantAnimation>

class DuScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    DuScrollBar(Qt::Orientation orientation, QWidget *parent = nullptr);
    DuScrollBar(QWidget *parent = nullptr);
    virtual QSize sizeHint() const override;

protected:
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEvent *) override;
#else
    void enterEvent(QEnterEvent *) override;
#endif
    void leaveEvent(QEvent *) override;

private slots:
    void changeWidth(QVariant width);

private:
    void setupUi();
    void connectEvents();

    QVariantAnimation m_sizeAnimation;
    int m_width;
};

#endif // DUSCROLLBAR_H
