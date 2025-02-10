#include <QApplication>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QInputDialog>
#include <QTreeView>
#include <QFileSystemModel>
#include <QSplitter>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextDocument>
#include <QFontDialog>

class ReplaceWindow : public QWidget
{

public:
    explicit ReplaceWindow(QPlainTextEdit *editor, QWidget *parent = nullptr)
        : QWidget(parent), textEditor(editor)
    {
        setWindowTitle("Replace");
        setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint); // Independent Window

        QLabel *replaceLabel = new QLabel("Replace:");
        replaceBox = new QLineEdit(this);

        QLabel *replaceWithLabel = new QLabel("With:");
        replaceWithBox = new QLineEdit(this);

        replaceButton = new QPushButton("Replace", this);
        replaceAllButton = new QPushButton("Replace All", this);

        caseCheckBox = new QCheckBox("Match Case", this);
        regexCheckBox = new QCheckBox("Use Regular Expression", this);

        QHBoxLayout *replaceLayout = new QHBoxLayout();
        replaceLayout->addWidget(replaceLabel);
        replaceLayout->addWidget(replaceBox);

        QHBoxLayout *replaceWithLayout = new QHBoxLayout();
        replaceWithLayout->addWidget(replaceWithLabel);
        replaceWithLayout->addWidget(replaceWithBox);

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(replaceButton);
        buttonLayout->addWidget(replaceAllButton);

        QVBoxLayout *mainLayout = new QVBoxLayout();
        mainLayout->addLayout(replaceLayout);
        mainLayout->addLayout(replaceWithLayout);
        mainLayout->addLayout(buttonLayout);
        mainLayout->addWidget(caseCheckBox);
        mainLayout->addWidget(regexCheckBox);

        setLayout(mainLayout);

        connect(replaceButton, &QPushButton::clicked, this, &ReplaceWindow::replaceText);
        connect(replaceAllButton, &QPushButton::clicked, this, &ReplaceWindow::replaceAllText);
    }

protected:
    void closeEvent(QCloseEvent *event) override
    {
        delete this; // Destroy window on close
    }

private slots:
    void replaceText()
    {
        if (!textEditor)
            return;

        QTextCursor cursor = textEditor->textCursor();
        if (!cursor.hasSelection())
        {
            cursor = textEditor->document()->find(replaceBox->text(), cursor);
            textEditor->setTextCursor(cursor);

            //            QMessageBox::warning(this, "Replace", "No text selected.");
            // return;
        }
        if (cursor.hasSelection())
        {
            cursor.insertText(replaceWithBox->text()); // 🟢 Apply replacement
        }
        textEditor->update();
        textEditor->repaint();
        // QString replaceStr = replaceWithBox->text();
        // cursor.insertText(replaceStr);
    }

    void replaceAllText()
    {
        if (!textEditor)
            return;

        QString searchText = replaceBox->text();
        QString replaceText = replaceWithBox->text();
        if (searchText.isEmpty())
            return;

        QTextDocument *doc = textEditor->document();
        QTextCursor cursor(doc);
        cursor.beginEditBlock(); // 🟢 Start edit block

        QTextDocument::FindFlags flags;
        if (caseCheckBox->isChecked())
            flags |= QTextDocument::FindCaseSensitively;

        bool replaced = false;

        // 🔄 Loop through all occurrences and replace them
        while (!cursor.isNull() && !cursor.atEnd())
        {
            cursor = doc->find(searchText, cursor, flags);
            if (!cursor.isNull())
            {
                cursor.insertText(replaceText);
                replaced = true;
            }
        }

        cursor.endEditBlock();             // 🔴 Ensure changes apply immediately
        textEditor->setTextCursor(cursor); // 🟢 Force cursor update

        // 🚨 Notify the user if no matches were replaced
        if (!replaced)
        {
            QMessageBox::information(this, "Replace All", "No matches found.");
        }
        textEditor->update();
        textEditor->repaint();
    }

private:
    QLineEdit *replaceBox;
    QLineEdit *replaceWithBox;
    QPushButton *replaceButton, *replaceAllButton;
    QCheckBox *caseCheckBox, *regexCheckBox;
    QPlainTextEdit *textEditor;
};

