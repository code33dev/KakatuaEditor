#ifndef CODEEDITOR_HPP
#define CODEEDITOR_HPP

#include <QPlainTextEdit>
#include <QPainter>
#include <QPaintEvent>
#include <QTextBlock>

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    void setVerticalLineEnabled(bool enabled);
    void setVerticalLineColumn(int column);
    void setFont(const QFont &font);
    void highlightCurrentLine();
    void calculateVerticalLineX();


protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool verticalLineEnabled = false;
    int verticalLineColumn = 80;
    int verticalLineX = 80;
};

#endif // CODEEDITOR_HPP
