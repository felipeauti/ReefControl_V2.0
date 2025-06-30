#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>

// Compatibilidade ESP8266/ESP32
#ifdef ESP32
  #include <WebServer.h>
  #include <SPIFFS.h>
  #include <Update.h>
  #define WebServerClass WebServer
  #define FileSystemClass SPIFFS
#else
  #include <ESP8266WebServer.h>
  #include <LittleFS.h>
  #include <ESP8266HTTPUpdateServer.h>
  #define WebServerClass ESP8266WebServer
  #define FileSystemClass LittleFS
#endif

// Forward declarations
class RelayController;
class ConfigManager;
class NtpClientManager;
class SensorManager;

class WebServerManager {
private:
  WebServerClass _server;
  #ifndef ESP32
    ESP8266HTTPUpdateServer _httpUpdater;
  #endif
  RelayController* _relayController = nullptr;
  ConfigManager* _configManager = nullptr;
  NtpClientManager* _ntpClient = nullptr;
  SensorManager* _sensorManager = nullptr;
  bool _otaError = false;
  File _uploadFile;
  void serveFile(const String& path);

public:
  WebServerManager() : _server(80) {}
  bool begin(RelayController* relayController = nullptr, ConfigManager* configManager = nullptr, NtpClientManager* ntpClient = nullptr, SensorManager* sensorManager = nullptr);
  void handleClient();
};

#endif 