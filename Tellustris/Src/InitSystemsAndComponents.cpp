
#include "Systems/AnimatorSystem.h"
#include "Systems/TilemapAnimationsSystem.h"

#include "Components/AnimatorComponent.h"
#include "Components/TilemapComponent.h"
#include "Components/TilemapAnimationComponent.h"
#include "Components/TilemapColliderComponent.h"

void InitializeSystemsAndComponents()
{
	Ndk::InitializeSystem<AnimatorSystem>();
	Ndk::InitializeSystem<TilemapAnimationsSystem>();

	Ndk::InitializeComponent<AnimatorComponent>("01ATOR");
	Ndk::InitializeComponent<TilemapComponent>("02TMAP");
	Ndk::InitializeComponent<TilemapAnimationComponent>("03TANI");
	Ndk::InitializeComponent<TilemapColliderComponent>("04TCOL");
}