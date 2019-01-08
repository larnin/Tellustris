#include <NDK/Application.hpp>
#include <Nazara/Renderer.hpp>
#include <iostream>

int main()
{
	Ndk::Application application;

	Nz::RenderWindow& mainWindow = application.AddWindow<Nz::RenderWindow>();
	mainWindow.Create(Nz::VideoMode(800, 600, 32), "Test");

	while (application.Run())
	{
		mainWindow.Display();
	}

	return EXIT_SUCCESS;
}