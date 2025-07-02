# 🤖 **ROADMAP - SISTEMA DE AUTOMAÇÃO AVANÇADA**
## ReefControl v3.2.3-beta → v3.3.0-beta

---

## 📋 **VISÃO GERAL DO PROJETO**

### **Objetivo:**
Separar toda a lógica de controle automático das outputs em um sistema modular e escalável (`automation.cpp` + `automation.h`), permitindo 10 modos de automação diferentes por saída.

### **Motivação:**
- ✅ **Modularidade**: Separar lógica de automação da lógica de hardware
- ✅ **Escalabilidade**: Facilitar adição de novos modos de automação
- ✅ **Reutilização**: Sistema utilizável em qualquer projeto ESP8266/ESP32
- ✅ **Manutenibilidade**: Código organizado e testável
- ✅ **Performance**: Otimizado para recursos limitados

### **Convenção de Nomenclatura:**
- ✅ **Nomes Genéricos**: "Output 1", "Output 2", "Output 3", "Output 4"
- ✅ **Flexibilidade Total**: Usuário define o uso de cada saída
- ✅ **Sem Hardcoding**: Não assumir função específica (bomba, termostato, etc.)
- ✅ **Configurável**: Interface permite renomear outputs conforme necessidade

---

## 🏗️ **ARQUITETURA PROPOSTA**

### **1. ESTRUTURA DE ARQUIVOS**
```
src/
├── automation/
│   ├── Automation.h              # Classes e enums principais
│   ├── Automation.cpp            # Implementação da lógica
│   ├── AutomationTypes.h         # Tipos e estruturas
│   └── README.md                 # Documentação do sistema
├── core/
│   ├── RelayController.h         # Mantém interface atual
│   ├── RelayController.cpp       # Integra com Automation
│   └── ...outros arquivos...
```

### **2. CLASSES PRINCIPAIS**

#### **AutomationOutput** (Baseada no seu exemplo)
```cpp
class AutomationOutput {
private:
    uint8_t _pin;                    // GPIO da saída
    bool _currentState;              // Estado atual (ON/OFF)
    AutomationMode _mode;            // Modo de automação ativo
    uint32_t _lastUpdateTime;        // Controle de frequência de update
    
    // Union para economizar RAM (genial!)
    union AutomationConfig {
        ThermostatConfig thermostat;    // MODE_HEATER, MODE_COOLER
        SetpointConfig setpoint;        // MODE_SETPOINT_HYSTERESIS
        ScheduleConfig schedule;        // MODE_SCHEDULE
        TimerConfig timer;              // MODE_TIMER, MODE_SCHEDULE_TIMER
        LevelDualConfig levelDual;      // MODE_LEVEL_DUAL_SENSOR
        LevelSingleConfig levelSingle;  // MODE_LEVEL_SINGLE_SENSOR
        GenericSensorConfig generic;    // MODE_GENERIC_SENSOR
        // PidConfig pid;               // MODE_PID (futuro)
    } _config;
    
    // Máquinas de estado para lógicas complexas
    LevelState _levelState;
    
public:
    AutomationOutput(uint8_t pin);
    void update(float temp = 0, bool digitalLow = false, bool digitalHigh = false);
    void setMode(AutomationMode mode);
    bool getState() const;
    void setState(bool state);  // Apenas para MODE_MANUAL
    
    // Configuradores para cada modo
    void setThermostat(float threshold, bool isCooler = false);
    void setSetpoint(float target, float hysteresis);
    void setSchedule(const ScheduleTime onTimes[4], const ScheduleTime offTimes[4]);
    void setTimer(uint32_t duration_ms);
    void setLevelDual(uint8_t pinLow, uint8_t pinHigh, uint32_t timeout_s);
    void setLevelSingle(uint8_t pinLow, uint32_t waitDelay_s, uint32_t fillTime_s);
    void setGenericSensor(float threshold, bool triggerOnHigh, std::function<float()> readFunc);
    
    // Utilitários
    void resetLevelLearning();
    String getStatusJson() const;
};
```

