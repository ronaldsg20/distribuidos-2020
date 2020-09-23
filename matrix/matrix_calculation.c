#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#define BUFFER 1024
double **A;
double **B;
double **C;
int dimensions = 1024;

void matrix_calculation(double **A, double **B){
    // double matrix_result[dimensions][dimensions];
    double c = 0;
    // COLUMNAS DE B
    for (int i = 0; i < dimensions; i++){
        //FILAS DE A
        for (int j = 2; j < dimensions; j++){
            //COLUMNA DE A POR SU FILA
            for (int k = 0; k < dimensions; k++){
                c += A[j][k] * A[k][i];
            }  
            C[j][i] = c;
            c=0;
        }
    }
}


void set_matrix(){ 
   char buffer[BUFFER * BUFFER * sizeof(double)] ;
   char *record,*line;
   int i=0,j=0;
   int counter = 0;
   FILE *fstream = fopen("../files/32.csv","r");
   if(fstream == NULL)
   {
      printf("\n file opening failed ");
   }
   while((line=fgets(buffer,sizeof(buffer),fstream))!=NULL)
   {
     record = strtok(line,";");
     while(record != NULL)
     {
         printf("Counter: %d", counter);
        // printf("record : %s %d ",record, counter) ;    //here you can put the record into the array as per your requirement.
         A[i][j++] = atoi(record) ;
         record = strtok(NULL,";");
         counter++;
     }
     ++i ;
   }
}
void print_matrix(){
    for (int i = 0; i < dimensions; i++){
        for (int j = 0; j < dimensions; j++){
            printf("%2.1f \t", A[i][j]);
        }
        printf("\n");
    }
}

int main(){

    A = (double **)malloc(dimensions * sizeof(double *));
    B = (double **)malloc(dimensions  * sizeof(double *));
    C = (double **)malloc(dimensions  * sizeof(double *));
    
    for (int i = 0; i < dimensions; i++){
        A[i] = (double *)malloc(dimensions * sizeof(double));
        B[i] = (double *)malloc(dimensions * sizeof(double));
        C[i] = (double *)malloc(dimensions * sizeof(double));
    }

    // set_matrix();
    for (int i = 0; i < dimensions; i++){
        for (int j = 0; j < dimensions; j++){
            A[i][j] = 1e5;
            B[i][j] = 1e5;
        }
    }
    // printf("Matrix 50 25 %ld \n", sizeof(double));
    // print_matrix();
    clock_t begin = clock();
    matrix_calculation(A, B);
    clock_t end = clock();

    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("Time : %2.12f \n ",time_spent);
}
