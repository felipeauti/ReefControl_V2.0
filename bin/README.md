# 🐠 ReefControl v2.0 - Binários de Firmware

**Sistema de Automação para Aquários - ESP8266 D1 Mini**

---

## 📊 **VERSÃO MAIS RECENTE**

### **ReefControl_V2.0.11.bin** ⭐ **RECOMENDADO**
- **📅 Data de Compilação:** 27/06/2025 13:20
- **📦 Tamanho:** 417.0KB (427.024 bytes)
- **🧠 Uso de RAM:** 45.6% (37.356/81.920 bytes)
- **💾 Uso de Flash:** 40.5% (422.871/1.044.464 bytes)
- **🔥 Novidades:** NOMENCLATURA PADRONIZADA - APIs e interface unificadas com output1, output2, output3, output4!

**✨ PRINCIPAIS FUNCIONALIDADES v2.0.11:**
- ✅ **NOMENCLATURA PADRONIZADA** - Todas as APIs agora usam output1, output2, output3, output4
- ✅ **NOMES PADRÃO ATUALIZADOS** - Bomba, Termostato, Chiller, Skimmer (mais apropriados para aquarismo)
- ✅ **ÍCONES ESPECÍFICOS** - 🔌 Bomba, 🔥 Termostato, ❄️ Chiller, 🌪️ Skimmer
- ✅ **APIs CONSISTENTES** - WebServer, ConfigManager e interface HTML totalmente alinhados
- ✅ **PERSISTÊNCIA 100% CORRIGIDA** - Configurações de GPIO e nomes salvos permanentemente
- ✅ **Bug Crítico Resolvido** - setDefaults() inicializa corretamente as configurações de saídas
- ✅ **Interface 100% Limpa** - Zero mensagens verdes atrapalhando o layout
- ✅ **Anti-Interrupção de Digitação** - Pode digitar nomes completos sem ser interrompido
- ✅ **Debug Console Otimizado** - Logs organizados e interface profissional

---

## 📈 **CHANGELOG COMPLETO**

### **v2.0.11** - 27/06/2025 - NOMENCLATURA PADRONIZADA! 🏷️✨
**🎯 MELHORIA SOLICITADA:** Padronizar nomenclatura das saídas em todo o sistema!

**✅ MUDANÇAS IMPLEMENTADAS:**

**🏷️ Nomenclatura Unificada:**
- `pump1` → `output1` (Saída 1)
- `heater` → `output2` (Saída 2) 
- `light` → `output3` (Saída 3)
- `pump2` → `output4` (Saída 4)

**🏷️ Nomes Padrão Atualizados:**
- "Bomba Principal" → "Bomba" 🔌
- "Aquecedor" → "Termostato" 🔥
- "Iluminação LED" → "Chiller" ❄️
- "Bomba Reposição" → "Skimmer" 🌪️

**🔧 Arquivos Atualizados:**
- **ConfigManager:** setDefaults() com novos nomes padrão
- **WebServer:** APIs `/api/outputs` com nomenclatura output1-4
- **Interface HTML:** output.html, status.html com nova estrutura
- **MQTT Topics:** Sugestões atualizadas para output1-4
- **Todas as referências** pump1/heater/light/pump2 → output1/2/3/4

**🎯 RESULTADO:** Sistema 100% consistente com nomenclatura padronizada em todas as camadas!

### **v2.0.10** - 27/06/2025 - BUG CRÍTICO CORRIGIDO! 🐛➡️✅
**🎯 PROBLEMA RESOLVIDO:** Configurações de GPIO e nomes sempre voltavam ao padrão após reinicialização!

**🔍 CAUSA RAIZ IDENTIFICADA:**
- A função `setDefaults()` no ConfigManager NÃO estava inicializando o array `relay.outputs[]`
- Apenas as variáveis antigas de compatibilidade eram inicializadas
- Resultado: configurações ficavam com valores inválidos (zeros ou lixo de memória)

**✅ CORREÇÃO IMPLEMENTADA:**
```cpp
// ANTES (BUG):
void setDefaults() {
  // ... outras configurações ...
  relay.pump1Enabled = true;  // ← Só as antigas
  relay.heaterEnabled = true;
  // relay.outputs[] NÃO era inicializado! ❌
}

// DEPOIS (CORRIGIDO):
void setDefaults() {
  // ... outras configurações ...
  
  // Configurações de saídas - CORREÇÃO DO BUG!
  strcpy(relay.outputs[0].name, "Bomba Principal");
  relay.outputs[0].pin = 5;
  relay.outputs[0].enabled = true;
  // ... todos os 4 outputs inicializados corretamente ✅
}
```

**🎯 RESULTADO:** Agora as configurações de GPIO e nomes são REALMENTE persistentes e funcionam 100%!

### **v2.0.9** - 27/06/2025 - INTERFACE 100% LIMPA 🧹✨
**🎯 PROBLEMA RESOLVIDO:** Mensagens verdes de sucesso atrapalhavam o layout e deslocavam os cards!

