# Documentação OneWire - ReefControl

## Arquivos do Sistema

### Backend (Core)

1. **src/core/SensorManager.h**
- Definições principais do sistema de sensores
- Estruturas de dados:
  - `DS18B20Sensor`: Estrutura para sensores OneWire
  - `SensorData`: Dados gerais dos sensores
  - `SensorAlerts`: Sistema de alertas
- Constantes e configurações:
  - Pinos para diferentes plataformas (ESP32/ESP8266)
  - Limites e configurações do sistema
- Declaração de métodos para:
  - Leitura de sensores
  - Calibração
  - Persistência
  - Conversão de dados

2. **src/core/SensorManager.cpp**
- Implementação completa do gerenciador de sensores
- Funções principais:
  - `readTemperature()`: Leitura dos sensores DS18B20
  - `addDS18B20Sensor()`: Adiciona novo sensor
  - `removeDS18B20Sensor()`: Remove sensor
  - `calibrateDS18B20Sensor()`: Calibração
  - `saveSensorsToFile()`: Persistência
  - `loadSensorsFromFile()`: Carregamento
- Sistema de logs detalhado
- Tratamento de erros

3. **src/core/WebServer.cpp**
- APIs REST para sensores:
  - `/api/sensors`: Dados gerais
  - `/api/sensors/scan_onewire`: Busca endereços
  - `/api/onewire/scan`: Scan detalhado
  - `/api/ds18b20/sensors`: Lista sensores
  - `/api/ds18b20/add`: Adiciona sensor
  - `/api/ds18b20/remove`: Remove sensor
  - `/api/ds18b20/calibrate`: Calibração
  - `/api/ds18b20/rename`: Renomeia sensor
  - `/api/ds18b20/reset-calibration`: Reset de calibração

### Frontend (HTML/JavaScript)

1. **data/html/home.html**
- Dashboard principal
- Cards de sensores
- Atualização automática
- Integração com API

2. **data/html/sensors.html**
- Interface de gerenciamento
- Listagem de sensores
- Funções de calibração
- Configuração de nomes
- Remoção de sensores

3. **data/html/status.html**
- Monitoramento do sistema
- Status dos sensores
- Indicadores visuais
- Logs do sistema

4. **data/html/test_api.html**
- Página de teste minimalista
- Exibição de temperatura
- Atualização em tempo real

### Configuração (PlatformIO)

**platformio.ini**
```ini
# Bibliotecas OneWire
lib_deps = 
    paulstoffregen/OneWire@^2.3.7
    milesburton/DallasTemperature@^3.11.0

# Flags de compilação
build_flags =
    -DUSE_DS18B20
    -DTEMP_SENSOR_PIN=4
    -DMAX_SENSORS=2  # Compact
    -DMAX_SENSORS=8  # Pro
```

## Páginas com Exibição de Temperatura

### 1. Test API (test_api.html)
- Página minimalista focada em temperatura
- Exibe temperatura em tempo real
- Atualização automática a cada 5 segundos
- Design limpo e direto
- Ideal para testes e debug

### 2. Dashboard Principal (home.html)
- Exibe temperatura junto com outros sensores
- Cards com status visual (ok, warning, error)
- Atualização automática a cada 5 segundos
- Integrado com outros parâmetros (pH, TDS, nível)
- Endpoint: `/api/sensors`

### 3. Configuração de Sensores (sensors.html)
- Interface completa de gerenciamento de sensores DS18B20
- Funcionalidades:
  - Lista todos os sensores OneWire
  - Mostra endereço físico de cada sensor
  - Permite calibração individual
  - Permite renomear sensores
  - Permite remover sensores
  - Configuração de offset
- Endpoints:
  - `/api/ds18b20/sensors` - Lista sensores
  - `/api/ds18b20/add` - Adiciona sensor
  - `/api/ds18b20/remove` - Remove sensor
  - `/api/ds18b20/calibrate` - Calibração
  - `/api/ds18b20/rename` - Renomeia sensor

### 4. Status do Sistema (status.html)
- Página de monitoramento geral
- Seção dedicada para sensores
- Atualização em tempo real
- Indicadores visuais de status
- Integrado com informações do sistema

## Endpoints da API

### Sensores
- `/api/sensors` - Retorna dados de todos os sensores
- `/api/ds18b20/sensors` - Lista sensores DS18B20
- `/api/ds18b20/add` - Adiciona novo sensor
- `/api/ds18b20/remove` - Remove sensor existente
- `/api/ds18b20/calibrate` - Calibra sensor
- `/api/ds18b20/rename` - Renomeia sensor

### Formato de Resposta
```json
{
  "sensors": [
    {
      "name": "Nome do Sensor",
      "address": "28ff51eb61160367",
      "temperature": 19.56,
      "offset": 0.0
    }
  ],
  "version": "ReefControl v3.2.2"
}
```

## Configurações no platformio.ini

### Versão Compact (ESP8266)
```ini
lib_deps = 
    paulstoffregen/OneWire@^2.3.7
    milesburton/DallasTemperature@^3.11.0

build_flags =
    -DUSE_DS18B20
    -DTEMP_SENSOR_PIN=4
    -DMAX_SENSORS=2
```

### Versão Pro (ESP32)
```ini
lib_deps = 
    paulstoffregen/OneWire@^2.3.7
    milesburton/DallasTemperature@^3.11.0

build_flags =
    -DUSE_DS18B20
    -DTEMP_SENSOR_PIN=4
    -DMAX_SENSORS=8
``` 