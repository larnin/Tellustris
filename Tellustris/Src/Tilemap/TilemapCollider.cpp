
#include "Tilemap/TilemapCollider.h"

#include <vector>
#include <cassert>

void TilemapCollider::addTile(unsigned int tileID, unsigned int layer)
{
	auto it = m_tiles.find(tileID);
	if (it != m_tiles.end())
		it->second = layer;
	else m_tiles.emplace(tileID, layer);

	m_event.send({});
}

void TilemapCollider::removeTile(unsigned int tileID)
{
	auto it = m_tiles.find(tileID);
	if (it != m_tiles.end())
	{
		m_tiles.erase(it);
		m_event.send({});
	}
}

bool TilemapCollider::haveTile(unsigned int tileID) const
{
	return m_tiles.find(tileID) != m_tiles.end(); 
}

unsigned int TilemapCollider::layer(unsigned int tileID) const
{
	assert(haveTile(tileID));
	return m_tiles.find(tileID)->second;
}

std::vector<unsigned int> TilemapCollider::tiles(unsigned int layer) const
{
	std::vector<unsigned int> tiles;

	for (const auto & t : m_tiles)
		if (t.second == layer)
			tiles.push_back(t.first);
	return tiles;
}
