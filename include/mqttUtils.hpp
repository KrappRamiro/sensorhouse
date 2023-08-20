#pragma once
#include "Arduino.h"
#include "AsyncMqttClient.hpp"

/*
Templates dont need to go in a separate cpp file https://stackoverflow.com/questions/13269116/template-classes-multiple-file-project-how-to
*/

/// @brief This function publishes a payload to the broker
/// @tparam T
/// @param asyncMqttClient The async mqtt client object to use
/// @param item The item topic to use
/// @param payload The payload to publish
/// @param qos the mqtt QoS to use (0, 1 or 2)
/// @param retain Should the topic be retained?
/// @return The result of the operation
template <typename T>
uint16_t publishWrapper(AsyncMqttClient& asyncMqttClient, const char* item, const T& payload, uint8_t qos, bool retain)
{

	// Assuming there is a PLACE string defined via compilation flag
	char topic[256];
	snprintf(topic, sizeof(topic), "%s/set/%s", PLACE, item); // Safely concatenate the strings

	if constexpr (std::is_same_v<T, float>) { // If the payload is a float
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
