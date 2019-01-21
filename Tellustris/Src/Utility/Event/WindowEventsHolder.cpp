#include "Utility/Event/WindowEventsHolder.h"
#include "Utility/Event/WindowEventArgs.h"
#include "Utility/Event/Event.h"

WindowEventsHolder::WindowEventsHolder(Nz::EventHandler & e)
{
	OnGainedFocus.Connect(e.OnGainedFocus,
		[](const Nz::EventHandler*) {StaticEvent<GainedFocusEvent>::send({}); });
	OnLostFocus.Connect(e.OnLostFocus,
		[](const Nz::EventHandler*) {StaticEvent<LostFocusEvent>::send({}); });
	OnKeyPressed.Connect(e.OnKeyPressed,
		[](const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& event) {StaticEvent<KeyPressedEvent>::send({ event }); });
	OnKeyReleased.Connect(e.OnKeyReleased,
		[](const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& event) {StaticEvent<KeyReleasedEvent>::send({ event }); });
	OnMouseButtonDoubleClicked.Connect(e.OnMouseButtonDoubleClicked,
		[](const Nz::EventHandler*, const Nz::WindowEvent::MouseButtonEvent& event) {StaticEvent<MouseButtonDoubleClickedEvent>::send({ event }); });
	OnMouseButtonPressed.Connect(e.OnMouseButtonPressed,
		[](const Nz::EventHandler*, const Nz::WindowEvent::MouseButtonEvent& event) {StaticEvent<MouseButtonPressedEvent>::send({ event }); });
	OnMouseButtonReleased.Connect(e.OnMouseButtonReleased,
		[](const Nz::EventHandler*, const Nz::WindowEvent::MouseButtonEvent& event) {StaticEvent<MouseButtonReleasedEvent>::send({ event }); });
	OnMouseEntered.Connect(e.OnMouseEntered,
		[](const Nz::EventHandler*) {StaticEvent<MouseEnteredEvent>::send({}); });
	OnMouseLeft.Connect(e.OnMouseLeft,
		[](const Nz::EventHandler*) {StaticEvent<MouseLeftEvent>::send({}); });
	OnMouseMoved.Connect(e.OnMouseMoved,
		[](const Nz::EventHandler*, const Nz::WindowEvent::MouseMoveEvent & event) {StaticEvent<MouseMovedEvent>::send({ event }); });
	OnMouseWheelMoved.Connect(e.OnMouseWheelMoved,
		[](const Nz::EventHandler*, const Nz::WindowEvent::MouseWheelEvent& event) {StaticEvent<MouseWheelMovedEvent>::send({event}); });
	OnMoved.Connect(e.OnMoved,
		[](const Nz::EventHandler*, const Nz::WindowEvent::PositionEvent & event) {StaticEvent<MovedEvent>::send({ event }); });
	OnQuit.Connect(e.OnQuit,
		[](const Nz::EventHandler*) {StaticEvent<QuitEvent>::send({}); });
	OnResized.Connect(e.OnResized,
		[](const Nz::EventHandler*, const Nz::WindowEvent::SizeEvent & event) {StaticEvent<ResizedEvent>::send({ event }); });
	OnTextEntered.Connect(e.OnTextEntered,
		[](const Nz::EventHandler*, const Nz::WindowEvent::TextEvent & event) {StaticEvent<TextEnteredEvent>::send({ event }); });
}
