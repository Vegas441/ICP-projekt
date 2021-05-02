#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include <QInputDialog>
#include <QMessageBox>
#include "../MQTTheader.h"
using namespace MQTThead;



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_actionConnect_triggered();

    void displayTree();


    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::MainWindow *ui;
};

QTreeWidgetItem* setTreeChild(QTreeWidgetItem *root, QString text);

void updateTreeChild(QTreeWidgetItem *root, QString text);

QTreeWidgetItem* setTreeRoot(Ui::MainWindow *ui, QString text);

void addHistoryItem(Ui::MainWindow *ui, QString text);

#endif // MAINWINDOW_H
