#ifndef MAP_H
#define MAP_H
typedef struct Map Map;

// Returns the number of entries currently stored in the map.
int mapGetSize(Map *map);

// Creates a new map with the requested initial capacity. The implementation uses a fixed initial backing store and ignores the requested size for capacity growth.
Map *mapCreate(unsigned long size);

// Inserts or updates a value for the provided key and returns 0 on success, 1 when the key already existed, and a negative value is not used by this implementation.
int mapAdd(Map *map, unsigned long long key, void *value);

// Returns the value associated with the provided key, or null if it is absent.
void *mapGet(Map *map, unsigned long long key);

// Removes the value for the given key and returns it, if present. If the key does not exist, the return value is undefined by the current implementation.
void *mapRemove(Map *map, unsigned long long key);

// Destroys the map and frees its internal storage. The current implementation returns 0 on success.
int mapDestroy(Map *map);
#endif
