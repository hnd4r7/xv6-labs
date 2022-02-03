#include "kernel/types.h"
#include "user/user.h"

#define PRIME_NUMBER 35
#define READEND 0
#define WRITEEND 1

void child(int* pl);

int main(int argc, char* argv[]){
    int p[2];
    pipe(p);

    int pid = fork();
    if(pid == 0){
        child(p);
    }else{
        close(p[READEND]);
        for (int i=2; i < PRIME_NUMBER+1; i++){
            write(p[WRITEEND], &i, sizeof(int));
        }
        close(p[WRITEEND]);
        wait((void*)0);
    }
    exit(0);
}

void child(int *pl){
    int n;
    int pr[2];

    close(pl[WRITEEND]); // Be careful to close file descriptors that a process doesn't need, because otherwise your program will run xv6 out of resources before the first process reaches 35.

    int len = read(pl[0], &n, sizeof(int));
    if (len == 0){
        exit(0);
    }
   
    pipe(pr);

    int pid = fork();
    if (pid == 0){
        child(pr);
    }else{
        close(pr[0]);

        printf("prime %d\n", n);
        int prime = n;

        while((read(pl[0], &n, sizeof(int))) != 0){
            if(n % prime != 0){
                write(pr[1], &n, sizeof(int));
            }
        }
        close(pr[1]);
        wait((void *)0);
        exit(0);
    }
}