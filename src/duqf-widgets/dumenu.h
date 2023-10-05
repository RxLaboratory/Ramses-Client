#ifndef DUMENU_H
#define DUMENU_H

#include <QMenu>

class DuMenu : public QMenu
{
    Q_OBJECT

public:
    DuMenu(const QString &title, QWidget *parent = nullptr);
    DuMenu(QWidget *parent = nullptr);

    void setShadowWidth(int w);
    int shadowWidth() const;

    void ensureLoaded();

protected:
    void paintEvent(QPaintEvent*e) override;
    void resizeEvent(QResizeEvent *e) override;

private:
    void setupUi();

    QPixmap m_cache = QPixmap();
    int m_shadowWidth = 5;

};

#endif // DUMENU_H
