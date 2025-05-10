// monitor.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int activ = 1; 

// semnal pentru oprire
void handle_stop(int sig) {
    printf("Se inchide în 5 secunde...\n");
    usleep(5000000); 
    activ = 0;
}

int main() {
    signal(SIGUSR2, handle_stop);  

    printf("Monitorul a pornit. PID: %d\n", getpid());

    while (activ) {
        pause(); 
    }

    printf("Monitorul s-a inchis.\n");
    return 0;
}
