#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printFinalTopology (int rank, int topology[]) {

    printf("%d -> ", rank);
    int aux = 0;
    printf("%d:", topology[0]);
    int maxFor0 = topology[1];
    int count = 2;
    while(aux < maxFor0){
        if(aux < maxFor0 - 1 )
            printf("%d,", topology[count]);
        else
            printf("%d ", topology[count]);
        count++;
        aux++;
    }
    printf("%d:", topology[count]); 
    count++; 
    int maxFor1 = topology[count];
    count++;
    aux = 0;
    while(aux < maxFor1){
        if(aux < maxFor1 - 1)
            printf("%d,", topology[count]);
        else
            printf("%d ", topology[count]);
        count++;
        aux++;
    }
    printf("%d:", topology[count]);
    count++;
    int maxFor2 = topology[count];
    count++;
    aux = 0;
    while(aux < maxFor2){
        if(aux < maxFor2 - 1)
            printf("%d,", topology[count]);
        else
            printf("%d\n", topology[count]);
        count++;
        aux++;
    }
}

int main (int argc, char *argv[])
{
    int  nProc, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nProc);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int N;
    int recv_num;
    int nrProcWorker;
    int rankWorker;
    MPI_Status status;
    int size = 0;
    int temp;

    int number_lines;

    // First process starts the circle.
    if (rank == 0) {
        // cluster 0
        //FILE* file = fopen("../tests/test1/cluster0.txt", "r");
        FILE* file = fopen("cluster0.txt", "r");
        if(file == NULL){
            printf("Error opening file");
            return -1;
        }
        fscanf(file, "%d", &size);
        

        int content[size + 2];
        content[0] = rank;
        content[1] = size;
        size = size + 2;

        for(int i = 2; i < size; i++) {
            fscanf(file, "%d", &content[i]);
        }


        MPI_Send(&size, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(&content, size, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("M(0,1)\n");

        MPI_Send(&size, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
        MPI_Send(&content, size, MPI_INT, 2, 0, MPI_COMM_WORLD);
        printf("M(0,2)\n");


        int primit1_size;
        MPI_Recv(&primit1_size, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        int primit1[primit1_size];
        MPI_Recv(&primit1, primit1_size, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);


        int primit2_size;
        MPI_Recv(&primit2_size, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
        int primit2[primit2_size];
        MPI_Recv(&primit2, primit2_size, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
  
        
        // vector_pt_workeri = array + primit1 + primit 2
        int final_size = size + primit1_size + primit2_size;
        int arrayWorkers[final_size];
        for(int i = 0 ;i < final_size; i++){
            if(i < size)
                arrayWorkers[i] = content[i];
            else if(i >= size && i < size + primit1_size) {
                for(int j = 0; j < primit1_size; j++) {
                    arrayWorkers[i] = primit1[j];
                    i++;
                }
                i--;
            }                
            else if(i >= size + primit1_size && i < size + primit1_size + primit2_size) {
                for(int j = 0; j < primit2_size; j++) {
                    arrayWorkers[i] = primit2[j];
                    i++;
                }
                i--;
            }
        }


        for (int i = 2; i < size; i++) {
            MPI_Send(&final_size, 1, MPI_INT, content[i], 1, MPI_COMM_WORLD);
            MPI_Send(&arrayWorkers, final_size, MPI_INT, content[i], 1, MPI_COMM_WORLD);
        }

        printFinalTopology(rank, arrayWorkers);
    } 
    else if (rank == 1){

        //FILE* file = fopen("../tests/test1/cluster1.txt", "r");
        FILE* file = fopen("cluster1.txt", "r");

        if(file == NULL){
            printf("Error opening file");
            return -1;
        }
        fscanf(file, "%d", &size);

        int content[size + 2];
        content[0] = rank;
        content[1] = size;
        size = size + 2;

        for(int i = 2; i < size; i++) {
            fscanf(file, "%d", &content[i]);
        }


        MPI_Send(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&content, size, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("M(1,0)\n");



        MPI_Send(&size, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
        MPI_Send(&content, size, MPI_INT, 2, 0, MPI_COMM_WORLD);
        printf("M(1,2)\n");


        int primit0_size;
        MPI_Recv(&primit0_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        int primit0[primit0_size];
        MPI_Recv(&primit0, primit0_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        int primit2_size;
        MPI_Recv(&primit2_size, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
        int primit2[primit2_size];
        MPI_Recv(&primit2, primit2_size, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);

        int final_size = size + primit0_size + primit2_size;
        int arrayWorkers[final_size];
        for(int i = 0 ;i < final_size; i++){
            if(i < primit0_size) {
                for(int j = 0; j < primit0_size; j++) {
                    arrayWorkers[i] = primit0[j];
                    i++;
                }
                i--;
            }
            else if(i >= primit0_size && i < size + primit0_size) {
                for(int j = 0; j < size; j++) {
                    arrayWorkers[i] = content[j];
                    i++;
                }
                i--;
            }                
            else if(i >= size + primit0_size && i < size + primit0_size + primit2_size) {
                for(int j = 0; j < primit2_size; j++) {
                    arrayWorkers[i] = primit2[j];
                    i++;
                }
                i--;
            }
        }

        for (int i = 2; i < size; i++) {
            MPI_Send(&final_size, 1, MPI_INT, content[i], 1, MPI_COMM_WORLD);
            MPI_Send(&arrayWorkers, final_size, MPI_INT, content[i], 1, MPI_COMM_WORLD);
        }
    

        printFinalTopology(rank, arrayWorkers);

    }
    else if (rank == 2){

         //FILE* file = fopen("../tests/test1/cluster2.txt", "r");
        FILE* file = fopen("cluster2.txt", "r");
        if(file == NULL){
            printf("Error opening file");
            return -1;
        }
        fscanf(file, "%d", &size);

        int content[size + 2];
        content[0] = rank;
        content[1] = size;
        size = size + 2;

        for(int i = 2; i < size; i++) {
            fscanf(file, "%d", &content[i]);
        }

        MPI_Send(&size, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(&content, size, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("M(2,1)\n");
        
        MPI_Send(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&content, size, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("M(2,0)\n");

        int primit0_size;
        MPI_Recv(&primit0_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        int primit0[primit0_size];
        MPI_Recv(&primit0, primit0_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        int primit1_size;
        MPI_Recv(&primit1_size, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        int primit1[primit1_size];
        MPI_Recv(&primit1, primit1_size, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);

        int final_size = size + primit1_size + primit0_size;
        int arrayWorkers[final_size];
        for(int i = 0; i < final_size; i++){
            if(i < primit0_size) {
                for(int j = 0; j < primit0_size; j++) {
                    arrayWorkers[i] = primit0[j];
                    i++;
                }
                i--;
            }
            else if(i >= primit0_size && i < primit0_size + primit1_size) {
                for(int j = 0; j < primit1_size; j++) {
                    arrayWorkers[i] = primit1[j];
                    i++;
                }
                i--;
            }                
            else if(i >= primit0_size + primit1_size && i < size + primit1_size + primit0_size) {
                for(int j = 0; j < size; j++) {
                    arrayWorkers[i] = content[j];
                    i++;
                }
                i--;
            }
        }


        for (int i = 2; i < size; i++) {
            MPI_Send(&final_size, 1, MPI_INT, content[i], 1, MPI_COMM_WORLD);
            MPI_Send(&arrayWorkers, final_size, MPI_INT, content[i], 1, MPI_COMM_WORLD);
        }


        printFinalTopology(rank, arrayWorkers);
        
    }
    else{
        int primit_size;
        MPI_Recv(&primit_size, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
        int full_topology[primit_size];
        MPI_Recv(&full_topology, primit_size, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);


        printFinalTopology(rank, full_topology);
        
    }


    MPI_Finalize();

}