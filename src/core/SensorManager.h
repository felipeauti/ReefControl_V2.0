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
#include "ConfigManager.h"

// Pinos dos sensores para Wemos D1 Mini
#define ONE_WIRE_BUS D3    // GPIO0 - Sensor de temperatura DS18B20
#define PH_TDS_PIN A0      // A0 - Sensor pH/TDS analógico
#define LEVEL_PIN A0       // A0 - Sensor de nível (compartilhado ou separado)

// Estrutura para dados dos sensores
struct SensorData {
  // Temperatura
  float temperature = 25.0;
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

// Classe principal
class SensorManager {
private:
  ConfigManager* _config;
  OneWire _oneWire;
  DallasTemperature _tempSensor;
  SensorData _data;
  SensorAlerts _alerts;
  
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
  
public:
  // Construtor
  SensorManager();
  
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
};

#endif // SENSOR_MANAGER_H 