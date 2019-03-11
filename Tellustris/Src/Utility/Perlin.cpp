
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
	splitValue(x, m_size, m_frequence, x1, x2, dec);

	float v1 = m_distrib(m_gen(x1));
	float v2 = m_distrib(m_gen(x2));

	return cosLerp(v1, v2, dec);
}

float Perlin::operator()(float x, float y) const
{
	float decX, decY;
	size_t x1, x2, y1, y2;
	splitValue(x, m_size, m_frequence, x1, x2, decX);
	splitValue(y, m_size, m_frequence, y1, y2, decY);

	float v1 = m_distrib(m_gen(x1, y1));
	float v2 = m_distrib(m_gen(x2, y1));
	float v3 = m_distrib(m_gen(x1, y2));
	float v4 = m_distrib(m_gen(x2, y2));

	return square2DLerp(v1, v2, v3, v4, decX, decY);
}

float Perlin::operator()(float x, float y, float z) const
{
	float decX, decY, decZ;
	size_t x1, x2, y1, y2, z1, z2;
	splitValue(x, m_size, m_frequence, x1, x2, decX);
	splitValue(y, m_size, m_frequence, y1, y2, decY);
	splitValue(z, m_size, m_frequence, z1, z2, decZ);

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

Perlin::SplitOut Perlin::splitValue2(float value, size_t size, size_t frequence)
{
	SplitOut out;

	int f = static_cast<int>(frequence);
	float x = value / size * f;

	out.dec = x - std::floor(x);

	int intValue = static_cast<int>(std::floor(x));
	int chunk = ((intValue < 0 ? -f + 1 : 0) + intValue) / f;
	out.x1 = intValue - chunk * f;
	out.x2 = out.x1 < f - 1 ? out.x1 + 1 : 0;

	return out;
}

void Perlin::splitValue(float value, size_t size, size_t frequence, size_t & outX1, size_t & outX2, float & outDec)
{
	int f = static_cast<int>(frequence);
	float x = value / size * f;

	outDec = x - std::floor(x);

	int intValue = static_cast<int>(std::floor(x));
	int chunk = ((intValue < 0 ? -f + 1 : 0) + intValue) / f;
	outX1 = intValue - chunk * f;
	outX2 = outX1 < f - 1 ? outX1 + 1 : 0;
}

Perlin2D::Perlin2D(size_t size, float amplitude, size_t frequence, size_t seed)
	: m_size(size), m_frequence(frequence), m_data(frequence * frequence, 0)
{
	RandomHash gen(seed);
	std::uniform_real_distribution<float> d(-amplitude, amplitude);


	for(size_t x = 0 ; x < frequence ; x++)
		for (size_t y = 0; y < frequence; y++)
			m_data[x + y * frequence] = d(gen(x, y));
}

float Perlin2D::operator()(float x, float y) const
{
	/*float decX, decY;
	size_t x1, x2, y1, y2;
	Perlin::splitValue(x, m_size, m_frequence, x1, x2, decX);
	Perlin::splitValue(y, m_size, m_frequence, y1, y2, decY);*/

	auto [x1, x2, decX] = Perlin::splitValue2(x, m_size, m_frequence);
	auto [y1, y2, decY] = Perlin::splitValue2(y, m_size, m_frequence);

	float v1 = m_data[x1 + y1 * m_frequence];
	float v2 = m_data[x2 + y1 * m_frequence];
	float v3 = m_data[x1 + y2 * m_frequence];
	float v4 = m_data[x2 + y2 * m_frequence];

	return square2DLerp(v1, v2, v3, v4, decX, decY);
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

float squareLerp(float a, float b, float x)
{
	if (x <= 0.5f)
		return linearLerp(a, b, 2 * x * x);
	return linearLerp(a, b, -2 * (x - 1)*(x - 1) + 1);
}

float cos2DLerp(float a, float b, float c, float d, float x, float y)
{
	float x1 = cosLerp(a, b, x);
	float x2 = cosLerp(c, d, x);
	return cosLerp(x1, x2, y);
}

float linear2DLerp(float a, float b, float c, float d, float x, float y)
{
	float x1 = linearLerp(a, b, x);
	float x2 = linearLerp(c, d, x);
	return linearLerp(x1, x2, y);
}

float square2DLerp(float a, float b, float c, float d, float x, float y)
{
	float x1 = squareLerp(a, b, x);
	float x2 = squareLerp(c, d, x);
	return squareLerp(x1, x2, y);
}

float cos3DLerp(float a, float b, float c, float d, float e, float f, float g, float h, float x, float y, float z)
{
	float x1 = cosLerp(a, e, x);
	float x2 = cosLerp(b, f, x);
	float x3 = cosLerp(c, g, x);
	float x4 = cosLerp(d, h, x);
	return cos2DLerp(x1, x2, x3, x4, y, z);
}