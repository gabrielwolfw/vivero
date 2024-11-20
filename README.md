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
En la carpeta Driver/Character_device se encuentran las instrucciones de instalación

