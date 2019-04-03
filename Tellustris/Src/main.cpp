
#include "InitSystemsAndComponents.h"
#include "Systems/AnimatorSystem.h"
#include "Systems/TilemapAnimationsSystem.h"
#include "Systems/BehaviourSystem.h"
#include "Components/AnimatorComponent.h"
#include "Components/TilemapComponent.h"
#include "Components/TilemapAnimationComponent.h"
#include "Components/TilemapColliderComponent.h"
#include "Components/BehaviourComponent.h"
#include "Utility/Event/WindowEventsHolder.h"
#include "GameData/Behaviours/ViewUpdaterBehaviour.h"
#include "GameData/WorldMap.h"
#include "GameData/Behaviours/WorldRenderBehaviour.h"
#include "GameData/LoadRessources.h"
#include "GameData/LoadSettings.h"
#include "Utility/Perlin.h"
#include "Utility/enumiterators.h"

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
#include <NDK/Systems/PhysicsSystem2D.hpp>
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <NDK/Components/DebugComponent.hpp>
#include <Nazara/Graphics/TileMap.hpp>

#include <iostream>
#include <random>
#include <chrono>

#include "Utility/Ressource.h"

//int main()
//{
//	Ndk::Application application;
//
//	Nz::RenderWindow& mainWindow = application.AddWindow<Nz::RenderWindow>();
//	mainWindow.Create(Nz::VideoMode(800, 600, 32), "Test");
//	mainWindow.SetFramerateLimit(60);
//
//	Nz::DebugDrawer::SetPrimaryColor(Nz::Color::Red);
//	Nz::DebugDrawer::SetSecondaryColor(Nz::Color::Red);
//
//	Nz::Vector3f center{ 400, 300, -20 };
//	const float size = 30.f;
//
//	float a = 0;
//
//	while (application.Run())
//	{
//		a += 0.01f;
//
//		Nz::Vector3f pos{ size * cos(a), size * sin(a), 0 };
//		Nz::DebugDrawer::DrawLine(center + pos, center - pos);
//
//		mainWindow.Display();
//	}
//		
//	return EXIT_SUCCESS;
//}
//
//int main()
//{
//	Ndk::Application application;
//
//	InitializeSystemsAndComponents();
//
//	Nz::TextureSampler::SetDefaultFilterMode(Nz::SamplerFilter::SamplerFilter_Nearest);
//
//	Nz::RenderWindow& mainWindow = application.AddWindow<Nz::RenderWindow>();
//	mainWindow.Create(Nz::VideoMode(800, 600, 32), "Test");
//	mainWindow.SetFramerateLimit(60);
//
//	WindowEventsHolder windowEventsHolder(mainWindow.GetEventHandler());
//
//	Ndk::World& world = application.AddWorld();
//	world.AddSystem<AnimatorSystem>();
//	world.AddSystem<TilemapAnimationsSystem>();
//	world.AddSystem<BehaviourSystem>();
//	world.GetSystem<Ndk::RenderSystem>().SetGlobalUp(Nz::Vector3f::Down());
//
//	Ndk::PhysicsSystem2D::DebugDrawOptions options;
//	options.polygonCallback = [](const Nz::Vector2f* vertices, std::size_t vertexCount, float radius, Nz::Color outline, Nz::Color fillColor, void* userData)
//	{
//		for (std::size_t i = 0; i < vertexCount - 1; ++i)
//			Nz::DebugDrawer::DrawLine(vertices[i], vertices[i + 1]);
//
//		Nz::DebugDrawer::DrawLine(vertices[vertexCount - 1], vertices[0]);
//	};
//
//	world.GetSystem<Ndk::PhysicsSystem2D>().DebugDraw(options);
//
//
//	Ndk::EntityHandle viewEntity = world.CreateEntity();
//	viewEntity->AddComponent<Ndk::NodeComponent>();
//	Ndk::CameraComponent& viewer = viewEntity->AddComponent<Ndk::CameraComponent>();
//	viewer.SetTarget(&mainWindow);
//	viewer.SetProjectionType(Nz::ProjectionType_Orthogonal);
//
//	/*{
//		const int frameSize = 40;
//		auto anim = Animation::New(true);
//		for (unsigned int i = 0; i < 6; i++)
//			anim->push_back(Frame{ Nz::Recti(frameSize*i, frameSize, frameSize, frameSize), Nz::Vector2i(frameSize / 2, frameSize / 2),  0.1f });
//
//		auto ator = Animator::New();
//		auto rightIndex = ator->addAnimation("Right", anim);
//		auto leftIndex = ator->addAnimation("Left", anim, true);
//		ator->addTransition(rightIndex, leftIndex, "loops>=2");
//		ator->addTransition(leftIndex, rightIndex, "loops>=2");
//
//		auto sprite = Nz::Sprite::New(Nz::Material::New("Translucent3D"));
//		sprite->SetTexture("Img/moth.png");
//
//		Ndk::EntityHandle mothEntity = world.CreateEntity();
//		auto & node = mothEntity->AddComponent<Ndk::NodeComponent>();
//		node.SetPosition(mainWindow.GetSize().x / 2.f, mainWindow.GetSize().y / 2.f, -10);
//		node.SetScale(2, 2, 2);
//		auto & graph = mothEntity->AddComponent<Ndk::GraphicsComponent>();
//		graph.Attach(sprite);
//		auto & animComp = mothEntity->AddComponent<AnimatorComponent>();
//		animComp.attachSprite(sprite);
//		animComp.attachAnimator(ator);
//	}
//
//	{
//		std::mt19937 rand;
//		std::uniform_int_distribution<unsigned> d(0, 100);
//
//		auto tilemap = Tilemap::New(20, 20, 16, 0);
//		for(size_t x = 0 ; x < tilemap->width() ; x++)
//			for (size_t y = 0; y < tilemap->height(); y++)
//			{
//				auto v = d(rand);
//				if (v < 3)
//					tilemap->setTile(x, y, v + 2);
//				else tilemap->setTile(x, y, 1);
//			}
//		auto clifNb = d(rand) / 10;
//		for (unsigned int i = 0; i < clifNb; i++)
//		{
//			auto x = std::uniform_int_distribution<size_t>(0, tilemap->width())(rand); 
//			auto y = std::uniform_int_distribution<size_t>(0, tilemap->height())(rand);
//			auto w = std::uniform_int_distribution<size_t>(2, 7)(rand);
//
//			for (unsigned int j = 0; j < w; j++)
//			{
//				if (x + j >= tilemap->width())
//					break;
//				if (j == 0)
//					tilemap->setTile(x + j, y, 5);
//				else if (j == w - 1)
//					tilemap->setTile(x + j, y, 7);
//				else tilemap->setTile(x + j, y, 6);
//			}
//		}
//
//		auto mat = Nz::Material::New("Translucent3D");
//		mat->SetDiffuseMap("Img/tile.png");
//		auto tilemapR = Nz::TileMap::New(Nz::Vector2ui(20, 20), Nz::Vector2f(16.f, 16.f));
//		tilemapR->SetMaterial(0, mat);
//
//		Ndk::EntityHandle tilemapEntity = world.CreateEntity();
//		auto & node = tilemapEntity->AddComponent<Ndk::NodeComponent>();
//		node.SetScale(2, 2, 2);
//		node.SetPosition(0, 0, -20);
//		auto & graph = tilemapEntity->AddComponent<Ndk::GraphicsComponent>();
//		auto & map = tilemapEntity->AddComponent<TilemapComponent>();
//		graph.Attach(tilemapR);
//		map.attachRenderer(tilemapR);
//		map.attachTilemap(tilemap);
//	}*/
//
//	Ndk::EntityHandle player = world.CreateEntity();
//	{
//		auto mat = Nz::Material::New("Translucent3D");
//		mat->EnableDepthSorting(true);
//		//mat->EnableDepthWrite(true);
//		auto sprite = Nz::Sprite::New(mat);
//		sprite->SetTexture("Img/square.png");
//		sprite->SetSize(14, 14);
//
//		auto & node = player->AddComponent<Ndk::NodeComponent>();
//		node.SetPosition(mainWindow.GetSize().x / 2.f, mainWindow.GetSize().y / 2.f, -10);
//		node.SetScale(2, 2, 2);
//		auto & graph = player->AddComponent<Ndk::GraphicsComponent>();
//		graph.Attach(sprite);
//		auto & behaviour = player->AddComponent<BehaviourComponent>();
//		behaviour.attach(std::make_unique<ViewUpdaterBehaviour>());
//	}
//
//	Ndk::EntityHandle tilemapEntity = world.CreateEntity();
//	{
//		std::mt19937 rand;
//		std::bernoulli_distribution d(0.5);
//
//		TileCollider col;
//		col.type = TileColliderType::Full;
//
//		auto tilemap = Tilemap::New(20, 20, 16, 0);
//		for (size_t x = 0; x < tilemap->width(); x++)
//			for (size_t y = 0; y < tilemap->height(); y++)
//				if (d(rand))
//					tilemap->setTile(x, y, { 8, col });
//
//		auto anims = TilemapAnimations::New(0.1f);
//		anims->registerAnimation(8, { 9, 11, 13, 12, 11, 10, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 }, "2*sqrt((x-0.5)*(x-0.5)+(y-0.5)*(y-0.5))");
//		anims->registerAnimation(16, { 17, 19, 21, 23, 22, 21, 20, 19, 18, 17, 16 }, "1");
//
//		auto mat = Nz::Material::New("Translucent3D");
//		mat->EnableDepthSorting(true);
//		//mat->EnableDepthWrite(true);
//		mat->SetDiffuseMap("Img/tile2.png");
//		auto tilemapR = Nz::TileMap::New(Nz::Vector2ui(20, 20), Nz::Vector2f(16.f, 16.f), 1, Nz::Vector2f(0, 1));
//		tilemapR->SetMaterial(0, mat);
//
//		auto & node = tilemapEntity->AddComponent<Ndk::NodeComponent>();
//		node.SetScale(2, 2, 2);
//		node.SetPosition(0, 0, -20);
//		auto & graph = tilemapEntity->AddComponent<Ndk::GraphicsComponent>();
//		auto & map = tilemapEntity->AddComponent<TilemapComponent>();
//		auto & anim = tilemapEntity->AddComponent<TilemapAnimationComponent>();
//		auto & collider = tilemapEntity->AddComponent<TilemapColliderComponent>();
//		graph.Attach(tilemapR);
//		map.attachRenderer(tilemapR);
//		map.attachTilemap(tilemap);
//		anim.attachRenderer(tilemapR);
//		anim.attachTilemap(tilemap);
//		anim.attachAnimations(anims);
//		collider.attachTilemap(tilemap);
//	}
//
//	auto & playerNode = player->GetComponent<Ndk::NodeComponent>();
//	auto & mapAnim = tilemapEntity->GetComponent<TilemapAnimationComponent>();
//
//	auto & collider = tilemapEntity->GetComponent<TilemapColliderComponent>();
//
//	std::vector<Nz::Vector2ui> overlapPos;
//
//	while (application.Run())
//	{
//		const float speed = 300.f;
//		Nz::Vector2f dir(Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Right) - Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Left)
//			, Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Down) - Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Up));
//		if (dir.GetSquaredLength() > 0.1f)
//			dir.Normalize();
//		dir *= speed * application.GetUpdateTime();
//
//		auto pos = playerNode.GetPosition();
//		pos.x += dir.x;
//		pos.y += dir.y;
//		playerNode.SetPosition(pos);
//
//		std::vector<Nz::Vector2ui> newPos;
//		newPos.push_back(Nz::Vector2ui((pos.x ) / 32, (pos.y ) / 32));
//		newPos.push_back(Nz::Vector2ui((pos.x + 28) / 32, (pos.y) / 32));
//		newPos.push_back(Nz::Vector2ui((pos.x + 28) / 32, (pos.y + 28) / 32));
//		newPos.push_back(Nz::Vector2ui((pos.x) / 32, (pos.y + 28) / 32));
//
//		for (const auto & p : newPos)
//		{
//			if (std::find(overlapPos.begin(), overlapPos.end(), p) == overlapPos.end() && p.x < 20 && p.y < 20)
//				mapAnim.playAnimation(p.x, p.y, 16);
//		}
//		overlapPos = newPos;
//
//		mainWindow.Display();
//	}
//
//	return EXIT_SUCCESS;
//}

