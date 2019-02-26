#pragma once

#include <limits>


class RandomHashValue
{
public:
	using result_type = size_t;

	result_type min() const
	{
		return std::numeric_limits<size_t>::min();
	}

	result_type max() const
	{
		return std::numeric_limits<size_t>::max();
	}

	result_type operator()() const
	{
		return value;
	}

	result_type value;
};

class RandomHash
{
public:
	RandomHash(size_t seed)
		: m_seed(seed)
	{
	
	}

	static size_t hash(size_t value)
	{
		value += 1ULL;
		value ^= value >> 33ULL;
		value *= 0xff51afd7ed558ccdULL;
		value ^= value >> 33ULL;
		value *= 0xc4ceb9fe1a85ec53ULL;
		value ^= value >> 33ULL;
		return value;
	}

	template <typename T>
	static size_t hash(size_t seed, T arg)
	{
		size_t value = hash(seed);
		value = hash(value * 101 + static_cast<size_t>(arg));
		return value;
	}

	template <typename T, typename ... Args>
	static size_t hash(size_t seed, T arg, Args ... args)
	{
		size_t value = hash(seed, std::forward<Args>(args)...);
		value = hash(value * 37 + static_cast<size_t>(arg));
		return value;
	}

	template <typename ... Args>
	RandomHashValue & operator()(Args ... args) const
	{
		m_value.value = hash(m_seed, std::forward<Args>(args)...);
		return m_value;
	}

	void seed(size_t value)
	{
		m_seed = value;
	}

	size_t seed() const
	{
		return m_seed;
	}

private:
	size_t m_seed;
	mutable RandomHashValue m_value;
};

