
#include "Animator/Animator.h"
#include "Utility/Expression/ExpressionParser.h"

#include <algorithm>
#include <cassert>

using ConditionExpressionParser = NExpression::ExpressionParser<int>;

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

unsigned int Animator::addTransition(const std::string & fromState, const std::string & toState, const std::string & condition)
{
	return addTransition(animationIndex(fromState), animationIndex(toState), condition);
}

unsigned int Animator::addTransition(const std::string & fromState, const std::string & toState, const ConditionExpression & expression)
{
	return addTransition(animationIndex(fromState), animationIndex(toState), expression);
}

unsigned int Animator::addTransition(unsigned int fromState, unsigned int toState, const std::string & condition)
{
	assert(!haveTransition(fromState, toState));
	assert(animationExist(fromState));
	assert(animationExist(toState));

	ConditionExpressionParser parser;
	m_transitions.push_back({ fromState, toState, condition, parser.evaluate(condition) });
	m_states[fromState].transitionIndexs.push_back(static_cast<unsigned int>(m_transitions.size() - 1));
	return static_cast<unsigned int>(m_transitions.size() - 1);
}

unsigned int Animator::addTransition(unsigned int fromState, unsigned int toState, const ConditionExpression & expression)
{
	assert(!haveTransition(fromState, toState));
	assert(animationExist(fromState));
	assert(animationExist(toState));

	m_transitions.push_back({ fromState, toState, expression.toString(), expression });
	m_states[fromState].transitionIndexs.push_back(static_cast<unsigned int>(m_transitions.size() - 1));
	return static_cast<unsigned int>(m_transitions.size() - 1);
}

void Animator::removeTransition(const std::string & fromState, const std::string & toState)
{
	removeTransition(animationIndex(fromState), animationIndex(toState));
}

void Animator::removeTransition(unsigned int fromState, unsigned int toState)
{
	assert(haveTransition(fromState, toState));

	auto index = getTransition(fromState, toState);
	m_transitions.erase(m_transitions.begin() + index);
	for (auto & s : m_states)
	{
		for (size_t i = 0; i < s.transitionIndexs.size(); i++)
			if (s.transitionIndexs[i] > index)
				s.transitionIndexs[i]--;
	}
}

bool Animator::haveTransition(const std::string & fromState, const std::string & toState) const
{
	return haveTransition(animationIndex(fromState), animationIndex(toState));
}

bool Animator::haveTransition(unsigned int fromState, unsigned int toState) const
{
	auto it = std::find_if(m_transitions.begin(), m_transitions.end(), [fromState, toState](const auto & t) { return t.fromState == fromState && t.toState == toState; });

	return it != m_transitions.end();
}

unsigned int Animator::getTransition(const std::string & fromState, const std::string & toState) const
{
	return getTransition(animationIndex(fromState), animationIndex(toState));
}

unsigned int Animator::getTransition(unsigned int fromState, unsigned int toState) const
{
	assert(haveTransition(fromState, toState));

	auto it = std::find_if(m_transitions.begin(), m_transitions.end(), [fromState, toState](const auto & t) { return t.fromState == fromState && t.toState == toState; });

	return static_cast<unsigned int>(std::distance(m_transitions.begin(), it));
}

unsigned int Animator::transitionsCount() const
{
	return static_cast<unsigned int>(m_transitions.size());
}

std::vector<unsigned int> Animator::outStateTransitionsIndexs(const std::string & state) const
{
	return outStateTransitionsIndexs(animationIndex(state));
}

std::vector<unsigned int> Animator::outStateTransitionsIndexs(unsigned int stateIndex) const
{
	assert(animationExist(stateIndex));

	return m_states[stateIndex].transitionIndexs;
}

std::vector<unsigned int> Animator::inStateTransitionsIndexs(const std::string & state) const
{
	return inStateTransitionsIndexs(animationIndex(state));
}

std::vector<unsigned int> Animator::inStateTransitionsIndexs(unsigned int stateIndex) const
{
	assert(animationExist(stateIndex));

	std::vector<unsigned int> indexs;
	for (unsigned int i = 0; i < m_transitions.size(); i++)
		if (m_transitions[i].toState == stateIndex)
			indexs.push_back(i);

	return indexs;
}

unsigned int Animator::transitionFromStateIndex(unsigned int index) const
{
	assert(index < m_transitions.size());

	return m_transitions[index].fromState;
}

unsigned int Animator::transitionToStateIndex(unsigned int index) const
{
	assert(index < m_transitions.size());

	return m_transitions[index].toState;
}

std::string Animator::transitionConditionString(unsigned int index) const
{
	assert(index < m_transitions.size());

	return m_transitions[index].condition;
}

ConditionExpression Animator::transitionConditionExpression(unsigned int index) const
{
	assert(index < m_transitions.size());

	return m_transitions[index].expression;
}

void Animator::setTransitionCondition(unsigned int index, const std::string & condition)
{
	assert(index < m_transitions.size());

	m_transitions[index].condition = condition;
	ConditionExpressionParser parser;
	m_transitions[index].expression = parser.evaluate(condition);
}

void Animator::setTransitionCondition(unsigned int index, const ConditionExpression & expression)
{
	assert(index < m_transitions.size());

	m_transitions[index].condition = expression.toString();
	m_transitions[index].expression = expression;
}
