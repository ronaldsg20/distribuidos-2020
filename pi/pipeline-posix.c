#include <stdio.h>
#include <time.h>
#include<unistd.h>
#define IT 1e9

int pi_calculate(double *pi, int start, int end){
    for (int i = start; i < end; i++){
        *pi = *pi + (double)(4.0/(2*i+1));
        i++;
        *pi = *pi - (double)(4.0/(2*i+1));
    }
    return 0;
}

int main(){
    int pipefd[2], r;
    pid_t pid;
    double pi = 0;
    double pi_hijo = 0;
    clock_t begin = clock();

    // r = pipe(pipefd);

    if(r == -1){
        printf("error");
        return 0;
    }

    pid = fork();

    if(pid == -1){
        printf("error");
        return 0;
    }
    if(pid == 0){
        close(pipefd[1]);
        pi_calculate(&pi_hijo,(IT/2) ,IT);
        read(pipefd[0], &pi_hijo, sizeof(double));
    }else{
        close(pipefd[0]);
        pi_calculate(&pi,0 ,(IT/2));
        write(pipefd[1], &pi, sizeof(double));
        close(pipefd[1]);
    }
    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("%2.12f \n",pi);
    printf("%2.12f \n",time_spent);
}
