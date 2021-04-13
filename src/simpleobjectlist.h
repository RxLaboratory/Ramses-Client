#ifndef SIMPLEOBJECTLIST_H
#define SIMPLEOBJECTLIST_H

#include <QScrollArea>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QtDebug>

class SimpleObjectList : public QScrollArea
{
    Q_OBJECT
public:
    explicit SimpleObjectList(QWidget *parent = nullptr);

    void addWidget(QWidget *w);

signals:
    void currentIndexChanged(int);

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void setupUi();
    QVBoxLayout *mainLayout;
};

#endif // SIMPLELISTWIDGET_H
