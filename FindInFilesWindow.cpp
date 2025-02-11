#include "FindInFilesWindow.hpp"

FindInFilesWindow::FindInFilesWindow(QTabWidget *tabWidget, QMap<QString, QPlainTextEdit *> &openTabs, QWidget *parent)
    : QWidget(parent), tabWidget(tabWidget), openTabs(openTabs)
{
    setWindowTitle("Find in Files");
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);

    QLabel *searchLabel = new QLabel("Search for:");
    searchBox = new QLineEdit(this);

    QLabel *folderLabel = new QLabel("Folder:");
    folderBox = new QLineEdit(this);
    QPushButton *browseButton = new QPushButton("Browse", this);

    QLabel *extensionLabel = new QLabel("File Types:");
    extensionBox = new QLineEdit(this);
    extensionBox->setPlaceholderText("e.g., *.cpp *.h *.txt");

    caseCheckBox = new QCheckBox("Match Case", this);
    regexCheckBox = new QCheckBox("Use Regular Expression", this);

    searchButton = new QPushButton("Search", this);
    resultList = new QListWidget(this);

    QHBoxLayout *folderLayout = new QHBoxLayout();
    folderLayout->addWidget(folderBox);
    folderLayout->addWidget(browseButton);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(searchLabel);
    mainLayout->addWidget(searchBox);
    mainLayout->addWidget(folderLabel);
    mainLayout->addLayout(folderLayout);
    mainLayout->addWidget(extensionLabel);
    mainLayout->addWidget(extensionBox);
    mainLayout->addWidget(caseCheckBox);
    mainLayout->addWidget(regexCheckBox);
    mainLayout->addWidget(searchButton);
    mainLayout->addWidget(resultList);

    setLayout(mainLayout);

    connect(browseButton, &QPushButton::clicked, this, &FindInFilesWindow::selectFolder);
    connect(searchButton, &QPushButton::clicked, this, &FindInFilesWindow::searchFiles);
    connect(resultList, &QListWidget::itemClicked, this, &FindInFilesWindow::openSelectedFile);
}

void FindInFilesWindow::selectFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Folder");
    if (!dir.isEmpty())
    {
        folderBox->setText(dir);
    }
}

void FindInFilesWindow::searchFiles()
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
        QMessageBox::information(this, "Find in Files", "No matches found.");
    }
}

void FindInFilesWindow::openSelectedFile(QListWidgetItem *item)
{
    QString filePath = item->data(Qt::UserRole).toString();
    int lineNumber = item->data(Qt::UserRole + 1).toInt();

    if (!filePath.isEmpty())
    {
        openFileAtLine(filePath, lineNumber);
    }
}

void FindInFilesWindow::openFileAtLine(const QString &filePath, int lineNumber)
{
    if (openTabs.contains(filePath))
    {
        tabWidget->setCurrentWidget(openTabs[filePath]);
    }
    else
    {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            QPlainTextEdit *editor = new QPlainTextEdit();
            editor->setPlainText(in.readAll());
            editor->setProperty("filePath", filePath);
            editor->setLineWrapMode(QPlainTextEdit::NoWrap);

            int tabIndex = tabWidget->addTab(editor, QFileInfo(filePath).fileName());
            tabWidget->setTabToolTip(tabIndex, filePath);
            tabWidget->setCurrentIndex(tabIndex);

            openTabs[filePath] = editor;
            file.close();
        }
    }

    QPlainTextEdit *editor = openTabs[filePath];
    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineNumber - 1);
    editor->setTextCursor(cursor);
    editor->setFocus();
}
