#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MAX_NODES 3  // Define el número de nodos que usarás

void process_data(int rank) {
    // Función de ejemplo para procesar datos según el rank del nodo
    printf("Nodo %d está procesando datos.\n", rank);
}

int main(int argc, char** argv) {
    int rank, size;
    int data;
    MPI_Status status;

    // Inicializa MPI
    MPI_Init(&argc, &argv);

    // Obtén el número total de nodos (procesos) y el rank del nodo actual
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Comprobamos que el número de nodos no exceda el límite
    if (size > MAX_NODES) {
        if (rank == 0) {
            printf("Error: el número máximo de nodos es %d.\n", MAX_NODES);
        }
        MPI_Finalize();
        return -1;
    }

    // Si eres el nodo principal (rank 0), distribuye datos
    if (rank == 0) {
        data = 100;  // Datos de ejemplo
        printf("Nodo principal (rank 0) enviando datos a los otros nodos.\n");

        // Enviar datos a otros nodos
        for (int i = 1; i < size; i++) {
            MPI_Send(&data, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        // Recibe datos desde el nodo principal (rank 0)
        MPI_Recv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        printf("Nodo %d recibió el dato %d desde el nodo principal.\n", rank, data);
    }

    // Cada nodo procesa los datos recibidos
    process_data(rank);

    // Finaliza la ejecución de MPI
    MPI_Finalize();
    return 0;
}
