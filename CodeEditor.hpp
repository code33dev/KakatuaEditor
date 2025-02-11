#ifndef CODEEDITOR_HPP
#define CODEEDITOR_HPP

#include <QPlainTextEdit>
#include <QPainter>
#include <QPaintEvent>

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    void setVerticalLineEnabled(bool enabled);
    void setVerticalLineColumn(int column);
    void setFont(const QFont &font);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool verticalLineEnabled = false;
    int verticalLineColumn = 80;
};

#endif // CODEEDITOR_HPP
