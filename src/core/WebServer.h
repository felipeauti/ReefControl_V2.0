#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ESP8266HTTPUpdateServer.h>

class WebServerManager {
private:
  ESP8266WebServer _server;
  ESP8266HTTPUpdateServer _httpUpdater;
  bool _otaError = false;
  void serveFile(const String& path);

public:
  WebServerManager() : _server(80) {}
  bool begin();
  void handleClient();
};

#endif 