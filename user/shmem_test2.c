#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"



int
main(int argc, char *argv[])
{
    int pid;
    int size = 12;
    char * str = (char*) malloc(size);
    str = "Hello chiled";
    if((pid = fork()) > 0){
        wait(0);
    } 
    else{
        int* addr = map_shared_pages(getpid(),pid, str, size);
        if (addr == 0)
        {
            printf("Bad\n");
            exit(1);
        }
        else{
            printf( "%s\n",(char *) addr);
            exit(0);
        }
    }
  exit(0);
}
