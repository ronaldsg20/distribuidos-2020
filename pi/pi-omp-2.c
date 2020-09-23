
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#define THREADS 8
#define IT 4e9
//global variables
int N, T;
double *vsum;


void pi_fun(int j)
{  
    unsigned long int ini = (int)(IT/THREADS)*(j-1);
    unsigned long int fin = (int)(IT/THREADS)+ini;
    unsigned long int i;
    for(i = ini; i < fin; i++)
    {

        if(i % 2 == 0){
            vsum[j] += 4.0/((2*i)+1); 
            
                  }
        else{
            vsum[j] -= 4.0/((2*i)+1);
            
                  }
    }

}

int main(int argc, char **argv)
{

    vsum = malloc((THREADS+1) * sizeof(*vsum));
    if(vsum == NULL) {
        fprintf(stderr, "Memory allocation problem\n");
        exit(1);
    }

    for(int p=0; p<=THREADS; p++)        //initialize array to 0
    {
        vsum[p] = 0;
    }

    double pi = 0.0;

    printf("Iterations: %f \n", IT);
    printf("Threads: %d \n", THREADS);
    //launch threads
    clock_t begin = clock();
    #pragma omp parallel num_threads(THREADS)
	{
        int i = omp_get_thread_num();
        pi_fun(i);
    }

    for(int x=1; x<=THREADS; x++)
    {
	printf("[%d] : %.20f\n",x, vsum[x]);
        pi += vsum[x];

    }
    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("pi : %.20f\n", pi);
    printf("Time Spent : %2.12f\n", time_spent);
    free(vsum);
}
