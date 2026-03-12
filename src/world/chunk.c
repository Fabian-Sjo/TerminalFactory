#include "chunk.h"


void setChunkTile(Chunk *chunk, int x, int y, Tile *tile){
	chunk->tiles[x][y] = tile;
}

Tile *getChunkTile(Chunk *chunk, int x, int y){
	return (chunk->tiles[x][y]);
}
void setChunkGroundTile(Chunk *chunk, int x, int y, GroundTile *tile){
	chunk->groundTiles[x][y] = tile;
}

GroundTile *getChunkGroundTile(Chunk *chunk, int x, int y){
	return (chunk->groundTiles[x][y]);
}
