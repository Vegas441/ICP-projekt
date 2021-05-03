#include <string>
#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QtConcurrent/QtConcurrentRun>
#include "../MQTTheader.h"
#include "../MQTTheader.cpp"
#include <mqtt/client.h>
using namespace MQTThead;

int historyLimit = 0;
QTreeWidgetItem *currentTreeItem = nullptr;
std::tuple <QString, QString> externalView;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    displayTree();
    ui->tabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    if (currentTreeItem != nullptr);
        MainWindow::on_treeWidget_itemClicked(currentTreeItem, 0);
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
    ui->listWidget_2->insertItem(0, item);
}

void addValueItem(Ui::MainWindow *ui, QString text)
{
    QListWidgetItem *item = new QListWidgetItem(ui->listValue);
    item->setText(text);
    ui->listValue->addItem(item);
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    currentTreeItem = item;
    ui->listWidget_2->clear(); // Clear the widgets first
    ui->listValue->clear();
        for(int i = 0; i < int(MQTThead::messageHistory.size()); i++) {
            QString tmp_topic = std::get<0>(messageHistory[i]);
            std::vector<QString> tmp_history = std::get<1>(messageHistory[i]);
            // Parse the topic out of TreeWidget item
            std::string find = item->text(0).toStdString();
            std::string token = find.substr(0, find.find(":"));
            QString item_topic = QString::fromStdString(token);
            //
            if (item_topic == tmp_topic) { // Match topic in message history with item topic
                if (tmp_history.size() < 1) {
                    break;
                }
                int count = 0;
                for( auto &pointer : tmp_history ) { // Display the items in ListWidget
                    QString message = pointer;
                    count++;
                    if (historyLimit != 0) {
                        if (count > historyLimit) {
                            break;
                        }
                    }
                    if (message.size() > 20) {
                        // Shorten the message if necessary
                        std::string tmp_message = message.toStdString();
                        tmp_message = tmp_message.substr(0, 20) + "...";
                        message = QString::fromStdString(tmp_message);
                    }
                    addHistoryItem(ui, QString::fromStdString(std::to_string(count)) + ". " + message);
                }
                // Display current value
                QString current_message = tmp_history.back();
                if (current_message.size() > 50) {
                    // Shorten the message if necessary
                    std::string tmp_message = current_message.toStdString();
                    tmp_message = tmp_message.substr(0, 50) + "...";
                    externalView = std::make_tuple(tmp_topic, current_message);
                    current_message = QString::fromStdString(tmp_message);
                    ui->ExWindowView->setEnabled(true);
                }
                addValueItem(ui, QString::fromStdString("Topic: " + tmp_topic.toStdString() + "\nMessage: " + current_message.toStdString()));
                break;
            }
        }
}

void MainWindow::on_toolButton_clicked()
{
    bool ok;
    QString historyLimitStr;
    historyLimitStr = QInputDialog::getText(this, tr("Settings"), tr("Enter amount of messages to be shown in message history:"), QLineEdit::Normal, historyLimitStr, &ok);
    ui->label_2->setText("Limit: " + historyLimitStr);
    historyLimit = historyLimitStr.toInt();
}


void MainWindow::on_ExWindowView_clicked()
{
    QString message = std::get<1>(externalView);
    ui->tab_3->setEnabled(true);
    ui->textBrowser->clear();
    ui->textBrowser->setText(message);
    ui->tabWidget->setCurrentIndex(1);
}
