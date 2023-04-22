#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){
    fork();
    printf("hello world\n");

    return EXIT_SUCCESS;
}