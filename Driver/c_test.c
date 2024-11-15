#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_PATH "/dev/ttyACM0"

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd == -1) {
        perror("Error abriendo el dispositivo");
        return 1;
    }

    // Enviar un comando ioctl al driver (por ejemplo)
    int ret = ioctl(fd, MY_IOCTL_CMD, NULL);
    if (ret == -1) {
        perror("Error en ioctl");
        close(fd);
        return 1;
    }

    // Usar read() o write() para interactuar con el dispositivo
    // ...

    close(fd);
    return 0;
}
