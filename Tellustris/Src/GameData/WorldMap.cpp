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

Matrix<Tile> WorldMap::getTiles(int x, int y, int width, int height, size_t layer) const
{
	assert(width > 0);
	assert(height > 0);

	auto minChunk = posToWorldChunkPos(x, y);
	auto maxChunk = posToWorldChunkPos(x + width - 1, y + height - 1);

	Matrix<Tile> tiles(width, height);

	for (int i = minChunk.x; i <= maxChunk.x; i++)
	{
		for (int j = minChunk.y; j <= maxChunk.y; j++)
		{
			auto minPos = tilePosToPos(0u, 0u, i, j);
			auto maxPos = tilePosToPos(static_cast<unsigned int>(Chunk::chunkSize - 1), static_cast<unsigned int>(Chunk::chunkSize - 1), i, j);

			Nz::Vector2ui cMin(0, 0);
			Nz::Vector2ui cMax(Chunk::chunkSize - 1, Chunk::chunkSize - 1);

			Nz::Vector2ui tilesMin(0, 0);

			if (minPos.x < x)
				cMin.x = x - minPos.x;
			else
			{
				cMin.x = 0;
				tilesMin.x = minPos.x - x;
			}
			if (minPos.y < y)
				cMin.y = y - minPos.y;
			else
			{
				cMin.y = 0;
				tilesMin.y = minPos.y - y;
			}
			if (static_cast<int>(cMax.x - cMin.x + 1) > width - tilesMin.x)
				cMax.x = width + cMin.x - 1 - tilesMin.x;
			if (static_cast<int>(cMax.y - cMin.y + 1) > height - tilesMin.y)
				cMax.y = height + cMin.y - 1 - tilesMin.y;

			auto& chunk = getChunk(i, j);

			for (unsigned int k = 0; k <= cMax.x - cMin.x; k++)
				for (unsigned int l = 0; l <= cMax.y - cMin.y; l++)
					tiles(k + tilesMin.x, l + tilesMin.y) = chunk.getTile(cMin.x + k, cMin.y + l, layer);
		}
	}

	return tiles;
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
	return worldToLocalChunkPos(posToWorldChunkPos(x, y));
}

Nz::Vector2ui WorldMap::posToTilePos(const Nz::Vector2i & pos) const
{
	return posToTilePos(pos.x, pos.y);
}

Nz::Vector2ui WorldMap::posToTilePos(int x, int y) const
{
	int chunkX = ((x < 0 ? -static_cast<int>(Chunk::chunkSize) + 1 : 0) + x) / static_cast<int>(Chunk::chunkSize);
	int chunkY = ((y < 0 ? -static_cast<int>(Chunk::chunkSize) + 1 : 0) + y) / static_cast<int>(Chunk::chunkSize);
	chunkX *= static_cast<int>(Chunk::chunkSize);
	chunkY *= static_cast<int>(Chunk::chunkSize);

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
	x = ((x < 0 ? -static_cast<int>(Chunk::chunkSize) + 1 : 0) + x) / static_cast<int>(Chunk::chunkSize);
	y = ((y < 0 ? -static_cast<int>(Chunk::chunkSize) + 1 : 0) + y) / static_cast<int>(Chunk::chunkSize);

	return Nz::Vector2i(x, y);
}

Nz::Vector2ui WorldMap::worldToLocalChunkPos(const Nz::Vector2i & pos) const
{
	return worldToLocalChunkPos(pos.x, pos.y);
}

Nz::Vector2ui WorldMap::worldToLocalChunkPos(int x, int y) const
{
	if (x < 0)
		x = static_cast<int>(m_width) + (x + 1) % static_cast<int>(m_width) - 1;
	else x %= m_width;

	if (y < 0)
		y = static_cast<int>(m_height) + (y + 1) % static_cast<int>(m_height) - 1;
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