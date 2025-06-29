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
    if (millis() - _data.tempLastRead < _data.readInterval) return;
    
    Serial.println("\n🌡️ Lendo temperaturas...");
    Serial.printf("📊 Sensores cadastrados: %d\n", _numDS18B20Sensors);
    
    _tempSensor.requestTemperatures();
    bool anyValidReading = false;

    // Lê todos os sensores cadastrados
    for(int i=0; i<_numDS18B20Sensors; i++) {
        Serial.printf("🔍 Lendo sensor %d ('%s')...\n", i, _ds18b20Sensors[i].name.c_str());
        
        // Debug do endereço
        Serial.print("📍 Endereço: ");
        for (uint8_t j = 0; j < 8; j++) {
            if (_ds18b20Sensors[i].address[j] < 16) Serial.print("0");
            Serial.print(_ds18b20Sensors[i].address[j], HEX);
        }
        Serial.println();
        
        float rawTemp = _tempSensor.getTempC(_ds18b20Sensors[i].address);
        Serial.printf("📡 Temperatura raw: %.2f°C\n", rawTemp);
        
        if (rawTemp != DEVICE_DISCONNECTED_C && rawTemp > -50 && rawTemp < 100) {
            _ds18b20Sensors[i].rawTemperature = rawTemp;
            _ds18b20Sensors[i].temperature = rawTemp + _ds18b20Sensors[i].offset;
            _ds18b20Sensors[i].isValid = true;
            _ds18b20Sensors[i].lastRead = millis();
            anyValidReading = true;

            Serial.printf("✅ %s: %.2f°C (raw: %.2f°C, offset: %.2f°C)\n", 
                _ds18b20Sensors[i].name.c_str(),
                _ds18b20Sensors[i].temperature,
                _ds18b20Sensors[i].rawTemperature,
                _ds18b20Sensors[i].offset);
        } else {
            _ds18b20Sensors[i].isValid = false;
            Serial.printf("❌ Erro ao ler sensor '%s'! (%.2f°C)\n", 
                _ds18b20Sensors[i].name.c_str(), 
                rawTemp);
        }
    }

    _data.tempValid = anyValidReading;
    _data.tempLastRead = millis();
    
    if (!anyValidReading) {
        Serial.println("⚠️ Nenhuma leitura válida obtida!");
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

bool SensorManager::addDS18B20Sensor(const String& address, const String& name) {
    if (_numDS18B20Sensors >= MAX_DS18B20_SENSORS) {
        Serial.println("❌ Número máximo de sensores atingido!");
        return false;
    }

    // Converte o endereço string para bytes
    DeviceAddress addr;
    for(int i=0; i<8; i++) {
        addr[i] = strtoul(address.substring(i*2, i*2+2).c_str(), NULL, 16);
    }

    // Verifica se o sensor existe no barramento
    bool sensorFound = false;
    DeviceAddress tempAddr;
    for(int i=0; i<_tempSensor.getDeviceCount(); i++) {
        if(_tempSensor.getAddress(tempAddr, i)) {
            if(memcmp(addr, tempAddr, 8) == 0) {
                sensorFound = true;
                break;
            }
        }
    }

    if(!sensorFound) {
        Serial.println("❌ Sensor não encontrado no barramento!");
        return false;
    }

    // Verifica se o sensor já está cadastrado
    for(int i=0; i<_numDS18B20Sensors; i++) {
        if(memcmp(addr, _ds18b20Sensors[i].address, 8) == 0) {
            Serial.println("❌ Sensor já cadastrado!");
            return false;
        }
    }

    // Adiciona o novo sensor
    memcpy(_ds18b20Sensors[_numDS18B20Sensors].address, addr, 8);
    _ds18b20Sensors[_numDS18B20Sensors].name = name;
    _ds18b20Sensors[_numDS18B20Sensors].temperature = 25.0;
    _ds18b20Sensors[_numDS18B20Sensors].rawTemperature = 25.0;
    _ds18b20Sensors[_numDS18B20Sensors].offset = 0.0;
    _ds18b20Sensors[_numDS18B20Sensors].isValid = true;
    _ds18b20Sensors[_numDS18B20Sensors].lastRead = 0;
    _numDS18B20Sensors++;

    Serial.printf("✅ Sensor DS18B20 '%s' adicionado com sucesso!\n", name.c_str());
    return true;
}

bool SensorManager::removeDS18B20Sensor(const String& address) {
    DeviceAddress addr;
    for(int i=0; i<8; i++) {
        addr[i] = strtoul(address.substring(i*2, i*2+2).c_str(), NULL, 16);
    }

    // Procura o sensor
    for(int i=0; i<_numDS18B20Sensors; i++) {
        if(memcmp(addr, _ds18b20Sensors[i].address, 8) == 0) {
            // Remove movendo os sensores restantes
            for(int j=i; j<_numDS18B20Sensors-1; j++) {
                memcpy(&_ds18b20Sensors[j], &_ds18b20Sensors[j+1], sizeof(DS18B20Sensor));
            }
            _numDS18B20Sensors--;
            Serial.println("✅ Sensor removido com sucesso!");
            return true;
        }
    }

    Serial.println("❌ Sensor não encontrado!");
    return false;
}

DS18B20Sensor* SensorManager::getDS18B20Sensor(const String& address) {
    DeviceAddress addr;
    for(int i=0; i<8; i++) {
        addr[i] = strtoul(address.substring(i*2, i*2+2).c_str(), NULL, 16);
    }

    for(int i=0; i<_numDS18B20Sensors; i++) {
        if(memcmp(addr, _ds18b20Sensors[i].address, 8) == 0) {
            return &_ds18b20Sensors[i];
        }
    }
    return nullptr;
}

String SensorManager::getDS18B20SensorsJson() const {
    String json = "[";
    for(int i=0; i<_numDS18B20Sensors; i++) {
        if(i > 0) json += ",";
        json += "{";
        json += "\"address\":\"";
        for(int j=0; j<8; j++) {
            if(_ds18b20Sensors[i].address[j] < 16) json += "0";
            json += String(_ds18b20Sensors[i].address[j], HEX);
        }
        json += "\",";
        json += "\"name\":\"" + _ds18b20Sensors[i].name + "\",";
        json += "\"temperature\":" + String(_ds18b20Sensors[i].temperature, 2) + ",";
        json += "\"rawTemperature\":" + String(_ds18b20Sensors[i].rawTemperature, 2) + ",";
        json += "\"offset\":" + String(_ds18b20Sensors[i].offset, 2) + ",";
        json += "\"isValid\":" + String(_ds18b20Sensors[i].isValid ? "true" : "false");
        json += "}";
    }
    json += "]";
    return json;
}

void SensorManager::calibrateDS18B20Sensor(const String& address, float measuredTemp) {
    DS18B20Sensor* sensor = getDS18B20Sensor(address);
    if(sensor) {
        sensor->offset = measuredTemp - sensor->rawTemperature;
        sensor->temperature = sensor->rawTemperature + sensor->offset;
        Serial.printf("✅ Sensor calibrado! Offset: %.2f°C\n", sensor->offset);
    }
}

void SensorManager::resetDS18B20Calibration(const String& address) {
    DS18B20Sensor* sensor = getDS18B20Sensor(address);
    if(sensor) {
        sensor->offset = 0.0;
        sensor->temperature = sensor->rawTemperature;
        Serial.println("✅ Calibração resetada!");
    }
} 