/**
 * @file SingletonMqttClient.hpp
 * @author Krapp Ramiro (krappramiro@disroot.org)
 * @brief Provides a singleton wrapper for the MQTT client functionality. It instantiates its own WiFi client
 *
 * Example usage:
 * \code {.cpp}
#include "SingletonMqttClient.hpp"
#include "mqttUtils.hpp"
#include "secrets.hpp"
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Ticker.h>
#include <Wire.h>

float temperature;
float humidity;

AsyncMqttClient& mqttClient = SingletonMqttClient::getInstance();

// create some auxiliary timer variables to publish the readings every 10 seconds
unsigned long previousMillis = 0;
const long interval = 10000;

void setup()
{
	Serial.begin(115200);
	while (!Serial) {
		delay(100);
	}
	SingletonMqttClient::setup();
}

void loop()
{

	unsigned long currentMillis = millis();

	if (currentMillis - previousMillis >= interval) {
		previousMillis = currentMillis;
		float temperature = random(20, 30);
		publishWrapper(mqttClient, "temperature", temperature, 2, true);
	}
}
 * \endcode
 *
 * @version 1.0
 * @date 2023-08-20
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <AsyncMqttClient.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <secrets.hpp>

/**
 * @class SingletonMqttClient
 * @brief Provides a singleton wrapper for the MQTT client functionality. It instantiates its own WiFi client.
 * For example usage, please read the doxygen at the top of the file.
 * Remember to use assing it to a variable using: AsyncMqttClient& mqttClient = SingletonMqttClient::getInstance();
 * and to setup it using SingletonMqttClient::setup();
 */
class SingletonMqttClient {
public:
	/**
	 * @brief Get the instance of the SingletonMqttClient.
	 * @return Reference to the instance.
	 */
	static AsyncMqttClient& getInstance();

	// Delete the copy constructor and assignment operator
	SingletonMqttClient(const SingletonMqttClient&) = delete;
	SingletonMqttClient& operator=(const SingletonMqttClient&) = delete;

	// @brief The ticker timer used for reconnecting to wifi
	static Ticker mqttReconnectTimer;
	static Ticker wifiReconnectTimer;
	static WiFiEventHandler wifiConnectHandler;
	static WiFiEventHandler wifiDisconnectHandler;
	/**
	 * @brief This funcion MUST be called if you want the class to work, it setups the whole Singleton
	 *
	 */
	static void setup();

private:
	// Private constructor to prevent external instantiation
	SingletonMqttClient();
	// Static member to hold the single instance as a reference
	static AsyncMqttClient instance;
	/**
	 * @brief Connect to WiFi, should be called in the setup() so it starts the whole process
	 */
	static void connectToWifi();

	// Static event handlers
	/**
	 * @brief Connect to the MQTT broker.
	 */
	static void connectToMqtt();
	/**
	 * @brief Callback for WiFi connection event.
	 * @param event WiFi connection event.
	 */
	static void onWifiConnect(const WiFiEventStationModeGotIP& event);
	/**
	 * @brief Callback for WiFi disconnection event.
	 * @param event WiFi disconnection event.
	 */
	static void onWiFiDisconnect(const WiFiEventStationModeDisconnected& event);
	/**
	 * @brief Callback for MQTT connection event.
	 * @param sessionPresent Whether a session is present.
	 */
	static void onMqttConnect(bool sessionPresent);
	/**
	 * @brief Callback for MQTT disconnection event.
	 * @param reason Disconnect reason.
	 */
	static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
	/**
	 * @brief Callback for MQTT publish event.
	 * @param packetID Publish packet ID.
	 */
	static void onMqttPublish(uint16_t packetID);
};