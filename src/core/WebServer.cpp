#include "WebServer.h"
#include <LittleFS.h>

bool WebServerManager::begin() {
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
    String json = "{\"wifi\":true,\"mqtt\":false,\"uptime\":";
    json += millis();
    json += ",\"heap\":" + String(ESP.getFreeHeap()) + "}";
    _server.send(200, "application/json", json);
  });
  
  // Página 404
  _server.onNotFound([this]() {
    _server.send(404, "text/html", 
      "<!DOCTYPE html><html><head><title>404 - Página não encontrada</title></head>"
      "<body><h1>404 - Página não encontrada</h1>"
      "<p><a href='/home'>Voltar ao início</a></p></body></html>");
  });
  
  _server.begin();
  Serial.println("🌐 Servidor web completo iniciado na porta 80");
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