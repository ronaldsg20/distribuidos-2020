
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define THREADS 8
#define IT 40000000000
//global variables
int N, T;
double *vsum;


void* pi_fun(void* arg)
{
    long j = (long)arg;    
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

    pthread_exit(0);
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
    //launch threads
    pthread_t tids[THREADS+1];

    for(long i = 1; i<=THREADS; i++)
    {
        pthread_create(&tids[i], NULL, pi_fun, (void*)i);
    }

    //wait for threads...
    for(int k = 1; k<=THREADS; k++)
    {
        pthread_join(tids[k], NULL);
    }

    for(int x=1; x<=THREADS; x++)
    {
	printf("[%d] : %.20f\n",x, vsum[x]);
        pi += vsum[x];

    }

    printf("pi : %.20f\n", pi);

    free(vsum);
}
