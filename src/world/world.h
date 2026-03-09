#include "tiles.h"

typedef struct World World;

void generate(int chunkX, int chunkY);

Tile getTile(World *world, int x, int y);

void setTile(World *world, Tile tile);
