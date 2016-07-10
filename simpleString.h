#pragma once

#include "Classes\tstring\tstring.h"

// simpleString<type, size>
//  type = char or wchar_t
//  size = the size of the buffer to allocate.
//
template <typename T, size_t N>
struct simpleString {
	static_assert(N > 1, "N Must be > 1");
	static_assert(std::is_same<char, std::remove_cv_t<T>>::value || std::is_same<wchar_t, std::remove_cv_t<T>>::value, "Type must be char or wchar_t");

	using value_type = T;
	using pointer = std::add_pointer_t<value_type>;
	using reference = std::add_lvalue_reference_t<value_type>;
	using const_value_type = std::add_const_t<value_type>;
	using const_pointer = std::add_const_t<pointer>;
	using const_reference = std::add_const_t<reference>;
	using size_type = std::size_t;

	constexpr simpleString()
		: m_data{ 0 }
	{
	}
	constexpr simpleString(const simpleString &other) = default;
	constexpr simpleString(simpleString &&other) = default;
	constexpr simpleString(const_pointer other)
		: m_data{ 0 }
	{
		_ts_strcpyn(m_data, other, N);
	}
	constexpr simpleString(const value_type &other)
		: m_data{ other, value_type{} }
	{
	}
	simpleString &operator =(const simpleString &other) = default;
	simpleString &operator =(simpleString &&other) = default;
	simpleString &operator =(const_pointer other) noexcept {
		_ts_strcpyn(m_data, other, N);
		return *this;
	}
	simpleString &operator +=(const_pointer other) noexcept {
		size_type nLen = length();
		size_type nOtherLen = _ts_strlen(other);
		size_type nDiff = N - (nLen + nOtherLen);

		if (nDiff > 1U)	// > 1 to account for zero char
			_ts_strncat(m_data, other, nDiff);

		return *this;
	}
	simpleString &operator +=(const value_type &other) noexcept {
		size_type nLen = length();
		size_type nDiff = N - (nLen + 1);

		if (nDiff > 1U) {
			// > 1 to account for zero char
			m_data[nLen] = other;
			m_data[nLen + 1] = value_type();
		}
		return *this;
	}
	constexpr bool operator ==(const simpleString &other) const noexcept { return (*this == other.data()); }
	constexpr bool operator !=(const simpleString &other) const noexcept { return !(*this == other); }

	constexpr bool operator ==(const_pointer other) const noexcept { return (_ts_strncmp(m_data, other, N) == 0); }
	constexpr bool operator !=(const_pointer other) const noexcept { return !(*this == other); }

	constexpr explicit operator bool() const noexcept { return !empty(); }
	constexpr operator pointer() const noexcept { return const_cast<pointer>(m_data); }
	constexpr reference operator [](const size_type &iOffSet) const noexcept { return m_data[iOffSet]; }
	constexpr size_type length() const { return _ts_strnlen((const_pointer)m_data, N); }
	constexpr const size_type size() const noexcept { return N; }
	constexpr pointer data() const noexcept { return const_cast<pointer>(m_data); }
	constexpr bool empty() const noexcept { return (m_data[0] == value_type()); }
	void clear() noexcept { m_data[0] = value_type(); }

private:
	value_type m_data[N];
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
