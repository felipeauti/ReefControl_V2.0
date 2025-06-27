#include "WiFiManager.h"

bool WiFiConnectionManager::begin() {
  WiFi.begin("Dlink2901", "Ks@872036#%");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    _connected = true;
    Serial.printf("\n✅ WiFi conectado: %s\n", WiFi.localIP().toString().c_str());
    return true;
  } else {
    _connected = false;
    Serial.println("\n❌ Falha na conexão WiFi");
    return false;
  }
} 