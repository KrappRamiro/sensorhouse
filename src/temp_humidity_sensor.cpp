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
		Serial.println("This should be publishing");
		Serial.println(publishWrapper(mqttClient, "temperature", temperature, 2, true));
		Serial.println(publishWrapper(mqttClient, "temperature", 1, 2, true));
		Serial.println(publishWrapper(mqttClient, "motd", "life could be dream", 2, true));
		// mqttClient.publish("test", 2, true, "esto es un test");
	}
}