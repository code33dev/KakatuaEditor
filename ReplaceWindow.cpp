#include "ReplaceWindow.hpp"

ReplaceWindow::ReplaceWindow(QPlainTextEdit *editor, QWidget *parent)
    : QWidget(parent), textEditor(editor)
{
    setWindowTitle("Replace");
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);

    QLabel *replaceLabel = new QLabel("Replace:");
    replaceBox = new QLineEdit(this);

    QLabel *replaceWithLabel = new QLabel("With:");
    replaceWithBox = new QLineEdit(this);

    replaceButton = new QPushButton("Replace", this);
    replaceAllButton = new QPushButton("Replace All", this);

    caseCheckBox = new QCheckBox("Match Case", this);
    regexCheckBox = new QCheckBox("Use Regular Expression", this);

    QHBoxLayout *replaceLayout = new QHBoxLayout();
    replaceLayout->addWidget(replaceLabel);
    replaceLayout->addWidget(replaceBox);

    QHBoxLayout *replaceWithLayout = new QHBoxLayout();
    replaceWithLayout->addWidget(replaceWithLabel);
    replaceWithLayout->addWidget(replaceWithBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(replaceButton);
    buttonLayout->addWidget(replaceAllButton);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(replaceLayout);
    mainLayout->addLayout(replaceWithLayout);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(caseCheckBox);
    mainLayout->addWidget(regexCheckBox);

    setLayout(mainLayout);

    connect(replaceButton, &QPushButton::clicked, this, &ReplaceWindow::replaceText);
    connect(replaceAllButton, &QPushButton::clicked, this, &ReplaceWindow::replaceAllText);
}

void ReplaceWindow::closeEvent(QCloseEvent *event)
{
    delete this;
}

void ReplaceWindow::replaceText()
{
    if (!textEditor)
        return;

    QTextCursor cursor = textEditor->textCursor();
    if (!cursor.hasSelection())
    {
        cursor = textEditor->document()->find(replaceBox->text(), cursor);
        textEditor->setTextCursor(cursor);
    }

    if (cursor.hasSelection())
    {
        cursor.insertText(replaceWithBox->text());
    }

    textEditor->update();
    textEditor->repaint();
}

void ReplaceWindow::replaceAllText()
{
    if (!textEditor)
        return;

    QString searchText = replaceBox->text();
    QString replaceText = replaceWithBox->text();
    if (searchText.isEmpty())
        return;

    QTextDocument *doc = textEditor->document();
    QTextCursor cursor(doc);
    cursor.beginEditBlock();

    QTextDocument::FindFlags flags;
    if (caseCheckBox->isChecked())
        flags |= QTextDocument::FindCaseSensitively;

    bool replaced = false;

    while (!cursor.isNull() && !cursor.atEnd())
    {
        cursor = doc->find(searchText, cursor, flags);
        if (!cursor.isNull())
        {
            cursor.insertText(replaceText);
            replaced = true;
        }
    }

    cursor.endEditBlock();
    textEditor->setTextCursor(cursor);

    if (!replaced)
    {
        QMessageBox::information(this, "Replace All", "No matches found.");
    }

    textEditor->update();
    textEditor->repaint();
}
