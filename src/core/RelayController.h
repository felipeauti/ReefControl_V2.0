#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#include <Arduino.h>
#include "ConfigManager.h"

// Pinos dos relés - compatibilidade ESP8266/ESP32
#ifdef ESP32
  #define RELAY1_PIN 5   // GPIO5 - Bomba Principal
  #define RELAY2_PIN 18  // GPIO18 - Aquecedor
  #define RELAY3_PIN 19  // GPIO19 - Iluminação
  #define RELAY4_PIN 21  // GPIO21 - Bomba Reposição
#else
  #define RELAY1_PIN D1  // GPIO5 - Bomba Principal
  #define RELAY2_PIN D2  // GPIO4 - Aquecedor  
  #define RELAY3_PIN D5  // GPIO14 - Iluminação
  #define RELAY4_PIN D6  // GPIO12 - Bomba Reposição
#endif

class RelayController {
private:
  ConfigManager* _config;
  bool _pump1State = false;
  bool _heaterState = false;
  bool _lightState = false;
  bool _pump2State = false;

public:
  bool begin(ConfigManager* config = nullptr);
  void autoControl(float temperature, float ph);
  
  bool getPump1State() const { return _pump1State; }
  bool getHeaterState() const { return _heaterState; }
  bool getLightState() const { return _lightState; }
  bool getPump2State() const { return _pump2State; }
  
  void setPump1(bool state);
  void setHeater(bool state);
  void setLight(bool state);
  void setPump2(bool state);
};

#endif 