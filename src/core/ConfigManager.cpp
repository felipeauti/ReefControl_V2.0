/**
 * ConfigManager.cpp
 * Implementação do Gerenciador de Configurações
 */

#include "ConfigManager.h"

// Compatibilidade ESP8266/ESP32
#ifdef ESP32
  #include <SPIFFS.h>
  #define FileSystemClass SPIFFS
#else
  #include <LittleFS.h>
  #define FileSystemClass LittleFS
#endif

const char* ConfigManager::CONFIG_FILE = "/config.json";

bool ConfigManager::begin() {
  #ifdef ESP32
    if (!FileSystemClass.begin(true)) {
      Serial.println("❌ Erro ao inicializar SPIFFS");
      return false;
    }
    Serial.println("✅ SPIFFS inicializado");
  #else
    if (!FileSystemClass.begin()) {
      Serial.println("❌ Erro ao inicializar LittleFS");
      return false;
    }
    Serial.println("✅ LittleFS inicializado");
  #endif
  
  // Tentar carregar configurações existentes
  if (!loadFromFile()) {
    Serial.println("⚠️ Configurações não encontradas, usando padrões");
    setDefaults();
    save(); // Salvar configurações padrão
  }
  
  _initialized = true;
  return true;
}

bool ConfigManager::load() {
  return loadFromFile();
}

bool ConfigManager::save() {
  return saveToFile();
}

void ConfigManager::reset() {
  setDefaults();
  save();
  Serial.println("🔄 Configurações resetadas para padrão");
}

bool ConfigManager::loadFromFile() {
  if (!FileSystemClass.exists(CONFIG_FILE)) {
    return false;
  }
  
  File file = FileSystemClass.open(CONFIG_FILE, "r");
  if (!file) {
    Serial.println("❌ Erro ao abrir arquivo de configuração");
    return false;
  }
  
  String content = file.readString();
  file.close();
  
  return fromJson(content);
}

bool ConfigManager::saveToFile() {
  File file = FileSystemClass.open(CONFIG_FILE, "w");
  if (!file) {
    Serial.println("❌ Erro ao criar arquivo de configuração");
    return false;
  }
  
  String json = toJson();
  size_t written = file.print(json);
  file.close();
  
  if (written > 0) {
    Serial.println("✅ Configurações salvas com sucesso");
    return true;
  } else {
    Serial.println("❌ Erro ao salvar configurações");
    return false;
  }
}

void ConfigManager::setDefaults() {
  // WiFi padrões
  strcpy(wifi.ssid, "Dlink2901");
  strcpy(wifi.password, "Ks@872036#%");
  strcpy(wifi.hostname, "reefcontrol");
  
  // MQTT padrões
  mqtt.enabled = false;
  strcpy(mqtt.server, "192.168.1.1");
  mqtt.port = 1883;
  
  // NTP padrões
  ntp.enabled = true;
  strcpy(ntp.timezone, "America/Sao_Paulo");
  
  // Sensores padrões
  sensor.tempEnabled = true;
  sensor.phEnabled = true;
  sensor.tdsEnabled = true;
  sensor.levelEnabled = true;
  
  // Configurações de saídas - CORREÇÃO DO BUG!
  strcpy(relay.outputs[0].name, "Bomba");
  relay.outputs[0].pin = 5;
  relay.outputs[0].enabled = true;
  relay.outputs[0].autoMode = true;
  relay.outputs[0].onTime = 8;
  relay.outputs[0].offTime = 22;
  relay.outputs[0].onTemp = 0.0;
  relay.outputs[0].offTemp = 0.0;
  relay.outputs[0].interval = 0;
  relay.outputs[0].duration = 0;
  
  strcpy(relay.outputs[1].name, "Termostato");
  relay.outputs[1].pin = 4;
  relay.outputs[1].enabled = true;
  relay.outputs[1].autoMode = true;
  relay.outputs[1].onTime = 0;
  relay.outputs[1].offTime = 0;
  relay.outputs[1].onTemp = 24.0;
  relay.outputs[1].offTemp = 26.0;
  relay.outputs[1].interval = 0;
  relay.outputs[1].duration = 0;
  
  strcpy(relay.outputs[2].name, "Chiller");
  relay.outputs[2].pin = 14;
  relay.outputs[2].enabled = true;
  relay.outputs[2].autoMode = true;
  relay.outputs[2].onTime = 8;
  relay.outputs[2].offTime = 20;
  relay.outputs[2].onTemp = 0.0;
  relay.outputs[2].offTemp = 0.0;
  relay.outputs[2].interval = 0;
  relay.outputs[2].duration = 0;
  
  strcpy(relay.outputs[3].name, "Skimmer");
  relay.outputs[3].pin = 12;
  relay.outputs[3].enabled = true;
  relay.outputs[3].autoMode = false;
  relay.outputs[3].onTime = 0;
  relay.outputs[3].offTime = 0;
  relay.outputs[3].onTemp = 0.0;
  relay.outputs[3].offTemp = 0.0;
  relay.outputs[3].interval = 60;
  relay.outputs[3].duration = 30;
  
  // Relés padrões (compatibilidade)
  relay.pump1Enabled = true;
  relay.heaterEnabled = true;
  relay.lightEnabled = true;
  relay.pump2Enabled = true;
  
  // Display padrões
  display.enabled = true;
  display.type = 0; // OLED
  display.address = 0x3C;
  
  // Sistema padrões
  strcpy(system.deviceName, "ReefControl");
  strcpy(system.version, "2.0.0");
  system.debugMode = true;
}

