#include <stdio.h> // printf
#include <stdlib.h> // EXIT_SUCCESS
#include <unistd.h> // fork

const size_t TOTAL_LOOPS = 2;

/// @brief Функция принтираща идентификаторън на текущият процес заедно с идентификаторът на своят родителски процес 
void print_current_process_pid_and_ppid(char*);

int main(void){ 
    print_current_process_pid_and_ppid("MAIN:");
    /*
                   Pm            преди fork();
              /         \
            Pc          Pm       it = 0
           /   \      /    \
         Pcc   Pc    Pcm   Pm    it = 1
    */
    for(size_t it = 0; it < TOTAL_LOOPS; it++){
        /* fork() ще копира текущият процесс и ще му направи копие*/
        fork();

        print_current_process_pid_and_ppid("");
    }

    return EXIT_SUCCESS;
}

void print_current_process_pid_and_ppid(char* msg){
    printf("\n%s Current PID: %d\nParent PID:%d\n",msg,getpid(),getppid());
}