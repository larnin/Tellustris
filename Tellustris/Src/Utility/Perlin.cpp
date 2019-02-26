
#include "Utility/Perlin.h"

#include <cassert>

Perlin::Perlin(size_t size, float amplitude, size_t frequence, size_t seed)
	: m_size(size), m_amplitude(amplitude), m_frequence(frequence), m_gen(seed), m_distrib(-amplitude, amplitude)
{

}

float Perlin::operator()(float x) const
{
	float dec;
	size_t x1, x2;
	splitValue(x, x1, x2, dec);

	float v1 = m_distrib(m_gen(x1));
	float v2 = m_distrib(m_gen(x2));

	return cosLerp(v1, v2, dec);
}

float Perlin::operator()(float x, float y) const
{
	float decX, decY;
	size_t x1, x2, y1, y2;
	splitValue(x, x1, x2, decX);
	splitValue(y, y1, y2, decY);

	float v1 = m_distrib(m_gen(x1, y1));
	float v2 = m_distrib(m_gen(x2, y1));
	float v3 = m_distrib(m_gen(x1, y2));
	float v4 = m_distrib(m_gen(x2, y2));

	return cos2DLerp(v1, v2, v3, v4, decX, decY);
}

float Perlin::operator()(float x, float y, float z) const
{
	float decX, decY, decZ;
	size_t x1, x2, y1, y2, z1, z2;
	splitValue(x, x1, x2, decX);
	splitValue(y, y1, y2, decY);
	splitValue(z, z1, z2, decZ);

	float v1 = m_distrib(m_gen(x1, y1, z1));
	float v3 = m_distrib(m_gen(x1, y1, z2));
	float v2 = m_distrib(m_gen(x1, y2, z1));
	float v4 = m_distrib(m_gen(x1, y2, z2));
	float v5 = m_distrib(m_gen(x2, y1, z1));
	float v6 = m_distrib(m_gen(x2, y2, z1));
	float v7 = m_distrib(m_gen(x2, y1, z2));
	float v8 = m_distrib(m_gen(x2, y2, z2));

	return cos3DLerp(v1, v2, v3, v4, v5, v6, v7, v8, decX, decY, decZ);
}

void Perlin::splitValue(float value, size_t & outX1, size_t & outX2, float & outDec) const
{
	float x = value / m_size * m_frequence;

	outDec = x - std::floor(x);

	int frequence = static_cast<int>(m_frequence);

	int intValue = static_cast<int>(std::floor(x));
	int chunk = ((intValue < 0 ? -frequence + 1 : 0) + intValue) / frequence;
	outX1 = intValue - chunk * frequence;
	outX2 = outX1 < m_frequence - 1 ? outX1 + 1 : 0;
}

float linearLerp(float a, float b, float x)
{
	return a * (1 - x) + b * x;
}

float cosLerp(float a, float b, float x)
{
	const float PI(3.14159f);
	float k = (1 - cos(x * PI)) / 2;
	return linearLerp(a, b, k);
}

float cos2DLerp(float a, float b, float c, float d, float x, float y)
{
	float x1 = cosLerp(a, b, x);
	float x2 = cosLerp(c, d, x);
	return cosLerp(x1, x2, y);
}

float cos3DLerp(float a, float b, float c, float d, float e, float f, float g, float h, float x, float y, float z)
{
	float x1 = cosLerp(a, e, x);
	float x2 = cosLerp(b, f, x);
	float x3 = cosLerp(c, g, x);
	float x4 = cosLerp(d, h, x);
	return cos2DLerp(x1, x2, x3, x4, y, z);
}