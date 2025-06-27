#ifndef CAPABILITIES_H
#define CAPABILITIES_H

// =============================================================================
// SISTEMA DE CAPABILITIES - ReefControl v2.1.0+
// =============================================================================

struct SystemCapabilities {
    const char* version;
    const char* hardware;
    int maxOutputs;
    int maxSensors;
    bool hasAdvancedAutomation;
    bool hasAnalytics;
    bool hasMQTTAdvanced;
    bool hasMultiSensors;
    bool hasGraphics;
    int ramAvailable;
    int flashAvailable;
};

// =============================================================================
// CONFIGURAÇÕES POR VERSÃO
// =============================================================================

#ifdef COMPACT_VERSION
    constexpr SystemCapabilities CAPS = {
        .version = VERSION_STRING,
        .hardware = "ESP8266",
        .maxOutputs = MAX_OUTPUTS,
        .maxSensors = MAX_SENSORS,
        .hasAdvancedAutomation = false,
        .hasAnalytics = false,
        .hasMQTTAdvanced = false,
        .hasMultiSensors = false,
        .hasGraphics = false,
        .ramAvailable = 80000,    // ~80KB ESP8266
        .flashAvailable = 1048576 // ~1MB Flash
    };
    
    // Configurações específicas Compact
    #define ENABLE_BASIC_MQTT
    #define ENABLE_SIMPLE_AUTOMATION
    #define COMPACT_INTERFACE
    
#elif defined(PRO_VERSION)
    constexpr SystemCapabilities CAPS = {
        .version = VERSION_STRING,
        .hardware = "ESP32",
        .maxOutputs = MAX_OUTPUTS,
        .maxSensors = MAX_SENSORS,
        .hasAdvancedAutomation = true,
        .hasAnalytics = true,
        .hasMQTTAdvanced = true,
        .hasMultiSensors = true,
        .hasGraphics = true,
        .ramAvailable = 520000,   // ~520KB ESP32
        .flashAvailable = 4194304 // ~4MB Flash
    };
    
    // Configurações específicas Pro
    #define ENABLE_FULL_MQTT
    #define ENABLE_ADVANCED_AUTOMATION
    #define ENABLE_ANALYTICS
    #define ENABLE_GRAPHICS
    #define PRO_INTERFACE
    
#else
    // Fallback para versões legacy (v2.0.12 e anteriores)
    constexpr SystemCapabilities CAPS = {
        .version = "ReefControl v2.0.12",
        .hardware = "ESP8266",
        .maxOutputs = 4,
        .maxSensors = 2,
        .hasAdvancedAutomation = false,
        .hasAnalytics = false,
        .hasMQTTAdvanced = false,
        .hasMultiSensors = false,
        .hasGraphics = false,
        .ramAvailable = 80000,
        .flashAvailable = 1048576
    };
    
    // Configurações legacy
    #define ENABLE_BASIC_MQTT
    #define COMPACT_INTERFACE
    
#endif

// =============================================================================
// FUNÇÕES AUXILIARES
// =============================================================================

inline bool isCompactVersion() {
    #ifdef COMPACT_VERSION
        return true;
    #else
        return false;
    #endif
}

inline bool isProVersion() {
    #ifdef PRO_VERSION
        return true;
    #else
        return false;
    #endif
}

inline const char* getVersionString() {
    return CAPS.version;
}

inline const char* getHardwarePlatform() {
    return CAPS.hardware;
}

inline int getMaxOutputs() {
    return CAPS.maxOutputs;
}

inline int getMaxSensors() {
    return CAPS.maxSensors;
}

// =============================================================================
// DEBUG E INFORMAÇÕES
// =============================================================================

inline void printCapabilities() {
    Serial.println("=== SYSTEM CAPABILITIES ===");
    Serial.printf("Version: %s\n", CAPS.version);
    Serial.printf("Hardware: %s\n", CAPS.hardware);
    Serial.printf("Max Outputs: %d\n", CAPS.maxOutputs);
    Serial.printf("Max Sensors: %d\n", CAPS.maxSensors);
    Serial.printf("Advanced Automation: %s\n", CAPS.hasAdvancedAutomation ? "YES" : "NO");
    Serial.printf("Analytics: %s\n", CAPS.hasAnalytics ? "YES" : "NO");
    Serial.printf("MQTT Advanced: %s\n", CAPS.hasMQTTAdvanced ? "YES" : "NO");
    Serial.printf("Multi Sensors: %s\n", CAPS.hasMultiSensors ? "YES" : "NO");
    Serial.printf("Graphics: %s\n", CAPS.hasGraphics ? "YES" : "NO");
    Serial.printf("RAM Available: %d bytes\n", CAPS.ramAvailable);
    Serial.printf("Flash Available: %d bytes\n", CAPS.flashAvailable);
    Serial.println("============================");
}

#endif // CAPABILITIES_H 