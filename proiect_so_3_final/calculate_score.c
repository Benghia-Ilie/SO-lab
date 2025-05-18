#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "treasure_manager.h"

#define MAX_USERS 100

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hunt_id>\n", argv[0]);
        return 1;
    }

    char cale[128];
    snprintf(cale, sizeof(cale), "%s/treasures.dat", argv[1]);

    int fd = open(cale, O_RDONLY);
    if (fd < 0) {
        perror("Eroare la deschiderea fisierului treasures.dat");
        return 1;
    }

    Treasure t;
    int scoruri[MAX_USERS] = {0};
    char utilizatori[MAX_USERS][MAX_LEN];
    int utilizatori_count = 0;

    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        int index = -1;
        for (int i = 0; i < utilizatori_count; i++) {
            if (strcmp(utilizatori[i], t.nume_utilizator) == 0) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            if (utilizatori_count >= MAX_USERS) {
                fprintf(stderr, "Prea multi utilizatori.\n");
                close(fd);
                return 1;
            }
            index = utilizatori_count++;
            strncpy(utilizatori[index], t.nume_utilizator, MAX_LEN);
            scoruri[index] = 0;
        }

        scoruri[index] += t.value;
    }

    close(fd);

    for (int i = 0; i < utilizatori_count; i++) {
        printf("%s: %d\n", utilizatori[i], scoruri[i]);
    }

    return 0;
}

