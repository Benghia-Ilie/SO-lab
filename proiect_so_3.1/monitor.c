// monitor.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "treasure_manager.h" 

int activ = 1; 

// semnal pentru oprire
void handle_stop(int sig) {
    printf("Se inchide în 5 secunde...\n");
    usleep(5000000); 
    activ = 0;
}

// semnal pentru executie comanda
void handle_command(int sig) {
    FILE *f = fopen("cmd.txt", "r");
    if (!f) {
        perror("Nu pot citi cmd.txt");
        return;
    }

    char linie[100];
    fgets(linie, sizeof(linie), f);
    fclose(f);

    linie[strcspn(linie, "\n")] = 0;

    if (strncmp(linie, "list_treasures", 15) == 0) {
        int hunt_id;
        sscanf(linie + 15, "%d", &hunt_id);
        list_treasures(hunt_id);
    }
    else if (strncmp(linie, "view_treasure", 14) == 0) {
        int hunt_id, id;
        sscanf(linie + 14, "%d %d", &hunt_id, &id);
        view_treasure(hunt_id, id);
    }
    else {
        printf("Comanda necunoscuta.\n");
    }
}

int main() {
    signal(SIGUSR2, handle_stop);     
    signal(SIGUSR1, handle_command);  

    printf("Monitorul a pornit. PID: %d\n", getpid());

    while (activ) {
        pause(); 
    }

    printf("Monitorul s-a inchis.\n");
    return 0;
}