bool ConfigManager::validateConfig() {
  // Validar configurações críticas
  if (strlen(wifi.ssid) == 0) return false;
  if (mqtt.port < 1 || mqtt.port > 65535) return false;
  if (ntp.syncInterval < 60) return false;
  
  return true;
}

void ConfigManager::setDeviceName(const String& name) {
  name.toCharArray(system.deviceName, sizeof(system.deviceName));
}

void ConfigManager::setWiFiCredentials(const String& ssid, const String& password) {
  ssid.toCharArray(wifi.ssid, sizeof(wifi.ssid));
  password.toCharArray(wifi.password, sizeof(wifi.password));
}

void ConfigManager::setMqttServer(const String& server, int port, const String& user, const String& pass) {
  server.toCharArray(mqtt.server, sizeof(mqtt.server));
  mqtt.port = port;
  user.toCharArray(mqtt.username, sizeof(mqtt.username));
  pass.toCharArray(mqtt.password, sizeof(mqtt.password));
}

String ConfigManager::toJson() {
  // Buffer otimizado para evitar stack overflow
  #ifdef ESP32
    DynamicJsonDocument doc(3072);  // Reduzido para 3KB
  #else
    DynamicJsonDocument doc(2048);
  #endif
  
  // WiFi
  JsonObject wifiObj = doc.createNestedObject("wifi");
  wifiObj["ssid"] = wifi.ssid;
  wifiObj["password"] = wifi.password;
  wifiObj["hostname"] = wifi.hostname;
  wifiObj["staticIP"] = wifi.staticIP;
  wifiObj["ip"] = wifi.ip;
  wifiObj["gateway"] = wifi.gateway;
  wifiObj["subnet"] = wifi.subnet;
  wifiObj["dns"] = wifi.dns;
  
  // MQTT
  JsonObject mqttObj = doc.createNestedObject("mqtt");
  mqttObj["enabled"] = mqtt.enabled;
  mqttObj["server"] = mqtt.server;
  mqttObj["port"] = mqtt.port;
  mqttObj["username"] = mqtt.username;
  mqttObj["password"] = mqtt.password;
  mqttObj["clientId"] = mqtt.clientId;
  mqttObj["topicPrefix"] = mqtt.topicPrefix;
  mqttObj["publishInterval"] = mqtt.publishInterval;
  
  // NTP
  JsonObject ntpObj = doc.createNestedObject("ntp");
  ntpObj["enabled"] = ntp.enabled;
  ntpObj["server1"] = ntp.server1;
  ntpObj["server2"] = ntp.server2;
  ntpObj["server3"] = ntp.server3;
  ntpObj["timezone"] = ntp.timezone;
  ntpObj["timezoneOffset"] = ntp.timezoneOffset;  // Adicionar timezone numérico
  ntpObj["syncInterval"] = ntp.syncInterval;
  
  // Sensores
  JsonObject sensorObj = doc.createNestedObject("sensor");
  sensorObj["tempEnabled"] = sensor.tempEnabled;
  sensorObj["tempOffset"] = sensor.tempOffset;
  sensorObj["tempMin"] = sensor.tempMin;
  sensorObj["tempMax"] = sensor.tempMax;
  sensorObj["phEnabled"] = sensor.phEnabled;
  sensorObj["phOffset"] = sensor.phOffset;
  sensorObj["phMin"] = sensor.phMin;
  sensorObj["phMax"] = sensor.phMax;
  sensorObj["phCalibration4"] = sensor.phCalibration4;
  sensorObj["phCalibration7"] = sensor.phCalibration7;
  sensorObj["tdsEnabled"] = sensor.tdsEnabled;
  sensorObj["tdsOffset"] = sensor.tdsOffset;
  sensorObj["tdsMin"] = sensor.tdsMin;
  sensorObj["tdsMax"] = sensor.tdsMax;
  sensorObj["levelEnabled"] = sensor.levelEnabled;
  sensorObj["levelMin"] = sensor.levelMin;
  sensorObj["levelMax"] = sensor.levelMax;
  
  // Relés
  JsonObject relayObj = doc.createNestedObject("relay");
  
  // Configurações de saídas
  JsonArray outputsArray = relayObj.createNestedArray("outputs");
  for (int i = 0; i < 4; i++) {
    JsonObject output = outputsArray.createNestedObject();
    output["name"] = relay.outputs[i].name;
    output["pin"] = relay.outputs[i].pin;
    output["enabled"] = relay.outputs[i].enabled;
    output["autoMode"] = relay.outputs[i].autoMode;
    output["onTime"] = relay.outputs[i].onTime;
    output["offTime"] = relay.outputs[i].offTime;
    output["onTemp"] = relay.outputs[i].onTemp;
    output["offTemp"] = relay.outputs[i].offTemp;
    output["interval"] = relay.outputs[i].interval;
    output["duration"] = relay.outputs[i].duration;
  }
  
  // Compatibilidade com código antigo
  relayObj["pump1Enabled"] = relay.pump1Enabled;
  relayObj["pump1AutoMode"] = relay.pump1AutoMode;
  relayObj["pump1OnTime"] = relay.pump1OnTime;
  relayObj["pump1OffTime"] = relay.pump1OffTime;
  relayObj["heaterEnabled"] = relay.heaterEnabled;
  relayObj["heaterAutoMode"] = relay.heaterAutoMode;
  relayObj["heaterOnTemp"] = relay.heaterOnTemp;
  relayObj["heaterOffTemp"] = relay.heaterOffTemp;
  relayObj["lightEnabled"] = relay.lightEnabled;
  relayObj["lightAutoMode"] = relay.lightAutoMode;
  relayObj["lightOnTime"] = relay.lightOnTime;
  relayObj["lightOffTime"] = relay.lightOffTime;
  relayObj["pump2Enabled"] = relay.pump2Enabled;
  relayObj["pump2AutoMode"] = relay.pump2AutoMode;
  relayObj["pump2Interval"] = relay.pump2Interval;
  relayObj["pump2Duration"] = relay.pump2Duration;
  
  // Display
  JsonObject displayObj = doc.createNestedObject("display");
  displayObj["enabled"] = display.enabled;
  displayObj["type"] = display.type;
  displayObj["width"] = display.width;
  displayObj["height"] = display.height;
  displayObj["address"] = display.address;
  displayObj["flipScreen"] = display.flipScreen;
  displayObj["brightness"] = display.brightness;
  displayObj["screensaver"] = display.screensaver;
  displayObj["screensaverTime"] = display.screensaverTime;
  displayObj["pageInterval"] = display.pageInterval;
  
  // Sistema
  JsonObject systemObj = doc.createNestedObject("system");
  systemObj["deviceName"] = system.deviceName;
  systemObj["version"] = system.version;
  systemObj["debugMode"] = system.debugMode;
  systemObj["ledIndicator"] = system.ledIndicator;
  systemObj["watchdogTimeout"] = system.watchdogTimeout;
  systemObj["autoRestart"] = system.autoRestart;
  systemObj["restartInterval"] = system.restartInterval;
  
  String output;
  serializeJsonPretty(doc, output);
  return output;
}

