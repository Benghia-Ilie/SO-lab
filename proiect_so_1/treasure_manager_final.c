#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#define MAX_LEN 128

// definire structura pentru a retine informatiile unei comori
struct stat st = {0};

typedef struct 
{
    int id;
    char nume_utilizator[MAX_LEN];
    float latitudine;
    float longitudine;
    char indiciu[MAX_LEN];
    int value;
} Treasure;

// functie care scrie in fisierul de log pentru o anumita vanatoare
void log_comanda(int hunt_id, const char* mesaj)
{
    char cale_log[64], link_log[64];
    snprintf(cale_log, sizeof(cale_log), "%d/logged_hunt", hunt_id);

    int log_fd = open(cale_log, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd >= 0)
    {
        dprintf(log_fd, "%s\n", mesaj);
        close(log_fd);
    }

    snprintf(link_log, sizeof(link_log), "logged_hunt-%d", hunt_id);
    if (access(link_log, F_OK) == -1)
    {
        symlink(cale_log, link_log);
    }
}

// functie care creeaza directorul pentru o vanatoare noua
void add_hunt(int hunt_id)
{
    char nume_folder[10];
    snprintf(nume_folder, sizeof(nume_folder), "%d", hunt_id);
    printf("Creare director pentru hunt: %s\n", nume_folder);

    if (stat(nume_folder, &st) == -1)
    {
        if (mkdir(nume_folder, 0777) != 0)
        {
            perror("Eroare la crearea directorului");
            return;
        }
    }

    log_comanda(hunt_id, "Creare vanatoare");
}

// functie care adauga o comoara in fisierul treasures.dat dintr-un anumit director
void add_treasure(int hunt_id)
{
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

    char nume_folder[10], cale_fisier[64];
    snprintf(nume_folder, sizeof(nume_folder), "%d", hunt_id);
    snprintf(cale_fisier, sizeof(cale_fisier), "%s/treasures.dat", nume_folder);

    int file = open(cale_fisier, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (file < 0)
    {
        perror("Eroare la deschiderea fisierului treasures.dat");
        return;
    }

    write(file, &t, sizeof(Treasure));
    close(file);

    printf("Comoara a fost adaugata cu succes.\n");

    char mesaj_log[256];
    snprintf(mesaj_log, sizeof(mesaj_log), "Adaugare comoara ID %d", t.id);
    log_comanda(hunt_id, mesaj_log);
}

// functie care afiseaza toate comorile dintr-o anumita vanatoare
void list_treasures(int hunt_id)
{
    char cale[64];
    snprintf(cale, sizeof(cale), "%d/treasures.dat", hunt_id);

    struct stat st;
    if (stat(cale, &st) == -1)
    {
        perror("Nu am gasit fisierul treasures.dat");
        return;
    }

    printf("Nume hunt: %d\nDimensiune: %ld bytes\nUltima modificare: %s\n", hunt_id, st.st_size, ctime(&st.st_mtime));

    int fd = open(cale, O_RDONLY);
    if (fd < 0)
    {
        perror("Eroare la deschiderea fisierului");
        return;
    }

    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure))
    {
        printf("ID: %d | Nume: %s | Coordonate: %.2f %.2f | Valoare: %d\nIndiciu: %s\n\n",
               t.id, t.nume_utilizator, t.latitudine, t.longitudine, t.value, t.indiciu);
    }

    close(fd);
    log_comanda(hunt_id, "Listare comori");
}

// functie care afiseaza detaliile unei comori specifice, identificata dupa id
void view_treasure(int hunt_id, int id)
{
    char cale[64];
    snprintf(cale, sizeof(cale), "%d/treasures.dat", hunt_id);

    int fd = open(cale, O_RDONLY);
    if (fd < 0)
    {
        perror("Nu pot deschide fisierul");
        return;
    }

    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure))
    {
        if (t.id == id)
        {
            printf("ID: %d\nNume: %s\nLatitudine: %.2f\nLongitudine: %.2f\nValoare: %d\nIndiciu: %s\n",
                   t.id, t.nume_utilizator, t.latitudine, t.longitudine, t.value, t.indiciu);
            close(fd);

            char mesaj_log[256];
            snprintf(mesaj_log, sizeof(mesaj_log), "Vizualizare comoara ID %d", id);
            log_comanda(hunt_id, mesaj_log);
            return;
        }
    }

    printf("Comoara cu ID-ul %d nu a fost gasita.\n", id);
    close(fd);
}

