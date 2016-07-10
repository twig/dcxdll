#pragma once

#include "Classes\tstring\tstring.h"

// refString<type, size>
//  type = char or wchar_t
//  size = the size of the buffer already allocated.
//
// NB: NO buffer is allocated, all data MUST be preallocated.
template <typename T, size_t N>
struct refString
{
	static_assert(N > 1, "N Must be > 1");
	static_assert(std::is_same<char, std::remove_cv_t<T>>::value || std::is_same<wchar_t, std::remove_cv_t<T>>::value, "Type must be char or wchar_t");

	using value_type = T;
	using const_value_type = std::add_const_t<value_type>;
	using size_type = std::size_t;
	using pointer = std::add_pointer_t<value_type>;
	using const_pointer = std::add_const_t<pointer>;
	using reference = std::add_lvalue_reference_t<value_type>;
	using const_reference = std::add_const_t<reference>;

	constexpr refString() = delete;
	constexpr refString(const refString &other) = delete;
	constexpr refString(refString &&other) = delete;

	constexpr refString(const_pointer other) noexcept
		: m_data(other)
	{

	}
	//constexpr refString(const_reference (other)[N]) noexcept
	//	: m_data(other)
	//{

	//}

	~refString() = default;

	const refString &operator =(const value_type *const other) const noexcept {
		_ts_strcpyn(m_data, other, N);
		return *this;
	}

	const refString &operator =(const value_type &other) const noexcept {
		m_data[0] = other;
		m_data[1] = value_type();
		return *this;
	}

	const refString &operator +=(const value_type *const other) const noexcept {
		size_type nLen = length();
		size_type nOtherLen = _ts_strlen(other);
		size_type nDiff = N - (nLen + nOtherLen);

		if (nDiff > 1U)	// > 1 to account for zero char
			_ts_strncat(m_data, other, nDiff);

		return *this;
	}

	const refString &operator +=(const value_type &other) const noexcept {
		size_type nLen = length();
		size_type nDiff = N - (nLen + 1);

		if (nDiff > 1U) {
			// > 1 to account for zero char
			m_data[nLen] = other;
			m_data[nLen + 1] = value_type();
		}
		return *this;
	}

	constexpr bool operator ==(const refString &other) const noexcept { return (*this == other.data()); }
	constexpr bool operator !=(const refString &other) const noexcept { return !(*this == other); }

	constexpr bool operator ==(const_pointer other) const noexcept { return (_ts_strncmp(m_data, other, N) == 0); }
	constexpr bool operator !=(const_pointer other) const noexcept { return !(*this == other); }

	constexpr explicit operator bool() const noexcept { return !empty(); }
	constexpr operator pointer() const noexcept { return const_cast<pointer>(m_data); }
	constexpr reference operator [](const size_type &iOffSet) const noexcept { return m_data[iOffSet]; }
	constexpr size_type length() const { return _ts_strnlen(m_data, N); }
	constexpr const size_type size() const noexcept { return N; }
	constexpr pointer data() const noexcept { return const_cast<pointer>(m_data); }
	constexpr bool empty() const noexcept { return (m_data == nullptr || m_data[0] == value_type()); }
	constexpr void clear() const noexcept { if (!empty()) m_data[0] = value_type(); }

private:
	const_pointer m_data;
};

