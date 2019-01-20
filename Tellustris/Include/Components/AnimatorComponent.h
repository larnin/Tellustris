#pragma once

#include "Animator/Animator.h"

#include <Nazara/Graphics/Sprite.hpp>
#include <NDK/Component.hpp>

#include <vector>

class AnimatorComponent : public Ndk::Component<AnimatorComponent>
{
	struct PropertyValue
	{
		std::string name;
		int value;
	};

	struct Transition
	{
		ConditionExpression expression;
		unsigned int nextStateIndex;
	};

public:
	friend class AnimatorSystem;

	AnimatorComponent();

	void attachAnimator(AnimatorRef animator);
	void attachSprite(Nz::SpriteRef sprite);
	bool detachSprite(Nz::SpriteRef sprite);
	void detachAllSprite();

	size_t propertyIndex(const std::string & name); //change the size of the stack if the property is not set yet
	size_t setProperty(const std::string & name, int value); //return the index of the property
	void setProperty(size_t index, int value);
	int property(const std::string & name) const;
	int property(size_t index) const;
	void resetProperties(bool keepIndexs = true);

	void setSpeed(float speed);
	float speed() const;

	void setCurrentState(const std::string & name);
	void setCurrentState(unsigned int index);
	unsigned int currentState() const;
	std::string currentStateName() const;
	AnimatorRef animator() const;

	static Ndk::ComponentIndex componentIndex;

private:
	void update(float elapsedTime);
	void changeState(unsigned int nextState);
	void addDefaultProperties();
	void updateDefaultProperties();
	void updateSprite();

	AnimatorRef m_animator;
	std::vector<Nz::SpriteRef> m_sprites;

	unsigned int m_currentState = 0;
	float m_stateTime = 0.f;
	float m_animationTime = 0.f;
	float m_animationDuration = 0.f;
	std::vector<Transition> m_transitionExpressions;

	std::vector<PropertyValue> m_properties;
	
	float m_speed = 1;

	size_t m_propertyTime = 0; //total duration of the current state
	size_t m_propertyTimeSingle = 0; //duration on the animation, come back to 0 after each loop
	size_t m_propertyFinished = 0; //property set to 1 when the animation is finished, not on loops
	size_t m_propertyLoop = 0; //loop count
};