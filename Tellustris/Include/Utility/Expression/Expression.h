#pragma once

#include "ExpressionValue.h"

#include <vector>
#include <memory>
#include <cassert>

namespace NExpression
{
	namespace
	{
		template <typename T>
		struct ParameterIndex
		{
			Parameter<T> *parameter = nullptr;
			size_t index = 0;
		};
	}

	template <typename T>
	class ExpressionParser;

	template <typename T>
	class Expression
	{
	public:
		friend class ExpressionParser<T>;

		T compute() const
		{
			return m_value->get();
		}

		void setParameter(const std::string & name, T value)
		{
			setParameter(nameIndex(name), value);
		}

		void setParameter(size_t index, T value)
		{
			for (auto & p : m_parameters)
			{
				if (p.index == index)
					p.parameter->set(value);
			}
		}

		void resetParameters()
		{
			for (auto & p : m_parameters)
			{
				T zero = {};
				for (auto & p : m_parameters)
					p.parameter->set(zero);
			}
		}
		
		size_t nameIndex(const std::string & name)
		{
			for (size_t i = 0; i < m_parameterNames.size(); i++)
				if (m_parameterNames[i] == name)
					return i;

			return m_parameterNames.size();
		}

		std::string toString()
		{
			return m_value->toString();
		}

	private:
		Expression() = default;

		void addParameter(Parameter<T> * parameter)
		{
			auto index = nameIndex(parameter->name());
			if (index == m_parameterNames.size())
				m_parameterNames.push_back(parameter->name());
			m_parameters.push_back(ParameterIndex<T>{ parameter, index });
		}

		ValueRef<T> m_value;

		std::vector<std::string> m_parameterNames;
		std::vector<ParameterIndex<T>> m_parameters;
	};
}