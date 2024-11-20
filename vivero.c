#include "sistema_distribuido.h"

int main(int argc, char** argv) {
    int rank, size;

    // Inicializar AES (solo una vez)
    if (rank == 0) {
        inicializar_aes();
        // Distribuir la clave a otros nodos si es necesario
    }
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (size != 3) {
        printf("Este programa requiere exactamente 3 procesos\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }
    
    // Ejecutamos la función correspondiente según el rank
    switch(rank) {
        case 0:
            nodo_recoleccion();
            break;
        case 1:
            nodo_calculos();
            break;
        case 2:
            nodo_predicciones();
            break;
    }
    
    MPI_Finalize();
    return 0;
}