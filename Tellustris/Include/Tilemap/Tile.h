#pragma once

#include <Nazara/Physics2D/Collider2D.hpp>

enum class TileColliderType : unsigned int
{
	Empty,
	Full,
	Triangle,
	Half,
	Quarter,
	CentredHalf,
	Max = CentredHalf
};

enum class TileColliderRotation : unsigned int
{
	R_0,
	R_90,
	R180,
	R270
};

/// unsigned int value
/// [0] xFlipped
/// [1] yFlipped
/// [2-3] rotation
/// [4-15] colliderType
/// [16-32] collisionLayer

struct TileCollider
{
	TileCollider(unsigned int value = 0);
	unsigned int toInt() const;
	void fromInt(unsigned int value);
	bool haveCollision() const;
	bool haveFullCollision() const;

	Nz::Collider2DRef toCollider(const Nz::Vector2f & pos, const Nz::Vector2f & size);

	TileColliderType type;
	TileColliderRotation rotation;
	bool xFlipped;
	bool yFlipped;
	unsigned int collisionLayer;

private:
	void transform(Nz::Vector2f * data, unsigned int size);
	static void moveAndScale(Nz::Vector2f * data, unsigned int size, const Nz::Vector2f & pos, const Nz::Vector2f & scale);
};

struct Tile
{
	unsigned int id = 0;
	TileCollider collider;
};
