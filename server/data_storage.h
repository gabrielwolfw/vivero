#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

// Definimos el número máximo de sensores
#define MAX_SENSORS 10

// Estructura para almacenar los datos de un sensor
typedef struct {
    int sensor_id;
    float temperature;
    float humidity;
} SensorData;

// Prototipos de las funciones

// Función para almacenar los datos de un sensor
void store_sensor_data(int sensor_id, float temperature, float humidity);

// Función para imprimir los datos de los sensores almacenados
void print_sensor_data(void);

// Función para obtener los datos de un sensor por su ID
SensorData* get_sensor_data(int sensor_id);

#endif // DATA_STORAGE_H
