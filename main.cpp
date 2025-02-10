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

class SyntaxHighlighter : public QSyntaxHighlighter
{
public:
    SyntaxHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
    {
        QTextCharFormat keywordFormat;
        keywordFormat.setForeground(Qt::blue);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns = {
            "\\bint\\b", "\\bdouble\\b", "\\bfloat\\b", "\\bchar\\b",
            "\\breturn\\b", "\\bvoid\\b", "\\bif\\b", "\\belse\\b",
            "\\bfor\\b", "\\bwhile\\b", "\\bclass\\b", "\\bpublic\\b",
            "\\bprivate\\b", "\\bprotected\\b", "\\bnamespace\\b"};

        QTextCharFormat commentFormat;
        commentFormat.setForeground(Qt::darkGreen);
        commentFormat.setFontItalic(true);

        QTextCharFormat stringFormat;
        stringFormat.setForeground(Qt::darkRed);

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
        editMenu->addAction("Undo", QKeySequence::Undo, this, &TextEditor::undo);
        editMenu->addAction("Redo", QKeySequence::Redo, this, &TextEditor::redo);
        editMenu->addSeparator();
        editMenu->addAction("Cut", QKeySequence::Cut, this, &TextEditor::cut);
        editMenu->addAction("Copy", QKeySequence::Copy, this, &TextEditor::copy);
        editMenu->addAction("Paste", QKeySequence::Paste, this, &TextEditor::paste);
        editMenu->addAction("Select All", QKeySequence::SelectAll, this, &TextEditor::selectAll);
        editMenu->addSeparator();
        editMenu->addAction("Find", QKeySequence::Find, this, &TextEditor::findText);
        editMenu->addAction("Replace", QKeySequence::Replace, this, &TextEditor::replaceText);
        editMenu->addSeparator();
        editMenu->addAction("Find in Files", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F), this, &TextEditor::findInFiles);
        editMenu->addAction("Replace in Files", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H), this, &TextEditor::replaceInFiles);
        editMenu->addSeparator();
        editMenu->addAction("Go to Line", QKeySequence(Qt::CTRL | Qt::Key_F12), this, &TextEditor::goToLine);

        setMenuBar(menuBar);
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
    TextEditor window;
    window.show();
    return app.exec();
}
