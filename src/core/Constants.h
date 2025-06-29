#ifndef CONSTANTS_H
#define CONSTANTS_H

// Definições de Hardware
#ifdef COMPACT_VERSION
    #define ONE_WIRE_BUS 4  // D2 no Wemos D1 Mini (GPIO4)
#else
    #define ONE_WIRE_BUS 4  // GPIO4 no ESP32
#endif

// Configurações de Debug
#ifdef DEBUG_ONEWIRE
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
#endif

// Configurações de Sensores
#define MAX_TEMP_SENSORS 8
#define TEMP_READ_INTERVAL 5000  // 5 segundos
#define TEMP_PRECISION 12        // Precisão do sensor (9-12 bits)

// Configurações de Rede
#define WIFI_CONNECT_TIMEOUT 180  // 3 minutos
#define MQTT_RECONNECT_DELAY 5000 // 5 segundos

// Configurações de Arquivo
#define CONFIG_FILE "/config.json"
#define SCHEDULE_FILE "/schedule.json"
#define SENSOR_CONFIG_FILE "/sensors.json"

// Configurações de Buffer
#ifndef JSON_BUFFER_SIZE
    #define JSON_BUFFER_SIZE 1024
#endif

// Configurações de Tempo
#ifndef ONEWIRE_INIT_DELAY
    #define ONEWIRE_INIT_DELAY 1000  // 1 segundo
#endif

// Constantes para sensores DS18B20
#define MAX_DS18B20_SENSORS 4  // Máximo de sensores DS18B20 suportados
#define MAX_SENSORS 8  // Máximo total de sensores suportados

// Constantes para sistema de logs
#define MAX_LOG_ENTRIES 100  // Máximo de entradas no buffer de logs
#define MAX_LOG_LENGTH 128   // Tamanho máximo de cada entrada de log

// Tipos de log
enum LogType {
    LOG_INFO,
    LOG_SUCCESS,
    LOG_WARNING,
    LOG_ERROR
};

#endif // CONSTANTS_H 