bool ConfigManager::fromJson(const String& json) {
  // Buffer otimizado para evitar stack overflow
  #ifdef ESP32
    DynamicJsonDocument doc(3072);  // Reduzido para 3KB
  #else
    DynamicJsonDocument doc(2048);
  #endif
  DeserializationError error = deserializeJson(doc, json);
  
  if (error) {
    Serial.printf("❌ Erro ao parsear JSON: %s\n", error.c_str());
    return false;
  }
  
  // WiFi
  if (doc.containsKey("wifi")) {
    JsonObject wifiObj = doc["wifi"];
    strcpy(wifi.ssid, wifiObj["ssid"] | wifi.ssid);
    strcpy(wifi.password, wifiObj["password"] | wifi.password);
    strcpy(wifi.hostname, wifiObj["hostname"] | wifi.hostname);
    wifi.staticIP = wifiObj["staticIP"] | wifi.staticIP;
    strcpy(wifi.ip, wifiObj["ip"] | wifi.ip);
    strcpy(wifi.gateway, wifiObj["gateway"] | wifi.gateway);
    strcpy(wifi.subnet, wifiObj["subnet"] | wifi.subnet);
    strcpy(wifi.dns, wifiObj["dns"] | wifi.dns);
  }
  
  // MQTT
  if (doc.containsKey("mqtt")) {
    JsonObject mqttObj = doc["mqtt"];
    mqtt.enabled = mqttObj["enabled"] | mqtt.enabled;
    strcpy(mqtt.server, mqttObj["server"] | mqtt.server);
    mqtt.port = mqttObj["port"] | mqtt.port;
    strcpy(mqtt.username, mqttObj["username"] | mqtt.username);
    strcpy(mqtt.password, mqttObj["password"] | mqtt.password);
    strcpy(mqtt.clientId, mqttObj["clientId"] | mqtt.clientId);
    strcpy(mqtt.topicPrefix, mqttObj["topicPrefix"] | mqtt.topicPrefix);
    mqtt.publishInterval = mqttObj["publishInterval"] | mqtt.publishInterval;
  }
  
  // NTP
  if (doc.containsKey("ntp")) {
    JsonObject ntpObj = doc["ntp"];
    ntp.enabled = ntpObj["enabled"] | ntp.enabled;
    strcpy(ntp.server1, ntpObj["server1"] | ntp.server1);
    strcpy(ntp.server2, ntpObj["server2"] | ntp.server2);
    strcpy(ntp.server3, ntpObj["server3"] | ntp.server3);
    strcpy(ntp.timezone, ntpObj["timezone"] | ntp.timezone);
    ntp.timezoneOffset = ntpObj["timezoneOffset"] | ntp.timezoneOffset;  // Carregar timezone numérico
    ntp.syncInterval = ntpObj["syncInterval"] | ntp.syncInterval;
  }
  
  // Relés
  if (doc.containsKey("relay")) {
    JsonObject relayObj = doc["relay"];
    
    // Carregar configurações de saídas
    if (relayObj.containsKey("outputs")) {
      JsonArray outputsArray = relayObj["outputs"];
      int index = 0;
      for (JsonObject output : outputsArray) {
        if (index < 4) {
          String name = output["name"] | relay.outputs[index].name;
          name.toCharArray(relay.outputs[index].name, sizeof(relay.outputs[index].name));
          
          relay.outputs[index].pin = output["pin"] | relay.outputs[index].pin;
          relay.outputs[index].enabled = output["enabled"] | relay.outputs[index].enabled;
          relay.outputs[index].autoMode = output["autoMode"] | relay.outputs[index].autoMode;
          relay.outputs[index].onTime = output["onTime"] | relay.outputs[index].onTime;
          relay.outputs[index].offTime = output["offTime"] | relay.outputs[index].offTime;
          relay.outputs[index].onTemp = output["onTemp"] | relay.outputs[index].onTemp;
          relay.outputs[index].offTemp = output["offTemp"] | relay.outputs[index].offTemp;
          relay.outputs[index].interval = output["interval"] | relay.outputs[index].interval;
          relay.outputs[index].duration = output["duration"] | relay.outputs[index].duration;
          index++;
        }
      }
    }
    
    // Compatibilidade com código antigo
    relay.pump1Enabled = relayObj["pump1Enabled"] | relay.pump1Enabled;
    relay.pump1AutoMode = relayObj["pump1AutoMode"] | relay.pump1AutoMode;
    relay.pump1OnTime = relayObj["pump1OnTime"] | relay.pump1OnTime;
    relay.pump1OffTime = relayObj["pump1OffTime"] | relay.pump1OffTime;
    relay.heaterEnabled = relayObj["heaterEnabled"] | relay.heaterEnabled;
    relay.heaterAutoMode = relayObj["heaterAutoMode"] | relay.heaterAutoMode;
    relay.heaterOnTemp = relayObj["heaterOnTemp"] | relay.heaterOnTemp;
    relay.heaterOffTemp = relayObj["heaterOffTemp"] | relay.heaterOffTemp;
    relay.lightEnabled = relayObj["lightEnabled"] | relay.lightEnabled;
    relay.lightAutoMode = relayObj["lightAutoMode"] | relay.lightAutoMode;
    relay.lightOnTime = relayObj["lightOnTime"] | relay.lightOnTime;
    relay.lightOffTime = relayObj["lightOffTime"] | relay.lightOffTime;
    relay.pump2Enabled = relayObj["pump2Enabled"] | relay.pump2Enabled;
    relay.pump2AutoMode = relayObj["pump2AutoMode"] | relay.pump2AutoMode;
    relay.pump2Interval = relayObj["pump2Interval"] | relay.pump2Interval;
    relay.pump2Duration = relayObj["pump2Duration"] | relay.pump2Duration;
  }
  
  return validateConfig();
}

