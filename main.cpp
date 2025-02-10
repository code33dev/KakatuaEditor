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

class SyntaxHighlighter : public QSyntaxHighlighter {
public:
    SyntaxHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
        QTextCharFormat keywordFormat;
        keywordFormat.setForeground(Qt::blue);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns = {
            "\\bint\\b", "\\bdouble\\b", "\\bfloat\\b", "\\bchar\\b",
            "\\breturn\\b", "\\bvoid\\b", "\\bif\\b", "\\belse\\b",
            "\\bfor\\b", "\\bwhile\\b", "\\bclass\\b", "\\bpublic\\b",
            "\\bprivate\\b", "\\bprotected\\b", "\\bnamespace\\b"
        };

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
    void highlightBlock(const QString &text) override {
        for (const HighlightingRule &rule : rules) {
            QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
            while (matchIterator.hasNext()) {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }
    }

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> rules;
};

class TextEditor : public QMainWindow {
public:
    TextEditor() {
        setWindowTitle("Qt6 Syntax Highlighter with Menu");
        resize(800, 600);

        editor = new QPlainTextEdit(this);
        setCentralWidget(editor);
        highlighter = new SyntaxHighlighter(editor->document());

        createMenuBar();
    }

private:
    QPlainTextEdit *editor;
    SyntaxHighlighter *highlighter;

    void createMenuBar() {
        QMenuBar *menuBar = new QMenuBar(this);
        QMenu *fileMenu = menuBar->addMenu("&File");

        QAction *openAction = new QAction("Open", this);
        openAction->setShortcut(QKeySequence::Open);
        connect(openAction, &QAction::triggered, this, &TextEditor::openFile);
        fileMenu->addAction(openAction);

        QAction *exitAction = new QAction("Exit", this);
        exitAction->setShortcut(QKeySequence::Quit);
        connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);
        fileMenu->addAction(exitAction);

        setMenuBar(menuBar);
    }

    void openFile() {
        QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "C++ Files (*.cpp *.h);;All Files (*)");
        if (filePath.isEmpty()) return;

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

        QTextStream in(&file);
        editor->setPlainText(in.readAll());
        file.close();
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    TextEditor window;
    window.show();
    return app.exec();
}
