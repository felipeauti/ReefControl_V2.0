# 🗺️ ROADMAP COMPLETO - ReefControl v2.0.11 → v2.3.0

## 📊 **VISÃO GERAL DO PROJETO**

### **Estado Atual (v2.0.11)**
- ✅ 4 saídas configuráveis
- ✅ Interface web funcional
- ✅ Persistência de configurações
- ✅ Controle manual básico
- ✅ APIs REST completas

### **Objetivo Final (v2.3.0)**
- 🎯 8 saídas configuráveis
- 🎯 Sistema de automação inteligente
- 🎯 Interface moderna e intuitiva
- 🎯 Validação avançada de configurações
- 🎯 Controles manuais aprimorados

---

## 🚀 **FASES DE DESENVOLVIMENTO**

### **FASE 1: MELHORIAS BÁSICAS (v2.0.12)**
**⏱️ Tempo Estimado: 2-3 horas**
**🎯 Objetivo: Melhorar UX e controles básicos**

#### **1.1 Padronização Visual**
- [ ] Substituir todos os ícones por 🔌 (tomada genérica)
- [ ] Atualizar títulos para "Saída 1", "Saída 2", etc.
- [ ] Manter nomes personalizáveis pelos usuários

#### **1.2 Controles Manuais Aprimorados**
- [ ] Remover botão "Testar"
- [ ] Criar botões separados "LIGA" e "DESLIGA"
- [ ] Implementar bypass de automação no modo manual
- [ ] Melhorar feedback visual dos estados

#### **1.3 Validação Básica de GPIO**
- [ ] Implementar validação em tempo real
- [ ] Destacar GPIOs duplicados em vermelho
- [ ] Mostrar aviso ao tentar salvar configuração inválida
- [ ] Sugerir GPIOs disponíveis

**📋 Arquivos Afetados:**
- `data/html/output.html`
- `src/html/output.html`
- `src/core/WebServer.cpp`

---

### **FASE 2: EXPANSÃO PARA 8 SAÍDAS (v2.1.0)**
**⏱️ Tempo Estimado: 4-6 horas**
**🎯 Objetivo: Dobrar capacidade do sistema**

#### **2.1 Reestruturação Backend**
- [ ] Expandir `OutputConfig outputs[8]` em ConfigManager.h
- [ ] Atualizar RelayController para 8 saídas
- [ ] Criar novos métodos getOutput1-8() e setOutput1-8()
- [ ] Implementar configurações padrão para 8 saídas

#### **2.2 Expansão das APIs**
- [ ] Atualizar `/api/outputs` para 8 saídas (output1-output8)
- [ ] Expandir `/api/outputs/config` para suportar 8 configurações
- [ ] Atualizar parsing JSON para 8 saídas
- [ ] Implementar validação de GPIO para 8 saídas

#### **2.3 Interface Redesenhada**
- [ ] Redesenhar grid para acomodar 8 cards
- [ ] Implementar layout responsivo (2x4 desktop, 1x8 mobile)
- [ ] Otimizar performance para mais elementos
- [ ] Adicionar paginação ou scroll se necessário

#### **2.4 Validação Avançada**
- [ ] Implementar matriz de GPIOs ocupados
- [ ] Validação cruzada entre saídas e sensores
- [ ] Alertas visuais para conflitos
- [ ] Sugestões inteligentes de GPIOs livres

**📋 Arquivos Afetados:**
- `src/core/ConfigManager.h`
- `src/core/ConfigManager.cpp`
- `src/core/RelayController.h`
- `src/core/RelayController.cpp`
- `src/core/WebServer.cpp`
- `data/html/output.html`
- `src/html/output.html`

---

### **FASE 3: SISTEMA DE AUTOMAÇÃO (v2.2.0)**
**⏱️ Tempo Estimado: 8-12 horas**
**🎯 Objetivo: Implementar automação inteligente**

#### **3.1 Estrutura de Automação**
- [ ] Criar enum `AutomationType` com todos os tipos
- [ ] Implementar struct `AutomationConfig`
- [ ] Adicionar campos de automação em `OutputConfig`
- [ ] Criar sistema de prioridades para automações

#### **3.2 Tipos de Automação**

**🕐 Schedule (Horários Programados)**
- [ ] Configuração de horário de liga/desliga
- [ ] Suporte a múltiplos horários por dia
- [ ] Configuração de dias da semana
- [ ] Modo fotoperíodo para iluminação

**🌡️ Controle por Temperatura**
- [ ] Temperatura alta (chiller)
- [ ] Temperatura baixa (aquecedor)
- [ ] Configuração de histerese
- [ ] Limites de segurança

**🧪 Controle por pH**
- [ ] pH alto (bomba dosadora ácido)
- [ ] pH baixo (bomba dosadora base)
- [ ] Tempo mínimo entre acionamentos
- [ ] Configuração de dead-band

**⚡ Controle por ORP**
- [ ] ORP alto (parar ozônio)
- [ ] ORP baixo (ligar ozônio)
- [ ] Configuração de limites seguros
- [ ] Tempo de ciclo mínimo

