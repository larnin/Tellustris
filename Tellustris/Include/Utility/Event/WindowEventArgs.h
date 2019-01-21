#pragma once

#include <Nazara/Platform/Event.hpp>
#include <Nazara/Platform/Enums.hpp>

template <typename T, Nz::WindowEventType type>
struct WindowEvent
{
	T value;
};

template <Nz::WindowEventType type>
struct VoidWindowEvent {};

using GainedFocusEvent = VoidWindowEvent<Nz::WindowEventType_GainedFocus>;
using LostFocusEvent = VoidWindowEvent<Nz::WindowEventType_LostFocus>;
using KeyPressedEvent = WindowEvent<Nz::WindowEvent::KeyEvent, Nz::WindowEventType_KeyPressed>;
using KeyReleasedEvent = WindowEvent<Nz::WindowEvent::KeyEvent, Nz::WindowEventType_KeyReleased>;
using MouseButtonDoubleClickedEvent = WindowEvent<Nz::WindowEvent::MouseButtonEvent, Nz::WindowEventType_MouseButtonDoubleClicked>;
using MouseButtonPressedEvent = WindowEvent<Nz::WindowEvent::MouseButtonEvent, Nz::WindowEventType_MouseButtonPressed>;
using MouseButtonReleasedEvent = WindowEvent<Nz::WindowEvent::MouseButtonEvent, Nz::WindowEventType_MouseButtonReleased>;
using MouseEnteredEvent = VoidWindowEvent<Nz::WindowEventType_MouseEntered>;
using MouseLeftEvent = VoidWindowEvent<Nz::WindowEventType_MouseLeft>;
using MouseMovedEvent = WindowEvent<Nz::WindowEvent::MouseMoveEvent, Nz::WindowEventType_MouseMoved>;
using MouseWheelMovedEvent = WindowEvent<Nz::WindowEvent::MouseWheelEvent, Nz::WindowEventType_MouseWheelMoved>;
using MovedEvent = WindowEvent<Nz::WindowEvent::PositionEvent, Nz::WindowEventType_Moved>;
using QuitEvent = VoidWindowEvent<Nz::WindowEventType_Quit>;
using ResizedEvent = WindowEvent<Nz::WindowEvent::SizeEvent, Nz::WindowEventType_Resized>;
using TextEnteredEvent = WindowEvent<Nz::WindowEvent::TextEvent, Nz::WindowEventType_TextEntered>;