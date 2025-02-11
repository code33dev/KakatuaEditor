#include "ProcedureTreeWindow.hpp"
#include <QRegularExpression>
#include <QDebug>

ProcedureTreeWindow::ProcedureTreeWindow(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("PL/I Procedures");
    setGeometry(800, 100, 300, 500);  // Position & size

    QVBoxLayout *layout = new QVBoxLayout(this);
    treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderLabel("Procedures in Open Files");
    
    layout->addWidget(treeWidget);
    setLayout(layout);
}

// ✅ Updates the tree when new code is opened
void ProcedureTreeWindow::updateProcedures(const QString &code, const QString &fileName)
{
    treeWidget->clear();  // Clear old entries
    QTreeWidgetItem *fileItem = new QTreeWidgetItem(treeWidget);
    fileItem->setText(0, fileName);
    treeWidget->addTopLevelItem(fileItem);

    parsePL1Procedures(code, fileItem);
    treeWidget->expandAll();  // Auto-expand tree
}

// ✅ Extracts procedure names using regex
void ProcedureTreeWindow::parsePL1Procedures(const QString &code, QTreeWidgetItem *parentItem)
{
    QRegularExpression procRegex("\\bPROCEDURE\\s+(\\w+)\\b", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator matchIterator = procRegex.globalMatch(code);

    while (matchIterator.hasNext())
    {
        QRegularExpressionMatch match = matchIterator.next();
        QString procName = match.captured(1);

        QTreeWidgetItem *procItem = new QTreeWidgetItem(parentItem);
        procItem->setText(0, procName);
    }
}
// ✅ Updates the procedure tree when switching tabs
