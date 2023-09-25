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

float getRandomFloat(long min, long max)
{
	float randomTemperature = random(min, max);
	// Generate a random integer between 0 and 99
	int randomInt = random(100);
	// Convert the integer to a float and divide by 100 to get a decimal between 0.00 and 0.99
	float randomDecimal = static_cast<float>(randomInt) / 100.0f;
	return randomTemperature + randomDecimal;
}
float getTemperature()
{
	return getRandomFloat(-20, 60);
}

float getHumidity()
{
	return getRandomFloat(0, 99);
}

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

	if (currentMillis - previousMillis < interval) {
		// Return early if the condition is not met
		return;
	}
	previousMillis = currentMillis;

	float temperature = getTemperature();
	float humidity = getHumidity();
	publishWrapper(mqttClient, "temperature", temperature, 2, true);
	publishWrapper(mqttClient, "humidity", humidity, 2, true);
}