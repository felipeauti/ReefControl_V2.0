#ifndef CONSTANTS_H
#define CONSTANTS_H

// =============================================================================
// INFORMAÇÕES DE VERSÃO E SISTEMA - ReefControl v3.5.0
// =============================================================================
// Esta é a fonte única da verdade para informações de versão
// Todos os outros arquivos devem referenciar estas constantes

#define REEFCONTROL_VERSION_MAJOR 3
#define REEFCONTROL_VERSION_MINOR 5  
#define REEFCONTROL_VERSION_PATCH 0
#define REEFCONTROL_VERSION_SUFFIX "release"

// Versão formatada como string
#define REEFCONTROL_VERSION_STRING "v3.5.0"

// Versão completa com hardware (definida em tempo de compilação)
#ifdef PRO_VERSION
    #define REEFCONTROL_FULL_VERSION "ReefControl Pro v3.5.0"
    #define REEFCONTROL_HARDWARE "ESP32 Pro"
#elif defined(COMPACT_VERSION)
    #define REEFCONTROL_FULL_VERSION "ReefControl Compact v3.5.0"
    #define REEFCONTROL_HARDWARE "ESP8266 Compact"
#else
    #define REEFCONTROL_FULL_VERSION "ReefControl v3.5.0"
    #define REEFCONTROL_HARDWARE "Unknown"
#endif

// Informações de build
#define REEFCONTROL_BUILD_DATE __DATE__
#define REEFCONTROL_BUILD_TIME __TIME__

// API Version para compatibilidade
#define REEFCONTROL_API_VERSION "2.2.0"

// =============================================================================
// CONSTANTES DE HARDWARE
// =============================================================================

// Constantes para sensores DS18B20
#define MAX_DS18B20_SENSORS 4  // Máximo de sensores DS18B20 suportados
#define MAX_SENSORS 8  // Máximo total de sensores suportados

// Constantes para sistema de logs
#define MAX_LOG_ENTRIES 100  // Máximo de entradas no buffer de logs
#define MAX_LOG_LENGTH 128   // Tamanho máximo de cada entrada de log

// =============================================================================
// FUNÇÕES AUXILIARES DE VERSÃO
// =============================================================================

// Função inline para obter versão como string
inline const char* getReefControlVersion() {
    return REEFCONTROL_VERSION_STRING;
}

// Função inline para obter versão completa
inline const char* getReefControlFullVersion() {
    return REEFCONTROL_FULL_VERSION;
}

// Função inline para obter hardware
inline const char* getReefControlHardware() {
    return REEFCONTROL_HARDWARE;
}

// Função inline para obter API version
inline const char* getReefControlApiVersion() {
    return REEFCONTROL_API_VERSION;
}

// Tipos de log
enum LogType {
    LOG_INFO,
    LOG_SUCCESS,
    LOG_WARNING,
    LOG_ERROR
};

#endif // CONSTANTS_H 