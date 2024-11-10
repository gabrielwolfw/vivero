// calculos.c
#include <math.h>
#include "calculos.h"

// Cálculo de presión de vapor de saturación (Es)
static float calcularEs(float temperatura) {
    return 0.6108 * exp((17.27 * temperatura) / (temperatura + 237.3));
}

// Calcular VPD
float calcularVPD(float temperatura, float humedadRelativa) {
    float Es = calcularEs(temperatura);
    return Es * (1 - humedadRelativa / 100);
}

// Calcular Índice de Calor en Fahrenheit
float calcularIndiceCalor(float temperaturaF, float humedadRelativa) {
    float c1 = -42.379, c2 = 2.04901523, c3 = 10.14333127;
    float c4 = -0.22475541, c5 = -6.83783e-3, c6 = -5.481717e-2;
    float c7 = 1.22874e-3, c8 = 8.5282e-4, c9 = -1.99e-6;

    return c1 + c2 * temperaturaF + c3 * humedadRelativa +
           c4 * temperaturaF * humedadRelativa + c5 * pow(temperaturaF, 2) +
           c6 * pow(humedadRelativa, 2) + c7 * pow(temperaturaF, 2) * humedadRelativa +
           c8 * temperaturaF * pow(humedadRelativa, 2) +
           c9 * pow(temperaturaF, 2) * pow(humedadRelativa, 2);
}

// Calcular Punto de Rocío
float calcularPuntoRocio(float temperatura, float humedadRelativa) {
    return temperatura - ((100 - humedadRelativa) / 5);
}
