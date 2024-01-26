#ifndef DUSVGICONENGINE_H
#define DUSVGICONENGINE_H

#include <QIconEngine>
#include <QSvgRenderer>

class DuSVGIconEngine : public QIconEngine {

public:
    DuSVGIconEngine();
    explicit DuSVGIconEngine(const QString &file);

    // Engine overrides

    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode,
               QIcon::State state) override;
    QIconEngine *clone() const override;
    QPixmap pixmap(const QSize &size, QIcon::Mode mode,
                   QIcon::State state) override;

    void setFile(const QString &file);
    void setCheckedFile(const QString &checkedFile);

    QColor mainColor() const;
    void setMainColor(const QColor &color);

    QColor checkedColor() const;
    void setCheckedColor(const QColor &color);

private:
    QString _data;
    QString _checkedData;
    QColor _mainColor;
    QColor _checkedColor;

    static const QRegularExpression _reColor;
};

#endif // DUSVGICONENGINE_H
