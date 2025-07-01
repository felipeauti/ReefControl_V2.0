# APIs do ReefControl

Este documento contém a documentação completa de todas as APIs disponíveis no sistema ReefControl.

## 1. APIs de Sensores e Status

### 1.1 Status dos Sensores
```http
GET /api/sensors
```
Retorna os dados atuais de todos os sensores do sistema.

**Resposta**
```json
{
    "temp": 25.5,
    "ph": 7.2,
    "tds": 450,
    "level": 100,
    "version": "3.0.2"
}
```

### 1.2 Status do Sistema
```http
GET /api/status
```
Retorna informações gerais sobre o estado do sistema.

**Resposta**
```json
{
    "version": "v2.0.12",
    "build_date": "Jan 1 2024 12:00:00",
    "wifi": true,
    "mqtt": false,
    "uptime": 3600000,
    "heap": 50000,
    "flash_size": 4194304,
    "chip_id": "ABC123",
    "sdk_version": "2.2.1"
}
```

### 1.3 Capabilities do Sistema
```http
GET /api/capabilities
```
Retorna as capacidades e recursos disponíveis no sistema.

**Resposta**
```json
{
    "version": "3.0.2",
    "hardware": "ESP32",
    "maxOutputs": 8,
    "maxSensors": 4,
    "hasAdvancedAutomation": true,
    "hasAnalytics": true,
    "hasMQTTAdvanced": true,
    "hasMultiSensors": true,
    "hasGraphics": true,
    "ramAvailable": 320000,
    "flashAvailable": 4194304,
    "isCompact": false,
    "isPro": true
}
```

## 2. APIs de Controle de Saídas

### 2.1 Status das Saídas
```http
GET /api/outputs
```
Retorna o estado atual de todas as saídas/relés.

**Resposta**
```json
{
    "output1": {
        "state": true,
        "pin": 5,
        "name": "Bomba"
    },
    "output2": {
        "state": false,
        "pin": 4,
        "name": "Termostato"
    },
    "output3": {
        "state": true,
        "pin": 14,
        "name": "Chiller"
    },
    "output4": {
        "state": false,
        "pin": 12,
        "name": "Skimmer"
    }
}
```

### 2.2 Controle de Saídas
```http
POST /api/outputs
```
Controla o estado das saídas individuais.

**Corpo da Requisição**
```json
{
    "relay": "output1",
    "state": true
}
```

**Resposta**
```json
{
    "success": true
}
```

### 2.3 Configuração das Saídas
```http
GET /api/outputs/config
```
Retorna as configurações atuais das saídas.

```http
POST /api/outputs/config
```
Salva novas configurações para as saídas.

## 3. APIs de NTP (Sincronização de Tempo)

### 3.1 Configuração NTP
```http
GET /api/ntp/config
```
Retorna as configurações atuais do NTP.

**Resposta**
```json
{
    "enabled": true,
    "server1": "pool.ntp.org",
    "server2": "time.google.com",
    "server3": "time.cloudflare.com",
    "timezone": -3,
    "timezoneString": "America/Sao_Paulo",
    "syncInterval": 3600
}
```

```http
POST /api/ntp/config
```
Atualiza as configurações do NTP.

### 3.2 Status NTP
```http
GET /api/ntp/status
```
Retorna o status atual da sincronização NTP.

**Resposta**
```json
{
    "synchronized": true,
    "timestamp": 1710123456,
    "lastSync": "2024-03-11 10:30:45",
    "currentTime": 1710123456,
    "status": "synchronized",
    "uptime": 3600
}
```

### 3.3 Forçar Sincronização
```http
POST /api/ntp/sync
```
Força uma sincronização imediata com os servidores NTP.

**Resposta**
```json
{
    "success": true,
    "message": "Sincronização NTP bem-sucedida",
    "timestamp": 1710123456
}
```

## 4. APIs de Sistema e Arquivos

### 4.1 Upload de Arquivos
```http
POST /api/upload
```
Permite o upload de arquivos para o sistema.

**Tipos de Arquivo Suportados**
- .html
- .css
- .js
- .json
- .png
- .jpg
- .gif
- .ico

### 4.2 Informações do Sistema
```http
GET /api/system/info
```
Retorna informações sobre o sistema de arquivos.

**Resposta**
```json
{
    "totalBytes": 4194304,
    "usedBytes": 1048576,
    "freeSpace": 3145728
}
```

## Notas Importantes

1. Todas as APIs retornam respostas em formato JSON
2. Em caso de erro, as APIs retornam um código de status HTTP apropriado e um objeto JSON com a propriedade "error"
3. As APIs de controle (POST) requerem que o sistema esteja inicializado corretamente
4. O sistema possui logging detalhado para diagnóstico de problemas
5. A sincronização NTP é crítica para o funcionamento correto do sistema de automação temporal 