
#include "InitSystemsAndComponents.h"

#include <NDK/Application.hpp>
#include <Nazara/Renderer.hpp>
#include <iostream>

#include "Utility/Expression/ExpressionParser.h"


int main()
{
	InitializeSystemsAndComponents();

	Ndk::Application application;

	Nz::RenderWindow& mainWindow = application.AddWindow<Nz::RenderWindow>();
	mainWindow.Create(Nz::VideoMode(800, 600, 32), "Test");

	NExpression::ExpressionParser<float> parser;
	auto e = parser.evaluate("atan2(x, y)");
	std::cout << e.toString() << std::endl;

	e.setParameter("x", 4);
	e.setParameter("y", -1);
	std::cout << e.compute() << std::endl;

	e.resetParameters();
	e.setParameter("x", 1);
	e.setParameter("y", 8);
	std::cout << e.compute() << std::endl;



	while (application.Run())
	{
		mainWindow.Display();
	}

	return EXIT_SUCCESS;
}