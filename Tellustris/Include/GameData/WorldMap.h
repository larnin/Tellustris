#pragma once

#include "Chunk.h"

#include <vector>

class WorldMap
{
public:
	WorldMap(size_t chunksX, size_t chunksY);

	const Chunk & getChunk(int x, int y) const;
	Chunk & getChunk(int x, int y);

	Tile getTile(int x, int y, size_t layer) const;
	void setTile(int x, int y, Tile tile, size_t layer);
		
	Nz::Vector2ui posToChunkPos(const Nz::Vector2f & pos) const;
	Nz::Vector2ui posToChunkPos(float x, float y) const;
	Nz::Vector2ui posToTilePos(const Nz::Vector2f & pos) const;
	Nz::Vector2ui posToTilePos(float x, float y) const;
	Nz::Vector2ui posToChunkPos(const Nz::Vector2i & pos) const;
	Nz::Vector2ui posToChunkPos(int x, int y) const;
	Nz::Vector2ui posToTilePos(const Nz::Vector2i & pos) const;
	Nz::Vector2ui posToTilePos(int x, int y) const;

	Nz::Vector2ui worldToLocalChunkPos(const Nz::Vector2i & pos) const;
	Nz::Vector2ui worldToLocalChunkPos(int x, int y) const;
	Nz::Vector2i tilePosToPos(const Nz::Vector2ui & tilePos, const Nz::Vector2ui & chunkPos) const;
	Nz::Vector2i tilePosToPos(unsigned int tileX, unsigned int tileY, unsigned int chunkX, unsigned int chunkY) const;
	Nz::Vector2f tilePosToPos(const Nz::Vector2f & tilePos, const Nz::Vector2ui & chunkPos) const;
	Nz::Vector2f tilePosToPos(float tileX, float tileY, unsigned int chunkX, unsigned int chunkY) const;

private:
	size_t coordToChunkIndex(size_t x, size_t y) const;

	size_t m_width;
	size_t m_height;
	std::vector<Chunk> m_chunks;
};