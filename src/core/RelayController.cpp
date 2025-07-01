/**
 * RelayController.cpp
 * Implementação do controlador de relés com suporte multi-plataforma
 */

#include "RelayController.h"

RelayController::RelayController() {
    _config = nullptr;
    _outputStates = nullptr;
    _lastToggle = nullptr;
    _numOutputs = 0;
}

RelayController::~RelayController() {
    cleanupOutputs();
}

void RelayController::cleanupOutputs() {
    if (_outputStates) {
        delete[] _outputStates;
        _outputStates = nullptr;
    }
    if (_lastToggle) {
        delete[] _lastToggle;
        _lastToggle = nullptr;
    }
}

bool RelayController::isValidOutput(int index) const {
    if (!_config || !_outputStates) return false;
    if (index < 0 || index >= _numOutputs) return false;
    #ifdef ESP32
        return true;  // ESP32 suporta todas as 8 saídas
    #else
        return index < 4;  // ESP8266 suporta apenas 4 saídas
    #endif
}

void RelayController::initOutputs() {
    // Determina número de saídas baseado no hardware
    #ifdef ESP32
        _numOutputs = 8;  // Pro version
    #else
        _numOutputs = 4;  // Compact version
    #endif
    
    // Aloca arrays dinâmicos
    cleanupOutputs();
    _outputStates = new bool[_numOutputs]();  // Inicializa com false
    _lastToggle = new unsigned long[_numOutputs]();  // Inicializa com 0
    
    // Configura os pinos conforme ConfigManager
    for (int i = 0; i < _numOutputs; i++) {
        if (isValidOutput(i)) {
            pinMode(_config->relay.outputs[i].pin, OUTPUT);
            digitalWrite(_config->relay.outputs[i].pin, HIGH);  // Relés são ativos em LOW
            _outputStates[i] = false;
            _lastToggle[i] = 0;
            
            Serial.printf("✓ Saída %d configurada: %s (GPIO %d)\n", 
                i, 
                _config->relay.outputs[i].name,
                _config->relay.outputs[i].pin
            );
        }
    }
}

bool RelayController::begin(ConfigManager* config) {
    _config = config;
    
    if (!_config) {
        Serial.println("❌ RelayController: ConfigManager não inicializado!");
        return false;
    }
    
    initOutputs();
    Serial.printf("✅ RelayController inicializado com %d saídas\n", _numOutputs);
    return true;
}

void RelayController::autoControl(float temperature, float ph) {
    if (!_config || !_outputStates || !_lastToggle) return;
    
    unsigned long now = millis();
    
    // Controle automático para cada saída
    for (int i = 0; i < _numOutputs; i++) {
        if (!isValidOutput(i)) continue;
        if (!_config->relay.outputs[i].enabled) continue;
        if (!_config->relay.outputs[i].autoMode) continue;
        
        // Controle por temperatura
        if (_config->relay.outputs[i].onTemp > 0 || _config->relay.outputs[i].offTemp > 0) {
            if (temperature < _config->relay.outputs[i].onTemp && !_outputStates[i]) {
                setOutput(i, true);
                Serial.printf("🌡️ Saída %s ligada por temperatura (%.1f°C)\n", 
                    _config->relay.outputs[i].name, temperature);
            }
            else if (temperature > _config->relay.outputs[i].offTemp && _outputStates[i]) {
                setOutput(i, false);
                Serial.printf("🌡️ Saída %s desligada por temperatura (%.1f°C)\n", 
                    _config->relay.outputs[i].name, temperature);
            }
        }
        
        // Controle por horário
        if (_config->relay.outputs[i].onTime > 0 || _config->relay.outputs[i].offTime > 0) {
            time_t nowTime;
            time(&nowTime);
            struct tm* timeinfo = localtime(&nowTime);
            int currentHour = timeinfo->tm_hour;
            
            if (currentHour >= _config->relay.outputs[i].onTime && 
                currentHour < _config->relay.outputs[i].offTime && 
                !_outputStates[i]) {
                setOutput(i, true);
                Serial.printf("⏰ Saída %s ligada por horário (%d:00)\n", 
                    _config->relay.outputs[i].name, currentHour);
            }
            else if ((currentHour < _config->relay.outputs[i].onTime || 
                     currentHour >= _config->relay.outputs[i].offTime) && 
                     _outputStates[i]) {
                setOutput(i, false);
                Serial.printf("⏰ Saída %s desligada por horário (%d:00)\n", 
                    _config->relay.outputs[i].name, currentHour);
            }
        }
        
        // Controle por intervalo
        if (_config->relay.outputs[i].interval > 0 && _config->relay.outputs[i].duration > 0) {
            unsigned long interval = _config->relay.outputs[i].interval * 60000UL;  // min para ms
            unsigned long duration = _config->relay.outputs[i].duration * 1000UL;   // seg para ms
            
            if (!_outputStates[i] && (now - _lastToggle[i] >= interval)) {
                setOutput(i, true);
                _lastToggle[i] = now;
                Serial.printf("⏱️ Saída %s ligada por intervalo\n", 
                    _config->relay.outputs[i].name);
            }
            else if (_outputStates[i] && (now - _lastToggle[i] >= duration)) {
                setOutput(i, false);
                Serial.printf("⏱️ Saída %s desligada por duração\n", 
                    _config->relay.outputs[i].name);
            }
        }
    }
}

bool RelayController::setOutput(int index, bool state) {
    if (!isValidOutput(index)) return false;
    
    digitalWrite(_config->relay.outputs[index].pin, state ? LOW : HIGH);
    _outputStates[index] = state;
    
    Serial.printf("%s Saída %s (%d) %s\n", 
        state ? "🔌" : "🔍",
        _config->relay.outputs[index].name,
        _config->relay.outputs[index].pin,
        state ? "ligada" : "desligada"
    );
    
    return true;
}

bool RelayController::getOutputState(int index) const {
    if (!isValidOutput(index)) return false;
    return _outputStates[index];
}

void RelayController::updateConfig() {
    if (_config) {
        initOutputs();  // Reinicializa os pinos com novas configurações
    }
}