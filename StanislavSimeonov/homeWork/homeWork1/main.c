#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void printProcessInformation(){
    printf("\nCurrent PID: %d\n",getpid());
    printf("The Parent Pid of the Current Pid: %d\n",getppid());
}

int main(void){

    for(size_t i = 0; i < 2; i++){
        fork();
        printProcessInformation();
    }    
    
    return EXIT_SUCCESS;
}