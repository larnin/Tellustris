
#include "Components/TilemapAnimationComponent.h"
#include "Utility/Expression/ExpressionParser.h"

#include <algorithm>


Ndk::ComponentIndex TilemapAnimationComponent::componentIndex;

TilemapAnimationComponent::TilemapAnimationComponent(float startTime)
	: m_time(startTime)
{

}

TilemapAnimationComponent::TilemapAnimationComponent(const TilemapAnimationComponent & map)
{
	*this = map;
}

TilemapAnimationComponent & TilemapAnimationComponent::operator=(const TilemapAnimationComponent & map)
{
	m_time = map.m_time;

	attachTilemap(map.m_tilemap);
	attachAnimations(map.m_tileAnimations);
	for (auto & r : map.m_renderers)
		attachRenderer(r);
	return *this;
}

void TilemapAnimationComponent::attachTilemap(TilemapRef tilemap)
{
	m_mapModifiedEvent.disconnect();

	m_tilemap = tilemap;

	if (m_tilemap)
		m_mapModifiedEvent = m_tilemap->registerTilemapModifiedCallback([this](const Tilemap::TilemapModified & t) {onTilemapUpdate(t.x, t.y); });

	updateAnimations();
}

void TilemapAnimationComponent::attachRenderer(Nz::TileMapRef renderer)
{
	auto it = std::find(m_renderers.begin(), m_renderers.end(), renderer);
	if (it == m_renderers.end())
	{
		m_renderers.push_back(renderer);
	}
}

bool TilemapAnimationComponent::detachRenderer(Nz::TileMapRef renderer)
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

void TilemapAnimationComponent::detachAllRenderers()
{
	m_renderers.clear();
}

void TilemapAnimationComponent::attachAnimations(TilemapAnimationsRef animations)
{
	m_animModifiedEvent.disconnect();

	m_tileAnimations = animations;

	updateAnimations();

	if (m_tileAnimations)
		m_animModifiedEvent = m_tileAnimations->registerAnimationsModifiedCallback([this](const TilemapAnimations::TilemapAnimationsModified & t) {onAnimationUpdate(); });
}

void TilemapAnimationComponent::update(float elapsedTime)
{
	if (!m_tileAnimations)
		return;
	if (!m_tilemap)
		return;

	m_time += elapsedTime;
	auto fTime = m_tileAnimations->frameTime();
	auto tileSize = m_tilemap->tileSize();
	auto tileDelta = m_tilemap->tileDelta();

	std::vector<Nz::Vector2ui> sizes;
	for (const auto & r : m_renderers)
	{
		const auto& material = r->GetMaterial();
		NazaraAssert(material->HasDiffuseMap(), "Sprite material has no diffuse map");
		auto diffuseMap = material->GetDiffuseMap();

		sizes.push_back(Nz::Vector2ui(diffuseMap->GetWidth(), diffuseMap->GetHeight()));
	}

	for (auto & a : m_playingAnimations)
		updateFrame(a, sizes, fTime, tileSize, tileDelta, a.delay);

	for (int i = static_cast<int>(m_tempPlayingAnimations.size()) - 1; i >= 0; i--)
	{
		auto & a = m_tempPlayingAnimations[i];
		if (m_time - a.delay > fTime * m_tileAnimations->animation(a.animationIndex).size())
		{
			auto it = std::find_if(m_playingAnimations.begin(), m_playingAnimations.end(), [a](const auto & p) {return p.x == a.x && p.y == a.y; });
			if (it != m_playingAnimations.end())
				it->overWrite = false;

			removeFrame(m_tempPlayingAnimations[i].x, m_tempPlayingAnimations[i].y);
			m_tempPlayingAnimations[i] = m_tempPlayingAnimations.back();
			m_tempPlayingAnimations.pop_back();

		}
	}

	for (auto & a : m_tempPlayingAnimations)
		updateFrame(a, sizes, fTime, tileSize, tileDelta, a.delay);
}

void TilemapAnimationComponent::playAnimation(size_t x, size_t y, unsigned int tileID)
{
	auto it = std::find_if(m_tempPlayingAnimations.begin(), m_tempPlayingAnimations.end(), [x, y](const auto & p) {return p.x == x && p.y == y; });
	if (it != m_tempPlayingAnimations.end())
	{
		*it = m_tempPlayingAnimations.back();
		m_tempPlayingAnimations.pop_back();
	}

	assert(m_tileAnimations->haveAnimation(tileID));
	auto animSize = m_tileAnimations->animation(tileID).size();
	m_tempPlayingAnimations.push_back(PlayingAnimation{ m_tileAnimations->index(tileID), x, y, animSize, m_time, false });

	it = std::find_if(m_playingAnimations.begin(), m_playingAnimations.end(), [x, y](const auto & p) {return p.x == x && p.y == y; });
	if (it != m_playingAnimations.end())
		it->overWrite = true;
}

