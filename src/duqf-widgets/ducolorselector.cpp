#include "ducolorselector.h"

#include "duqf-widgets/duicon.h"
#include "duqf-app/duui.h"

DuColorSelector::DuColorSelector(QWidget *parent) : QWidget(parent)
{
    m_color = QColor(25,25,25);
    setupUi();
    connectEvents();
}

void DuColorSelector::updateColorEditStyle()
{
    if (ui_colorEdit->text().count() == 7)
    {
        m_color.setNamedColor(ui_colorEdit->text());
        QString style = "background-color: " + m_color.name() + ";";
        if (m_color.lightness() > 80) style += "color: #232323;";
        ui_colorEdit->setStyleSheet(style);
    }
    emit colorChanged(m_color);
}

void DuColorSelector::selectColor()
{
    this->setEnabled(false);
    QColorDialog cd( m_color, this );
    cd.setOptions(QColorDialog::DontUseNativeDialog);
    cd.setCurrentColor(m_color);
    //cd.setWindowFlags(Qt::FramelessWindowHint);
    //cd.move(this->parentWidget()->parentWidget()->geometry().center().x()-cd.geometry().width()/2,this->parentWidget()->parentWidget()->geometry().center().y()-cd.geometry().height()/2);
    if (cd.exec())
    {
        m_color = cd.selectedColor();
        ui_colorEdit->setText(m_color.name());
        updateColorEditStyle();
    }
    this->setEnabled(true);
}

void DuColorSelector::setupUi()
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *colorLayout = new QHBoxLayout(this);
    colorLayout->setSpacing(DuUI::adjustToDpi(3));
    colorLayout->setContentsMargins(0,0,0,0);

    ui_colorEdit = new DuLineEdit(this);
    colorLayout->addWidget(ui_colorEdit);

    ui_colorButton = new QToolButton(this);
    ui_colorButton->setIcon(DuIcon(":/icons/color-dialog"));
    colorLayout->addWidget(ui_colorButton);

}

void DuColorSelector::connectEvents()
{
    connect(ui_colorEdit, SIGNAL(editingFinished()), this, SLOT(updateColorEditStyle()));
    connect(ui_colorButton, SIGNAL(clicked()), this, SLOT(selectColor()));
}

const QColor &DuColorSelector::color() const
{
    return m_color;
}

void DuColorSelector::setColor(const QColor &newColor)
{
    if (m_color == newColor)
        return;
    m_color = newColor;
    ui_colorEdit->setText(m_color.name());
    updateColorEditStyle();
}

