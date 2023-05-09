#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PROCESS_IS_CHILD 0

int is_process_child(int pid){
    return pid == PROCESS_IS_CHILD;
}

void print_process_information(){
    printf("pid:%d ppid:%d pgroupid:%d\n",getpid(),getppid(),getpgrp());
}

void set_the_pgrp_id_of_the_calling_process_to_its_own_pid(){
    setpgid(0,0);
}

/*
 tazi funkciq klonira procesut n times i pokazva
 informaciq samo za child procesut
 */
int clone_the_calling_process_n_times_but_print_only_the_childs_info(int number){

    int pid;

        for(size_t i = 0; i < number; i++){
           pid= fork();
            if(is_process_child(pid)){
                print_process_information();
            }
            else if(!is_process_child(pid)){
                /*
                 terminira roditelskiqt proces
                 za da se pokaje informaciq tochno za
                 7 procesa
                 */
                exit(EXIT_SUCCESS);
            }
        }

    return EXIT_SUCCESS;
}


int main(){

    int pid;

    /*fork() klonira procesut i vruha 0 na
     noviqt proces i child pid na parent procesut

     Tova go pravq za da razdelq koi e child i koi parent process
     */
    pid = fork();

    /*
     ako procesut e child

     napravi negoviqt group id da e = pid
     sled tova pokaji informaciq na procesut

     i clonirai procesut 7 puti i pokaji informaciqta na procesut
     */
    if(is_process_child(pid)){

        set_the_pgrp_id_of_the_calling_process_to_its_own_pid();
        print_process_information();

        clone_the_calling_process_n_times_but_print_only_the_childs_info(7);

    }
    /*
     ako procesut ne e child znachi e roditel

     tova go pravq samo da pokaja grupata na procesut kakva e bila na roditelqt
     i che sum yspql da slova 8 procesa v druga grupa
     */
    else if(!is_process_child(pid)){
        print_process_information();
    }


    return EXIT_SUCCESS;
}
