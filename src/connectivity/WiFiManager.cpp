#include "WiFiManager.h"

bool WiFiConnectionManager::begin() {
  Serial.println("🌐 Inicializando WiFiManager...");
  
  // Configurar WiFiManager
  _wifiManager.setDebugOutput(true);
  _wifiManager.setConfigPortalTimeout(180); // 3 minutos timeout
  _wifiManager.setConnectTimeout(20);       // 20 segundos para conectar
  _wifiManager.setAPCallback([](WiFiManager* wm) {
    Serial.println("🔧 Modo AP ativado!");
    Serial.println("📱 Conecte-se ao WiFi: ReefControl_AP");
    Serial.println("🌐 Acesse: http://192.168.4.1");
    Serial.println("⚙️ Configure suas credenciais WiFi");
  });
  
  // Tentar autoConnect - se falhar, abre AP mode automaticamente
  _connected = _wifiManager.autoConnect("ReefControl_AP", "123456789");
  
  if (_connected) {
    Serial.printf("✅ WiFi conectado: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("📶 SSID: %s\n", WiFi.SSID().c_str());
    Serial.printf("📡 RSSI: %d dBm\n", WiFi.RSSI());
    return true;
  } else {
    Serial.println("❌ Falha na conexão WiFi ou timeout do portal");
    Serial.println("🔄 Sistema continuará em modo offline");
    return false;
  }
}

void WiFiConnectionManager::reset() {
  Serial.println("🔄 Resetando configurações WiFi...");
  _wifiManager.resetSettings();
  Serial.println("✅ Configurações WiFi apagadas");
}

void WiFiConnectionManager::setConfigPortalTimeout(int timeout) {
  _wifiManager.setConfigPortalTimeout(timeout);
}

void WiFiConnectionManager::setAPCallback(std::function<void(WiFiManager*)> func) {
  _wifiManager.setAPCallback(func);
} 