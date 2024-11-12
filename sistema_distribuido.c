#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "calculos.h"


// Estructuras de datos para compartir entre nodos
typedef struct {
    float temperatura;
    float humedad;
    float timestamp;
} SensorData;

typedef struct {
    float vpd;
    float indiceCalor;
    float puntoRocio;
    float timestamp;
} CalculatedData;

typedef struct {
    float tempPronostico;
    float humedadPronostico;
    char fecha[20];
} WeatherForecast;

// Función para el nodo de recolección (Rank 0)
void nodo_recoleccion() {
    SensorData data;
    // Simulamos lectura de sensores
    data.temperatura = 25.0;  // En grados Celsius
    data.humedad = 65.0;     // Porcentaje
    data.timestamp = time(NULL);
    
    // Enviamos datos al nodo de cálculos
    MPI_Send(&data, sizeof(SensorData), MPI_BYTE, 1, 0, MPI_COMM_WORLD);
    
    // Guardamos datos para Power BI
    FILE *fp = fopen("sensor_data.csv", "a");
    fprintf(fp, "%f,%f,%f\n", data.timestamp, data.temperatura, data.humedad);
    fclose(fp);
}

// Función para el nodo de cálculos (Rank 1)
void nodo_calculos() {
    SensorData data;
    CalculatedData results;
    
    // Recibimos datos del nodo de recolección
    MPI_Recv(&data, sizeof(SensorData), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    // Realizamos cálculos
    results.vpd = calcularVPD(data.temperatura, data.humedad);
    results.indiceCalor = calcularIndiceCalor((data.temperatura * 9/5) + 32, data.humedad);
    results.puntoRocio = calcularPuntoRocio(data.temperatura, data.humedad);
    results.timestamp = data.timestamp;
    
    // Enviamos resultados al nodo de predicciones
    MPI_Send(&results, sizeof(CalculatedData), MPI_BYTE, 2, 0, MPI_COMM_WORLD);
    
    // Guardamos resultados para Power BI
    FILE *fp = fopen("calculated_data.csv", "a");
    fprintf(fp, "%f,%f,%f,%f\n", 
            results.timestamp, results.vpd, 
            results.indiceCalor, results.puntoRocio);
    fclose(fp);
}

// Función para el nodo de predicciones (Rank 2)
void nodo_predicciones() {
    CalculatedData calc_data;
    WeatherForecast forecast;
    
    // Recibimos datos calculados
    MPI_Recv(&calc_data, sizeof(CalculatedData), MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    // TODO: Integrar con API AccuWeather
    // Por ahora simulamos datos de pronóstico
    forecast.tempPronostico = 26.0;
    forecast.humedadPronostico = 70.0;
    strftime(forecast.fecha, 20, "%Y-%m-%d", localtime(&(time_t){calc_data.timestamp}));
    
    // Guardamos predicciones para Power BI
    FILE *fp = fopen("forecast_data.csv", "a");
    fprintf(fp, "%s,%f,%f\n", 
            forecast.fecha, 
            forecast.tempPronostico, 
            forecast.humedadPronostico);
    fclose(fp);
}

int main(int argc, char** argv) {
    int rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (size != 3) {
        printf("Este programa requiere exactamente 3 procesos\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    // Ejecutamos la función correspondiente según el rank
    switch(rank) {
        case 0:
            nodo_recoleccion();
            break;
        case 1:
            nodo_calculos();
            break;
        case 2:
            nodo_predicciones();
            break;
    }
    
    MPI_Finalize();
    return 0;
}