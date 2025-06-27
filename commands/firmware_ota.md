# Como gerar o firmware.bin para OTA

Após compilar o projeto, o arquivo firmware.bin estará disponível em:

```
.pio/build/d1_mini/firmware.bin
```

Use esse arquivo para fazer upload via a página OTA do seu ESP8266 (AquaControl).

1. Acesse http://<ip_do_esp>/ota
2. Selecione o firmware.bin
3. Clique em Enviar
4. Aguarde o reboot automático

Pronto para testar OTA!
