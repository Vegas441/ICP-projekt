#include <string>
#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrentRun>
#include "../MQTTheader.h"
#include "../MQTTheader.cpp"
#include <mqtt/client.h>
using namespace MQTThead;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    displayTree();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::on_actionConnect_triggered()
{
    bool ok;
    QString adress;
    QMessageBox msgBox;
    adress = QInputDialog::getText(this, tr("Connect"), tr("Adress:"), QLineEdit::Normal, adress, &ok);
    if (ok && !adress.isEmpty()) {
        msgBox.setText("Connecting to:");
        msgBox.setInformativeText(adress);
        msgBox.setWindowTitle("");
        msgBox.exec();
    }

    auto *headptr = new MQTThead::tTopicCont();

    std::string ADDRESS = "tcp://localhost:1883";
    std::string CL_ID = "paho_cpp_sync_consume";
    std::vector<std::string> TOPICS = { "command" , "misc1" , "misc2"};

    QFuture<void> future = QtConcurrent::run(&MQTThead::MQTT_subscribe, ADDRESS, CL_ID, TOPICS, headptr, ui);

    MQTThead::print_struct(headptr);
}

void MainWindow::displayTree()
{
    QString label = "Topics";
    ui->treeWidget->setHeaderLabel(label);
    ui->treeWidget->setColumnCount(1);
}

QTreeWidgetItem* setTreeChild(QTreeWidgetItem *root, QString text)
{
    QTreeWidgetItem *child = new QTreeWidgetItem();
    child->setText(0, text);
    root->addChild(child);
    return child;
}

QTreeWidgetItem* setTreeRoot(Ui::MainWindow *ui, QString text)
{
    QTreeWidgetItem *root = new QTreeWidgetItem(ui->treeWidget);
    root->setText(0, text);
    ui->treeWidget->addTopLevelItem(root);
    return root;
}

void updateTreeChild(QTreeWidgetItem *child, QString text)
{
    child->setText(0, text);
}

void addHistoryItem(Ui::MainWindow *ui, QString text)
{
    QListWidgetItem *item = new QListWidgetItem(ui->listWidget_2);
    item->setText(text);
    ui->listWidget_2->addItem(item);
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    ui->listWidget_2->clear(); // Clear the widget first
        for(int i = 0; i < int(MQTThead::messageHistory.size()); i++) {
            QString tmp_topic = std::get<0>(messageHistory[i]);
            std::vector<QString> tmp_history = std::get<1>(messageHistory[i]);
            // Parse the topic out of TreeWidget item
            std::string find = item->text(0).toStdString();
            std::string token = find.substr(0, find.find(":"));
            QString item_topic = QString::fromStdString(token);
            //
            if (item_topic == tmp_topic) { // Match topic in message history with item topic
                int count = 0;
                for( auto &message : tmp_history ) { // Display the items in ListWidget
                    count++;
                    addHistoryItem(ui, QString::fromStdString(std::to_string(count)) + ". " + message);
                }
                break;
            }
        }
}
