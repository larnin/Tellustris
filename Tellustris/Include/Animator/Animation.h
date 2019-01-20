#pragma once

#include "Nazara/Math/Rect.hpp"
#include "Nazara/Math/Vector2.hpp"

#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/ObjectRef.hpp>

#include <vector>

struct Frame
{
	Nz::Recti rect;
	Nz::Vector2i offset;
	float time;
};

class Animation;

using AnimationRef = Nz::ObjectRef<Animation>;
using AnimationConstRef = Nz::ObjectRef<const Animation>;

class Animation : public Nz::RefCounted, private std::vector<Frame>
{
public:
	Animation(bool _loop = false);

	using std::vector<Frame>::push_back;
	using std::vector<Frame>::pop_back;
	using std::vector<Frame>::begin;
	using std::vector<Frame>::end;
	using std::vector<Frame>::front;
	using std::vector<Frame>::back;
	using std::vector<Frame>::insert;
	using std::vector<Frame>::erase;
	using std::vector<Frame>::emplace_back;
	using std::vector<Frame>::clear;
	using std::vector<Frame>::operator [];
	using std::vector<Frame>::size;
	using std::vector<Frame>::empty;

	float duration() const;
	Nz::Recti bounds() const;

	template<typename... Args> static AnimationRef New(Args&&... args)
	{
		auto object = std::make_unique<Animation>(std::forward<Args>(args)...);
		object->SetPersistent(false);

		return object.release();
	}

	Frame current(float time, bool checkLoop = true);

	bool loop = false;
};