#include "GameData/TileRenderDefinition.h"

#include <cassert>

TileRenderDefinition::TileRenderDefinition(Nz::MaterialRef mat)
	: m_mat(mat)
{

}

Tile TileRenderDefinition::renderTile(const LocalTileMatrix & mat, int x, int y)
{
	return mat(1, 1);
}

Nz::MaterialRef TileRenderDefinition::material() const
{
	return m_mat;
}