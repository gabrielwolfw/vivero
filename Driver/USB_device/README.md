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

Antes de compilar y cargar el driver, se debe quitar el driver por defecto para USB con, después conectar el Arduino:
```bash
sudo rmmod cdc_acm
```
Después ejecutar:

```bash 
make all
make load
```
 Verificar que aparece el mensaje: 
 - USB Device Inserted. Probing Arduino. 
 - USB Arduino device now attached to /dev/ard#

```bash 
make log
```

# Desinstalar el driver
```bash
make remove
sudo rm /dev/vivero
```

