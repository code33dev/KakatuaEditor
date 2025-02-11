#include "TextEditor.hpp"

TextEditor::TextEditor()
{
    setWindowTitle("Qt6 Text Editor with Tabs & File Explorer");
    resize(1000, 600);

    // File Explorer
    explorer = new QTreeView(this);
    fileModel = new QFileSystemModel(this);
    fileModel->setRootPath("");
    explorer->setModel(fileModel);
    explorer->setColumnHidden(1, true);
    explorer->setColumnHidden(2, true);
    explorer->setColumnHidden(3, true);

    // Tabbed Editor
    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &TextEditor::closeTab);

    // Splitter for Explorer & Tabs
    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(explorer);
    splitter->addWidget(tabWidget);
    splitter->setStretchFactor(1, 1);
    setCentralWidget(splitter);

    createMenuBar();
    connect(explorer, &QTreeView::doubleClicked, this, &TextEditor::openFileFromExplorer);
}

void TextEditor::createMenuBar()
{
    QMenuBar *menuBar = new QMenuBar(this);

    QMenu *fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction("Open Folder", this, &TextEditor::openFolder);
    fileMenu->addAction("Open File", QKeySequence::Open, this, &TextEditor::openFile);
    fileMenu->addAction("Save", QKeySequence::Save, this, &TextEditor::saveCurrentFile);
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", QKeySequence::Quit, qApp, &QApplication::quit);

    QMenu *editMenu = menuBar->addMenu("&Edit");
    editMenu->addAction("Change Font", this, &TextEditor::changeFont);
    editMenu->addAction("Undo", QKeySequence::Undo, this, &TextEditor::undo);
    editMenu->addAction("Redo", QKeySequence::Redo, this, &TextEditor::redo);
    editMenu->addSeparator();
    editMenu->addAction("Cut", QKeySequence::Cut, this, &TextEditor::cut);
    editMenu->addAction("Copy", QKeySequence::Copy, this, &TextEditor::copy);
    editMenu->addAction("Paste", QKeySequence::Paste, this, &TextEditor::paste);
    editMenu->addAction("Select All", QKeySequence::SelectAll, this, &TextEditor::selectAll);
    editMenu->addSeparator();
    editMenu->addAction("Find", QKeySequence::Find, this, &TextEditor::findWithWindow);
    editMenu->addAction("Replace", QKeySequence::Replace, this, &TextEditor::replaceWithWindow);
    editMenu->addSeparator();
    editMenu->addAction("Find in Files", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F), this, &TextEditor::findInFilesWithWindow);
    editMenu->addAction("Replace in Files", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H), this, &TextEditor::replaceInFilesWithWindow);
    editMenu->addSeparator();
    editMenu->addAction("Go to Line", QKeySequence(Qt::CTRL | Qt::Key_F12), this, &TextEditor::goToLine);

    setMenuBar(menuBar);
}

