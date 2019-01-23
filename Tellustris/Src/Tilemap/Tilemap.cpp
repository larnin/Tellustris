#include "Tilemap/Tilemap.h"

Tilemap::Tilemap(size_t width, size_t height, unsigned int tileSize, unsigned int tileDelta)
	: m_matrix(width, height)
	, m_tileSize(tileSize)
	, m_tileDelta(tileDelta)
{

}

Tilemap::TileType Tilemap::getTile(size_t x, size_t y) const
{
	return m_matrix(x, y);
}

void Tilemap::setTile(size_t x, size_t y, Tilemap::TileType value)
{
	m_matrix(x, y) = value;

	m_event.send({});
}

void Tilemap::setTileSize(unsigned int size)
{
	assert(size > 0);

	m_tileSize = size;

	m_event.send({});
}

void Tilemap::setTileDelta(unsigned int delta)
{
	m_tileDelta = delta;

	m_event.send({});
}