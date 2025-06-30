/**
 * ReefControl - Sistema de Automação para Aquários
 * Arquitetura Modular Profissional
 * 
 * @author ReefControl Team
 * @version 2.0.7
 * @date 2025
 */

#include <Arduino.h>

// Capabilities system
#include "capabilities/Capabilities.h"

// Core modules
#include "core/ConfigManager.h"
#include "core/SensorManager.h"
#include "core/RelayController.h"
#include "core/WebServer.h"

// Connectivity modules
#include "connectivity/WiFiManager.h"
#include "connectivity/MqttClient.h"

// Time modules
#include "time/NtpClient.h"
#include "time/TimeScheduler.h"

// Display modules
// Display removido para otimização

// Update modules
#include "update/OtaManager.h"

// Global objects
ConfigManager config;
SensorManager sensors;
RelayController relays;
WebServerManager webServer;
WiFiConnectionManager wifiManager;
MqttClientManager mqttClient;
NtpClientManager ntpClient;
TimeSchedulerManager scheduler;
// DisplayManager removido
OtaManager otaManager;

// System status
struct SystemStatus {
  bool wifiConnected = false;
  bool mqttConnected = false;
  bool ntpSynced = false;
  // displayActive removido
  unsigned long uptime = 0;
  unsigned long lastSensorRead = 0;
  unsigned long lastMqttPublish = 0;
  // lastDisplayUpdate removido
};

SystemStatus systemStatus;

/**
 * Setup inicial do sistema
 */
void setup() {
  Serial.begin(115200);
  delay(2000);  // Aguardar estabilização do sistema
  
  Serial.println();
  Serial.println("========================================");
  Serial.printf("🐠 %s\n", getVersionString());
  Serial.printf("💻 Hardware: %s\n", getHardwarePlatform());
  Serial.printf("💾 RAM livre: %d bytes\n", ESP.getFreeHeap());
  Serial.println("========================================");
  
  // Mostrar capabilities do sistema
  printCapabilities();
  
  // 1. Inicializar sistema de arquivos
  Serial.println("📂 Inicializando sistema de arquivos...");
  #ifdef ESP32
    if (!SPIFFS.begin(true)) {
      Serial.println("❌ ERRO: Falha ao inicializar SPIFFS!");
      Serial.println("⚠️ Tentando formatar SPIFFS...");
      if (!SPIFFS.format()) {
        Serial.println("❌ ERRO: Falha ao formatar SPIFFS!");
      } else {
        Serial.println("✅ SPIFFS formatado com sucesso!");
        if (!SPIFFS.begin(true)) {
          Serial.println("❌ ERRO: Falha ao inicializar SPIFFS após formatação!");
          delay(5000);
          ESP.restart();
        }
      }
    }
  #else
    if (!LittleFS.begin()) {
      Serial.println("❌ ERRO: Falha ao inicializar LittleFS!");
      Serial.println("⚠️ Tentando formatar LittleFS...");
      if (!LittleFS.format()) {
        Serial.println("❌ ERRO: Falha ao formatar LittleFS!");
      } else {
        Serial.println("✅ LittleFS formatado com sucesso!");
        if (!LittleFS.begin()) {
          Serial.println("❌ ERRO: Falha ao inicializar LittleFS após formatação!");
          delay(5000);
          ESP.restart();
        }
      }
    }
  #endif
  Serial.println("✅ Sistema de arquivos inicializado");
  
  // Listar arquivos para debug
  Serial.println("\n📂 Listando arquivos no sistema:");
  #ifdef ESP32
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while(file) {
      Serial.printf("  📄 %s (%d bytes)\n", file.name(), file.size());
      file = root.openNextFile();
    }
  #else
    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {
      Serial.printf("  📄 %s (%d bytes)\n", dir.fileName().c_str(), dir.fileSize());
    }
  #endif
  Serial.println();
  
  // 2. Inicializar configurações
  Serial.println("📋 Inicializando configurações...");
  yield();  // Proteger contra watchdog
  if (!config.begin()) {
    Serial.println("❌ ERRO: Falha ao inicializar configurações!");
    delay(5000);
    ESP.restart();
  }
  
  // 3. Inicializar hardware
  Serial.println("🔧 Inicializando hardware...");
  yield();  // Proteger contra watchdog
  sensors.begin();
  yield();
  relays.begin();
  yield();
  
  // 4. Display removido para otimização
  Serial.println("📺 Display removido - otimização de RAM");
  
  // 5. Inicializar WiFi
  Serial.println("📶 Inicializando WiFi...");
  wifiManager.begin();
  systemStatus.wifiConnected = wifiManager.isConnected();
  
  if (systemStatus.wifiConnected) {
    Serial.printf("✅ WiFi conectado: %s\n", WiFi.localIP().toString().c_str());
    // display.showStatus removido
    
    // 6. Inicializar NTP
    Serial.println("🕐 Inicializando NTP...");
    ntpClient.begin(
      config.ntp.server1,
      config.ntp.server2, 
      config.ntp.server3,
      config.ntp.syncInterval,
      config.ntp.timezoneOffset
    );
    systemStatus.ntpSynced = ntpClient.isTimeSet();
    
    // 7. Inicializar MQTT
    Serial.println("📡 Inicializando MQTT...");
    mqttClient.begin();
    
    // 8. Inicializar OTA
    Serial.println("🚀 Inicializando OTA...");
    otaManager.begin();
    
    // 9. Inicializar servidor web
    Serial.println("🌐 Inicializando servidor web...");
    webServer.begin(&relays, &config, &ntpClient, &sensors);
    
    // 10. Inicializar agendador
    Serial.println("⏰ Inicializando agendador...");
    scheduler.begin();
    
  } else {
    Serial.println("❌ WiFi não conectado - Modo offline");
    // display.showError removido
  }
  
  Serial.println("✅ Sistema inicializado com sucesso!");
  Serial.println("========================================");
  
  // Primeira leitura dos sensores
  sensors.readAll();
  systemStatus.lastSensorRead = millis();
  
  // Display removido - sem atualização inicial
}

