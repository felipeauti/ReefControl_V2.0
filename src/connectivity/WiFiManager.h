#ifndef WIFI_CONNECTION_MANAGER_H
#define WIFI_CONNECTION_MANAGER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>  // Biblioteca tzapu/WiFiManager

class WiFiConnectionManager {
private:
  WiFiManager _wifiManager;
  bool _connected = false;

public:
  bool begin();
  bool isConnected() const { return _connected; }
  void reset();
  void setConfigPortalTimeout(int timeout);
  void setAPCallback(std::function<void(WiFiManager*)> func);
};

#endif 