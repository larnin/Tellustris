#pragma once

#include "Tilemap/Tilemap.h"
#include "Tilemap/TilemapAnimations.h"
#include "Utility/Expression/Expression.h"

#include <NDK/Component.hpp>
#include <Nazara/Graphics/TileMap.hpp>
#include <Nazara/Core/Clock.hpp>

#include <vector>

using TileAnimation = std::vector<unsigned int>;
using TileAnimationExpression = NExpression::Expression<float>;

class TilemapAnimationComponent : public Ndk::Component<TilemapAnimationComponent>
{
	struct TileAnimationInfo
	{
		unsigned int tileID;
		TileAnimation animation;
		TileAnimationExpression expression;
	};

	struct PlayingAnimation
	{
		size_t animationIndex;
		size_t x;
		size_t y;
		size_t currentFrameIndex;
		float delay;
		bool overWrite;
	};

public:
	TilemapAnimationComponent(float startTime = 0);
	TilemapAnimationComponent(const TilemapAnimationComponent & map);
	TilemapAnimationComponent & operator=(const TilemapAnimationComponent & map);

	void attachTilemap(TilemapRef tilemap);
	void attachRenderer(Nz::TileMapRef renderer);
	bool detachRenderer(Nz::TileMapRef renderer);
	void detachAllRenderers();

	void attachAnimations(TilemapAnimationsRef animations);

	void playAnimation(size_t x, size_t y, unsigned int tileID);

	void update(float elapsedTime);

	static Ndk::ComponentIndex componentIndex;

private:
	void onTilemapUpdate(size_t x, size_t y);
	void onAnimationUpdate();

	void updateAnimations();
	void addTile(size_t x, size_t y, std::vector<PlayingAnimation> & vect, unsigned int tile);
	void updateFrame(PlayingAnimation & a, const std::vector<Nz::Vector2ui> & renderersSize, float fTime, unsigned int tileSize, unsigned int tileDelta, float delay);

	float m_time;
	std::vector<PlayingAnimation> m_playingAnimations;
	std::vector<PlayingAnimation> m_tempPlayingAnimations;

	TilemapRef m_tilemap;
	TilemapAnimationsRef m_tileAnimations;
	std::vector<Nz::TileMapRef> m_renderers;
	EventHolder<Tilemap::TilemapModified> m_mapModifiedEvent;
	EventHolder<TilemapAnimations::TilemapAnimationsModified> m_animModifiedEvent;
};