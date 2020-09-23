
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#define BUFFER 100000
#define THREADS 8
#define IT 40000000000
double **A;
double **B;
double **C;
int dimensions = 64;
//global variables
int N;


void print_matrix(double **X){
    for (int i = 0; i < dimensions; i++){
        for (int j = 0; j < dimensions; j++){
            printf("%2.1f ", X[i][j]);
        }
        printf("\n");
    }
}

void readMatrix(char *filename, double **M){
    printf("FileName: %s \n", filename);
    FILE *fstream = fopen(filename, "r");
    if(fstream == NULL){
        printf("\n file opening failed ");
        exit(-1);
    } 
    char *record,*line;
    char buffer[BUFFER];
    int i=0,j=0;
    while((line=fgets(buffer,sizeof(buffer),fstream))!=NULL)
   {
        record = strtok(line,",");
        printf("record: %s \n", record);
        while(record != NULL)
        {
            M[i][j++] = atof(record);            
            record = strtok(NULL,",");
        }
        ++i ;
   }
}

void writeMatrix(char *filename, double **R, int N){
    FILE *fp;
    int i,j;
    fp=fopen(filename,"w+");
    for(i=0 ; i<N; i++){
        for(j=0; j<N; j++){
            fprintf(fp,",%f ",R[i][j]);
        }
        fprintf(fp,"\n%d",i+1);
    }
    fclose(fp);
}

void matrix_calculation(int nThread){
    unsigned long int ini = (int)(dimensions/THREADS)*(nThread);
    unsigned long int fin = 0;
    if((nThread + 1) == THREADS){
        fin = (int)(dimensions%THREADS)+ini+1;
    } else {
        fin = (int)(dimensions/THREADS)+ini;
    }
    double c = 0;
    printf("Hilo: %d, Inicio: %ld, Fin: %ld \n", nThread, ini, fin);
    // COLUMNAS DE B
    for (int i = ini; i < fin; i++){
        //FILAS DE A
        for (int j = 0; j < dimensions; j++){
            //COLUMNA DE A POR SU FILA
            for (int k = 0; k < dimensions; k++){
                c += A[j][k] * B[k][i];
            }  
            C[j][i] = c;
            c=0;
        }
    }
}

int main(int argc, char **argv)
{
    // memory allocation
    A = (double **)malloc(dimensions * sizeof(double *));
    B = (double **)malloc(dimensions  * sizeof(double *));
    C = (double **)malloc(dimensions  * sizeof(double *));

    for (int i = 0; i < dimensions; i++){
        A[i] = (double *)malloc(dimensions * sizeof(double));
        B[i] = (double *)malloc(dimensions * sizeof(double));
        C[i] = (double *)malloc(dimensions * sizeof(double));
    }
    printf("Matrix multiplication Size: %d \n", dimensions);
    // matrix read
    readMatrix("../files/64.csv", A);
    // readMatrix("../files/64.csv", B);

    printf("Matrix multiplication Size: %d \n", dimensions);
    printf("Threads: %d \n", THREADS);
    // multiplication

    // clock_t begin = clock();

    // #pragma omp parallel num_threads(THREADS)
	// {
    //     int i = omp_get_thread_num();
    //     matrix_calculation(i);
    // }

    // clock_t end = clock();
    // double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    // printf("Time spent: %2.12f \n",time_spent);

    // write Matrix
    // writeMatrix("../files/32-o.csv", C, dimensions);
    
    print_matrix(A);
    free(A);
    free(B);
    free(C);
}
