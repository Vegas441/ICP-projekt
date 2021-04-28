CC = g++
CFLAGS = -std=c++17 -pthread -lpaho-mqtt3c -lpaho-mqttpp3

MQTTtest: main.o MQTTheader.o
	$(CC) $(CFLAGS) $< -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) -c $< -o $@

MQTTheader.o: MQTTheader.cpp MQTTheader.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm *.o MQTTtest