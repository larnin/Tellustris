#pragma once

#include "RandomHash.h"

#include <random>
#include <vector>

class Perlin
{
public:
	Perlin(size_t size, float amplitude, size_t frequence, size_t seed);

	float operator()(float x) const;
	float operator()(float x, float y) const;
	float operator()(float x, float y, float z) const;

	struct SplitOut
	{
		size_t x1;
		size_t x2;
		float dec;
	};


	static SplitOut splitValue2(float value, size_t size, size_t frequence);
	static void splitValue(float value, size_t size, size_t frequence, size_t & outX1, size_t & outX2, float & outDec);

private:

	size_t m_size;
	float m_amplitude;
	size_t m_frequence;

	RandomHash m_gen;
	std::uniform_real_distribution<float> m_distrib;
};

class Perlin2D
{
public:
	Perlin2D(size_t size, float amplitude, size_t frequence, size_t seed);

	float operator()(float x, float y) const;

private:
	size_t m_size;
	size_t m_frequence;

	std::vector<float> m_data;
};

/* retourne la valeur à la position x, avec a et b en valeurs aux bornes
 * x doit etre compris entre 0 et 1
 * */
float linearLerp(float a, float b, float x);
float cosLerp(float a, float b, float x);
float squareLerp(float a, float b, float x);

/* interpolation 2d, avec :
 * a en [0,0], b en [0,1], c en [1,0] et d en [1,1]
 * x et y compris entre 0 et 1
 * */
float cos2DLerp(float a, float b, float c, float d, float x, float y);
float linear2DLerp(float a, float b, float c, float d, float x, float y);
float square2DLerp(float a, float b, float c, float d, float x, float y);

/* interpolation 2d, avec :
 * a en [0,0,0], b en [0,0,1], c en [0,1,0] et d en [0,1,1]
 * e en [1,0,0], f en [1,0,1], g en [1,1,0] et h en [1,1,1]
 * x , y et z compris entre 0 et 1
 * */
float cos3DLerp(float a, float b, float c, float d, float e, float f, float g, float h, float x, float y, float z);

