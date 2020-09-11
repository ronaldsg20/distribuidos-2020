#include <stdio.h>
#define DIMENSIONS 700
#include <time.h>

void matrix_calculation(double matrix[DIMENSIONS][DIMENSIONS], double matrix_result[DIMENSIONS][DIMENSIONS]){
    // double matrix_result[DIMENSIONS][DIMENSIONS];
    double c = 0;
    for (int i = 0; i < DIMENSIONS; i++){
        for (int j = 0; j < DIMENSIONS; j++){
            for (int k = 0; k < DIMENSIONS; k++){
                c += matrix[j][k] * matrix[k][i];
            }  
            matrix_result[i][j] = c;
            c=0;
        }
    }
}

int main(){
    double matrix[DIMENSIONS][DIMENSIONS];
    double matrix_result[DIMENSIONS][DIMENSIONS];
    for (int i = 0; i < DIMENSIONS; i++){
        for (int j = 0; j < DIMENSIONS; j++){
            matrix[i][j] = 1e5;
        }
    }
    for (int i = 0; i < DIMENSIONS; i++){
        for (int j = 0; j < DIMENSIONS; j++){
            matrix_result[i][j] = 0;
        }
    }

    clock_t begin = clock();
    matrix_calculation(matrix, matrix_result);

    for (int i = 0; i < DIMENSIONS; i++){
        for (int j = 0; j < DIMENSIONS; j++){
            printf("%2.12f", matrix_result[i][j]);
        }
        printf("\n");
    }
    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("%2.12f",time_spent);
}
