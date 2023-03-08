/*
            Disciplina: Tópicos Avançados em Computação III
            Curso: Engenharia de Computação - UFGD - FACET

            Atividade Avaliativa 1
            Implementação do algoritmo one-to-all broadcast para a topologia de dispersão linear e hipercubo

            Docente: Rodrigo Porfírio da Silva Sacchi
            
            Discentes:
            Felipe Emanuel Ferreira     RGA: 20200712175441
            Samir Barcelos Murad        RGA: 20200712181691
*/

//inclusão de bibliotecas
#include <stdio.h> 
#include <time.h>
#include <math.h>
#include "mpi.h"

//protótipos das funções
void linear(int size, int rank, int* msg);
void hipercubo(int size, int rank, int* msg);
void gera_relatorio(double total_l, double total_h, int msg);



int main(int argc, char* argv[])
{
    int size, rank;
    int msg;
    double start_l, end_l, total_l;
    double start_h, end_h, total_h;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        printf("Entre com um inteiro:\n");
        scanf("%d", &msg);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0)
    {
        printf("\n------------------------------------------\n");
        printf("\n>>Iniciando comunicação linear...\n"); 
        start_l = MPI_Wtime();
    }
    
    linear(size, rank, &msg);
    if(rank == 0)
    {
        end_l = MPI_Wtime();
        total_l = end_l - start_l;
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(rank == 0)
    {
        printf("\n---------------------------------------\n");
        printf("\n>>Iniciando comunicação hipercubo...\n");
        start_h = MPI_Wtime();
    }

    hipercubo(size, rank, &msg);

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0)
    {
        end_h = MPI_Wtime();
        total_h = end_h - start_h;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0)
    {
    printf("\n---------------------------------------\n");
	printf("\n>>Gerando relatorio...\n");
        gera_relatorio(total_l, total_h, msg);
    }
    
    MPI_Finalize();
    return 0;
}

void linear(int size, int rank, int* msg)
{
    int incrementador = size, inc_aux = 0;
    int source, dest;
    int qntd_etapas = log2(size);
    int qntd_comm = 0;

    for(int i = 0; i < qntd_etapas; i++){
        incrementador = incrementador >> 1; //uso de operador bitwise para dividir por 2 ao mover um bit para a direita
        qntd_comm = pow(2, i);
        for(int j = 0; j < qntd_comm; j++)
        {
            MPI_Barrier(MPI_COMM_WORLD);
            source = j * inc_aux;
            if(rank == source)
            {
                dest = rank + incrementador;
                MPI_Send(msg, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                printf("Process[%d] sent >%d< to process[%d]\n", rank, *msg, dest);
            }
            dest = source + incrementador;
            if(rank == dest)
            {
                MPI_Recv(msg, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("Process[%d] received >%d< from process[%d]\n", rank, *msg, source);
            }
        }
        
        inc_aux = incrementador;
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

void hipercubo(int size, int rank, int* msg)
{
    int d = log2(size);
    int mask = pow(2, d) - 1;
    int dest, source;
    int potencia; // para não chamar o pow toda hora

    for(int i = d-1; i >= 0; i--)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        potencia = pow(2,i);
        mask ^= potencia;
        if((rank & mask) == 0)
        {
            if((rank & potencia) == 0)
            {
                dest = rank ^ potencia;
                MPI_Send(msg, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
		        printf("Process[%d] sent >%d< to process[%d]\n", rank, *msg, dest);
            }
            else
            {
                source = rank ^ potencia;
                MPI_Recv(msg, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		        printf("Process[%d] received >%d< from process[%d]\n", rank, *msg, source);
            }
        }
    }
}

void gera_relatorio(double total_l, double total_h, int msg)
{
    printf("\n---------------Relatorio---------------\n");
    printf("O tempo gasto para enviar a mensagem [%d] utilizando comunicação linear foi de %lf segundos\n", msg, total_l);
    printf("O tempo gasto para enviar a mensagem [%d] utilizando comunicação hipercubo foi de %lf segundos\n", msg, total_h);
    printf("---------------------------------------\n");
}
