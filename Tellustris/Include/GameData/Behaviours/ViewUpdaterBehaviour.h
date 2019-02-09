#pragma once

#include "Behaviour.h"

#include <NDK/Components/NodeComponent.hpp>

class ViewUpdaterBehaviour : public Behaviour
{
public:
	BehaviourRef clone() const override;

protected:
	void onStart() override;
	void onUpdate(float deltaTime) override;

private:
	Ndk::NodeComponent * m_node = nullptr;
	int m_oldX = std::numeric_limits<int>::lowest();
	int m_oldY = std::numeric_limits<int>::lowest();
};