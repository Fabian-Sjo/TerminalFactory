typedef struct Map Map;

Map *create();

int add(long key, void *value);

void *get(long key);

void *remove(long key);

int destroy(Map *map);