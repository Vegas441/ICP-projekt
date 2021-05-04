//
// Created by david on 28. 4. 2021.
//

#include "MQTTheader.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
#include <regex>
#include <mqtt/client.h>
#include <mqtt/exception.h>
#include <mqtt/message.h>
#include <mqtt/callback.h>
#include <mqtt/topic.h>
#include <MQTTAsync.h>
#include <MQTTClient.h>
//#include <QMainWindow>
//#include "ui_mainwindow.h"
//#include "connectdialog.h"
//#include <QInputDialog>
//#include <QMessageBox>
//#include "mainwindow.h"

using namespace std;
using namespace MQTThead;

namespace  MQTThead{
    // Define vector
    vector<tuple <QString, vector<QString>>> messageHistory;

    vector<tuple <string, vector<string>>> topicTree;
}


MQTThead::tTopicCont::tTopicCont() {
    topic="null";
    recv="null";
    sent="null";
    last_recv_msg="null";
    last_pub_msg="null";
    last_recv="null";
    last_pub="null";
    next=nullptr;
}

void MQTThead::appendMessage(tTopicCont *head, const string& topic, const string& cont, bool recieved){

    time_t givemetime = time(nullptr);
    string ltime(ctime(&givemetime));

    if(head->topic == "null"){
        head->topic = topic;
        if(recieved) {
            head->recv = cont;
            head->recv.append("\n");
            head->last_recv_msg = head->recv;
            head->last_recv = ltime;
        }else{
            head->sent = cont;
            head->sent.append("\n");
            head->last_pub_msg = head->sent;
            head->last_pub = ltime;
        }
        return;
    }

    auto *temp = head;
    while(temp->topic != topic){
        if(temp->next != nullptr)
            temp = temp->next;
        else{
            auto *newPtr = new tTopicCont();
            newPtr->topic = topic;
            if(recieved) {
                newPtr->recv = cont;
                newPtr->recv.append("\n");
                newPtr->last_recv_msg = newPtr->recv;
                newPtr->last_recv = ltime;
            }else{
                newPtr->sent = cont;
                newPtr->sent.append("\n");
                newPtr->last_pub_msg = newPtr->sent;
                newPtr->last_pub = ltime;
            }
            newPtr->next = nullptr;
            temp->next = newPtr;
            return;
        }
    }
    if(recieved) {
        if(temp->recv == "null")
            temp->recv = cont;
        else
            temp->recv.append(cont);
        temp->recv.append("\n");
        temp->last_recv = ltime;
        temp->last_recv_msg = cont;
        temp->last_recv_msg.append("\n");
    }else{
        if(temp->sent == "null")
            temp->sent = cont;
        else
            temp->sent.append(cont);
        temp->sent.append("\n");
        temp->last_pub = ltime;
        temp->last_pub_msg = cont;
        temp->last_pub_msg.append("\n");
    }
}

void MQTThead::print_struct(tTopicCont *head){
    auto *tmp = head;
    cout << "-----RECIEVED-MESSAGES------" << endl;
    do{
        if(tmp->recv != "null") {
            cout << "--Topic: " << tmp->topic << endl;
            cout << "--Last message recieved at: " << tmp->last_recv;
            cout << "--Messages: " << endl << tmp->recv << endl;
            cout << "--Last message: " << tmp->last_recv_msg << endl;
        }
        if (tmp->next == nullptr)
            break;
        tmp = tmp->next;
    }while(true);
    cout << "-----RECIEVED-MESSAGES------" << endl << endl;

    tmp = head;
    cout << "-----SENT-MESSAGES------" << endl;
    do{
        if(tmp->sent != "null") {
            cout << "--Topic: " << tmp->topic << endl;
            cout << "--Last message published at: " << tmp->last_pub;
            cout << "--Messages: " << endl << tmp->sent << endl;
            cout << "--Last message: " << tmp->last_pub_msg << endl;
        }
        if (tmp->next == nullptr)
            break;
        tmp = tmp->next;
    }while(true);
    cout << "-----SENT-MESSAGES------" << endl;
}

