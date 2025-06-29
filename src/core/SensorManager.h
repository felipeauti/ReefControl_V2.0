/**
 * SensorManager.h
 * Gerenciador de Sensores do ReefControl
 * 
 * Responsável por:
 * - Leitura de sensores (temperatura, pH, TDS, nível)
 * - Calibração e validação de dados
 * - Histórico e médias móveis
 * - Alertas e limites
 */

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include "ConfigManager.h"

#ifdef ESP32
  #include <SPIFFS.h>
  #define FileSystemClass SPIFFS
#else
  #include <LittleFS.h>
  #define FileSystemClass LittleFS
#endif

// Pinos dos sensores - compatibilidade ESP8266/ESP32
#ifdef ESP32
  #define ONE_WIRE_BUS 4     // GPIO4 - Sensor de temperatura DS18B20
  #define PH_TDS_PIN 36      // GPIO36 (ADC1_CH0) - Sensor pH/TDS analógico
  #define LEVEL_PIN 39       // GPIO39 (ADC1_CH3) - Sensor de nível
#else
  #define ONE_WIRE_BUS D2    // GPIO4 (D2) - Sensor de temperatura DS18B20 (Wemos D1 Mini)
  #define PH_TDS_PIN A0      // A0 - Sensor pH/TDS analógico
  #define LEVEL_PIN A0       // A0 - Sensor de nível (compartilhado ou separado)
#endif

// Estrutura para dados dos sensores
struct SensorData {
  // Temperatura
  float temperature = 25.0;  // Temperatura com offset aplicado
  float rawTemperature = 25.0;  // Temperatura raw do sensor
  float tempOffset = 0.0;  // Offset de calibração
  bool tempValid = false;
  unsigned long tempLastRead = 0;
  
  // pH
  float ph = 7.0;
  bool phValid = false;
  int phRaw = 0;
  unsigned long phLastRead = 0;
  
  // TDS (Total Dissolved Solids)
  int tds = 300;
  bool tdsValid = false;
  int tdsRaw = 0;
  unsigned long tdsLastRead = 0;
  
  // Nível da água
  int waterLevel = 80;
  bool levelValid = false;
  int levelRaw = 0;
  unsigned long levelLastRead = 0;
  
  // Estatísticas
  unsigned long totalReadings = 0;
  unsigned long lastReadTime = 0;
  unsigned long readInterval = 5000; // 5 segundos
};

// Estrutura para alertas
struct SensorAlerts {
  bool tempHigh = false;
  bool tempLow = false;
  bool phHigh = false;
  bool phLow = false;
  bool tdsHigh = false;
  bool tdsLow = false;
  bool levelLow = false;
  bool sensorError = false;
};

// Estrutura para sensor DS18B20
struct DS18B20Sensor {
  DeviceAddress address;  // Endereço OneWire do sensor
  String name;           // Nome amigável do sensor (ex: "Aquário")
  float temperature;     // Última temperatura lida
  float rawTemperature;  // Temperatura raw
  float offset;         // Offset de calibração
  bool isValid;         // Status do sensor
  unsigned long lastRead; // Última leitura
};

// Classe principal
class SensorManager {
private:
  ConfigManager* _config;
  OneWire _oneWire;
  DallasTemperature _tempSensor;
  SensorData _data;
  SensorAlerts _alerts;
  
  // Array de sensores DS18B20
  static const int MAX_DS18B20_SENSORS = 4;  // Máximo de 4 sensores
  DS18B20Sensor _ds18b20Sensors[MAX_DS18B20_SENSORS];
  int _numDS18B20Sensors = 0;  // Número atual de sensores
  
  // Arrays para médias móveis
  static const int MOVING_AVERAGE_SIZE = 10;
  float _tempHistory[MOVING_AVERAGE_SIZE];
  float _phHistory[MOVING_AVERAGE_SIZE];
  int _tdsHistory[MOVING_AVERAGE_SIZE];
  int _levelHistory[MOVING_AVERAGE_SIZE];
  int _historyIndex = 0;
  bool _historyFull = false;
  
  // Métodos privados
  void readTemperature();
  void readPH();
  void readTDS();
  void readWaterLevel();
  
  float calculateMovingAverage(float* array, float newValue);
  int calculateMovingAverage(int* array, int newValue);
  
  void checkAlerts();
  void validateReadings();
  
  // Calibração pH
  float mapPHValue(int rawValue);
  
  // Método privado para aplicar offset
  float applyTempOffset(float rawTemp) {
    return rawTemp + _data.tempOffset;
  }
  
  // Método privado para calcular temperatura com offset
  void updateTemperature(float rawTemp) {
    _data.rawTemperature = rawTemp;
    _data.temperature = applyTempOffset(rawTemp);
  }
  
  // Métodos para persistência
  bool saveSensorsToFile();
  bool loadSensorsFromFile();
  
public:
  // Construtor
  SensorManager() : _oneWire(ONE_WIRE_BUS), _tempSensor(&_oneWire) {
    _numDS18B20Sensors = 0;
  }
  
