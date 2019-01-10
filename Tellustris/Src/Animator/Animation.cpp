#include "Animator/Animation.h"

Animation::Animation(bool _loop)
	: loop(_loop)
{

}

float Animation::duration() const
{
	float time = 0;

	for (const auto & f : *this)
		time += f.time;
	return time;
}

Nz::Recti Animation::bounds() const
{
	if (empty())
		return Nz::Recti(0, 0, 0, 0);

	auto rect = front().rect;
	rect.x -= front().offset.x;
	rect.y -= front().offset.y;

	for (unsigned int i = 0; i < size(); i++)
	{
		const auto & f = (*this)[i];

		auto maxX = std::max(rect.x + rect.width, f.rect.x - f.offset.x + f.rect.width);
		auto maxY = std::max(rect.y + rect.height, f.rect.y - f.offset.y + f.rect.height);
		rect.x = std::min(rect.x, f.rect.x - f.offset.x);
		rect.y = std::min(rect.y, f.rect.y - f.offset.y);
		rect.width = maxX - rect.x;
		rect.height = maxY - rect.y;
	}

	return rect;
}
