#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <Arduino.h>

class OtaManager {
public:
  bool begin() { Serial.println("🚀 OTA stub inicializado"); return true; }
  void handle() {}
};

#endif 