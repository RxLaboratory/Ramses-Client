#include "dusvgiconengine.h"

#include <QPainter>
#include <QRegularExpression>
#include <QFile>

#include "duapp/dusettings.h"
#include "duapp/duui.h"

const QRegularExpression DuSVGIconEngine::_reColor = QRegularExpression("#(?:[a-f\\d]{3,6})", QRegularExpression::CaseInsensitiveOption);

DuSVGIconEngine::DuSVGIconEngine():
    QIconEngine()
{

}

DuSVGIconEngine::DuSVGIconEngine(const QString &file):
    QIconEngine()
{
    setFile(file);
}

void DuSVGIconEngine::paint(QPainter *painter, const QRect &rect,
                            QIcon::Mode mode, QIcon::State state)
{
    if (!painter->isActive()) return;

    QColor c;

    switch(state) {
    case QIcon::On:
        if (_checkedColor.isValid())
            c = _checkedColor;
        else
            c = DuSettings::i()->get(DuSettings::UI_FocusColor).value<QColor>();
        break;
    case QIcon::Off:
        if (_mainColor.isValid())
            c = _mainColor;
        else
            c = DuSettings::i()->get(DuSettings::UI_IconColor).value<QColor>();
        break;
    }

    switch(mode) {
    case QIcon::Disabled:
        c = DuUI::pushColor(c, 2);
        break;
    case QIcon::Active:
    case QIcon::Selected:
        c = DuUI::pullColor(c);
        break;
    case QIcon::Normal:
        break;
    }

    QString d;
    switch(state) {
    case QIcon::On:
        if (_checkedData != "")
            d = _checkedData.replace(_reColor, c.name());
        else
            d = _data.replace(_reColor, c.name());
        break;
    case QIcon::Off:
        d = _data.replace(_reColor, c.name());
        break;
    }

    QSvgRenderer renderer(d.toUtf8());
    renderer.render(painter, rect);
}

QIconEngine *DuSVGIconEngine::clone() const
{
    return new DuSVGIconEngine(*this);
}

QPixmap DuSVGIconEngine::pixmap(const QSize &size, QIcon::Mode mode,
                                QIcon::State state)
{
    // This function is necessary to create an EMPTY pixmap. It's called always
    // before paint()

    QImage img(size, QImage::Format_ARGB32);
    img.fill(qRgba(0, 0, 0, 0));
    QPixmap pix = QPixmap::fromImage(img, Qt::NoFormatConversion);
    {
        QPainter painter(&pix);
        QRect r(QPoint(0.0, 0.0), size);
        this->paint(&painter, r, mode, state);
    }
    return pix;
}

void DuSVGIconEngine::setFile(const QString &file)
{
    QFile f(file);
    if (f.open(QFile::ReadOnly))
    {
        _data = f.readAll();
        f.close();
    }
}

void DuSVGIconEngine::setCheckedFile(const QString &checkedFile)
{
    QFile f(checkedFile);
    if (f.open(QFile::ReadOnly))
    {
        _checkedData = f.readAll();
        f.close();
    }
}

void DuSVGIconEngine::setMainColor(const QColor &color)
{
    _mainColor = color;
}

void DuSVGIconEngine::setCheckedColor(const QColor &color)
{
    _checkedColor = color;
}

QColor DuSVGIconEngine::mainColor() const
{
    if (_mainColor.isValid()) return _mainColor;
    return DuSettings::i()->get(DuSettings::UI_IconColor).value<QColor>();
}

QColor DuSVGIconEngine::checkedColor() const
{
    if (_checkedColor.isValid()) return _checkedColor;
    return DuSettings::i()->get(
                              DuSettings::UI_FocusColor
                              ).value<QColor>();
}
