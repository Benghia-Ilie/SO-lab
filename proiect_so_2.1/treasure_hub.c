// treasure_hub.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

pid_t monitor_pid = -1;
int monitor_activ = 0;

// cand monitorul se inchide, primim acest semnal
void handle_monitor_exit(int sig) {
    int status;
    waitpid(monitor_pid, &status, 0); // asteapta copilul
    printf("[hub] Monitorul s-a inchis.\n");
    monitor_activ = 0;
    monitor_pid = -1;
}

int main() {
    char comanda[100];

    // pregatim handler pentru SIGCHLD (cand se inchide monitorul)
    signal(SIGCHLD, handle_monitor_exit);

    while (1) {
        printf("hub> ");
        fflush(stdout);
        fgets(comanda, sizeof(comanda), stdin);
        comanda[strcspn(comanda, "\n")] = 0; // elimina '\n'

        // START MONITOR
        if (strcmp(comanda, "start_monitor") == 0) {
            if (monitor_activ) {
                printf("[hub] Monitorul ruleaza deja.\n");
                continue;
            }

            pid_t pid = fork();
            if (pid == 0) {
                execl("./monitor", "monitor", NULL);
                perror("Eroare la executia monitorului");
                exit(1);
            } else {
                monitor_pid = pid;
                monitor_activ = 1;
                printf("[hub] Monitorul a fost pornit (PID %d).\n", pid);
            }
        }

    return 0;
}
