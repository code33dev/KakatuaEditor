#include "ReplaceInFilesWindow.hpp"

ReplaceInFilesWindow::ReplaceInFilesWindow(QTabWidget *tabWidget, QMap<QString, QPlainTextEdit *> &openTabs, QWidget *parent)
    : QWidget(parent), tabWidget(tabWidget), openTabs(openTabs)
{
    setWindowTitle("Replace in Files");
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);

    QLabel *searchLabel = new QLabel("Find:");
    searchBox = new QLineEdit(this);

    QLabel *replaceLabel = new QLabel("Replace with:");
    replaceBox = new QLineEdit(this);

    QLabel *folderLabel = new QLabel("Folder:");
    folderBox = new QLineEdit(this);
    QPushButton *browseButton = new QPushButton("Browse", this);

    QLabel *extensionLabel = new QLabel("File Types:");
    extensionBox = new QLineEdit(this);
    extensionBox->setPlaceholderText("e.g., *.cpp *.h *.txt");

    caseCheckBox = new QCheckBox("Match Case", this);
    regexCheckBox = new QCheckBox("Use Regular Expression", this);

    searchButton = new QPushButton("Find Matches", this);
    replaceAllButton = new QPushButton("Replace All", this);
    resultList = new QListWidget(this);

    QHBoxLayout *folderLayout = new QHBoxLayout();
    folderLayout->addWidget(folderBox);
    folderLayout->addWidget(browseButton);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(searchLabel);
    mainLayout->addWidget(searchBox);
    mainLayout->addWidget(replaceLabel);
    mainLayout->addWidget(replaceBox);
    mainLayout->addWidget(folderLabel);
    mainLayout->addLayout(folderLayout);
    mainLayout->addWidget(extensionLabel);
    mainLayout->addWidget(extensionBox);
    mainLayout->addWidget(caseCheckBox);
    mainLayout->addWidget(regexCheckBox);
    mainLayout->addWidget(searchButton);
    mainLayout->addWidget(replaceAllButton);
    mainLayout->addWidget(resultList);

    setLayout(mainLayout);

    connect(browseButton, &QPushButton::clicked, this, &ReplaceInFilesWindow::selectFolder);
    connect(searchButton, &QPushButton::clicked, this, &ReplaceInFilesWindow::searchFiles);
    connect(replaceAllButton, &QPushButton::clicked, this, &ReplaceInFilesWindow::replaceAllInFiles);
    connect(resultList, &QListWidget::itemClicked, this, &ReplaceInFilesWindow::openSelectedFile);
}

void ReplaceInFilesWindow::selectFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Folder");
    if (!dir.isEmpty())
    {
        folderBox->setText(dir);
    }
}

void ReplaceInFilesWindow::searchFiles()
{
    resultList->clear();

    QString searchPattern = searchBox->text();
    QString folderPath = folderBox->text();
    QString fileExtensions = extensionBox->text();

    if (searchPattern.isEmpty() || folderPath.isEmpty() || fileExtensions.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "Please enter search text, folder, and file extensions.");
        return;
    }

    QTextDocument::FindFlags flags;
    if (caseCheckBox->isChecked())
        flags |= QTextDocument::FindCaseSensitively;

    QDirIterator it(folderPath, fileExtensions.split(" "), QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString filePath = it.next();
        QFile file(filePath);

        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            int lineNumber = 1;
            while (!in.atEnd())
            {
                QString line = in.readLine();
                bool matchFound = regexCheckBox->isChecked() ? line.contains(QRegularExpression(searchPattern)) 
                                                             : line.contains(searchPattern, caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);

                if (matchFound)
                {
                    QListWidgetItem *item = new QListWidgetItem(QString("%1:%2 - %3").arg(filePath).arg(lineNumber).arg(line));
                    item->setData(Qt::UserRole, filePath);
                    item->setData(Qt::UserRole + 1, lineNumber);
                    resultList->addItem(item);
                }

                lineNumber++;
            }
            file.close();
        }
    }

    if (resultList->count() == 0)
    {
        QMessageBox::information(this, "Replace in Files", "No matches found.");
    }
}

void ReplaceInFilesWindow::replaceAllInFiles()
{
    QString searchText = searchBox->text();
    QString replaceText = replaceBox->text();
    QString folderPath = folderBox->text();
    QString fileExtensions = extensionBox->text();

    if (searchText.isEmpty() || folderPath.isEmpty() || fileExtensions.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "Please enter search text, folder, and file extensions.");
        return;
    }

    int replacedCount = 0;

    QDirIterator it(folderPath, fileExtensions.split(" "), QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString filePath = it.next();
        QFile file(filePath);

        if (file.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            QTextStream in(&file);
            QString content = in.readAll();
            file.close();

            if (content.contains(searchText))
            {
                content.replace(searchText, replaceText);
                if (file.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    QTextStream out(&file);
                    out << content;
                    replacedCount++;
                }
            }
        }
    }

    QMessageBox::information(this, "Replace in Files", QString("Replaced in %1 file(s).").arg(replacedCount));
}

void ReplaceInFilesWindow::openSelectedFile(QListWidgetItem *item)
{
    QString filePath = item->data(Qt::UserRole).toString();
    int lineNumber = item->data(Qt::UserRole + 1).toInt();

    if (!filePath.isEmpty())
    {
        openFileAtLine(filePath, lineNumber);
    }
}

void ReplaceInFilesWindow::openFileAtLine(const QString &filePath, int lineNumber)
{
    if (openTabs.contains(filePath))
    {
        tabWidget->setCurrentWidget(openTabs[filePath]);
    }

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QPlainTextEdit *editor = new QPlainTextEdit();
        editor->setPlainText(in.readAll());
        editor->setProperty("filePath", filePath);

        int tabIndex = tabWidget->addTab(editor, QFileInfo(filePath).fileName());
        tabWidget->setCurrentIndex(tabIndex);
        openTabs[filePath] = editor;
        file.close();
    }
}
