#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(void){
    printf("hello world\n");
    fork();

    return EXIT_SUCCESS;
}