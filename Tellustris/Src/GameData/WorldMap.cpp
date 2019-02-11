#include "GameData/WorldMap.h"

#include <numeric>

WorldMap::WorldMap(size_t chunksX, size_t chunksY)
	: m_width(chunksX)
	, m_height(chunksY)
{
	m_chunks.reserve(chunksX * chunksY);

	for(size_t y = 0 ; y < chunksY ; y++)
		for (size_t x = 0; x < chunksX; x++)
			m_chunks.emplace_back(Chunk::New());
}

const Chunk & WorldMap::getChunk(int x, int y) const
{
	auto pos = worldToLocalChunkPos(x, y);
	return *m_chunks[coordToChunkIndex(pos.x, pos.y)];
}

Chunk & WorldMap::getChunk(int x, int y) 
{
	auto pos = worldToLocalChunkPos(x, y);
	return *m_chunks[coordToChunkIndex(pos.x, pos.y)];
}

Tile WorldMap::getTile(int x, int y, size_t layer) const
{
	auto chunkPos = posToChunkPos(x, y);
	auto tilePos = posToTilePos(x, y);

	return m_chunks[coordToChunkIndex(chunkPos.x, chunkPos.y)]->getTile(tilePos.x, tilePos.y, layer);
}

void WorldMap::setTile(int x, int y, Tile tile, size_t layer)
{
	auto chunkPos = posToChunkPos(x, y);
	auto tilePos = posToTilePos(x, y);

	m_chunks[coordToChunkIndex(chunkPos.x, chunkPos.y)]->setTile(tilePos.x, tilePos.y, tile, layer);
}

Nz::Vector2ui WorldMap::posToChunkPos(const Nz::Vector2f & pos) const
{
	return posToChunkPos(pos.x, pos.y);
}

Nz::Vector2ui WorldMap::posToChunkPos(float x, float y) const
{
	return posToChunkPos(static_cast<int>(std::floor(x)), static_cast<int>(std::floor(y)));
}

Nz::Vector2ui WorldMap::posToTilePos(const Nz::Vector2f & pos) const
{
	return posToTilePos(pos.x, pos.y);
}

Nz::Vector2ui WorldMap::posToTilePos(float x, float y) const
{
	return posToTilePos(static_cast<int>(std::floor(x)), static_cast<int>(std::floor(y)));
}

Nz::Vector2ui WorldMap::posToChunkPos(const Nz::Vector2i & pos) const
{
	return posToChunkPos(pos.x, pos.y);
}

Nz::Vector2ui WorldMap::posToChunkPos(int x, int y) const
{
	x = (x < 0 ? -1 : 0) + x / Chunk::chunkSize;
	y = (y < 0 ? -1 : 0) + x / Chunk::chunkSize;

	return worldToLocalChunkPos(x, y);
}

Nz::Vector2ui WorldMap::posToTilePos(const Nz::Vector2i & pos) const
{
	return posToTilePos(pos.x, pos.y);
}

Nz::Vector2ui WorldMap::posToTilePos(int x, int y) const
{
	int chunkX = (x < 0 ? -1 : 0) + x / Chunk::chunkSize;
	int chunkY = (y < 0 ? -1 : 0) + y / Chunk::chunkSize;
	chunkX *= Chunk::chunkSize;
	chunkY *= Chunk::chunkSize;

	return Nz::Vector2ui(x - chunkX, y - chunkY);
}

Nz::Vector2i WorldMap::posToWorldChunkPos(const Nz::Vector2f & pos) const
{
	return posToWorldChunkPos(pos.x, pos.y);
}

Nz::Vector2i WorldMap::posToWorldChunkPos(float x, float y) const
{
	return posToWorldChunkPos(static_cast<int>(std::floor(x)), static_cast<int>(std::floor(y)));
}

Nz::Vector2i WorldMap::posToWorldChunkPos(const Nz::Vector2i & pos) const
{
	return posToWorldChunkPos(pos.x, pos.y);
}

Nz::Vector2i WorldMap::posToWorldChunkPos(int x, int y) const
{
	x = (x < 0 ? -1 : 0) + x / Chunk::chunkSize;
	y = (y < 0 ? -1 : 0) + x / Chunk::chunkSize;

	return Nz::Vector2i(x, y);
}

Nz::Vector2ui WorldMap::worldToLocalChunkPos(const Nz::Vector2i & pos) const
{
	return worldToLocalChunkPos(pos.x, pos.y);
}

Nz::Vector2ui WorldMap::worldToLocalChunkPos(int x, int y) const
{
	if (x < 0)
		x = static_cast<int>(m_width) - x % m_width;
	else x %= m_width;

	if (y < 0)
		y = static_cast<int>(m_height) - y % m_height;
	else y %= m_height;

	return Nz::Vector2ui(x, y);
}

Nz::Vector2i WorldMap::tilePosToPos(const Nz::Vector2ui & tilePos, const Nz::Vector2ui & chunkPos) const
{
	return tilePosToPos(tilePos.x, tilePos.y, chunkPos.x, chunkPos.y);
}

Nz::Vector2i WorldMap::tilePosToPos(unsigned int tileX, unsigned int tileY, unsigned int chunkX, unsigned int chunkY) const
{
	return Nz::Vector2i(tileX + chunkX * Chunk::chunkSize, tileY + chunkY * Chunk::chunkSize);
}

Nz::Vector2f WorldMap::tilePosToPos(const Nz::Vector2f & tilePos, const Nz::Vector2ui & chunkPos) const
{
	return tilePosToPos(tilePos.x, tilePos.y, chunkPos.x, chunkPos.y);
}

Nz::Vector2f WorldMap::tilePosToPos(float tileX, float tileY, unsigned int chunkX, unsigned int chunkY) const
{
	return Nz::Vector2f(tileX + chunkX * Chunk::chunkSize, tileY + chunkY * Chunk::chunkSize);
}

size_t WorldMap::coordToChunkIndex(size_t x, size_t y) const
{
	assert(x < m_width && y < m_height);

	return x + y * m_width;
}