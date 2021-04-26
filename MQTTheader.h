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

    typedef struct topicContents {
        string topic;
        string cont;
        struct topicContents *next;
    }tTopicCont;


    /**
     *
     * @param ADDRESS
     * @param USER_ID
     * @param TOPIC
     * @return
     */
    int MQTTfeed(string ADDRESS, string USER_ID, vector<string> TOPIC){

        mqtt::client MQTTclient(ADDRESS, USER_ID);
        mqtt::connect_options connOpts;
        connOpts.set_keep_alive_interval(20);

        try {
            MQTTclient.connect(connOpts);
            cout << "Connected to " << ADDRESS << endl;

            MQTTclient.subscribe(TOPIC,{0,0});

            while(true) {
                auto msg = MQTTclient.consume_message();

                if(msg) {
                    if (msg->get_topic() == "command" &&
                        msg->to_string() == "exit") {
                        cout << "Exit command recieved" << endl;
                        break;
                    }
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
            cout << "Disconnecting" << endl;
            MQTTclient.disconnect();
            cout << "Disconnected"  << endl;

        }
        catch (const mqtt::exception& exc){
            cerr << "Connection error: " << exc.what()
                 << "[" << exc.get_reason_code() << "]" << endl;
            return 1;
        }
        return 0;
    }
}

#endif //ICP_MQTTHEADER_H
