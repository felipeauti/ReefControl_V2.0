// 🧪 MICRO-FASE 1A - Teste de Conectividade Básica
// Arquivo: test-basic.js

console.log("✅ JavaScript carregado com sucesso!");
console.log("🔗 Conectividade JS → ESP32 funcionando!");

// Função global para teste manual
window.testBasicJS = function() {
    alert("🎉 Conectividade JS funcionando perfeitamente!");
    console.log("✅ Teste manual executado com sucesso!");
    return true;
};

// Teste automático de recursos básicos
window.testJSFeatures = function() {
    const results = {
        console: typeof console !== 'undefined',
        fetch: typeof fetch !== 'undefined',
        localStorage: typeof localStorage !== 'undefined',
        JSON: typeof JSON !== 'undefined',
        Date: typeof Date !== 'undefined'
    };
    
    console.log("🔍 Recursos JavaScript disponíveis:", results);
    
    const allOK = Object.values(results).every(r => r === true);
    if (allOK) {
        console.log("✅ Todos os recursos JS básicos estão disponíveis!");
    } else {
        console.warn("⚠️ Alguns recursos JS não estão disponíveis:", results);
    }
    
    return results;
};

// Auto-execução do teste
document.addEventListener('DOMContentLoaded', function() {
    console.log("📄 DOM carregado, executando testes automáticos...");
    testJSFeatures();
});

// Informações do teste
console.log("📋 MICRO-FASE 1A - Teste de Conectividade");
console.log("🎯 Objetivo: Garantir que arquivos JS são servidos corretamente");
console.log("🧪 Para testar manualmente: testBasicJS()");
console.log("🔍 Para ver recursos: testJSFeatures()"); 