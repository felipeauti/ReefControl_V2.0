# 🔄 AquaControl - Fluxo de Trabalho Atualizado

## 📁 Nova Estrutura de Pastas

```
AquaControl/
├── src/                     # Código fonte
├── bin/                     # Todos os binários (.bin)
├── commands/                # Documentações e comandos
├── platformio.ini           # Configuração do projeto
└── ...
```

## 🚀 Comandos de Build e Deploy

### 1. Compilar e Gerar Binário
```bash
# Compilar projeto
python -m platformio run

# Upload para ESP8266 (opcional)
python -m platformio run --target upload

# Gerar binário na pasta bin/
copy .pio\build\d1_mini\firmware.bin .\bin\AquaControl_v1.x.bin
```

### 2. Commit e Push
```bash  
# Adicionar alterações
git add .

# Commit com versão
git commit -m "🔧 AquaControl v1.x - Descrição das mudanças"

# Push para GitHub
git push origin master
```

### 3. Versionamento
```bash
# Criar tag de versão
git tag v1.x.x -m "Versão 1.x.x - Sistema OTA"

# Push da tag
git push origin --tags
```

## 📋 Convenções de Nomenclatura

### Binários (`bin/`)
- `AquaControl_v1.0.bin` - Versão principal
- `AquaControl_BETA_v1.1.bin` - Versão beta  
- `AquaControl_FIX_v1.0.1.bin` - Correção de bugs
- `AquaControl_FEATURE_nome.bin` - Nova funcionalidade

### Documentação (`commands/`)
- `WORKFLOW.md` - Este arquivo de fluxo
- `GITHUB.md` - Comandos Git
- `DICA_SERIAL.md` - Comandos PlatformIO
- `COMANDOS_WINDOWS.md` - Comandos específicos Windows
- `FIRMWARE_OTA.md` - Instruções OTA

## 🎯 Exemplo Prático

```bash
# Desenvolvimento completo
python -m platformio run
python -m platformio run --target upload
copy .pio\build\d1_mini\firmware.bin .\bin\AquaControl_v1.2.bin

# Versionamento
git add .
git commit -m "🚀 AquaControl v1.2 - Melhoria na interface OTA"
git tag v1.2.0 -m "Versão 1.2 - Interface OTA melhorada"
git push origin master
git push origin --tags
```

## 📝 Histórico de Versões

### v1.0 - Sistema Base
- WiFiManager configurado
- Interface web básica
- Sistema OTA funcional

### v1.1 - Melhorias na Interface  
- Progresso simulado
- CSS responsivo
- Páginas de erro personalizadas

### v1.2 - Redirecionamento Automático
- Detecção de reinício
- Fallback manual
- Sistema robusto de upload

---

*Fluxo atualizado para organizar melhor o projeto AquaControl* 🚀 