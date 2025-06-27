# 🚀 AquaControl - Comandos GitHub

## 📋 Guia Completo de Comandos Git para o Projeto

### 🔧 Configuração Inicial do Repositório

```bash
# Inicializar repositório git
git init

# Adicionar todos os arquivos
git add .

# Fazer commit inicial
git commit -m "🚀 AquaControl - Sistema OTA Completo com Interface Web Moderna

✅ Funcionalidades implementadas:
- Interface web responsiva e moderna
- Sistema OTA com barra de progresso simulada 
- Redirecionamento automático após upload
- Detecção inteligente de reinício do dispositivo
- Páginas de sucesso e erro personalizadas
- Sistema de fallback manual
- Documentação completa de comandos seriais

🎯 Principais arquivos:
- src/principal.cpp - Interface web e sistema OTA
- src/main.cpp - Código principal do ESP8266  
- DICA_SERIAL.md - Comandos para monitor serial
- platformio.ini - Configuração do projeto

🔧 Hardware: ESP8266 (D1 Mini)
📡 Recursos: WiFiManager, OTA Update, WebServer, NTP"
```

### 🌐 Configuração do Repositório Remoto

```bash
# Adicionar repositório remoto
git remote add origin https://github.com/felipeauti/AquaControl.git

# Fazer push inicial
git push -u origin master
```

### 📊 Comandos de Verificação

```bash
# Verificar status do repositório
git status

# Ver histórico de commits
git log --oneline

# Ver repositórios remotos configurados
git remote -v

# Ver diferenças não commitadas
git diff

# Ver arquivos modificados
git status --porcelain
```

### 🔄 Comandos de Uso Diário

```bash
# Adicionar arquivos específicos
git add arquivo.cpp
git add src/

# Adicionar todos os arquivos modificados
git add .

# Fazer commit com mensagem
git commit -m "Descrição das mudanças"

# Push para o repositório remoto
git push origin master

# Pull do repositório remoto
git pull origin master

# Ver diferenças entre commits
git diff HEAD~1 HEAD
```

### 🌿 Comandos de Branch

```bash
# Criar nova branch
git checkout -b nova-feature

# Trocar de branch
git checkout master
git checkout nova-feature

# Listar branches
git branch

# Merge de branch
git checkout master
git merge nova-feature

# Deletar branch
git branch -d nova-feature
```

### 📝 Comandos de Histórico

```bash
# Ver histórico detalhado
git log

# Ver histórico de um arquivo específico
git log --follow src/principal.cpp

# Ver diferenças de um commit específico
git show c6c873f

# Ver arquivos modificados em um commit
git show --name-only c6c873f
```

### 🔧 Comandos Úteis para PlatformIO

```bash
# Sequência completa: build, upload e commit
python -m platformio run
python -m platformio run --target upload
copy .pio\build\d1_mini\firmware.bin .\AquaControl_v1.0.bin
git add .
git commit -m "Atualização do firmware v1.0"
git push origin master
```

### 🛠️ Comandos de Correção

```bash
# Desfazer último commit (mantém arquivos)
git reset --soft HEAD~1

# Desfazer mudanças não commitadas
git checkout -- arquivo.cpp

# Desfazer todas as mudanças não commitadas
git checkout .

# Alterar mensagem do último commit
git commit --amend -m "Nova mensagem"
```

### 📚 Comandos de Configuração

```bash
# Configurar nome e email (primeira vez)
git config --global user.name "Seu Nome"
git config --global user.email "seu.email@exemplo.com"

# Ver configurações
git config --list

# Configurar editor padrão
git config --global core.editor "code --wait"
```

### 🔍 Comandos de Pesquisa

```bash
# Pesquisar por texto no histórico
git log --grep="OTA"

# Pesquisar por mudanças em código
git log -S "WiFiManager"

# Ver quando uma linha foi modificada
git blame src/principal.cpp
```

### 🏷️ Comandos de Tags

```bash
# Criar tag
git tag v1.0.0

# Criar tag com mensagem
git tag -a v1.0.0 -m "Versão 1.0 - Sistema OTA Completo"

# Listar tags
git tag

# Push de tags
git push origin --tags
```

### 🔐 Comandos para Problemas Comuns

```bash
# Resolver conflitos após pull
git status
# Editar arquivos conflitantes
git add .
git commit -m "Resolvendo conflitos"

# Cancelar merge em andamento
git merge --abort

# Limpar arquivos não rastreados
git clean -fd
```

## 📂 Estrutura do Projeto

```
AquaControl/
├── src/
│   ├── main.cpp              # Código principal
│   ├── principal.cpp         # Interface web e OTA
│   ├── principal.h           # Header principal
│   ├── wifi_manager.cpp      # Gerenciamento WiFi
│   ├── wifi_manager.h        # Header WiFi
│   ├── real_time.cpp         # Funções de tempo real
│   └── real_time.h           # Header tempo real
├── platformio.ini            # Configuração PlatformIO
├── DICA_SERIAL.md           # Comandos serial
├── GITHUB.md                # Este arquivo
└── *.bin                    # Arquivos de firmware
```

## 🎯 Fluxo de Trabalho Recomendado

1. **Desenvolvimento:**
   ```bash
   git checkout -b nova-feature
   # Fazer mudanças
   python -m platformio run
   python -m platformio run --target upload
   ```

2. **Teste e Commit:**
   ```bash
   git add .
   git commit -m "Implementação de nova funcionalidade"
   ```

3. **Merge e Deploy:**
   ```bash
   git checkout master
   git merge nova-feature
   git push origin master
   git tag v1.1.0
   git push origin --tags
   ```

---

## 🔗 Links Úteis

- **Repositório:** https://github.com/felipeauti/AquaControl.git
- **PlatformIO:** https://platformio.org/
- **Git Docs:** https://git-scm.com/doc

---

*Documentação criada para o projeto AquaControl ESP8266* 🚀 