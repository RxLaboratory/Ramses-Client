#include "ducombobox.h"
#include "duqf-app/duui.h"

DuComboBox::DuComboBox(QWidget *parent) : QComboBox(parent)
{
    setFrame(false);

    // Fix appearance
    QWidget *w = view()->window();
    w->setWindowFlags( Qt::Popup | Qt::FramelessWindowHint);
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
    w->setAttribute(Qt::WA_TranslucentBackground, true);
#endif
    //w->setAttribute(Qt::WA_NoSystemBackground, true);
    //w->setAutoFillBackground(false);
    DuUI::setCSS(w,
                 "QComboBoxPrivateContainer {"
                 "background: rgba(0,0,0,0);"
                 "border: none;"
                 "padding: @padding-small;"
                 "border-radius: @padding-small;"
                 "background-color: #434343;"
                 "color: #e3e3e3;"
                 "}");

    // Set style
    setStyleSheet(DuUI::css("DuComboBox"));

    // Data signals
    connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this] (int i) { emit dataChanged( this->itemData(i)); } );
    connect(this, QOverload<int>::of(&QComboBox::activated),
            this, [this] (int i) { emit dataActivated( this->itemData(i)); } );
}

QSize DuComboBox::sizeHint() const {
    if (_bigHeight)
        return minimumSizeHint();
    else
        return QComboBox::sizeHint();
}

QSize DuComboBox::minimumSizeHint() const {
    if (_bigHeight)
        return QSize(50, QComboBox::minimumSizeHint().height());
    else
        return QComboBox::minimumSizeHint();
}

void DuComboBox::setShrinkable(bool big)
{
    _bigHeight = big;
}

int DuComboBox::setCurrentData(QVariant data)
{
    for (int i = 0; i < count(); i++)
    {
        if (itemData(i) == data)
        {
            setCurrentIndex(i);
            return i;
        }
    }

    setCurrentIndex( -1 );
    return -1;
}
