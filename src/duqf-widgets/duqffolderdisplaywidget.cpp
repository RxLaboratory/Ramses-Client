#include "duqffolderdisplaywidget.h"

DuQFFolderDisplayWidget::DuQFFolderDisplayWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();

    connect(exploreButton, &QToolButton::clicked, this, &DuQFFolderDisplayWidget::exploreButton_clicked);

    exploreButton->setEnabled(false);
}

void DuQFFolderDisplayWidget::setPath(QString p)
{
    p = QDir::toNativeSeparators(p);
    folderLabel->setText(p);
    folderLabel->setToolTip(p);
    exploreButton->setToolTip("Reveal folder at " + p);
    exploreButton->setEnabled(p != "");
}

void DuQFFolderDisplayWidget::exploreButton_clicked()
{
    QString path = folderLabel->text();
    if (path == "") return;
    if(!QFileInfo::exists(path))
    {
        QMessageBox::StandardButton rep = QMessageBox::question(this,
                                                                "The folder does not exist",
                                                                "This folder:\n\n" + path + "\n\ndoes not exist yet.\nDo you want to create it now?",
                                                                QMessageBox::Yes | QMessageBox::No,
                                                                QMessageBox::Yes);
        if (rep == QMessageBox::Yes) QDir(path).mkpath(".");
    }
    FileUtils::openInExplorer( path );
}


void DuQFFolderDisplayWidget::setupUi()
{
    this->setObjectName(QStringLiteral("DuQFFolderDisplayWidget"));
    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setSpacing(3);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    folderLabel = new QLabel(this);

    horizontalLayout->addWidget(folderLabel);

    exploreButton = new QToolButton(this);
    exploreButton->setObjectName(QStringLiteral("exploreButton"));
    exploreButton->setIcon(QIcon(":/icons/reveal-folder"));

    horizontalLayout->addWidget(exploreButton);
}
