# Comandos no Windows PowerShell

- Para empilhar (executar em sequência) comandos no PowerShell, use o ponto e vírgula `;` ao invés de `&&` (que é usado no bash).

**Exemplo:**
```powershell
comando1 ; comando2
```

- O operador `&&` não é suportado no PowerShell para encadear comandos.

- No CMD, pode-se usar `&&`, mas no PowerShell sempre use `;` para múltiplos comandos.
