#ifndef SEGURIDAD_H
#define SEGURIDAD_H

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <mpi.h>

#define KEY_SIZE 32
#define IV_SIZE 16

typedef struct {
    unsigned char* data;
    int length;
    unsigned char iv[IV_SIZE];
} DatosCifrados;

void inicializar_aes(int rank);
int cifrar_aes(const unsigned char* datos, int longitud, DatosCifrados* salida);
int descifrar_aes(const DatosCifrados* entrada, unsigned char* salida, int* longitud);

#endif