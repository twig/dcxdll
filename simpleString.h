#pragma once

#include "Classes\tstring\tstring.h"

// simpleString<type, size>
//  type = char or wchar_t
//  size = the size of the buffer to allocate.
//
template <typename T, size_t N>
struct simpleString {
	static_assert(N > 1, "N Must be > 1");
	static_assert(std::is_same_v<char, std::remove_cv_t<T>> || std::is_same_v<wchar_t, std::remove_cv_t<T>>, "Type must be char or wchar_t");

	using value_type = std::remove_cv_t<T>;
	using pointer = std::add_pointer_t<value_type>;
	using reference = std::add_lvalue_reference_t<value_type>;
	using const_value_type = std::add_const_t<value_type>;
	using const_pointer = std::add_const_t<pointer>;
	using const_reference = std::add_const_t<reference>;
	using size_type = std::size_t;

	constexpr simpleString() noexcept
		: m_data{ 0 }, m_pData(&m_data[0])
	{
	}
	constexpr simpleString(const simpleString<T,N> &other) = default;
	constexpr simpleString(simpleString<T,N> &&other) = default;
	constexpr simpleString(const_pointer other) noexcept
		: m_data{ 0 }, m_pData(&m_data[0])
	{
		_ts_strcpyn(&m_data[0], other, N);
	}
	constexpr simpleString(const value_type &other) noexcept
		: m_data{ other, value_type{} }, m_pData(&m_data[0])
	{
	}

	simpleString<T, N> &operator =(const simpleString<T, N> &other) = default;
	simpleString<T, N> &operator =(simpleString<T, N> &&other) = default;
	simpleString<T, N> &operator =(const_pointer other) noexcept
	{
		_ts_strcpyn(&m_data[0], other, N);
		return *this;
	}

	template <std::size_t otherSize>
	simpleString<T, N> &operator +=(const_value_type (&other)[otherSize]) noexcept
	{
		const size_type nLen = length();
		const size_type nOtherLen = otherSize;
		const size_type nDiff = N - (nLen + nOtherLen);

		if (nDiff > 1U)	// > 1 to account for zero char
			_ts_strncat(&m_data[0], &other[0], nDiff);

		return *this;
	}
	simpleString<T, N> &operator +=(const_pointer other) noexcept
	{
		const size_type nLen = length();
		const size_type nOtherLen = _ts_strlen(other);
		const size_type nDiff = N - (nLen + nOtherLen);

		if (nDiff > 1U)	// > 1 to account for zero char
			_ts_strncat(&m_data[0], other, nDiff);

		return *this;
	}
	simpleString<T, N> &operator +=(const_reference other) noexcept
	{
		const size_type nLen = length();
		const size_type nDiff = N - (nLen + 1);

		if (nDiff > 1U) {
			// > 1 to account for zero char
			m_data[nLen] = other;
			m_data[nLen + 1] = value_type();
		}
		return *this;
	}
	simpleString<T, N> &operator +=(const simpleString<T, N> &other) noexcept
	{
		const size_type nLen = length();
		const size_type nOtherLen = other.length();
		const size_type nDiff = N - (nLen + nOtherLen);

		if (nDiff > 1U)	// > 1 to account for zero char
			_ts_strncat(&m_data[0], &other.m_data[0], nDiff);

		return *this;
	}

	template <std::size_t otherSize>
	constexpr bool operator ==(const_value_type(&other)[otherSize]) const noexcept { return (*this == &other[0]); }
	constexpr bool operator ==(const simpleString<T, N> &other) const noexcept { return (*this == other.data()); }
	constexpr bool operator ==(const_pointer other) const noexcept { return (_ts_strncmp(&m_data[0], other, N) == 0); }

	template <std::size_t otherSize>
	constexpr bool operator !=(const_value_type(&other)[otherSize]) const noexcept { return !(*this == &other[0]); }
	constexpr bool operator !=(const simpleString<T, N> &other) const noexcept { return !(*this == other); }
	constexpr bool operator !=(const_pointer other) const noexcept { return !(*this == other); }

	constexpr explicit operator bool() const noexcept { return !empty(); }
	//constexpr operator pointer() const noexcept { return const_cast<pointer>(&m_data[0]); }
	constexpr operator const_pointer() const noexcept { return m_pData; }
	constexpr reference operator [](const size_type &iOffSet) const noexcept { return m_data[iOffSet]; }
	constexpr size_type length() const { return _ts_strnlen((const_pointer)&m_data[0], N); }
	constexpr const size_type size() const noexcept { return N; }
	constexpr const size_type bytes() const noexcept { return N * sizeof(value_type); }
	//constexpr pointer data() const noexcept { return const_cast<pointer>(&m_data[0]); }
	constexpr const_pointer data() const noexcept { return m_pData; }
	constexpr bool empty() const noexcept { return (m_data[0] == value_type()); }
	void clear() noexcept { m_data[0] = value_type(); }

private:
	value_type	m_data[N];
	pointer		m_pData;
};

template <size_t N>
using swString = simpleString<wchar_t, N>;

template <size_t N>
using sString = simpleString<char, N>;

template <size_t N>
using csString = simpleString<const char, N>;

template <size_t N>
using cswString = simpleString<const wchar_t, N>;

template <size_t N>
using stString = simpleString<TCHAR, N>;

template <size_t N>
using cstString = simpleString<const TCHAR, N>;