void TilemapAnimationComponent::onTilemapUpdate(size_t x, size_t y)
{
	if (!m_tileAnimations)
		return;
	if (!m_tilemap)
		return;

	if (x > m_tilemap->width() || y > m_tilemap->height())
		updateAnimations();
	else
	{
		auto it = std::find_if(m_playingAnimations.begin(), m_playingAnimations.end(), [x, y](const auto & t) { return x == t.x && y == t.y; });
		if (it != m_playingAnimations.end())
		{
			*it = m_playingAnimations.back();
			m_playingAnimations.pop_back();
		}

		it = std::find_if(m_tempPlayingAnimations.begin(), m_tempPlayingAnimations.end(), [x, y](const auto & t) {return x == t.x && y == t.y; });
		if (it != m_tempPlayingAnimations.end())
		{
			*it = m_tempPlayingAnimations.back();
			m_tempPlayingAnimations.pop_back();
		}

		addTile(x, y, m_playingAnimations, m_tilemap->getTile(x, y).id);
	}
}

void TilemapAnimationComponent::onAnimationUpdate()
{
	updateAnimations();
}

void TilemapAnimationComponent::updateAnimations()
{
	m_playingAnimations.clear();
	m_tempPlayingAnimations.clear();

	if (!m_tileAnimations)
		return;
	if (!m_tilemap)
		return;

	for(size_t x = 0; x < m_tilemap->width(); x++)
		for (size_t y = 0; y < m_tilemap->height(); y++)
			addTile(x, y, m_playingAnimations, m_tilemap->getTile(x, y).id);
}

void TilemapAnimationComponent::addTile(size_t x, size_t y, std::vector<TilemapAnimationComponent::PlayingAnimation> & vect, unsigned int tile)
{
	if (!m_tileAnimations->haveAnimation(tile))
		return;
	auto expression = m_tileAnimations->animationExpression(tile);
	expression.resetParameters();
	expression.setParameter("x", float(x) / m_tilemap->width());
	expression.setParameter("y", float(y) / m_tilemap->height());
	auto animSize = m_tileAnimations->animation(tile).size();
	float time = expression.compute();
	time *= m_tileAnimations->frameTime() * animSize;
	vect.push_back(PlayingAnimation{ m_tileAnimations->index(tile), x, y, animSize, expression.compute() , false});
	expression.resetParameters();

	auto it = std::find_if(m_tempPlayingAnimations.begin(), m_tempPlayingAnimations.end(), [x, y](const auto & p) {return p.x == x && p.y == y; });
	if (it != m_tempPlayingAnimations.end())
		vect.back().overWrite = true;
}


void TilemapAnimationComponent::updateFrame(PlayingAnimation & a, const std::vector<Nz::Vector2ui> & renderersSize, float fTime, unsigned int tileSize, unsigned int tileDelta, float delay)
{
	if (a.overWrite)
		return;

	const auto & anim = m_tileAnimations->animation(a.animationIndex);
	size_t index = static_cast<size_t>((m_time - a.delay) / fTime) % anim.size();

	if (index == a.currentFrameIndex)
		return;

	a.currentFrameIndex = index;

	for (size_t i = 0; i < m_renderers.size(); i++)
	{
		auto tileSpace = tileSize + tileDelta;
		auto nbWidth = (renderersSize[i].x + m_tilemap->tileDelta()) / tileSpace;
		assert(nbWidth > 0);

		float invWidth = 1.f / renderersSize[i].x;
		float invHeight = 1.f / renderersSize[i].y;

		auto tX = anim[a.currentFrameIndex] % nbWidth;
		auto tY = anim[a.currentFrameIndex] / nbWidth;

		Nz::Rectf rect(tX * tileSpace * invWidth, tY * tileSpace * invHeight, tileSize * invWidth, tileSize * invHeight);
		m_renderers[i]->EnableTile(Nz::Vector2ui(static_cast<unsigned int>(a.x), static_cast<unsigned int>(a.y)), rect);
	}
}

void TilemapAnimationComponent::removeFrame(size_t x, size_t y)
{
	auto it = std::find_if(m_playingAnimations.begin(), m_playingAnimations.end(), [x, y](const auto & t) { return x == t.x && y == t.y; });
	if (it != m_playingAnimations.end())
	{
		it->currentFrameIndex = ~0;
		return;
	}

	if (!m_tilemap)
		return;

	auto id = m_tilemap->getTile(x, y).id;

	if (id == 0)
	{
		for (auto & r : m_renderers)
			r->DisableTile(Nz::Vector2ui(static_cast<unsigned int>(x), static_cast<unsigned int>(y)));
		return;
	}

	auto tileSize = m_tilemap->tileSize();
	auto tileDelta = m_tilemap->tileDelta();

	for (auto & r : m_renderers)
	{
		const auto& material = r->GetMaterial();
		NazaraAssert(material->HasDiffuseMap(), "Sprite material has no diffuse map");
		auto diffuseMap = material->GetDiffuseMap();

		auto tileSpace = tileSize + tileDelta;
		auto nbWidth = (diffuseMap->GetWidth() + m_tilemap->tileDelta()) / tileSpace;
		assert(nbWidth > 0);

		float invWidth = 1.f / diffuseMap->GetWidth();
		float invHeight = 1.f / diffuseMap->GetHeight();

		auto tX = id % nbWidth;
		auto tY = id / nbWidth;

		Nz::Rectf rect(tX * tileSpace * invWidth, tY * tileSpace * invHeight, tileSize * invWidth, tileSize * invHeight);
		r->EnableTile(Nz::Vector2ui(static_cast<unsigned int>(x), static_cast<unsigned int>(y)), rect);
	}
}