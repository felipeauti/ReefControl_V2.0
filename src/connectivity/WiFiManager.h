#ifndef WIFI_CONNECTION_MANAGER_H
#define WIFI_CONNECTION_MANAGER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

class WiFiConnectionManager {
private:
  bool _connected = false;

public:
  bool begin();
  bool isConnected() const { return _connected; }
};

#endif 