#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Constants.h"

struct LogEntry {
    unsigned long timestamp;
    String message;
    LogType type;
};

class LogManager {
private:
    static LogEntry _logBuffer[MAX_LOG_ENTRIES];
    static int _logCount;
    static int _logIndex;
    
public:
    static void log(const String& message, LogType type = LOG_INFO) {
        LogEntry entry;
        entry.timestamp = millis();
        entry.message = message;
        entry.type = type;
        
        _logBuffer[_logIndex] = entry;
        _logIndex = (_logIndex + 1) % MAX_LOG_ENTRIES;
        if (_logCount < MAX_LOG_ENTRIES) _logCount++;
    }
    
    static String getLogsJson() {
        StaticJsonDocument<2048> doc;
        JsonArray logs = doc.createNestedArray("logs");
        
        int start = (_logIndex - _logCount + MAX_LOG_ENTRIES) % MAX_LOG_ENTRIES;
        for (int i = 0; i < _logCount; i++) {
            int idx = (start + i) % MAX_LOG_ENTRIES;
            JsonObject log = logs.createNestedObject();
            log["timestamp"] = _logBuffer[idx].timestamp;
            log["message"] = _logBuffer[idx].message;
            log["type"] = (int)_logBuffer[idx].type;
        }
        
        String output;
        serializeJson(doc, output);
        return output;
    }
    
    static void clear() {
        _logCount = 0;
        _logIndex = 0;
    }
};

// Inicialização das variáveis estáticas
LogEntry LogManager::_logBuffer[MAX_LOG_ENTRIES];
int LogManager::_logCount = 0;
int LogManager::_logIndex = 0;

#endif // LOG_MANAGER_H 