**💧 Controle por Nível**
- [ ] Nível alto (parar reposição)
- [ ] Nível baixo (ligar reposição)
- [ ] Proteção contra funcionamento a seco
- [ ] Tempo máximo de funcionamento

#### **3.3 Interface de Automação**
- [ ] Dropdown para seleção do tipo de automação
- [ ] Campos dinâmicos baseados no tipo selecionado
- [ ] Preview das configurações
- [ ] Modo de teste para automações

#### **3.4 Lógica de Controle**
- [ ] Criar classe `AutomationManager`
- [ ] Implementar loop de controle principal
- [ ] Sistema de logs de ações automáticas
- [ ] Modo manual com bypass de automação

**📋 Arquivos Afetados:**
- `src/core/ConfigManager.h`
- `src/core/ConfigManager.cpp`
- `src/automation/AutomationManager.h` (novo)
- `src/automation/AutomationManager.cpp` (novo)
- `src/core/WebServer.cpp`
- `data/html/output.html`
- `src/html/output.html`
- `src/main.cpp`

---

### **FASE 4: INTERFACE AVANÇADA (v2.3.0)**
**⏱️ Tempo Estimado: 4-6 horas**
**🎯 Objetivo: Interface moderna e intuitiva**

#### **4.1 Dashboard de Automação**
- [ ] Página dedicada para visualizar automações ativas
- [ ] Gráficos de histórico de acionamentos
- [ ] Status em tempo real de todas as automações
- [ ] Alertas e notificações

#### **4.2 Configuração Avançada**
- [ ] Wizard de configuração inicial
- [ ] Templates pré-definidos (Aquário Marinho, Doce, etc.)
- [ ] Importar/exportar configurações
- [ ] Backup automático de configurações

#### **4.3 Melhorias UX**
- [ ] Animações suaves
- [ ] Feedback visual aprimorado
- [ ] Tooltips explicativos
- [ ] Modo escuro/claro

#### **4.4 Sistema de Logs**
- [ ] Log de todas as ações automáticas
- [ ] Histórico de mudanças de configuração
- [ ] Exportação de logs
- [ ] Análise de padrões de uso

**📋 Arquivos Afetados:**
- `data/html/automation.html` (novo)
- `src/html/automation.html` (novo)
- `data/html/logs.html` (novo)
- `src/html/logs.html` (novo)
- CSS e JavaScript de todas as páginas

---

## 🎯 **DETALHAMENTO TÉCNICO**

### **Estrutura de Dados Proposta**

```cpp
enum AutomationType {
  NONE = 0,           // Sem automação
  SCHEDULE = 1,       // Por horário
  TEMP_HIGH = 2,      // Temperatura alta (chiller)
  TEMP_LOW = 3,       // Temperatura baixa (aquecedor)
  PH_HIGH = 4,        // pH alto (dosadora ácido)
  PH_LOW = 5,         // pH baixo (dosadora base)
  ORP_HIGH = 6,       // ORP alto (parar ozônio)
  ORP_LOW = 7,        // ORP baixo (ligar ozônio)
  LEVEL_HIGH = 8,     // Nível alto (parar reposição)
  LEVEL_LOW = 9,      // Nível baixo (ligar reposição)
  PHOTOPERIOD = 10    // Fotoperíodo (iluminação)
};

struct AutomationConfig {
  AutomationType type = NONE;
  
  // Configurações de horário
  int scheduleOnHour = 8;
  int scheduleOnMinute = 0;
  int scheduleOffHour = 22;
  int scheduleOffMinute = 0;
  bool weekdaysOnly = false;
  
  // Configurações de sensores
  float triggerValue = 0.0;      // Valor para ligar
  float resetValue = 0.0;        // Valor para desligar
  float hysteresis = 0.5;        // Histerese
  
  // Configurações de segurança
  int minCycleTime = 60;         // Tempo mínimo entre acionamentos (segundos)
  int maxRunTime = 300;          // Tempo máximo de funcionamento (segundos)
  
  // Estado da automação
  bool enabled = true;
  bool manualOverride = false;
  unsigned long lastTrigger = 0;
  unsigned long lastReset = 0;
};

struct OutputConfig {
  char name[32] = "Saída";
  int pin = 5;
  bool enabled = true;
  bool manualMode = false;       // Modo manual (bypass automação)
  bool currentState = false;     // Estado atual
  
  AutomationConfig automation;   // Configuração de automação
  
  // Estatísticas
  unsigned long totalOnTime = 0;
  unsigned long cycleCount = 0;
  unsigned long lastStateChange = 0;
};
```

### **Mapeamento de GPIOs (Wemos D1 Mini)**

