#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "treasure_manager.h"

void log_comanda(const char* hunt_id, const char* mesaj) {
    char cale_log[128], link_log[128];
    snprintf(cale_log, sizeof(cale_log), "%s/logged_hunt", hunt_id);

    int log_fd = open(cale_log, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd >= 0) {
        dprintf(log_fd, "%s\n", mesaj);
        close(log_fd);
    }

    snprintf(link_log, sizeof(link_log), "logged_hunt-%s", hunt_id);
    if (access(link_log, F_OK) == -1) {
        symlink(cale_log, link_log);
    }
}

void add_hunt(const char* hunt_id) {
    if (access(hunt_id, F_OK) == -1) {
        if (mkdir(hunt_id, 0777) != 0) {
            perror("Eroare la crearea directorului");
            return;
        }
    }
    log_comanda(hunt_id, "Creare vanatoare");
}

void add_treasure(const char* hunt_id) {
    Treasure t;
    printf("introduceti id-ul:\n");
    scanf("%d", &t.id);
    printf("introduceti numele:\n");
    scanf("%s", t.nume_utilizator);
    printf("introduceti latitudine:\n");
    scanf("%f", &t.latitudine);
    printf("introduceti longitudine:\n");
    scanf("%f", &t.longitudine);
    getchar();
    printf("introduceti indiciu:\n");
    fgets(t.indiciu, MAX_LEN, stdin);
    t.indiciu[strcspn(t.indiciu, "\n")] = '\0';
    printf("introduceti value:\n");
    scanf("%d", &t.value);

    char cale_fisier[128];
    snprintf(cale_fisier, sizeof(cale_fisier), "%s/treasures.dat", hunt_id);

    int fd = open(cale_fisier, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("Eroare la deschiderea fisierului treasures.dat");
        return;
    }

    write(fd, &t, sizeof(Treasure));
    close(fd);

    char msg[128];
    snprintf(msg, sizeof(msg), "Adaugare comoara ID %d", t.id);
    log_comanda(hunt_id, msg);
}

void list_treasures(const char* hunt_id) {
    char cale[128];
    snprintf(cale, sizeof(cale), "%s/treasures.dat", hunt_id);

    struct stat st;
    if (stat(cale, &st) == -1) {
        perror("Nu am gasit fisierul treasures.dat");
        return;
    }

    printf("Nume hunt: %s\nDimensiune: %ld bytes\nUltima modificare: %s\n", hunt_id, st.st_size, ctime(&st.st_mtime));

    int fd = open(cale, O_RDONLY);
    if (fd < 0) {
        perror("Eroare la deschiderea fisierului");
        return;
    }

    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        printf("ID: %d | Nume: %s | Coordonate: %.2f %.2f | Valoare: %d\nIndiciu: %s\n\n",
               t.id, t.nume_utilizator, t.latitudine, t.longitudine, t.value, t.indiciu);
    }

    close(fd);
    log_comanda(hunt_id, "Listare comori");
}

void view_treasure(const char* hunt_id, int id) {
    char cale[128];
    snprintf(cale, sizeof(cale), "%s/treasures.dat", hunt_id);

    int fd = open(cale, O_RDONLY);
    if (fd < 0) {
        perror("Nu pot deschide fisierul");
        return;
    }

    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (t.id == id) {
            printf("ID: %d\nNume: %s\nLatitudine: %.2f\nLongitudine: %.2f\nValoare: %d\nIndiciu: %s\n",
                   t.id, t.nume_utilizator, t.latitudine, t.longitudine, t.value, t.indiciu);
            close(fd);

            char msg[128];
            snprintf(msg, sizeof(msg), "Vizualizare comoara ID %d", id);
            log_comanda(hunt_id, msg);
            return;
        }
    }

    printf("Comoara cu ID-ul %d nu a fost gasita.\n", id);
    close(fd);
}

void remove_treasure(const char* hunt_id, int id) {
    char cale_init[128], cale_tmp[128];
    snprintf(cale_init, sizeof(cale_init), "%s/treasures.dat", hunt_id);
    snprintf(cale_tmp, sizeof(cale_tmp), "%s/tmp.dat", hunt_id);

    int fd1 = open(cale_init, O_RDONLY);
    int fd2 = open(cale_tmp, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd1 < 0 || fd2 < 0) {
        perror("Eroare la deschiderea fisierelor");
        return;
    }

    Treasure t;
    int found = 0;
    while (read(fd1, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (t.id != id) {
            write(fd2, &t, sizeof(Treasure));
        } else {
            found = 1;
        }
    }

    close(fd1);
    close(fd2);

    if (found) {
        rename(cale_tmp, cale_init);
        char msg[128];
        snprintf(msg, sizeof(msg), "Stergere comoara ID %d", id);
        log_comanda(hunt_id, msg);
        printf("Comoara cu id-ul %d a fost stearsa.\n", id);
    } else {
        unlink(cale_tmp);
        printf("Comoara cu ID-ul %d nu a fost gasita.\n", id);
    }
}

void remove_hunt(const char* hunt_id) {
    char path[128];

    snprintf(path, sizeof(path), "%s/treasures.dat", hunt_id);
    unlink(path);

    snprintf(path, sizeof(path), "%s/logged_hunt", hunt_id);
    unlink(path);

    snprintf(path, sizeof(path), "logged_hunt-%s", hunt_id);
    unlink(path);

    rmdir(hunt_id);

    printf("Vanatoarea %s a fost stearsa complet.\n", hunt_id);
}
// int main() {
//     add_hunt("hunt5");
//     add_treasure("hunt5");
//     list_treasures("hunt5");
//     return 0;
// }
