#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>

pid_t monitor_pid = -1;
int monitor_activ = 0;
int pipe_fd[2];

void handle_monitor_exit(int sig) {
    int status;
    waitpid(monitor_pid, &status, 0);
    printf("[hub] Monitorul s-a inchis.\n");
    monitor_activ = 0;
    monitor_pid = -1;
    close(pipe_fd[0]);
}

void trim_newline(char *s) {
    s[strcspn(s, "\n")] = 0;
}

int main() {
    char comanda[100];
    struct sigaction sa;
    sa.sa_handler = handle_monitor_exit;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);

    while (1) {
        printf("hub> ");
        fflush(stdout);
        fgets(comanda, sizeof(comanda), stdin);
        trim_newline(comanda);

        if (strcmp(comanda, "start_monitor") == 0) {
            if (monitor_activ) {
                printf("[hub] Monitorul ruleaza deja.\n");
                continue;
            }

            pipe(pipe_fd);
            pid_t pid = fork();
            if (pid == 0) {
                close(pipe_fd[0]);
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]);
                execl("./monitor", "monitor", NULL);
                perror("Eroare exec monitor");
                exit(1);
            } else {
                monitor_pid = pid;
                monitor_activ = 1;
                close(pipe_fd[1]);
                printf("[hub] Monitorul a fost pornit (PID %d).\n", pid);
            }

        } else if (strcmp(comanda, "stop_monitor") == 0) {
            if (!monitor_activ) {
                printf("[hub] Monitorul nu ruleaza.\n");
                continue;
            }
            kill(monitor_pid, SIGUSR2);

        } else if (strcmp(comanda, "exit") == 0) {
            if (monitor_activ) {
                printf("[hub] Nu poti iesi. Monitorul inca ruleaza.\n");
            } else {
                printf("[hub] La revedere!\n");
                break;
            }

        } else if (strncmp(comanda, "list_treasures", 14) == 0 ||
                   strncmp(comanda, "view_treasure", 13) == 0 ||
                   strcmp(comanda, "list_hunts") == 0) {

            if (!monitor_activ) {
                printf("[hub] Monitorul nu ruleaza. Foloseste 'start_monitor' mai intai.\n");
                continue;
            }

            FILE *f = fopen("cmd.txt", "w");
            if (!f) {
                perror("Eroare la scrierea cmd.txt");
                continue;
            }
            fprintf(f, "%s\n", comanda);
            fflush(f);
            fclose(f);

            kill(monitor_pid, SIGUSR1);
            usleep(100000);

            char buffer[2048];
            ssize_t n = read(pipe_fd[0], buffer, sizeof(buffer) - 1);
            if (n > 0) {
                buffer[n] = '\0';
                printf("[hub][output monitor]:\n%s\n", buffer);
            }

        } else if (strcmp(comanda, "calculate_score") == 0) {
            DIR *dir = opendir(".");
            struct dirent *entry;

            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_type == DT_DIR && strncmp(entry->d_name, "hunt", 4) == 0) {
                    char path[128];
                    snprintf(path, sizeof(path), "%s/treasures.dat", entry->d_name);

                    if (access(path, F_OK) == 0) {
                        int fd[2];
                        pipe(fd);
                        pid_t pid = fork();
                        if (pid == 0) {
                            close(fd[0]);
                            dup2(fd[1], STDOUT_FILENO);
                            close(fd[1]);
                            execl("./calculate_score", "calculate_score", entry->d_name, NULL);
                            perror("Eroare exec calculate_score");
                            exit(1);
                        } else {
                            close(fd[1]);
                            char buf[1024];
                            ssize_t n = read(fd[0], buf, sizeof(buf) - 1);
                            if (n > 0) {
                                buf[n] = '\0';
                                printf("[Scoruri pentru %s]:\n%s\n", entry->d_name, buf);
                            }
                            close(fd[0]);
                        }
                    }
                }
            }
            closedir(dir);

        } else {
            printf("[hub] Comanda necunoscuta.\n");
        }
    }

    return 0;
}
