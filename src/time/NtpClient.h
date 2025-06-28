#ifndef NTP_CLIENT_MANAGER_H
#define NTP_CLIENT_MANAGER_H

#include <Arduino.h>
#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <WiFiUdp.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <WiFiUdp.h>
#endif
#include <time.h>
#include <NTPClient.h>

class NtpClientManager {
private:
  WiFiUDP ntpUDP;
  NTPClient* timeClient;
  unsigned long lastSyncAttempt;
  unsigned long lastSuccessfulSync;
  bool ntpSynchronized;
  bool ntpEnabled;
  String primaryServer;
  String secondaryServer;
  String tertiaryServer;
  int syncInterval;
  int timezoneOffset;  // Offset UTC em horas
  
public:
  NtpClientManager() : 
    timeClient(nullptr),
    lastSyncAttempt(0),
    lastSuccessfulSync(0),
    ntpSynchronized(false),
    ntpEnabled(true),
    primaryServer("pool.ntp.org"),
    secondaryServer("time.google.com"),
    tertiaryServer("br.pool.ntp.org"),
    syncInterval(3600), // 1 hora padrão
    timezoneOffset(0)   // UTC padrão
  {}
  
  ~NtpClientManager() {
    if (timeClient) {
      delete timeClient;
    }
  }
  
  bool begin(const char* server1 = nullptr, const char* server2 = nullptr, const char* server3 = nullptr, int interval = 3600, int tzOffset = 0) {
    if (server1) primaryServer = String(server1);
    if (server2) secondaryServer = String(server2);
    if (server3) tertiaryServer = String(server3);
    syncInterval = interval;
    timezoneOffset = tzOffset;
    
    if (timeClient) delete timeClient;
    timeClient = new NTPClient(ntpUDP, primaryServer.c_str(), timezoneOffset * 3600, syncInterval * 1000);
    timeClient->begin();
    
    forceUpdate();
    return true;
  }
  
  void update() {
    if (!ntpEnabled || !timeClient) return;
    
    unsigned long now = millis();
    if (now - lastSyncAttempt > (syncInterval * 1000)) {
      attemptSync();
    }
    
    timeClient->update();
  }
  
  bool forceUpdate() {
    return attemptSync();
  }
  
  bool attemptSync() {
    if (!ntpEnabled || !timeClient) return false;
    
    lastSyncAttempt = millis();
    
    bool success = timeClient->forceUpdate();
    
    if (success) {
      time_t now = timeClient->getEpochTime();
      
      if (now > 1000000000) {
        ntpSynchronized = true;
        lastSuccessfulSync = millis();
        
        struct timeval tv;
        tv.tv_sec = now;
        tv.tv_usec = 0;
        settimeofday(&tv, NULL);
        
        return true;
      }
    }
    
    // Tentar servidor secundário
    delete timeClient;
    timeClient = new NTPClient(ntpUDP, secondaryServer.c_str(), timezoneOffset * 3600, syncInterval * 1000);
    timeClient->begin();
    
    success = timeClient->forceUpdate();
    if (success) {
      time_t now = timeClient->getEpochTime();
      
      if (now > 1000000000) {
        ntpSynchronized = true;
        lastSuccessfulSync = millis();
        
        struct timeval tv;
        tv.tv_sec = now;
        tv.tv_usec = 0;
        settimeofday(&tv, NULL);
        
        return true;
      }
    }
    
    ntpSynchronized = false;
    return false;
  }
  
  bool isTimeSet() const {
    time_t now;
    time(&now);
    return (now > 1000000000 && ntpSynchronized);
  }
  
  bool isSynchronized() const {
    return ntpSynchronized && isTimeSet();
  }
  
  unsigned long getLastSyncTime() const {
    return lastSuccessfulSync;
  }
  
  unsigned long getUptime() const {
    return millis() / 1000;
  }
  
  String getFormattedTime() const {
    if (!timeClient || !ntpSynchronized) {
      return "Não sincronizado";
    }
    
    time_t currentTime = getCurrentTime();
    if (currentTime > 1000000000) {
      struct tm *timeInfo = localtime(&currentTime);
      char buffer[64];
      strftime(buffer, sizeof(buffer), "%d/%m/%Y, %H:%M:%S", timeInfo);
      return String(buffer);
    }
    
    return timeClient->getFormattedTime();
  }
  
  String getStatus() const {
    if (!ntpEnabled) return "Desabilitado";
    if (!ntpSynchronized) return "Não sincronizado";
    if (!isTimeSet()) return "Tempo inválido";
    return "Sincronizado";
  }
  
  void setServers(const String& server1, const String& server2, const String& server3) {
    primaryServer = server1;
    secondaryServer = server2;
    tertiaryServer = server3;
    
    if (timeClient) {
      delete timeClient;
      timeClient = new NTPClient(ntpUDP, primaryServer.c_str(), timezoneOffset * 3600, syncInterval * 1000);
      timeClient->begin();
    }
  }
  
  void setSyncInterval(int interval) {
    syncInterval = interval;
    if (timeClient) {
      timeClient->setUpdateInterval(interval * 1000);
    }
  }
  
  void setEnabled(bool enabled) {
    ntpEnabled = enabled;
  }
  
  void setTimezone(int tzOffset) {
    timezoneOffset = tzOffset;
    if (timeClient) {
      delete timeClient;
      timeClient = new NTPClient(ntpUDP, primaryServer.c_str(), timezoneOffset * 3600, syncInterval * 1000);
      timeClient->begin();
    }
  }
  
  time_t getCurrentTime() const {
    if (timeClient && ntpSynchronized) {
      time_t ntpTime = timeClient->getEpochTime();
      if (ntpTime > 1000000000) {
        return ntpTime;
      }
    }
    
    time_t now;
    time(&now);
    return now;
  }
  
  String getLastSyncFormatted() const {
    if (lastSuccessfulSync == 0) return "Nunca sincronizado";
    
    unsigned long secondsAgo = (millis() - lastSuccessfulSync) / 1000;
    if (secondsAgo < 60) return String(secondsAgo) + "s atrás";
    if (secondsAgo < 3600) return String(secondsAgo / 60) + "min atrás";
    return String(secondsAgo / 3600) + "h atrás";
  }
};

#endif 