void TextEditor::replaceInFilesWithWindow()
    {
        ReplaceInFilesWindow *replaceWindow = new ReplaceInFilesWindow(tabWidget, openTabs);
        replaceWindow->show();
    }

    void TextEditor::findInFilesWithWindow()
    {
        FindInFilesWindow *findWindow = new FindInFilesWindow(tabWidget, openTabs);
        findWindow->show();
    }
    void TextEditor::replaceWithWindow()
    {
        QPlainTextEdit *editor = getCurrentEditor();
        if (!editor)
            return;

        ReplaceWindow *replaceWindow = new ReplaceWindow(editor);
        replaceWindow->show();
    }

    void TextEditor::findWithWindow()
    {
        QPlainTextEdit *editor = getCurrentEditor();
        if (!editor)
            return;

        FindWindow *findWindow = new FindWindow(editor);
        findWindow->show();
    }
    // void TextEditor::createMenuBar()
    // {
    //     QMenuBar *menuBar = new QMenuBar(this);

    //     // File Menu
    //     QMenu *fileMenu = menuBar->addMenu("&File");
    //     fileMenu->addAction("Open Folder", this, &TextEditor::openFolder);
    //     fileMenu->addAction("Open File", QKeySequence::Open, this, &TextEditor::openFile);
    //     fileMenu->addAction("Save", QKeySequence::Save, this, &TextEditor::saveCurrentFile);
    //     fileMenu->addSeparator();
    //     fileMenu->addAction("Exit", QKeySequence::Quit, qApp, &QApplication::quit);

    //     // **Edit Menu**
    //     QMenu *editMenu = menuBar->addMenu("&Edit");
    //     editMenu->addAction("Change Font", this, &TextEditor::changeFont);
    //     editMenu->addAction("Undo", QKeySequence::Undo, this, &TextEditor::undo);
    //     editMenu->addAction("Redo", QKeySequence::Redo, this, &TextEditor::redo);
    //     editMenu->addSeparator();
    //     editMenu->addAction("Cut", QKeySequence::Cut, this, &TextEditor::cut);
    //     editMenu->addAction("Copy", QKeySequence::Copy, this, &TextEditor::copy);
    //     editMenu->addAction("Paste", QKeySequence::Paste, this, &TextEditor::paste);
    //     editMenu->addAction("Select All", QKeySequence::SelectAll, this, &TextEditor::selectAll);
    //     editMenu->addSeparator();
    //     // editMenu->addAction("Find", QKeySequence::Find, this, &TextEditor::findText);
    //     editMenu->addAction("Find", QKeySequence::Find, this, &TextEditor::findWithWindow);
    //     editMenu->addAction("Replace", QKeySequence::Replace, this, &TextEditor::replaceWithWindow);
    //     editMenu->addSeparator();
    //     editMenu->addAction("Find in Files", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F), this, &TextEditor::findInFilesWithWindow);
    //     editMenu->addAction("Replace in Files", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H), this, &TextEditor::replaceInFilesWithWindow);

    //     editMenu->addSeparator();
    //     editMenu->addAction("Go to Line", QKeySequence(Qt::CTRL | Qt::Key_F12), this, &TextEditor::goToLine);

    //     setMenuBar(menuBar);
    // }
    void TextEditor::changeFont()
    {
        bool ok;
        QFont font = QFontDialog::getFont(&ok, QFont("Consolas", 12), this, "Choose Font");
        if (ok)
        {
            for (auto editor : openTabs.values())
            {
                editor->setFont(font);
            }
        }
    }
    void TextEditor::openFolder()
    {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Folder");
        if (!dir.isEmpty())
        {
            fileModel->setRootPath(dir);
            explorer->setRootIndex(fileModel->index(dir));
        }
    }

    void TextEditor::openFile()
    {
        QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt *.cpp *.h);;All Files (*)");
        if (!filePath.isEmpty())
        {
            openFileInTab(filePath);
        }
    }

    void TextEditor::openFileFromExplorer(const QModelIndex &index)
    {
        QString filePath = fileModel->filePath(index);
        if (QFileInfo(filePath).isFile())
        {
            openFileInTab(filePath);
        }
    }

    void TextEditor::openFileInTab(const QString &filePath)
    {
        if (openTabs.contains(filePath))
        {
            tabWidget->setCurrentWidget(openTabs[filePath]);
            return;
        }

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, "Error", "Could not open file:\n" + filePath);
            return;
        }

        QTextStream in(&file);
        QPlainTextEdit *editor = new QPlainTextEdit();
        editor->setPlainText(in.readAll());
        editor->setProperty("filePath", filePath);
        editor->setLineWrapMode(QPlainTextEdit::NoWrap);
        new SyntaxHighlighter(editor->document()); // Apply syntax highlighting
        file.close();

        QPalette palette = editor->palette();
        palette.setColor(QPalette::Base, QColor("#1E1E1E")); // Dark background
        palette.setColor(QPalette::Text, QColor("#D4D4D4")); // Light gray text
        editor->setPalette(palette);

        QFont font("Consolas", 12);
        editor->setFont(font);

        int tabIndex = tabWidget->addTab(editor, QFileInfo(filePath).fileName());
        tabWidget->setTabToolTip(tabIndex, filePath);
        tabWidget->setCurrentIndex(tabIndex);

        openTabs[filePath] = editor;
    }

    void TextEditor::saveCurrentFile()
    {
        QPlainTextEdit *editor = getCurrentEditor();
        if (!editor)
            return;

        QString filePath = editor->property("filePath").toString();
        if (filePath.isEmpty())
            return;

        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, "Error", "Could not save file");
            return;
        }

        QTextStream out(&file);
        out << editor->toPlainText();
        file.close();
    }

    void TextEditor::closeTab(int index)
    {
        QWidget *tab = tabWidget->widget(index);
        if (!tab)
            return;

        QString filePath = tab->property("filePath").toString();
        openTabs.remove(filePath);

        tabWidget->removeTab(index);
        tab->deleteLater();
    }

    void TextEditor::undo()
    {
        if (auto editor = getCurrentEditor())
            editor->undo();
    }
    void TextEditor::redo()
    {
        if (auto editor = getCurrentEditor())
            editor->redo();
    }
    void TextEditor::cut()
    {
        if (auto editor = getCurrentEditor())
            editor->cut();
    }
    void TextEditor::copy()
    {
        if (auto editor = getCurrentEditor())
            editor->copy();
    }
    void TextEditor::paste()
    {
        if (auto editor = getCurrentEditor())
            editor->paste();
    }
    void TextEditor::selectAll()
    {
        if (auto editor = getCurrentEditor())
            editor->selectAll();
    }

    void TextEditor::findText()
    {
        if (auto editor = getCurrentEditor())
        {
            bool ok;
            QString searchText = QInputDialog::getText(this, "Find", "Enter text to find:", QLineEdit::Normal, "", &ok);
            if (ok && !searchText.isEmpty() && !editor->find(searchText))
            {
                QMessageBox::information(this, "Find", "Text not found.");
            }
        }
    }

    void TextEditor::replaceText()
    {
        if (auto editor = getCurrentEditor())
        {
            bool ok;
            QString searchText = QInputDialog::getText(this, "Find", "Enter text to find:", QLineEdit::Normal, "", &ok);
            if (!ok || searchText.isEmpty())
                return;

            QString replaceText = QInputDialog::getText(this, "Replace", "Enter replacement text:", QLineEdit::Normal, "", &ok);
            if (!ok)
                return;

            editor->setPlainText(editor->toPlainText().replace(searchText, replaceText));
        }
    }

    void TextEditor::goToLine()
    {
        if (auto editor = getCurrentEditor())
        {
            bool ok;
            int lineNumber = QInputDialog::getInt(this, "Go to Line", "Enter line number:", 1, 1, editor->document()->blockCount(), 1, &ok);
            if (ok)
            {
                QTextCursor cursor = editor->textCursor();
                cursor.movePosition(QTextCursor::Start);
                cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineNumber - 1);
                editor->setTextCursor(cursor);
            }
        }
    }

    QPlainTextEdit* TextEditor::getCurrentEditor()
    {
        return qobject_cast<QPlainTextEdit *>(tabWidget->currentWidget());
    }
    void TextEditor::findInFiles()
    {
        QString searchText = QInputDialog::getText(this, "Find in Files", "Enter text to find:");
        if (searchText.isEmpty())
            return;

        QStringList foundFiles;

        QDir dir(fileModel->rootPath());
        QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

        for (const QString &file : files)
        {
            QFile f(dir.filePath(file));
            if (f.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&f);
                QString content = in.readAll();
                if (content.contains(searchText))
                {
                    foundFiles.append(file);
                }
            }
        }

        if (foundFiles.isEmpty())
        {
            QMessageBox::information(this, "Find in Files", "No matches found.");
        }
        else
        {
            QMessageBox::information(this, "Find in Files", "Found in:\n" + foundFiles.join("\n"));
        }
    }

    void TextEditor::replaceInFiles()
    {
        QString searchText = QInputDialog::getText(this, "Find in Files", "Enter text to find:");
        if (searchText.isEmpty())
            return;

        QString replaceText = QInputDialog::getText(this, "Replace in Files", "Enter replacement text:");
        if (replaceText.isEmpty())
            return;

        int replacedCount = 0;

        QDir dir(fileModel->rootPath());
        QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

        for (const QString &file : files)
        {
            QFile f(dir.filePath(file));
            if (f.open(QIODevice::ReadWrite | QIODevice::Text))
            {
                QTextStream in(&f);
                QString content = in.readAll();
                if (content.contains(searchText))
                {
                    content.replace(searchText, replaceText);
                    f.resize(0);
                    QTextStream out(&f);
                    out << content;
                    replacedCount++;
                }
            }
        }

        QMessageBox::information(this, "Replace in Files", QString("Replaced in %1 file(s).").arg(replacedCount));
    }