// functie care elimina o comoara din fisierul treasures.dat
void remove_treasure(int hunt_id, int id)
{
    char nume_folder[64];
    snprintf(nume_folder, sizeof(nume_folder), "%d", hunt_id);

    char nume_fisier[64];
    snprintf(nume_fisier, sizeof(nume_fisier), "%s/treasures.dat", nume_folder);

    char fisier_temp[64];
    snprintf(fisier_temp, sizeof(fisier_temp), "%s/tmp.dat", nume_folder);

    int fisier_initial = open(nume_fisier, O_RDONLY);
    int fisier_nou = open(fisier_temp, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fisier_initial < 0 || fisier_nou < 0)
    {
        perror("Eroare la deschiderea fisierelor");
        return;
    }

    Treasure t;
    int gasit = 0;
    while (read(fisier_initial, &t, sizeof(Treasure)) == sizeof(Treasure))
    {
        if (t.id != id)
        {
            write(fisier_nou, &t, sizeof(Treasure));
        }
        else
        {
            gasit = 1;
        }
    }

    close(fisier_initial);
    close(fisier_nou);

    if (gasit)
    {
        rename(fisier_temp, nume_fisier);
        printf("Comoara cu id-ul %d a fost stearsa.\n", id);

        char mesaj_log[256];
        snprintf(mesaj_log, sizeof(mesaj_log), "Stergere comoara ID %d", id);
        log_comanda(hunt_id, mesaj_log);
    }
    else
    {
        unlink(fisier_temp);
        printf("Comoara cu ID-ul %d nu a fost gasita.\n", id);
    }
}

// functie care sterge complet o vanatoare si toate fisierele asociate
void remove_hunt(int hunt_id)
{
    char cale[64];

    snprintf(cale, sizeof(cale), "%d/treasures.dat", hunt_id);
    unlink(cale);

    snprintf(cale, sizeof(cale), "%d/logged_hunt", hunt_id);
    unlink(cale);

    snprintf(cale, sizeof(cale), "logged_hunt-%d", hunt_id);
    unlink(cale);

    snprintf(cale, sizeof(cale), "%d", hunt_id);
    rmdir(cale);

    printf("Vanatoarea %d a fost stearsa complet.\n", hunt_id);
}

// functie principala cu meniu interactiv
int main()
{
    int optiune, hunt_id, id;

    do
    {
        printf("\n--- MENIU ---\n");
        printf("1. Creeaza vanatoare\n");
        printf("2. Adauga comoara\n");
        printf("3. Listeaza comori\n");
        printf("4. Vezi comoara dupa ID\n");
        printf("5. Sterge comoara\n");
        printf("6. Sterge vanatoarea\n");
        printf("0. Iesire\n");
        printf("Selectati optiunea: ");
        scanf("%d", &optiune);

        switch(optiune)
        {
            case 1:
                printf("ID vanatoare: ");
                scanf("%d", &hunt_id);
                add_hunt(hunt_id);
                break;
            case 2:
                printf("ID vanatoare: ");
                scanf("%d", &hunt_id);
                add_treasure(hunt_id);
                break;
            case 3:
                printf("ID vanatoare: ");
                scanf("%d", &hunt_id);
                list_treasures(hunt_id);
                break;
            case 4:
                printf("ID vanatoare: ");
                scanf("%d", &hunt_id);
                printf("ID comoara: ");
                scanf("%d", &id);
                view_treasure(hunt_id, id);
                break;
            case 5:
                printf("ID vanatoare: ");
                scanf("%d", &hunt_id);
                printf("ID comoara: ");
                scanf("%d", &id);
                remove_treasure(hunt_id, id);
                break;
            case 6:
                printf("ID vanatoare: ");
                scanf("%d", &hunt_id);
                remove_hunt(hunt_id);
                break;
            case 0:
                printf("Iesire din program.\n");
                break;
            default:
                printf("Optiune invalida.\n");
        }

    } while(optiune != 0);

    return 0;
}