**✅ CORREÇÕES:**
- **Função `showSuccessMessage()` Removida:**
  - Eliminada completamente a função que criava as telonas verdes
  - Todas as mensagens de sucesso redirecionadas para `debugLog()`
  - CSS `.success-msg` não é mais utilizado

- **Layout 100% Limpo:**
  - Cards permanecem alinhados perfeitamente
  - Zero interferência visual durante operações
  - Interface profissional sem distrações

- **Mensagens Organizadas:**
  - `✅ Configuração salva com sucesso!` → Debug Console
  - `✅ Aquecedor ligado!` → Debug Console  
  - `🔧 Testando por 2 segundos...` → Debug Console
  - `🎉 Todas as configurações salvas!` → Debug Console

- **Experiência Melhorada:**
  - Usuário pode focar nos controles principais
  - Debug console no final da página com todas as informações
  - Performance otimizada sem elementos DOM desnecessários

### **v2.0.8** - 26/06/2025 - ANTI-INTERRUPÇÃO DE DIGITAÇÃO 🚫✏️
**🎯 PROBLEMA RESOLVIDO:** Timer automático interrompia digitação nos campos de nome!

**✅ CORREÇÕES:**
- **Detecção Inteligente de Foco:**
  - Sistema verifica se usuário está digitando antes de atualizar interface
  - `setInterval` pausa quando detecta `document.activeElement` em campo de texto
  - Preserva valores durante render para evitar perda de dados

- **Proteção Durante Edição:**
  - Função `renderOutputs()` com backup/restore de valores ativos
  - Restauração automática de foco e posição do cursor
  - Zero interrupção durante digitação de nomes

- **Experiência Melhorada:**
  - Usuário pode digitar nomes completos sem ser cortado
  - Interface continua responsiva quando não há edição ativa
  - Mantém funcionalidade de atualização automática

**🎯 RESULTADO:** Agora você pode digitar tranquilo sem ser interrompido pelo sistema!

### **v2.0.7** - 26/06/2025 - INTERFACE OTIMIZADA 🎨
**🎯 MELHORIAS DE UX:** Debug console e logs inteligentes implementados!

**✅ OTIMIZAÇÕES:**
- **Debug Console Reposicionado:**
  - Movido do topo para o final da página
  - Não atrapalha mais a visualização dos controles principais
  - Margem superior de 30px para separação visual

- **Sistema de Logs Inteligentes:**
  - Implementada função `detectChanges()` para monitorar alterações
  - Logs apenas quando há mudanças reais (status, nomes, pinos)
  - Eliminado spam de logs a cada carregamento
  - Performance significativamente melhorada

- **Interface Mais Limpa:**
  - Foco nos controles principais (saídas, configurações)
  - Debug console menos intrusivo
  - Experiência de usuário profissional

**🎯 RESULTADO:** Interface muito mais limpa e logs úteis apenas quando necessário!

### **v2.0.6** - 26/06/2025 - PERSISTÊNCIA IMPLEMENTADA 🎯
**🔧 PROBLEMA RESOLVIDO:** Configurações de GPIO e nomes não eram retidas após reinicialização!

**✅ IMPLEMENTAÇÕES:**
- **ConfigManager Expandido:**
  - Nova estrutura `OutputConfig` para configurações individuais
  - Métodos `setOutputConfig()`, `getOutputsJson()`, `setOutputsFromJson()`
  - Persistência automática no LittleFS (`/config.json`)
  - Carregamento automático na inicialização

- **WebServer APIs:**
  - `GET /api/outputs/config` - Obter configurações salvas
  - `POST /api/outputs/config` - Salvar configurações
  - Integração com ConfigManager para persistência real
  - Logs detalhados de todas as operações

- **Interface Web:**
  - Carregamento automático de configurações persistidas
  - Botão "💾 Salvar Todas as Configurações" 
  - Fallback inteligente para configurações padrão
  - Debug console com logs de persistência
  - Mensagens de sucesso visuais

**🎯 RESULTADO:** Agora as configurações de GPIO e nomes são salvas permanentemente!

### **v2.0.5** - 26/06/2025 - OTA FUNCIONANDO! 🚀
- ✅ Sistema OTA 100% funcional com ESP8266HTTPUpdateServer nativo
- ✅ Upload via interface web funcionando perfeitamente
- ✅ Reinicialização automática após update
- ✅ 12 páginas HTML responsivas
- ✅ API REST com endpoints `/api/sensors` e `/api/status`
- ✅ Controle real de 4 relés (GPIO 5, 4, 14, 12)
- ✅ Sistema de arquivos LittleFS
- ✅ mDNS (reefcontrol.local)

### **v2.0.4** - 26/06/2025 - OTA Manual
- ⚠️ Tentativa de OTA com Update.h (não funcionou completamente)
- ✅ Sistema base estável

