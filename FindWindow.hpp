#ifndef FINDWINDOW_HPP
#define FINDWINDOW_HPP

#include "myQt6.hpp"

class FindWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FindWindow(QPlainTextEdit *editor, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void findNext();
    void findPrevious();
    void highlightAllMatches();
    void removeHighlights();

private:
    QLineEdit *searchBox;
    QPushButton *nextButton, *prevButton, *highlightButton;
    QCheckBox *caseCheckBox, *regexCheckBox;
    QPlainTextEdit *textEditor;
    int lastMatchPos;

    void search(QTextDocument::FindFlags flags);
};

#endif // FINDWINDOW_HPP
