#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#define THREADS 16
#define BUFFER 1024
double **A;
double **B;
double **C;
int dimensions = 1024;

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
                c += A[j][k] * A[k][i];
            }  
            C[j][i] = c;
            c=0;
        }
    }

    pthread_exit(0);
}

void print_matrix(){
    for (int i = 0; i < dimensions; i++){
        for (int j = 0; j < dimensions; j++){
            printf("%2.12f \t", C[i][j]);
        }
        printf("\n");
    }
}

void set_matrix(){ 
   char buffer[BUFFER] ;
   char *record,*line;
   int i=0,j=0;
//    int mat[dimensions][dimensions];
   FILE *fstream = fopen("../files/1024.csv","r");
   if(fstream == NULL)
   {
      printf("\n file opening failed ");
      exit(-1);
   }
   while((line=fgets(buffer,sizeof(buffer),fstream))!=NULL)
   {
        record = strtok(line,",");
        while(record != NULL)
        {
            A[i][j] = atof(record);            
            // printf("record : %f",A[i][j]) ;    //here you can put the record into the array as per your requirement.
            // mat[i][j++] = atoi(record) ;
            record = strtok(NULL,",");
            ++j;
        }
        ++i ;
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
    
    set_matrix();

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
