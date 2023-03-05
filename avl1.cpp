#include <stdio.h>
#include <time.h>
#include <math.h>
#include "mpi.h"

void linear(int size, int rank, int* msg);
void hipercubo(int size, int rank, int* msg);
void gera_relatorio(double total_l, double total_h, int msg);

int main(int argc, char* argv[]){
    int size, rank;
    int msg;
    double start_l, end_l, total_l;
    double start_h, end_h, total_h;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0){
        printf("Entre com um inteiro:\n");
        scanf("%d", &msg);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0){
        printf(">>Iniciando comunicação linear...\n");
        start_l = MPI_Wtime();
    }
    
    linear(size, rank, &msg);

    if(rank == 0){
        end_l = MPI_Wtime();
        total_l = end_l - start_l;
    }
    
    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0){
        printf(">>Iniciando comunicação hipercubo...\n");
        start_h = MPI_Wtime();
    }

    hipercubo(size, rank, &msg);

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0){
        end_h = MPI_Wtime();
        total_h = end_h - start_h;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0){
	printf(">>Gerando relatorio...\n");
        gera_relatorio(total_l, total_h, msg);
    }
    
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
               // printf("Process [%d]: Enviei uma mensagem para o processo [%d]\n", rank, dest);
            }
            dest = source + incrementador;
            if(rank == dest){
                MPI_Recv(msg, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
               // printf("Process [%d]: Mensagem recebida do processo [%d]\n", rank, source);
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
		//printf("Process [%d]: Enviei uma mensagem para o processo [%d]\n", rank, dest);
            }
            else{
                source = rank ^ potencia;
                MPI_Recv(msg, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//printf("Process [%d]: Mensagem recebida do processo [%d]\n", rank, source);
            }
        }
    }
}

void gera_relatorio(double total_l, double total_h, int msg){
    printf("---------------Relatorio---------------\n");
    printf("O tempo gasto para enviar a mensagem [%d] utilizando comunicação linear foi de %lf segundos\n", msg, total_l);
    printf("O tempo gasto para enviar a mensagem [%d] utilizando comunicação hipercubo foi de %lf segundos\n", msg, total_h);
    printf("---------------------------------------\n");
}
