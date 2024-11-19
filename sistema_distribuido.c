#include "sistema_distribuido.h"
#include "seguridad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Función para el nodo de recolección (Rank 0)
void nodo_recoleccion() {
    SensorData data;
    DatosCifrados datos_cifrados;
    
    // Simulamos lectura de sensores
    data.temperatura = 25.0;
    data.humedad = 65.0;
    data.timestamp = time(NULL);
    
    printf("\nDatos originales a enviar:\n");
    printf("Temperatura: %.2f°C\n", data.temperatura);
    printf("Humedad: %.2f%%\n", data.humedad);
    printf("Timestamp: %ld\n", (long)data.timestamp);
    
    // Cifrar datos usando AES
    int result = cifrar_aes((unsigned char*)&data, sizeof(SensorData), &datos_cifrados);
    if (result != 0) {
        fprintf(stderr, "Error cifrando datos con AES: %d\n", result);
        return;
    }
    
    printf("\nDatos cifrados exitosamente:\n");
    printf("Longitud: %d bytes\n", datos_cifrados.length);
    printf("IV: %02X %02X %02X %02X\n", 
           datos_cifrados.iv[0], datos_cifrados.iv[1],
           datos_cifrados.iv[2], datos_cifrados.iv[3]);
    
    // Enviar datos cifrados al nodo de cálculos
    MPI_Send(&datos_cifrados.length, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    MPI_Send(datos_cifrados.iv, IV_SIZE, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
    MPI_Send(datos_cifrados.data, datos_cifrados.length, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
    
    printf("Datos enviados al nodo de cálculos\n");
    free(datos_cifrados.data);
}

// Función para el nodo de cálculos (Rank 1)
void nodo_calculos() {
    DatosCifrados datos_recibidos, datos_a_enviar;
    SensorData data;
    CalculatedData results;
    int longitud_descifrada;
    
    // Recibir datos cifrados
    MPI_Recv(&datos_recibidos.length, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(datos_recibidos.iv, IV_SIZE, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    datos_recibidos.data = (unsigned char*)malloc(datos_recibidos.length);
    MPI_Recv(datos_recibidos.data, datos_recibidos.length, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    printf("\nDatos cifrados recibidos del nodo recolección\n");
    
    // Descifrar datos
    if (descifrar_aes(&datos_recibidos, (unsigned char*)&data, &longitud_descifrada) != 0) {
        fprintf(stderr, "Error descifrando datos\n");
        free(datos_recibidos.data);
        return;
    }
    
    printf("Datos descifrados exitosamente:\n");
    printf("Temperatura: %.2f°C\n", data.temperatura);
    printf("Humedad: %.2f%%\n", data.humedad);
    
    // Realizar cálculos
    results.vpd = calcularVPD(data.temperatura, data.humedad);
    results.indiceCalor = calcularIndiceCalor((data.temperatura * 9/5) + 32, data.humedad);
    results.puntoRocio = calcularPuntoRocio(data.temperatura, data.humedad);
    results.timestamp = data.timestamp;
    
    printf("\nCálculos realizados:\n");
    printf("VPD: %.2f\n", results.vpd);
    printf("Índice de calor: %.2f\n", results.indiceCalor);
    printf("Punto de rocío: %.2f\n", results.puntoRocio);
    
    // Cifrar resultados
    if (cifrar_aes((unsigned char*)&results, sizeof(CalculatedData), &datos_a_enviar) != 0) {
        fprintf(stderr, "Error cifrando resultados\n");
        free(datos_recibidos.data);
        return;
    }
    
    // Enviar resultados cifrados al nodo de predicciones
    MPI_Send(&datos_a_enviar.length, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
    MPI_Send(datos_a_enviar.iv, IV_SIZE, MPI_BYTE, 2, 0, MPI_COMM_WORLD);
    MPI_Send(datos_a_enviar.data, datos_a_enviar.length, MPI_BYTE, 2, 0, MPI_COMM_WORLD);
    
    // Guardar resultados en CSV
    FILE *fp = fopen("calculated_data.csv", "a");
    if (fp != NULL) {
        if (ftell(fp) == 0) {
            fprintf(fp, "Timestamp,VPD,IndiceCalor,PuntoRocio\n");
        }
        fprintf(fp, "%ld,%.2f,%.2f,%.2f\n", 
                (long)results.timestamp, results.vpd, 
                results.indiceCalor, results.puntoRocio);
        fclose(fp);
        printf("\nResultados guardados en calculated_data.csv\n");
    }
    
    free(datos_recibidos.data);
}

// Función para el nodo de predicciones (Rank 2)
void nodo_predicciones() {
    DatosCifrados datos_recibidos;
    CalculatedData calc_data;
    WeatherForecast forecast;
    int longitud_descifrada;
    
    // Recibir datos cifrados
    MPI_Recv(&datos_recibidos.length, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(datos_recibidos.iv, IV_SIZE, MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    datos_recibidos.data = (unsigned char*)malloc(datos_recibidos.length);
    MPI_Recv(datos_recibidos.data, datos_recibidos.length, MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    // Descifrar datos
    if (descifrar_aes(&datos_recibidos, (unsigned char*)&calc_data, &longitud_descifrada) != 0) {
        fprintf(stderr, "Error descifrando datos en nodo predicciones\n");
        free(datos_recibidos.data);
        return;
    }
    
    printf("\nObteniendo pronóstico del clima...\n");
    obtener_pronostico(&forecast);
    
    // Manejar la fecha
    time_t tiempo_actual = (time_t)calc_data.timestamp;
    struct tm *tiempo_info = localtime(&tiempo_actual);
    strftime(forecast.fecha, 20, "%Y-%m-%d", tiempo_info);
    
    // Guardar datos en CSV
    FILE *fp = fopen("forecast_data.csv", "a");
    if (fp != NULL) {
        if (ftell(fp) == 0) {
            fprintf(fp, "Fecha,Temperatura,ProbabilidadLluvia,CondicionClima,Momento,VPD,IndiceCalor,PuntoRocio\n");
        }
        fprintf(fp, "%s,%.2f,%d,%s,%s,%.2f,%.2f,%.2f\n", 
                forecast.fecha,
                forecast.tempPronostico,
                forecast.probabilidadLluvia,
                forecast.condicionClima,
                forecast.esHoraDia ? "Día" : "Noche",
                calc_data.vpd,
                calc_data.indiceCalor,
                calc_data.puntoRocio);
        fclose(fp);
        printf("\nDatos guardados en forecast_data.csv\n");
    }
    
    free(datos_recibidos.data);
}