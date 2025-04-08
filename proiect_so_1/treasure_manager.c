#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX_LEN 128

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

void add_treasure(int hunt_id)
{
    int result;
    Treasure t;

    printf("introduceti id-ul:\n");
    scanf("%d", &t.id);
    printf("introduceti numele:\n");
    scanf("%s", &t.nume_utilizator);
    printf("introduceti latitudine:\n");
    scanf("%f", &t.latitudine);
    printf("introduceti longitudine:\n");
    scanf("%f", &t.longitudine);
    getchar();
    printf("introduceti indiciu:\n");
    scanf("[^\n]", &t.indiciu);
    getchar();
    printf("introduceti value:\n");
    scanf("%d", &t.value);

    char nume_folder[10];
    sprintf(nume_folder, "%d", hunt_id );
    result = chdir(nume_folder);
    if(result == 0)
    {
        //facem fisierul daca acesta nu exista
        char nume_fisier[10];
        sprintf(nume_fisier, "%d", t.id);
        FILE* file = fopen(nume_fisier, "w");
        fprintf(file, "%d\n", t.id);
        fprintf(file, "%s\n", t.nume_utilizator);
        fprintf(file, "%f\n", t.latitudine);
        fprintf(file, "%f\n", t.longitudine);
        fprintf(file, "%s\n", t.indiciu);
        fprintf(file, "%d\n", t.value);

        fclose(file);
    }
    else
    {
        printf("nu exista folderul.\n");
        return;
    }


}

void add_hunt(int hunt_id)
{
    char nume_folder[10];
    //am convertit int hunt_id intr un string nume_folder 
    sprintf(nume_folder, "%d", hunt_id );
    printf("%s\n", nume_folder);


    if(stat(nume_folder, &st) == -1)
    {
        mkdir(nume_folder, 0777);
    }
}

int main()
{
    add_hunt(8);
    add_treasure(8);

    return 0;
}