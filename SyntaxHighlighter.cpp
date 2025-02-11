#include "SyntaxHighlighter.hpp"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    QTextCharFormat procedureNameFormat;
    procedureNameFormat.setForeground(QColor("#1E90FF")); // Dodger Blue
    procedureNameFormat.setFontWeight(QFont::Bold);

    QTextCharFormat controlFormat;
    controlFormat.setForeground(QColor("#FFD700")); // Gold
    controlFormat.setFontWeight(QFont::Bold);
    QStringList controlStructures = {
        "\\bIF\\b", "\\bTHEN\\b", "\\bELSE\\b", "\\bSELECT\\b", "\\bWHEN\\b", "\\bOTHERWISE\\b", "\\bEND\\b",
        "\\bDO\\b", "\\bDO UNTIL\\b", "\\bDO WHILE\\b", "\\bDO TO BY\\b", "\\bLEAVE\\b", "\\bITERATE\\b",
        "\\bGOTO\\b", "\\bCALL\\b", "\\bRETURN\\b"};

    // ✅ Logical Operators (Light Blue)
    QTextCharFormat logicalFormat;
    logicalFormat.setForeground(QColor("#00CED1")); // Dark Turquoise
    QStringList logicalOperators = {
        "&", "\\|", "¬", "\\^", "=", "\\^=", "<", ">", "<=", ">="};

    // ✅ Data Types (Orange)
    QTextCharFormat dataTypeFormat;
    dataTypeFormat.setForeground(QColor("#FFA500")); // Orange
    QStringList dataTypes = {
        "\\bBIT\\b", "\\bCHARACTER\\b", "\\bDECIMAL\\b", "\\bFIXED\\b", "\\bFLOAT\\b", "\\bCOMPLEX\\b",
        "\\bPOINTER\\b", "\\bFILE\\b", "\\bLABEL\\b"};

    // ✅ Keywords (Light Green)
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor("#32CD32")); // Lime Green
    QStringList keywords = {
        "\\bDECLARE\\b", "\\bDEFINE\\b", "\\bPROCEDURE\\b", "\\bENTRY\\b", "\\bSTATIC\\b", "\\bAUTOMATIC\\b",
        "\\bCONTROLLED\\b", "\\bBASED\\b", "\\bALIGNED\\b", "\\bAREA\\b", "\\bREFER\\b", "\\bPICTURE\\b",
        "\\bINITIAL\\b", "\\bDEFAULT\\b", "\\bVALUE\\b"};

    // ✅ Reserved Words (Purple)
    QTextCharFormat reservedFormat;
    reservedFormat.setForeground(QColor("#9370DB")); // Medium Purple
    QStringList reservedWords = {
        "\\bBEGIN\\b", "\\bEND\\b", "\\bPACKAGE\\b", "\\bIMPORT\\b", "\\bEXPORT\\b", "\\bINCLUDE\\b",
        "\\bINLINE\\b", "\\bCONDITION\\b", "\\bSIGNAL\\b", "\\bON\\b", "\\bERROR\\b", "\\bSTOP\\b",
        "\\bHALT\\b", "\\bRESIGNAL\\b", "\\bUNDEFINED\\b", "\\bSTORAGE\\b", "\\bUNION\\b", "\\bSTRUCTURE\\b",
        "\\bELEMENT\\b", "\\bSTREAM\\b", "\\bOPEN\\b", "\\bCLOSE\\b", "\\bREAD\\b", "\\bWRITE\\b",
        "\\bGET\\b", "\\bPUT\\b", "\\bSKIP\\b", "\\bPAGE\\b"};

    // ✅ Comments (Dark Green, Italic)
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor("#008000")); // Green
    commentFormat.setFontItalic(true);
    rules.append({QRegularExpression("/\\*.*?\\*/"), commentFormat}); // PL/I Block Comments

    // ✅ Strings (Cyan)
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor("#00FFFF"));                  // Cyan
    rules.append({QRegularExpression("'[^']*'"), stringFormat});    // Single Quotes
    rules.append({QRegularExpression("\"[^\"]*\""), stringFormat}); // Double Quotes
    rules.append({QRegularExpression("\\bPROCEDURE\\s+(\\w+)"), procedureNameFormat});
    rules.append({QRegularExpression("\\bCALL\\s+(\\w+)"), procedureNameFormat});
    // ✅ Add All Patterns with Their Respective Colors
    for (const QString &pattern : controlStructures)
        rules.append({QRegularExpression(pattern), controlFormat});

    for (const QString &pattern : logicalOperators)
        rules.append({QRegularExpression(pattern), logicalFormat});

    for (const QString &pattern : dataTypes)
        rules.append({QRegularExpression(pattern), dataTypeFormat});

    for (const QString &pattern : keywords)
        rules.append({QRegularExpression(pattern), keywordFormat});

    for (const QString &pattern : reservedWords)
        rules.append({QRegularExpression(pattern), reservedFormat});
}

// void SyntaxHighlighter::highlightBlock(const QString &text)
// {
//     for (const HighlightingRule &rule : rules)
//     {
//         QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
//         while (matchIterator.hasNext())
//         {
//             QRegularExpressionMatch match = matchIterator.next();
//             setFormat(match.capturedStart(), match.capturedLength(), rule.format);
//         }
//     }
// }

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : rules)
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            if (match.lastCapturedIndex() == 1)  // ✅ Ensure we're highlighting the procedure name
            {
                setFormat(match.capturedStart(1), match.capturedLength(1), rule.format);
            }
            else
            {
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }
    }
}

