#include "duqffolderselectorwidget.h"

DuQFFolderSelectorWidget::DuQFFolderSelectorWidget(SelectorType type, QWidget *parent) :
    QWidget(parent)
{
    setupUi();

    _type = type;
    _dialogTitle = "";

    if (type == File) exploreButton->hide();

    connect(browseButton, &QToolButton::clicked, this, &DuQFFolderSelectorWidget::browseButton_clicked);
    connect(exploreButton, &QToolButton::clicked, this, &DuQFFolderSelectorWidget::exploreButton_clicked);
    connect(folderEdit, &QLineEdit::textChanged, this, &DuQFFolderSelectorWidget::pathChanging);
    connect(folderEdit, &QLineEdit::editingFinished, this, &DuQFFolderSelectorWidget::folderEdit_editingFinished);
}

QString DuQFFolderSelectorWidget::path()
{
    return folderEdit->text();
}

void DuQFFolderSelectorWidget::setPath(QString p)
{
    p = QDir::toNativeSeparators(p);
    folderEdit->setText(p);
    if (p == "" ) p = folderEdit->placeholderText();
    folderEdit->setToolTip(p);
    exploreButton->setToolTip("Reveal folder at " + p);
}

void DuQFFolderSelectorWidget::setPlaceHolderText(QString t)
{
    folderEdit->setPlaceholderText(t);
    setPath(folderEdit->text());
}

void DuQFFolderSelectorWidget::setDialogTitle(QString t)
{
    _dialogTitle = t;
}

void DuQFFolderSelectorWidget::browseButton_clicked()
{
    QString p = "";
    QString d = folderEdit->text();
    if (d == "") d = folderEdit->toolTip();
    if (d == "") d = folderEdit->placeholderText();
    if (_type == Folder) p = QFileDialog::getExistingDirectory(this, _dialogTitle, folderEdit->text());
    else p = QFileDialog::getOpenFileName(this, _dialogTitle, folderEdit->text());
    if (p != "")
    {
        setPath(p);
        emit pathChanged(p);
    }
}

void DuQFFolderSelectorWidget::exploreButton_clicked()
{
    QString path = folderEdit->text();
    if (path == "" ) path = folderEdit->placeholderText();
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

void DuQFFolderSelectorWidget::folderEdit_editingFinished()
{
    setPath(folderEdit->text());
    emit pathChanged(folderEdit->text());
}

void DuQFFolderSelectorWidget::setupUi()
{
    this->setObjectName(QStringLiteral("DuQFFolderSelectorWidget"));
    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setSpacing(3);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    folderEdit = new QLineEdit(this);

    horizontalLayout->addWidget(folderEdit);

    browseButton = new QToolButton(this);
    browseButton->setIcon(QIcon(":/icons/browse-folder"));

    horizontalLayout->addWidget(browseButton);

    exploreButton = new QToolButton(this);
    exploreButton->setObjectName(QStringLiteral("exploreButton"));
    exploreButton->setIcon(QIcon(":/icons/reveal-folder"));

    horizontalLayout->addWidget(exploreButton);
}