//int main()
//{
//	Ndk::Application application;
//		
//	InitializeSystemsAndComponents();
//
//	const size_t size = 2000;
//
//	auto img = Nz::Image::New(Nz::ImageType::ImageType_2D, Nz::PixelFormatType::PixelFormatType_RGBA8, size, size);
//
//	Perlin2D perlin1(size / 4, 1.f / 2, 5, 5);
//	Perlin2D perlin2(size / 4, 1.f / 4, 10, 6);
//	Perlin2D perlin3(size / 4, 1.f / 8, 20, 7);
//	Perlin2D perlin4(size / 4, 1.f / 16, 40, 8);
//	Perlin2D perlin5(size / 4, 1.f / 32, 80, 9);
//	Perlin2D perlin6(size / 4, 1.f / 64, 160, 10);
//
//	std::chrono::time_point<std::chrono::system_clock> start, end;
//	start = std::chrono::system_clock::now();
//
//	for(int i = 0 ; i < size; i++)
//		for (int j = 0; j < size; j++)
//		{
//			float v = 0.5f + (perlin1(i, j) +perlin2(i, j) + perlin3(i, j) + perlin4(i, j) + perlin5(i, j) + perlin6(i, j)) / 1.2f;
//			v = v < 0 ? 0 : v;
//			v = v > 1 ? 1 : v;
//
//			img->SetPixelColor(Nz::Color(v * 255, v * 255, v * 255), i, j);
//		}
//
//	end = std::chrono::system_clock::now();
//
//	int elapsedms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
//
//	std::cout << "elapsed time: " << elapsedms << "ms\n";
//
//	img->SaveToFile("img.png");
//
//	std::getchar();
//
//}

