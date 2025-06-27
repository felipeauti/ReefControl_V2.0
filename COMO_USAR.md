# 🚀 Como Usar o ReefControl no Wemos D1 Mini

## ⚡ Setup Rápido

### 1️⃣ **Configurar Wi-Fi**
Edite o arquivo `src/main.cpp` nas linhas 10-11:
```cpp
const char* ssid = "SUA_REDE_WIFI";        // ← Coloque o nome da sua rede
const char* password = "SUA_SENHA_WIFI";   // ← Coloque a senha da sua rede
```

### 2️⃣ **Compilar e Fazer Upload**
```bash
# Compilar
python -m platformio run

# Upload para o Wemos
python -m platformio run --target upload

# Ver logs (opcional)
python -m platformio device monitor --port COM3 --baud 115200
```

### 3️⃣ **Acessar a Interface Web**
1. Abra o monitor serial para ver o IP
2. Acesse no navegador: `http://IP_DO_WEMOS`
3. **Login:** `admin` / **Senha:** `123456`

## 🔌 Conexões de Hardware

### **Wemos D1 Mini - Pinout:**
```
┌─────────────┐
│ RST    3V3  │
│ A0     D8   │  ← Sensor pH/TDS (A0)
│ D0     D7   │  
│ D1     D6   │  ← Relé 1 - Bomba Principal (D1)
│ D2     D5   │  ← Relé 2 - Aquecedor (D2)
│ D3     D4   │  ← Relé 3 - Iluminação (D3)
│ D4     D0   │  ← Relé 4 - Bomba Reposição (D4)
│ GND    5V   │  ← Sensor DS18B20 (D5)
└─────────────┘
```

### **Sensores:**
- **DS18B20 (Temperatura):** D5 (GPIO 14)
- **pH/TDS Sensor:** A0 (Analógico)
- **Relés 4x:** D1, D2, D3, D4

## 📱 Interface Web - Páginas Disponíveis

| URL | Descrição |
|-----|-----------|
| `/` | **Login** - Página inicial |
| `/home` | **Dashboard** - Visão geral |
| `/outputs` | **Controle Manual** - Liga/desliga equipamentos |
| `/api/status` | **API** - Dados em JSON |
| `/api/control` | **API** - Controlar saídas |

## 🔧 Funcionalidades Ativas

### ✅ **Funcionando:**
- 🌐 **Servidor Web** - Interface completa
- 📊 **Dashboard** - Monitoramento em tempo real
- 🔌 **Controle Manual** - 4 relés controláveis
- 🌡️ **Sensor Temperatura** - DS18B20
- 🧪 **Simulação pH/TDS** - Dados de exemplo
- 🚀 **OTA Updates** - Atualizações sem fio
- 📡 **mDNS** - Acesso via `reefcontrol.local`
- 🧠 **Automação** - Aquecedor automático

### 🔄 **API REST:**

#### **GET /api/status** - Status dos sensores
```json
{
  "temp": 24.5,
  "ph": 7.8,
  "tds": 380,
  "level": 85,
  "uptime": 1234,
  "freeheap": 25000
}
```

#### **POST /api/control** - Controlar saídas
```json
{
  "output": "pump1",  // pump1, heater, light, pump2
  "state": true       // true = ligar, false = desligar
}
```

## 🏠 Automação Integrada

### **Termostato Automático:**
- Liga aquecedor se temperatura < 24°C
- Desliga aquecedor se temperatura > 26°C

### **Indicadores Visual:**
- LED piscando = Sistema funcionando
- LED fixo = Conectado ao Wi-Fi

## 🛠️ Comandos Úteis

### **Upload + Monitor:**
```bash
python -m platformio run --target upload && python -m platformio device monitor --port COM3 --baud 115200
```

### **Se porta ocupada:**
```bash
taskkill /F /IM python.exe
```

### **OTA Update:**
- Acesse a aba **OTA** na interface web
- Faça upload do arquivo `.bin` gerado
- Senha OTA: `123456`

## 🔗 Acessos Rápidos

### **URLs Diretas:**
- **Dashboard:** `http://IP_WEMOS/home`
- **Controle:** `http://IP_WEMOS/outputs`
- **API Status:** `http://IP_WEMOS/api/status`

### **mDNS (se suportado):**
- **Dashboard:** `http://reefcontrol.local/home`
- **Controle:** `http://reefcontrol.local/outputs`

## 📱 Como Usar no Celular

1. Conecte na mesma rede Wi-Fi
2. Abra o navegador
3. Digite o IP do Wemos
4. Faça login: `admin` / `123456`
5. Use os toggles para controlar equipamentos

## 🔒 Segurança

### **Alterar Senha:**
Edite no `main.cpp` linha ~90:
```javascript
if(username==='admin'&&password==='123456'){  // ← Altere aqui
```

### **Senha OTA:**
Edite no `main.cpp` linha ~420:
```cpp
ArduinoOTA.setPassword("123456");  // ← Altere aqui
```

---

## 🎯 **Tudo Pronto!**

Agora você tem um sistema completo de automação para aquário rodando no Wemos D1 Mini com interface web profissional! 🐠🚀 