#!/usr/bin/env python3
"""
Script para upload dos arquivos HTML para ESP8266 via LittleFS
Copie todas as páginas HTML de src/html/ para data/html/
Execute: python data/upload_files.py
"""

import os
import shutil

def copy_html_files():
    # Diretórios
    src_dir = "src/html"
    dest_dir = "data/html"
    
    # Criar diretório de destino se não existir
    os.makedirs(dest_dir, exist_ok=True)
    
    # Copiar todos os arquivos HTML
    if os.path.exists(src_dir):
        for filename in os.listdir(src_dir):
            if filename.endswith('.html'):
                src_file = os.path.join(src_dir, filename)
                dest_file = os.path.join(dest_dir, filename)
                shutil.copy2(src_file, dest_file)
                print(f"Copiado: {filename}")
        
        print(f"\n✅ Arquivos copiados para {dest_dir}")
        print("\nPróximos passos:")
        print("1. pio run --target uploadfs (para upload do sistema de arquivos)")
        print("2. pio run --target upload (para upload do firmware)")
    else:
        print(f"❌ Diretório {src_dir} não encontrado!")

if __name__ == "__main__":
    copy_html_files() 