### **v2.0.3** - 26/06/2025 - Teste OTA
- ⚠️ Primeira tentativa de implementação OTA
- ✅ APIs funcionando

### **v2.0.2** - 26/06/2025 - API Corrigida
- ✅ APIs `/api/sensors` e `/api/status` funcionando
- ✅ Correções de bugs menores

### **v2.0.1** - 26/06/2025 - Primeira Tentativa OTA
- ⚠️ OTA não funcionando ainda
- ✅ Base do sistema implementada

### **v2.0.0** - 26/06/2025 - Base Funcional
- ✅ Sistema base com controle de relés
- ✅ Interface web básica
- ✅ Leitura de sensores

---

## 🔧 **ESPECIFICAÇÕES TÉCNICAS**

### **Hardware Suportado:**
- **Microcontrolador:** ESP8266 D1 Mini
- **Frequência:** 80MHz
- **RAM Total:** 80KB
- **Flash Total:** 1MB (1.044.464 bytes)
- **Sistema de Arquivos:** LittleFS

### **Pinout das Saídas:**
- **GPIO 5 (D1):** Bomba Principal 🔌
- **GPIO 4 (D2):** Aquecedor 🔥  
- **GPIO 14 (D5):** Iluminação LED 💡
- **GPIO 12 (D6):** Bomba Reposição 🔌

### **Conectividade:**
- **WiFi:** 802.11 b/g/n (2.4GHz)
- **Protocolo Web:** HTTP/1.1
- **API:** REST JSON
- **OTA:** HTTP Upload nativo
- **mDNS:** reefcontrol.local

### **Sensores Suportados:**
- **Temperatura:** DS18B20 (OneWire)
- **pH:** Sensor analógico com calibração
- **TDS:** Medidor de sólidos dissolvidos
- **Nível:** Sensor ultrassônico ou capacitivo

---

## 📥 **COMO INSTALAR**

### **Método 1: OTA (Recomendado)**
1. Acesse `http://192.168.68.135/ota`
2. Faça upload do arquivo `ReefControl_V2.0.9.bin`
3. Aguarde a atualização automática
4. Sistema reinicia automaticamente

### **Método 2: Via Cabo USB**
1. Use PlatformIO ou Arduino IDE
2. Conecte o ESP8266 via USB
3. Faça upload do firmware
4. Configure o WiFi pelo AP "ReefControl_AP"

### **Método 3: Binário Direto**
```bash
esptool.py --port COM3 --baud 921600 write_flash 0x0 ReefControl_V2.0.7.bin
```

---

## 🌐 **ACESSO AO SISTEMA**

### **URLs Principais:**
- **Login:** `http://192.168.68.135/login`
- **Home:** `http://192.168.68.135/home`
- **Controle de Saídas:** `http://192.168.68.135/output`
- **Sensores:** `http://192.168.68.135/sensors`
- **OTA Update:** `http://192.168.68.135/ota`
- **Configurações:** `http://192.168.68.135/config`

### **APIs REST:**
- **GET** `/api/outputs` - Status das saídas
- **POST** `/api/outputs` - Controlar saídas
- **GET** `/api/outputs/config` - Obter configurações salvas
- **POST** `/api/outputs/config` - Salvar configurações
- **GET** `/api/sensors` - Dados dos sensores
- **GET** `/api/status` - Status do sistema

---

## 🛠️ **TROUBLESHOOTING**

### **Problema: Configurações não salvam**
- ✅ **RESOLVIDO na v2.0.6!** Sistema de persistência implementado
- Use o botão "💾 Salvar Todas as Configurações"
- Verifique logs no Debug Console (agora no final da página)

### **Problema: Muitos logs no Debug Console**
- ✅ **RESOLVIDO na v2.0.7!** Sistema de logs inteligentes implementado
- Debug Console movido para o final da página
- Logs apenas quando há mudanças reais

### **Problema: Não consegue conectar**
- Verifique se está na rede correta
- Tente `reefcontrol.local` se suportado
- Reset de fábrica: segure RESET por 10s

### **Problema: OTA falha**
- Arquivo deve ter exatamente o nome correto
- Tamanho máximo: 1MB
- Use apenas arquivos `.bin` oficiais

### **Problema: Relés não funcionam**
- Verifique pinout no `/output`
- Teste individual com botão "Testar"
- Verifique logs no Serial Monitor

---

## 📞 **SUPORTE**

- **GitHub:** [ReefControl_V2.0](https://github.com/felipeauti/ReefControl_V2.0)
- **Documentação:** [Wiki do Projeto](https://github.com/felipeauti/ReefControl_V2.0/wiki)
- **Issues:** [Reportar Bugs](https://github.com/felipeauti/ReefControl_V2.0/issues)

---

**🐠 ReefControl v2.0 - Automação Profissional para Aquários**  
*Desenvolvido com ❤️ para a comunidade aquarista* 