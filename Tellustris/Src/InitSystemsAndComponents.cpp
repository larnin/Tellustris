
#include "Systems/AnimatorSystem.h"
#include "Systems/TilemapAnimationsSystem.h"
#include "Systems/BehaviourSystem.h"

#include "Components/AnimatorComponent.h"
#include "Components/TilemapComponent.h"
#include "Components/TilemapAnimationComponent.h"
#include "Components/TilemapColliderComponent.h"
#include "Components/BehaviourComponent.h"

void InitializeSystemsAndComponents()
{
	Ndk::InitializeSystem<AnimatorSystem>();
	Ndk::InitializeSystem<TilemapAnimationsSystem>();
	Ndk::InitializeSystem<BehaviourSystem>();

	Ndk::InitializeComponent<AnimatorComponent>("01ATOR");
	Ndk::InitializeComponent<TilemapComponent>("02TMAP");
	Ndk::InitializeComponent<TilemapAnimationComponent>("03TANI");
	Ndk::InitializeComponent<TilemapColliderComponent>("04TCOL");
	Ndk::InitializeComponent<BehaviourComponent>("05BEHA");
}