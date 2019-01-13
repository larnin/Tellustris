
#include "ExpressionParser.h"
#include "Utility/StringOperation.h"
#include "Utility/StaticRandomGenerator.h"

#include <cassert>
#include <exception>
#include <stdexcept>
#include <type_traits>
#include <algorithm>
#include <random>

namespace NExpression
{
	template <typename T>
	ExpressionParser<T>::ExpressionParser()
	{
		if constexpr (std::is_same_v<int, T>)
			setParseFunction([](const auto & s) {return std::stoi(s); });
		if constexpr (std::is_same_v<long, T>)
			setParseFunction([](const auto & s) {return std::stol(s); });
		if constexpr (std::is_same_v<long long, T>)
			setParseFunction([](const auto & s) {return std::stoll(s); });

		if constexpr (std::is_same_v<unsigned long, T> || std::is_same_v<unsigned int, T>)
			setParseFunction([](const auto & s) {return std::stoul(s); });
		if constexpr (std::is_same_v<unsigned long long, T>)
			setParseFunction([](const auto & s) {return std::stoul(s); });

		if constexpr (std::is_same_v<float, T>)
			setParseFunction([](const auto & s) {return std::stof(s); });
		if constexpr (std::is_same_v<double, T>)
			setParseFunction([](const auto & s) {return std::stod(s); });
		if constexpr (std::is_same_v<long double, T>)
			setParseFunction([](const auto & s) {return std::stold(s); });

		if constexpr (std::is_integral_v<T> && !std::is_same_v<bool, T>)
		{
			m_functions.emplace("sqrt", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return static_cast<T>(std::sqrt(values[0]));
			});
			m_functions.emplace("ln", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return static_cast<T>(std::log(values[0]));
			});
			m_functions.emplace("log", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return static_cast<T>(std::log10(values[0]));
			});
			m_functions.emplace("min", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return *std::min_element(values.begin(), values.end());
			});
			m_functions.emplace("max", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return *std::max_element(values.begin(), values.end());
			});
			m_functions.emplace("clamp", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				if (values.size() < 3)
					return values[0];
				return std::min(std::max(values[0], values[1]), values[2]);
			});
			m_functions.emplace("rand", [](const std::vector<T> & values) -> T
			{
				StaticRandomGenerator<std::mt19937> rand;
				if (values.empty())
					return rand();
				T min = 0;
				T max = 0;
				if (values.size() == 1)
				{
					min = std::min(T(0), values[0]);
					max = std::max(T(0), values[0]);
				}
				else
				{
					min = std::min(values[0], values[1]);
					max = std::max(values[0], values[1]);
				}
				std::uniform_int_distribution<T> d(min, max);
				return d(rand);
			});
		}

		if constexpr (std::is_floating_point_v<T>)
		{
			m_constants.emplace("pi", T(3.141592653589793));
			m_constants.emplace("e", T(2.718281828459046));

			m_functions.emplace("sqrt", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return std::sqrt(values[0]);
			});
			m_functions.emplace("ln", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return std::log(values[0]);
			});
			m_functions.emplace("log", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return std::log10(values[0]);
			});
			m_functions.emplace("sin", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return std::sin(values[0]);
			});
			m_functions.emplace("cos", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return std::cos(values[0]);
			});
			m_functions.emplace("tan", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return std::tan(values[0]);
			});
			m_functions.emplace("asin", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return std::asin(values[0]);
			});
			m_functions.emplace("acos", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return std::acos(values[0]);
			});
			m_functions.emplace("atan", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return std::atan(values[0]);
			});
			m_functions.emplace("atan2", [](const std::vector<T> & values) -> T
			{
				if (values.size() < 2)
					return 0;
				return std::atan2(values[0], values[1]);
			});
			m_functions.emplace("min", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return *std::min_element(values.begin(), values.end());
			});
			m_functions.emplace("max", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				return *std::max_element(values.begin(), values.end());
			});
			m_functions.emplace("clamp", [](const std::vector<T> & values) -> T
			{
				if (values.empty())
					return 0;
				if (values.size() < 3)
					return values[0];
				return std::min(std::max(values[0], values[1]), values[2]);
			});
			m_functions.emplace("rand", [](const std::vector<T> & values) -> T
			{
				StaticRandomGenerator<std::mt19937> rand;
				T min = 0;
				T max = 1;
				if (values.size() == 1)
				{
					min = std::min(T(0), values[0]);
					max = std::max(T(0), values[0]);
				}
				else if(values.size() > 1)
				{
					min = std::min(values[0], values[1]);
					max = std::max(values[0], values[1]);
				}
				std::uniform_real_distribution<T> d(min, max);
				return d(rand);
			});
		}
	}

	template <typename T>
	void ExpressionParser<T>::addConstant(const std::string & name, T value)
	{
		auto it = m_constants.find(name);
		if (it == m_constants.end())
			m_constants.emplace(name, value);
		else it->second = value;
	}

	template <typename T>
	void ExpressionParser<T>::removeConstant(const std::string & name)
	{
		m_constants.erase(name);
	}

	template <typename T>
	bool ExpressionParser<T>::haveConstant(const std::string & name) const
	{
		auto it = m_constants.find(name);
		return it != m_constants.end();
	}

	template <typename T>
	T ExpressionParser<T>::getConstant(const std::string & name) const
	{
		assert(haveConstant(name));
		auto it = m_constants.find(name);
		return it->second;
	}

	template <typename T>
	void ExpressionParser<T>::addFunction(const std::string & name, ExpressionFunction<T> func)
	{
		assert(func);
		auto it = m_functions.find(name);
		if (it == m_functions.end())
			m_functions.emplace(name, func);
		else it->second = func;
	}

	template <typename T>
	void ExpressionParser<T>::removeFunction(const std::string & name)
	{
		m_functions.erase(name);
	}

	template <typename T>
	bool ExpressionParser<T>::haveFunction(const std::string & name)
	{
		auto it = m_functions.find(name);
		return it != m_functions.end();
	}

	template <typename T>
	ExpressionFunction<T> ExpressionParser<T>::getFunction(const std::string & name) const
	{
		assert(haveFunction(name));
		auto it = m_functions.find(name);
		return it->second;
	}

	template <typename T>
	Expression<T> ExpressionParser<T>::evaluate(const std::string & s)
	{
		m_expression = std::move(Expression<T>());

		m_expression.m_value = parse(s);

		return std::move(m_expression);
	}

	template <typename T>
	ValueRef<T> ExpressionParser<T>::parse(std::string s)
	{
		trim(s);
		auto index = s.find('(');
		while (index != std::string::npos)
		{
			substitudeBracket(s, index);
			index = s.find('(');
		}

		auto value = parseOperators(s);
		if (value)
			return value;

		for (const auto & c : m_constants)
		{
			if (c.first == s)
				return std::make_unique<Constant<T>>(s, c.second);
		}

		auto pos = s.find("&");
		if(pos > 0 && pos != std::string::npos)
		{
			auto fName = s.substr(0, pos);
			for (const auto & f : m_functions)
			{
				if (f.first == fName)
				{
					auto inner = s.substr(pos + 1, s.size() - pos - 2); //...&...;
					size_t bracketIndex = std::stoull(inner);
					if (bracketIndex < m_bracketHeap.size())
						inner = m_bracketHeap[bracketIndex];
					else throw std::invalid_argument("Can't parse substitude token");
					auto parts = split(inner, ',');
					std::vector<ValueRef<T>> values;
					values.reserve(parts.size());
					for (const auto & p : parts)
					{
						auto trimed = trim_copy(p);
						if (!trimed.empty())
							values.emplace_back(parse(trimed));
					}
					return std::make_unique<Function<T>>(f.first, f.second, values.begin(), values.end());
				}
			}
		}

		auto posBegin = s.find('&');
		auto posEnd = s.find(';');
		if (posBegin != std::string::npos && posEnd != std::string::npos && posEnd >= 2)
		{
			auto inner = s.substr(posBegin + 1, posEnd - posBegin - 1);
			size_t bracketIndex = std::stoull(inner);
			if (bracketIndex < m_bracketHeap.size())
				return parse(m_bracketHeap[bracketIndex]);
			throw std::invalid_argument("Can't parse substitude token");
		}
		if (!m_parseFunction)
			throw std::invalid_argument("No parse function found!");
		try
		{
			return std::make_unique<Number<T>>(m_parseFunction(s));
		}
		catch (std::exception e) {}

		if (validIdentifier(s))
		{
			auto param = std::make_unique<Parameter<T>>(s);
			m_expression.addParameter(param.get());
			return param;
		}

		throw std::invalid_argument("Reached unexpected end within the parsing tree");
	}

	template <typename T>
	ValueRef<T> ExpressionParser<T>::parseOperators(std::string & s)
	{
		if (s.find('+') != std::string::npos)
		{
			auto parts = split(s, '+');
			std::vector<ValueRef<T>> exp;
			exp.reserve(parts.size());
			for (const auto & p : parts)
			{
				auto trimed = trim_copy(p);
				if (!trimed.empty())
					exp.emplace_back(parse(trimed));
			}
			if (exp.empty())
				throw std::invalid_argument("Nothing to add!");
			if (exp.size() == 1)
				return std::move(exp[0]);
			return std::make_unique<OperationSum<T>>(exp.begin(), exp.end());
		}
		else if (s.find('-') != std::string::npos)
		{
			auto parts = split(s, '-');
			std::vector<ValueRef<T>> exp;
			exp.reserve(parts.size());
			if (!parts[0].empty())
				exp.emplace_back(parse(parts[0]));
			for (size_t i = 1; i < parts.size(); i++)
			{
				auto trimed = trim_copy(parts[i]);
				if (!trimed.empty())
					exp.emplace_back(std::make_unique<OperationNegate<T>>(parse(trimed)));
			}
			if (exp.empty())
				throw std::invalid_argument("Nothing to sub!");
			if (exp.size() == 1)
				return std::move(exp[0]);
			return std::make_unique<OperationSum<T>>(exp.begin(), exp.end());
		}
		else if (s.find('*') != std::string::npos)
		{
			auto parts = split(s, '*');
			std::vector<ValueRef<T>> exp;
			exp.reserve(parts.size());
			for (const auto & p : parts)
				exp.emplace_back(parse(p));
			if (exp.size() == 1)
				return std::move(exp[0]);
			return std::make_unique<OperationProduct<T>>(exp.begin(), exp.end());
		}
		else if (s.find('/') != std::string::npos)
		{
			auto parts = split(s, '/');
			std::vector<ValueRef<T>> exp;
			exp.reserve(parts.size());
			for (const auto & p : parts)
				exp.emplace_back(parse(p));
			if (exp.size() == 1)
				return std::move(exp[0]);
			return std::make_unique<OperationReciprocal<T>>(exp.begin(), exp.end());
		}
		else if (s.find('^') != std::string::npos)
		{
			auto pos = s.find('^');
			return std::make_unique<OperationPower<T>>(parse(s.substr(0, pos)), parse(s.substr(pos + 1)));
		}		
		else if (s.find("&&") != std::string::npos)
		{
			auto pos = s.find("&&");
			return std::make_unique<ConditionAnd<T>>(parse(s.substr(0, pos)), parse(s.substr(pos + 2)));
		}
		else if (s.find("||") != std::string::npos)
		{
			auto pos = s.find("||");
			return std::make_unique<ConditionOr<T>>(parse(s.substr(0, pos)), parse(s.substr(pos + 2)));
		}
		else if (s.find(">=") != std::string::npos)
		{
			auto pos = s.find(">=");
			return std::make_unique<ComparisonSupOrEqual<T>>(parse(s.substr(0, pos)), parse(s.substr(pos + 2)));
		}
		else if (s.find("<=") != std::string::npos)
		{
			auto pos = s.find("<=");
			return std::make_unique<ComparisonSubOrEqual<T>>(parse(s.substr(0, pos)), parse(s.substr(pos + 2)));
		}
		else if (s.find("!=") != std::string::npos)
		{
			auto pos = s.find("!=");
			return std::make_unique<ComparisonUnequal<T>>(parse(s.substr(0, pos)), parse(s.substr(pos + 2)));
		}
		else if (s.find("==") != std::string::npos)
		{
			auto pos = s.find("==");
			return std::make_unique<ComparisonEqual<T>>(parse(s.substr(0, pos)), parse(s.substr(pos + 2)));
		}
		else if (s.find('=') != std::string::npos)
		{
			auto pos = s.find('=');
			return std::make_unique<ComparisonEqual<T>>(parse(s.substr(0, pos)), parse(s.substr(pos + 1)));
		}
		else if (s.find('>') != std::string::npos)
		{
			auto pos = s.find('>');
			return std::make_unique<ComparisonSup<T>>(parse(s.substr(0, pos)), parse(s.substr(pos + 1)));
		}
		else if (s.find('<') != std::string::npos)
		{
			auto pos = s.find('<');
			return std::make_unique<ComparisonSub<T>>(parse(s.substr(0, pos)), parse(s.substr(pos + 1)));
		}
		else if (s.find('!') != std::string::npos)
		{
			auto pos = s.find('!');
			return std::make_unique<ConditionNot<T>>(parse(s.substr(pos + 1)));
		}

		return {};
	}

	template <typename T>
	void ExpressionParser<T>::substitudeBracket(std::string & s, size_t index)
	{
		auto close = findClosingBracket(s, index, '(', ')');
		if (close != std::string::npos)
		{
			std::string inner = s.substr(index + 1, close - index - 1);
			m_bracketHeap.push_back(inner);
			std::string sub = "&" + (std::to_string(m_bracketHeap.size() - 1)) + ";";
			s = s.substr(0, index) + sub + s.substr(close + 1);
		}
		else throw std::invalid_argument("Bracket not closed!");

	}

	template <typename T>
	size_t ExpressionParser<T>::findClosingBracket(std::string & s, size_t start, char openC, char closeC)
	{
		size_t counter = 0;
		for (size_t i = start; i < s.size(); i++)
		{
			if (s[i] == openC)
				counter++;
			if (s[i] == closeC)
				counter--;
			if (counter == 0)
				return i;
		}
		return std::string::npos;
	}

	template <typename T>
	bool ExpressionParser<T>::validIdentifier(std::string s)
	{
		trim(s);
		if (s.empty())
			return false;
		if (s.size() < 1)
			return false;
		if (s.find(' ') != std::string::npos)
			return false;
		if (std::string("abcdefghijklmnopqrstuvwxyz").find(std::tolower(s[0])) == std::string::npos)
			return false;
		if (m_constants.find(s) != m_constants.end())
			return false;
		if (m_functions.find(s) != m_functions.end())
			return false;
		return true;
	}
}