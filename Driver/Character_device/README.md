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
Conectar arduino y verificar con sudo dmesg si aparece en puerto ttyACM0, ir a la carpeta Driver y ejecutar
```bash 
make all
make load
make log
```
En el log del Kernel, el driver escribe el número de dispositivo que registro, para el siguiente paso utilizar el Major que corresponde

```bash 
sudo mknod /dev/vivero c MAJOR 0
```

El archivo interface_test.c tiene un ejemplo de cómo escribir al driver, al momento los comandos disponibles son TURN_ON_LED y TURN_OFF_LED, para pruebas

# Desinstalar el driver
```bash
make remove
sudo rm /dev/vivero
```