```cpp
// GPIOs disponíveis para saídas
const int AVAILABLE_GPIOS[8] = {
  5,   // D1 - Recomendado
  4,   // D2 - Recomendado  
  14,  // D5 - Recomendado
  12,  // D6 - Recomendado
  13,  // D7 - Recomendado
  15,  // D8 - Cuidado (pull-down)
  0,   // D3 - Cuidado (boot)
  2    // D4 - Cuidado (boot + LED)
};

// GPIOs reservados para sensores
const int SENSOR_GPIOS[4] = {
  A0,  // ADC - pH, TDS
  // I2C reservado para display
  // D1 (GPIO5) - SCL
  // D2 (GPIO4) - SDA
};
```

---

## 📋 **CHECKLIST DE IMPLEMENTAÇÃO**

### **FASE 1 - v2.0.12**
- [ ] **Frontend**: Ícones padronizados
- [ ] **Frontend**: Botões Liga/Desliga
- [ ] **Frontend**: Validação GPIO visual
- [ ] **Backend**: Validação GPIO na API
- [ ] **Teste**: Validar todas as funcionalidades
- [ ] **Deploy**: Compilar e testar no hardware

### **FASE 2 - v2.1.0**
- [ ] **Backend**: Expandir ConfigManager para 8 saídas
- [ ] **Backend**: Expandir RelayController para 8 saídas
- [ ] **Backend**: Atualizar APIs para 8 saídas
- [ ] **Frontend**: Interface para 8 saídas
- [ ] **Frontend**: Layout responsivo
- [ ] **Validação**: Testes com 8 saídas
- [ ] **Deploy**: Compilar e testar no hardware

### **FASE 3 - v2.2.0**
- [ ] **Backend**: Estrutura de automação
- [ ] **Backend**: AutomationManager
- [ ] **Backend**: Lógica de controle
- [ ] **Backend**: APIs de automação
- [ ] **Frontend**: Interface de automação
- [ ] **Frontend**: Configuração dinâmica
- [ ] **Integração**: Testes de automação
- [ ] **Deploy**: Compilar e testar no hardware

### **FASE 4 - v2.3.0**
- [ ] **Frontend**: Dashboard de automação
- [ ] **Frontend**: Sistema de logs
- [ ] **Frontend**: Melhorias UX
- [ ] **Backend**: Sistema de logs
- [ ] **Integração**: Testes completos
- [ ] **Deploy**: Versão final

---

## ⚠️ **RISCOS E MITIGAÇÕES**

### **Riscos Identificados**
1. **Memória**: Expansão pode consumir muita RAM
2. **Performance**: 8 saídas podem impactar performance
3. **Complexidade**: Sistema de automação muito complexo
4. **Hardware**: Limitações do ESP8266

### **Mitigações**
1. **Otimização**: Usar estruturas eficientes
2. **Profiling**: Monitorar uso de memória
3. **Modularidade**: Implementar em etapas
4. **Testes**: Validar em hardware real

---

## 📊 **MÉTRICAS DE SUCESSO**

### **Funcionalidade**
- [ ] 8 saídas funcionando independentemente
- [ ] Automação respondendo aos sensores
- [ ] Interface responsiva em mobile/desktop
- [ ] Configurações persistindo corretamente

### **Performance**
- [ ] Tempo de resposta < 2s para mudanças
- [ ] Uso de RAM < 80% do disponível
- [ ] Interface fluida sem travamentos
- [ ] Automação executando em < 1s

### **Usabilidade**
- [ ] Configuração intuitiva
- [ ] Feedback visual claro
- [ ] Recuperação de erros
- [ ] Documentação clara

---

## 🎯 **PRÓXIMOS PASSOS**

### **Imediato**
1. **Aprovar roadmap** com o usuário
2. **Definir prioridades** das fases
3. **Começar Fase 1** (melhorias básicas)

### **Curto Prazo**
1. **Implementar Fase 1** (2-3 horas)
2. **Testar no hardware** real
3. **Coletar feedback** do usuário

### **Médio Prazo**
1. **Implementar Fase 2** (expansão 8 saídas)
2. **Validar arquitetura** expandida
3. **Preparar Fase 3** (automação)

### **Longo Prazo**
1. **Sistema completo** funcionando
2. **Documentação** completa
3. **Versão estável** para produção

---

## 📞 **PONTOS DE DECISÃO**

### **Decisão 1: Priorização**
- **Opção A**: Implementar todas as fases sequencialmente
- **Opção B**: Focar apenas nas fases 1 e 2
- **Opção C**: Implementar funcionalidades específicas

### **Decisão 2: Complexidade**
- **Opção A**: Sistema completo com todas as automações
- **Opção B**: Sistema básico com automações essenciais
- **Opção C**: Sistema modular com automações opcionais

### **Decisão 3: Interface**
- **Opção A**: Interface única para todas as configurações
- **Opção B**: Interfaces separadas por funcionalidade
- **Opção C**: Wizard de configuração guiado

---

**🚀 RECOMENDAÇÃO FINAL**

Começar com a **FASE 1** para validar as melhorias básicas, depois partir para a **FASE 2** (8 saídas) que é o maior valor agregado. A **FASE 3** (automação) pode ser implementada gradualmente conforme a necessidade.

**Próxima ação**: Implementar FASE 1 - Melhorias Básicas (v2.0.12) 🎯 