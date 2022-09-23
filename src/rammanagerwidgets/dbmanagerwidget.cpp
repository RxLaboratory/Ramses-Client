#include "dbmanagerwidget.h"

#include "dbinterface.h"

DBManagerWidget::DBManagerWidget(QWidget *parent)
    : QWidget{parent}
{
    setupUi();
    connectEvents();
}

void DBManagerWidget::clean()
{
    QString report = DBInterface::instance()->cleanDabaBase();
    ui_reportEdit->setMarkdown(report);
    ui_reportEdit->setEnabled(true);
    ui_cancelCleanButton->setEnabled(true);
    ui_acceptCleanButton->setEnabled(true);
    ui_cleanButton->setEnabled(false);

    QMessageBox::information(this,
                             tr("Finished cleaning the database"),
                             tr("The database has been cleaned.\n"
                                "Please review the changes before accepting (or cancelling) them.")
                             );
}

void DBManagerWidget::cancel()
{
    QMessageBox::information(this,
                             tr("Cancelling changes"),
                             tr("The application needs to restart to undo all changes.")
                             );

    bool ok = DBInterface::instance()->undoClean();

    if (!ok) QMessageBox::warning(this,
                                  tr("Can't undo!"),
                                  tr("Sorry, I can't undo the cleaning!\n\n"
                                     "Syncing has been disabled for now, you can either check what may be wrong, or close the application.")
                                  );
    ui_cancelCleanButton->setEnabled(false);
    ui_acceptCleanButton->setEnabled(false);
}

void DBManagerWidget::accept()
{
    ui_cancelCleanButton->setEnabled(false);
    ui_acceptCleanButton->setEnabled(false);
    ui_cleanButton->setEnabled(true);
    DBInterface::instance()->acceptClean();
}

void DBManagerWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(3);

    ui_cleanButton = new QPushButton(tr("Clean Database"), this);
    mainLayout->addWidget(ui_cleanButton);

    ui_reportEdit = new QTextEdit();
    ui_reportEdit->setEnabled(false);
    ui_reportEdit->setReadOnly(true);
    mainLayout->addWidget(ui_reportEdit);

    QHBoxLayout *validButtonLayout = new QHBoxLayout();
    validButtonLayout->setSpacing(3);
    mainLayout->addLayout(validButtonLayout);

    ui_cancelCleanButton = new QPushButton(tr("Undo changes"), this);
    ui_cancelCleanButton->setEnabled(false);
    validButtonLayout->addWidget(ui_cancelCleanButton);

    ui_acceptCleanButton = new QPushButton(tr("Accept changes"), this);
    ui_acceptCleanButton->setEnabled(false);
    validButtonLayout->addWidget(ui_acceptCleanButton);
}

void DBManagerWidget::connectEvents()
{
    connect(ui_cleanButton, &QPushButton::clicked, this, &DBManagerWidget::clean);
    connect(ui_cancelCleanButton, &QPushButton::clicked, this, &DBManagerWidget::cancel);
    connect(ui_acceptCleanButton, &QPushButton::clicked, this, &DBManagerWidget::accept);
}
