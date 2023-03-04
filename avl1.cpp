#include <stdio.h>
#include <time.h>
#include <math.h>
#include "mpi.h"

void linear(int size, int rank, int* msg);
void hipercubo(int size, int rank, int* msg);

int main(int argc, char* argv[]){
    int size, rank;
    int msg;
    int choice = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0){
        printf("Entre com um inteiro:\n>>");
        scanf("%d", &msg);
    } 
    do
    {
        printf("digite 1 para linear\n2 para hipercubo\n3 para fechar: ");
        scanf("%d", &choice);
        switch(choice)
        {
            case 1: 
                MPI_Barrier(MPI_COMM_WORLD);
                linear(size, rank, &msg);   
                break;
                
            case 2: 
                MPI_Barrier(MPI_COMM_WORLD);
                hipercubo(size, rank, &msg);   
                break;
            case 3: 
                printf("finalizando..."); 
                break;
            default:
                printf("digite um valor valido");
                break;
        }
        
    }while(choice != 3);
    
    MPI_Finalize();
    return 0;
}

void linear(int size, int rank, int* msg){
    int incrementador = size, inc_aux = 0;
    int source, dest;
    int qntd_etapas = log2(size);
    int qntd_comm = 0;

    for(int i = 0; i < qntd_etapas; i++){
        incrementador = incrementador >> 1;
        qntd_comm = pow(2, i);
        for(int j = 0; j < qntd_comm; j++){
            source = j * inc_aux;
            if(rank == source){
                dest = rank + incrementador;
                MPI_Send(msg, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                printf("Process [%d]: Enviei uma mensagem para o processo [%d]\n", rank, dest);
            }
            dest = source + incrementador;
            if(rank == dest){
                MPI_Recv(msg, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("Processo[%d]: Mensagem recebida do processo [%d]", rank, source);
            }
        }
        inc_aux = incrementador;
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

void hipercubo(int size, int rank, int* msg){
    int d = log2(size);
    int mask = pow(2, d) - 1;
    int dest, source;
    int potencia; // para não chamar o pow toda hora

    for(int i = d-1; i >= 0; i--){
        potencia = pow(2,i);
        mask = mask ^ potencia;
        if((rank & mask) == 0){
            if((rank & potencia) == 0){ //mudei de mask & potencia pra rank & potencia
                dest = rank ^ potencia;
                MPI_Send(msg, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            }
            else{
                source = rank ^ potencia;
                MPI_Recv(msg, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    }
}
 