int main()
{
	Ndk::Application application;

	InitializeSystemsAndComponents();
	RessourceLoader::loadAll("./Data/");
	SettingsLoader::loadAll("./Data/");

	Nz::TextureSampler::SetDefaultFilterMode(Nz::SamplerFilter::SamplerFilter_Nearest);

	Nz::RenderWindow& mainWindow = application.AddWindow<Nz::RenderWindow>();
	mainWindow.Create(Nz::VideoMode(900, 900, 32), "Test");
	mainWindow.SetFramerateLimit(60);

	WindowEventsHolder windowEventsHolder(mainWindow.GetEventHandler());

	Ndk::World& world = application.AddWorld();
	world.AddSystem<AnimatorSystem>();
	world.AddSystem<TilemapAnimationsSystem>();
	world.AddSystem<BehaviourSystem>();
	world.GetSystem<Ndk::RenderSystem>().SetGlobalUp(Nz::Vector3f::Down());

	Ndk::EntityHandle player = world.CreateEntity();
	{
		auto mat = Nz::Material::New(*Ressource<Nz::Material>::get("default"));
		auto sprite = Nz::Sprite::New(mat);
		sprite->SetTexture(Ressource<Nz::Texture>::get("Img/square.png"));
		sprite->SetSize(1, 1);
		auto & node = player->AddComponent<Ndk::NodeComponent>();
		node.SetPosition(0, 0, 1);
		auto & graph = player->AddComponent<Ndk::GraphicsComponent>();
		graph.Attach(sprite);
		auto & behaviour = player->AddComponent<BehaviourComponent>();
		behaviour.attach(std::make_unique<ViewUpdaterBehaviour>());
	}

	Ndk::EntityHandle camera = world.CreateEntity();
	{
		auto & node = camera->AddComponent<Ndk::NodeComponent>();
		node.SetParent(player->GetComponent<Ndk::NodeComponent>());
		node.SetPosition(-(mainWindow.GetSize().x / 32.f), -(mainWindow.GetSize().y / 32.f), 20);
		Ndk::CameraComponent& viewer = camera->AddComponent<Ndk::CameraComponent>();
		viewer.SetTarget(&mainWindow);
		viewer.SetProjectionType(Nz::ProjectionType_Orthogonal);
		viewer.SetSize(mainWindow.GetSize().x / 16.f, mainWindow.GetSize().y / 16.f);
	}

	size_t chunkNb = 2;
	WorldMap map(chunkNb, chunkNb);

	Ndk::EntityHandle mapEntity = world.CreateEntity();
	{
		auto def = TileDefinition::New();
		def->addTexture(Ressource<Nz::Texture>::get("Img/tile4.png"));
		for (auto c : TileConnexionType::Max)
			def->addTile(1, c, 2, 0, 1);
		for (size_t i = 2; i <= 5; i++)
		{
			size_t matID = 0;
			if (i == 2)
				matID = 4;
			else if (i == 3)
				matID = 5;
			else if (i == 4)
				matID = 2;
			else if (i == 5)
				matID = 3;
			for (auto c : TileConnexionType::Max)
				def->addTile(i, c, (matID - 1) * (static_cast<size_t>(TileConnexionType::Max) + 1) + static_cast<size_t>(c) + 1, 0, 1);
		}
			
		for (size_t i = 1; i <= 5; i++)
			def->addAllowedLayers(i, 0, 1);

		size_t size = chunkNb * Chunk::chunkSize;
		Perlin2D perlinGround1(size, 1.f / 2, 5, 5);
		Perlin2D perlinGround2(size, 1.f / 4, 10, 6);
		Perlin2D perlinSand(size, 1.f, 5, 8);

		for(size_t x = 0 ; x < size ; x++)
			for (size_t y = 0; y < size; y++)
			{
				unsigned int id = 0;
				auto height = perlinGround1(x, y) + perlinGround2(x, y);
				auto isSand = perlinSand(x, y) > 0 && std::abs(height) < 0.1f;

				if (height < 0)
				{
					if (!isSand)
						id = 1;
					else id = 2;
				}
				else
				{
					if (!isSand)
						id = 5;
					else id = 4;
				}
				map.setTile(x, y, Tile{ id, 0 }, 0);
			}

		mapEntity->AddComponent<Ndk::NodeComponent>();
		auto & behaviour = mapEntity->AddComponent<BehaviourComponent>();
		behaviour.attach(std::make_unique<WorldRenderBehaviour>(map, def, 25.0f));
	}


	auto & playerNode = player->GetComponent<Ndk::NodeComponent>();
	auto & camNode = camera->GetComponent<Ndk::NodeComponent>();
	float time = 0;

	while (application.Run())
	{
		const float speed = 30.f;
		Nz::Vector2f dir(Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Right) - Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Left)
			, Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Down) - Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Up));
		if (dir.GetSquaredLength() > 0.1f)
			dir.Normalize();
		dir *= speed * application.GetUpdateTime();
			
		auto pos = playerNode.GetPosition();
		pos.x += dir.x;
		pos.y += dir.y;
		pos.z = 1;
		playerNode.SetPosition(pos);

		time += application.GetUpdateTime();

		//camNode.SetRotation(Nz::Quaternionf(Nz::EulerAnglesf(40 * sin(time / 10), 20 * cos(time / 8), 0)));
			
		mainWindow.Display();
	}

	SettingsLoader::unloadAll();
	RessourceLoader::unloadAll();
}
