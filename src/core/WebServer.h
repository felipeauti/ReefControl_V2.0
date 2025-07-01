#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include "RelayController.h"
#include "ConfigManager.h"
#include "SensorManager.h"
#include "../time/NtpClient.h"

// Compatibilidade ESP32/ESP8266
#ifdef ESP32
  #include <WebServer.h>
  #include <SPIFFS.h>
  #define FileSystemClass SPIFFS
  #define WebServerClass WebServer
#else
  #include <ESP8266WebServer.h>
  #include <ESP8266HTTPUpdateServer.h>
  #include <LittleFS.h>
  #define FileSystemClass LittleFS
  #define WebServerClass ESP8266WebServer
#endif

class WebServerManager {
private:
    WebServerClass _server;
    #ifndef ESP32
        ESP8266HTTPUpdateServer _httpUpdater;
    #endif
    
    RelayController* _relayController;
    ConfigManager* _configManager;
    NtpClientManager* _ntpClient;
    SensorManager* _sensorManager;
    File _uploadFile;

    void serveFile(const String& path);

public:
    WebServerManager() : _server(80) {}
    bool begin(RelayController* relayController, ConfigManager* configManager, NtpClientManager* ntpClient, SensorManager* sensorManager);
    void handleClient();
};

#endif