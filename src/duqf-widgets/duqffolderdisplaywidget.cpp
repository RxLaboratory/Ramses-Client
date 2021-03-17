#include "duqffolderdisplaywidget.h"

DuQFFolderDisplayWidget::DuQFFolderDisplayWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();

    _path = "";

    connect(exploreButton, &QToolButton::clicked, this, &DuQFFolderDisplayWidget::exploreButton_clicked);

    exploreButton->setEnabled(false);
}

void DuQFFolderDisplayWidget::setPath(QString p)
{
    p = QDir::toNativeSeparators(p);
    _path = p;
    if (p.count() > 30) p = p.replace(0, p.count()-30, "(...)");
    folderLabel->setText(p);
    folderLabel->setToolTip(p);
    exploreButton->setToolTip("Reveal folder at " + p);
    exploreButton->setEnabled(p != "");
}

void DuQFFolderDisplayWidget::exploreButton_clicked()
{
    if (_path == "") return;
    if(!QFileInfo::exists(_path))
    {
        QMessageBox::StandardButton rep = QMessageBox::question(this,
                                                                "The folder does not exist",
                                                                "This folder:\n\n" + _path + "\n\ndoes not exist yet.\nDo you want to create it now?",
                                                                QMessageBox::Yes | QMessageBox::No,
                                                                QMessageBox::Yes);
        if (rep == QMessageBox::Yes) QDir(_path).mkpath(".");
    }
    FileUtils::openInExplorer( _path );
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
