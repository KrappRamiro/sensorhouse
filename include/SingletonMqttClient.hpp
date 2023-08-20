// SingletonMqttClient.hpp
#pragma once
#include <AsyncMqttClient.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <secrets.hpp>

class SingletonMqttClient {
public:
	// Static member function to access the instance
	static AsyncMqttClient& getInstance();

	// Delete the copy constructor and assignment operator
	SingletonMqttClient(const SingletonMqttClient&) = delete;
	SingletonMqttClient& operator=(const SingletonMqttClient&) = delete;

	static Ticker mqttReconnectTimer;
	static Ticker wifiReconnectTimer;
	static WiFiEventHandler wifiConnectHandler;
	static WiFiEventHandler wifiDisconnectHandler;
	static void setup();

private:
	// Private constructor to prevent external instantiation
	SingletonMqttClient();

	// Static member to hold the single instance as a reference
	static AsyncMqttClient instance;
	static void connectToWifi();
	static void connectToMqtt();
	static void onWifiConnect(const WiFiEventStationModeGotIP& event);
	static void onWiFiDisconnect(const WiFiEventStationModeDisconnected& event);
	static void onMqttConnect(bool sessionPresent);
	static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
	static void onMqttPublish(uint16_t packetID);
};