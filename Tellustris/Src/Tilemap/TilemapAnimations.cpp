
#include "Tilemap/TilemapAnimations.h"
#include "Utility/Expression/ExpressionParser.h"

#include <algorithm>
#include <cassert>

TilemapAnimations::TilemapAnimations(float frameTime)
	: m_frameTime(frameTime)
{ }

void TilemapAnimations::registerAnimation(unsigned int tileID, const TileAnimation & animation, const TileAnimationExpression & expression)
{
	assert(!animation.empty());

	auto it = std::find_if(m_tileAnimations.begin(), m_tileAnimations.end(), [tileID](const auto & a) {return a.tileID == tileID; });
	if (it != m_tileAnimations.end())
	{
		it->animation = animation;
		it->expression = expression;
	}
	else m_tileAnimations.push_back(TileAnimationInfo{ tileID, animation, expression });

	m_event.send({});
}

void TilemapAnimations::registerAnimation(unsigned int tileID, const TileAnimation & animation, const std::string & expression)
{
	NExpression::ExpressionParser<float> parser;
	registerAnimation(tileID, animation, parser.evaluate(expression));
}

void TilemapAnimations::removeAnimation(unsigned int tileID)
{
	auto it = std::find_if(m_tileAnimations.begin(), m_tileAnimations.end(), [tileID](const auto & a) {return a.tileID == tileID; });
	if (it != m_tileAnimations.end())
	{
		*it = m_tileAnimations.back();
		m_tileAnimations.pop_back();
		m_event.send({});
	}
}

bool TilemapAnimations::haveAnimation(unsigned int tileID) const
{
	return std::find_if(m_tileAnimations.begin(), m_tileAnimations.end(), [tileID](const auto & a) {return a.tileID == tileID; }) != m_tileAnimations.end();
}

TileAnimation TilemapAnimations::animation(unsigned int tileID) const
{
	assert(haveAnimation(tileID));
	
	auto it = std::find_if(m_tileAnimations.begin(), m_tileAnimations.end(), [tileID](const auto & a) {return a.tileID == tileID; });
	assert(it != m_tileAnimations.end());
	return it->animation;
}

TileAnimationExpression TilemapAnimations::animationExpression(unsigned int tileID) const
{
	assert(haveAnimation(tileID));

	auto it = std::find_if(m_tileAnimations.begin(), m_tileAnimations.end(), [tileID](const auto & a) {return a.tileID == tileID; });
	assert(it != m_tileAnimations.end());
	return it->expression;
}

size_t TilemapAnimations::index(unsigned int tileID) const
{
	assert(haveAnimation(tileID));

	auto it = std::find_if(m_tileAnimations.begin(), m_tileAnimations.end(), [tileID](const auto & a) {return a.tileID == tileID; });
	assert(it != m_tileAnimations.end());
	return std::distance(m_tileAnimations.begin(), it);
}

const TileAnimation & TilemapAnimations::animation(size_t index) const
{
	assert(index < m_tileAnimations.size());
	return m_tileAnimations[index].animation;
}

void TilemapAnimations::setFrameTime(float time)
{
	m_frameTime = time;
	m_event.send({});
}

float TilemapAnimations::frameTime() const
{
	return m_frameTime;
}