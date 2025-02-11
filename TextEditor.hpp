#ifndef TEXTEDITOR_HPP
#define TEXTEDITOR_HPP

#include "myQt6.hpp"
#include "CodeEditor.hpp"
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
    void updateStatusBar();                        // Updates Caps Lock, Line, and Column
    void updateCapsLockStatus();                   // Checks if Caps Lock is on
    void toggleVerticalLine();                     // Enable/Disable the vertical line
    void setVerticalLineColumn();                  // Set column for the vertical line
    void drawVerticalLine(QPlainTextEdit *editor); // Draw the vertical line
    
    

private:
    QTreeView *explorer;
    QFileSystemModel *fileModel;
    QTabWidget *tabWidget;
    QMap<QString, QPlainTextEdit *> openTabs;
    QStatusBar *statusBar;            // Status bar widget
    QLabel *capsLockLabel;            // Label to display Caps Lock status
    QLabel *positionLabel;            // Label to display current line and column
    bool verticalLineEnabled = false; // Stores ON/OFF state
    int verticalLineColumn = 80;      // Default column for the vertical line
    QAction *toggleLineAction;        // Action for toggling the vertical line
};

#endif // TEXTEDITOR_HPP
