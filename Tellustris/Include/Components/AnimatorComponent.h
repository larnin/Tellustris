#pragma once

#include <NDK/Component.hpp>

class AnimatorComponent : public Ndk::Component<AnimatorComponent>
{
public:
	AnimatorComponent();

	static Ndk::ComponentIndex componentIndex;
};