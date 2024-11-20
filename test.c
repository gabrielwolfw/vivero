#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "prediccion_clima.h"

// Estructura simplificada para pruebas
typedef struct {
    float vpd;
    float indiceCalor;
    float puntoRocio;
    float timestamp;
} CalculatedData;

int main() {
    // Crear estructuras necesarias
    CalculatedData calc_data;
    WeatherForecast forecast;
    time_t tiempo_actual;
    struct tm *tiempo_info;
    
    // Simular datos calculados
    calc_data.timestamp = time(NULL);
    
    // Obtener pronóstico
    printf("Obteniendo pronóstico del clima...\n");
    obtener_pronostico(&forecast);
    
    // Manejar la fecha
    tiempo_actual = (time_t)calc_data.timestamp;
    tiempo_info = localtime(&tiempo_actual);
    strftime(forecast.fecha, 20, "%Y-%m-%d", tiempo_info);
    
    // Mostrar resultados en consola
    printf("\nResultados del pronóstico:\n");
    printf("Fecha: %s\n", forecast.fecha);
    printf("Temperatura: %.2f°C\n", forecast.tempPronostico);
    printf("Humedad: %.2f%%\n", forecast.humedadPronostico);
    
    // Guardar datos en CSV
    FILE *fp = fopen("forecast_data.csv", "a");
    if (fp != NULL) {
        // Primero, si el archivo está vacío, escribimos los encabezados
        fseek(fp, 0, SEEK_END);
        if (ftell(fp) == 0) {
            fprintf(fp, "Fecha,Temperatura,ProbabilidadLluvia,CondicionClima,Momento\n");
        }
        
        // Luego escribimos los datos
        fprintf(fp, "%s,%.2f,%d,%s,%s\n", 
                forecast.fecha,
                forecast.tempPronostico,
                forecast.probabilidadLluvia,
                forecast.condicionClima,
                forecast.esHoraDia ? "Día" : "Noche");
        
        fclose(fp);
        printf("\nDatos guardados en forecast_data.csv\n");
    } else {
        fprintf(stderr, "Error al abrir forecast_data.csv\n");
    }
    
    return 0;
}