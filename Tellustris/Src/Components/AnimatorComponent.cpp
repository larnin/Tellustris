
#include "Components/AnimatorComponent.h"

Ndk::ComponentIndex AnimatorComponent::componentIndex;

constexpr int defaultPropertyValue = 0;

constexpr auto propertyTimeName = "time";
constexpr auto propertyTimeSingleName = "timeSingle";
constexpr auto propertyFinishedName = "finished";
constexpr auto propertyLoopName = "loops";

AnimatorComponent::AnimatorComponent()
{
	addDefaultProperties();
}

void AnimatorComponent::attachAnimator(AnimatorRef animator)
{
	m_animator = animator;
	if (m_animator)
		changeState(m_animator->defaultAnimationIndex());
}

void AnimatorComponent::attachSprite(Nz::SpriteRef sprite)
{
	assert(sprite);
	auto it = std::find(m_sprites.begin(), m_sprites.end(), sprite);
	if (it == m_sprites.end())
		m_sprites.push_back(sprite);
}

bool AnimatorComponent::detachSprite(Nz::SpriteRef sprite)
{
	auto it = std::find(m_sprites.begin(), m_sprites.end(), sprite);
	if (it != m_sprites.end())
	{
		*it = m_sprites.back();
		m_sprites.pop_back();
		return true;
	}
	return false;
}

void AnimatorComponent::detachAllSprite()
{
	m_sprites.clear();
}

size_t AnimatorComponent::propertyIndex(const std::string & name)
{
	for (size_t i = 0; i < m_properties.size(); i++)
		if (m_properties[i].name == name)
			return i;
	m_properties.push_back(PropertyValue{ name, defaultPropertyValue });
	return m_properties.size() - 1;
}

size_t AnimatorComponent::setProperty(const std::string & name, int value)
{
	auto index = propertyIndex(name);
	setProperty(index, value);
	return index;
}

void AnimatorComponent::setProperty(size_t index, int value)
{
	m_properties[index].value = value;
}

int AnimatorComponent::property(const std::string & name) const
{
	for (const auto & p : m_properties)
		if (p.name == name)
			return p.value;
	return defaultPropertyValue;
}

int AnimatorComponent::property(size_t index) const
{
	if (index < m_properties.size())
		return m_properties[index].value;
	return defaultPropertyValue;
}

void AnimatorComponent::resetProperties(bool keepIndexs)
{
	if (!keepIndexs)
	{
		m_properties.clear();
		addDefaultProperties();
		return;
	}

	for (auto & p : m_properties)
		p.value = defaultPropertyValue;
}

void AnimatorComponent::setSpeed(float speed)
{
	m_speed = speed;
}

float AnimatorComponent::speed() const
{
	return m_speed;
}

void AnimatorComponent::setCurrentState(const std::string & name)
{
	assert(m_animator);
	setCurrentState(m_animator->animationIndex(name));
}

void AnimatorComponent::setCurrentState(unsigned int index)
{
	assert(m_animator);
	assert(index < m_animator->animationCount());

	changeState(index);
}

unsigned int AnimatorComponent::currentState() const
{
	return m_currentState;
}

std::string AnimatorComponent::currentStateName() const
{
	assert(m_animator);
	return m_animator->animationName(m_currentState);
}

AnimatorRef AnimatorComponent::animator() const
{
	return m_animator;
}

void AnimatorComponent::update(float elapsedTime)
{
	if (!m_animator)
		return;

	m_animationTime += elapsedTime * m_speed;
	auto anim = m_animator->animation(m_currentState);
	assert(anim);
	if(anim->loop)
		m_animationTime = fmod(m_animationTime, m_animationDuration);
	else m_animationTime = std::min(m_animationTime, m_animationDuration);
	m_stateTime += elapsedTime * m_speed;

	updateDefaultProperties();
	for (auto & t : m_transitionExpressions)
	{
		t.expression.resetParameters();

		for (const auto & p : m_properties)
			t.expression.setParameter(p.name, p.value);

		if (t.expression.compute())
		{
			changeState(t.nextStateIndex);
			return;
		}
	}

	updateSprite();
}

void AnimatorComponent::changeState(unsigned int nextState)
{
	assert(m_animator);
	assert(nextState < m_animator->animationCount());

	m_transitionExpressions.clear();
	m_currentState = nextState;
	m_stateTime = 0.f;
	m_animationTime = 0.f;

	auto anim = m_animator->animation(m_currentState);
	assert(anim);
	m_animationDuration = anim->duration();

	auto transitionsIndexs = m_animator->outStateTransitionsIndexs(m_currentState);
	for (auto index : transitionsIndexs)
	{
		Transition t;
		t.expression = m_animator->transitionConditionExpression(index);
		t.nextStateIndex = m_animator->transitionToStateIndex(index);
		m_transitionExpressions.push_back(t);
	}

	updateSprite();
}

void AnimatorComponent::addDefaultProperties()
{
	m_propertyTime = propertyIndex(propertyTimeName);
	m_propertyTimeSingle = propertyIndex(propertyTimeSingleName);
	m_propertyFinished = propertyIndex(propertyFinishedName);
	m_propertyLoop = propertyIndex(propertyLoopName);
}

void AnimatorComponent::updateDefaultProperties()
{
	assert(m_animator);

	setProperty(m_propertyTime, static_cast<int>(m_stateTime));
	setProperty(m_propertyTimeSingle, static_cast<int>(m_animationTime));
	auto anim = m_animator->animation(m_currentState);
	assert(anim);
	int loopNb = static_cast<int>(m_stateTime / m_animationDuration);
	if (!anim->loop)
		loopNb = std::min(1, loopNb);
	setProperty(m_propertyFinished, !anim->loop && loopNb > 0);
	setProperty(m_propertyLoop, loopNb);
}

void AnimatorComponent::updateSprite()
{
	assert(m_animator);
	auto anim = m_animator->animation(m_currentState);
	assert(anim);

	auto frame = anim->current(m_animationTime, false);

	for (auto & s : m_sprites)
	{
		//does the same than SetTextureRect to be able to flip uvs
		const auto& material = s->GetMaterial();
		NazaraAssert(material->HasDiffuseMap(), "Sprite material has no diffuse map");
		auto diffuseMap = material->GetDiffuseMap();

		float invWidth = 1.f / diffuseMap->GetWidth();
		float invHeight = 1.f / diffuseMap->GetHeight();
		
		Nz::Rectf rect(invWidth * frame.rect.x, invHeight * frame.rect.y, invWidth * frame.rect.width, invHeight * frame.rect.height);
		if (m_animator->isAnimationXFlipped(m_currentState))
		{
			rect.width *= -1;
			rect.x -= rect.width;
		}
		if (m_animator->isAnimationYFlipped(m_currentState))
		{
			rect.height *= -1;
			rect.y -= rect.height;
		}

		s->SetTextureCoords(rect);
		s->SetSize(Nz::Vector2f(static_cast<float>(frame.rect.width), static_cast<float>(frame.rect.height)));
		s->SetOrigin(Nz::Vector3f(static_cast<float>(frame.offset.x), static_cast<float>(frame.offset.y), 0));
	}
}