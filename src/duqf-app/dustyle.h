#ifndef DUSTYLE_H
#define DUSTYLE_H

#include <QProxyStyle>

class DuStyle : public QProxyStyle
{
public:
    explicit DuStyle(QString name) : QProxyStyle(name) {};

    void drawControl(QStyle::ControlElement element, const QStyleOption *option,
                     QPainter *painter, const QWidget *widget = nullptr) const override;

    int styleHint(StyleHint hint, const QStyleOption *option = nullptr,
                  const QWidget *widget = nullptr, QStyleHintReturn *returnData = nullptr) const override;
};

#endif // DUSTYLE_H
