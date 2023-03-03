#include <stdio.h>
#include <time.h>
#include <math.h>
#include "mpi.h"

int main(int argc, char* argv[]){
    int size, rank, i, j;
    int incrementador, inc_aux;
    int source, dest, msg;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WOLRD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int qntd_etapas = log2(size);
    int qntd_comm = 0;

    if (rank == 0){
        printf("Entre com um inteiro:\n>>");
        scanf("%d", &msg);
    } 

    MPI_Barrier(MPI_COMM_WORLD);
    
    for(i = 1; i <= qntd_etapas; i++){
        incrementador = size/pow(2,1);
        qntd_comm = pow(2, i-1);
        for(j = 0; j < qntd_comm; j++){
            source = j*inc_aux;
            if(rank == source){
                dest = rank + incrementador;
                MPI_Send(&msg, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                printf("Processo[%d]: Enviei uma mensagem para o processo %d\n", rank, dest);
            }
            dest = source + incrementador;
            if(rank == dest){
                MPI_Recv(&msg, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
                printf("Processo[%d]: Mensagem recebida do processo %d", rank, source);
            }
        }
        inc_aux = incrementador;
        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}