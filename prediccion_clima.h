#ifndef PREDICCION_CLIMA_H
#define PREDICCION_CLIMA_H

#include <curl/curl.h>
#include <time.h>
#include <json-c/json.h>

#define API_KEY "3qjdFKCUXqUi206Y9UJhxCBKExAWQvQm"
#define LOCATION_KEY "186745" // Ajusta este código según tu ubicación

// Estructura para el pronóstico del clima
typedef struct {
    float tempPronostico;
    float humedadPronostico;
    char fecha[20];
} WeatherForecast;

// Estructura para manejar la memoria de respuesta
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Declaración de funciones
void obtener_pronostico(WeatherForecast *forecast);

#endif