
#include "Components/TilemapColliderComponent.h"
#include "Utility/Matrix.h"

#include <NDK/World.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/CollisionComponent2D.hpp>
#include <Nazara/Physics2D/Collider2D.hpp>

Ndk::ComponentIndex TilemapColliderComponent::componentIndex;

TilemapColliderComponent::TilemapColliderComponent(const TilemapColliderComponent & map)
{
	*this = map;
}

TilemapColliderComponent & TilemapColliderComponent::operator=(const TilemapColliderComponent & map)
{
	attachTilemap(map.m_tilemap);
	attachColliders(map.m_colliders);

	return *this;
}

void TilemapColliderComponent::attachTilemap(TilemapRef tilemap)
{
	m_mapModifiedEvent.disconnect();

	m_tilemap = tilemap;

	if (m_tilemap)
		m_mapModifiedEvent = m_tilemap->registerTilemapModifiedCallback([this](const auto & e) {onTilemapModified(e.x, e.y); });
}

void TilemapColliderComponent::attachColliders(TilemapColliderRef collider)
{
	m_colliderModifiedEvent.disconnect();

	m_colliders = collider;

	if (m_colliders)
		m_colliderModifiedEvent = m_colliders->registerAnimationsModifiedCallback([this](const auto &) {onColliderModified(); });
}

void TilemapColliderComponent::onTilemapModified(size_t x, size_t y)
{
	assert(m_tilemap);

	if (!m_colliders)
		return;

	if (x >= m_tilemap->width() || y >= m_tilemap->height())
		updateLayers();

	updateLayer(m_colliders->layer(m_tilemap->getTile(x, y)));
}

void TilemapColliderComponent::onColliderModified()
{
	updateLayers();
}


void TilemapColliderComponent::updateLayers()
{
	for (auto & e : m_layers)
		e.entity->Kill();
	m_layers.clear();

	for(size_t x = 0; x < m_tilemap->width(); x++)
		for (size_t y = 0; y < m_tilemap->height(); y++)
		{
			auto id = m_colliders->layer(m_tilemap->getTile(x, y));
			if (std::find_if(m_layers.begin(), m_layers.end(), [id](const auto & l)
			{ return id == l.id; }) == m_layers.end())
				updateLayer(id);
		}
}

void TilemapColliderComponent::updateLayer(unsigned int index)
{
	auto it = std::find_if(m_layers.begin(), m_layers.end(), [index](const auto & l) { return index == l.id; });
	if (it != m_layers.end())
	{
		it->entity->Kill();
		*it = m_layers.back();
		m_layers.pop_back();
	}
	
	auto thisEntity = GetEntity();
	assert(thisEntity->HasComponent<Ndk::NodeComponent>());
	auto thisNode = thisEntity->GetComponent<Ndk::NodeComponent>();

	auto entity = thisEntity->GetWorld()->CreateEntity();
	auto & node = entity->AddComponent<Ndk::NodeComponent>();
	node.SetParent(thisNode);

	Matrix<bool> mat(m_tilemap->width(), m_tilemap->height(), false);

	std::vector<Nz::Collider2DRef> colliders;
	float tileSize = static_cast<float>(m_tilemap->tileSize());

	for (size_t x = 0; x < m_tilemap->width(); x++)
		for (size_t y = 0; y < m_tilemap->height(); y++)
		{
			if (mat(x, y))
				continue;

			if (m_colliders->layer(m_tilemap->getTile(x, y)) == index)
			{
				size_t width = 1;
				for (size_t i = 1; i + x < m_tilemap->width(); i++)
					if (m_colliders->layer(m_tilemap->getTile(x + i, y)) != index)
						break;
					else width++;
				size_t height = 1;
				for (size_t j = 1; j + y < m_tilemap->height(); j++)
				{
					bool allValid = true;
					for(size_t i = 0; i < width; i++)
						if (m_colliders->layer(m_tilemap->getTile(x + i, y + j)) != index)
						{
							allValid = false;
							break;
						}
					if (allValid)
						height++;
				}

				for (size_t i = 0; i < width; i++)
					for (size_t j = 0; j < height; j++)
						mat(x + i, y + j) = true;

				colliders.push_back(Nz::BoxCollider2D::New(Nz::Rectf(x * tileSize, y * tileSize, width * tileSize, height * tileSize)));
			}
		}


	Nz::CompoundCollider2DRef collider = Nz::CompoundCollider2D::New(colliders);
	auto & collision = entity->AddComponent<Ndk::CollisionComponent2D>();
	collision.SetGeom(collider);

	m_layers.push_back(ColliderLayer{ index, entity });
}
