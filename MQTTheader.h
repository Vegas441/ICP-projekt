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

            string recv;
            string sent;
            string last_recv;
            string last_pub;
            tTopicCont *next;

            tTopicCont();
    };


    /**
     * @brief
     * @param elem
     * @param msg
     * @param recieved Flag indicating whether message was recieved from server or sent
     */
    void appendMessage(tTopicCont *head, const string& topic, const string& cont, bool recieved);

    /**
     * @brief Prints contents of message structure
     * @param head Pointer to head of linked list
     */
    void print_struct(tTopicCont *head);

    /**
     * @brief Establishes connection to server and puts messages from subscribed topics into linked list
     * @param ADDRESS server adresss
     * @param USER_ID
     * @param TOPICS vector of topics which to subscribe from
     * @param headptr Pointer to message structure
     * @return 1 on error, 0 on success
     */
    int MQTT_subscribe(const string& ADDRESS, const string& USER_ID, const vector<string>& TOPICS, tTopicCont *headptr);

    /**
     * @brief Establishes connection to server and publishes messages to topics
     * @param msg Message to be sent
     * @param ADDRESS Server address
     * @param USER_ID
     * @param TOPICS vector of topics to publish to
     * @param headptr Pointer to message structure
     * @return 1 on error, 0 on success
     */
    int MQTT_publish(const string& msg, const string& ADDRESS, const string& USER_ID, const vector<string>& TOPICS, tTopicCont *headptr);
}

#endif //ICP_MQTTHEADER_H
