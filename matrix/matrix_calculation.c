#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#define BUFFER 256
double **A;
double **B;
double **C;
int dimensions = 256;

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
   char buffer[BUFFER] ;
   char *record,*line;
   int i=0,j=0;
//    int mat[dimensions][dimensions];
   FILE *fstream = fopen("../files/256.csv","r");
   if(fstream == NULL)
   {
      printf("\n file opening failed ");
      exit(-1);
   }
   while((line=fgets(buffer,sizeof(buffer),fstream))!=NULL)
   {
        record = strtok(line,";");
        while(record != NULL)
        {
            A[i][j] = atof(record);            
            // printf("record : %f",A[i][j]) ;    //here you can put the record into the array as per your requirement.
            // mat[i][j++] = atoi(record) ;
            record = strtok(NULL,";");
            ++j;
        }
        ++i ;
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

    set_matrix();
    clock_t begin = clock();
    matrix_calculation(A, B);
    clock_t end = clock();

    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("%2.12f",time_spent);
}
