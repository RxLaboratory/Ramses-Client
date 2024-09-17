#include "dulabel.h"

#include <QPainter>
#include <QRegularExpression>

const QRegularExpression DuLabel::_imageRE = QRegularExpression(
    "\\n?DuLabel{\\s+image:\\surl\\(.*\\)\\s*;\\s*\\n*image-position:\\s*\\w*(?:\\s\\w*)?\\s*;\\s*}"
    );

DuLabel::DuLabel(QWidget *parent):
    QLabel(parent)
{

}

DuLabel::DuLabel(const QString &text, QWidget *parent):
    QLabel(parent)
{
    _text = text;
}

DuLabel::DuLabel(const QString &text, Qt::TextElideMode mode, QWidget *parent):
    QLabel(parent)
{
    _text = text;
    _elideMode = mode;
    if (_elideMode == Qt::ElideNone)
        setSizePolicy(
            QSizePolicy::Minimum,
            QSizePolicy::Fixed
            );
    else
        setSizePolicy(
            QSizePolicy::Preferred,
            QSizePolicy::Fixed
            );
}

void DuLabel::setText(const QString &text)
{
    if (text == _text) return;
    _text = text;
    if (!_useCustomTooltip) QLabel::setToolTip(_text);
    update();
}

void DuLabel::setImage(const QString &i)
{
    QString style = this->styleSheet();
    style = style.replace(_imageRE,"");

    if (i == "") {
        this->setStyleSheet( style );
        _hasPixmap = false;
        return;
    }

    style += "\nDuLabel{ image: url(" + i + "); image-position: center top; }";
    this->setStyleSheet(style);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _hasPixmap = true;
    update();
}

void DuLabel::mousePressEvent(QMouseEvent *)
{
    _clicked = true;
}

void DuLabel::mouseReleaseEvent(QMouseEvent *)
{
    if (_clicked) emit clicked();
    _clicked = false;
}

const QString &DuLabel::text() const
{
    return _text;
}

void DuLabel::setToolTip(const QString &toolTip)
{
    _useCustomTooltip = true;
    QLabel::setToolTip(toolTip);
}

void DuLabel::setElideMode(Qt::TextElideMode mode)
{
    if (mode == _elideMode) return;
    _elideMode = mode;
    update();
}

void DuLabel::paintEvent(QPaintEvent *e)
{
    QLabel::paintEvent(e);

    QPainter painter(this);
    if (!painter.isActive()) return;

    QFontMetrics fontMetrics = painter.fontMetrics();

    QString elidedLine = fontMetrics.elidedText(_text, _elideMode, width());

    int ascent = fontMetrics.ascent();
    int x = 0;
    int y = ascent;

    // Center vertically
    Qt::Alignment align = alignment();
    if (align.testFlag(Qt::AlignVCenter)) {
        int moreHeight = height() - ascent;
        y = moreHeight/2 + ascent;
    }
    // Bottom alignment
    else if (align.testFlag(Qt::AlignBottom)) {
        y = height();
    }

    // Center horizontally
    if (align.testFlag(Qt::AlignHCenter)) {
        int w = fontMetrics.horizontalAdvance(elidedLine);
        int moreWidth = width() - w;
        x = moreWidth / 2 + w;
    }
    // Right alignment
    else if (align.testFlag(Qt::AlignRight)) {
        x = width();
    }

    painter.drawText(QPoint(x, y), elidedLine);
}

Qt::TextElideMode DuLabel::elideMode() const
{
    return _elideMode;
}

void DuLabel::setTextColor(const QColor &color)
{
    QPalette palette = this->palette();
    palette.setColor(this->backgroundRole(), color);
    palette.setColor(this->foregroundRole(), color);
    this->setPalette(palette);
}

QSize DuLabel::sizeHint() const
{
    if (!this->isVisible() || _hasPixmap) return QLabel::sizeHint();
    if (_text == "") return this->minimumSizeHint();

    // Just a text (or nothing)
    QFontMetrics fontMetrics = this->fontMetrics();
    return fontMetrics.size(Qt::TextShowMnemonic, _text);
}

