
#include "GameData/Chunk.h"

#include <cassert>

Chunk::Chunk()
{
	m_tilemaps.push_back(TilemapLayer{ Tilemap::New(chunkSize, chunkSize, tileSize, tileDelta), 0.f, 1 });
	m_tilemaps.push_back(TilemapLayer{ Tilemap::New(chunkSize, chunkSize, tileSize, tileDelta), 0.5f, 1 });
}

Tile Chunk::getTile(size_t x, size_t y, size_t layer) const
{
	assert(x < chunkSize);
	assert(y < chunkSize);

	if (layer >= m_tilemaps.size())
		return {};

	assert(m_tilemaps[layer].tilemap);

	return m_tilemaps[layer].tilemap->getTile(x, y);
}

void Chunk::setTile(size_t x, size_t y, Tile tile, size_t layer)
{
	assert(x < chunkSize);
	assert(y < chunkSize);

	bool haveNew = tilesEqual(tile, {});

	if (layer >= m_tilemaps.size())
	{
		if (!haveNew)
			return;
		for (size_t i = m_tilemaps.size(); i < layer; i++)
		{
			m_tilemaps.push_back(TilemapLayer{ Tilemap::New(chunkSize, chunkSize, tileSize, tileDelta), static_cast<float>(i) - 1 });
			m_event.send(LayerChanged{ i, LayerChanged::ChangeState::added });
		}
	}

	auto t = m_tilemaps[layer].tilemap->getTile(x, y);
	bool haveOld = tilesEqual(t, {});
	m_tilemaps[layer].tilemap->setTile(x, y, tile);

	//tile counting to automaticaly remove layer
	if (layer < staticLayers)
		return;

	if (haveOld == haveNew)
		return;

	if (haveOld)
		m_tilemaps[layer].tileCount--;
	if (haveNew)
		m_tilemaps[layer].tileCount++;

	if (layer == m_tilemaps.size() - 1 && m_tilemaps[layer].tileCount == 0)
	{
		while (m_tilemaps.size() > staticLayers && m_tilemaps.back().tileCount == 0)
		{
			m_event.send(LayerChanged{ m_tilemaps.size() - 1, LayerChanged::ChangeState::removed });
			m_tilemaps.pop_back();
		}
	}
}

TilemapRef Chunk::getMap(size_t layer)
{
	if (!haveLayer(layer))
		return {};
	return m_tilemaps[layer].tilemap;
}

bool Chunk::haveLayer(size_t layer) const
{
	return m_tilemaps.size() > layer;
}

void Chunk::setLayerHeight(size_t layer, float height)
{
	assert(haveLayer(layer));
	m_tilemaps[layer].height = height;
	m_event.send(LayerChanged{ layer, LayerChanged::ChangeState::heightChanged });
}

float Chunk::layerHeight(size_t layer) const
{
	assert(haveLayer(layer));
	return m_tilemaps[layer].height;
}

size_t Chunk::layerCount() const
{
	return m_tilemaps.size();
}

bool Chunk::tilesEqual(const Tile & t1, const Tile & t2)
{
	if (t1.id != t2.id)
		return false;
	if (t1.collider.type == TileColliderType::Empty && t2.collider.type == TileColliderType::Empty)
		return true;

	return t1.collider.toInt() == t2.collider.toInt();
}