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
        int pipe_fd[2];
        pipe(pipe_fd);
        pid_t pid = fork();

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

        // STOP MONITOR
        } else if (strcmp(comanda, "stop_monitor") == 0) {
            if (!monitor_activ) {
                printf("[hub] Monitorul nu ruleaza.\n");
                continue;
            }
            kill(monitor_pid, SIGUSR2); // trimite semnal de oprire

        // EXIT
        } else if (strcmp(comanda, "exit") == 0) {
            if (monitor_activ) {
                printf("[hub] Nu poti iesi. Monitorul inca ruleaza.\n");
            } else {
                printf("[hub] La revedere!\n");
                break;
            }

        // COMENZI TRIMISE MONITORULUI
        } else if (strncmp(comanda, "list_treasures", 14) == 0 ||
                   strncmp(comanda, "view_treasure", 13) == 0 ||
                   strcmp(comanda, "list_hunts") == 0) {

            if (!monitor_activ) {
                printf("[hub] Monitorul nu ruleaza. Foloseste 'start_monitor' mai intai.\n");
                continue;
            }

            FILE *f = fopen("cmd.txt", "w");
            if (!f) {
                perror("Eroare la scrierea fisierului de comanda");
                continue;
            }
            fprintf(f, "%s\n", comanda);
            fclose(f);

            kill(monitor_pid, SIGUSR1); 
            char buffer[2048];
            ssize_t n = read(pipe_fd[0], buffer, sizeof(buffer) - 1);
            if (n > 0) {
                buffer[n] = '\0';
                printf("output monitor:\n%s\n", buffer);
            }
        } else {
            printf("[hub] Comanda necunoscuta.\n");
        }
    }

    return 0;
}
