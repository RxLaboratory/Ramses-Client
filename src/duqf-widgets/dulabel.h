#ifndef DULABEL_H
#define DULABEL_H

#include <QLabel>

class DuLabel : public QLabel
{
    Q_OBJECT
public:
    DuLabel(QWidget *parent = nullptr);
    DuLabel(const QString &text, QWidget *parent = nullptr);
    DuLabel(const QString &text, Qt::TextElideMode mode, QWidget *parent = nullptr);

    const QString & text() const;

    void setToolTip(const QString &toolTip);

    void setElideMode(Qt::TextElideMode mode);
    Qt::TextElideMode elideMode() const;

    void setTextColor(const QColor &color);

    virtual QSize sizeHint() const override;

signals:
    void clicked();

public slots:
    void setText(const QString &text);
    void setImage(const QString &i);

protected:
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void paintEvent(QPaintEvent *e) override;

private:
    QSize _originalPixmapSize;
    QString _text;
    bool _hasPixmap = false;
    Qt::TextElideMode _elideMode = Qt::ElideNone;
    bool _useCustomTooltip = false;
    bool _clicked = false;

    static const QRegularExpression _imageRE;
};

#endif // DULABEL_H
