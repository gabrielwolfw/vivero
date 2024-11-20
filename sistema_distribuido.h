#ifndef SISTEMA_DISTRIBUIDO_H
#define SISTEMA_DISTRIBUIDO_H

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "prediccion_clima.h"
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

// Declaración de funciones
void nodo_recoleccion(void);
void nodo_calculos(void);
void nodo_predicciones(void);

#endif