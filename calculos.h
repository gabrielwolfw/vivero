// calculos.h
#ifndef CALCULOS_H
#define CALCULOS_H

// Umbrales para la toma de decisiones
#define UMBRAL_VPD 1.5         // kPa - Déficit de Presión de Vapor máximo aceptable
#define UMBRAL_INDICE_CALOR 85.0  // °F - Índice de calor máximo aceptable
#define UMBRAL_HUMEDAD_SUELO 40.0  // % - Porcentaje mínimo de humedad en suelo

// Funciones para cálculos de indicadores
float calcularVPD(float temperatura, float humedadRelativa);
float calcularIndiceCalor(float temperaturaF, float humedadRelativa);
float calcularPuntoRocio(float temperatura, float humedadRelativa);

#endif
