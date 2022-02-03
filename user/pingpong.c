#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NULL ((void *)0)
int main(int argc, char const *argv[]){
    int pp[2]; //父进程写 子进程读
    int pc[2]; //子进程写 父进程读

    char buf[1];
    pipe(pc);
    pipe(pp);

    int pid = fork();
    if (pid == 0)
    {
        close(pp[1]);
        close(pc[0]);
        read(pp[0], buf, 1);
        printf("%d: received ping\n", getpid());
        write(pc[1], " ", 1);
        close(pp[0]);
        close(pc[1]);
    }
    else
    {
        close(pp[0]);
        close(pc[1]);
        write(pp[1], " ", 1);
        read(pc[0], buf, 1);
        printf("%d: received pong\n", getpid());
        close(pc[0]);
        close(pp[1]);
    }
    exit(0);
}
