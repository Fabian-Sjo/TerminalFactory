
typedef enum
{
	TILE_NONE,
	TILE_PART_OF_ENTITY,
	TILE_ENTITY
} TileType;

/*
has type
	EMPTY(type)
	has data
		PART OF MULTI (parent, type(gets real type from parent))
		has state
			SINGLE (1 sprite, state, type)
			MULTI CORE (n sprites, state, type)
			*/
typedef struct Data
{
	struct // PART OF ENTITY
	{
		int originX, originY;
	};
	struct // ENTITY
	{
		int state;
		int sprite;
	};
} Data;

typedef struct
{

	TileType type;
	Data *data;

} Tile;

void getType();
void getSprite();
void getState();