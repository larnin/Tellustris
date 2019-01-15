#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace NExpression
{
	template <typename T>
	class IValue;

	template <typename T>
	using ValueRef = std::unique_ptr<IValue<T>>;

	template <typename T>
	class Expression;

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

		virtual ValueRef<T> clone() const = 0;

		virtual void registerParameter(Expression<T> & expression) = 0;
	};

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

		ValueRef<T> clone() const override
		{
			return std::make_unique<Number<T>>(m_value);
		}

		void registerParameter(Expression<T> &) override {}

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

		ValueRef<T> clone() const override
		{
			auto v = std::make_unique<Parameter<T>>(m_name);
			v->set(Number<T>::m_value);
			return v;
		}
		
		void registerParameter(Expression<T> & expression) override
		{
			expression.addParameter(this);
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

		ValueRef<T> clone() const override
		{
			std::vector<ValueRef<T>> values;
			for (const auto & v : m_values)
				values.emplace_back(v->clone());
			return std::make_unique<OperationSum<T>>(values.begin(), values.end());
		}

		void registerParameter(Expression<T> & expression) override
		{
			for (auto & v : m_values)
				v->registerParameter(expression);
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

		ValueRef<T> clone() const override
		{
			return std::make_unique<OperationNegate<T>>(m_value->clone());
		}

		void registerParameter(Expression<T> & expression) override
		{
			m_value->registerParameter(expression);
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

		ValueRef<T> clone() const override
		{
			std::vector<ValueRef<T>> values;
			for (const auto & v : m_values)
				values.emplace_back(v->clone());
			return std::make_unique<OperationProduct<T>>(values.begin(), values.end());
		}

		void registerParameter(Expression<T> & expression) override
		{
			for (auto & v : m_values)
				v->registerParameter(expression);
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

		ValueRef<T> clone() const override
		{
			std::vector<ValueRef<T>> values;
			for (const auto & v : m_values)
				values.emplace_back(v->clone());
			return std::make_unique<OperationReciprocal<T>>(values.begin(), values.end());
		}

		void registerParameter(Expression<T> & expression) override
		{
			for (auto & v : m_values)
				v->registerParameter(expression);
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

		ValueRef<T> clone() const override
		{
			return std::make_unique<OperationPower<T>>(m_value->clone(), m_power->clone());
		}

		void registerParameter(Expression<T> & expression) override
		{
			m_value->registerParameter(expression);
			m_power->registerParameter(expression);
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

		ValueRef<T> clone() const override
		{
			return std::make_unique<Constant<T>>(m_name, m_value);
		}

		void registerParameter(Expression<T> & expression) override {};

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

		ValueRef<T> clone() const override
		{
			std::vector<ValueRef<T>> values;
			for (const auto & v : m_values)
				values.emplace_back(v->clone());
			return std::make_unique<Function<T>>(m_functionName, m_function, values.begin(), values.end());
		}

		void registerParameter(Expression<T> & expression) override
		{
			for (auto & v : m_values)
				v->registerParameter(expression);
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

		ValueRef<T> clone() const override
		{
			return std::make_unique<ComparisonSup<T>>(m_left->clone(), m_right->clone());
		}

		void registerParameter(Expression<T> & expression) override
		{
			m_left->registerParameter(expression);
			m_right->registerParameter(expression);
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

		ValueRef<T> clone() const override
		{
			return std::make_unique<ComparisonSupOrEqual<T>>(m_left->clone(), m_right->clone());
		}

		void registerParameter(Expression<T> & expression) override
		{
			m_left->registerParameter(expression);
			m_right->registerParameter(expression);
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

		ValueRef<T> clone() const override
		{
			return std::make_unique<ComparisonSub<T>>(m_left->clone(), m_right->clone());
		}

		void registerParameter(Expression<T> & expression) override
		{
			m_left->registerParameter(expression);
			m_right->registerParameter(expression);
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

		ValueRef<T> clone() const override
		{
			return std::make_unique<ComparisonSubOrEqual<T>>(m_left->clone(), m_right->clone());
		}

		void registerParameter(Expression<T> & expression) override
		{
			m_left->registerParameter(expression);
			m_right->registerParameter(expression);
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

		ValueRef<T> clone() const override
		{
			return std::make_unique<ComparisonEqual<T>>(m_left->clone(), m_right->clone());
		}

		void registerParameter(Expression<T> & expression) override
		{
			m_left->registerParameter(expression);
			m_right->registerParameter(expression);
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

		ValueRef<T> clone() const override
		{
			return std::make_unique<ComparisonUnequal<T>>(m_left->clone(), m_right->clone());
		}

		void registerParameter(Expression<T> & expression) override
		{
			m_left->registerParameter(expression);
			m_right->registerParameter(expression);
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

		ValueRef<T> clone() const override
		{
			return std::make_unique<ConditionOr<T>>(m_left->clone(), m_right->clone());
		}

		void registerParameter(Expression<T> & expression) override
		{
			m_left->registerParameter(expression);
			m_right->registerParameter(expression);
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

		ValueRef<T> clone() const override
		{
			return std::make_unique<ConditionAnd<T>>(m_left->clone(), m_right->clone());
		}

		void registerParameter(Expression<T> & expression) override
		{
			m_left->registerParameter(expression);
			m_right->registerParameter(expression);
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

		ValueRef<T> clone() const override
		{
			return std::make_unique<ConditionNot<T>>(m_value->clone());
		}

		void registerParameter(Expression<T> & expression) override
		{
			m_value->registerParameter(expression);
		}

	private:
		ValueRef<T> m_value;
	};
}