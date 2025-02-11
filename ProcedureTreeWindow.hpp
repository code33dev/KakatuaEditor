#ifndef PROCEDURETREEWINDOW_HPP
#define PROCEDURETREEWINDOW_HPP

#include <QWidget>
#include <QTreeWidget>
#include <QString>
#include <QVBoxLayout>
#include <QTabWidget>

class ProcedureTreeWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ProcedureTreeWindow(QWidget *parent = nullptr);
    void updateProcedures(const QString &code, const QString &fileName);

private:
    QTreeWidget *treeWidget;

    void parsePL1Procedures(const QString &code, QTreeWidgetItem *parentItem);
};

#endif // PROCEDURETREEWINDOW_HPP