void ConfigManager::printConfig() {
  Serial.println("📋 Configurações atuais:");
  Serial.printf("  Device: %s v%s\n", system.deviceName, system.version);
  Serial.printf("  WiFi: %s\n", wifi.ssid);
  Serial.printf("  MQTT: %s (%s:%d)\n", mqtt.enabled ? "Habilitado" : "Desabilitado", mqtt.server, mqtt.port);
  Serial.printf("  NTP: %s (%s)\n", ntp.enabled ? "Habilitado" : "Desabilitado", ntp.timezone);
  Serial.printf("  Display: %s (Tipo %d)\n", display.enabled ? "Habilitado" : "Desabilitado", display.type);
  Serial.printf("  Debug: %s\n", system.debugMode ? "Habilitado" : "Desabilitado");
}

// Métodos para configurações de saídas
bool ConfigManager::setOutputConfig(int index, const String& name, int pin) {
  if (index < 0 || index >= 4) {
    return false;
  }
  
  name.toCharArray(relay.outputs[index].name, sizeof(relay.outputs[index].name));
  relay.outputs[index].pin = pin;
  
  Serial.printf("🔧 Saída %d configurada: %s (GPIO %d)\n", index, relay.outputs[index].name, pin);
  return save(); // Salvar automaticamente
}

