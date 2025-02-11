#include "SyntaxHighlighter.hpp"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent) 
    : QSyntaxHighlighter(parent)
{
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::yellow);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QStringList keywordPatterns = {
        "\\bint\\b", "\\bdouble\\b", "\\bfloat\\b", "\\bchar\\b",
        "\\breturn\\b", "\\bvoid\\b", "\\bif\\b", "\\belse\\b",
        "\\bfor\\b", "\\bwhile\\b", "\\bclass\\b", "\\bpublic\\b",
        "\\bprivate\\b", "\\bprotected\\b", "\\bnamespace\\b"
    };

    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::darkGreen);
    commentFormat.setFontItalic(false);

    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::white);

    for (const QString &pattern : keywordPatterns)
    {
        rules.append({QRegularExpression(pattern), keywordFormat});
    }

    rules.append({QRegularExpression("//[^\n]*"), commentFormat});
    rules.append({QRegularExpression("\".*?\""), stringFormat});
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : rules)
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
