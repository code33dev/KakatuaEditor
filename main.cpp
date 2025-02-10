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
#include <QMenu>
#include <QVBoxLayout>
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
        setWindowTitle("Qt6 Syntax Highlighter with Full Menus");
        resize(800, 600);

        explorer = new QTreeView(this);
        fileModel = new QFileSystemModel(this);
        fileModel->setRootPath(""); // Start with no directory loaded
        explorer->setModel(fileModel);
        explorer->setColumnHidden(1, true); // Hide file size
        explorer->setColumnHidden(2, true); // Hide file type
        explorer->setColumnHidden(3, true); // Hide date modified

        editor = new QPlainTextEdit(this);
        setCentralWidget(editor);
        highlighter = new SyntaxHighlighter(editor->document());

        QSplitter *splitter = new QSplitter(this);
        splitter->addWidget(explorer);
        splitter->addWidget(editor);
        splitter->setStretchFactor(1, 1);
        setCentralWidget(splitter);

        createMenuBar();
    }

private:
    QPlainTextEdit *editor;
    SyntaxHighlighter *highlighter;
    QString currentFilePath;
    QTreeView *explorer;
    QFileSystemModel *fileModel;
    void createMenuBar()
    {
        QMenuBar *menuBar = new QMenuBar(this);

        // File Menu
        QMenu *fileMenu = menuBar->addMenu("&File");
        fileMenu->addAction("New", QKeySequence::New, this, &TextEditor::newFile);
        fileMenu->addAction("Open", QKeySequence::Open, this, &TextEditor::openFile);
        fileMenu->addAction("Save", QKeySequence::Save, this, &TextEditor::saveFile);
        fileMenu->addAction("Save All", this, &TextEditor::saveAllFiles);
        fileMenu->addAction("Reload", this, &TextEditor::reloadFile);
        fileMenu->addAction("Close File", this, &TextEditor::closeFile);
        fileMenu->addSeparator();
        fileMenu->addAction("Exit", QKeySequence::Quit, qApp, &QApplication::quit);

        // Edit Menu
        QMenu *editMenu = menuBar->addMenu("&Edit");
        editMenu->addAction("Select All", QKeySequence::SelectAll, editor, &QPlainTextEdit::selectAll);
        editMenu->addAction("Copy", QKeySequence::Copy, editor, &QPlainTextEdit::copy);
        editMenu->addAction("Cut", QKeySequence::Cut, editor, &QPlainTextEdit::cut);
        editMenu->addAction("Paste", QKeySequence::Paste, editor, &QPlainTextEdit::paste);
        editMenu->addSeparator();
        editMenu->addAction("Undo", QKeySequence::Undo, editor, &QPlainTextEdit::undo);
        editMenu->addAction("Redo", QKeySequence::Redo, editor, &QPlainTextEdit::redo);
        editMenu->addSeparator();
        editMenu->addAction("Find", QKeySequence::Find, this, &TextEditor::findText);
        editMenu->addAction("Replace", QKeySequence::Replace, this, &TextEditor::replaceText);
        editMenu->addAction("Find in Files", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F), this, &TextEditor::findInFiles);
        editMenu->addAction("Replace in Files", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H), this, &TextEditor::replaceInFiles);
        editMenu->addAction("Go to line", QKeySequence(Qt::CTRL | Qt::Key_F12), this, &TextEditor::goToLine);

        QMenu *settingsMenu = menuBar->addMenu("&Settings");
        settingsMenu->addAction("Select All", QKeySequence::SelectAll, editor, &QPlainTextEdit::selectAll);
        settingsMenu->addAction("Copy", QKeySequence::Copy, editor, &QPlainTextEdit::copy);
        settingsMenu->addAction("Cut", QKeySequence::Cut, editor, &QPlainTextEdit::cut);
        settingsMenu->addAction("Paste", QKeySequence::Paste, editor, &QPlainTextEdit::paste);
        settingsMenu->addSeparator();
        settingsMenu->addAction("Undo", QKeySequence::Undo, editor, &QPlainTextEdit::undo);
        settingsMenu->addAction("Redo", QKeySequence::Redo, editor, &QPlainTextEdit::redo);
        settingsMenu->addSeparator();
        settingsMenu->addAction("Find", QKeySequence::Find, this, &TextEditor::findText);
        settingsMenu->addAction("Replace", QKeySequence::Replace, this, &TextEditor::replaceText);
        settingsMenu->addAction("Find in Files", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F), this, &TextEditor::findInFiles);
        settingsMenu->addAction("Replace in Files", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H), this, &TextEditor::replaceInFiles);
        settingsMenu->addAction("Go to line", QKeySequence(Qt::CTRL | Qt::Key_F12), this, &TextEditor::goToLine);

        QMenu *systemDTEngineMenu = menuBar->addMenu("&DTEngine");
        systemDTEngineMenu->addAction("Select All", QKeySequence::SelectAll, editor, &QPlainTextEdit::selectAll);
        systemDTEngineMenu->addAction("Copy", QKeySequence::Copy, editor, &QPlainTextEdit::copy);
        systemDTEngineMenu->addAction("Cut", QKeySequence::Cut, editor, &QPlainTextEdit::cut);
        systemDTEngineMenu->addAction("Paste", QKeySequence::Paste, editor, &QPlainTextEdit::paste);
        systemDTEngineMenu->addSeparator();
        systemDTEngineMenu->addAction("Undo", QKeySequence::Undo, editor, &QPlainTextEdit::undo);
        systemDTEngineMenu->addAction("Redo", QKeySequence::Redo, editor, &QPlainTextEdit::redo);
        systemDTEngineMenu->addSeparator();
        systemDTEngineMenu->addAction("Find", QKeySequence::Find, this, &TextEditor::findText);
        systemDTEngineMenu->addAction("Replace", QKeySequence::Replace, this, &TextEditor::replaceText);
        systemDTEngineMenu->addAction("Find in Files", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F), this, &TextEditor::findInFiles);
        systemDTEngineMenu->addAction("Replace in Files", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H), this, &TextEditor::replaceInFiles);
        systemDTEngineMenu->addAction("Go to line", QKeySequence(Qt::CTRL | Qt::Key_F12), this, &TextEditor::goToLine);

        setMenuBar(menuBar);

        connect(explorer, &QTreeView::doubleClicked, this, &TextEditor::openFileFromExplorer);

    }

    void newFile()
    {
        editor->clear();
        currentFilePath.clear();
        setWindowTitle("New File - Qt6 Syntax Highlighter");
    }

    void openFile()
    {
        QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "C++ Files (*.cpp *.h);;All Files (*)");
        if (filePath.isEmpty())
            return;

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, "Error", "Could not open file");
            return;
        }

        QTextStream in(&file);
        editor->setPlainText(in.readAll());
        file.close();

        currentFilePath = filePath;
        setWindowTitle(filePath);
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
     void openFileFromExplorer(const QModelIndex &index) {
        QString filePath = fileModel->filePath(index);
        if (QFileInfo(filePath).isFile()) {
            loadFile(filePath);
        }
    }
     void loadFile(const QString &filePath) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            editor->setPlainText(in.readAll());
            currentFilePath = filePath;
            setWindowTitle(filePath);
        }
    }
    void saveFile()
    {
        if (currentFilePath.isEmpty())
        {
            saveFileAs();
        }
        else
        {
            writeFile(currentFilePath);
        }
    }

    void saveAllFiles()
    {
        saveFile();
        QMessageBox::information(this, "Save All", "All files have been saved.");
    }

    void reloadFile()
    {
        if (currentFilePath.isEmpty())
            return;
        openFile();
    }

    void closeFile()
    {
        editor->clear();
        currentFilePath.clear();
        setWindowTitle("Qt6 Syntax Highlighter");
    }

    void findText()
    {
        QString searchText = QInputDialog::getText(this, "Find", "Enter text to find:");
        if (!searchText.isEmpty())
        {
            editor->find(searchText);
        }
    }

    void replaceText()
    {
        QString searchText = QInputDialog::getText(this, "Replace", "Enter text to find:");
        if (searchText.isEmpty())
            return;
        QString replaceText = QInputDialog::getText(this, "Replace", "Enter replacement text:");
        editor->setPlainText(editor->toPlainText().replace(searchText, replaceText));
    }

    void findInFiles()
    {
        QMessageBox::information(this, "Find in Files", "Feature not implemented yet.");
    }

    void replaceInFiles()
    {
        QMessageBox::information(this, "Replace in Files", "Feature not implemented yet.");
    }

    void writeFile(const QString &filePath)
    {
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

    void saveFileAs()
    {
    }

    void goToLine()
    {
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TextEditor window;
    window.show();
    return app.exec();
}
