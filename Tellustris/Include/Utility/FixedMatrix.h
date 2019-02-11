#pragma once

#include <cassert>
#include <array>

template <typename T, size_t WIDTH, size_t HEIGHT>
class FixedMatrix
{
public:

	using reference = typename std::array<T, WIDTH * HEIGHT>::reference;
	using const_reference = typename std::array<T, WIDTH * HEIGHT>::const_reference;
	using iterator = typename std::array<T, WIDTH * HEIGHT>::iterator;
	using const_iterator = typename std::array<T, WIDTH * HEIGHT>::const_iterator;

	FixedMatrix(T defaultValue = T())
	{
		m_datas.fill(defaultValue);
	}

	reference operator()(size_t x, size_t y)
	{
		assert(x < WIDTH && y < HEIGHT);
		return m_datas[x + y * WIDTH];
	}

	const_reference operator()(size_t x, size_t y) const
	{
		assert(x < WIDTH && y < HEIGHT);
		return m_datas[x + y * WIDTH];
	}

	size_t width() const
	{
		return WIDTH;
	}

	size_t height() const
	{
		return HEIGHT;
	}

	iterator begin()
	{
		return m_datas.begin();
	}

	iterator end()
	{
		return m_datas.end();
	}

	const_iterator begin() const
	{
		return m_datas.begin();
	}

	const_iterator end() const
	{
		return m_datas.end();
	}

private:
	std::array<T, WIDTH * HEIGHT> m_datas;
};