#### **AutomationManager** (Nova classe integradora)
```cpp
class AutomationManager {
private:
    AutomationOutput* _outputs[MAX_OUTPUTS];  // Array de outputs
    ConfigManager* _config;                   // Referência para configurações
    SensorManager* _sensors;                  // Referência para sensores
    int _numOutputs;
    
public:
    AutomationManager();
    ~AutomationManager();
    
    bool begin(ConfigManager* config, SensorManager* sensors, int numOutputs);
    void update();  // Atualiza todas as saídas
    
    // Interface para RelayController
    AutomationOutput* getOutput(int index);
    bool setOutputMode(int index, AutomationMode mode);
    bool setOutputState(int index, bool state);
    bool getOutputState(int index) const;
    
    // Configuração em massa
    bool loadFromConfig();
    bool saveToConfig();
    String getStatusJson() const;
};
```

---

## 📊 **10 MODOS DE AUTOMAÇÃO DETALHADOS**

### **1. MODE_HEATER** 🌡️ (Aquecimento)
- **Lógica**: `temp < threshold` → Liga | `temp >= threshold` → Desliga
- **Uso**: Aquecedores, resistências
- **Config**: `threshold` (float)

### **2. MODE_COOLER** ❄️ (Resfriamento)  
- **Lógica**: `temp > threshold` → Liga | `temp <= threshold` → Desliga
- **Uso**: Resfriadores, ventiladores, sistemas de refrigeração
- **Config**: `threshold` (float)

### **3. MODE_SETPOINT_HYSTERESIS** 🎯 (Setpoint com Histerese)
- **Lógica**: Mantém temperatura no setpoint ±histerese
- **Uso**: Controle preciso de temperatura
- **Config**: `target` (float), `hysteresis` (float)
- **Exemplo**: Target 25°C, Histerese 0.5°C (Liga <24.5°C, Desliga >25.5°C)

### **4. MODE_PID** 🔬 (Futuro)
- **Lógica**: Controle PID completo
- **Config**: `setpoint`, `kp`, `ki`, `kd`, `deadband`
- **Features**: Auto-tuning, wind-up protection

### **5. MODE_SCHEDULE** ⏰ (Agendamento)
- **Lógica**: Liga/desliga em horários programados
- **Uso**: Iluminação, sistemas automatizados, equipamentos temporizados
- **Config**: 4 horários ON + 4 horários OFF por saída
- **Features**: Configuração via modal/popup

### **6. MODE_TIMER** ⏱️ (Timer)
- **Lógica**: Liga por tempo determinado e desliga automaticamente  
- **Uso**: Bombas de dosagem, UV, ozônio, sistemas temporizados
- **Config**: `duration_ms` (uint32_t)

### **7. MODE_SCHEDULE_TIMER** ⏰⏱️ (Schedule + Timer)
- **Lógica**: Schedule liga → Timer conta → Desliga automaticamente
- **Uso**: Dosagem automática, sistemas temporizados, equipamentos programados
- **Config**: Schedule + Timer duration

### **8. MODE_LEVEL_DUAL_SENSOR** 💧 (Reposição com 2 sensores)
- **Lógica**: Nível baixo liga → Nível alto desliga
- **Features**: 
  - Timer de segurança
  - Sistema de aprendizado (média de 10 operações)
  - Auto-ajuste de timing (média + 20%)
  - Reset de aprendizado
- **Config**: `pinLow`, `pinHigh`, `safetyTimeout`, `fillTimeout`

### **9. MODE_LEVEL_SINGLE_SENSOR** 💧 (Reposição com 1 sensor)
- **Lógica**: Timer-based com sensor único
- **Features**:
  - Delay ajustável antes de ligar
  - Tempo fixo de enchimento
  - Sistema de aprendizado similar ao dual
- **Config**: `pinLow`, `waitDelay`, `fillTime`

### **10. MODE_GENERIC_SENSOR** 🔧 (Sensor Configurável)
- **Lógica**: Totalmente configurável via função callback
- **Features**:
  - Suporte a qualquer sensor (Temperatura, ds18b20(sensores já adicionados), DHT11, DHT22, pH, TDS, ORP, etc.)
  - Threshold configurável
  - Lógica alta/baixa selecionável
- **Config**: `threshold`, `triggerOnHigh`, `readValueFunc`

---

## 🔧 **INTEGRAÇÃO COM SISTEMA ATUAL**

### **1. RelayController Modifications**
```cpp
// RelayController.h - Mantém interface atual
class RelayController {
private:
    AutomationManager* _automation;  // Nova dependência
    
public:
    // APIs atuais mantidas (compatibilidade)
    bool setOutput(int index, bool state);
    bool getOutputState(int index) const;
    
    // Novas APIs de automação
    bool setOutputAutomation(int index, AutomationMode mode);
    AutomationOutput* getAutomationOutput(int index);
    
    // Automação ativa
    void autoControl(float temperature, float ph = 0) override;
};
```

