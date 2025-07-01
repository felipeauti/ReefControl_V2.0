/**
 * RelayController.h
 * Controlador de Relés com suporte a ESP8266 (4 saídas) e ESP32 (8 saídas)
 * Integrado com ConfigManager para configuração dinâmica
 */

#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#include <Arduino.h>
#include "ConfigManager.h"
#include "capabilities/Capabilities.h"

class RelayController {
private:
    ConfigManager* _config = nullptr;
    bool* _outputStates = nullptr;  // Array dinâmico para estados
    int _numOutputs = 0;            // Número de saídas (4 ou 8 dependendo do hardware)
    unsigned long* _lastToggle = nullptr;  // Array para controle de intervalos

    void initOutputs();             // Inicializa os pinos conforme configuração
    void cleanupOutputs();          // Limpa recursos alocados
    bool isValidOutput(int index) const;  // Valida índice da saída

public:
    RelayController();
    ~RelayController();
    
    // Inicialização e configuração
    bool begin(ConfigManager* config);
    void updateConfig();  // Atualiza configurações em runtime
    
    // Controle de saídas
    bool setOutput(int index, bool state);
    bool getOutputState(int index) const;
    int getNumOutputs() const { return _numOutputs; }
    
    // Controle automático
    void autoControl(float temperature, float ph = 0);
    
    // Compatibilidade com código legado (deprecated)
    [[deprecated("Use setOutput() instead")]]
    void setPump1(bool state) { setOutput(0, state); }
    [[deprecated("Use setOutput() instead")]]
    void setHeater(bool state) { setOutput(1, state); }
    [[deprecated("Use setOutput() instead")]]
    void setLight(bool state) { setOutput(2, state); }
    [[deprecated("Use setOutput() instead")]]
    void setPump2(bool state) { setOutput(3, state); }
    
    [[deprecated("Use getOutputState() instead")]]
    bool getPump1State() const { return getOutputState(0); }
    [[deprecated("Use getOutputState() instead")]]
    bool getHeaterState() const { return getOutputState(1); }
    [[deprecated("Use getOutputState() instead")]]
    bool getLightState() const { return getOutputState(2); }
    [[deprecated("Use getOutputState() instead")]]
    bool getPump2State() const { return getOutputState(3); }
};

#endif // RELAY_CONTROLLER_H