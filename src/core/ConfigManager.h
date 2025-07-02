/**
 * ConfigManager.h
 * Gerenciador de Configurações do ReefControl
 * 
 * Responsável por:
 * - Carregar/salvar configurações na EEPROM/LittleFS
 * - Configurações WiFi, MQTT, NTP, Sensores, etc.
 * - Valores padrão e validação
 */

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Compatibilidade ESP8266/ESP32
#ifdef ESP32
  #include <SPIFFS.h>
#else
  #include <LittleFS.h>
#endif

// Estruturas de configuração
struct WiFiConfig {
  char ssid[32] = "ReefControl_AP";
  char password[64] = "123456789";
  char hostname[32] = "reefcontrol";
  bool staticIP = false;
  char ip[16] = "192.168.1.100";
  char gateway[16] = "192.168.1.1";
  char subnet[16] = "255.255.255.0";
  char dns[16] = "8.8.8.8";
};

struct MqttConfig {
  bool enabled = false;
  char server[64] = "192.168.1.1";
  int port = 1883;
  char username[32] = "";
  char password[64] = "";
  char clientId[32] = "reefcontrol";
  char topicPrefix[32] = "reefcontrol";
  int publishInterval = 10; // segundos
};

struct NtpConfig {
  bool enabled = true;
  char server1[64] = "pool.ntp.org";
  char server2[64] = "time.google.com";
  char server3[64] = "br.pool.ntp.org";
  char timezone[64] = "America/Sao_Paulo"; // Manter compatibilidade
  int timezoneOffset = 0; // UTC offset em horas (novo campo)
  int syncInterval = 3600; // segundos
};

struct SensorConfig {
  // Temperatura
  float tempOffset = 0.0;  // Offset de calibração
  bool tempEnabled = true;
  int tempPin = 4;  // GPIO4 (D2) para DS18B20
  
  // pH
  float phOffset = 0.0;
  bool phEnabled = false;
  int phPin = 0;  // A0
  float ph4Voltage = 2.0;
  float ph7Voltage = 2.5;
  
  // TDS
  float tdsOffset = 0.0;
  bool tdsEnabled = false;
  int tdsPin = 0;  // A0
  float tdsKValue = 1.0;
  
  // Nível
  float levelOffset = 0.0;
  bool levelEnabled = false;
  int levelPin = 0;  // A0
  float levelEmpty = 0.0;
  float levelFull = 100.0;
};

struct OutputConfig {
  char name[32];
  int pin;
  bool enabled;
  bool autoMode;
  int onTime;   // hora para ligar
  int offTime;  // hora para desligar
  float onTemp;  // temperatura para ligar (aquecedor)
  float offTemp; // temperatura para desligar (aquecedor)
  int interval;  // intervalo em minutos
  int duration;  // duração em segundos
};

struct RelayConfig {
  // Configurações das saídas - Sistema moderno
  OutputConfig outputs[4] = {
    {"Bomba", 5, true, true, 8, 22, 0.0, 0.0, 0, 0},
    {"Termostato", 4, true, true, 0, 0, 24.0, 26.0, 0, 0},
    {"Chiller", 14, true, true, 8, 20, 0.0, 0.0, 0, 0},
    {"Skimmer", 12, true, false, 0, 0, 0.0, 0.0, 60, 30}
  };
};

struct DisplayConfig {
  bool enabled = true;
  int type = 0; // 0=OLED SSD1306, 1=LCD I2C
  int width = 128;
  int height = 64;
  int address = 0x3C;
  bool flipScreen = false;
  int brightness = 255;
  bool screensaver = true;
  int screensaverTime = 300; // segundos
  int pageInterval = 5; // segundos entre páginas
};

struct SystemConfig {
  char deviceName[32] = "ReefControl";
      char version[32] = "3.2.3-beta";
  bool debugMode = true;
  bool ledIndicator = true;
  int watchdogTimeout = 30; // segundos
  bool autoRestart = true;
  int restartInterval = 86400; // segundos (24h)
};

// Classe principal
class ConfigManager {
private:
  static const char* CONFIG_FILE;
  bool _initialized = false;
  
  // Métodos privados
  bool loadFromFile();
  bool saveToFile();
  void setDefaults();
  bool validateConfig();

public:
  // Estruturas de configuração
  WiFiConfig wifi;
  MqttConfig mqtt;
  NtpConfig ntp;
  SensorConfig sensor;
  RelayConfig relay;
  DisplayConfig display;
  SystemConfig system;
  
  // Métodos públicos
  bool begin();
  bool load();
  bool save();
  void reset();
  
  // Getters específicos
  String getDeviceName() { return String(system.deviceName); }
  String getVersion() { return String(system.version); }
  bool isDebugMode() { return system.debugMode; }
  
  // Setters específicos
  void setDeviceName(const String& name);
  void setWiFiCredentials(const String& ssid, const String& password);
  void setMqttServer(const String& server, int port, const String& user, const String& pass);
  
  // Configurações de saídas
  bool setOutputConfig(int index, const String& name, int pin);
  OutputConfig* getOutputConfig(int index);
  String getOutputsJson();
  bool setOutputsFromJson(const String& json);
  
  // Utilitários
  String toJson();
  bool fromJson(const String& json);
  void printConfig();
};

#endif // CONFIG_MANAGER_H 