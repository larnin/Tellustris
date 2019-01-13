#pragma once

#include <chrono>

template <typename Gen>
class StaticRandomGenerator
{
public:

	using result_type = typename Gen::result_type;
	
	//this genertor can't be seeded
	StaticRandomGenerator() = default;
	StaticRandomGenerator(const StaticRandomGenerator &) = default;
	StaticRandomGenerator(result_type) {}
	template<class Sseq>
	StaticRandomGenerator(Sseq &) {}

	void seed() {}
	void seed(result_type) {}
	template<class Sseq>
	void seed(Sseq &) {}

	result_type operator()()
	{
		return m_gen();
	}

	void discard(unsigned long long z)
	{
		m_gen.discard(z);
	}

	
	static result_type min()
	{
		return Gen::min();
	}

	static result_type max()
	{
		return Gen::max();
	}

	friend bool operator==(const StaticRandomGenerator<Gen> & left, const StaticRandomGenerator<Gen> & right)
	{
		//left and right use the same internal generator
		return true;
	}

	friend bool operator!=(const StaticRandomGenerator<Gen> & left, const StaticRandomGenerator<Gen> & right)
	{
		return false;
	}


private:
	static Gen m_gen;
};

template <typename Gen>
Gen StaticRandomGenerator<Gen>::m_gen(static_cast<typename Gen::result_type>(std::chrono::system_clock::now().time_since_epoch().count()));
