#include "WebServer.h"
#include "RelayController.h"
#include "ConfigManager.h"
#include "SensorManager.h"
#include "../time/NtpClient.h"
#include "../capabilities/Capabilities.h"

// Compatibilidade WiFi ESP8266/ESP32
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

bool WebServerManager::begin(RelayController* relayController, ConfigManager* configManager, NtpClientManager* ntpClient, SensorManager* sensorManager) {
  _relayController = relayController;
  _configManager = configManager;
  _ntpClient = ntpClient;
  _sensorManager = sensorManager;
  // Página inicial - redireciona para login
  _server.on("/", [this]() {
    _server.sendHeader("Location", "/login");
    _server.send(302, "text/plain", "");
  });
  
  // Páginas HTML principais (sem .html)
  _server.on("/login", [this]() { serveFile("/html/login.html"); });
  _server.on("/home", [this]() { serveFile("/html/home.html"); });
  _server.on("/config", [this]() { serveFile("/html/config.html"); });
  _server.on("/sensors", [this]() { serveFile("/html/sensors.html"); });
  _server.on("/output", [this]() { serveFile("/html/output.html"); });
  _server.on("/mqtt", [this]() { serveFile("/html/mqtt.html"); });
  _server.on("/ota", [this]() { serveFile("/html/ota.html"); });
  _server.on("/ntp_server", [this]() { serveFile("/html/ntp_server.html"); });
  _server.on("/display", [this]() { serveFile("/html/display.html"); });
  _server.on("/system", [this]() { serveFile("/html/system.html"); });
  _server.on("/logica", [this]() { serveFile("/html/logica.html"); });
  _server.on("/status", [this]() { serveFile("/html/status.html"); });
  _server.on("/file_upload", [this]() { serveFile("/html/file_upload.html"); });
  
  // Páginas HTML principais (com .html para compatibilidade)
  _server.on("/login.html", [this]() { serveFile("/html/login.html"); });
  _server.on("/home.html", [this]() { serveFile("/html/home.html"); });
  _server.on("/config.html", [this]() { serveFile("/html/config.html"); });
  _server.on("/sensors.html", [this]() { serveFile("/html/sensors.html"); });
  _server.on("/output.html", [this]() { serveFile("/html/output.html"); });
  _server.on("/mqtt.html", [this]() { serveFile("/html/mqtt.html"); });
  _server.on("/ota.html", [this]() { serveFile("/html/ota.html"); });
  _server.on("/ntp_server.html", [this]() { serveFile("/html/ntp_server.html"); });
  _server.on("/display.html", [this]() { serveFile("/html/display.html"); });
  _server.on("/system.html", [this]() { serveFile("/html/system.html"); });
  _server.on("/logica.html", [this]() { serveFile("/html/logica.html"); });
  _server.on("/status.html", [this]() { serveFile("/html/status.html"); });
  _server.on("/file_upload.html", [this]() { serveFile("/html/file_upload.html"); });
  
  // API para dados dos sensores
  _server.on("/api/sensors", [this]() {
    if (!_sensorManager) {
      _server.send(500, "application/json", "{\"error\":\"SensorManager não inicializado\"}");
      return;
    }

    String json = "{";
    json += "\"temp\":" + String(_sensorManager->getTemperature(), 1) + ",";
    json += "\"ph\":" + String(_sensorManager->getPH(), 1) + ",";
    json += "\"tds\":" + String(_sensorManager->getTDS()) + ",";
    json += "\"level\":" + String(_sensorManager->getWaterLevel());
    json += "}";
    _server.send(200, "application/json", json);
  });
  
  // API para status do sistema
  _server.on("/api/status", [this]() {
    String json = "{";
        json += "\"version\":\"v2.0.12\",";  
    json += "\"build_date\":\"" + String(__DATE__) + " " + String(__TIME__) + "\",";
    json += "\"wifi\":true,";
    json += "\"mqtt\":false,";
    json += "\"uptime\":" + String(millis()) + ",";
    json += "\"heap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"flash_size\":" + String(ESP.getFlashChipSize()) + ",";
    #ifdef ESP32
      json += "\"chip_id\":\"" + String((uint32_t)ESP.getEfuseMac(), HEX) + "\",";
      json += "\"sdk_version\":\"" + String(ESP.getSdkVersion()) + "\"";
    #else
      json += "\"chip_id\":\"" + String(ESP.getChipId(), HEX) + "\",";
      json += "\"sdk_version\":\"" + String(ESP.getSdkVersion()) + "\"";
    #endif
    json += "}";
    _server.send(200, "application/json", json);
  });

  // API para capabilities do sistema
  _server.on("/api/capabilities", [this]() {
    String json = "{";
    json += "\"version\":\"" + String(CAPS.version) + "\",";
    json += "\"hardware\":\"" + String(CAPS.hardware) + "\",";
    json += "\"maxOutputs\":" + String(CAPS.maxOutputs) + ",";
    json += "\"maxSensors\":" + String(CAPS.maxSensors) + ",";
    json += "\"hasAdvancedAutomation\":" + String(CAPS.hasAdvancedAutomation ? "true" : "false") + ",";
    json += "\"hasAnalytics\":" + String(CAPS.hasAnalytics ? "true" : "false") + ",";
    json += "\"hasMQTTAdvanced\":" + String(CAPS.hasMQTTAdvanced ? "true" : "false") + ",";
    json += "\"hasMultiSensors\":" + String(CAPS.hasMultiSensors ? "true" : "false") + ",";
    json += "\"hasGraphics\":" + String(CAPS.hasGraphics ? "true" : "false") + ",";
    json += "\"ramAvailable\":" + String(CAPS.ramAvailable) + ",";
    json += "\"flashAvailable\":" + String(CAPS.flashAvailable) + ",";
    json += "\"isCompact\":" + String(isCompactVersion() ? "true" : "false") + ",";
    json += "\"isPro\":" + String(isProVersion() ? "true" : "false");
    json += "}";
    _server.send(200, "application/json", json);
  });

  // API para controle das saídas/relés - GET (status)
  _server.on("/api/outputs", HTTP_GET, [this]() {
    if (!_relayController) {
      _server.send(500, "application/json", "{\"error\":\"RelayController não inicializado\"}");
      return;
    }
    
    String json = "{";
    
    // Se temos ConfigManager, usar configurações persistidas
    if (_configManager) {
      json += "\"output1\":{";
      json += "\"state\":" + String(_relayController->getPump1State() ? "true" : "false") + ",";
      json += "\"pin\":" + String(_configManager->relay.outputs[0].pin) + ",";
      json += "\"name\":\"" + String(_configManager->relay.outputs[0].name) + "\"},";
      
      json += "\"output2\":{";
      json += "\"state\":" + String(_relayController->getHeaterState() ? "true" : "false") + ",";
      json += "\"pin\":" + String(_configManager->relay.outputs[1].pin) + ",";
      json += "\"name\":\"" + String(_configManager->relay.outputs[1].name) + "\"},";
      
      json += "\"output3\":{";
      json += "\"state\":" + String(_relayController->getLightState() ? "true" : "false") + ",";
      json += "\"pin\":" + String(_configManager->relay.outputs[2].pin) + ",";
      json += "\"name\":\"" + String(_configManager->relay.outputs[2].name) + "\"},";
      
      json += "\"output4\":{";
      json += "\"state\":" + String(_relayController->getPump2State() ? "true" : "false") + ",";
      json += "\"pin\":" + String(_configManager->relay.outputs[3].pin) + ",";
      json += "\"name\":\"" + String(_configManager->relay.outputs[3].name) + "\"}";
    } else {
      // Fallback para valores padrão
      json += "\"output1\":{";
      json += "\"state\":" + String(_relayController->getPump1State() ? "true" : "false") + ",";
      json += "\"pin\":5,\"name\":\"Bomba\"},";
      
      json += "\"output2\":{";
      json += "\"state\":" + String(_relayController->getHeaterState() ? "true" : "false") + ",";
      json += "\"pin\":4,\"name\":\"Termostato\"},";
      
      json += "\"output3\":{";
      json += "\"state\":" + String(_relayController->getLightState() ? "true" : "false") + ",";
      json += "\"pin\":14,\"name\":\"Chiller\"},";
      
      json += "\"output4\":{";
      json += "\"state\":" + String(_relayController->getPump2State() ? "true" : "false") + ",";
      json += "\"pin\":12,\"name\":\"Skimmer\"}";
    }
    
    json += "}";
    _server.send(200, "application/json", json);
  });

  // API para controle das saídas/relés - POST (controle)
  _server.on("/api/outputs", HTTP_POST, [this]() {
    if (!_relayController) {
      _server.send(500, "application/json", "{\"error\":\"RelayController não inicializado\"}");
      return;
    }
    
    String body = _server.arg("plain");
    Serial.println("📦 Comando recebido: " + body);
    
    // Parse simples do JSON (formato: {"relay":"output1","state":true})
    if (body.indexOf("output1") > 0) {
      bool state = body.indexOf("true") > 0;
      _relayController->setPump1(state);
      Serial.println("🔌 Saída 1: " + String(state ? "LIGADA" : "DESLIGADA"));
    }
    else if (body.indexOf("output2") > 0) {
      bool state = body.indexOf("true") > 0;
      _relayController->setHeater(state);
      Serial.println("🔥 Saída 2: " + String(state ? "LIGADA" : "DESLIGADA"));
    }
    else if (body.indexOf("output3") > 0) {
      bool state = body.indexOf("true") > 0;
      _relayController->setLight(state);
      Serial.println("💡 Saída 3: " + String(state ? "LIGADA" : "DESLIGADA"));
    }
    else if (body.indexOf("output4") > 0) {
      bool state = body.indexOf("true") > 0;
      _relayController->setPump2(state);
      Serial.println("🔌 Saída 4: " + String(state ? "LIGADA" : "DESLIGADA"));
    }
    
    _server.send(200, "application/json", "{\"success\":true}");
  });

  // API para salvar configurações de saídas - POST
  _server.on("/api/outputs/config", HTTP_POST, [this]() {
    if (!_configManager) {
      _server.send(500, "application/json", "{\"error\":\"ConfigManager não inicializado\"}");
      return;
    }
    
    String body = _server.arg("plain");
    Serial.println("🔧 Configuração recebida: " + body);
    
    if (_configManager->setOutputsFromJson(body)) {
      Serial.println("✅ Configurações de saídas salvas com sucesso!");
      _server.send(200, "application/json", "{\"success\":true,\"message\":\"Configurações salvas com sucesso!\"}");
    } else {
      Serial.println("❌ Erro ao salvar configurações de saídas");
      _server.send(400, "application/json", "{\"error\":\"Erro ao salvar configurações\"}");
    }
  });

  // API para obter configurações de saídas - GET
  _server.on("/api/outputs/config", HTTP_GET, [this]() {
    if (!_configManager) {
      _server.send(500, "application/json", "{\"error\":\"ConfigManager não inicializado\"}");
      return;
    }
    
    String json = _configManager->getOutputsJson();
    _server.send(200, "application/json", json);
  });

  // API para configurações NTP - GET
  _server.on("/api/ntp/config", HTTP_GET, [this]() {
    if (!_configManager) {
      _server.send(500, "application/json", "{\"error\":\"ConfigManager não inicializado\"}");
      return;
    }
    
    String json = "{";
    json += "\"enabled\":" + String(_configManager->ntp.enabled ? "true" : "false") + ",";
    json += "\"server1\":\"" + String(_configManager->ntp.server1) + "\",";
    json += "\"server2\":\"" + String(_configManager->ntp.server2) + "\",";
    json += "\"server3\":\"" + String(_configManager->ntp.server3) + "\",";
    json += "\"timezone\":" + String(_configManager->ntp.timezoneOffset) + ",";  // Numérico
    json += "\"timezoneString\":\"" + String(_configManager->ntp.timezone) + "\",";  // Compatibilidade
    json += "\"syncInterval\":" + String(_configManager->ntp.syncInterval);
    json += "}";
    _server.send(200, "application/json", json);
  });

  // API para configurações NTP - POST
  _server.on("/api/ntp/config", HTTP_POST, [this]() {
    if (!_configManager) {
      _server.send(500, "application/json", "{\"error\":\"ConfigManager não inicializado\"}");
      return;
    }
    
    String body = _server.arg("plain");
    Serial.println("🕐 Configuração NTP recebida: " + body);
    
    // Parse JSON simples
    if (body.indexOf("enabled") > 0) {
      _configManager->ntp.enabled = body.indexOf("\"enabled\":true") > 0;
    }
    if (body.indexOf("server1") > 0) {
      int start = body.indexOf("\"server1\":\"") + 11;
      int end = body.indexOf("\"", start);
      if (start > 10 && end > start) {
        String server = body.substring(start, end);
        server.toCharArray(_configManager->ntp.server1, sizeof(_configManager->ntp.server1));
      }
    }
    if (body.indexOf("server2") > 0) {
      int start = body.indexOf("\"server2\":\"") + 11;
      int end = body.indexOf("\"", start);
      if (start > 10 && end > start) {
        String server = body.substring(start, end);
        server.toCharArray(_configManager->ntp.server2, sizeof(_configManager->ntp.server2));
      }
    }
    if (body.indexOf("server3") > 0) {
      int start = body.indexOf("\"server3\":\"") + 11;
      int end = body.indexOf("\"", start);
      if (start > 10 && end > start) {
        String server = body.substring(start, end);
        server.toCharArray(_configManager->ntp.server3, sizeof(_configManager->ntp.server3));
      }
    }
    if (body.indexOf("timezone") > 0) {
      // Verificar se é timezone numérico (novo formato)
      int numStart = body.indexOf("\"timezone\":") + 11;
      int numEnd = body.indexOf(",", numStart);
      if (numEnd == -1) numEnd = body.indexOf("}", numStart);
      
      if (numStart > 10 && numEnd > numStart) {
        String tzValue = body.substring(numStart, numEnd);
        tzValue.trim();
        
        // Se for numérico, usar como offset UTC
        if (tzValue.indexOf("\"") == -1) {
          _configManager->ntp.timezoneOffset = tzValue.toInt();
          Serial.println("🌍 Timezone numérico configurado: UTC" + String(tzValue.toInt() >= 0 ? "+" : "") + tzValue);
        } else {
          // Formato string (compatibilidade)
          int start = body.indexOf("\"timezone\":\"") + 12;
          int end = body.indexOf("\"", start);
          if (start > 11 && end > start) {
            String tz = body.substring(start, end);
            tz.toCharArray(_configManager->ntp.timezone, sizeof(_configManager->ntp.timezone));
          }
        }
      }
    }
    if (body.indexOf("syncInterval") > 0) {
      int start = body.indexOf("\"syncInterval\":") + 15;
      int end = body.indexOf("}", start);
      if (end == -1) end = body.indexOf(",", start);
      if (start > 14 && end > start) {
        _configManager->ntp.syncInterval = body.substring(start, end).toInt();
      }
    }
    
    if (_configManager->save()) {
      Serial.println("✅ Configurações NTP salvas com sucesso!");
      
      // Aplicar configurações no NTPClient real
      if (_ntpClient) {
        Serial.println("🔍 DEBUG CONFIG: Aplicando configurações NTP...");
        Serial.println("🔍 DEBUG CONFIG: Servidor1 = " + String(_configManager->ntp.server1));
        Serial.println("🔍 DEBUG CONFIG: Servidor2 = " + String(_configManager->ntp.server2));
        Serial.println("🔍 DEBUG CONFIG: Servidor3 = " + String(_configManager->ntp.server3));
        Serial.println("🔍 DEBUG CONFIG: Intervalo = " + String(_configManager->ntp.syncInterval));
        
        _ntpClient->setServers(
          String(_configManager->ntp.server1),
          String(_configManager->ntp.server2),
          String(_configManager->ntp.server3)
        );
        _ntpClient->setSyncInterval(_configManager->ntp.syncInterval);
        _ntpClient->setTimezone(_configManager->ntp.timezoneOffset);  // Aplicar timezone
        _ntpClient->setEnabled(_configManager->ntp.enabled);
        
        // Forçar sincronização com novas configurações
        Serial.println("🔍 DEBUG CONFIG: Forçando sincronização...");
        bool configSync = _ntpClient->forceUpdate();
        Serial.println("🔍 DEBUG CONFIG: Resultado sincronização = " + String(configSync));
      }
      
      _server.send(200, "application/json", "{\"success\":true,\"message\":\"Configurações NTP salvas com sucesso!\"}");
    } else {
      Serial.println("❌ Erro ao salvar configurações NTP");
      _server.send(500, "application/json", "{\"error\":\"Erro ao salvar configurações NTP\"}");
    }
  });

  // API para status NTP - GET
  _server.on("/api/ntp/status", HTTP_GET, [this]() {
    if (!_ntpClient) {
      _server.send(500, "application/json", "{\"error\":\"NTP Client não inicializado\"}");
      return;
    }
    
    Serial.println("🔍 DEBUG API: Obtendo status NTP...");
    
    // DIAGNÓSTICO COMPLETO PRIMEIRO
    Serial.println("🔍 DEBUG API: Estado do NTPClient:");
    Serial.println("🔍 DEBUG API: - _ntpClient pointer = " + String((unsigned long)_ntpClient, HEX));
    Serial.println("🔍 DEBUG API: - WiFi conectado = " + String(WiFi.isConnected()));
    Serial.println("🔍 DEBUG API: - Uptime = " + String(millis()/1000) + "s");
    
    bool synchronized = _ntpClient->isSynchronized();
    Serial.println("🔍 DEBUG API: isSynchronized = " + String(synchronized));
    
    time_t now = _ntpClient->getCurrentTime();
    Serial.println("🔍 DEBUG API: getCurrentTime retornou = " + String(now));
    Serial.println("🔍 DEBUG API: Data Unix timestamp " + String(now) + " = " + String(ctime(&now)));
    
    // FORÇAR verificação do que está acontecendo!
    if (now < 1000000000) {
      Serial.println("❌ PROBLEMA ENCONTRADO: Timestamp muito baixo!");
      Serial.println("❌ Isso indica que o sistema não foi sincronizado!");
      
      // Verificar se NTP está realmente funcionando
      if (_ntpClient->isSynchronized()) {
        Serial.println("⚠️ PARADOXO: isSynchronized=true mas timestamp baixo!");
      } else {
        Serial.println("⚠️ CONFIRMADO: isSynchronized=false - NTP não sincronizado!");
      }
    } else {
      Serial.println("✅ Timestamp parece válido (> 1 bilhão)");
    }
    
    String status = _ntpClient->getStatus();
    String lastSync = _ntpClient->getLastSyncFormatted();
    unsigned long uptime = _ntpClient->getUptime();
    
    String json = "{";
    json += "\"synchronized\":" + String(synchronized ? "true" : "false") + ",";
    json += "\"timestamp\":" + String(now) + ",";
    json += "\"lastSync\":\"" + lastSync + "\",";
    json += "\"currentTime\":" + String(now) + ",";
    json += "\"status\":\"" + status + "\",";
    json += "\"uptime\":" + String(uptime);
    json += "}";
    _server.send(200, "application/json", json);
  });

  // API para forçar sincronização NTP - POST
  _server.on("/api/ntp/sync", HTTP_POST, [this]() {
    if (!_ntpClient) {
      _server.send(500, "application/json", "{\"error\":\"NTP Client não inicializado\"}");
      return;
    }
    
    Serial.println("🕐 Forçando sincronização NTP via API...");
    Serial.println("🔍 DEBUG SYNC: Antes do forceUpdate");
    
    time_t beforeSync = _ntpClient->getCurrentTime();
    Serial.println("🔍 DEBUG SYNC: Tempo antes = " + String(beforeSync));
    
    bool success = _ntpClient->forceUpdate();
    Serial.println("🔍 DEBUG SYNC: forceUpdate retornou = " + String(success));
    
    time_t afterSync = _ntpClient->getCurrentTime();
    Serial.println("🔍 DEBUG SYNC: Tempo depois = " + String(afterSync));
    
    // VERIFICAÇÃO CRÍTICA
    if (success && afterSync < 1000000000) {
      Serial.println("🚨 ERRO CRÍTICO: Sync reportou sucesso mas timestamp ainda baixo!");
      Serial.println("🚨 Tentando diagnóstico mais profundo...");
      
      // Verificar estado do NTPClient
      Serial.println("🔍 isSynchronized: " + String(_ntpClient->isSynchronized()));
      Serial.println("🔍 getStatus: " + _ntpClient->getStatus());
      Serial.println("🔍 getLastSyncFormatted: " + _ntpClient->getLastSyncFormatted());
    }
    
    String json = "{";
    json += "\"success\":" + String(success ? "true" : "false") + ",";
    json += "\"message\":\"" + String(success ? "Sincronização NTP bem-sucedida" : "Falha na sincronização NTP") + "\",";
    json += "\"timestamp\":" + String(millis());
    json += "}";
    _server.send(200, "application/json", json);
    
    Serial.println(success ? "✅ Sincronização NTP bem-sucedida" : "❌ Falha na sincronização NTP");
  });

  // API para upload de arquivos - POST
  _server.on("/api/upload", HTTP_POST, 
    [this]() {
      _server.send(200, "application/json", "{\"success\":true,\"message\":\"Upload concluído\"}");
    },
    [this]() {
      HTTPUpload& upload = _server.upload();
      
      if (upload.status == UPLOAD_FILE_START) {
        String filename = "/html/" + upload.filename;
        Serial.println("📤 Iniciando upload: " + filename);
        
        if (!filename.endsWith(".html") && !filename.endsWith(".css") && 
            !filename.endsWith(".js") && !filename.endsWith(".json") &&
            !filename.endsWith(".png") && !filename.endsWith(".jpg") && 
            !filename.endsWith(".gif") && !filename.endsWith(".ico")) {
          Serial.println("❌ Tipo de arquivo não permitido: " + filename);
          return;
        }
        
        _uploadFile = FileSystemClass.open(filename, "w");
        if (!_uploadFile) {
          Serial.println("❌ Erro ao criar arquivo: " + filename);
        }
      } 
      else if (upload.status == UPLOAD_FILE_WRITE) {
        if (_uploadFile) {
          _uploadFile.write(upload.buf, upload.currentSize);
        }
      } 
      else if (upload.status == UPLOAD_FILE_END) {
        if (_uploadFile) {
          _uploadFile.close();
          Serial.println("✅ Upload concluído: " + upload.filename + " (" + String(upload.totalSize) + " bytes)");
        }
      }
    }
  );

  // API para informações do sistema
  _server.on("/api/system/info", [this]() {
    #ifdef ESP32
      size_t totalBytes = FileSystemClass.totalBytes();
      size_t usedBytes = FileSystemClass.usedBytes();
      size_t freeSpace = totalBytes - usedBytes;
    #else
      FSInfo fs_info;
      FileSystemClass.info(fs_info);
      size_t totalBytes = fs_info.totalBytes;
      size_t usedBytes = fs_info.usedBytes;
      size_t freeSpace = totalBytes - usedBytes;
    #endif
    
    String json = "{";
    json += "\"version\":\"" + String(getVersionString()) + "\",";
    json += "\"uptime\":\"" + String(millis() / 1000) + "s\",";
    json += "\"freeSpace\":" + String(freeSpace) + ",";
    json += "\"totalSpace\":" + String(totalBytes) + ",";
    json += "\"usedSpace\":" + String(usedBytes) + ",";
    json += "\"wifi\":\"" + WiFi.SSID() + " (" + WiFi.RSSI() + "dBm)\"";
    json += "}";
    _server.send(200, "application/json", json);
  });

  // OTA Update - Sistema nativo (ESP8266 apenas)
  #ifndef ESP32
    _httpUpdater.setup(&_server, "/update");
    Serial.println("🚀 Sistema OTA nativo ESP8266 configurado em /update");
  #else
    Serial.println("🚀 Sistema OTA ESP32 disponível via /update (implementação futura)");
  #endif
  
  // Página 404
  _server.onNotFound([this]() {
    _server.send(404, "text/html", 
      "<!DOCTYPE html><html><head><title>404 - Página não encontrada</title></head>"
      "<body><h1>404 - Página não encontrada</h1>"
      "<p><a href='/home'>Voltar ao início</a></p></body></html>");
  });
  
  _server.begin();
  Serial.println("🌐 Servidor web completo iniciado na porta 80");
  Serial.println("🚀 OTA Update habilitado em /update");
  return true;
}

void WebServerManager::serveFile(const String& path) {
  if (FileSystemClass.exists(path)) {
    File file = FileSystemClass.open(path, "r");
    if (file) {
      _server.streamFile(file, "text/html");
      file.close();
    } else {
      _server.send(500, "text/html", "Erro ao abrir arquivo");
    }
  } else {
    _server.send(404, "text/html", "Arquivo não encontrado: " + path);
  }
}

void WebServerManager::handleClient() {
  _server.handleClient();
} 