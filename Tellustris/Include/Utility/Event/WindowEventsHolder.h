#pragma once
#include <Nazara/Platform/EventHandler.hpp>

class WindowEventsHolder
{
public:
	WindowEventsHolder(Nz::EventHandler & e);
	~WindowEventsHolder() = default;

	NazaraSlot(Nz::EventHandler, OnGainedFocus, OnGainedFocus);
	NazaraSlot(Nz::EventHandler, OnLostFocus, OnLostFocus);
	NazaraSlot(Nz::EventHandler, OnKeyPressed, OnKeyPressed);
	NazaraSlot(Nz::EventHandler, OnKeyReleased, OnKeyReleased);
	NazaraSlot(Nz::EventHandler, OnMouseButtonDoubleClicked, OnMouseButtonDoubleClicked);
	NazaraSlot(Nz::EventHandler, OnMouseButtonPressed, OnMouseButtonPressed);
	NazaraSlot(Nz::EventHandler, OnMouseButtonReleased, OnMouseButtonReleased);
	NazaraSlot(Nz::EventHandler, OnMouseEntered, OnMouseEntered);
	NazaraSlot(Nz::EventHandler, OnMouseLeft, OnMouseLeft);
	NazaraSlot(Nz::EventHandler, OnMouseMoved, OnMouseMoved);
	NazaraSlot(Nz::EventHandler, OnMouseWheelMoved, OnMouseWheelMoved);
	NazaraSlot(Nz::EventHandler, OnMoved, OnMoved);
	NazaraSlot(Nz::EventHandler, OnQuit, OnQuit);
	NazaraSlot(Nz::EventHandler, OnResized, OnResized);
	NazaraSlot(Nz::EventHandler, OnTextEntered, OnTextEntered);
};

