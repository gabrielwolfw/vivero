#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_rank;
    int world_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int data = 100;

    if (world_rank == 0) {
        // Nodo maestro envía datos
        printf("Master node sending data: %d\n", data);
        
        for (int i = 1; i < world_size; i++) {
            MPI_Send(&data, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Recibir y mostrar los datos procesados en orden
        int result;
        for (int i = 1; i < world_size; i++) {
            MPI_Recv(&result, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Received processed data from node %d: %d\n", i, result);
        }

    } else {
        // Nodos trabajadores reciben datos y procesan
        MPI_Recv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Node %d received data: %d\n", world_rank, data);

        int processed_data = data + world_rank;

        // Enviar datos procesados al maestro
        MPI_Send(&processed_data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
