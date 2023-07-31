// SingletonMqttClient.h
#pragma once
#include <AsyncMqttClient.h>

class SingletonMqttClient {
public:
	// Static member function to access the instance
	static AsyncMqttClient& getInstance();

	// Delete the copy constructor and assignment operator
	SingletonMqttClient(const SingletonMqttClient&) = delete;
	SingletonMqttClient& operator=(const SingletonMqttClient&) = delete;

private:
	// Private constructor to prevent external instantiation
	SingletonMqttClient();

	// Static member to hold the single instance as a reference
	static AsyncMqttClient instance;
};