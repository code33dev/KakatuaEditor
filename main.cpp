// #include <QApplication>
// #include <QMainWindow>
// #include <QPlainTextEdit>
// #include <QFile>
// #include <QTextStream>
// #include <QFileDialog>
// #include <QSyntaxHighlighter>
// #include <QRegularExpression>
// #include <QTextCharFormat>
// #include <QMenuBar>
// #include <QMenu>
// #include <QAction>
// #include <QMessageBox>
// #include <QInputDialog>
// #include <QTreeView>
// #include <QFileSystemModel>
// #include <QSplitter>
// #include <QVBoxLayout>
// #include <QTabWidget>
// #include <QDialog>
// #include <QLabel>
// #include <QLineEdit>
// #include <QPushButton>
// #include <QCheckBox>
// #include <QHBoxLayout>
// #include <QTextCursor>
// #include <QTextDocument>
// #include <QFontDialog>
// #include <QWidget>
// #include <QListWidget>
// #include <QDirIterator>

#include "myQt6.hpp"
#include "TextEditor.hpp"

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
