#include "dufolderselectorwidget.h"

#include "duutils/utils.h"
#include "duwidgets/duicon.h"

DuFolderSelectorWidget::DuFolderSelectorWidget(SelectorType type, QWidget *parent) :
    QWidget(parent)
{
    setupUi();

    m_type = type;
    m_dialogTitle = "";

    if (type == File) ui_exploreButton->hide();

    connect(ui_browseButton, &QToolButton::clicked, this, &DuFolderSelectorWidget::browseButton_clicked);
    connect(ui_exploreButton, &QToolButton::clicked, this, &DuFolderSelectorWidget::exploreButton_clicked);
    connect(ui_deleteButton, &QToolButton::clicked, this, &DuFolderSelectorWidget::deleteButton_clicked);
    connect(ui_folderEdit, &QLineEdit::textChanged, this, &DuFolderSelectorWidget::pathChanging);
    connect(ui_folderEdit, &QLineEdit::editingFinished, this, &DuFolderSelectorWidget::folderEdit_editingFinished);
}

QString DuFolderSelectorWidget::path()
{
    return ui_folderEdit->text();
}

void DuFolderSelectorWidget::setPath(QString p)
{
    p = QDir::toNativeSeparators(p);
    ui_folderEdit->setText(p);
    if (p == "" ) p = ui_folderEdit->placeholderText();
    ui_folderEdit->setToolTip(p);
    ui_exploreButton->setToolTip("Reveal folder at " + p);
}

void DuFolderSelectorWidget::setPlaceHolderText(QString t)
{
    ui_folderEdit->setPlaceholderText(t);
    setPath(ui_folderEdit->text());
}

void DuFolderSelectorWidget::setDialogTitle(QString t)
{
    m_dialogTitle = t;
}

void DuFolderSelectorWidget::setMode(SelectorMode newMode)
{
    m_mode = newMode;
}

void DuFolderSelectorWidget::setFilter(const QString &filter)
{
    m_filter = filter;
}

void DuFolderSelectorWidget::showDeleteButton(QString trashFolder, bool show)
{
    m_trashFolder = trashFolder;
    ui_deleteButton->setVisible(show);
}

void DuFolderSelectorWidget::showRevealButton(bool show)
{
    if (show) ui_exploreButton->show();
    else ui_exploreButton->hide();
}

void DuFolderSelectorWidget::browseButton_clicked()
{
    QString p = "";
    QString d = ui_folderEdit->text();
    if (d == "") d = ui_folderEdit->toolTip();
    if (d == "") d = ui_folderEdit->placeholderText();
    if (m_type == Folder) p = QFileDialog::getExistingDirectory(this, m_dialogTitle, ui_folderEdit->text());
    else if (m_type == File && m_mode == Save) p = QFileDialog::getSaveFileName(this, m_dialogTitle, ui_folderEdit->text(), m_filter);
    else p = QFileDialog::getOpenFileName(this, m_dialogTitle, ui_folderEdit->text(), m_filter);
    if (p != "")
    {
        setPath(p);
        emit pathChanged(p);
    }
}

void DuFolderSelectorWidget::exploreButton_clicked()
{
    QString path = ui_folderEdit->text();
    if (path == "" ) path = ui_folderEdit->placeholderText();
    FileUtils::openInExplorer( path, true );
}

void DuFolderSelectorWidget::folderEdit_editingFinished()
{
    setPath(ui_folderEdit->text());
    emit pathChanged(ui_folderEdit->text());
}

void DuFolderSelectorWidget::deleteButton_clicked()
{
    QMessageBox::StandardButton confirm = QMessageBox::question( this,
        "Confirm deletion",
        "Are you sure you want to move this file to the trash?" );

    if ( confirm != QMessageBox::Yes) return;

    QString origin = ui_folderEdit->text();
    if (origin == "") origin = ui_folderEdit->placeholderText();
    QFileInfo originInfo(origin);
    if(!originInfo.exists()) return;

    // First, try to move to trash
    if( m_trashFolder != "" )
    {
        // Move any already existing file in trash
        QDir originDir = originInfo.dir();
        QString destination = originDir.absolutePath() + "/" + m_trashFolder + "/" + originInfo.fileName();
        originDir.mkdir(m_trashFolder);
        FileUtils::move(origin, destination, true);
    }
    else
    {
        FileUtils::moveToTrash(origin);
    }
    emit fileRemoved();
}

void DuFolderSelectorWidget::setupUi()
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    this->setObjectName(QStringLiteral("DuQFFolderSelectorWidget"));
    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setSpacing(3);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    ui_folderEdit = new QLineEdit(this);

    horizontalLayout->addWidget(ui_folderEdit);

    ui_deleteButton = new QToolButton(this);
    ui_deleteButton->setIcon(DuIcon(":/icons/remove"));
    ui_deleteButton->setVisible(false);

    horizontalLayout->addWidget(ui_deleteButton);

    ui_browseButton = new QToolButton(this);
    ui_browseButton->setIcon(DuIcon(":/icons/selector-pop-up"));

    horizontalLayout->addWidget(ui_browseButton);

    ui_exploreButton = new QToolButton(this);
    ui_exploreButton->setObjectName(QStringLiteral("exploreButton"));
    ui_exploreButton->setIcon(DuIcon(":/icons/folder"));

    horizontalLayout->addWidget(ui_exploreButton);
}


