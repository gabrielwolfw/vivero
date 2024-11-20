#include <stdio.h>      // Para printf, sscanf
#include <string.h>     // Para memset
#include <unistd.h>     // Para close, usleep
#include <fcntl.h>      // Para open, O_RDWR
#include <termios.h>    // Para configurar el puerto serial
#include <errno.h>      // Para manejo de errores
#include <signal.h>     // Para señales
#include <stdlib.h>     // Para exit
#include <sys/ioctl.h>  // Para ioctl
#include <time.h>       // Para medir tiempo
#include "calculos.h"   // Incluye funciones de cálculos
#include "Driver/Character_device/greenhouse_interface.h" // Interfaz del dispositivo

int serial_port = -1;
time_t tiempo_inicio_riego = 0; // Variable para guardar el tiempo de inicio del riego
int riego_abierto = 0;          // Bandera para saber si el riego está activo

// Manejador de señal para cierre seguro
void handle_sigint(int sig) {
    (void)sig;
    if (serial_port != -1) {
        printf("\nCerrando el puerto serial...\n");
        close(serial_port);
    }
    printf("Programa terminado.\n");
    exit(0);
}

// Configurar puerto serial
int configure_serial_port(int serial_port_fd) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(serial_port_fd, &tty) != 0) {
        perror("Error al obtener atributos del puerto");
        return -1;
    }

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag |= CLOCAL;
    tty.c_cflag |= CREAD;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;

    return tcsetattr(serial_port_fd, TCSANOW, &tty);
}

// Enviar comando al solenoide
int send_command(const char *command) {
    int dev = open("/dev/vivero", O_WRONLY);
    if (dev == -1) {
        perror("Error al abrir el dispositivo");
        return -1;
    }

    int result;
    if (strcmp(command, "Abrir tubo") == 0) {
        result = ioctl(dev, OPEN_WATER, NULL);
    } else if (strcmp(command, "Cerrar tubo") == 0) {
        result = ioctl(dev, CLOSE_WATER, NULL);
    } else {
        printf("Comando desconocido: %s\n", command);
        close(dev);
        return -1;
    }

    if (result < 0) {
        perror("Error al enviar el comando");
    } else {
        printf("Comando enviado: %s\n", command);
    }

    close(dev);
    return result;
}

int main() {
    signal(SIGINT, handle_sigint);

    size_t buf_pos = 0;

    const char *portname = "/dev/ttyACM0";
    serial_port = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);

    if (serial_port < 0) {
        perror("Error al abrir el puerto serie");
        return 1;
    }

    if (configure_serial_port(serial_port) != 0) {
        close(serial_port);
        return 1;
    }

    printf("Esperando datos del sensor...\n");

    char read_buf[256];
    const float umbralVPD = 1.5;
    const float umbralIndiceCalor = 85.0;
    const float umbralHumedadSuelo = 40.0;

    while (1) {
        char c;
        int n = read(serial_port, &c, 1);

        if (n < 0) {
            usleep(10000);
            continue;
        } else if (n == 0) {
            usleep(10000);
            continue;
        } else {
            if (c == '\n') {
                read_buf[buf_pos] = '\0';
                float humedadSuelo, temperatura, humedadAmbiental;
                int parsed = sscanf(read_buf, "%f,%f,%f", &humedadSuelo, &temperatura, &humedadAmbiental);

                if (parsed == 3) {
                    printf("Humedad del suelo: %.2f %%\n", humedadSuelo);
                    printf("Temperatura: %.2f °C\n", temperatura);
                    printf("Humedad ambiental: %.2f %%\n", humedadAmbiental);

                    float vpd = calcularVPD(temperatura, humedadAmbiental);
                    float temperaturaF = temperatura * 9.0 / 5.0 + 32;
                    float indiceCalor = calcularIndiceCalor(temperaturaF, humedadAmbiental);

                    printf("VPD: %.2f\n", vpd);
                    printf("Índice de Calor: %.2f\n", indiceCalor);

                    // Activar o desactivar riego según umbrales
                    if (!riego_abierto && (humedadSuelo < umbralHumedadSuelo || vpd > umbralVPD || indiceCalor > umbralIndiceCalor)) {
                        send_command("Abrir tubo");
                        riego_abierto = 1;
                        tiempo_inicio_riego = time(NULL);
                    } else if (riego_abierto && (time(NULL) - tiempo_inicio_riego >= 30)) {
                        send_command("Cerrar tubo");
                        riego_abierto = 0;
                    }
                }
                buf_pos = 0;
                memset(&read_buf, '\0', sizeof(read_buf));
            } else {
                if (buf_pos < sizeof(read_buf) - 1) {
                    read_buf[buf_pos++] = c;
                } else {
                    buf_pos = 0;
                    memset(&read_buf, '\0', sizeof(read_buf));
                }
            }
        }
    }

    close(serial_port);
    return 0;
}