#ifndef FINDINFILES_WINDOW_HPP
#define FINDINFILES_WINDOW_HPP

#include "myQt6.hpp"

class FindInFilesWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FindInFilesWindow(QTabWidget *tabWidget, QMap<QString, QPlainTextEdit *> &openTabs, QWidget *parent = nullptr);

private slots:
    void selectFolder();
    void searchFiles();
    void openSelectedFile(QListWidgetItem *item);

private:
    QLineEdit *searchBox, *folderBox, *extensionBox;
    QPushButton *searchButton;
    QCheckBox *caseCheckBox, *regexCheckBox;
    QListWidget *resultList;
    QTabWidget *tabWidget;
    QMap<QString, QPlainTextEdit *> &openTabs;

    void openFileAtLine(const QString &filePath, int lineNumber);
};

#endif // FINDINFILES_WINDOW_HPP
