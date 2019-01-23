
#include "Systems/AnimatorSystem.h"

#include "Components/AnimatorComponent.h"
#include "Components/TilemapComponent.h"

void InitializeSystemsAndComponents()
{
	Ndk::InitializeSystem<AnimatorSystem>();

	Ndk::InitializeComponent<AnimatorComponent>("01ATOR");
	Ndk::InitializeComponent<TilemapComponent>("02TMAP");
}