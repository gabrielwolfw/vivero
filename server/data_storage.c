#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_storage.h"

// Almacenar los datos de los sensores en memoria (array estático)
SensorData sensor_data[MAX_SENSORS];

// Función para almacenar los datos de un sensor
void store_sensor_data(int sensor_id, float temperature, float humidity) {
    for (int i = 0; i < MAX_SENSORS; i++) {
        if (sensor_data[i].sensor_id == 0) {  // Encuentra un espacio vacío
            sensor_data[i].sensor_id = sensor_id;
            sensor_data[i].temperature = temperature;
            sensor_data[i].humidity = humidity;
            break;
        }
    }
}

// Función para imprimir los datos de los sensores almacenados
void print_sensor_data(void) {
    for (int i = 0; i < MAX_SENSORS; i++) {
        if (sensor_data[i].sensor_id != 0) {  // Si el sensor tiene datos
            printf("Sensor ID: %d, Temperature: %.2f, Humidity: %.2f\n",
                   sensor_data[i].sensor_id,
                   sensor_data[i].temperature,
                   sensor_data[i].humidity);
        }
    }
}

// Función para obtener los datos de un sensor por su ID
SensorData* get_sensor_data(int sensor_id) {
    for (int i = 0; i < MAX_SENSORS; i++) {
        if (sensor_data[i].sensor_id == sensor_id) {
            return &sensor_data[i];
        }
    }
    return NULL;  // Si no se encuentra el sensor
}
