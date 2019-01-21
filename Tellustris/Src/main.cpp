
#include "InitSystemsAndComponents.h"
#include "Systems/AnimatorSystem.h"
#include "Components/AnimatorComponent.h"
#include "Utility/Event/WindowEventsHolder.h"

#include <NDK/Application.hpp>
#include <NDK/World.hpp> 
#include <NDK/Systems/RenderSystem.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/CameraComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <iostream>

#include "Utility/Expression/ExpressionParser.h"


int main()
{
	InitializeSystemsAndComponents();

	Nz::TextureSampler::SetDefaultFilterMode(Nz::SamplerFilter::SamplerFilter_Nearest);

	Ndk::Application application;

	Nz::RenderWindow& mainWindow = application.AddWindow<Nz::RenderWindow>();
	mainWindow.Create(Nz::VideoMode(800, 600, 32), "Test");
	mainWindow.SetFramerateLimit(60);

	WindowEventsHolder windowEventsHolder(mainWindow.GetEventHandler());

	Ndk::World& world = application.AddWorld();
	world.AddSystem<AnimatorSystem>();
	world.GetSystem<Ndk::RenderSystem>().SetGlobalUp(Nz::Vector3f::Down());

	Ndk::EntityHandle viewEntity = world.CreateEntity();
	viewEntity->AddComponent<Ndk::NodeComponent>();
	Ndk::CameraComponent& viewer = viewEntity->AddComponent<Ndk::CameraComponent>();
	viewer.SetTarget(&mainWindow);
	viewer.SetProjectionType(Nz::ProjectionType_Orthogonal);

	const int frameSize = 40;
	auto anim = Animation::New(true);
	for (unsigned int i = 0; i < 6; i++)
		anim->push_back(Frame{ Nz::Recti(frameSize*i, frameSize, frameSize, frameSize), Nz::Vector2i(frameSize / 2, frameSize / 2),  0.1f });

	auto ator = Animator::New();
	auto rightIndex = ator->addAnimation("Right", anim);
	auto leftIndex = ator->addAnimation("Left", anim, true);
	ator->addTransition(rightIndex, leftIndex, "loops>=2");
	ator->addTransition(leftIndex, rightIndex, "loops>=2");

	auto sprite = Nz::Sprite::New(Nz::Material::New("Translucent2D"));
	sprite->SetTexture("Img/moth.png");

	Ndk::EntityHandle mothEntity = world.CreateEntity();
	auto & node = mothEntity->AddComponent<Ndk::NodeComponent>();
	node.SetPosition(mainWindow.GetSize().x / 2.f, mainWindow.GetSize().y / 2.f, -10);
	node.SetScale(2, 2, 2);
	auto & graph = mothEntity->AddComponent<Ndk::GraphicsComponent>();
	graph.Attach(sprite);
	auto & animComp = mothEntity->AddComponent<AnimatorComponent>();
	animComp.attachSprite(sprite);
	animComp.attachAnimator(ator);

	while (application.Run())
	{
		mainWindow.Display();
	}

	return EXIT_SUCCESS;
}