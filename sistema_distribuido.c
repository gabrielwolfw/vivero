#include "sistema_distribuido.h"

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
    if (fp != NULL) {
        fprintf(fp, "%f,%f,%f\n", data.timestamp, data.temperatura, data.humedad);
        fclose(fp);
    } else {
        fprintf(stderr, "Error al abrir sensor_data.csv\n");
    }
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
    if (fp != NULL) {
        fprintf(fp, "%f,%f,%f,%f\n", 
                results.timestamp, results.vpd, 
                results.indiceCalor, results.puntoRocio);
        fclose(fp);
    } else {
        fprintf(stderr, "Error al abrir calculated_data.csv\n");
    }
}

// Función para el nodo de predicciones (Rank 2)
void nodo_predicciones() {
    CalculatedData calc_data;
    WeatherForecast forecast;
    time_t tiempo_actual;
    struct tm *tiempo_info;
    
    // Recibimos datos calculados del nodo anterior
    MPI_Recv(&calc_data, sizeof(CalculatedData), MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    // Obtener pronóstico
    printf("\nNodo de predicciones: Obteniendo pronóstico del clima...\n");
    obtener_pronostico(&forecast);
    
    // Manejar la fecha
    tiempo_actual = (time_t)calc_data.timestamp;
    tiempo_info = localtime(&tiempo_actual);
    strftime(forecast.fecha, 20, "%Y-%m-%d", tiempo_info);
    
    // Mostrar resultados en consola
    printf("\nResultados del pronóstico:\n");
    printf("Fecha: %s\n", forecast.fecha);
    printf("Temperatura: %.2f°C\n", forecast.tempPronostico);
    printf("Condición: %s\n", forecast.condicionClima);
    printf("Probabilidad de lluvia: %d%%\n", forecast.probabilidadLluvia);
    printf("Momento del día: %s\n", forecast.esHoraDia ? "Día" : "Noche");
    
    // Guardar datos en CSV
    FILE *fp = fopen("forecast_data.csv", "a");
    if (fp != NULL) {
        // Verificar si el archivo está vacío para escribir encabezados
        fseek(fp, 0, SEEK_END);
        if (ftell(fp) == 0) {
            fprintf(fp, "Fecha,Temperatura,ProbabilidadLluvia,CondicionClima,Momento,VPD,IndiceCalor,PuntoRocio\n");
        }
        
        // Escribir datos incluyendo los cálculos recibidos del nodo anterior
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
    } else {
        fprintf(stderr, "Error al abrir forecast_data.csv\n");
    }
}