  // Métodos principais
  bool begin(ConfigManager* config = nullptr);
  void readAll();
  void update();
  
  // Getters para dados dos sensores
  float getTemperature() const { return _data.temperature; }
  float getPH() const { return _data.ph; }
  int getTDS() const { return _data.tds; }
  int getWaterLevel() const { return _data.waterLevel; }
  
  // Getters para dados brutos
  int getPHRaw() const { return _data.phRaw; }
  int getTDSRaw() const { return _data.tdsRaw; }
  int getLevelRaw() const { return _data.levelRaw; }
  
  // Status dos sensores
  bool isTemperatureValid() const { return _data.tempValid; }
  bool isPHValid() const { return _data.phValid; }
  bool isTDSValid() const { return _data.tdsValid; }
  bool isLevelValid() const { return _data.levelValid; }
  bool hasValidReadings() const { return _data.tempValid || _data.phValid; }
  
  // Alertas
  SensorAlerts getAlerts() const { return _alerts; }
  bool hasAlerts() const;
  String getAlertsString() const;
  
  // Estatísticas
  unsigned long getTotalReadings() const { return _data.totalReadings; }
  unsigned long getLastReadTime() const { return _data.lastReadTime; }
  unsigned long getUptime() const { return millis(); }
  
  // Configurações
  void setReadInterval(unsigned long interval) { _data.readInterval = interval; }
  unsigned long getReadInterval() const { return _data.readInterval; }
  
  // Calibração
  void calibratePH(float ph4Value, float ph7Value);
  void resetCalibration();
  
  // Métodos de calibração de temperatura
  float getRawTemperature() const { 
    return _data.rawTemperature;
  }
  
  float getTempOffset() const { 
    return _data.tempOffset; 
  }
  
  void calibrateTemperature(float measuredTemp) {
    // Calcula o offset como a diferença entre a temperatura medida e a temperatura raw
    _data.tempOffset = measuredTemp - _data.rawTemperature;
    // Atualiza a temperatura com o novo offset
    _data.temperature = applyTempOffset(_data.rawTemperature);
    
    // Salva o offset no ConfigManager
    if (_config) {
      _config->sensor.tempOffset = _data.tempOffset;
      _config->save();
      Serial.printf("💾 Offset de temperatura salvo: %.2f°C\n", _data.tempOffset);
    }
  }
  
  void resetTempCalibration() {
    _data.tempOffset = 0.0;
    _data.temperature = _data.rawTemperature;
    
    // Persiste o reset no ConfigManager
    if (_config) {
      _config->sensor.tempOffset = 0.0;
      _config->save();
      Serial.println("🔄 Calibração de temperatura resetada");
    }
  }
  
  // Utilitários
  String toJson() const;
  void printReadings() const;
  void printDiagnostics() const;
  
  // Métodos estáticos para conversões
  static float celsiusToFahrenheit(float celsius);
  static float fahrenheitToCelsius(float fahrenheit);
  static String temperatureToString(float temp, bool fahrenheit = false);
  static String phToString(float ph);
  static String tdsToString(int tds);
  static String levelToString(int level);

  // Busca endereços OneWire disponíveis
  String scanOneWireAddresses() {
    String addresses = "";
    DeviceAddress addr;
    int deviceCount = _tempSensor.getDeviceCount();
    
    for(int i=0; i<deviceCount; i++) {
      if(_tempSensor.getAddress(addr, i)) {
        if(i > 0) addresses += ",";
        for (uint8_t j = 0; j < 8; j++) {
          if (addr[j] < 16) addresses += "0";
          addresses += String(addr[j], HEX);
        }
      }
    }
    return addresses;
  }

  // Métodos para gerenciar sensores DS18B20
  bool addDS18B20Sensor(const String& address, const String& name);
  bool removeDS18B20Sensor(const String& address);
  DS18B20Sensor* getDS18B20Sensor(const String& address);
  String getDS18B20SensorsJson() const;
  void calibrateDS18B20Sensor(const String& address, float measuredTemp);
  void resetDS18B20Calibration(const String& address);
  
  // Busca endereços OneWire disponíveis e retorna em formato JSON
  String scanOneWireAddressesJson() {
    String json = "[";
    DeviceAddress addr;
    int deviceCount = _tempSensor.getDeviceCount();
    
    for(int i=0; i<deviceCount; i++) {
      if(_tempSensor.getAddress(addr, i)) {
        if(i > 0) json += ",";
        json += "{\"address\":\"";
        for (uint8_t j = 0; j < 8; j++) {
          if (addr[j] < 16) json += "0";
          json += String(addr[j], HEX);
        }
        json += "\",\"inUse\":";
        // Verifica se o endereço já está em uso
        bool inUse = false;
        for(int k=0; k<_numDS18B20Sensors; k++) {
          if(memcmp(addr, _ds18b20Sensors[k].address, 8) == 0) {
            inUse = true;
            break;
          }
        }
        json += inUse ? "true" : "false";
        json += "}";
      }
    }
    json += "]";
    return json;
  }
};

#endif // SENSOR_MANAGER_H 