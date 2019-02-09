#pragma once

#include "Tilemap/Tilemap.h"

#include <vector>

class Chunk
{
	struct TilemapLayer
	{
		TilemapRef tilemap;
		float height = 0.f;
		unsigned int tileCount = 0;
	};

public:
	static const size_t chunkSize = 32;
	static const unsigned int tileSize = 32;
	static const unsigned int tileDelta = 1;
	static const size_t staticLayers = 2; //2 layers are present all the time

	Chunk();

	Tile getTile(size_t x, size_t y, size_t layer) const;
	void setTile(size_t x, size_t y, Tile tile, size_t layer);
	bool haveLayer(size_t layer) const;
	void setLayerHeight(size_t layer, float height);
	float layerHeight(size_t layer) const;

private:
	static bool tilesEqual(const Tile & t1, const Tile & t2);

	std::vector<TilemapLayer> m_tilemaps;
};