#pragma once

#include "ExpressionValue.h"
#include "Expression.h"

#include <memory>
#include <vector>
#include <map>
#include <functional>

namespace NExpression
{
	template <typename T>
	class ExpressionParser
	{
	public:
		ExpressionParser();

		void setParseFunction(const std::function<T(std::string)> & f)
		{
			m_parseFunction = f;
		}

		void addConstant(const std::string & name, T value);
		void removeConstant(const std::string & name);
		bool haveConstant(const std::string & name) const;
		T getConstant(const std::string & name) const;

		void addFunction(const std::string & name, ExpressionFunction<T> func);
		void removeFunction(const std::string & name);
		bool haveFunction(const std::string & name);
		ExpressionFunction<T> getFunction(const std::string & name) const;

		Expression<T> evaluate(const std::string & s);

	private:
		ValueRef<T> parse(std::string s);
		ValueRef<T> parseOperators(std::string & s);

		void substitudeBracket(std::string & s, size_t index);
		size_t findClosingBracket(std::string & s, size_t start, char openC, char closeC);
		bool validIdentifier(std::string s);

		std::vector<std::string> m_bracketHeap;
		std::function<T(std::string)> m_parseFunction;
		Expression<T> m_expression;

		std::map<std::string, T> m_constants;
		std::map<std::string, ExpressionFunction<T>> m_functions;
	};
}

#include "ExpressionParser.inl"