class FindWindow : public QWidget
{

public:
    explicit FindWindow(QPlainTextEdit *editor, QWidget *parent = nullptr)
        : QWidget(parent), textEditor(editor)
    {
        setWindowTitle("Find");

        QLabel *label = new QLabel("Find:");
        searchBox = new QLineEdit(this);

        nextButton = new QPushButton("Next", this);
        prevButton = new QPushButton("Previous", this);
        highlightButton = new QPushButton("Highlight All", this);
        caseCheckBox = new QCheckBox("Match Case", this);
        regexCheckBox = new QCheckBox("Use Regular Expression", this);

        QHBoxLayout *topLayout = new QHBoxLayout();
        topLayout->addWidget(label);
        topLayout->addWidget(searchBox);

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(prevButton);
        buttonLayout->addWidget(nextButton);
        buttonLayout->addWidget(highlightButton);

        QVBoxLayout *mainLayout = new QVBoxLayout();
        mainLayout->addLayout(topLayout);
        mainLayout->addLayout(buttonLayout);
        mainLayout->addWidget(caseCheckBox);
        mainLayout->addWidget(regexCheckBox);

        setLayout(mainLayout);

        connect(nextButton, &QPushButton::clicked, this, &FindWindow::findNext);
        connect(prevButton, &QPushButton::clicked, this, &FindWindow::findPrevious);
        connect(highlightButton, &QPushButton::clicked, this, &FindWindow::highlightAllMatches);
    }

private slots:
    void findNext()
    {
        search(QTextDocument::FindFlags()); // Use correct FindFlags
    }

    void findPrevious()
    {
        search(QTextDocument::FindBackward);
    }
    void closeEvent(QCloseEvent *event) override
    {
        removeHighlights(); // 🔄 Unmark all highlights before closing
        QWidget::closeEvent(event);
        delete this; // Ensure proper cleanup
    }
    void removeHighlights()
    {
        if (!textEditor)
            return;

        QTextDocument *document = textEditor->document();
        QTextCursor cursor(document);

        // Reset the entire document background
        cursor.select(QTextCursor::Document);
        QTextCharFormat defaultFormat;
        defaultFormat.setBackground(Qt::transparent);
        cursor.mergeCharFormat(defaultFormat);
    }
    void highlightAllMatches()
    {
        if (!textEditor)
            return;

        QTextDocument *document = textEditor->document();
        QTextCursor highlightCursor(document);
        QTextCharFormat highlightFormat;
        highlightFormat.setBackground(Qt::yellow);

        // 🔄 Step 1: Remove Previous Highlights
        QTextCursor defaultCursor(document);
        defaultCursor.select(QTextCursor::Document);
        QTextCharFormat defaultFormat;
        defaultFormat.setBackground(Qt::transparent);
        defaultCursor.mergeCharFormat(defaultFormat);

        QString searchText = searchBox->text();
        if (searchText.isEmpty())
            return;

        QTextDocument::FindFlags flags;
        if (caseCheckBox->isChecked())
            flags |= QTextDocument::FindCaseSensitively;

        bool found = false;

        // 🔍 Step 2: Find All Matches & Apply Highlight
        while (!highlightCursor.isNull() && !highlightCursor.atEnd())
        {
            highlightCursor = document->find(searchText, highlightCursor, flags);
            if (!highlightCursor.isNull())
            {
                highlightCursor.mergeCharFormat(highlightFormat);
                found = true;
            }
        }

        // 🚨 Step 3: If No Matches Found, Show Warning
        if (!found)
        {
            QMessageBox::information(this, "Find", "No matches found.");
        }
    }

private:
    QLineEdit *searchBox;
    QPushButton *nextButton, *prevButton, *highlightButton;
    QCheckBox *caseCheckBox, *regexCheckBox;
    QPlainTextEdit *textEditor;
    int lastMatchPos;

    void search(QTextDocument::FindFlags flags)
    {
        if (!textEditor)
            return;

        QString searchText = searchBox->text();
        if (searchText.isEmpty())
            return;

        if (caseCheckBox->isChecked())
            flags |= QTextDocument::FindCaseSensitively;
        if (regexCheckBox->isChecked())
            searchText = QRegularExpression(searchText).pattern();

        QTextCursor cursor = textEditor->textCursor();
        cursor = textEditor->document()->find(searchText, cursor, flags);

        if (!cursor.isNull())
        {
            textEditor->setTextCursor(cursor);
            lastMatchPos = cursor.position();
        }
        else
        {
            QMessageBox::information(this, "Find", "No more matches found.");
        }
    }
};

