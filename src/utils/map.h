#ifndef MAP_H
#define MAP_H
typedef struct Map Map;

Map *mapCreate(unsigned long size);

int mapAdd(Map *map, long key, void *value);

void *mapGet(Map *map, long key);

void *mapRemove(Map *map, long key);

int mapDestroy(Map *map);
#endif
