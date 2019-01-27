#pragma once

#include "Utility/Event/Event.h"
#include "Utility/Expression/Expression.h"

#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/ObjectRef.hpp>

class TilemapAnimations;

using TilemapAnimationsRef = Nz::ObjectRef<TilemapAnimations>;
using TilemapAnimationsConstRef = Nz::ObjectRef<const TilemapAnimations>;

using TileAnimation = std::vector<unsigned int>;
using TileAnimationExpression = NExpression::Expression<float>;

class TilemapAnimations : public Nz::RefCounted
{
	struct TileAnimationInfo
	{
		unsigned int tileID;
		TileAnimation animation;
		TileAnimationExpression expression;
	};

public:
	struct TilemapAnimationsModified {};

	TilemapAnimations(float frameTime = 1.f);

	void registerAnimation(unsigned int tileID, const TileAnimation & animation, const TileAnimationExpression & expression);
	void registerAnimation(unsigned int tileID, const TileAnimation & animation, const std::string & expression);
	void removeAnimation(unsigned int tileID);
	bool haveAnimation(unsigned int tileID) const;
	TileAnimation animation(unsigned int tileID) const;
	TileAnimationExpression animationExpression(unsigned int tileID) const;
	size_t index(unsigned int tileID) const;
	const TileAnimation & animation(size_t index) const;

	void setFrameTime(float time);
	float frameTime() const;

	EventHolder<TilemapAnimationsModified> registerAnimationsModifiedCallback(std::function<void(const TilemapAnimationsModified &)> callback)
	{
		return m_event.connect(callback);
	}

	template<typename... Args> static TilemapAnimationsRef New(Args&&... args)
	{
		auto object = std::make_unique<TilemapAnimations>(std::forward<Args>(args)...);
		object->SetPersistent(false);

		return object.release();
	}

private:
	Event<TilemapAnimationsModified> m_event;

	std::vector<TileAnimationInfo> m_tileAnimations;
	float m_frameTime;
};