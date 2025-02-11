#include "CodeEditor.hpp"

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {}

void CodeEditor::setVerticalLineEnabled(bool enabled)
{
    verticalLineEnabled = enabled;
    viewport()->update();  // Force repaint
}

void CodeEditor::setVerticalLineColumn(int column)
{
    verticalLineColumn = column;
    viewport()->update();  // Force repaint
}

void CodeEditor::paintEvent(QPaintEvent *event)
{
    QPlainTextEdit::paintEvent(event);

    if (!verticalLineEnabled)
        return;

    QPainter painter(viewport());
    painter.setPen(QColor(Qt::red));

    int charWidth = fontMetrics().horizontalAdvance('M');  // Get character width
    int xPos = verticalLineColumn * charWidth;

    painter.drawLine(xPos, 0, xPos, viewport()->height());
}
