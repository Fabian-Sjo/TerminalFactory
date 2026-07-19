#ifndef TILES_H
#define TILES_H
#include <stdbool.h>
#include "../graphical/sprite.h"
#include "../utils/vector2.h"

typedef struct World World;
typedef struct GameData GameData;

typedef enum
{
	TILE_NONE = 0,
	TILE_ROCK,
	TILE_ERROR,
	TILE_BELT,
	TILE_BIG,
	TILE_COUNT
} TileKind;
typedef enum Direction
{
	// TODO negative values for previewing tiles, very hacky but it works for now
	DIR_NORTH = -1,
	DIR_EAST = -2,
	DIR_SOUTH = -3,
	DIR_WEST = -4
} Direction;
typedef struct TileEntity
{
	int instanceID;
} TileEntity;

typedef struct Tile
{
	TileKind kind;
	bool isFunctional;
	Vector2Int pos;
	union tiles
	{
		TileEntity entity;
		Sprite sprite;
	};
} Tile;

typedef struct TileDefinition
{
	bool isFunctional;
	// TODO if instanceID is negative, the tile is being previewed and instanceID is used for the direction instead, very ugly but it works for now
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

// Returns the definition for the requested tile kind, or null for invalid kinds.
TileDefinition *getTileDefinition(TileKind kind);
// Returns the sprite to render for the provided tile at the given position. Functional tiles delegate to their definition callback and non-functional tiles use their stored sprite.
Sprite getTileSprite(Tile tile, Vector2Int pos, GameData gameData);
// Returns the logical size of the tile kind in world units. Invalid kinds default to {1,1}.
Vector2Int getTileSize(TileKind kind);
// Returns the tile's origin offset used when rendering or placing it. Invalid kinds return {0,0}.
Vector2Int getTileOriginOffset(TileKind kind);

#endif
