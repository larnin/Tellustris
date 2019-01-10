
#include "Animator/Animator.h"

#include <algorithm>
#include <cassert>

unsigned int Animator::addAnimation(const std::string & name, AnimationRef anim, bool xFlipped, bool yFlipped)
{
	assert(!animationExist(name));

	m_states.push_back({ name, anim, xFlipped, yFlipped, {} });
	return static_cast<unsigned int>(m_states.size() - 1);
}

void Animator::removeAnimation(const std::string & name)
{
	return removeAnimation(animationIndex(name));
}

void Animator::removeAnimation(unsigned int index)
{
	assert(animationExist(index));

	m_transitions.erase(std::remove_if(m_transitions.begin(), m_transitions.end(), [index](const auto t)
	{
		return t.fromState == index || t.toState == index;
	}), m_transitions.end());

	for (auto & t : m_transitions)
	{
		if (t.fromState > index)
			t.fromState--;
		if (t.toState > index)
			t.toState--;
	}

	m_states.erase(m_states.begin() + index);

	if (m_defaultStateIndex > index)
		m_defaultStateIndex--;
}

unsigned int Animator::animationIndex(const std::string & name) const
{
	assert(!m_states.empty());

	for (unsigned int i = 0; i < m_states.size(); i++)
		if (m_states[i].name == name)
			return i;

	assert(false);
	return 0;
}

std::string Animator::animationName(unsigned int index) const
{
	assert(animationExist(index));
	return m_states[index].name;
}

unsigned int Animator::animationCount() const
{
	return static_cast<unsigned int>(m_states.size());
}

void Animator::renameAnimation(const std::string & name, const std::string & newName)
{
	renameAnimation(animationIndex(name), newName);
}

void Animator::renameAnimation(unsigned int index, const std::string & newName)
{
	assert(animationExist(index));
	assert(!animationExist(newName));

	m_states[index].name = newName;
}

bool Animator::animationExist(const std::string & name) const
{
	for (const auto & s : m_states)
		if (s.name == name)
			return true;
	return false;
}

bool Animator::animationExist(unsigned int index) const
{
	return index <= m_states.size();
}

AnimationRef Animator::animation(const std::string & name) const
{
	return animation(animationIndex(name));
}

AnimationRef Animator::animation(unsigned int index) const
{
	assert(animationExist(index));
	return m_states[index].animation;
}

void Animator::setAnimation(const std::string & name, AnimationRef animation)
{
	setAnimation(animationIndex(name), animation);
}

void Animator::setAnimation(unsigned int index, AnimationRef animation)
{
	assert(animationExist(index));
	m_states[index].animation = animation;
}

bool Animator::isAnimationXFlipped(const std::string & name) const
{
	return isAnimationXFlipped(animationIndex(name));
}

bool Animator::isAnimationXFlipped(unsigned int index) const
{
	assert(animationExist(index));
	return m_states[index].xFlipped;
}

void Animator::setAnimationXFlipped(const std::string & name, bool flipped)
{
	setAnimationXFlipped(animationIndex(name), flipped);
}

void Animator::setAnimationXFlipped(unsigned int index, bool flipped)
{
	assert(animationExist(index));
	m_states[index].xFlipped = flipped;
}

bool Animator::isAnimationYFlipped(const std::string & name) const
{
	return isAnimationYFlipped(animationIndex(name));
}

bool Animator::isAnimationYFlipped(unsigned int index) const
{
	assert(animationExist(index));
	return m_states[index].yFlipped;
}

void Animator::setAnimationYFlipped(const std::string & name, bool flipped)
{
	setAnimationYFlipped(animationIndex(name), flipped);
}

void Animator::setAnimationYFlipped(unsigned int index, bool flipped)
{
	assert(animationExist(index));
	m_states[index].yFlipped = flipped;
}

unsigned int Animator::defaultAnimationIndex() const
{
	return m_defaultStateIndex;
}

std::string Animator::defaultAnimationName() const
{
	return animationName(defaultAnimationIndex());
}

void Animator::setDefaultAnimation(const std::string & name)
{
	setDefaultAnimation(animationIndex(name));
}

void Animator::setDefaultAnimation(unsigned int index)
{
	assert(animationExist(index));
	m_defaultStateIndex = index;
}