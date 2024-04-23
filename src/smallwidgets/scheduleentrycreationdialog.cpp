#include "scheduleentrycreationdialog.h"

ScheduleEntryCreationDialog::ScheduleEntryCreationDialog(QWidget *parent) :
    QDialog(parent)
{
    this->setWindowTitle(tr("Create schedule entry"));

    auto mainLayout = new QVBoxLayout(this);

    auto formLayout = new QFormLayout();
    mainLayout->addLayout(formLayout);

    ui_titleEdit = new DuLineEdit(this);
    ui_titleEdit->setPlaceholderText(tr("Short Title"));
    formLayout->addRow(tr("Title"), ui_titleEdit);

    ui_commentEdit = new DuQFTextEdit(this);
    ui_commentEdit->setUseMarkdown(true);
    ui_commentEdit->setPlaceholderText(tr("Add some details here.\nMarkdown supported."));
    formLayout->addRow(tr("Details"), ui_commentEdit);

    ui_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mainLayout->addWidget(ui_buttonBox);

    connect(ui_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString ScheduleEntryCreationDialog::title() const
{
    return ui_titleEdit->text();
}

QString ScheduleEntryCreationDialog::comment() const
{
    return ui_commentEdit->toMarkdown();
}
