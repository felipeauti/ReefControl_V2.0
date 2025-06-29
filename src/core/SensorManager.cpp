#include "SensorManager.h"

void SensorManager::readAll() {
    readTemperature();
    readPH();
    readTDS();
    readWaterLevel();
    checkAlerts();
    validateReadings();
    _data.totalReadings++;
    _data.lastReadTime = millis();
}

void SensorManager::update() {
    if (millis() - _data.lastReadTime >= _data.readInterval) {
        readAll();
    }
}

void SensorManager::readTemperature() {
    _tempSensor.requestTemperatures();
    float temp = _tempSensor.getTempCByIndex(0);
    
    if (temp != DEVICE_DISCONNECTED_C && temp > -50 && temp < 100) {
        float rawTemp = calculateMovingAverage(_tempHistory, temp);
        updateTemperature(rawTemp);
        _data.tempValid = true;
        _data.tempLastRead = millis();
        Serial.printf("🌡️ Temperatura: %.2f°C (raw: %.2f°C, offset: %.2f°C)\n", 
                     _data.temperature, _data.rawTemperature, _data.tempOffset);
    } else {
        _data.tempValid = false;
    }
}

void SensorManager::readPH() {
    // Implementação futura
    _data.phValid = false;
}

void SensorManager::readTDS() {
    // Implementação futura
    _data.tdsValid = false;
}

void SensorManager::readWaterLevel() {
    // Implementação futura
    _data.levelValid = false;
}

float SensorManager::calculateMovingAverage(float* array, float newValue) {
    array[_historyIndex] = newValue;
    _historyIndex = (_historyIndex + 1) % MOVING_AVERAGE_SIZE;
    if (_historyIndex == 0) _historyFull = true;
    
    float sum = 0;
    int count = _historyFull ? MOVING_AVERAGE_SIZE : _historyIndex;
    for (int i = 0; i < count; i++) {
        sum += array[i];
    }
    return sum / count;
}

int SensorManager::calculateMovingAverage(int* array, int newValue) {
    array[_historyIndex] = newValue;
    _historyIndex = (_historyIndex + 1) % MOVING_AVERAGE_SIZE;
    if (_historyIndex == 0) _historyFull = true;
    
    int sum = 0;
    int count = _historyFull ? MOVING_AVERAGE_SIZE : _historyIndex;
    for (int i = 0; i < count; i++) {
        sum += array[i];
    }
    return sum / count;
}

void SensorManager::checkAlerts() {
    // Implementação futura
}

void SensorManager::validateReadings() {
    // Implementação futura
}

float SensorManager::mapPHValue(int rawValue) {
    // Implementação futura
    return 7.0;
}

bool SensorManager::hasAlerts() const {
    return _alerts.tempHigh || _alerts.tempLow || 
           _alerts.phHigh || _alerts.phLow || 
           _alerts.tdsHigh || _alerts.tdsLow || 
           _alerts.levelLow || _alerts.sensorError;
}

String SensorManager::getAlertsString() const {
    // Implementação futura
    return "";
}

void SensorManager::calibratePH(float ph4Value, float ph7Value) {
    // Implementação futura
}

void SensorManager::resetCalibration() {
    // Implementação futura
}

String SensorManager::toJson() const {
    // Implementação futura
    return "";
}

void SensorManager::printReadings() const {
    // Implementação futura
}

void SensorManager::printDiagnostics() const {
    // Implementação futura
}

float SensorManager::celsiusToFahrenheit(float celsius) {
    return (celsius * 9.0 / 5.0) + 32.0;
}

float SensorManager::fahrenheitToCelsius(float fahrenheit) {
    return (fahrenheit - 32.0) * 5.0 / 9.0;
}

String SensorManager::temperatureToString(float temp, bool fahrenheit) {
    if (fahrenheit) {
        temp = celsiusToFahrenheit(temp);
    }
    return String(temp, 1) + (fahrenheit ? "°F" : "°C");
}

String SensorManager::phToString(float ph) {
    return String(ph, 2);
}

String SensorManager::tdsToString(int tds) {
    return String(tds) + " ppm";
}

String SensorManager::levelToString(int level) {
    return String(level) + "%";
} 