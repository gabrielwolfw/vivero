# vivero
OS

# Dependencias del driver:
```bash
sudo apt update && sudo apt upgrade  -y
sudo apt install -y build-essential
sudo apt-get install -y linux-headers-$(uname -r)
```

# Permisos para USB
```bash
sudo usermod -a -G dialout username
```

# Instalación del driver

Antes de compilar y cargar el driver, se debe quitar el driver por defecto para USB con:
```bash
sudo rmmod cdc_acm
```
Despues 
```bash 
make all
make load
```
Después conectar el Arduino, y verificar que aparece el mensaje: USB Device Inserted. Probing Arduino. 

```bash 
make log
```

# Desinstalar el driver
```bash
make remove
sudo rm /dev/vivero
```

