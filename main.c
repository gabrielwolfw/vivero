#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "calculos.h" // Incluir el archivo de encabezado de los cálculos

int serial_port = -1;

// Manejador de señal para cierre seguro
void handle_sigint(int sig) {
    (void)sig;
    if (serial_port != -1) {
        int modemBits = TIOCM_DTR | TIOCM_RTS;
        ioctl(serial_port, TIOCMBIS, &modemBits);
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
int send_command(int serial_port_fd, const char *command, int max_retries) {
    int attempt = 0;
    int n;

    while (attempt < max_retries) {
        n = write(serial_port_fd, command, strlen(command));
        if (n >= 0) {
            write(serial_port_fd, "\n", 1);
            printf("Comando enviado: %s\n", command);
            return 0;
        }
        perror("Error al escribir en el puerto");
        attempt++;
        usleep(500000);
    }
    return -1;
}

int main() {
    signal(SIGINT, handle_sigint);

    // Definición de buf_pos como size_t para evitar el conflicto y las advertencias de signo
    size_t buf_pos = 0;

    const char *portname = "/dev/cu.usbmodem141301";
    serial_port = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);

    if (serial_port < 0) {
        perror("Error al abrir el puerto serie");
        return 1;
    }

    int flags = fcntl(serial_port, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    if (fcntl(serial_port, F_SETFL, flags) != 0) {
        perror("Error al establecer el puerto en modo bloqueante");
        close(serial_port);
        return 1;
    }

    if (configure_serial_port(serial_port) != 0) {
        close(serial_port);
        return 1;
    }

    printf("Esperando 2 segundos para estabilización...\n");
    usleep(2000000);

    char read_buf[256];

    // Definir umbrales
    const float umbralVPD = 1.5;  // Ajustar según el contexto
    const float umbralIndiceCalor = 85.0;  // Ajustar en °F
    const float umbralHumedadSuelo = 40.0;  // Porcentaje mínimo de humedad en suelo

    printf("Esperando datos del sensor...\n");

    while (1) {
        char c;
        int n = read(serial_port, &c, 1);

        if (n < 0) {
            perror("Error al leer del puerto");
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

                    // Calcular indicadores
                    float vpd = calcularVPD(temperatura, humedadAmbiental);
                    float temperaturaF = temperatura * 9.0 / 5.0 + 32;
                    float indiceCalor = calcularIndiceCalor(temperaturaF, humedadAmbiental);

                    printf("VPD: %.2f\n", vpd);
                    printf("Índice de Calor: %.2f\n", indiceCalor);

                    // Activar o desactivar riego según umbrales
                    if (humedadSuelo < umbralHumedadSuelo || vpd > umbralVPD || indiceCalor > umbralIndiceCalor) {
                        send_command(serial_port, "Abrir tubo", 3);
                    } else {
                        send_command(serial_port, "Cerrar tubo", 3);
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
