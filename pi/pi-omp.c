#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <omp.h>
#define THREADS 8
#define IT 40000000000
//global variables
double *vsum;

int main(int argc, char **argv)
{

    vsum = malloc((THREADS+1) * sizeof(*vsum));
    if(vsum == NULL) {
        fprintf(stderr, "Memory allocation problem\n");
        exit(1);
    }

    for(int p=0; p<=THREADS; p++)        //initialize array to 0
    {
        vsum[p] = 0.0;
    }

    double pi = 0.0;
    //launch threads

    printf("")
	#pragma omp parallel num_threads(THREADS)
	{
		int j = omp_get_thread_num();    
		unsigned long int ini = (int)(IT/THREADS)*j;
		unsigned long int fin = (int)(IT/THREADS)+ini;
		unsigned long int i;
        printf("hilo %d inicio: %lu    fin: %lu \n",j,ini,fin);
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

    for(int x=0; x<THREADS; x++)
    {
	printf("[%d] : %.20f\n",x, vsum[x]);
        pi += vsum[x];

    }

    printf("pi  : %.20f\n", pi);

    free(vsum);
}
