#include "FindWindow.hpp"

FindWindow::FindWindow(QPlainTextEdit *editor, QWidget *parent)
    : QWidget(parent), textEditor(editor)
{
    setWindowTitle("Find");

    QLabel *label = new QLabel("Find:");
    searchBox = new QLineEdit(this);

    nextButton = new QPushButton("Next", this);
    prevButton = new QPushButton("Previous", this);
    highlightButton = new QPushButton("Highlight All", this);
    caseCheckBox = new QCheckBox("Match Case", this);
    regexCheckBox = new QCheckBox("Use Regular Expression", this);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(label);
    topLayout->addWidget(searchBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(prevButton);
    buttonLayout->addWidget(nextButton);
    buttonLayout->addWidget(highlightButton);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(caseCheckBox);
    mainLayout->addWidget(regexCheckBox);

    setLayout(mainLayout);

    connect(nextButton, &QPushButton::clicked, this, &FindWindow::findNext);
    connect(prevButton, &QPushButton::clicked, this, &FindWindow::findPrevious);
    connect(highlightButton, &QPushButton::clicked, this, &FindWindow::highlightAllMatches);
}

void FindWindow::closeEvent(QCloseEvent *event)
{
    removeHighlights();
    QWidget::closeEvent(event);
    delete this;
}

void FindWindow::findNext()
{
    search(QTextDocument::FindFlags());
}

void FindWindow::findPrevious()
{
    search(QTextDocument::FindBackward);
}

void FindWindow::removeHighlights()
{
    if (!textEditor)
        return;

    QTextDocument *document = textEditor->document();
    QTextCursor cursor(document);

    cursor.select(QTextCursor::Document);
    QTextCharFormat defaultFormat;
    defaultFormat.setBackground(Qt::transparent);
    cursor.mergeCharFormat(defaultFormat);
}

void FindWindow::highlightAllMatches()
{
    if (!textEditor)
        return;

    QTextDocument *document = textEditor->document();
    QTextCursor highlightCursor(document);
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::yellow);

    QTextCursor defaultCursor(document);
    defaultCursor.select(QTextCursor::Document);
    QTextCharFormat defaultFormat;
    defaultFormat.setBackground(Qt::transparent);
    defaultCursor.mergeCharFormat(defaultFormat);

    QString searchText = searchBox->text();
    if (searchText.isEmpty())
        return;

    QTextDocument::FindFlags flags;
    if (caseCheckBox->isChecked())
        flags |= QTextDocument::FindCaseSensitively;

    bool found = false;

    while (!highlightCursor.isNull() && !highlightCursor.atEnd())
    {
        highlightCursor = document->find(searchText, highlightCursor, flags);
        if (!highlightCursor.isNull())
        {
            highlightCursor.mergeCharFormat(highlightFormat);
            found = true;
        }
    }

    if (!found)
    {
        QMessageBox::information(this, "Find", "No matches found.");
    }
}

void FindWindow::search(QTextDocument::FindFlags flags)
{
    if (!textEditor)
        return;

    QString searchText = searchBox->text();
    if (searchText.isEmpty())
        return;

    if (caseCheckBox->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    if (regexCheckBox->isChecked())
        searchText = QRegularExpression(searchText).pattern();

    QTextCursor cursor = textEditor->textCursor();
    cursor = textEditor->document()->find(searchText, cursor, flags);

    if (!cursor.isNull())
    {
        textEditor->setTextCursor(cursor);
        lastMatchPos = cursor.position();
    }
    else
    {
        QMessageBox::information(this, "Find", "No more matches found.");
    }
}
