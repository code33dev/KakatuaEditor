#ifndef REPLACEINFILES_WINDOW_HPP
#define REPLACEINFILES_WINDOW_HPP

#include "myQt6.hpp"

class ReplaceInFilesWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ReplaceInFilesWindow(QTabWidget *tabWidget, QMap<QString, QPlainTextEdit *> &openTabs, QWidget *parent = nullptr);

private slots:
    void selectFolder();
    void searchFiles();
    void replaceAllInFiles();
    void openSelectedFile(QListWidgetItem *item);

private:
    QLineEdit *searchBox, *replaceBox, *folderBox, *extensionBox;
    QPushButton *searchButton, *replaceAllButton;
    QCheckBox *caseCheckBox, *regexCheckBox;
    QListWidget *resultList;
    QTabWidget *tabWidget;
    QMap<QString, QPlainTextEdit *> &openTabs;

    void openFileAtLine(const QString &filePath, int lineNumber);
};

#endif // REPLACEINFILES_WINDOW_HPP
