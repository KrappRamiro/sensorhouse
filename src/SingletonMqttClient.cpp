// SingletonMqttClient.cpp

#include "SingletonMqttClient.hpp"

// Initialize the static member
AsyncMqttClient SingletonMqttClient::instance;

AsyncMqttClient& SingletonMqttClient::getInstance()
{
	// Return the reference to the instance
	return instance;
}

SingletonMqttClient::SingletonMqttClient()
{
	// Private constructor
	
}
