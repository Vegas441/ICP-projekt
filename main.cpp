#include <iostream>
#include <string>
#include "MQTTheader.h"

/**
 *
 * @return
 */
int main() {
    using namespace std;

    /**
     * Address and client id will be taken from GUI
    cout << "Address: ";
    cin >> ADDRESS;
    cout << "Client id: ";
    cin >> CL_ID;
    */

    string ADDRESS = "tcp://localhost:1883";
    string CL_ID = "paho_cpp_sync_consume";
    vector<string> TOPICS = { "command" , "misc" };

    MQTThead::MQTTfeed(ADDRESS,CL_ID,TOPICS);



}