### **2. ConfigManager Extensions**
```cpp
// Novas estruturas na configuração
struct AutomationConfig {
    AutomationMode mode;
    bool enabled;
    
    // Union com todas as configurações
    union {
        ThermostatConfig thermostat;
        SetpointConfig setpoint;
        ScheduleConfig schedule;
        TimerConfig timer;
        LevelDualConfig levelDual;
        LevelSingleConfig levelSingle;
        GenericSensorConfig generic;
    } params;
};

struct OutputConfig {
    char name[32];     // Ex: "Output 1", "Output 2", etc.
    int pin;
    bool enabled;
    AutomationConfig automation;  // Nova seção
};
```

### **3. WebServer APIs**
```cpp
// Novos endpoints REST
GET  /api/outputs/{id}/automation        // Status da automação
POST /api/outputs/{id}/automation/mode   // Configurar modo
POST /api/outputs/{id}/automation/config // Configurar parâmetros
GET  /api/automation/status              // Status geral
POST /api/automation/schedule/{id}       // Configurar schedule
```

---

## 🎨 **INTERFACE WEB**

### **1. Página Output.html - Extensões**
```html
<!-- Modal para configuração de automação -->
<div id="automationModal" class="modal">
    <div class="modal-content">
        <h3>⚙️ Configurar Automação - Output {N}</h3>
        
        <!-- Seletor de modo -->
        <select id="automationMode">
            <option value="0">Manual</option>
            <option value="1">Aquecedor</option>
            <option value="2">Resfriador</option>
            <option value="3">Setpoint + Histerese</option>
            <option value="5">Agendamento</option>
            <option value="6">Timer</option>
            <option value="7">Agendamento + Timer</option>
            <option value="8">Nível (2 sensores)</option>
            <option value="9">Nível (1 sensor)</option>
            <option value="10">Sensor Genérico</option>
        </select>
        
        <!-- Painéis de configuração dinâmicos -->
        <div id="configPanels">
            <!-- Painel para cada modo será mostrado dinamicamente -->
        </div>
    </div>
</div>
```

### **2. Sistema de Configuração Intuitiva**
- **Modal único** para todas as configurações
- **Painéis dinâmicos** baseados no modo selecionado
- **Validação em tempo real** dos parâmetros
- **Preview da configuração** antes de salvar

---

## 🗂️ **ESTRUTURAS DE DADOS**

### **1. Enums e Constantes**
```cpp
enum AutomationMode : uint8_t {
    MODE_MANUAL = 0,
    MODE_HEATER = 1,
    MODE_COOLER = 2,
    MODE_SETPOINT_HYSTERESIS = 3,
    MODE_PID = 4,           // Futuro
    MODE_SCHEDULE = 5,
    MODE_TIMER = 6,
    MODE_SCHEDULE_TIMER = 7,
    MODE_LEVEL_DUAL_SENSOR = 8,
    MODE_LEVEL_SINGLE_SENSOR = 9,
    MODE_GENERIC_SENSOR = 10
};

enum class LevelState : uint8_t {
    IDLE = 0,
    WAITING = 1,
    FILLING = 2,
    ERROR_TIMEOUT = 3
};

#define MAX_SCHEDULE_TIMES 4
#define LEVEL_LEARNING_SAMPLES 10
#define AUTOMATION_UPDATE_INTERVAL_MS 1000
```

### **2. Estruturas de Configuração**
```cpp
struct ScheduleTime {
    uint8_t hour;
    uint8_t minute;
    bool enabled;
};

struct ThermostatConfig {
    float threshold;
    bool isCooler;
};

struct SetpointConfig {
    float target;
    float hysteresis;
};

struct ScheduleConfig {
    ScheduleTime onTimes[MAX_SCHEDULE_TIMES];
    ScheduleTime offTimes[MAX_SCHEDULE_TIMES];
};

struct TimerConfig {
    uint32_t duration_ms;
};

struct LevelDualConfig {
    uint8_t pinLow;
    uint8_t pinHigh;
    uint32_t safetyTimeout_ms;
    uint32_t fillTimeout_ms;
    
    // Sistema de aprendizado
    uint32_t fillTimeHistory[LEVEL_LEARNING_SAMPLES];
    uint8_t operationCount;
    bool learningComplete;
    uint32_t averageFillTime_ms;
};

struct LevelSingleConfig {
    uint8_t pinLow;
    uint32_t waitDelay_ms;
    uint32_t fillTime_ms;
    
    // Sistema de aprendizado similar
    uint32_t fillTimeHistory[LEVEL_LEARNING_SAMPLES];
    uint8_t operationCount;
    bool learningComplete;
};

struct GenericSensorConfig {
    float threshold;
    bool triggerOnHigh;
    std::function<float()> readValueFunc;
    char sensorName[16];  // Para identificação
};
```

