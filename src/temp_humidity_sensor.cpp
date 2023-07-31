#include "SingletonMqttClient.hpp"
#include "secrets.hpp"
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Ticker.h>
#include <Wire.h>

#define PUB_TEMPERATURE
#define PUB_HUMIDITY
#define PLACE "habitacion_ramiro" // TODO: Pasar esto como flag de compilacion
#define DEVICE_NAME "" // TODO: esto como flag de compilacion

float temperature;
float humidity;

AsyncMqttClient& mqttClient = SingletonMqttClient::getInstance();

Ticker mqttReconnectTimer;
Ticker wifiReconnectTimer;
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

// create some auxiliary timer variables to publish the readings every 10 seconds
unsigned long previousMillis = 0;
const long interval = 10000;

// ================       Callback function - they are executed asynchronously.       ================ //
#pragma region callback
/// @brief connects your ESP8266 to your router:
void connectToWifi()
{
	Serial.printf("Connecting to Wifi %s\n", WIFI_SSID);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

/// @brief connects your ESP8266 to your MQTT broker
void connectToMqtt()
{
	Serial.printf("Connecting to MQTT broker: %s:%i\n", MQTT_HOST, MQTT_PORT);
	mqttClient.connect();
}

/// @brief after a successful connection with the router and MQTT broker, it prints the ESP8266 IP address
void onWifiConnect(const WiFiEventStationModeGotIP& event)
{
	Serial.println("Connected to Wifi!");
	connectToMqtt();
}

/// @brief if the connection is lost, it starts a timer and tries to reconnect.
void onWiFiDisconnect(const WiFiEventStationModeDisconnected& event)
{
	Serial.println("Disconnected from WiFi");
	mqttReconnectTimer.detach();
	wifiReconnectTimer.once(2, connectToWifi);
}

/// @brief This function runs after starting a session with the broker.
/// @param sessionPresent
void onMqttConnect(bool sessionPresent)
{
	Serial.println("Connected to MQTT Broker!.");
	Serial.printf("Session %s present\n", sessionPresent ? "is" : "is not");
}

/// @brief If the ESP8266 loses connection with the MQTT broker, it calls this function which prints a message in the serial monitor and tries to reconnect
/// @param reason
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
	Serial.printf("Disconnected from MQTT, reason: %i\n", reason);
	if (WiFi.isConnected()) {
		mqttReconnectTimer.once(2, connectToMqtt);
	}
}

/// @brief When you publish a message to an MQTT topic, the onMqttPublish() function is called. It prints the packet id in the Serial Monitor.
/// @param packetID
void onMqttPublish(uint16_t packetID)
{
	Serial.printf("Publish acknowledged with packedID: %u\n", packetID);
}
#pragma endregion callback
// ==============================          End of callback functions          ======================== //

template <typename T>
uint16_t publishWrapper(AsyncMqttClient& asyncMqttClient, const char* item, const T& payload, uint8_t qos, bool retain)
{
	// Assuming there is a PLACE string defined via compilation flag
	char topic[256];
	snprintf(topic, sizeof(topic), "%s/set/%s", PLACE, item); // Safely concatenate the strings

	if constexpr (std::is_same_v<T, float>) {
		char payloadStr[20]; // A float with 18 digits and 2 decimals should fit within 20 characters
		snprintf(payloadStr, sizeof(payloadStr), "%.2f", payload);
		return asyncMqttClient.publish(topic, qos, retain, payloadStr);
	} else {
		// For other types, publish as is
		// Make sure the payload type can be converted to a string (e.g., int, char[], etc.)
		// If not, handle the appropriate conversion here or raise a compilation error.
		// Example: return asyncMqttClient.publish(topic, qos, retain, String(payload).c_str());
		return asyncMqttClient.publish(topic, qos, retain, String(payload).c_str());
	}
}

void setup()
{
	Serial.begin(115200);
	while (!Serial) {
		delay(100);
	}
	// assign callbacks that will handle what happens when the ESP connects or disconnects to your Wi-Fi connection
	wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect); // Assigns the onWifiConnect function to the onStationModeGotIP event
	wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWiFiDisconnect);

	// assign all the callbacks functions. These functions will be executed automatically when needed
	mqttClient.onConnect(onMqttConnect);
	mqttClient.onDisconnect(onMqttDisconnect);
	mqttClient.onPublish(onMqttPublish);

	mqttClient.setServer(MQTT_HOST, MQTT_PORT);
	// mqttClient.setCredentials(MQTT_USER, MQTT_PASSWORD);
	mqttClient.setClientId("ESP8266-test");

	connectToWifi();
}

void loop()
{
	unsigned long currentMillis = millis();

	if (currentMillis - previousMillis >= interval) {
		previousMillis = currentMillis;
		float temperature = random(20, 30);
		publishWrapper(mqttClient, "temperature", temperature, 2, true);
		publishWrapper(mqttClient, "temperature", 1, 2, true);
		publishWrapper(mqttClient, "motd", "life could be dream", 2, true);
		// mqttClient.publish("test", 2, true, "esto es un test");
	}
}