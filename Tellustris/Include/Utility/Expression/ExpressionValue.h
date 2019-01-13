#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace NExpression
{
	template <typename T>
	class IValue
	{
	public:
		IValue() = default;
		IValue(const IValue &) = delete;
		IValue & operator=(const IValue &) = delete;
		virtual ~IValue() = default;

		virtual T get() const = 0;
		virtual std::string toString() const = 0;
	};

	template <typename T>
	using ValueRef = std::unique_ptr<IValue<T>>;

	template <typename T>
	class Number : public IValue<T>
	{
	public:
		Number(T value) : m_value(value) {}

		T get() const override
		{
			return m_value;
		}

		void set(T value)
		{
			m_value = value;
		}

		std::string toString() const override
		{
			return std::to_string(m_value);
		}

	protected:
		T m_value;
	};

	template <typename T>
	class Parameter : public Number<T>
	{
	public:
		Parameter(const std::string & name)
			: Number<T>(0)
			, m_name(name)
		{

		}

		std::string name() const
		{
			return m_name;
		}

		std::string toString() const override
		{
			return m_name;
		}

	private:
		std::string m_name;
	};

	template <typename T>
	class OperationSum : public IValue<T>
	{
	public:
		template <typename iterator>
		OperationSum(iterator begin, iterator end)
		{
			auto size = std::distance(begin, end);
			m_values.reserve(size);
			for (auto it = begin; it != end; ++it)
				m_values.emplace_back(std::move(*it));
		}

		T get() const override
		{
			T value = {};
			for (const auto & v : m_values)
				value += v->get();
			return value;
		}

		std::string toString() const override
		{
			std::string value = "( ";
			for (int i = 0; i < m_values.size(); i++)
			{
				value += m_values[i]->toString();
				if (i < m_values.size() - 1)
					value += " + ";
			}
			return value + " )";
		}

	private:
		std::vector<ValueRef<T>> m_values;
	};

	template <typename T>
	class OperationNegate : public IValue<T>
	{
	public:
		OperationNegate(ValueRef<T> value)
			: m_value(std::move(value))
		{

		}

		T get() const override
		{
			return -m_value->get();
		}

		std::string toString() const override
		{
			return "( -" + m_value->toString() + " )";
		}

	private:
		ValueRef<T> m_value;
	};

	template <typename T>
	class OperationProduct : public IValue<T>
	{
	public:
		template <typename iterator>
		OperationProduct(iterator begin, iterator end)
		{
			auto size = std::distance(begin, end);
			m_values.reserve(size);
			for (auto it = begin; it != end; ++it)
				m_values.emplace_back(std::move(*it));
		}

		T get() const override
		{
			T value = 1;
			for (const auto & v : m_values)
				value *= v->get();
			return value;
		}

		std::string toString() const override
		{
			std::string value = "( ";
			for (int i = 0; i < m_values.size(); i++)
			{
				value += m_values[i]->toString();
				if (i < m_values.size() - 1)
					value += " * ";
			}
			return value + " )";
		}

	private:
		std::vector<ValueRef<T>> m_values;
	};

	template <typename T>
	class OperationReciprocal : public IValue<T>
	{
	public:
		template <typename iterator>
		OperationReciprocal(iterator begin, iterator end)
		{
			auto size = std::distance(begin, end);
			m_values.reserve(size);
			for (auto it = begin; it != end; ++it)
				m_values.emplace_back(std::move(*it));
		}

		T get() const override
		{
			T value = m_values[0]->get();
			for (size_t i = 1; i < m_values.size(); i++)
				value /= m_values[i]->get();

			return value;
		}

		std::string toString() const override
		{
			std::string value = "( ";
			for (int i = 0; i < m_values.size(); i++)
			{
				value += m_values[i]->toString();
				if (i < m_values.size() - 1)
					value += " / ";
			}
			return value + " )";
		}

	private:
		std::vector<ValueRef<T>> m_values;
	};

	template <typename T>
	class OperationPower : public IValue<T>
	{
	public:
		OperationPower(ValueRef<T> value, ValueRef<T> power)
			: m_value(std::move(value))
			, m_power(std::move(power))
		{

		}

		T get() const override
		{
			return static_cast<T>(std::pow(m_value->get(), m_power->get()));
		}

		std::string toString() const override
		{
			return "( " + m_value->toString() + " ^ " + m_power->toString() + " )";
		}

	private:
		ValueRef<T> m_value;
		ValueRef<T> m_power;
	};

	template <typename T>
	class Constant : public IValue<T>
	{
	public:
		Constant(const std::string & name, T value)
			: m_name(name)
			, m_value(value)
		{

		}

		T get() const override
		{
			return m_value;
		}

		std::string toString() const override
		{
			return std::to_string(m_value);
		}

	private:
		std::string m_name;
		T m_value;
	};

	template <typename T>
	using ExpressionFunction = std::function<T(std::vector<T>)>;

	template <typename T>
	class Function : public IValue<T>
	{
	public:
		template <typename iterator>
		Function(const std::string & functionName, ExpressionFunction<T> f, iterator begin, iterator end)
			: m_functionName(functionName)
			, m_function(f)
		{
			auto size = std::distance(begin, end);
			m_values.reserve(size);
			for (auto it = begin; it != end; ++it)
				m_values.emplace_back(std::move(*it));
		}

		T get() const override
		{
			std::vector<T> values;
			values.reserve(m_values.size());
			for (const auto & v : m_values)
				values.push_back(v->get());

			return m_function(values);
		}

		std::string toString() const override
		{
			std::string value = m_functionName + "( ";
			for (int i = 0; i < m_values.size(); i++)
			{
				value += m_values[i]->toString();
				if (i < m_values.size() - 1)
					value += ", ";
			}
			return value + " )";
		}

	private:
		std::string m_functionName;
		ExpressionFunction<T> m_function;
		std::vector<ValueRef<T>> m_values;
	};

	template <typename T>
	class ComparisonSup : public IValue<T>
	{
	public:
		ComparisonSup(ValueRef<T> left, ValueRef<T> right)
			: m_left(std::move(left))
			, m_right(std::move(right))
		{

		}

		T get() const override
		{
			return m_left->get() > m_right->get();
		}

		std::string toString() const override
		{
			return "( " + m_left->toString() + " > " + m_right->toString() + " )";
		}

	private:
		ValueRef<T> m_left;
		ValueRef<T> m_right;
	};

	template <typename T>
	class ComparisonSupOrEqual : public IValue<T>
	{
	public:
		ComparisonSupOrEqual(ValueRef<T> left, ValueRef<T> right)
			: m_left(std::move(left))
			, m_right(std::move(right))
		{

		}

		T get() const override
		{
			return m_left->get() >= m_right->get();
		}

		std::string toString() const override
		{
			return "( " + m_left->toString() + " >= " + m_right->toString() + " )";
		}

	private:
		ValueRef<T> m_left;
		ValueRef<T> m_right;
	};

	template <typename T>
	class ComparisonSub : public IValue<T>
	{
	public:
		ComparisonSub(ValueRef<T> left, ValueRef<T> right)
			: m_left(std::move(left))
			, m_right(std::move(right))
		{

		}

		T get() const override
		{
			return m_left->get() < m_right->get();
		}

		std::string toString() const override
		{
			return "( " + m_left->toString() + " < " + m_right->toString() + " )";
		}

	private:
		ValueRef<T> m_left;
		ValueRef<T> m_right;
	};

	template <typename T>
	class ComparisonSubOrEqual : public IValue<T>
	{
	public:
		ComparisonSubOrEqual(ValueRef<T> left, ValueRef<T> right)
			: m_left(std::move(left))
			, m_right(std::move(right))
		{

		}

		T get() const override
		{
			return m_left->get() <= m_right->get();
		}

		std::string toString() const override
		{
			return "( " + m_left->toString() + " <= " + m_right->toString() + " )";
		}

	private:
		ValueRef<T> m_left;
		ValueRef<T> m_right;
	};

	template <typename T>
	class ComparisonEqual : public IValue<T>
	{
	public:
		ComparisonEqual(ValueRef<T> left, ValueRef<T> right)
			: m_left(std::move(left))
			, m_right(std::move(right))
		{

		}

		T get() const override
		{
			return m_left->get() == m_right->get();
		}

		std::string toString() const override
		{
			return "( " + m_left->toString() + " = " + m_right->toString() + " )";
		}

	private:
		ValueRef<T> m_left;
		ValueRef<T> m_right;
	};

	template <typename T>
	class ComparisonUnequal : public IValue<T>
	{
	public:
		ComparisonUnequal(ValueRef<T> left, ValueRef<T> right)
			: m_left(std::move(left))
			, m_right(std::move(right))
		{

		}

		T get() const override
		{
			return m_left->get() != m_right->get();
		}

		std::string toString() const override
		{
			return "( " + m_left->toString() + " != " + m_right->toString() + " )";
		}

	private:
		ValueRef<T> m_left;
		ValueRef<T> m_right;
	};

	template <typename T>
	class ConditionOr : public IValue<T>
	{
	public:
		ConditionOr(ValueRef<T> left, ValueRef<T> right)
			: m_left(std::move(left))
			, m_right(std::move(right))
		{

		}

		T get() const override
		{
			return m_left->get() || m_right->get();
		}

		std::string toString() const override
		{
			return "( " + m_left->toString() + " || " + m_right->toString() + " )";
		}

	private:
		ValueRef<T> m_left;
		ValueRef<T> m_right;
	};

	template <typename T>
	class ConditionAnd : public IValue<T>
	{
	public:
		ConditionAnd(ValueRef<T> left, ValueRef<T> right)
			: m_left(std::move(left))
			, m_right(std::move(right))
		{

		}

		T get() const override
		{
			return m_left->get() && m_right->get();
		}

		std::string toString() const override
		{
			return "( " + m_left->toString() + " && " + m_right->toString() + " )";
		}

	private:
		ValueRef<T> m_left;
		ValueRef<T> m_right;
	};

	template <typename T>
	class ConditionNot : public IValue<T>
	{
	public:
		ConditionNot(ValueRef<T> value)
			: m_value(std::move(value))
		{

		}

		T get() const override
		{
			T zero = {};
			return m_value->get() == zero ? T{ 1 } : T{ 0 };
		}

		std::string toString() const override
		{
			return "( ! " + m_value->toString() + " )";
		}

	private:
		ValueRef<T> m_value;
	};
}