---

## 📝 **PLANO DE IMPLEMENTAÇÃO**

### **FASE 1: Estrutura Base** (Estimativa: 2-3 dias)
1. ✅ Criar arquivos `automation/Automation.h` e `automation/Automation.cpp`
2. ✅ Implementar `AutomationOutput` classe base
3. ✅ Implementar modos básicos (MANUAL, HEATER, COOLER)
4. ✅ Testes unitários dos modos básicos

### **FASE 2: Integração com Sistema** (Estimativa: 1-2 dias)
1. ✅ Criar `AutomationManager` classe
2. ✅ Modificar `RelayController` para usar `AutomationManager`  
3. ✅ Estender `ConfigManager` com estruturas de automação
4. ✅ Manter compatibilidade com APIs existentes

### **FASE 3: Modos Avançados** (Estimativa: 3-4 dias)
1. ✅ Implementar `MODE_SETPOINT_HYSTERESIS`
2. ✅ Implementar `MODE_TIMER` 
3. ✅ Implementar `MODE_SCHEDULE` (requer integração com NTP)
4. ✅ Implementar `MODE_SCHEDULE_TIMER`
5. ✅ Testes de integração

### **FASE 4: Sistema de Nível** (Estimativa: 2-3 dias)
1. ✅ Implementar `MODE_LEVEL_DUAL_SENSOR`
2. ✅ Implementar sistema de aprendizado
3. ✅ Implementar `MODE_LEVEL_SINGLE_SENSOR`
4. ✅ Implementar reset de aprendizado
5. ✅ Testes com sensores digitais

### **FASE 5: Sensor Genérico** (Estimativa: 1-2 dias)
1. ✅ Implementar `MODE_GENERIC_SENSOR`
2. ✅ Sistema de callbacks para sensores
3. ✅ Integração com `SensorManager`
4. ✅ Testes com diferentes tipos de sensor

### **FASE 6: Interface Web** (Estimativa: 2-3 dias)
1. ✅ Modificar `output.html` com modal de automação
2. ✅ Implementar painéis dinâmicos de configuração
3. ✅ Criar APIs REST para automação
4. ✅ Sistema de configuração de schedule
5. ✅ Validação e preview das configurações

### **FASE 7: Testes e Documentação** (Estimativa: 1-2 dias)
1. ✅ Testes de integração completos
2. ✅ Testes de performance e memória
3. ✅ Documentação de uso
4. ✅ Exemplos de configuração

---

## 💾 **CONSIDERAÇÕES DE MEMÓRIA**

### **RAM Usage (Por Output)**
```cpp
// Cada AutomationOutput consumirá aproximadamente:
sizeof(AutomationOutput) = 
    8 bytes (pin, state, mode, times) +
    60 bytes (union config - maior estrutura) +
    4 bytes (level state) +
    4 bytes (padding)
= ~76 bytes por output

// Para ESP8266 (4 outputs): 76 * 4 = 304 bytes
// Para ESP32 (8 outputs): 76 * 8 = 608 bytes
// Perfeitamente aceitável!
```

### **Flash Usage**
- Estimativa: +15-20KB de código adicional
- Bem dentro dos limites do ESP8266/ESP32

---

## 🧪 **TESTES PLANEJADOS**

### **1. Testes Unitários**
- ✅ Cada modo de automação isoladamente
- ✅ Transições de estado
- ✅ Timers e timeouts
- ✅ Sistema de aprendizado

### **2. Testes de Integração**
- ✅ Integração com RelayController
- ✅ Integração com ConfigManager
- ✅ Integração com SensorManager
- ✅ APIs REST

### **3. Testes de Performance**
- ✅ Uso de CPU com 8 outputs ativas
- ✅ Uso de RAM em diferentes configurações
- ✅ Latência de resposta
- ✅ Estabilidade de longo prazo

---

## 🚀 **BENEFÍCIOS ESPERADOS**

