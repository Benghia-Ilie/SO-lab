#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilizare: %s <file_name>\n", argv[0]);
        return 1;
    }

    char *file_name = argv[1];

    int fd_in = open(file_name, O_RDONLY);
    if (fd_in < 0) {
        perror("Eroare la deschiderea fisierului de intrare");
        return 1;
    }

    struct stat st;
    if (fstat(fd_in, &st) < 0) {
        perror("Eroare la fstat");
        close(fd_in);
        return 1;
    }

    int digit_count = 0;
    char buffer[BUF_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(fd_in, buffer, BUF_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; ++i) {
            if (buffer[i] >= '0' && buffer[i] <= '9') {
                digit_count++;
            }
        }
    }

    if (bytes_read < 0) {
        perror("Eroare la citirea fisierului");
        close(fd_in);
        return 1;
    }

    close(fd_in); // Închidem fișierul de intrare

    // Deschidem același fișier pentru suprascriere (scriere output)
    int fd_out = open(file_name, O_WRONLY | O_TRUNC);
    if (fd_out < 0) {
        perror("Eroare la deschiderea fisierului de iesire");
        return 1;
    }

    dprintf(fd_out, "UID: %d\nDimensiune: %ld octeti\nCifre: %d\n",
            st.st_uid, (long)st.st_size, digit_count);

    close(fd_out); 
    unlink("my_link");

    if (symlink(file_name, "my_link") < 0) {
        perror("Eroare la crearea legaturii simbolice");
        return 1;
    }

    printf("Fisier procesat cu succes. Legatura simbolica 'my_link' -> %s creata.\n", file_name);
    return 0;
}
