#pragma once

#include "Animator/Animation.h"
#include "Utility/Expression/Expression.h"

#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/ObjectRef.hpp>

#include <vector>

namespace
{
	using ConditionExpression = NExpression::Expression<int>;

	struct AnimatorState
	{
		std::string name;
		AnimationRef animation;
		bool xFlipped;
		bool yFlipped;
		std::vector<unsigned int> transitionIndexs;
	};

	struct AnimatorTransition
	{
		unsigned int fromState;
		unsigned int toState;

		std::string condition;
		ConditionExpression expression;
	};
}

class Animator;

using AnimatorRef = Nz::ObjectRef<Animator>;
using AnimatorConstRef = Nz::ObjectRef<const Animator>;

class Animator : public Nz::RefCounted
{
public:

	Animator() = default;

	unsigned int addAnimation(const std::string & name, AnimationRef anim, bool xFlipped = false, bool yFlipped = false);
	void removeAnimation(const std::string & name);
	void removeAnimation(unsigned int index);

	unsigned int animationIndex(const std::string & name) const;
	std::string animationName(unsigned int index) const;
	unsigned int animationCount() const;

	void renameAnimation(const std::string & name, const std::string & newName);
	void renameAnimation(unsigned int index, const std::string & newName);

	bool animationExist(const std::string & name) const;
	bool animationExist(unsigned int index) const;
	AnimationRef animation(const std::string & name) const;
	AnimationRef animation(unsigned int index) const;
	void setAnimation(const std::string & name, AnimationRef animation);
	void setAnimation(unsigned int index, AnimationRef animation);

	bool isAnimationXFlipped(const std::string & name) const;
	bool isAnimationXFlipped(unsigned int index) const;
	void setAnimationXFlipped(const std::string & name, bool flipped);
	void setAnimationXFlipped(unsigned int index, bool flipped);
	bool isAnimationYFlipped(const std::string & name) const;
	bool isAnimationYFlipped(unsigned int index) const;
	void setAnimationYFlipped(const std::string & name, bool flipped);
	void setAnimationYFlipped(unsigned int index, bool flipped);

	unsigned int defaultAnimationIndex() const;
	std::string defaultAnimationName() const;
	void setDefaultAnimation(const std::string & name);
	void setDefaultAnimation(unsigned int index);


	unsigned int addTransition(const std::string & fromState, const std::string & toState, const std::string & condition);
	unsigned int addTransition(const std::string & fromState, const std::string & toState, const ConditionExpression & expression);
	unsigned int addTransition(unsigned int fromState, unsigned int toState, const std::string & condition);
	unsigned int addTransition(unsigned int fromState, unsigned int toState, const ConditionExpression & expression);

	void removeTransition(const std::string & fromState, const std::string & toState);
	void removeTransition(unsigned int fromState, unsigned int toState);

	bool haveTransition(const std::string & fromState, const std::string & toState) const;
	bool haveTransition(unsigned int fromState, unsigned int toState) const;
	unsigned int getTransition(const std::string & fromState, const std::string & toState) const;
	unsigned int getTransition(unsigned int fromState, unsigned int toState) const;

	unsigned int transitionsCount() const;
	std::vector<unsigned int> outStateTransitionsIndexs(const std::string & state) const;
	std::vector<unsigned int> outStateTransitionsIndexs(unsigned int stateIndex) const;
	std::vector<unsigned int> inStateTransitionsIndexs(const std::string & state) const;
	std::vector<unsigned int> inStateTransitionsIndexs(unsigned int stateIndex) const;

	unsigned int transitionFromStateIndex(unsigned int index) const;
	unsigned int transitionToStateIndex(unsigned int index) const;

	std::string transitionConditionString(unsigned int index) const;
	ConditionExpression transitionConditionExpression(unsigned int index) const;
	void setTransitionCondition(unsigned int index, const std::string & condition);
	void setTransitionCondition(unsigned int index, const ConditionExpression & expression);

	template<typename... Args> static AnimatorRef New(Args&&... args)
	{
		auto object = std::make_unique<Animator>(std::forward<Args>(args)...);
		object->SetPersistent(false);

		return object.release();
	}

private:
	std::vector<AnimatorState> m_states;
	std::vector<AnimatorTransition> m_transitions;
	unsigned int m_defaultStateIndex = 0;
};