### **Para Desenvolvedores:**
- ✅ **Código modular** e reutilizável
- ✅ **Fácil manutenção** e extensão
- ✅ **Testes isolados** de cada funcionalidade
- ✅ **API limpa** e intuitiva

### **Para Usuários:**
- ✅ **10 modos de automação** poderosos
- ✅ **Configuração intuitiva** via interface web
- ✅ **Sistema de aprendizado** automático
- ✅ **Flexibilidade total** de configuração

### **Para o Sistema:**
- ✅ **Performance otimizada** (update 1x/segundo)
- ✅ **Uso eficiente de RAM** (union structures)
- ✅ **Compatibilidade mantida** com código existente
- ✅ **Escalabilidade** para futuras expansões

---

## ❓ **PONTOS PARA APROVAÇÃO**

### **1. Arquitetura Geral**
- ✅ Separação em `automation/` está adequada?
- ✅ `AutomationManager` + `AutomationOutput` faz sentido?
- ✅ Integração com `RelayController` preserva compatibilidade?

### **2. Estruturas de Dados**
- ✅ Union para economizar RAM é boa estratégia?
- ✅ Enums e constantes estão bem definidos?
- ✅ Sistema de aprendizado está bem modelado?

### **3. Interface de Usuário**
- ✅ Modal único vs páginas separadas para configuração?
- ✅ Painéis dinâmicos baseados no modo selecionado?
- ✅ APIs REST propostas estão adequadas?

### **4. Performance e Memória**
- ✅ Update de 1 segundo é adequado para todos os modos?
- ✅ Uso de RAM estimado está aceitável?
- ✅ `std::function` no ESP8266 é problemático?

### **5. Implementação**
- ✅ Fases propostas estão bem divididas?
- ✅ Estimativas de tempo são realistas?
- ✅ Testes planejados são suficientes?

---

## 🎯 **PRÓXIMOS PASSOS (Após Aprovação)**

1. **Criar branch `feature/automation-system`**
2. **Implementar FASE 1** (estrutura base)
3. **Fazer commit granular** de cada funcionalidade
4. **Testes contínuos** a cada fase
5. **Documentação paralela** ao desenvolvimento

---

**⚠️ IMPORTANTE**: Este roadmap **NÃO ALTERA NADA** no código atual até receber sua aprovação explícita. Todas as modificações serão feitas em branch separada para preservar a estabilidade do sistema atual.

**🚀 READY FOR APPROVAL!** 

Que partes você gostaria de modificar ou tem dúvidas antes de começarmos a implementação? 

---

## 💾 **PERSISTÊNCIA DE DADOS - LittleFS/SPIFFS**

### **1. Estrutura de Arquivos de Configuração**
```
/automation/
├── config.json              # Configurações gerais de automação
├── outputs/
│   ├── output_0.json         # Configuração específica da saída 0
│   ├── output_1.json         # Configuração específica da saída 1
│   ├── output_2.json         # Configuração específica da saída 2
│   └── output_3.json         # Configuração específica da saída 3
├── schedules/
│   ├── schedule_0.json       # Agendamentos da saída 0
│   ├── schedule_1.json       # Agendamentos da saída 1
│   └── ...
└── learning/
    ├── level_learning_0.json # Dados de aprendizado da saída 0
    ├── level_learning_1.json # Dados de aprendizado da saída 1
    └── ...
```

### **2. Formatos JSON de Persistência**

#### **Arquivo Principal: `/automation/config.json`**
```json
{
  "automation": {
    "version": "1.0.0",
    "enabled": true,
    "updateInterval": 1000,
    "debug": false,
    "outputs": {
      "count": 4,
      "configPath": "/automation/outputs/"
    },
    "lastSaved": "2025-01-20T10:30:00Z"
  }
}
```

