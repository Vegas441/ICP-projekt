//
// Created by david on 25. 4. 2021.
//

#ifndef ICP_MQTTHEADER_H
#define ICP_MQTTHEADER_H
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <mqtt/client.h>
#include <mqtt/exception.h>
#include <mqtt/message.h>
#include <mqtt/callback.h>
#include <mqtt/topic.h>
#include <MQTTAsync.h>
#include <MQTTClient.h>

namespace MQTThead
{
    using namespace std;

    /**
     * @brief Structure for topic and history of messages
     */
    typedef struct topicContents {
        string topic;
        string cont;
        struct topicContents *next;
    }tTopicCont;


    /**
     *
     * @param elem
     * @param msg
     */
    void appendMessage(tTopicCont *head, string topic, string cont){
        if(head->topic == "null"){
            head->topic = topic;
        }

        tTopicCont *temp = (tTopicCont*)malloc(sizeof(tTopicCont));
        temp = head;
        while(temp->topic != topic){
            if(temp->next != nullptr)
                temp = temp->next;
            else{
                tTopicCont *newPtr = (tTopicCont*)malloc(sizeof(tTopicCont));
                newPtr->topic = topic;
                newPtr->cont = cont;
                newPtr->next = nullptr;

                temp->next = newPtr;
                return;
            }
        }
        temp->cont.append(cont);
    }


    /**
     *
     * @param ADDRESS
     * @param USER_ID
     * @param TOPIC
     * @return
     */
    int MQTTfeed(string ADDRESS, string USER_ID, vector<string> TOPICS){

        mqtt::client MQTTclient(ADDRESS, USER_ID);
        mqtt::connect_options connOpts;
        connOpts.set_keep_alive_interval(20);

        tTopicCont *headptr = (tTopicCont*)malloc(sizeof(tTopicCont));
        headptr->topic = "null";
        headptr->next = nullptr;

        try {
            //Connection
            MQTTclient.connect(connOpts);
            cout << "Connected to " << ADDRESS << endl;

            //Subscribing to topics
            for( auto &topic : TOPICS) {
                MQTTclient.subscribe(topic, 0);
            }

            while (true) {
                auto msg = MQTTclient.consume_message();

                if (msg) {
                    if (msg->to_string() == "exit") {
                        cout << "Exit command recieved" << endl;
                        break;
                    }
                    appendMessage(headptr,msg->get_topic(),msg->to_string());
                    cout << msg->get_topic() << ": " << msg->to_string() << endl;
                }
                /*
                else if(!MQTTclient.is_connected()) {
                    cout << "Lost connection" << endl;
                    while(!MQTTclient.is_connected()){
                        this_thread::sleep_for(chrono::milliseconds(250));
                    }
                    cout << "Reestablishing connection" << endl;
                }
                */
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
}

#endif //ICP_MQTTHEADER_H
