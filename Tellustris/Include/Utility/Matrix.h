#pragma once

#include <vector>
#include <cassert>

template <typename T>
class Matrix
{
public:

	using reference = typename std::vector<T>::reference;
	using const_reference = typename std::vector<T>::const_reference;
	using iterator = typename std::vector<T>::iterator;
	using const_iterator = typename std::vector<T>::const_iterator;

	Matrix(size_t width, size_t height, T defaultValue = T())
		: m_datas(width*height, defaultValue)
		, m_width(width)
		, m_height(height)
	{}

	reference operator()(size_t x, size_t y)
	{
		assert(x < m_width && y < m_height);
		return m_datas[x + y * m_width];
	}

	const_reference operator()(size_t x, size_t y) const
	{
		assert(x < m_width && y < m_height);
		return m_datas[x + y * m_width];
	}

	size_t width() const
	{
		return m_width;
	}

	size_t height() const
	{
		return m_height;
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

	void resize(size_t newWidth, size_t newHeight, T defaultValue = T())
	{
		Matrix m(newWidth, newHeight, defaultValue);

		for (unsigned int i(0); i < std::min(m_width, newWidth); i++)
			for (unsigned int j(0); j < std::min(m_height, newHeight); j++)
				m(i, j) = std::move(operator ()(i, j));
		m_datas = m.m_datas;
		m_width = newWidth;
		m_height = newHeight;
	}

private:
	std::vector<T> m_datas;
	size_t m_width;
	size_t m_height;
};