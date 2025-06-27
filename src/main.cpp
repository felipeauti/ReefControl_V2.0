/**
 * ReefControl - Sistema de Automação para Aquários
 * Arquitetura Modular Profissional
 * 
 * @author ReefControl Team
 * @version 2.0.7
 * @date 2025
 */

#include <Arduino.h>

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
  Serial.println();
  Serial.println("========================================");
  Serial.println("🐠 ReefControl v2.0.9 - Interface Limpa Sem Mensagens");
  Serial.println("========================================");
  
  // 1. Inicializar configurações
  Serial.println("📋 Inicializando configurações...");
  config.begin();
  
  // 2. Inicializar hardware
  Serial.println("🔧 Inicializando hardware...");
  sensors.begin();
  relays.begin();
  
  // 3. Display removido para otimização
  Serial.println("📺 Display removido - otimização de RAM");
  
  // 4. Inicializar WiFi
  Serial.println("📶 Inicializando WiFi...");
  wifiManager.begin();
  systemStatus.wifiConnected = wifiManager.isConnected();
  
  if (systemStatus.wifiConnected) {
    Serial.printf("✅ WiFi conectado: %s\n", WiFi.localIP().toString().c_str());
    // display.showStatus removido
    
    // 5. Inicializar NTP
    Serial.println("🕐 Inicializando NTP...");
    ntpClient.begin();
    systemStatus.ntpSynced = ntpClient.isTimeSet();
    
    // 6. Inicializar MQTT
    Serial.println("📡 Inicializando MQTT...");
    mqttClient.begin();
    
    // 7. Inicializar OTA
    Serial.println("🚀 Inicializando OTA...");
    otaManager.begin();
    
    // 8. Inicializar servidor web
    Serial.println("🌐 Inicializando servidor web...");
    webServer.begin(&relays, &config);
    
    // 9. Inicializar agendador
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
    
    // Log dos sensores
    Serial.printf("📊 Temp: %.1f°C | pH: %.1f | TDS: %d | Nível: %d%%\n", 
                  sensors.getTemperature(), sensors.getPH(), 
                  sensors.getTDS(), sensors.getWaterLevel());
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
  
  // Pequeno delay para não sobrecarregar o processador
  delay(50);
} 