OutputConfig* ConfigManager::getOutputConfig(int index) {
  if (index < 0 || index >= 4) {
    return nullptr;
  }
  return &relay.outputs[index];
}

String ConfigManager::getOutputsJson() {
  // Buffer otimizado para ESP32 Pro
  #ifdef ESP32
    DynamicJsonDocument doc(1536);  // Reduzido para 1.5KB
  #else
    DynamicJsonDocument doc(1024);
  #endif
  
  JsonArray outputsArray = doc.createNestedArray("outputs");
  
  for (int i = 0; i < 4; i++) {
    JsonObject output = outputsArray.createNestedObject();
    output["index"] = i;
    output["name"] = relay.outputs[i].name;
    output["pin"] = relay.outputs[i].pin;
    output["enabled"] = relay.outputs[i].enabled;
    output["autoMode"] = relay.outputs[i].autoMode;
    output["onTime"] = relay.outputs[i].onTime;
    output["offTime"] = relay.outputs[i].offTime;
    output["onTemp"] = relay.outputs[i].onTemp;
    output["offTemp"] = relay.outputs[i].offTemp;
    output["interval"] = relay.outputs[i].interval;
    output["duration"] = relay.outputs[i].duration;
  }
  
  String result;
  serializeJson(doc, result);
  return result;
}

bool ConfigManager::setOutputsFromJson(const String& json) {
  // Buffer otimizado para ESP32 Pro
  #ifdef ESP32
    DynamicJsonDocument doc(1536);  // Reduzido para 1.5KB
  #else
    DynamicJsonDocument doc(1024);
  #endif
  DeserializationError error = deserializeJson(doc, json);
  
  if (error) {
    Serial.printf("❌ Erro ao parsear JSON de saídas: %s\n", error.c_str());
    return false;
  }
  
  if (!doc.containsKey("outputs")) {
    return false;
  }
  
  JsonArray outputsArray = doc["outputs"];
  
  for (JsonObject output : outputsArray) {
    int index = output["index"] | -1;
    if (index >= 0 && index < 4) {
      String name = output["name"] | relay.outputs[index].name;
      name.toCharArray(relay.outputs[index].name, sizeof(relay.outputs[index].name));
      
      relay.outputs[index].pin = output["pin"] | relay.outputs[index].pin;
      relay.outputs[index].enabled = output["enabled"] | relay.outputs[index].enabled;
      relay.outputs[index].autoMode = output["autoMode"] | relay.outputs[index].autoMode;
      relay.outputs[index].onTime = output["onTime"] | relay.outputs[index].onTime;
      relay.outputs[index].offTime = output["offTime"] | relay.outputs[index].offTime;
      relay.outputs[index].onTemp = output["onTemp"] | relay.outputs[index].onTemp;
      relay.outputs[index].offTemp = output["offTemp"] | relay.outputs[index].offTemp;
      relay.outputs[index].interval = output["interval"] | relay.outputs[index].interval;
      relay.outputs[index].duration = output["duration"] | relay.outputs[index].duration;
      
      Serial.printf("🔧 Saída %d atualizada: %s (GPIO %d)\n", index, relay.outputs[index].name, relay.outputs[index].pin);
    }
  }
  
  return save(); // Salvar automaticamente
} 