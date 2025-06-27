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
  char timezone[64] = "America/Sao_Paulo";
  int syncInterval = 3600; // segundos
};

struct SensorConfig {
  // Temperatura
  bool tempEnabled = true;
  float tempOffset = 0.0;
  float tempMin = 20.0;
  float tempMax = 30.0;
  
  // pH
  bool phEnabled = true;
  float phOffset = 0.0;
  float phMin = 6.5;
  float phMax = 8.5;
  float phCalibration4 = 512.0;  // Valor ADC para pH 4
  float phCalibration7 = 410.0;  // Valor ADC para pH 7
  
  // TDS
  bool tdsEnabled = true;
  float tdsOffset = 0.0;
  int tdsMin = 200;
  int tdsMax = 500;
  
  // Nível da água
  bool levelEnabled = true;
  int levelMin = 20;
  int levelMax = 95;
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
  // Configurações individuais das saídas
  OutputConfig outputs[4] = {
    {"Bomba", 5, true, true, 8, 22, 0.0, 0.0, 0, 0},
    {"Termostato", 4, true, true, 0, 0, 24.0, 26.0, 0, 0},
    {"Chiller", 14, true, true, 8, 20, 0.0, 0.0, 0, 0},
    {"Skimmer", 12, true, false, 0, 0, 0.0, 0.0, 60, 30}
  };
  
  // Compatibilidade com código antigo
  bool pump1Enabled = true;
  bool pump1AutoMode = true;
  int pump1OnTime = 8;
  int pump1OffTime = 22;
  
  bool heaterEnabled = true;
  bool heaterAutoMode = true;
  float heaterOnTemp = 24.0;
  float heaterOffTemp = 26.0;
  
  bool lightEnabled = true;
  bool lightAutoMode = true;
  int lightOnTime = 8;
  int lightOffTime = 20;
  
  bool pump2Enabled = true;
  bool pump2AutoMode = false;
  int pump2Interval = 60;
  int pump2Duration = 30;
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
  char version[16] = "2.0.12";
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