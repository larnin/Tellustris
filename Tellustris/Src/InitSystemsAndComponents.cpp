
#include "Systems/AnimatorSystem.h"

#include "Components/AnimatorComponent.h"

void InitializeSystemsAndComponents()
{
	Ndk::InitializeSystem<AnimatorSystem>();

	Ndk::InitializeComponent<AnimatorComponent>("01ATOR");
}