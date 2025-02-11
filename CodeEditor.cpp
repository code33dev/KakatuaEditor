#include "CodeEditor.hpp"

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {}

void CodeEditor::setVerticalLineEnabled(bool enabled)
{
    verticalLineEnabled = enabled;
    viewport()->update(); // Force repaint
}

void CodeEditor::setVerticalLineColumn(int column)
{
    verticalLineColumn = column;
    viewport()->update(); // Force repaint
}

void CodeEditor::paintEvent(QPaintEvent *event)
{
    QPlainTextEdit::paintEvent(event);

    if (!verticalLineEnabled)
        return;

    QPainter painter(viewport());
    painter.setPen(QColor(Qt::red));

    // ✅ Get the actual font used in the editor
    QFont editorFont = this->font();

    // ✅ Get the font size
    int fontSize = editorFont.pointSize();

    // ✅ Get the font metrics based on the editor's font size
    QFontMetrics fm(editorFont);

    // ✅ Get the width of a single character considering font size
    int charWidth = fm.horizontalAdvance('M');

    // ✅ Calculate the X position based on the column, font size, and scrolling
    int xPos = (verticalLineColumn * charWidth) - contentOffset().x() + document()->documentMargin();

    // ✅ Draw the vertical line precisely at the correct column
    painter.drawLine(xPos, 0, xPos, viewport()->height());
}
void CodeEditor::setFont(const QFont &font)
{
    QPlainTextEdit::setFont(font);  // ✅ Apply new font to editor
    viewport()->update();  // ✅ Force a repaint to realign the vertical line
}
