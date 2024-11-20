#include "seguridad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char clave[KEY_SIZE];

void inicializar_aes(int rank) {
    if (rank == 0) {
        // El nodo 0 genera la clave
        if (RAND_bytes(clave, KEY_SIZE) != 1) {
            fprintf(stderr, "Error generando clave AES\n");
            exit(1);
        }
        printf("Clave AES generada exitosamente\n");
    }
    
    // Distribuir la clave a todos los nodos
    MPI_Bcast(clave, KEY_SIZE, MPI_BYTE, 0, MPI_COMM_WORLD);
    
    if (rank != 0) {
        printf("[Proceso %d] Clave AES recibida\n", rank);
    }
}

int cifrar_aes(const unsigned char* datos, int longitud, DatosCifrados* salida) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        printf("Error: No se pudo crear el contexto de cifrado\n");
        return -1;
    }

    // Generar IV aleatorio
    if (RAND_bytes(salida->iv, IV_SIZE) != 1) {
        printf("Error: No se pudo generar el IV\n");
        EVP_CIPHER_CTX_free(ctx);
        return -2;
    }

    // Inicializar cifrado
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, clave, salida->iv) != 1) {
        printf("Error: No se pudo inicializar el cifrado\n");
        EVP_CIPHER_CTX_free(ctx);
        return -3;
    }

    salida->data = (unsigned char*)malloc(longitud + EVP_MAX_BLOCK_LENGTH);
    if (!salida->data) {
        printf("Error: No se pudo asignar memoria\n");
        EVP_CIPHER_CTX_free(ctx);
        return -4;
    }

    int len;
    if (EVP_EncryptUpdate(ctx, salida->data, &len, datos, longitud) != 1) {
        printf("Error: Fallo en EncryptUpdate\n");
        free(salida->data);
        EVP_CIPHER_CTX_free(ctx);
        return -5;
    }
    salida->length = len;

    int len_final;
    if (EVP_EncryptFinal_ex(ctx, salida->data + len, &len_final) != 1) {
        printf("Error: Fallo en EncryptFinal\n");
        free(salida->data);
        EVP_CIPHER_CTX_free(ctx);
        return -6;
    }
    salida->length += len_final;

    EVP_CIPHER_CTX_free(ctx);
    return 0;
}
int descifrar_aes(const DatosCifrados* entrada, unsigned char* salida, int* longitud) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Error creando contexto de descifrado\n");
        return -1;
    }

    // Inicializar descifrado AES
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, clave, entrada->iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        fprintf(stderr, "Error inicializando descifrado\n");
        return -1;
    }

    int len;
    // Descifrar datos
    if (EVP_DecryptUpdate(ctx, salida, &len, entrada->data, entrada->length) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        fprintf(stderr, "Error descifrando datos\n");
        return -1;
    }
    *longitud = len;

    // Finalizar descifrado
    int len_final;
    if (EVP_DecryptFinal_ex(ctx, salida + len, &len_final) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        fprintf(stderr, "Error finalizando descifrado\n");
        return -1;
    }
    *longitud += len_final;

    EVP_CIPHER_CTX_free(ctx);
    return 0;
}