#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{   
    uint64 address;
    int child_pid;
    int ppid = getpid();
    char *str = (char*)malloc(20);
    int pid = fork();
    if (pid < 0) {
        printf("Fork failed\n");
        exit(1);
    } else if(pid > 0){// Parent process
        strcpy(str, "Hello Child\n");
        wait(0);
    }
    else{ // Child process: pid==0
        sleep(10);
        child_pid = getpid();
        address = map_shared_pages(ppid,child_pid,(uint64)str, 20);
        printf("chiled proc read:%s\n", (char*)address);
        exit(0);
    }
    free(str);
    exit(0);
}
