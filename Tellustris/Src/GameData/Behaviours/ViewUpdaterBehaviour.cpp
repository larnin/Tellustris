
#include "GameData/Behaviours/ViewUpdaterBehaviour.h"
#include "Utility/Event/Args.h"
#include "Utility/Event/Event.h"
#include "Utility/Event/Args.h"

#include <cassert>

BehaviourRef ViewUpdaterBehaviour::clone() const
{
	return std::make_unique<ViewUpdaterBehaviour>();
}

void ViewUpdaterBehaviour::onStart()
{
	assert(getEntity().IsValid());

	m_node = &getEntity()->GetComponent<Ndk::NodeComponent>();
}

void ViewUpdaterBehaviour::onUpdate(float deltaTime)
{
	auto pos = m_node->GetPosition();
	int x = static_cast<int>(std::floor(pos.x));
	int y = static_cast<int>(std::floor(pos.y));

	if (x != m_oldX || y != m_oldY)
	{
		m_oldX = x;
		m_oldY = y;

		StaticEvent<CenterViewUpdate>::send({ static_cast<float>(x), static_cast<float>(y) });
	}
}