void MQTThead::get_subtopic_tree(tTopicCont *headptr){
    auto *tmp = headptr;
    int i=0;
    vector<vector<string>> v_vector;
    vector<string> root_vector;

    //puts all subtopics into v_vector
    do{
        if(regex_match(tmp->topic,regex("[A-Za-z0-9]*/[A-Za-z0-9#_]*"))) {
            string s = tmp->topic.substr(0, tmp->topic.find("/"));
            //regex expr ();
            root_vector.push_back(s);    //pushes root topics
            auto *tmpx = headptr;
            do {
                if (regex_match(tmpx->topic, regex("[A-Za-z0-9]*/[A-Za-z0-9#_]*"))
                    && s.compare(tmp->topic.substr(0, tmpx->topic.find("/"))) == 0)
                    v_vector[i].push_back(tmpx->topic);  //pushes subtopics
                if (tmpx->next == nullptr)
                    break;
                tmpx = tmpx->next;
            } while (tmpx->next != nullptr);
            i++;
        }
        if(tmp->next == nullptr)
            break;
        tmp = tmp->next;
    }while(tmp->next != nullptr);

    i=0;
    for( auto &s : root_vector ){
        tuple<string,vector<string>>tpl = make_tuple(s,v_vector[i]);
        topicTree.push_back(tpl);
        i++;
    }

    //puts single topics(without subtopics) into tree
    tmp = headptr;
    do{
        if (regex_match(tmp->topic,regex("[A-Za-z0-9#_]*"))){
            string s = tmp->topic;
            bool match = false;
            for( auto &tpl : topicTree ){
                if(s.compare(get<0>(tpl)) == 0) {
                    match = true;
                    break;
                }
            }
            if(!match){
                tuple<string,vector<string>> tpl = make_tuple(s,vector<string>());
                topicTree.push_back(tpl);
            }
        }
        if(tmp->next == nullptr)
            break;
        tmp = tmp->next;
    }while(tmp->next != nullptr);
}

int MQTThead::MQTT_subscribe(const string& ADDRESS, const string& USER_ID, const vector<string>& TOPICS, tTopicCont *headptr, Ui::MainWindow *ui){

    mqtt::client MQTTclient(ADDRESS, USER_ID);
    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(10);

    try {
        int messageCnt = 0;
        //Connection
        MQTTclient.connect(connOpts);
        cout << "Connected to " << ADDRESS << endl;
        vector<QTreeWidgetItem*> topicList;
         //Subscribing to topics
         for( auto &topic : TOPICS ) {
             QTreeWidgetItem *result;
             MQTTclient.subscribe(topic, 0);
             result = setTreeRoot(ui, QString::fromStdString(topic));
             topicList.push_back(result);
             // Pushback topics with empty message vector
             std::vector<QString> history;
             messageHistory.push_back(std::make_tuple(QString::fromStdString(topic), history));
         }
         while (true) {
             auto msg = MQTTclient.consume_message();

             if (msg) {
                 messageCnt ++;
                 //TODO exit bude ovladany uzivatelom
                 if (msg->to_string() == "exit") {
                     cout << "Exit command recieved" << endl;
                     break;
                 }
                 cout << msg->get_topic() << ": " << msg->to_string() << endl;
                 // Update GUI
                 int i = 0;
                 for( auto &topic : TOPICS ) {
                     // Find matching topic from initial topic list
                     if (topic == msg->get_topic()) {
                         string message = msg->to_string(); // For TreeWidget only
                         // Shorten the topic to fit the widget
                         if (msg->to_string().size() > 8)
                             message = message.substr(0, 8) + "...";
                         // Update TreeWidget item
                         updateTreeChild(topicList[i], QString::fromStdString(msg->get_topic() + ": " + message +
                                                                              "    [messages: " + std::to_string(messageCnt) + "]"));
                         // Update tuple
                         std::vector<QString> tmp_history = std::get<1>(messageHistory[i]); // Get vector of messages
                         std::tuple <QString, std::vector<QString>> tmp_tuple = messageHistory[i]; // Get temporary tuple
                         tmp_history.push_back(QString::fromStdString(msg->to_string())); // Add newest message to history
                         // Replace original tuple with temporary one with newest message added to history
                         messageHistory[i] = std::make_tuple(QString::fromStdString(msg->get_topic()), tmp_history);
                         break;
                     }
                     i++;
                 }
                 appendMessage(headptr,msg->get_topic(),msg->to_string(),true);
             }
         }

         //Disconnection
         MQTTclient.disconnect();
         cout << "Disconnected" << endl;

    }
    catch (const mqtt::exception &exc) {
        cerr << "Connection error: " << exc.what()
        << "[" << exc.get_reason_code() << "]" << endl;
        return 1;
    }
    return 0;
}

int MQTThead::MQTT_publish(const string& msg, const string& ADDRESS, const string& USER_ID, const vector<string>& TOPICS, tTopicCont *headptr){

    mqtt::client MQTTclient(ADDRESS, USER_ID);
    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(10);

    try{
        MQTTclient.connect(connOpts);
        cout << "Connected to " << ADDRESS << endl;

        for( auto &topic : TOPICS ){
            auto pub_msg = mqtt::make_message(topic,msg);
            pub_msg->set_qos(0);
            MQTTclient.publish(pub_msg);
            appendMessage(headptr, topic, msg, false);
        }

        //Disconnection
        MQTTclient.disconnect();
        cout << "Disconnected" << endl;

    }
    catch(const mqtt::exception &exc){
        cerr << "Connection error: " << exc.what()
             << "[" << exc.get_reason_code() << "]" << endl;
        return 1;
    }
    return 0;
}