#### **Configuração por Saída: `/automation/outputs/output_N.json`**
```json
{
  "output": {
    "index": 0,
    "name": "Output 1",
    "pin": 5,
    "enabled": true,
    "mode": "MODE_SETPOINT_HYSTERESIS",
    "currentState": false,
    "config": {
      "thermostat": {
        "threshold": 25.0,
        "isCooler": false
      },
      "setpoint": {
        "target": 25.0,
        "hysteresis": 0.5
      },
      "schedule": {
        "onTimes": [
          {"hour": 8, "minute": 0, "enabled": true},
          {"hour": 14, "minute": 0, "enabled": true},
          {"hour": 0, "minute": 0, "enabled": false},
          {"hour": 0, "minute": 0, "enabled": false}
        ],
        "offTimes": [
          {"hour": 22, "minute": 0, "enabled": true},
          {"hour": 2, "minute": 0, "enabled": true},
          {"hour": 0, "minute": 0, "enabled": false},
          {"hour": 0, "minute": 0, "enabled": false}
        ]
      },
      "timer": {
        "duration_ms": 300000
      },
      "levelDual": {
        "pinLow": 12,
        "pinHigh": 13,
        "safetyTimeout_ms": 600000,
        "fillTimeout_ms": 300000
      },
      "levelSingle": {
        "pinLow": 12,
        "waitDelay_ms": 5000,
        "fillTime_ms": 120000
      },
      "generic": {
        "threshold": 7.5,
        "triggerOnHigh": false,
        "sensorName": "pH_sensor",
        "sensorType": "pH"
      }
    },
    "lastUpdated": "2025-01-20T10:30:00Z"
  }
}
```

#### **Dados de Aprendizado: `/automation/learning/level_learning_N.json`**
```json
{
  "levelLearning": {
    "outputIndex": 0,
    "mode": "MODE_LEVEL_DUAL_SENSOR",
    "fillTimeHistory": [
      125000, 128000, 122000, 130000, 127000,
      124000, 129000, 126000, 123000, 131000
    ],
    "operationCount": 10,
    "learningComplete": true,
    "averageFillTime_ms": 126500,
    "adjustedTimeout_ms": 151800,  // média + 20%
    "statistics": {
      "minFillTime": 122000,
      "maxFillTime": 131000,
      "standardDeviation": 3162.2,
      "confidenceLevel": 0.95
    },
    "lastOperation": "2025-01-20T10:25:00Z",
    "createdAt": "2025-01-15T14:20:00Z"
  }
}
```

### **3. Classes de Persistência**

#### **AutomationPersistence** (Nova classe)
```cpp
class AutomationPersistence {
private:
    static const char* CONFIG_DIR;
    static const char* OUTPUT_DIR;
    static const char* SCHEDULE_DIR;
    static const char* LEARNING_DIR;
    
public:
    // Inicialização
    static bool begin();
    static bool createDirectories();
    
    // Configuração geral
    static bool saveGeneralConfig(const AutomationGeneralConfig& config);
    static bool loadGeneralConfig(AutomationGeneralConfig& config);
    
    // Configuração por saída
    static bool saveOutputConfig(int index, const AutomationOutput& output);
    static bool loadOutputConfig(int index, AutomationOutput& output);
    
    // Dados de aprendizado
    static bool saveLearningData(int index, const LevelLearningData& data);
    static bool loadLearningData(int index, LevelLearningData& data);
    
    // Backup e restauração
    static bool createBackup(const String& backupName);
    static bool restoreBackup(const String& backupName);
    
    // Utilitários
    static bool fileExists(const String& path);
    static String generateTimestamp();
    static bool validateJsonStructure(const String& json, const String& schema);
};
```

#### **Integração com AutomationManager**
```cpp
class AutomationManager {
private:
    AutomationPersistence _persistence;
    unsigned long _lastSaveTime;
    bool _needsSave;
    
public:
    // Persistência automática
    void enableAutosave(bool enabled, uint32_t interval_ms = 30000);
    void markForSave();
    void forceSave();
    
    // Carregamento inicial
    bool loadAllConfigurations();
    bool saveAllConfigurations();
    
    // Backup automático
    void createDailyBackup();
    
private:
    void _autoSaveTask();
    void _validateAndRepairConfigs();
};
```

### **4. Sistema de Backup e Segurança**

#### **Backup Automático**
```cpp
// Sistema de backup incremental
struct BackupManager {
    static bool createDailyBackup();
    static bool createConfigSnapshot();
    static bool validateBackupIntegrity(const String& backupPath);
    
    // Estrutura de backups
    // /backups/
    // ├── daily/
    // │   ├── 2025-01-20_automation_backup.json
    // │   └── 2025-01-19_automation_backup.json
    // └── manual/
    //     └── user_backup_20250120_103000.json
};
```

#### **Validação de Integridade**
```cpp
class ConfigValidator {
public:
    static bool validateOutputConfig(const String& json);
    static bool validateLearningData(const String& json);
    static bool repairCorruptedConfig(int outputIndex);
    static bool resetToDefaults(int outputIndex);
    
private:
    static bool _checkJsonSchema(const String& json, const String& expectedSchema);
    static bool _validateNumericRanges(const JsonDocument& doc);
    static bool _validatePinAssignments(const JsonDocument& doc);
};
```

