#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "treasure_manager.h"

int activ = 1;

void handle_stop(int sig) {
    fprintf(stdout, "[monitor] Se inchide in 5 secunde...\n");
    fflush(stdout);
    usleep(5000000);
    activ = 0;
}

void handle_command(int sig) {
    FILE *f = fopen("cmd.txt", "r");
    if (!f) {
        perror("[monitor] Nu pot citi cmd.txt");
        return;
    }

    char linie[100];
    fgets(linie, sizeof(linie), f);
    fclose(f);
    linie[strcspn(linie, "\n")] = 0;

    fprintf(stdout, "[monitor] Linia primita: \"%s\"\n", linie);

    if (strncmp(linie, "list_treasures ", 15) == 0) {
        char hunt_id[64];
        if (sscanf(linie + 15, "%63s", hunt_id) == 1) {
            fprintf(stdout, "[monitor] Execut list_treasures pentru hunt_id=%s\n", hunt_id);
            list_treasures(hunt_id);
        } else {
            fprintf(stdout, "[monitor] Format invalid: list_treasures <hunt_id>\n");
        }
    } else if (strncmp(linie, "view_treasure ", 14) == 0) {
        char hunt_id[64];
        int id;
        if (sscanf(linie + 14, "%63s %d", hunt_id, &id) == 2) {
            fprintf(stdout, "[monitor] Execut view_treasure pentru hunt_id=%s id=%d\n", hunt_id, id);
            view_treasure(hunt_id, id);
        } else {
            fprintf(stdout, "[monitor] Format invalid: view_treasure <hunt_id> <id>\n");
        }
    } else if (strcmp(linie, "list_hunts") == 0) {
        fprintf(stdout, "[monitor] Comanda 'list_hunts' nu este implementata.\n");
    } else {
        fprintf(stdout, "[monitor] Comanda necunoscuta: %s\n", linie);
    }
    fprintf(stdout, "[monitor] Linia primita: \"%s\"\n", linie);


    fflush(stdout);
}

int main() {
    struct sigaction sa_stop, sa_cmd;

    sa_stop.sa_handler = handle_stop;
    sigemptyset(&sa_stop.sa_mask);
    sa_stop.sa_flags = 0;
    sigaction(SIGUSR2, &sa_stop, NULL);

    sa_cmd.sa_handler = handle_command;
    sigemptyset(&sa_cmd.sa_mask);
    sa_cmd.sa_flags = 0;
    sigaction(SIGUSR1, &sa_cmd, NULL);

    fprintf(stdout, "[monitor] Monitorul a pornit. PID: %d\n", getpid());
    fflush(stdout);

    while (activ) {
        pause();
    }

    fprintf(stdout, "[monitor] Monitorul s-a inchis.\n");
    fflush(stdout);
    return 0;
}

