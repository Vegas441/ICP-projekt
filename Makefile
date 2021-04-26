MQTT: MQTT.o
	g++ -std=c++17 -pthread -lpaho-mqtt3c -lpaho-mqttpp3 MQTT.o -o MQTTtest

MQTT.o: main.cpp
	g++ -c -std=c++17 -pthread -lpaho-mqtt3c -lpaho-mqttpp3 main.cpp -o MQTT.o

clean:
	-rm *.o $(objects)