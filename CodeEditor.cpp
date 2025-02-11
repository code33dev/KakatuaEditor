#include "CodeEditor.hpp"

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    // ✅ Set a black background
    QPalette p = palette();
    p.setColor(QPalette::Base, QColor("#000000")); // Black background
    p.setColor(QPalette::Text, QColor("#FFFFFF")); // White text
    setPalette(p);
    setStyleSheet("QPlainTextEdit { background-color: #000000; color: #FFFFFF; }");

    // ✅ Connect cursor movement to highlight the current line
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    highlightCurrentLine(); // Apply highlight immediately
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);
    highlightCurrentLine(); // Apply highlight immediately
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) // ✅ Only highlight if editor is editable
    {
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor(50, 50, 50, 100); // ✅ Dark gray highlight

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection(); // ✅ Apply the highlight to the entire line

        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::setVerticalLineEnabled(bool enabled)
{
    verticalLineEnabled = enabled;
    viewport()->update(); // Force repaint
}

void CodeEditor::setVerticalLineColumn(int column)
{
    if (column >= 0) // Ensure valid column
    {
        verticalLineColumn = column;
        calculateVerticalLineX();  // ✅ Recalculate the X position
        viewport()->update();  // ✅ Force repaint
    }
}

void CodeEditor::paintEvent(QPaintEvent *event)
{
     QPlainTextEdit::paintEvent(event);

    if (!verticalLineEnabled)
        return;

    QPainter painter(viewport());
    painter.setPen(QColor(Qt::red));

    // ✅ Adjust for scrolling
    int xPos = verticalLineX;// - contentOffset().x();

    // ✅ Draw the vertical line in the same position for all lines
    painter.drawLine(xPos, 0, xPos, viewport()->height());
}
void CodeEditor::calculateVerticalLineX()
{
    QFontMetrics fm(this->font());
    
    std::string charOffset = "";
    for(int i = 0; i < verticalLineColumn-1; i++)
    {
        charOffset.push_back(' ');
    }
    QString offset(charOffset.c_str());
    int charWidth = fm.horizontalAdvance(offset); // ✅ Only count spaces (' ')
    // ✅ Calculate the exact position based on N white spaces
    verticalLineX = charWidth;
}

void CodeEditor::setFont(const QFont &font)
{
    QPlainTextEdit::setFont(font); // ✅ Apply new font to editor
    viewport()->update();          // ✅ Force a repaint to realign the vertical line
}
