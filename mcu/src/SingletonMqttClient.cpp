// SingletonMqttClient.cpp
#include "SingletonMqttClient.hpp"

// Initialize the static member
AsyncMqttClient SingletonMqttClient::instance;
Ticker SingletonMqttClient::mqttReconnectTimer;
Ticker SingletonMqttClient::wifiReconnectTimer;
WiFiEventHandler SingletonMqttClient::wifiConnectHandler;
WiFiEventHandler SingletonMqttClient::wifiDisconnectHandler;

AsyncMqttClient& SingletonMqttClient::getInstance()
{
	// Return the reference to the instance
	return instance;
}

SingletonMqttClient::SingletonMqttClient()
{
}
void SingletonMqttClient::setup()
{
	Serial.println("Started setup of Singleton Mqtt Client");

	// Private constructor
	// assign callbacks that will handle what happens when the ESP connects or disconnects to your Wi-Fi connection
	wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect); // Assigns the onWifiConnect function to the onStationModeGotIP event
	wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWiFiDisconnect);

	// assign all the callbacks functions. These functions will be executed automatically when needed
	instance.onConnect(onMqttConnect);
	instance.onDisconnect(onMqttDisconnect);
	instance.onPublish(onMqttPublish);

	instance.setServer(MQTT_HOST, MQTT_PORT);
	// instance.setCredentials(MQTT_USER, MQTT_PASSWORD);
	instance.setClientId("ESP8266-test");

	// After the setup of all the callbacks, connect to wifi
	connectToWifi();
	Serial.println("Finished setup of Singleton Mqtt Client");
}

void SingletonMqttClient::connectToWifi()
{
	Serial.printf("Connecting to Wifi %s\n", WIFI_SSID);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void SingletonMqttClient::connectToMqtt()
{
	Serial.printf("Connecting to MQTT broker: %s:%i\n", MQTT_HOST, MQTT_PORT);
	SingletonMqttClient::getInstance().connect();
}

void SingletonMqttClient::onWifiConnect(const WiFiEventStationModeGotIP& event)
{
	Serial.println("Connected to Wifi!");
	connectToMqtt();
}

void SingletonMqttClient::onWiFiDisconnect(const WiFiEventStationModeDisconnected& event)
{
	Serial.println("Disconnected from WiFi");
	mqttReconnectTimer.detach();
	wifiReconnectTimer.once(2, SingletonMqttClient::connectToWifi);
}

void SingletonMqttClient::onMqttConnect(bool sessionPresent)
{
	Serial.println("Connected to MQTT Broker!.");
	Serial.printf("Session %s present\n", sessionPresent ? "is" : "is not");
}

void SingletonMqttClient::onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
	Serial.printf("Disconnected from MQTT, reason: %i\n", reason);
	if (WiFi.isConnected()) {
		mqttReconnectTimer.once(2, SingletonMqttClient::connectToMqtt);
	}
}

void SingletonMqttClient::onMqttPublish(uint16_t packetID)
{
	Serial.printf("Publish acknowledged with packedID: %u\n", packetID);
}
