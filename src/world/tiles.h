#ifndef TILES_H
#define TILES_H

#include "../graphical/sprite.h"
#include "../utils/vector2.h"

typedef struct World World;
typedef struct GameData GameData;

typedef enum
{
	TILE_NONE = 0,
	TILE_ERROR,
	TILE_BELT,
	TILE_BIG,
	TILE_COUNT
} TileKind;
typedef struct Tile
{
	TileKind kind;
	int instanceID;
	Vector2Int pos;
} Tile;

typedef struct TileDefinition
{
	Sprite (*getSprite)(int instanceID, Vector2Int pos, GameData *gameData);
	void (*tick)(int instanceID, GameData *gameData);
	void (*init)(Tile tile, GameData *gameData);
	void (*destroy)(int instanceID, GameData *gameData);
	Vector2Int size;
	int sizeOfInstance;
	char name[8];
	char icon;
} TileDefinition;

extern const TileDefinition TILE_DEFS[TILE_COUNT];

TileDefinition *getTileDefinition(TileKind kind);
Vector2Int getTileSize(TileKind kind);
Vector2Int getTileOriginOffset(TileKind kind);

#endif
