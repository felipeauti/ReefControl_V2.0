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
    syncInterval(3600) // 1 hora padrão
  {}
  
  ~NtpClientManager() {
    if (timeClient) {
      delete timeClient;
    }
  }
  
  bool begin(const char* server1 = nullptr, const char* server2 = nullptr, const char* server3 = nullptr, int interval = 3600) {
    Serial.println("🕐 Inicializando sistema NTP real...");
    
    if (server1) primaryServer = String(server1);
    if (server2) secondaryServer = String(server2);
    if (server3) tertiaryServer = String(server3);
    syncInterval = interval;
    
    // Configurar timezone para Brasil (-3h UTC)
    configTime(-3 * 3600, 0, primaryServer.c_str(), secondaryServer.c_str());
    
    // Criar cliente NTP com servidor primário (UTC, sem offset duplo)
    if (timeClient) delete timeClient;
    timeClient = new NTPClient(ntpUDP, primaryServer.c_str(), 0, syncInterval * 1000);
    
    timeClient->begin();
    
    Serial.println("✅ Sistema NTP inicializado com servidores:");
    Serial.println("  - Primário: " + primaryServer);
    Serial.println("  - Secundário: " + secondaryServer);
    Serial.println("  - Terciário: " + tertiaryServer);
    Serial.println("  - Intervalo: " + String(syncInterval) + "s");
    
    // Tentar primeira sincronização
    forceUpdate();
    
    return true;
  }
  
  void update() {
    if (!ntpEnabled || !timeClient) return;
    
    // Atualizar automaticamente baseado no intervalo
    unsigned long now = millis();
    if (now - lastSyncAttempt > (syncInterval * 1000)) {
      attemptSync();
    }
    
    // Manter cliente atualizado
    timeClient->update();
  }
  
  bool forceUpdate() {
    Serial.println("🔄 Forçando sincronização NTP manual...");
    return attemptSync();
  }
  
  bool attemptSync() {
    if (!ntpEnabled) {
      Serial.println("❌ DEBUG: NTP desabilitado!");
      return false;
    }
    if (!timeClient) {
      Serial.println("❌ DEBUG: timeClient é NULL!");
      return false;
    }
    
    lastSyncAttempt = millis();
    
    Serial.println("🌐 Tentando sincronizar com: " + primaryServer);
    Serial.println("🔍 DEBUG: Chamando timeClient->forceUpdate()...");
    
    bool success = timeClient->forceUpdate();
    Serial.println("🔍 DEBUG: forceUpdate() retornou: " + String(success));
    
    if (success) {
      time_t now = timeClient->getEpochTime();
      Serial.println("🔍 DEBUG: Timestamp obtido do NTP: " + String(now));
      Serial.println("🔍 DEBUG: Data formatada do NTP: " + timeClient->getFormattedTime());
      
      if (now > 1000000000) { // Sanity check
        ntpSynchronized = true;
        lastSuccessfulSync = millis();
        
        // Configurar sistema time
        Serial.println("🔍 DEBUG: ANTES settimeofday - timestamp NTP: " + String(now));
        
        time_t systemTimeBefore;
        time(&systemTimeBefore);
        Serial.println("🔍 DEBUG: ANTES settimeofday - sistema: " + String(systemTimeBefore));
        
        struct timeval tv;
        tv.tv_sec = now;
        tv.tv_usec = 0;
        int result = settimeofday(&tv, NULL);
        Serial.println("🔍 DEBUG: settimeofday retornou: " + String(result));
        
        // Verificar se settimeofday funcionou
        time_t systemTimeAfter;
        time(&systemTimeAfter);
        Serial.println("🔍 DEBUG: DEPOIS settimeofday - sistema: " + String(systemTimeAfter));
        Serial.println("🔍 DEBUG: Diferença: " + String(systemTimeAfter - systemTimeBefore));
        
        Serial.println("✅ Sincronização NTP bem-sucedida!");
        Serial.println("🕐 Hora atual: " + timeClient->getFormattedTime());
        return true;
      } else {
        Serial.println("❌ DEBUG: Timestamp inválido recebido do NTP: " + String(now));
      }
    }
    
    // Tentar servidor secundário
    Serial.println("⚠️ Falha no servidor primário, tentando secundário: " + secondaryServer);
    delete timeClient;
    timeClient = new NTPClient(ntpUDP, secondaryServer.c_str(), 0, syncInterval * 1000);
    timeClient->begin();
    
    success = timeClient->forceUpdate();
    if (success) {
      time_t now = timeClient->getEpochTime();
      Serial.println("🔍 DEBUG: Timestamp do servidor secundário: " + String(now));
      
      if (now > 1000000000) {
        ntpSynchronized = true;
        lastSuccessfulSync = millis();
        
        Serial.println("🔍 DEBUG: ANTES settimeofday SECUNDÁRIO - timestamp: " + String(now));
        
        time_t systemTimeBefore;
        time(&systemTimeBefore);
        Serial.println("🔍 DEBUG: ANTES settimeofday SECUNDÁRIO - sistema: " + String(systemTimeBefore));
        
        struct timeval tv;
        tv.tv_sec = now;
        tv.tv_usec = 0;
        int result = settimeofday(&tv, NULL);
        Serial.println("🔍 DEBUG: settimeofday SECUNDÁRIO retornou: " + String(result));
        
        // Verificar se settimeofday funcionou
        time_t systemTimeAfter;
        time(&systemTimeAfter);
        Serial.println("🔍 DEBUG: DEPOIS settimeofday SECUNDÁRIO - sistema: " + String(systemTimeAfter));
        Serial.println("🔍 DEBUG: Diferença SECUNDÁRIO: " + String(systemTimeAfter - systemTimeBefore));
        
        Serial.println("✅ Sincronização com servidor secundário bem-sucedida!");
        return true;
      } else {
        Serial.println("❌ DEBUG: Timestamp inválido do servidor secundário: " + String(now));
      }
    }
    
    Serial.println("❌ Falha na sincronização NTP com todos os servidores");
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
    
    // Usar timestamp direto e formatar manualmente se necessário
    time_t currentTime = getCurrentTime();
    if (currentTime > 1000000000) {
      // Usar formatação personalizada para garantir timezone correto
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
    
    Serial.println("🔧 Servidores NTP atualizados:");
    Serial.println("  - Primário: " + primaryServer);
    Serial.println("  - Secundário: " + secondaryServer);
    Serial.println("  - Terciário: " + tertiaryServer);
    
    // Recriar cliente com novo servidor
    if (timeClient) {
      delete timeClient;
      timeClient = new NTPClient(ntpUDP, primaryServer.c_str(), 0, syncInterval * 1000);
      timeClient->begin();
    }
  }
  
  void setSyncInterval(int interval) {
    syncInterval = interval;
    if (timeClient) {
      timeClient->setUpdateInterval(interval * 1000);
    }
    Serial.println("⏰ Intervalo de sincronização: " + String(interval) + "s");
  }
  
  void setEnabled(bool enabled) {
    ntpEnabled = enabled;
    Serial.println("🕐 NTP " + String(enabled ? "habilitado" : "desabilitado"));
  }
  
  // Getters para APIs
  time_t getCurrentTime() const {
    // USAR NTPClient diretamente - mais confiável que time() no ESP32
    if (timeClient && ntpSynchronized) {
      time_t ntpTime = timeClient->getEpochTime();
      Serial.println("🔍 DEBUG getCurrentTime: NTPClient retornou - " + String(ntpTime));
      if (ntpTime > 1000000000) {
        return ntpTime;
      }
    }
    
    // Fallback para sistema (pode retornar epoch zero)
    time_t now;
    time(&now);
    Serial.println("🔍 DEBUG getCurrentTime: Sistema retornou (fallback) - " + String(now));
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