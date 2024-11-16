# vivero
OS

# Dependencias del driver:
```bash
sudo apt update && sudo apt upgrade  -y
sudo apt install -y build-essential
sudo apt-get install -y linux-headers-$(uname -r)
```

# Permisos para USB / Arduino
```bash
sudo usermod -a -G dialout username
```

# Driver serial
En la carpeta Character_device está la versión que usa un dispositivo de caracter para acceder al arduino, actualemente solo puede escribir

# Driver USB
En la carpeta USB_device está la versión que usa la interfaz USB para comunicarse con el arduino, actualmente solo puede escribir y la opción de leer está dando errores

