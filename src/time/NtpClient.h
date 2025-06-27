#ifndef NTP_CLIENT_MANAGER_H
#define NTP_CLIENT_MANAGER_H

#include <Arduino.h>

class NtpClientManager {
public:
  bool begin() { Serial.println("🕐 NTP stub inicializado"); return true; }
  void update() {}
  bool isTimeSet() const { return true; }
};

#endif 