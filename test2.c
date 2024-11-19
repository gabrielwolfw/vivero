#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h> 
#include "sistema_distribuido.h"
#include "seguridad.h"

// Función para simular datos de sensores
SensorData simular_datos_sensor() {
    SensorData data;
    data.temperatura = 22.40;  // Valor fijo para pruebas
    data.humedad = 51.00;      // Valor fijo para pruebas
    data.timestamp = time(NULL);
    return data;
}

int main(int argc, char** argv) {
    int rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 3) {
        if (rank == 0) {
            printf("Error: Este programa requiere exactamente 3 procesos\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Inicializar AES con distribución de clave
    if (rank == 0) {
        printf("\n=== Iniciando sistema distribuido del vivero ===\n");
    }
    inicializar_aes(rank);
    
    // Asegurar que todos los procesos tienen la clave antes de continuar
    MPI_Barrier(MPI_COMM_WORLD);
    
    
    
    if (size != 3) {
        if (rank == 0) {
            printf("Error: Este programa requiere exactamente 3 procesos\n");
        }
        MPI_Finalize();
        return 1;
    }
    
    printf("\n[Proceso %d] Iniciado en %s\n", rank, getenv("HOSTNAME"));
    
    switch(rank) {
        case 0: {
            printf("\n=== NODO DE RECOLECCIÓN ===\n");
            SensorData data = simular_datos_sensor();
            printf("Datos simulados:\n");
            printf("  Temperatura: %.2f°C\n", data.temperatura);
            printf("  Humedad: %.2f%%\n", data.humedad);
            printf("  Timestamp: %ld\n", (long)data.timestamp);
            
            DatosCifrados datos_cifrados;
            int result = cifrar_aes((unsigned char*)&data, sizeof(SensorData), &datos_cifrados);
            
            if (result == 0) {
                printf("\nDatos cifrados exitosamente:\n");
                printf("  Longitud: %d bytes\n", datos_cifrados.length);
                printf("  IV: %02X %02X %02X %02X\n",
                       datos_cifrados.iv[0], datos_cifrados.iv[1],
                       datos_cifrados.iv[2], datos_cifrados.iv[3]);
                printf("  Datos: %02X %02X %02X %02X\n",
                       datos_cifrados.data[0], datos_cifrados.data[1],
                       datos_cifrados.data[2], datos_cifrados.data[3]);
                
                nodo_recoleccion();
                printf("\n[Nodo Recolección] Datos enviados al nodo de cálculos\n");
            } else {
                printf("Error al cifrar datos: %d\n", result);
            }
            break;
        }
        case 1: {
            printf("\n=== NODO DE CÁLCULOS ===\n");
            nodo_calculos();
            printf("\n[Nodo Cálculos] Procesamiento completado\n");
            
            FILE *fp = fopen("calculated_data.csv", "r");
            if (fp != NULL) {
                char buffer[256];
                printf("\nDatos calculados:\n");
                while (fgets(buffer, sizeof(buffer), fp) != NULL) {
                    printf("%s", buffer);
                }
                fclose(fp);
            } else {
                printf("No se pudo abrir calculated_data.csv\n");
            }
            break;
        }
        case 2: {
            printf("\n=== NODO DE PREDICCIONES ===\n");
            nodo_predicciones();
            printf("\n[Nodo Predicciones] Pronóstico completado\n");
            break;
        }
    }
    
    printf("\n[Proceso %d] Finalizado\n", rank);
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (rank == 0) {
        printf("\n=== Fin de la ejecución ===\n\n");
    }
    // Añadir la conexión entre nodos de diferentes maquinas de azure
    MPI_Finalize();
    return 0;
}