/**
 * SensorManager.cpp
 * Implementação básica do Gerenciador de Sensores
 */

#include "SensorManager.h"

SensorManager::SensorManager() : _oneWire(ONE_WIRE_BUS), _tempSensor(&_oneWire) {
  // Inicializar arrays de histórico
  for (int i = 0; i < MOVING_AVERAGE_SIZE; i++) {
    _tempHistory[i] = 25.0;
    _phHistory[i] = 7.0;
    _tdsHistory[i] = 300;
    _levelHistory[i] = 80;
  }
}

bool SensorManager::begin(ConfigManager* config) {
  _config = config;
  
  Serial.println("🌡️ Inicializando sensores...");
  
  // Inicializar sensor de temperatura
  _tempSensor.begin();
  int deviceCount = _tempSensor.getDeviceCount();
  
  if (deviceCount > 0) {
    Serial.printf("✅ %d sensor(es) de temperatura encontrado(s)\n", deviceCount);
    _data.tempValid = true;
  } else {
    Serial.println("⚠️ Nenhum sensor de temperatura encontrado");
    _data.tempValid = false;
  }
  
  // Configurar pino analógico
  pinMode(PH_TDS_PIN, INPUT);
  
  Serial.println("✅ Sensores inicializados");
  return true;
}

void SensorManager::readAll() {
  unsigned long currentTime = millis();
  
  if (currentTime - _data.lastReadTime >= _data.readInterval) {
    readTemperature();
    readPH();
    readTDS();
    readWaterLevel();
    
    checkAlerts();
    validateReadings();
    
    _data.totalReadings++;
    _data.lastReadTime = currentTime;
  }
}

void SensorManager::update() {
  readAll();
}

void SensorManager::readTemperature() {
  if (_data.tempValid) {
    _tempSensor.requestTemperatures();
    float temp = _tempSensor.getTempCByIndex(0);
    
    if (temp != DEVICE_DISCONNECTED_C && temp > -50 && temp < 100) {
      _data.temperature = calculateMovingAverage(_tempHistory, temp);
      if (_config) {
        _data.temperature += _config->sensor.tempOffset;
      }
      _data.tempLastRead = millis();
    } else {
      _data.tempValid = false;
    }
  } else {
    // Simular temperatura se sensor não disponível
    _data.temperature = 24.5 + random(-10, 20) / 10.0;
  }
}

void SensorManager::readPH() {
  int rawValue = analogRead(PH_TDS_PIN);
  _data.phRaw = rawValue;
  
  // Conversão básica (precisa calibração)
  float ph = mapPHValue(rawValue);
  _data.ph = calculateMovingAverage(_phHistory, ph);
  
  if (_config) {
    _data.ph += _config->sensor.phOffset;
  }
  
  _data.phValid = true;
  _data.phLastRead = millis();
}

void SensorManager::readTDS() {
  // Simular TDS baseado no pH (implementação básica)
  int tds = 300 + random(-50, 100);
  _data.tds = calculateMovingAverage(_tdsHistory, tds);
  _data.tdsRaw = _data.phRaw; // Compartilhado com pH
  
  if (_config) {
    _data.tds += _config->sensor.tdsOffset;
  }
  
  _data.tdsValid = true;
  _data.tdsLastRead = millis();
}

void SensorManager::readWaterLevel() {
  // Simular nível da água
  int level = 80 + random(-15, 20);
  _data.waterLevel = calculateMovingAverage(_levelHistory, level);
  _data.levelRaw = analogRead(PH_TDS_PIN);
  
  _data.levelValid = true;
  _data.levelLastRead = millis();
}

float SensorManager::calculateMovingAverage(float* array, float newValue) {
  array[_historyIndex] = newValue;
  
  float sum = 0;
  int count = _historyFull ? MOVING_AVERAGE_SIZE : _historyIndex + 1;
  
  for (int i = 0; i < count; i++) {
    sum += array[i];
  }
  
  _historyIndex = (_historyIndex + 1) % MOVING_AVERAGE_SIZE;
  if (_historyIndex == 0) _historyFull = true;
  
  return sum / count;
}

int SensorManager::calculateMovingAverage(int* array, int newValue) {
  array[_historyIndex] = newValue;
  
  int sum = 0;
  int count = _historyFull ? MOVING_AVERAGE_SIZE : _historyIndex + 1;
  
  for (int i = 0; i < count; i++) {
    sum += array[i];
  }
  
  return sum / count;
}

void SensorManager::checkAlerts() {
  if (!_config) return;
  
  // Reset alerts
  _alerts = SensorAlerts();
  
  // Verificar temperatura
  if (_data.tempValid) {
    _alerts.tempHigh = (_data.temperature > _config->sensor.tempMax);
    _alerts.tempLow = (_data.temperature < _config->sensor.tempMin);
  }
  
  // Verificar pH
  if (_data.phValid) {
    _alerts.phHigh = (_data.ph > _config->sensor.phMax);
    _alerts.phLow = (_data.ph < _config->sensor.phMin);
  }
  
  // Verificar TDS
  if (_data.tdsValid) {
    _alerts.tdsHigh = (_data.tds > _config->sensor.tdsMax);
    _alerts.tdsLow = (_data.tds < _config->sensor.tdsMin);
  }
  
  // Verificar nível
  if (_data.levelValid) {
    _alerts.levelLow = (_data.waterLevel < _config->sensor.levelMin);
  }
}

