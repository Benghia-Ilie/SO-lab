#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
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
        scanf("%s", t.nume_utilizator);
        printf("introduceti latitudine:\n");
        scanf("%f", &t.latitudine);
        printf("introduceti longitudine:\n");
        scanf("%f", &t.longitudine);
        getchar();
        printf("introduceti indiciu:\n");
        fgets(t.indiciu, MAX_LEN, stdin);
        t.indiciu[strcspn(t.indiciu, "\n")] = '\0'; // Remove trailing newline
        printf("introduceti value:\n");
        scanf("%d", &t.value);
    
        char nume_folder[10];
        snprintf(nume_folder, sizeof(nume_folder), "%d", hunt_id);
        result = chdir(nume_folder);
        if (result != 0)
        {
            perror("Error changing directory");
            return;
        }
    
        char nume_fisier[10];
        snprintf(nume_fisier, sizeof(nume_fisier), "%d", t.id);
    
        // Use open to create and write to the file
        int file = open(nume_fisier, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (file < 0)
        {
            perror("Error opening file");
            chdir(".."); // Return to the parent directory
            return;
        }
    
        // Write treasure data to the file
        dprintf(file, "%d\n", t.id);
        dprintf(file, "%s\n", t.nume_utilizator);
        dprintf(file, "%f\n", t.latitudine);
        dprintf(file, "%f\n", t.longitudine);
        dprintf(file, "%s\n", t.indiciu);
        dprintf(file, "%d\n", t.value);
    
        close(file); // Close the file descriptor
        chdir(".."); // Return to the parent directory
    }
    
    void add_hunt(int hunt_id)
    {
        char nume_folder[10];
        snprintf(nume_folder, sizeof(nume_folder), "%d", hunt_id);
        printf("Creating hunt directory: %s\n", nume_folder);
    
        if (stat(nume_folder, &st) == -1)
        {
            if (mkdir(nume_folder, 0777) != 0)
            {
                perror("Error creating directory");
            }
        }
    }
    void remove_treasure(int hunt_id, int id)
    {
        char cale[64], cale_tmp[64];
        snprintf(cale, sizeof(cale), "%d/treasure.dat", hunt_id);
        snprintf(cale_tmp, sizeof(cale_tmp), "%d/tmp.dat", hunt_id);

        int file_in = open(cale, O_RDONLY);
        int file_out = open(cale_tmp, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(file_out < 0 || file_in < 0)
        {
            perror("Error opening file");
            return;
        }
        Treasure t;
        int gasit = 0;
        while(read(file_in, &t, sizeof(Treasure)) == sizeof(Treasure))
        {
            if(t.id != id)
            {
                write(file_out, &t, sizeof(Treasure));
            }
            else
            {
                gasit = 1;
            }
        }

        close(file_in);
        close(file_out);   
    }

    int main()
    {
        add_hunt(8);
        //add_treasure(8);
        remove_treasure(8, 1);
        


    
        return 0;
    }