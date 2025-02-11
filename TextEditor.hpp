#ifndef TEXTEDITOR_HPP
#define TEXTEDITOR_HPP

#include "myQt6.hpp"
#include "SyntaxHighlighter.hpp"
#include "FindWindow.hpp"
#include "ReplaceWindow.hpp"
#include "FindInFilesWindow.hpp"
#include "ReplaceInFilesWindow.hpp"

class TextEditor : public QMainWindow
{
    Q_OBJECT

public:
    TextEditor();

private slots:
    void openFolder();
    void openFile();
    void openFileFromExplorer(const QModelIndex &index);
    // void openFileInTab(const QString &filePath);
    void saveCurrentFile();
    void closeTab(int index);
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void findText();
    void replaceText();
    void goToLine();
    void findWithWindow();
    void replaceWithWindow();
    void findInFilesWithWindow();
    void replaceInFilesWithWindow();
    void findInFiles();
    void replaceInFiles();
    void changeFont();
    void createMenuBar();
    QPlainTextEdit *getCurrentEditor();
    void openFileInTab(const QString &filePath);
    void updateStatusBar();      // Updates Caps Lock, Line, and Column
    void updateCapsLockStatus(); // Checks if Caps Lock is on
private:
    QTreeView *explorer;
    QFileSystemModel *fileModel;
    QTabWidget *tabWidget;
    QMap<QString, QPlainTextEdit *> openTabs;
    QStatusBar *statusBar; // Status bar widget
    QLabel *capsLockLabel; // Label to display Caps Lock status
    QLabel *positionLabel; // Label to display current line and column
};

#endif // TEXTEDITOR_HPP
