#include "RelayController.h"

bool RelayController::begin(ConfigManager* config) {
  _config = config;
  
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  
  // Estados iniciais
  setPump1(true);
  setHeater(false);
  setLight(true);
  setPump2(false);
  
  Serial.println("✅ Relés inicializados");
  return true;
}

void RelayController::autoControl(float temperature, float ph) {
  if (!_config) return;
  
  // Controle automático do aquecedor
  if (_config->relay.heaterAutoMode) {
    if (temperature < _config->relay.heaterOnTemp && !_heaterState) {
      setHeater(true);
      Serial.println("🔥 Aquecedor ligado automaticamente");
    } else if (temperature > _config->relay.heaterOffTemp && _heaterState) {
      setHeater(false);
      Serial.println("❄️ Aquecedor desligado automaticamente");
    }
  }
}

void RelayController::setPump1(bool state) {
  _pump1State = state;
  digitalWrite(RELAY1_PIN, state ? LOW : HIGH);
}

void RelayController::setHeater(bool state) {
  _heaterState = state;
  digitalWrite(RELAY2_PIN, state ? LOW : HIGH);
}

void RelayController::setLight(bool state) {
  _lightState = state;
  digitalWrite(RELAY3_PIN, state ? LOW : HIGH);
}

void RelayController::setPump2(bool state) {
  _pump2State = state;
  digitalWrite(RELAY4_PIN, state ? LOW : HIGH);
} 