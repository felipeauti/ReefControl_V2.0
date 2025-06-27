#include "WebServer.h"
#include <LittleFS.h>
#include <ESP8266HTTPUpdateServer.h>
#include "RelayController.h"
#include "ConfigManager.h"

bool WebServerManager::begin(RelayController* relayController, ConfigManager* configManager) {
  _relayController = relayController;
  _configManager = configManager;
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
  
  // API para dados dos sensores
  _server.on("/api/sensors", [this]() {
    String json = "{\"temperature\":25.5,\"ph\":7.2,\"tds\":320,\"level\":85}";
    _server.send(200, "application/json", json);
  });
  
  // API para status do sistema
  _server.on("/api/status", [this]() {
    String json = "{";
        json += "\"version\":\"v2.0.5\",";  
    json += "\"build_date\":\"" + String(__DATE__) + " " + String(__TIME__) + "\",";
    json += "\"wifi\":true,";
    json += "\"mqtt\":false,";
    json += "\"uptime\":" + String(millis()) + ",";
    json += "\"heap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"flash_size\":" + String(ESP.getFlashChipSize()) + ",";
    json += "\"chip_id\":\"" + String(ESP.getChipId(), HEX) + "\",";
    json += "\"sdk_version\":\"" + String(ESP.getSdkVersion()) + "\"";
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
      json += "\"pump1\":{";
      json += "\"state\":" + String(_relayController->getPump1State() ? "true" : "false") + ",";
      json += "\"pin\":" + String(_configManager->relay.outputs[0].pin) + ",";
      json += "\"name\":\"" + String(_configManager->relay.outputs[0].name) + "\"},";
      
      json += "\"heater\":{";
      json += "\"state\":" + String(_relayController->getHeaterState() ? "true" : "false") + ",";
      json += "\"pin\":" + String(_configManager->relay.outputs[1].pin) + ",";
      json += "\"name\":\"" + String(_configManager->relay.outputs[1].name) + "\"},";
      
      json += "\"light\":{";
      json += "\"state\":" + String(_relayController->getLightState() ? "true" : "false") + ",";
      json += "\"pin\":" + String(_configManager->relay.outputs[2].pin) + ",";
      json += "\"name\":\"" + String(_configManager->relay.outputs[2].name) + "\"},";
      
      json += "\"pump2\":{";
      json += "\"state\":" + String(_relayController->getPump2State() ? "true" : "false") + ",";
      json += "\"pin\":" + String(_configManager->relay.outputs[3].pin) + ",";
      json += "\"name\":\"" + String(_configManager->relay.outputs[3].name) + "\"}";
    } else {
      // Fallback para valores padrão
      json += "\"pump1\":{";
      json += "\"state\":" + String(_relayController->getPump1State() ? "true" : "false") + ",";
      json += "\"pin\":5,\"name\":\"Bomba Principal\"},";
      
      json += "\"heater\":{";
      json += "\"state\":" + String(_relayController->getHeaterState() ? "true" : "false") + ",";
      json += "\"pin\":4,\"name\":\"Aquecedor\"},";
      
      json += "\"light\":{";
      json += "\"state\":" + String(_relayController->getLightState() ? "true" : "false") + ",";
      json += "\"pin\":14,\"name\":\"Iluminação LED\"},";
      
      json += "\"pump2\":{";
      json += "\"state\":" + String(_relayController->getPump2State() ? "true" : "false") + ",";
      json += "\"pin\":12,\"name\":\"Bomba Reposição\"}";
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
    
    // Parse simples do JSON (formato: {"relay":"pump1","state":true})
    if (body.indexOf("pump1") > 0) {
      bool state = body.indexOf("true") > 0;
      _relayController->setPump1(state);
      Serial.println("🔌 Bomba Principal: " + String(state ? "LIGADA" : "DESLIGADA"));
    }
    else if (body.indexOf("heater") > 0) {
      bool state = body.indexOf("true") > 0;
      _relayController->setHeater(state);
      Serial.println("🔥 Aquecedor: " + String(state ? "LIGADO" : "DESLIGADO"));
    }
    else if (body.indexOf("light") > 0) {
      bool state = body.indexOf("true") > 0;
      _relayController->setLight(state);
      Serial.println("💡 Iluminação: " + String(state ? "LIGADA" : "DESLIGADA"));
    }
    else if (body.indexOf("pump2") > 0) {
      bool state = body.indexOf("true") > 0;
      _relayController->setPump2(state);
      Serial.println("🔌 Bomba Reposição: " + String(state ? "LIGADA" : "DESLIGADA"));
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

  // OTA Update - Sistema nativo ESP8266
  _httpUpdater.setup(&_server, "/update");
  Serial.println("🚀 Sistema OTA nativo configurado em /update");
  
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
  if (LittleFS.exists(path)) {
    File file = LittleFS.open(path, "r");
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