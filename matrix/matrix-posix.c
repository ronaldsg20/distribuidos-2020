#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#define THREADS 8
double **A;
double **B;
double **C;
int dimensions = 120;

typedef struct thread_data{
    long principio;
    long fin; 
} thread_data;

void *matrix_calculation(void *arg){
    thread_data *interval  = (thread_data *)arg;
    double c = 0;
    // COLUMNAS DE B
    for (int i = 0; i < dimensions; i++){
        //FILAS DE A
        for (int j = interval->principio; j < interval->fin; j++){
            //COLUMNAS DE A
            for (int k = 0; k < dimensions; k++){
                c += A[j][k] * B[k][i];
            }  
            C[j][i] = c;
            c=0;
        }
    }

    pthread_exit(NULL);
}

void print_matrix(){
    for (int i = 0; i < dimensions; i++){
        for (int j = 0; j < dimensions; j++){
            printf("%2.12f \t", C[i][j]);
        }
        printf("\n");
    }
}

int main(){
    pthread_t tids[THREADS];
    int blocks = 0;
    thread_data interval[THREADS];

    A = (double **)malloc(dimensions * sizeof(double *));
    B = (double **)malloc(dimensions  * sizeof(double *));
    C = (double **)malloc(dimensions  * sizeof(double *));
    
    for (int i = 0; i < dimensions; i++){
        A[i] = (double *)malloc(dimensions * sizeof(double));
        B[i] = (double *)malloc(dimensions * sizeof(double));
        C[i] = (double *)malloc(dimensions * sizeof(double));
    }

    for (int i = 0; i < dimensions; i++){
        for (int j = 0; j < dimensions; j++){
            A[i][j] = 1e2;
            B[i][j] = 1e2;
        }
    }

    blocks = floor(dimensions/THREADS);
    clock_t begin = clock();


    for(long i = 0; i < THREADS + 1; i++){
        interval[i].principio = ((blocks)*i);
        if (i == THREADS){
            interval[i].fin = ((blocks*i)+(dimensions%THREADS));
        }else{
            interval[i].fin = ((blocks)*(i+1));
        }
        pthread_create(&tids[i], NULL, matrix_calculation, &interval[i]);
    }

    for(int j = 0; j < THREADS; j++){
        pthread_join(tids[j], NULL);
    } 
    clock_t end = clock();
    // print_matrix();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("%2.12f",time_spent);
}
