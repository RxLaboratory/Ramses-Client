#include "ducolorselector.h"

#include "duwidgets/duicon.h"
#include "qcolordialog.h"
#include "qtoolbutton.h"
#include "duapp/duui.h"

DuColorSelector::DuColorSelector(QWidget *parent) : QWidget(parent)
{
    m_color = QColor(25,25,25);
    setupUi();
    connectEvents();
}

void DuColorSelector::updateColorEditStyle()
{
    QString style = "background-color: " + m_color.name() + ";";
    if (m_color.lightness() > 80) style += "color: #232323;";
    ui_colorEdit->setStyleSheet(style);
    emit colorChanged(m_color);
}

void DuColorSelector::selectColor()
{
    this->setEnabled(false);
    QColorDialog cd( m_color, this );
    cd.setOptions(QColorDialog::DontUseNativeDialog);
    cd.setCurrentColor(m_color);

    if (DuUI::execDialog(&cd))
    {
        QColor col = cd.selectedColor();
        if (col != m_color) {
            m_color = col;
            ui_colorEdit->setText(m_color.name());
            updateColorEditStyle();
            emit colorEdited(m_color);
        }
    }
    this->setEnabled(true);
}

void DuColorSelector::setupUi()
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QBoxLayout *colorLayout = DuUI::addBoxLayout(Qt::Horizontal, this);

    ui_colorEdit = new DuLineEdit(this);
    colorLayout->addWidget(ui_colorEdit);

    ui_colorButton = new QToolButton(this);
    ui_colorButton->setIcon(DuIcon(":/icons/color-dialog"));
    colorLayout->addWidget(ui_colorButton);

}

void DuColorSelector::connectEvents()
{
    connect(ui_colorEdit, &DuLineEdit::editingFinished, this, [this]() {
        QString colName = ui_colorEdit->text();
        if (colName.length() != 7 &&
            colName.length() != 4)
            return;

        QColor col(colName);
        if (col == m_color)
            return;
        m_color = col;
        updateColorEditStyle();
        emit colorEdited(m_color);
    });

    connect(ui_colorButton, &QToolButton::clicked, this, &DuColorSelector::selectColor);
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

