# Dicas para Upload e Monitor Serial ESP8266/ESP32

## 🔧 Liberando Porta Serial Ocupada

Sempre que a porta serial (COM) estiver ocupada durante o upload, execute o comando abaixo para liberar a porta antes de tentar novamente:

```powershell
taskkill /F /IM python.exe
```

Depois, tente o upload novamente normalmente.

> **Motivo:** O monitor serial do PlatformIO/VSCode ou outro programa pode estar usando a porta, impedindo o upload do firmware.

**Nunca se esqueça deste passo!**

## 📡 Comandos Serial Bem-Sucedidos

### 1. Listar Dispositivos Conectados
```powershell
python -m platformio device list
```

### 2. Compilar o Projeto
```powershell
python -m platformio run
```

### 3. Upload do Firmware
```powershell
python -m platformio run --target upload
```

### 4. Monitor Serial
```powershell
python -m platformio device monitor --port COM3 --baud 115200
```

### 5. Upload + Monitor em Sequência
```powershell
python -m platformio run --target upload ; python -m platformio device monitor --port COM3 --baud 115200
```

### 6. Comandos Úteis Adicionais
- **Verificar porta específica:**
```powershell
python -m platformio device monitor --port COM3 --baud 115200 --filter direct
```

- **Limpar build:**
```powershell
python -m platformio run --target clean
```

## 📝 Notas Importantes

- **Baud Rate:** O projeto usa 115200 (definido em `Serial.begin(115200)`)
- **Porta:** Geralmente COM3 para ESP8266 com chip CH340
- **Sair do Monitor:** Pressione `Ctrl+C` para sair do monitor serial
- **Comando Base:** Use `python -m platformio` ao invés de `pio` ou `platformio` diretamente