/**
 * Loop principal do sistema
 */
void loop() {
  unsigned long currentTime = millis();
  systemStatus.uptime = currentTime;
  
  // Processar conexões
  if (systemStatus.wifiConnected) {
    webServer.handleClient();
    otaManager.handle();
    mqttClient.loop();
    ntpClient.update();
  }
  
  // Ler sensores a cada 5 segundos
  if (currentTime - systemStatus.lastSensorRead > 5000) {
    sensors.readAll();
    systemStatus.lastSensorRead = currentTime;
    
    // Controle automático
    relays.autoControl(sensors.getTemperature(), sensors.getPH());
    
    // Log dos sensores removido para não poluir o terminal
  }
  
  // Publicar no MQTT a cada 10 segundos
  if (systemStatus.wifiConnected && mqttClient.isConnected() && 
      currentTime - systemStatus.lastMqttPublish > 10000) {
    
    mqttClient.publishSensorData(sensors.getTemperature(), sensors.getPH(), 
                                sensors.getTDS(), sensors.getWaterLevel());
    mqttClient.publishSystemStatus(systemStatus.uptime, ESP.getFreeHeap(), 
                                  WiFi.RSSI());
    systemStatus.lastMqttPublish = currentTime;
  }
  
  // Display removido - sem atualizações
  
  // Processar agendamentos
  scheduler.update();
  
  // Atualizar status das conexões
  systemStatus.wifiConnected = WiFi.status() == WL_CONNECTED;
  systemStatus.mqttConnected = mqttClient.isConnected();
  systemStatus.ntpSynced = ntpClient.isTimeSet();
  // systemStatus.displayActive removido
  
  // Proteção contra watchdog timeout e yield para sistema
  yield();
  delay(100);  // Aumentado para reduzir carga do processador
} 