class SyntaxHighlighter : public QSyntaxHighlighter
{
public:
    SyntaxHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
    {
        QTextCharFormat keywordFormat;
        keywordFormat.setForeground(Qt::yellow);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns = {
            "\\bint\\b", "\\bdouble\\b", "\\bfloat\\b", "\\bchar\\b",
            "\\breturn\\b", "\\bvoid\\b", "\\bif\\b", "\\belse\\b",
            "\\bfor\\b", "\\bwhile\\b", "\\bclass\\b", "\\bpublic\\b",
            "\\bprivate\\b", "\\bprotected\\b", "\\bnamespace\\b"};

        QTextCharFormat commentFormat;
        commentFormat.setForeground(Qt::darkGreen);
        commentFormat.setFontItalic(false);

        QTextCharFormat stringFormat;
        stringFormat.setForeground(Qt::white);

        for (const QString &pattern : keywordPatterns)
            rules.append({QRegularExpression(pattern), keywordFormat});

        rules.append({QRegularExpression("//[^\n]*"), commentFormat});
        rules.append({QRegularExpression("\".*?\""), stringFormat});
    }

protected:
    void highlightBlock(const QString &text) override
    {
        for (const HighlightingRule &rule : rules)
        {
            QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
            while (matchIterator.hasNext())
            {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }
    }

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> rules;
};

class TextEditor : public QMainWindow
{
public:
    TextEditor()
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

private:
    QTreeView *explorer;
    QFileSystemModel *fileModel;
    QTabWidget *tabWidget;
    QMap<QString, QPlainTextEdit *> openTabs;

    void replaceWithWindow()
    {
        QPlainTextEdit *editor = getCurrentEditor();
        if (!editor)
            return;

        ReplaceWindow *replaceWindow = new ReplaceWindow(editor);
        replaceWindow->show();
    }

    void findWithWindow()
    {
        QPlainTextEdit *editor = getCurrentEditor();
        if (!editor)
            return;

        FindWindow *findWindow = new FindWindow(editor);
        findWindow->show();
    }
    void createMenuBar()
    {
        QMenuBar *menuBar = new QMenuBar(this);

        // File Menu
        QMenu *fileMenu = menuBar->addMenu("&File");
        fileMenu->addAction("Open Folder", this, &TextEditor::openFolder);
        fileMenu->addAction("Open File", QKeySequence::Open, this, &TextEditor::openFile);
        fileMenu->addAction("Save", QKeySequence::Save, this, &TextEditor::saveCurrentFile);
        fileMenu->addSeparator();
        fileMenu->addAction("Exit", QKeySequence::Quit, qApp, &QApplication::quit);

        // **Edit Menu**
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
        // editMenu->addAction("Find", QKeySequence::Find, this, &TextEditor::findText);
        editMenu->addAction("Find", QKeySequence::Find, this, &TextEditor::findWithWindow);
        editMenu->addAction("Replace", QKeySequence::Replace, this, &TextEditor::replaceWithWindow);
        editMenu->addSeparator();
        editMenu->addAction("Find in Files", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F), this, &TextEditor::findInFiles);
        editMenu->addAction("Replace in Files", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H), this, &TextEditor::replaceInFiles);
        editMenu->addSeparator();
        editMenu->addAction("Go to Line", QKeySequence(Qt::CTRL | Qt::Key_F12), this, &TextEditor::goToLine);

        setMenuBar(menuBar);
    }
    void changeFont()
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
    void openFolder()
    {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Folder");
        if (!dir.isEmpty())
        {
            fileModel->setRootPath(dir);
            explorer->setRootIndex(fileModel->index(dir));
        }
    }

    void openFile()
    {
        QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt *.cpp *.h);;All Files (*)");
        if (!filePath.isEmpty())
        {
            openFileInTab(filePath);
        }
    }

    void openFileFromExplorer(const QModelIndex &index)
    {
        QString filePath = fileModel->filePath(index);
        if (QFileInfo(filePath).isFile())
        {
            openFileInTab(filePath);
        }
    }

    void openFileInTab(const QString &filePath)
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

    void saveCurrentFile()
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

    void closeTab(int index)
    {
        QWidget *tab = tabWidget->widget(index);
        if (!tab)
            return;

        QString filePath = tab->property("filePath").toString();
        openTabs.remove(filePath);

        tabWidget->removeTab(index);
        tab->deleteLater();
    }

    void undo()
    {
        if (auto editor = getCurrentEditor())
            editor->undo();
    }
    void redo()
    {
        if (auto editor = getCurrentEditor())
            editor->redo();
    }
    void cut()
    {
        if (auto editor = getCurrentEditor())
            editor->cut();
    }
    void copy()
    {
        if (auto editor = getCurrentEditor())
            editor->copy();
    }
    void paste()
    {
        if (auto editor = getCurrentEditor())
            editor->paste();
    }
    void selectAll()
    {
        if (auto editor = getCurrentEditor())
            editor->selectAll();
    }

    void findText()
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

    void replaceText()
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

    void goToLine()
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

    QPlainTextEdit *getCurrentEditor()
    {
        return qobject_cast<QPlainTextEdit *>(tabWidget->currentWidget());
    }

    void findInFiles()
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

    void replaceInFiles()
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
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QFile file("darktheme.qss");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString styleSheet = file.readAll();
        app.setStyleSheet(styleSheet);
    }
    TextEditor window;
    window.show();
    return app.exec();
}
