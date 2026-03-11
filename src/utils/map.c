#include "map.h"
#include <stdlib.h>

#define INITIAL_SIZE 32

struct Map {
    int capacity;
    int number_of_elements;
    unsigned long long *key;
    void **value;
};

Map *mapCreate(unsigned long size_of_values) {
    Map *map = calloc(1,sizeof(Map));

    unsigned long long *key = calloc(INITIAL_SIZE, sizeof(unsigned long long));
    void **value = calloc(INITIAL_SIZE, size_of_values);

    map->capacity = INITIAL_SIZE;
    map->key = key;
    map->value = value;
    return map;
}

int mapAdd(Map *map, unsigned long long key, void *value) {
    if (mapGet(map, key) != NULL) {
        return 1;
    }
    if (map->capacity <= map->number_of_elements) {
        map->capacity *= 1.5;
        //unsigned long long *oldKeys = map->key;
        //void **oldValues = map->value;
        //map->key = calloc(map->capacity, sizeof(map->key[0]));
        //map->value = calloc(map->capacity, sizeof(map->value[0]));

        map->key = realloc(map->key, map->capacity);
        map->value = realloc(map->value, map->capacity);
    }
    map->key[map->number_of_elements] = key;
    map->value[map->number_of_elements] = value;

    map->number_of_elements++;
    return 0;
}

void *mapGet(Map *map, unsigned long long key) {
    for (int i = 0; i < map->number_of_elements; i++) {
        if (map->key[i] == key) {
          return map->value[i];
        }
    }
    return NULL;
}

void *mapRemove(Map *map, unsigned long long key) {
    void *value;
    int found_index;

    for (int i = 0; i < map->number_of_elements; i++) {
        if (map->key[i] == key) {
            value = map->value[i];
            found_index = i;
            break;
        }
    }
    map->key[found_index] = map->key[map->number_of_elements - 1];
    map->value[found_index] = map->value[map->number_of_elements - 1];
    map->value[map->number_of_elements - 1] = NULL;
    map->number_of_elements--;
	return value;
}

int mapDestroy(Map *map) {
    free(map->key);
    free(map->value);
    free(map);
    map = NULL;
    return 0;
}
