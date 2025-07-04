# **ROADMAP DE REFATORAÇÃO ONEWIRE - IMPLEMENTAÇÃO CONTROLADA**

## **OBJETIVO**
Centralizar a configuração de pinos OneWire no `platformio.ini` e eliminar redundâncias, com validação rigorosa a cada passo.

---

## **PASSO 1: ANÁLISE E BACKUP**
### **Ações:**
1. **Backup completo** do projeto atual
2. **Documentar** todas as ocorrências de `ONE_WIRE_BUS` e `TEMP_SENSOR_PIN`
3. **Mapear** dependências e arquivos afetados

### **Validação:**
- [ ] Backup criado e testado
- [ ] Lista completa de arquivos afetados
- [ ] Código atual compila sem erros
- [ ] Sensores funcionam corretamente

### **Critério de Aprovação:**
✅ **APROVADO APENAS** se o sistema atual estiver 100% funcional

---

## **PASSO 2: BUSCA GLOBAL E MAPEAMENTO**
### **Ações:**
1. **Buscar** todas as ocorrências de `ONE_WIRE_BUS` no código
2. **Buscar** todas as ocorrências de `TEMP_SENSOR_PIN` no código
3. **Identificar** qual definição está sendo usada em cada arquivo

### **Validação:**
- [ ] Lista completa de arquivos que usam `ONE_WIRE_BUS`
- [ ] Lista completa de arquivos que usam `TEMP_SENSOR_PIN`
- [ ] Mapeamento de dependências entre arquivos

### **Critério de Aprovação:**
✅ **APROVADO APENAS** se tivermos mapeamento 100% completo

---

## **PASSO 3: TESTE ISOLADO - TEMP_SENSOR_PIN**
### **Ações:**
1. **Criar branch** específico para teste
2. **Alterar APENAS** o construtor do `SensorManager`:
   ```cpp
   // DE: _oneWire(ONE_WIRE_BUS)
   // PARA: _oneWire(TEMP_SENSOR_PIN)
   ```
3. **Manter** todas as outras definições

### **Validação:**
- [ ] Código compila sem erros
- [ ] ESP8266 usa GPIO4 corretamente
- [ ] ESP32 usa GPIO32 corretamente
- [ ] Sensores OneWire funcionam
- [ ] Debug mostra pino correto

### **Critério de Aprovação:**
✅ **APROVADO APENAS** se sensores funcionarem perfeitamente

---

## **PASSO 4: LIMPEZA CONSTANTS.H**
### **Ações:**
1. **Remover APENAS** as definições de `ONE_WIRE_BUS` do `Constants.h`
2. **Manter** todas as outras definições intactas
3. **Não alterar** mais nada

### **Validação:**
- [ ] Código compila sem erros
- [ ] Sensores continuam funcionando
- [ ] Não há regressão de funcionalidade
- [ ] Debug continua mostrando pino correto

### **Critério de Aprovação:**
✅ **APROVADO APENAS** se não houver impacto negativo

---

## **PASSO 5: LIMPEZA SENSORMANAGER.H**
### **Ações:**
1. **Remover APENAS** as definições duplicadas de `ONE_WIRE_BUS`
2. **Manter** definições de outros pinos (PH_TDS_PIN, LEVEL_PIN)
3. **Não alterar** mais nada

### **Validação:**
- [ ] Código compila sem erros
- [ ] Sensores OneWire funcionam
- [ ] Outros sensores não afetados
- [ ] Sistema completo funcional

### **Critério de Aprovação:**
✅ **APROVADO APENAS** se sistema completo funcionar

---

## **PASSO 6: SUBSTITUIÇÃO GLOBAL**
### **Ações:**
1. **Substituir** todas as ocorrências restantes de `ONE_WIRE_BUS` por `TEMP_SENSOR_PIN`
2. **Atualizar** mensagens de debug
3. **Verificar** se não há referências perdidas

### **Validação:**
- [ ] Código compila sem erros
- [ ] Busca por `ONE_WIRE_BUS` retorna 0 resultados
- [ ] Sensores funcionam perfeitamente
- [ ] Debug mostra informações corretas

### **Critério de Aprovação:**
✅ **APROVADO APENAS** se não existir mais `ONE_WIRE_BUS` no código

---

## **PASSO 7: TESTE FINAL E VALIDAÇÃO**
### **Ações:**
1. **Compilar** ambas as versões (Compact e Pro)
2. **Testar** em hardware real
3. **Validar** leitura de sensores
4. **Verificar** logs de debug

### **Validação:**
- [ ] ESP8266 (Compact) usa GPIO4
- [ ] ESP32 (Pro) usa GPIO32
- [ ] Sensores DS18B20 funcionam
- [ ] Scan OneWire funciona
- [ ] Interface web mostra dados corretos

### **Critério de Aprovação:**
✅ **APROVADO APENAS** se tudo funcionar perfeitamente

---

## **ROLLBACK PLAN**
### **Em caso de falha em qualquer passo:**
1. **Parar imediatamente**
2. **Restaurar** backup do passo anterior
3. **Analisar** causa da falha
4. **Corrigir** problema antes de continuar

---

## **CRITÉRIOS DE SUCESSO FINAL**
- ✅ Uma única definição de pino OneWire (no platformio.ini)
- ✅ Zero redundâncias no código
- ✅ Funcionamento perfeito em ambas as versões
- ✅ Código mais limpo e maintível

**REGRA DE OURO: NUNCA AVANÇAR SEM APROVAÇÃO DO PASSO ANTERIOR**

---

## **SITUAÇÃO ATUAL (BAGUNÇA)**
```
platformio.ini → TEMP_SENSOR_PIN=4 (ESP8266) / TEMP_SENSOR_PIN=32 (ESP32)
Constants.h    → ONE_WIRE_BUS=4 (força GPIO4 sempre)
SensorManager.h → ONE_WIRE_BUS=4 (duplica novamente)
```

## **RESULTADO ESPERADO**
```
platformio.ini → TEMP_SENSOR_PIN=4 (ESP8266) / TEMP_SENSOR_PIN=32 (ESP32)
Constants.h    → (sem definição OneWire)
SensorManager.h → (sem definição OneWire)
Código usa     → TEMP_SENSOR_PIN diretamente
``` 