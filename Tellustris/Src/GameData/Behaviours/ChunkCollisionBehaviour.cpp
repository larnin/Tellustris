#include "GameData/Behaviours/ChunkCollisionBehaviour.h"
#include "Utility/Settings.h"
#include "GameData/CollisionDefinition.h"

#include <NDK/World.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/CollisionComponent2D.hpp>

ChunkCollisionBehaviour::ChunkCollisionBehaviour(Chunk & chunk)
	: m_chunk(chunk)
{

}

BehaviourRef ChunkCollisionBehaviour::clone() const
{
	auto collision = std::make_unique<ChunkCollisionBehaviour>(m_chunk);
	return std::move(collision);
}

void ChunkCollisionBehaviour::onEnable()
{
	m_layerChangedHolder = m_chunk.registerLayerChangedCallback([this](const auto & e) {updateCollisions(); });
	updateCollisions();
}

void ChunkCollisionBehaviour::onDisable()
{
	clearCollisions();
	m_mapModified.clear();
	m_layerChangedHolder.disconnect();
}

void ChunkCollisionBehaviour::updateCollisions()
{
	clearCollisions();

	std::vector<unsigned int> createdLayers;

	for (size_t i = 0; i < m_chunk.layerCount(); i++)
	{
		auto layer = m_chunk.getMap(i);

		if (m_mapModified.size() <= i)
			m_mapModified.push_back(layer->registerTilemapModifiedCallback([this](const auto & e) {updateCollisions(); }));

		for(size_t x = 0 ; x < Chunk::chunkSize ; x++)
			for (size_t y = 0; y < Chunk::chunkSize; y++)
			{
				auto collider = layer->getTile(x, y).collider;
				if (!collider.haveCollision())
					continue;

				if (std::find(createdLayers.begin(), createdLayers.end(), collider.collisionLayer) == createdLayers.end())
				{
					createdLayers.push_back(collider.collisionLayer);
					createCollisionLayer(collider.collisionLayer);
				}
			}
	}

	for (size_t i = 0; i < m_layers.size(); i++)
	{
		const auto & l = m_layers[i];
		if (std::find(createdLayers.begin(), createdLayers.end(), l.id) == createdLayers.end())
		{
			l.entity->Kill();
			m_layers[i] = std::move(m_layers.back());
			m_layers.pop_back();
			i--;
		}
	}
}

void ChunkCollisionBehaviour::clearCollisions()
{
	for (auto & layer : m_layers)
		layer.entity->Kill();
	m_layers.clear();
}

void ChunkCollisionBehaviour::createCollisionLayer(unsigned int collisionLayer)
{
	Ndk::EntityHandle entity;
	auto it = std::find_if(m_layers.begin(), m_layers.end(), [collisionLayer](const auto & l) {return l.id == collisionLayer; });
	if (it != m_layers.end())
		entity = it->entity;
	else
	{
		entity = createEntity();
		m_layers.push_back(ColliderLayer{ collisionLayer, entity});
	}

	Matrix<bool> mat(Chunk::chunkSize, Chunk::chunkSize, false);

	std::vector<Nz::Collider2DRef> colliders;

	for (size_t x = 0; x < Chunk::chunkSize; x++)
		for (size_t y = 0; y < Chunk::chunkSize; y++)
		{
			if (!haveFullCollision(x, y, collisionLayer))
			{
				for (size_t i = 0; i < m_chunk.layerCount(); i++)
				{
					auto collider = m_chunk.getTile(x, y, i).collider;
					if (!collider.haveCollision() || collider.collisionLayer != collisionLayer)
						continue;
					
					colliders.push_back(collider.toCollider(Nz::Vector2f(static_cast<float>(x), static_cast<float>(y)), Nz::Vector2f(1, 1)));
				}
			}
			else
			{
				size_t width = 1;
				for (size_t i = 1; i + x < Chunk::chunkSize; i++)
				{
					if (mat(x + i, y))
						break;
					if (!haveFullCollision(x + i, y, collisionLayer))
						break;
					width++;
				}
				size_t height = 1;
				for (size_t j = 1; j + y < Chunk::chunkSize; j++)
				{
					bool allValid = true;
					for (size_t i = 0; i < width; i++)
					{
						bool haveCollision = haveFullCollision(x + i, y + j, collisionLayer);
						if (mat(x + i, y + j) || !haveCollision )
						{
							allValid = false;
							break;
						}
					}
					if (allValid)
						height++;
					else break;
				}

				for (size_t i = 0; i < width; i++)
					for (size_t j = 0; j < height; j++)
						mat(x + i, y + j) = true;

				colliders.push_back(Nz::BoxCollider2D::New(Nz::Rectf(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height))));
			}
		}

	auto & collision = entity->GetComponent<Ndk::CollisionComponent2D>();
	auto collider = Nz::CompoundCollider2D::New(colliders);

	auto def = Settings<CollisionDefinition>::value();
	if (def->haveLayer(collisionLayer))
	{
		collider->SetCategoryMask(1 << collisionLayer);
		collider->SetCollisionMask(def->collisionAndTriggerMask(collisionLayer));
		collider->SetCollisionGroup(0);
		collider->SetCollisionId(collisionLayer);
	}

	collision.SetGeom(collider);
}

Ndk::EntityHandle ChunkCollisionBehaviour::createEntity()
{
	auto entity = getEntity()->GetWorld()->CreateEntity();

	auto & node = entity->AddComponent<Ndk::NodeComponent>();
	node.SetParent(getEntity());
	node.SetPosition(Nz::Vector3f::Zero());
	entity->AddComponent<Ndk::CollisionComponent2D>();

	return entity;
}


bool ChunkCollisionBehaviour::haveFullCollision(size_t x, size_t y, unsigned int collisionLayer) const
{
	for (size_t i = 0; i < m_chunk.layerCount(); i++)
	{
		auto collider = m_chunk.getTile(x, y, i).collider;
		if (collider.collisionLayer == collisionLayer && collider.haveFullCollision())
			return true;
	}
	return false;
}