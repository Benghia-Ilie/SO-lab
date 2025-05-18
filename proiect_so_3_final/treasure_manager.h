#ifndef TREASURE_MANAGER_H
#define TREASURE_MANAGER_H

#define MAX_LEN 128

typedef struct {
    int id;
    char nume_utilizator[MAX_LEN];
    float latitudine;
    float longitudine;
    char indiciu[MAX_LEN];
    int value;
} Treasure;

void list_treasures(const char* hunt_id);
void view_treasure(const char* hunt_id, int id);
void add_hunt(const char* hunt_id);
void add_treasure(const char* hunt_id);
void remove_treasure(const char* hunt_id, int id);
void remove_hunt(const char* hunt_id);

#endif
