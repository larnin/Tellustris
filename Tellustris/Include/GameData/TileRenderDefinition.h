#pragma once

#include "Tilemap/Tile.h"
#include "Utility/FixedMatrix.h"

#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/ObjectRef.hpp>

class TileRenderDefinition;

using TileRenderDefinitionRef = Nz::ObjectRef<TileRenderDefinition>;
using TileRenderDefinitionConstRef = Nz::ObjectRef<const TileRenderDefinition>;

using LocalTileMatrix = FixedMatrix<Tile, 3, 3>;

class TileRenderDefinition : public Nz::RefCounted
{
public:
	TileRenderDefinition(Nz::MaterialRef mat);

	Tile renderTile(const LocalTileMatrix & mat, int x, int y);
	Nz::MaterialRef material() const;

	template<typename... Args> static TileRenderDefinitionRef New(Args&&... args)
	{
		auto object = std::make_unique<TileRenderDefinition>(std::forward<Args>(args)...);
		object->SetPersistent(false);

		return object.release();
	}

private:
	Nz::MaterialRef m_mat;
};