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
    
    // Primeiro lê todos os sensores físicos
    DeviceAddress addr;
    int deviceCount = _tempSensor.getDeviceCount();
    Serial.printf("📊 Total de sensores físicos: %d\n", deviceCount);
    
    _tempSensor.requestTemperatures();
    bool anyValidReading = false;
    
    // Lê todos os sensores físicos encontrados
    for(int i=0; i<deviceCount; i++) {
        if(_tempSensor.getAddress(addr, i)) {
            // Mostra endereço
            Serial.print("🔍 Endereço do sensor ");
            Serial.print(i);
            Serial.print(": ");
            for (uint8_t j = 0; j < 8; j++) {
                if (addr[j] < 16) Serial.print("0");
                Serial.print(addr[j], HEX);
            }
            Serial.println();
            
            // Mostra resolução
            Serial.printf("📊 Resolução: %d bits\n", _tempSensor.getResolution(addr));
            
            // Lê temperatura
            float rawTemp = _tempSensor.getTempC(addr);
            if (rawTemp != DEVICE_DISCONNECTED_C && rawTemp > -50 && rawTemp < 100) {
                Serial.printf("📡 Temperatura: %.2f°C\n", rawTemp);
                anyValidReading = true;
            } else {
                Serial.printf("❌ Erro ao ler temperatura! (%.2f°C)\n", rawTemp);
            }
        }
    }

    // Agora lê os sensores cadastrados
    Serial.printf("\n📊 Sensores cadastrados: %d\n", _numDS18B20Sensors);
    
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
        Serial.println("\n⚠️ Nenhuma leitura válida obtida!");
    }
    Serial.println();
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

    // Adiciona o sensor
    memcpy(_ds18b20Sensors[_numDS18B20Sensors].address, addr, 8);
    _ds18b20Sensors[_numDS18B20Sensors].name = name;
    _ds18b20Sensors[_numDS18B20Sensors].offset = 0.0;
    _ds18b20Sensors[_numDS18B20Sensors].temperature = 25.0;
    _ds18b20Sensors[_numDS18B20Sensors].rawTemperature = 25.0;
    _ds18b20Sensors[_numDS18B20Sensors].isValid = false; // Será validado na primeira leitura
    _ds18b20Sensors[_numDS18B20Sensors].lastRead = 0;
    _numDS18B20Sensors++;

    // Salva os sensores no arquivo
    saveSensorsToFile();

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
                _ds18b20Sensors[j] = _ds18b20Sensors[j+1];
            }
            _numDS18B20Sensors--;
            
            // Salva os sensores no arquivo
            saveSensorsToFile();
            
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
        json += "{\"address\":\"";
        for (uint8_t j = 0; j < 8; j++) {
            if (_ds18b20Sensors[i].address[j] < 16) json += "0";
            json += String(_ds18b20Sensors[i].address[j], HEX);
        }
        json += "\",\"name\":\"" + _ds18b20Sensors[i].name + "\",";
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
        saveSensorsToFile();
        Serial.printf("✅ Sensor calibrado! Offset: %.2f°C\n", sensor->offset);
    }
}

void SensorManager::resetDS18B20Calibration(const String& address) {
    DS18B20Sensor* sensor = getDS18B20Sensor(address);
    if(sensor) {
        sensor->offset = 0.0;
        sensor->temperature = sensor->rawTemperature;
        saveSensorsToFile();
        Serial.println("✅ Calibração resetada!");
    }
}

bool SensorManager::saveSensorsToFile() {
    DynamicJsonDocument doc(1024);
    JsonArray sensors = doc.createNestedArray("sensors");

    for(int i=0; i<_numDS18B20Sensors; i++) {
        JsonObject sensor = sensors.createNestedObject();
        
        // Converte o endereço para string
        String address = "";
        for (uint8_t j = 0; j < 8; j++) {
            if (_ds18b20Sensors[i].address[j] < 16) address += "0";
            address += String(_ds18b20Sensors[i].address[j], HEX);
        }
        
        sensor["address"] = address;
        sensor["name"] = _ds18b20Sensors[i].name;
        sensor["offset"] = _ds18b20Sensors[i].offset;
    }

    // Serializa o JSON para string primeiro (para debug)
    String jsonStr;
    serializeJsonPretty(doc, jsonStr);
    
    Serial.println("\n💾 Salvando sensores em arquivo...");
    Serial.println("📄 Conteúdo a ser salvo:");
    Serial.println(jsonStr);

    // Abre o arquivo para escrita
    File file = FileSystemClass.open("/sensors.json", "w");
    if (!file) {
        Serial.println("❌ Erro ao abrir arquivo para escrita!");
        return false;
    }

    // Serializa diretamente para o arquivo
    if (serializeJson(doc, file) == 0) {
        Serial.println("❌ Erro ao escrever no arquivo!");
        file.close();
        return false;
    }

    file.close();
    Serial.printf("✅ %d sensor(es) salvo(s) com sucesso!\n", _numDS18B20Sensors);
    return true;
}

