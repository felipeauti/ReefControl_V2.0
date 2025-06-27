#ifndef MQTT_CLIENT_MANAGER_H
#define MQTT_CLIENT_MANAGER_H

#include <Arduino.h>

class MqttClientManager {
public:
  bool begin() { Serial.println("📡 MQTT stub inicializado"); return true; }
  void loop() {}
  bool isConnected() const { return false; }
  void publishSensorData(float temp, float ph, int tds, int level) {}
  void publishSystemStatus(unsigned long uptime, uint32_t heap, int rssi) {}
};

#endif 