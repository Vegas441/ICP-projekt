#include <iostream>
#include <string>
#include "MQTTheader.h"
#include "MQTTheader.cpp"

/**
 *
 * @return
 */
int main() {
    using namespace std;
    /**
     * Address and client id will be taken from GUI
    */

    //TODO headptr bude vo finalnom programe inicializovany globalne
    auto *headptr = new MQTThead::tTopicCont();

    string ADDRESS = "tcp://localhost:1883";
    string CL_ID = "paho_cpp_sync_consume";
    vector<string> TOPICS = { "command" , "misc1" , "misc2"};

    MQTThead::MQTT_subscribe(ADDRESS,CL_ID,TOPICS,headptr);

    string PAYLOAD = "Hello World, this is a test message";
    MQTThead::MQTT_publish(PAYLOAD,ADDRESS,CL_ID,TOPICS,headptr);

    MQTThead::print_struct(headptr);

}
