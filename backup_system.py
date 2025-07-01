import os
import sys
import subprocess
import shutil
import zipfile
from datetime import datetime
import json
import time

def check_dependencies():
    """Verifica e instala dependências necessárias"""
    try:
        import tkinter as tk
        print("✓ Tkinter já está instalado")
        return True
    except ImportError:
        print("⚠️ Tkinter não encontrado. Tentando instalar...")
        try:
            subprocess.check_call([sys.executable, "-m", "pip", "install", "-r", "requirements.txt"])
            print("✓ Tkinter instalado com sucesso")
            return True
        except subprocess.CalledProcessError as e:
            print(f"❌ Erro ao instalar dependências: {str(e)}")
            return False

def select_backup_folder():
    """Abre diálogo para selecionar pasta de destino"""
    import tkinter as tk
    from tkinter import filedialog
    
    # Cria uma janela tkinter oculta
    root = tk.Tk()
    root.withdraw()
    
    # Abre diálogo para selecionar pasta
    folder_path = filedialog.askdirectory(
        title='Selecione a pasta para salvar o backup',
        mustexist=True
    )
    
    return folder_path

def safe_remove_tree(path):
    """Remove diretório e seu conteúdo com tratamento de erro"""
    if not os.path.exists(path):
        return
        
    def on_rm_error(func, path, exc_info):
        # Espera um pouco e tenta novamente
        time.sleep(1)
        try:
            os.chmod(path, 0o777)
            func(path)
        except:
            print(f"⚠️ Não foi possível remover: {path}")
    
    try:
        shutil.rmtree(path, onerror=on_rm_error)
    except Exception as e:
        print(f"⚠️ Erro ao limpar pasta temporária: {str(e)}")

def create_backup():
    """Cria backup do sistema"""
    # Solicita pasta de destino
    backup_folder = select_backup_folder()
    if not backup_folder:
        print('❌ Operação cancelada pelo usuário')
        return
    
    # Configurações
    backup_folders = ['data', 'src', 'bin']
    backup_files = ['platformio.ini']
    temp_dir = 'temp_backup'
    
    # Nome do arquivo com timestamp
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    backup_name = f'ReefControl_backup_{timestamp}'
    
    try:
        # Limpa e cria diretório temporário
        safe_remove_tree(temp_dir)
        os.makedirs(temp_dir)
        
        # Copia pastas
        for folder in backup_folders:
            if os.path.exists(folder):
                shutil.copytree(folder, os.path.join(temp_dir, folder))
                print(f'✓ Copiada pasta: {folder}')
            else:
                print(f'⚠️ Pasta não encontrada: {folder}')
        
        # Copia arquivos
        for file in backup_files:
            if os.path.exists(file):
                shutil.copy2(file, temp_dir)
                print(f'✓ Copiado arquivo: {file}')
            else:
                print(f'⚠️ Arquivo não encontrado: {file}')
        
        # Verifica se existe config.json na pasta data
        config_paths = [
            os.path.join('data', 'config.json'),  # ESP8266
            os.path.join('data', 'spiffs', 'config.json'),  # ESP32
        ]
        
        config_found = False
        for config_path in config_paths:
            if os.path.exists(config_path):
                # Cria pasta configs no backup
                configs_dir = os.path.join(temp_dir, 'configs')
                os.makedirs(configs_dir, exist_ok=True)
                
                # Copia config.json para pasta configs
                shutil.copy2(config_path, os.path.join(configs_dir, 'config.json'))
                print(f'✓ Copiado arquivo de configuração: {config_path}')
                config_found = True
                break
        
        if not config_found:
            print('⚠️ Arquivo de configuração não encontrado')
        
        # Cria arquivo de informações
        info = {
            'version': 'v3.2.2-beta',
            'date': datetime.now().isoformat(),
            'folders': backup_folders,
            'files': backup_files,
            'has_config': config_found
        }
        
        with open(os.path.join(temp_dir, 'backup_info.json'), 'w') as f:
            json.dump(info, f, indent=2)
        
        # Cria ZIP na pasta selecionada
        zip_path = os.path.join(backup_folder, f'{backup_name}.zip')
        with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
            for root, dirs, files in os.walk(temp_dir):
                for file in files:
                    file_path = os.path.join(root, file)
                    arcname = os.path.relpath(file_path, temp_dir)
                    zipf.write(file_path, arcname)
        
        print(f'\n✓ Backup criado com sucesso em:')
        print(f'  {zip_path}')
        print(f'✓ Tamanho: {os.path.getsize(zip_path) / (1024*1024):.2f} MB')
        if config_found:
            print('✓ Configurações dos sensores incluídas no backup')
        
    except Exception as e:
        print(f'\n❌ Erro durante backup: {str(e)}')
    
    finally:
        # Limpa diretório temporário
        safe_remove_tree(temp_dir)
        print('✓ Limpeza concluída')

if __name__ == '__main__':
    print('🔄 Iniciando backup do ReefControl...\n')
    
    # Verifica dependências antes de prosseguir
    if check_dependencies():
        create_backup()
    else:
        print("\n❌ Não foi possível continuar devido a dependências faltantes")
        sys.exit(1) 