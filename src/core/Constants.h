#ifndef CONSTANTS_H
#define CONSTANTS_H

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