void SensorManager::validateReadings() {
  // Validar se leituras estão dentro de faixas razoáveis
  if (_data.temperature < -10 || _data.temperature > 50) {
    _data.tempValid = false;
  }
  
  if (_data.ph < 0 || _data.ph > 14) {
    _data.phValid = false;
  }
  
  if (_data.tds < 0 || _data.tds > 2000) {
    _data.tdsValid = false;
  }
  
  if (_data.waterLevel < 0 || _data.waterLevel > 100) {
    _data.levelValid = false;
  }
}

float SensorManager::mapPHValue(int rawValue) {
  // Mapeamento básico - precisa calibração real
  if (_config) {
    float ph4 = _config->sensor.phCalibration4;
    float ph7 = _config->sensor.phCalibration7;
    
    // Interpolação linear básica
    float slope = (7.0 - 4.0) / (ph7 - ph4);
    return 7.0 + slope * (rawValue - ph7);
  }
  
  // Fallback: conversão simples
  return map(rawValue, 0, 1024, 0, 1400) / 100.0;
}

bool SensorManager::hasAlerts() const {
  return _alerts.tempHigh || _alerts.tempLow || 
         _alerts.phHigh || _alerts.phLow ||
         _alerts.tdsHigh || _alerts.tdsLow ||
         _alerts.levelLow || _alerts.sensorError;
}

String SensorManager::getAlertsString() const {
  String alerts = "";
  if (_alerts.tempHigh) alerts += "Temp Alta; ";
  if (_alerts.tempLow) alerts += "Temp Baixa; ";
  if (_alerts.phHigh) alerts += "pH Alto; ";
  if (_alerts.phLow) alerts += "pH Baixo; ";
  if (_alerts.tdsHigh) alerts += "TDS Alto; ";
  if (_alerts.tdsLow) alerts += "TDS Baixo; ";
  if (_alerts.levelLow) alerts += "Nível Baixo; ";
  if (_alerts.sensorError) alerts += "Erro Sensor; ";
  
  return alerts.length() > 0 ? alerts.substring(0, alerts.length() - 2) : "Nenhum";
}

void SensorManager::calibratePH(float ph4Value, float ph7Value) {
  if (_config) {
    _config->sensor.phCalibration4 = ph4Value;
    _config->sensor.phCalibration7 = ph7Value;
    _config->save();
    Serial.println("✅ Calibração pH salva");
  }
}

void SensorManager::resetCalibration() {
  if (_config) {
    _config->sensor.phCalibration4 = 512.0;
    _config->sensor.phCalibration7 = 410.0;
    _config->save();
    Serial.println("🔄 Calibração pH resetada");
  }
}

String SensorManager::toJson() const {
  String json = "{";
  json += "\"temperature\":" + String(_data.temperature, 1) + ",";
  json += "\"ph\":" + String(_data.ph, 1) + ",";
  json += "\"tds\":" + String(_data.tds) + ",";
  json += "\"waterLevel\":" + String(_data.waterLevel) + ",";
  json += "\"tempValid\":" + String(_data.tempValid ? "true" : "false") + ",";
  json += "\"phValid\":" + String(_data.phValid ? "true" : "false") + ",";
  json += "\"tdsValid\":" + String(_data.tdsValid ? "true" : "false") + ",";
  json += "\"levelValid\":" + String(_data.levelValid ? "true" : "false") + ",";
  json += "\"totalReadings\":" + String(_data.totalReadings) + ",";
  json += "\"uptime\":" + String(millis());
  json += "}";
  return json;
}

void SensorManager::printReadings() const {
  // Print removido para não poluir o terminal
}

void SensorManager::printDiagnostics() const {
  Serial.println("🔍 Diagnóstico dos Sensores:");
  Serial.printf("  Temperatura: %.1f°C %s\n", _data.temperature, _data.tempValid ? "✅" : "❌");
  Serial.printf("  pH: %.1f %s\n", _data.ph, _data.phValid ? "✅" : "❌");
  Serial.printf("  TDS: %d ppm %s\n", _data.tds, _data.tdsValid ? "✅" : "❌");
  Serial.printf("  Nível: %d%% %s\n", _data.waterLevel, _data.levelValid ? "✅" : "❌");
  Serial.printf("  Total leituras: %lu\n", _data.totalReadings);
  Serial.printf("  Alertas: %s\n", getAlertsString().c_str());
}

// Métodos estáticos
float SensorManager::celsiusToFahrenheit(float celsius) {
  return (celsius * 9.0 / 5.0) + 32.0;
}

float SensorManager::fahrenheitToCelsius(float fahrenheit) {
  return (fahrenheit - 32.0) * 5.0 / 9.0;
}

String SensorManager::temperatureToString(float temp, bool fahrenheit) {
  if (fahrenheit) {
    return String(celsiusToFahrenheit(temp), 1) + "°F";
  }
  return String(temp, 1) + "°C";
}

String SensorManager::phToString(float ph) {
  return String(ph, 1);
}

String SensorManager::tdsToString(int tds) {
  return String(tds) + " ppm";
}

String SensorManager::levelToString(int level) {
  return String(level) + "%";
} 