#ifndef REPLACEWINDOW_HPP
#define REPLACEWINDOW_HPP

#include "myQt6.hpp"


class ReplaceWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ReplaceWindow(QPlainTextEdit *editor, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void replaceText();
    void replaceAllText();

private:
    QLineEdit *replaceBox;
    QLineEdit *replaceWithBox;
    QPushButton *replaceButton, *replaceAllButton;
    QCheckBox *caseCheckBox, *regexCheckBox;
    QPlainTextEdit *textEditor;
};

#endif // REPLACEWINDOW_HPP
