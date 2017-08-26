#pragma once

#include <functional>

template <class T>
class CaseDefault;

template <class T>
class CaseBreakDefault
{
public:
	typedef std::function<void()> Block;

private:
	const T &m_value;
	bool m_match;
	bool m_break;

public:
	explicit CaseBreakDefault(const T &value) :
		m_value(value),
		m_match(false),
		m_break(false)
	{
	}

	CaseBreakDefault(const CaseBreakDefault &other) :
		m_value(other.m_value),
		m_match(other.m_match),
		m_break(other.m_break),
		Block(other.Block)
	{
	}

	CaseBreakDefault<T> &Case(const T &value, const Block &block)
	{
		if (m_break)
			return *this;

		if (m_value == value)
			m_match = true;

		if (m_match && block)
			block();

		return *this;
	}

	CaseBreakDefault<T> &Case(const T &value)
	{
		return Case(value, nullptr);
	}

	CaseDefault<T> &Break()
	{
		if (m_match)
			m_break = true;

		return static_cast<CaseDefault<T> &>(*this);
	}

	void Default(const Block &block)
	{
		if (m_break)
			return;

		if (block)
			block();
	}

private:
	CaseBreakDefault & operator =(const CaseBreakDefault &) = delete;
};

template <class T>
class CaseDefault : public CaseBreakDefault<T>
{
public:
	explicit CaseDefault(const T &value) :
		CaseBreakDefault(value)
	{
	}

private:
	using CaseBreakDefault::Break;
};

template <class T>
CaseDefault<T> Switch(const T &value)
{
	return CaseDefault<T>(value);
}
