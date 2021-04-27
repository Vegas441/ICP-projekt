/**
 * @author David Svaty
 * @date 25.4.2021
 * @brief MQTT client header
 */

#ifndef ICP_MQTTHEADER_H
#define ICP_MQTTHEADER_H
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
namespace MQTThead
{
    using namespace std;

    /**
     * @class class for topic and history of messages
     */
    class tTopicCont {
        public:
            string topic;
            string cont;
            char* last_update;
            tTopicCont *next;

            tTopicCont(){
                topic = "null";
                cont = "null";
                last_update = nullptr;
                next = nullptr;
            }
    };


    /**
     * @brief
     * @param elem
     * @param msg
     */
    void appendMessage(tTopicCont *head, const string& topic, const string& cont){
        if(head->topic == "null"){
            head->topic = topic;
            head->cont = cont;
            head->cont.append("\n");
            const time_t givemetime = time(nullptr);
            head->last_update = asctime(localtime(&givemetime));
            return;
        }

        auto *temp = head;
        while(temp->topic != topic){
            if(temp->next != nullptr)
                temp = temp->next;
            else{
                auto *newPtr = new tTopicCont();
                newPtr->topic = topic;
                newPtr->cont = cont;
                newPtr->cont.append("\n");

                const time_t givemetime = time(nullptr);
                newPtr->last_update = asctime(localtime(&givemetime));
                newPtr->next = nullptr;

                temp->next = newPtr;
                return;
            }
        }
        temp->cont.append(cont);
        temp->cont.append("\n");
        const time_t givemetime = time(nullptr);
        temp->last_update = asctime(localtime(&givemetime));
    }

    /**
     * @brief Prints contents of message structure
     * @param head Pointer to head of linked list
     */
    void print_struct(tTopicCont *head){
        auto *tmp = head;
        cout << "-----STRUCTURE-CONTENTS------" << endl;
        do{
            cout << "--Topic: " << tmp->topic << endl;
            cout << "--Last message recieved at: "  << tmp->last_update;
            cout << "--Messages: " << endl << tmp->cont << endl;
            if(tmp->next == nullptr)
                break;
            tmp = tmp->next;
        }while(true);
        cout << "-----STRUCTURE-CONTENTS------" << endl;

    }

    /**
     * @brief Establishes connection to server and puts messages from subscribed topics into linked list
     * @param ADDRESS server adresss
     * @param USER_ID
     * @param TOPICS vector of topics which to subscribe from
     * @return 1 on error, 0 on success
     */
    int MQTT_subscribe(const string& ADDRESS, const string& USER_ID, const vector<string>& TOPICS){

        mqtt::client MQTTclient(ADDRESS, USER_ID);
        mqtt::connect_options connOpts;
        connOpts.set_keep_alive_interval(20);

        auto *headptr = new tTopicCont();

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
                    if (msg->to_string() == "exit") {
                        cout << "Exit command recieved" << endl;
                        break;
                    }
                    //cout << msg->get_topic() << ": " << msg->to_string() << endl;
                    appendMessage(headptr,msg->get_topic(),msg->to_string());
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
            print_struct(headptr);
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
