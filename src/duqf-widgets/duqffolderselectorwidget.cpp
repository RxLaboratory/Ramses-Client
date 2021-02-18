#include "duqffolderselectorwidget.h"

DuQFFolderSelectorWidget::DuQFFolderSelectorWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _dialogTitle = "";

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
    folderEdit->setText(p);
}

void DuQFFolderSelectorWidget::setPlaceHolderText(QString t)
{
    folderEdit->setPlaceholderText(t);
}

void DuQFFolderSelectorWidget::setDialogTitle(QString t)
{
    _dialogTitle = t;
}

void DuQFFolderSelectorWidget::browseButton_clicked()
{
    QString p = QFileDialog::getExistingDirectory(this, _dialogTitle, folderEdit->text());
    if (p != "")
    {
        folderEdit->setText(p);
        emit pathChanged(p);
    }
}

void DuQFFolderSelectorWidget::exploreButton_clicked()
{
    QString path = folderEdit->text();
    if (path == "" ) path = folderEdit->placeholderText();
    FileUtils::openInExplorer( path );
}

void DuQFFolderSelectorWidget::folderEdit_editingFinished()
{
    emit pathChanged(folderEdit->text());
}