### **5. Performance e Otimização**

#### **Salvamento Inteligente**
```cpp
// Sistema que evita salvamentos desnecessários
class SmartSave {
private:
    uint32_t _configHashes[MAX_OUTPUTS];  // Hash das configurações
    unsigned long _lastSaveTime[MAX_OUTPUTS];
    
public:
    bool needsSave(int outputIndex, const AutomationOutput& output);
    void markSaved(int outputIndex, const AutomationOutput& output);
    uint32_t calculateConfigHash(const AutomationOutput& output);
};
```

#### **Cache de Configuração**
```cpp
// Cache em RAM para configurações frequentemente acessadas
class ConfigCache {
private:
    AutomationConfig _cache[MAX_OUTPUTS];
    bool _cacheValid[MAX_OUTPUTS];
    unsigned long _cacheTime[MAX_OUTPUTS];
    
public:
    bool getCachedConfig(int index, AutomationConfig& config);
    void setCachedConfig(int index, const AutomationConfig& config);
    void invalidateCache(int index = -1);  // -1 = invalidar tudo
    
private:
    static const uint32_t CACHE_EXPIRE_MS = 300000;  // 5 minutos
};
```

### **6. Migração e Compatibilidade**

#### **Sistema de Versioning**
```cpp
class ConfigMigration {
public:
    static bool migrateFromVersion(const String& fromVersion, const String& toVersion);
    static bool migrateOldRelayConfig();  // Migrar do sistema antigo
    static String getCurrentVersion();
    
private:
    static bool _migrateV1_0_to_V1_1();
    static bool _migrateV1_1_to_V1_2();
    // ... outras migrações futuras
};
```

#### **Compatibilidade com Sistema Atual**
```cpp
// Migração do RelayConfig atual para AutomationConfig
class LegacyMigration {
public:
    static bool migrateLegacyOutputs(const RelayConfig& oldConfig);
    static bool createAutomationFromLegacy(int outputIndex, const OutputConfig& oldOutput);
    
private:
    static AutomationMode _mapLegacyModeToAutomation(bool autoMode, const OutputConfig& config);
    static bool _migrateLegacySchedule(const OutputConfig& oldOutput, ScheduleConfig& newSchedule);
};
```

### **7. Implementação Detalhada**

#### **FASE 2.5: Sistema de Persistência** (Estimativa: 2-3 dias)
1. ✅ Implementar `AutomationPersistence` classe base
2. ✅ Criar sistema de diretórios no LittleFS/SPIFFS
3. ✅ Implementar serialização/deserialização JSON
4. ✅ Sistema de validação de configurações
5. ✅ Integração com `AutomationManager`

#### **FASE 2.6: Backup e Segurança** (Estimativa: 1-2 dias)
1. ✅ Sistema de backup automático
2. ✅ Validação de integridade
3. ✅ Recovery de configurações corrompidas
4. ✅ Migração de configurações legacy

#### **FASE 2.7: Otimização** (Estimativa: 1 dia)
1. ✅ Sistema de cache inteligente
2. ✅ Salvamento otimizado (apenas quando necessário)
3. ✅ Compressão de dados de aprendizado
4. ✅ Limpeza automática de backups antigos

### **8. Estrutura de Dados Persistentes**

#### **Configuração Geral**
```cpp
struct AutomationGeneralConfig {
    char version[16] = "1.0.0";
    bool enabled = true;
    uint32_t updateInterval = 1000;
    bool debugMode = false;
    uint8_t outputCount = 4;
    char lastSaved[32];
    
    // Configurações globais de backup
    bool autoBackup = true;
    uint32_t backupInterval = 86400000;  // 24 horas
    uint8_t maxBackups = 7;              // 7 dias de backup
    
    // Convenção de nomes padrão
    char defaultOutputNames[MAX_OUTPUTS][32] = {
        "Output 1", "Output 2", "Output 3", "Output 4",
        "Output 5", "Output 6", "Output 7", "Output 8"  // Para ESP32 Pro
    };
};
```

