#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ESP8266HTTPUpdateServer.h>

// Forward declarations
class RelayController;
class ConfigManager;

class WebServerManager {
private:
  ESP8266WebServer _server;
  ESP8266HTTPUpdateServer _httpUpdater;
  RelayController* _relayController = nullptr;
  ConfigManager* _configManager = nullptr;
  bool _otaError = false;
  void serveFile(const String& path);

public:
  WebServerManager() : _server(80) {}
  bool begin(RelayController* relayController = nullptr, ConfigManager* configManager = nullptr);
  void handleClient();
};

#endif 