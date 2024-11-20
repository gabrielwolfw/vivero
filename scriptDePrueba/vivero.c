#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/ioctl.h>

// Variable global para el descriptor del puerto serial
int serial_port = -1;

// Manejador de señales para cerrar el puerto serial al recibir SIGINT
void handle_sigint(int sig) {
    if (serial_port != -1) {
        // Mantener DTR y RTS en alto antes de cerrar
        int modemBits = TIOCM_DTR | TIOCM_RTS;
        if (ioctl(serial_port, TIOCMBIS, &modemBits) < 0) {
            perror("Error al mantener DTR y RTS en alto antes de cerrar");
        }
        printf("\nCerrando el puerto serial...\n");
        close(serial_port);
    }
    printf("Programa terminado por el usuario.\n");
    exit(0);
}

// Función para configurar el puerto serial
int configure_serial_port(int serial_port_fd) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    // Leer las configuraciones actuales del puerto
    if (tcgetattr(serial_port_fd, &tty) != 0) {
        perror("Error al obtener atributos del puerto");
        return -1;
    }

    // Configurar baud rate
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    // Configurar parámetros de control
    tty.c_cflag &= ~PARENB; // Sin paridad
    tty.c_cflag &= ~CSTOPB; // Un bit de parada
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;     // 8 bits de datos
    tty.c_cflag &= ~HUPCL;  // Desactivar hang-up-on-close
    tty.c_cflag |= CLOCAL;  // Ignorar las líneas de control del módem

    // Desactivar control de flujo
    tty.c_cflag &= ~CRTSCTS;

    // Modo de lectura
    tty.c_cflag |= CREAD; // Activar lectura

    // Opciones de entrada
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Desactivar control de flujo por software

    // Opciones de control de línea
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Modo raw

    // Opciones de salida
    tty.c_oflag &= ~OPOST; // Sin procesamiento de salida

    // Configurar VMIN y VTIME para que read() bloquee hasta recibir al menos 1 byte
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;

    // Aplicar configuraciones
    if (tcsetattr(serial_port_fd, TCSANOW, &tty) != 0) {
        perror("Error al establecer atributos del puerto");
        return -1;
    }

    // Flush de los buffers
    tcflush(serial_port_fd, TCIOFLUSH);

    return 0;
}

// Función para enviar comandos con reintentos
int send_command(int serial_port_fd, const char *command, int max_retries) {
    int attempt = 0;
    int n;

    while (attempt < max_retries) {
        n = write(serial_port_fd, command, strlen(command));
        if (n < 0) {
            perror("Error al escribir en el puerto serial");
            attempt++;
            printf("Reintentando escribir en el puerto serial (%d/%d)...\n", attempt, max_retries);
            usleep(500000); // Esperar 500 ms antes de reintentar
        } else {
            // Escribir el carácter de nueva línea
            n = write(serial_port_fd, "\n", 1);
            if (n < 0) {
                perror("Error al escribir el carácter de nueva línea en el puerto serial");
                attempt++;
                printf("Reintentando escribir el carácter de nueva línea (%d/%d)...\n", attempt, max_retries);
                usleep(500000);
            } else {
                printf("Comando enviado: %s\n", command);
                return 0; // Éxito
            }
        }
    }

    return -1; // Fallo después de reintentos
}

int main() {
    // Registrar el manejador de señales para SIGINT (Ctrl + C)
    signal(SIGINT, handle_sigint);

    // Nombre del puerto serial
    const char *portname = "/dev/cu.usbmodem141301"; // Reemplaza con el puerto serial

    // Comando a enviar
    const char *command = "Cerrar tubo"; // "Cerrar tubo" o "Abrir tubo"

    // Abrir el puerto serial en modo lectura/escritura y no controlar el terminal
    serial_port = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
    // O_NOCTTY: No hacer que este dispositivo serial sea el terminal controlador del proceso
    // O_NDELAY: No bloquear la apertura del puerto

    // Verificar si el puerto se abrió correctamente
    if (serial_port < 0) {
        perror("Error al abrir el puerto serie");
        return 1;
    }

    // Establecer el descriptor de archivo en modo bloqueante
    int flags = fcntl(serial_port, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    if (fcntl(serial_port, F_SETFL, flags) != 0) {
        perror("Error al establecer el puerto en modo bloqueante");
        close(serial_port);
        return 1;
    }

    // Establecer DTR y RTS en alto para evitar reiniciar el Arduino al abrir el puerto
    int modemBits = TIOCM_DTR | TIOCM_RTS;
    if (ioctl(serial_port, TIOCMBIS, &modemBits) < 0) {
        perror("Error al establecer DTR y RTS en alto");
        close(serial_port);
        return 1;
    }

    // Configurar el puerto serial
    if (configure_serial_port(serial_port) != 0) {
        close(serial_port);
        return 1;
    }

    // Agregar retraso para permitir que el Arduino se estabilice
    printf("Esperando 2 segundos para que el Arduino se estabilice...\n");
    usleep(2000000); // 2 segundos

    // Enviar el comando al Arduino con hasta 3 reintentos
    if (send_command(serial_port, command, 3) != 0) {
        printf("No se pudo enviar el comando después de varios intentos. Saliendo...\n");
        close(serial_port);
        return 1;
    }

    // Búfer para leer datos del Arduino
    char read_buf[256];
    int buf_pos = 0;

    printf("Esperando lecturas del Arduino...\n");

    while (1) {
        char c;
        int n = read(serial_port, &c, 1);

        if (n < 0) {
            perror("Error al leer del puerto");
            // No salir del programa, intentar continuar
            usleep(10000); // Esperar 10 ms antes de continuar
            continue;
        } else if (n == 0) {
            // No hay datos disponibles, esperar un momento
            usleep(10000); // 10 milisegundos
            continue;
        } else {
            // Si leímos un carácter, procesarlo
            if (c == '\n') {
                // Fin de línea, procesar el mensaje
                read_buf[buf_pos] = '\0'; // Terminar la cadena

                // Procesar los datos recibidos
                float humedadSuelo, temperatura, humedadAmbiental;
                int parsed = sscanf(read_buf, "%f,%f,%f", &humedadSuelo, &temperatura, &humedadAmbiental);

                if (parsed == 3) {
                    printf("Humedad de suelo: %.2f %%\n", humedadSuelo);
                    printf("Temperatura: %.2f °C\n", temperatura);
                    printf("Humedad ambiental: %.2f %%\n\n", humedadAmbiental);
                } else {
                    // nada
                }

                // Reiniciar el búfer
                buf_pos = 0;
                memset(&read_buf, '\0', sizeof(read_buf));
            } else {
                // Almacenar el carácter en el búfer
                if (buf_pos < sizeof(read_buf) - 1) {
                    read_buf[buf_pos++] = c;
                } else {
                    // Búfer lleno, reiniciarlo
                    printf("Búfer lleno, reiniciando\n");
                    buf_pos = 0;
                    memset(&read_buf, '\0', sizeof(read_buf));
                }
            }
        }
    }

    // Nunca llegará aquí debido al bucle infinito, pero es buena práctica
    close(serial_port);
    return 0;
}