#### **Dados de Aprendizado Estendidos**
```cpp
struct LevelLearningData {
    uint8_t outputIndex;
    AutomationMode mode;
    
    // Histórico de operações
    uint32_t fillTimeHistory[LEVEL_LEARNING_SAMPLES];
    uint8_t operationCount;
    bool learningComplete;
    
    // Estatísticas calculadas
    uint32_t averageFillTime_ms;
    uint32_t adjustedTimeout_ms;    // média + 20%
    uint32_t minFillTime;
    uint32_t maxFillTime;
    float standardDeviation;
    
    // Metadados
    char lastOperation[32];
    char createdAt[32];
    uint32_t totalOperations;       // Total desde o início
    uint32_t successfulOperations;  // Operações bem-sucedidas
    float successRate;              // Taxa de sucesso
};
```

### **9. APIs de Persistência**

#### **Endpoints REST para Backup**
```cpp
// Novos endpoints para gerenciamento de configurações
GET  /api/automation/config/export        // Exportar todas as configurações
POST /api/automation/config/import        // Importar configurações
GET  /api/automation/backup/list          // Listar backups disponíveis
POST /api/automation/backup/create        // Criar backup manual
POST /api/automation/backup/restore/{id}  // Restaurar backup específico
DELETE /api/automation/backup/{id}        // Deletar backup
GET  /api/automation/learning/export/{id} // Exportar dados de aprendizado
POST /api/automation/learning/reset/{id}  // Reset dados de aprendizado
```

#### **Interface Web para Backup**
```html
<!-- Nova seção na página de configuração -->
<div class="config-section">
    <div class="section-title">💾 Backup e Configurações</div>
    
    <div class="backup-controls">
        <button id="createBackup" class="btn btn-success">
            📦 Criar Backup Manual
        </button>
        <button id="exportConfig" class="btn btn-primary">
            📤 Exportar Configurações
        </button>
        <input type="file" id="importConfig" accept=".json" style="display:none">
        <button id="importConfigBtn" class="btn btn-warning">
            📥 Importar Configurações
        </button>
    </div>
    
    <div class="backup-list">
        <h4>Backups Disponíveis:</h4>
        <div id="backupsList">
            <!-- Lista dinâmica de backups -->
        </div>
    </div>
</div>
```

### **10. Considerações de Implementação**

#### **Uso de Memória**
```cpp
// Estimativa de uso do sistema de persistência:
// - AutomationPersistence: ~200 bytes (estático)
// - ConfigCache (4 outputs): ~300 bytes
// - BackupManager: ~100 bytes
// - Total adicional: ~600 bytes
// 
// Arquivos no filesystem:
// - config.json: ~500 bytes
// - output_N.json (4 outputs): ~800 bytes cada = 3.2KB
// - learning_N.json (4 outputs): ~400 bytes cada = 1.6KB
// - Total usado: ~5.3KB (muito aceitável)
```

#### **Performance de I/O**
```cpp
// Otimizações implementadas:
// 1. Cache em RAM das configurações ativas
// 2. Salvamento apenas quando há mudanças (hash comparison)
// 3. Salvamento assíncrono para não bloquear o loop principal
// 4. Compactação JSON para reduzir tamanho dos arquivos
// 5. Validação rápida de integridade antes de salvar
```

---

## 💾 **CONSIDERAÇÕES DE MEMÓRIA** (Atualizado)

### **RAM Usage (Por Output) - Com Persistência**
```cpp
// Cada AutomationOutput + Persistência:
sizeof(AutomationOutput) = 76 bytes (anterior)
sizeof(ConfigCache entry) = 75 bytes
sizeof(LearningData cache) = 60 bytes
sizeof(SmartSave tracking) = 12 bytes
= ~223 bytes por output

// Para ESP8266 (4 outputs): 223 * 4 = 892 bytes
// Para ESP32 (8 outputs): 223 * 8 = 1.784KB
// Ainda muito aceitável!
```

### **Flash Usage - Com Persistência**
- Sistema de persistência: +8-10KB de código
- Configurações salvas: ~5-8KB de dados
- Backups (7 dias): ~35-50KB máximo
- **Total adicional**: ~50-70KB (perfeitamente viável)

---

**✅ PERSISTÊNCIA COMPLETA INTEGRADA!**

Agora todas as configurações de automação serão:
- ✅ **Salvas automaticamente** no LittleFS/SPIFFS
- ✅ **Restauradas** na inicialização
- ✅ **Validadas** contra corrupção
- ✅ **Backup automático** diário
- ✅ **Migração** de configurações antigas
- ✅ **Exportação/Importação** via interface web

O sistema de aprendizado de nível com **média + 20%** e o suporte estendido a **sensores DS18B20, DHT11, DHT22** também está contemplado!

Quer que eu ajuste mais algum detalhe antes da aprovação final? 