bool SensorManager::loadSensorsFromFile() {
    // Verifica se o arquivo existe
    if (!FileSystemClass.exists("/sensors.json")) {
        Serial.println("⚠️ Arquivo de sensores não encontrado.");
        return false;
    }

    // Abre o arquivo para leitura
    File file = FileSystemClass.open("/sensors.json", "r");
    if (!file) {
        Serial.println("❌ Erro ao abrir arquivo para leitura!");
        return false;
    }

    // Lê o conteúdo do arquivo
    String jsonStr = file.readString();
    file.close();

    Serial.println("📄 Conteúdo do arquivo sensors.json:");
    Serial.println(jsonStr);

    // Parse do JSON - Buffer aumentado para ESP8266
    #ifdef ESP32
        DynamicJsonDocument doc(1024);
    #else
        DynamicJsonDocument doc(512); // Reduzido para ESP8266
    #endif
    
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
        Serial.print("❌ Erro ao fazer parse do JSON: ");
        Serial.println(error.c_str());
        return false;
    }

    // Limpa os sensores atuais
    _numDS18B20Sensors = 0;

    // Carrega os sensores do arquivo
    JsonArray sensors = doc["sensors"];
    for(JsonObject sensor : sensors) {
        String address = sensor["address"].as<String>();
        String name = sensor["name"].as<String>();
        float offset = sensor["offset"] | 0.0;

        Serial.printf("\n📥 Carregando sensor '%s'...\n", name.c_str());
        Serial.printf("📍 Endereço: %s\n", address.c_str());
        Serial.printf("📊 Offset: %.2f°C\n", offset);

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

        if(sensorFound && _numDS18B20Sensors < MAX_DS18B20_SENSORS) {
            // Adiciona o sensor
            memcpy(_ds18b20Sensors[_numDS18B20Sensors].address, addr, 8);
            _ds18b20Sensors[_numDS18B20Sensors].name = name;
            _ds18b20Sensors[_numDS18B20Sensors].offset = offset;
            _ds18b20Sensors[_numDS18B20Sensors].temperature = 25.0;
            _ds18b20Sensors[_numDS18B20Sensors].rawTemperature = 25.0;
            _ds18b20Sensors[_numDS18B20Sensors].isValid = true;
            _ds18b20Sensors[_numDS18B20Sensors].lastRead = 0;
            _numDS18B20Sensors++;
            
            Serial.printf("✅ Sensor '%s' carregado com sucesso!\n", name.c_str());
        } else if (!sensorFound) {
            Serial.printf("⚠️ Sensor '%s' não encontrado no barramento - ignorando\n", name.c_str());
        } else {
            Serial.println("⚠️ Número máximo de sensores atingido!");
            break;
        }
    }

    Serial.printf("\n📥 Total de %d sensor(es) carregado(s) do arquivo\n", _numDS18B20Sensors);
    return true;
}

bool SensorManager::begin(ConfigManager* config) {
    _config = config;
    
    // Inicializa o sistema de arquivos
    if (!FileSystemClass.begin()) {
        Serial.println("❌ Erro ao inicializar sistema de arquivos!");
        return false;
    }
    
    Serial.println("\n🌡️ Inicializando sensores de temperatura...");
    
    #ifdef ESP32
        Serial.printf("📍 Pino OneWire: GPIO%d\n", ONE_WIRE_BUS);
    #else
        Serial.printf("📍 Pino OneWire: D2 (GPIO%d)\n", ONE_WIRE_BUS);
    #endif
    
    // Inicializa o sensor OneWire com delay de proteção
    _tempSensor.begin();
    delay(1000); // Delay aumentado para ESP8266
    
    // Verifica se há sensores conectados
    int deviceCount = _tempSensor.getDeviceCount();
    Serial.printf("🔍 Sensores DS18B20 encontrados: %d\n", deviceCount);
    
    if (deviceCount == 0) {
        Serial.println("⚠️ Nenhum sensor DS18B20 encontrado! Verifique as conexões.");
        Serial.println("📌 Dicas de conexão para ESP8266:");
        Serial.println("1. Use o pino D2 (GPIO4)");
        Serial.println("2. Resistor pullup de 4.7k entre DATA e VCC");
        Serial.println("3. Verifique a alimentação (3.3V)");
    }
    
    _tempSensor.setResolution(12); // Configura resolução para 12 bits (0.0625°C)
    
    // Carrega os sensores salvos
    if (!loadSensorsFromFile()) {
        Serial.println("⚠️ Nenhum sensor salvo encontrado.");
    }
    
    // Carrega o offset do ConfigManager
    if (_config) {
        _data.tempOffset = _config->sensor.tempOffset;
        Serial.printf("📥 Offset de temperatura carregado: %.2f°C\n", _data.tempOffset);
    }
    
    Serial.println("✅ Sensores inicializados");
    return true;
} 