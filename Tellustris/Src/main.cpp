
#include "InitSystemsAndComponents.h"
#include "Systems/AnimatorSystem.h"
#include "Systems/TilemapAnimationsSystem.h"
#include "Components/AnimatorComponent.h"
#include "Components/TilemapComponent.h"
#include "Components/TilemapAnimationComponent.h"
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
#include <Nazara/Platform/Keyboard.hpp>

#include <iostream>
#include <random>

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
	world.AddSystem<TilemapAnimationsSystem>();
	world.GetSystem<Ndk::RenderSystem>().SetGlobalUp(Nz::Vector3f::Down());

	Ndk::EntityHandle viewEntity = world.CreateEntity();
	viewEntity->AddComponent<Ndk::NodeComponent>();
	Ndk::CameraComponent& viewer = viewEntity->AddComponent<Ndk::CameraComponent>();
	viewer.SetTarget(&mainWindow);
	viewer.SetProjectionType(Nz::ProjectionType_Orthogonal);

	/*{
		const int frameSize = 40;
		auto anim = Animation::New(true);
		for (unsigned int i = 0; i < 6; i++)
			anim->push_back(Frame{ Nz::Recti(frameSize*i, frameSize, frameSize, frameSize), Nz::Vector2i(frameSize / 2, frameSize / 2),  0.1f });

		auto ator = Animator::New();
		auto rightIndex = ator->addAnimation("Right", anim);
		auto leftIndex = ator->addAnimation("Left", anim, true);
		ator->addTransition(rightIndex, leftIndex, "loops>=2");
		ator->addTransition(leftIndex, rightIndex, "loops>=2");

		auto sprite = Nz::Sprite::New(Nz::Material::New("Translucent3D"));
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
	}

	{
		std::mt19937 rand;
		std::uniform_int_distribution<unsigned> d(0, 100);

		auto tilemap = Tilemap::New(20, 20, 16, 0);
		for(size_t x = 0 ; x < tilemap->width() ; x++)
			for (size_t y = 0; y < tilemap->height(); y++)
			{
				auto v = d(rand);
				if (v < 3)
					tilemap->setTile(x, y, v + 2);
				else tilemap->setTile(x, y, 1);
			}
		auto clifNb = d(rand) / 10;
		for (unsigned int i = 0; i < clifNb; i++)
		{
			auto x = std::uniform_int_distribution<size_t>(0, tilemap->width())(rand); 
			auto y = std::uniform_int_distribution<size_t>(0, tilemap->height())(rand);
			auto w = std::uniform_int_distribution<size_t>(2, 7)(rand);

			for (unsigned int j = 0; j < w; j++)
			{
				if (x + j >= tilemap->width())
					break;
				if (j == 0)
					tilemap->setTile(x + j, y, 5);
				else if (j == w - 1)
					tilemap->setTile(x + j, y, 7);
				else tilemap->setTile(x + j, y, 6);
			}
		}

		auto mat = Nz::Material::New("Translucent3D");
		mat->SetDiffuseMap("Img/tile.png");
		auto tilemapR = Nz::TileMap::New(Nz::Vector2ui(20, 20), Nz::Vector2f(16.f, 16.f));
		tilemapR->SetMaterial(0, mat);

		Ndk::EntityHandle tilemapEntity = world.CreateEntity();
		auto & node = tilemapEntity->AddComponent<Ndk::NodeComponent>();
		node.SetScale(2, 2, 2);
		node.SetPosition(0, 0, -20);
		auto & graph = tilemapEntity->AddComponent<Ndk::GraphicsComponent>();
		auto & map = tilemapEntity->AddComponent<TilemapComponent>();
		graph.Attach(tilemapR);
		map.attachRenderer(tilemapR);
		map.attachTilemap(tilemap);
	}*/

	Ndk::EntityHandle player = world.CreateEntity();
	{
		auto mat = Nz::Material::New("Translucent3D");
		mat->EnableDepthSorting(true);
		//mat->EnableDepthWrite(true);
		auto sprite = Nz::Sprite::New(mat);
		sprite->SetTexture("Img/square.png");
		sprite->SetSize(14, 14);

		auto & node = player->AddComponent<Ndk::NodeComponent>();
		node.SetPosition(mainWindow.GetSize().x / 2.f, mainWindow.GetSize().y / 2.f, -10);
		node.SetScale(2, 2, 2);
		auto & graph = player->AddComponent<Ndk::GraphicsComponent>();
		graph.Attach(sprite);
	}

	Ndk::EntityHandle tilemapEntity = world.CreateEntity();
	{
		auto tilemap = Tilemap::New(20, 20, 16, 0);
		for (size_t x = 0; x < tilemap->width(); x++)
			for (size_t y = 0; y < tilemap->height(); y++)
				tilemap->setTile(x, y, 8);

		auto anims = TilemapAnimations::New(0.1f);
		anims->registerAnimation(8, { 9, 11, 13, 12, 11, 10, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 }, "2*sqrt((x-0.5)*(x-0.5)+(y-0.5)*(y-0.5))");
		anims->registerAnimation(16, { 17, 19, 21, 23, 22, 21, 20, 19, 18, 17, 16 }, "1");

		auto mat = Nz::Material::New("Translucent3D");
		mat->EnableDepthSorting(true);
		//mat->EnableDepthWrite(true);
		mat->SetDiffuseMap("Img/tile2.png");
		auto tilemapR = Nz::TileMap::New(Nz::Vector2ui(20, 20), Nz::Vector2f(16.f, 16.f), 1, Nz::Vector2f(0, 1));
		tilemapR->SetMaterial(0, mat);

		auto & node = tilemapEntity->AddComponent<Ndk::NodeComponent>();
		node.SetScale(2, 2, 2);
		node.SetPosition(0, 0, -20);
		auto & graph = tilemapEntity->AddComponent<Ndk::GraphicsComponent>();
		auto & map = tilemapEntity->AddComponent<TilemapComponent>();
		auto & anim = tilemapEntity->AddComponent<TilemapAnimationComponent>();
		graph.Attach(tilemapR);
		map.attachRenderer(tilemapR);
		map.attachTilemap(tilemap);
		anim.attachRenderer(tilemapR);
		anim.attachTilemap(tilemap);
		anim.attachAnimations(anims);
	}

	auto & playerNode = player->GetComponent<Ndk::NodeComponent>();
	auto & mapAnim = tilemapEntity->GetComponent<TilemapAnimationComponent>();

	std::vector<Nz::Vector2ui> overlapPos;

	while (application.Run())
	{
		const float speed = 300.f;
		Nz::Vector2f dir(Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Right) - Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Left)
			, Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Down) - Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Up));
		if (dir.GetSquaredLength() > 0.1f)
			dir.Normalize();
		dir *= speed * application.GetUpdateTime();

		auto pos = playerNode.GetPosition();
		pos.x += dir.x;
		pos.y += dir.y;
		playerNode.SetPosition(pos);

		std::vector<Nz::Vector2ui> newPos;
		newPos.push_back(Nz::Vector2ui((pos.x ) / 32, (pos.y ) / 32));
		newPos.push_back(Nz::Vector2ui((pos.x + 28) / 32, (pos.y) / 32));
		newPos.push_back(Nz::Vector2ui((pos.x + 28) / 32, (pos.y + 28) / 32));
		newPos.push_back(Nz::Vector2ui((pos.x) / 32, (pos.y + 28) / 32));

		for (const auto & p : newPos)
		{
			if (std::find(overlapPos.begin(), overlapPos.end(), p) == overlapPos.end() && p.x < 20 && p.y < 20)
				mapAnim.playAnimation(p.x, p.y, 16);
		}
		overlapPos = newPos;

		mainWindow.Display();
	}

	return EXIT_SUCCESS;
}