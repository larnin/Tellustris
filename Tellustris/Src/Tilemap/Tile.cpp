#include "Tilemap/Tile.h"

TileCollider::TileCollider(unsigned int value)
{
	fromInt(value);
}

unsigned int TileCollider::toInt() const
{
	int value(0);
	if (xFlipped)
		value += 1 << 0;
	if (yFlipped)
		value += 1 << 1;
	value += static_cast<unsigned int>(rotation) << 2;
	value += static_cast<unsigned int>(type) << 4;
	value += collisionLayer << 16;

	return value;
}

void TileCollider::fromInt(unsigned int value)
{
	xFlipped = (value & 0b1) > 0;
	yFlipped = (value & 0b10) > 0;
	rotation = static_cast<TileColliderRotation>((value & 0b1100) >> 2);
	type = static_cast<TileColliderType>((value & 0xFFF0) >> 4);
	collisionLayer = (value & 0xFFFF0000) >> 16;
}

bool TileCollider::haveCollision() const
{
	return type != TileColliderType::Empty;
}

bool TileCollider::haveFullCollision() const
{
	return type == TileColliderType::Full;
}

Nz::Collider2DRef TileCollider::toCollider(const Nz::Vector2f & pos, const Nz::Vector2f & size)
{
	if (type == TileColliderType::Empty)
		return {};
	if (type == TileColliderType::Full)
		return Nz::BoxCollider2D::New(Nz::Rectf(pos.x, pos.y, size.x, size.y));
	if (type == TileColliderType::Triangle)
	{
		Nz::Vector2f data[3];
		data[0] = { -0.5f, -0.5f };
		data[1] = { 0.5f, -0.5f };
		data[2] = { 0.5f, 0.5f };

		transform(data, 3);
		moveAndScale(data, 3, pos, size);
		return Nz::ConvexCollider2D::New(Nz::SparsePtr<Nz::Vector2f>(data), 3);
	}
	if (type == TileColliderType::Half)
	{
		Nz::Vector2f data[4];
		data[0] = { -0.5f, -0.5f };
		data[1] = { 0.f, -0.5f };
		data[2] = { 0.f, 0.5f };
		data[3] = { -0.5f, 0.5f };

		transform(data, 4);
		moveAndScale(data, 4, pos, size);
		return Nz::ConvexCollider2D::New(Nz::SparsePtr<Nz::Vector2f>(data), 4);
	}
	if (type == TileColliderType::Quarter)
	{
		Nz::Vector2f data[4];
		data[0] = { -0.5f, -0.5f };
		data[1] = { 0.f, -0.5f };
		data[2] = { 0.f, 0.f };
		data[3] = { -0.5f, 0.f };

		transform(data, 4);
		moveAndScale(data, 4, pos, size);
		return Nz::ConvexCollider2D::New(Nz::SparsePtr<Nz::Vector2f>(data), 4);
	}
	if (type == TileColliderType::CentredHalf)
	{
		Nz::Vector2f data[4];
		data[0] = { -0.25f, -0.5f };
		data[1] = { 0.25f, -0.5f };
		data[2] = { 0.25f, 0.5f };
		data[3] = { -0.25f, 0.5f };

		transform(data, 4);
		moveAndScale(data, 4, pos, size);
		return Nz::ConvexCollider2D::New(Nz::SparsePtr<Nz::Vector2f>(data), 4);
	}
	assert(false);
	return {};
}

void TileCollider::transform(Nz::Vector2f * data, unsigned int size)
{
	for (unsigned int i = 0; i < size; i++)
	{
		auto & pos = data[i];
		if (xFlipped)
			pos.x *= -1;
		if (yFlipped)
			pos.y *= -1;

		for (unsigned int j = 0; j < static_cast<unsigned int>(rotation); j++)
		{
			float temp = pos.x;
			pos.x = -pos.y;
			pos.y = temp;
		}
	}
}


void TileCollider::moveAndScale(Nz::Vector2f * data, unsigned int size, const Nz::Vector2f & pos, const Nz::Vector2f & scale)
{
	for (unsigned int i = 0; i < size; i++)
	{
		auto & p = data[i];
		//from [-0.5;0.5] to [0;1]
		p.x += 0.5f;
		p.y += 0.5f;
		
		p *= scale;
		p += pos;
	}
}