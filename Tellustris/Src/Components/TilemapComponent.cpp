
#include "Components/TilemapComponent.h"

Ndk::ComponentIndex TilemapComponent::componentIndex;

TilemapComponent::TilemapComponent(const TilemapComponent & map)
{
	*this = map;
}

TilemapComponent & TilemapComponent::operator=(const TilemapComponent & map)
{
	attachTilemap(map.m_tilemap);
	for (auto & r : map.m_renderers)
		attachRenderer(r);
	return *this;
}

void TilemapComponent::attachTilemap(TilemapRef tilemap)
{
	m_modifiedEvent.disconnect();

	m_tilemap = tilemap;

	if (m_tilemap)
		m_modifiedEvent = m_tilemap->registerTilemapModifiedCallback([this](const Tilemap::TilemapModified & t) {onTilemapUpdate(t.x, t.y); });
	
	for (auto & r : m_renderers)
		updateRenderer(r);
}

void TilemapComponent::attachRenderer(Nz::TileMapRef renderer)
{
	auto it = std::find(m_renderers.begin(), m_renderers.end(), renderer);
	if (it == m_renderers.end())
	{
		m_renderers.push_back(renderer);
		updateRenderer(renderer);
	}
}

bool TilemapComponent::detachRenderer(Nz::TileMapRef renderer)
{
	auto it = std::find(m_renderers.begin(), m_renderers.end(), renderer);
	if (it != m_renderers.end())
	{
		*it = m_renderers.back();
		m_renderers.pop_back();
		return true;
	}
	return false;
}

void TilemapComponent::detachAllRenderers()
{
	m_renderers.clear();
}

void TilemapComponent::updateRenderer(Nz::TileMapRef renderer)
{
	if (!m_tilemap)
		return;

	const auto& material = renderer->GetMaterial();
	NazaraAssert(material->HasDiffuseMap(), "Sprite material has no diffuse map");
	auto diffuseMap = material->GetDiffuseMap();

	auto width = diffuseMap->GetWidth();
	auto height = diffuseMap->GetHeight();

	auto tileSize = m_tilemap->tileSize();
	auto nbWidth = (width + m_tilemap->tileDelta()) / (tileSize + m_tilemap->tileDelta());
	assert(nbWidth > 0);
	auto tileSpace = m_tilemap->tileDelta() + m_tilemap->tileSize();

	float invWidth = 1.f / diffuseMap->GetWidth();
	float invHeight = 1.f / diffuseMap->GetHeight();

	for(size_t x = 0 ; x < m_tilemap->width() ; x++)
		for (size_t y = 0; y < m_tilemap->height(); y++)
		{
			auto tile = m_tilemap->getTile(x, y);
			if (tile == 0)
				renderer->DisableTile(Nz::Vector2ui(static_cast<unsigned int>(x), static_cast<unsigned int>(y)));
			else
			{
				auto tX = tile % nbWidth;
				auto tY = tile / nbWidth;

				Nz::Rectf rect(tX * tileSpace * invWidth, tY * tileSpace * invHeight, tileSize * invWidth, tileSize * invHeight);
				renderer->EnableTile(Nz::Vector2ui(static_cast<unsigned int>(x), static_cast<unsigned int>(y)), rect);
			}
		}
}

void TilemapComponent::onTilemapUpdate(size_t x, size_t y)
{
	if (!m_tilemap)
		return;

	if(x >= m_tilemap->width() || y >= m_tilemap->height())
		for (auto t : m_renderers)
			updateRenderer(t);
	else
	{
		auto tile = m_tilemap->getTile(x, y);
		for (auto renderer : m_renderers)
		{
			if (tile == 0)
				renderer->DisableTile(Nz::Vector2ui(static_cast<unsigned int>(x), static_cast<unsigned int>(y)));
			else
			{
				const auto& material = renderer->GetMaterial();
				NazaraAssert(material->HasDiffuseMap(), "Sprite material has no diffuse map");
				auto diffuseMap = material->GetDiffuseMap();

				auto width = diffuseMap->GetWidth();
				auto height = diffuseMap->GetHeight();

				auto tileSize = m_tilemap->tileSize();
				auto nbWidth = (width + m_tilemap->tileDelta()) / (tileSize + m_tilemap->tileDelta());
				assert(nbWidth > 0);
				auto tileSpace = m_tilemap->tileDelta() + m_tilemap->tileSize();

				float invWidth = 1.f / diffuseMap->GetWidth();
				float invHeight = 1.f / diffuseMap->GetHeight();

				auto tX = tile % nbWidth;
				auto tY = tile / nbWidth;

				Nz::Rectf rect(tX * tileSpace * invWidth, tY * tileSpace * invHeight, tileSize * invWidth, tileSize * invHeight);
				renderer->EnableTile(Nz::Vector2ui(static_cast<unsigned int>(x), static_cast<unsigned int>(y)), rect);
			}
		}
		
	}
}