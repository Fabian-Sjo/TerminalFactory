#ifndef MAP_H
#define MAP_H
typedef struct Map Map;

Map *mapCreate(unsigned long size);

int mapAdd(Map *map, unsigned long long key, void *value);

void *mapGet(Map *map, unsigned long long key);

void *mapRemove(Map *map, unsigned long long key);

int mapDestroy(Map *map);
#endif
