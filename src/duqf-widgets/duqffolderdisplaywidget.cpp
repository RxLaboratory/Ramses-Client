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

    FileUtils::openInExplorer( _path, true );
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
