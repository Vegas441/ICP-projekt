//
// Created by david on 28. 4. 2021.
//

#include "MQTTheader.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
#include <mqtt/client.h>
#include <mqtt/exception.h>
#include <mqtt/message.h>
#include <mqtt/callback.h>
#include <mqtt/topic.h>
#include <MQTTAsync.h>
#include <MQTTClient.h>

//TODO repair date
using namespace std;
using namespace MQTThead;

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


int MQTThead::MQTT_subscribe(const string& ADDRESS, const string& USER_ID, const vector<string>& TOPICS, tTopicCont *headptr){

    mqtt::client MQTTclient(ADDRESS, USER_ID);
    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(10);

    try {
        //Connection
        MQTTclient.connect(connOpts);
        cout << "Connected to " << ADDRESS << endl;

         //Subscribing to topics
         for( auto &topic : TOPICS ) {
             MQTTclient.subscribe(topic, 0);
         }

         while (true) {
             auto msg = MQTTclient.consume_message();

             if (msg) {
                 //TODO exit bude ovladany uzivatelom
                 if (msg->to_string() == "exit") {
                     cout << "Exit command recieved" << endl;
                     break;
                 }
                 //cout << msg->get_topic() << ": " << msg->to_string() << endl;
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

