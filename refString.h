#pragma once

#include "Classes\tstring\tstring.h"

// refString<type, size>
//  type = char or wchar_t
//  size = the size of the buffer already allocated.
//
// NB: NO buffer is allocated, all data MUST be preallocated.
template <typename T, size_t N>
class refString
{
	static_assert(N > 1, "N Must be > 1");
	static_assert(std::is_same_v<char, std::remove_cv_t<T>> || std::is_same_v<wchar_t, std::remove_cv_t<T>>, "Type must be char or wchar_t");

public:
	using value_type = typename T;
	using const_value_type = std::add_const_t<value_type>;
	using size_type = std::size_t;
	using ptrdiff_type = std::ptrdiff_t;
	using pointer = std::add_pointer_t<value_type>;
	using const_pointer = std::add_const_t<pointer>;
	using reference = std::add_lvalue_reference_t<value_type>;
	using const_reference = std::add_const_t<reference>;

	constexpr refString() noexcept = default;
	constexpr refString(const refString<T, N> &other) noexcept = default;
	constexpr refString(refString<T, N> &&other) noexcept = default;

	constexpr explicit refString(const_pointer other) noexcept
		: m_data(other)
	{	}

	constexpr refString(std::nullptr_t other) noexcept
	{
	}

	~refString() noexcept = default;

	const refString<T,N> &operator =(const value_type *const other) const noexcept
	{
		_ts_strcpyn(m_data, other, N);
		return *this;
	}

	const refString<T, N> &operator =(const value_type &other) const noexcept
	{
		m_data[0] = other;
		m_data[1] = value_type();
		return *this;
	}

	template <typename otherT, typename = std::enable_if_t<std::is_same_v<otherT::value_type, value_type> && std::is_member_function_pointer_v<decltype(&otherT::data)>> >
	const refString<T, N> &operator =(const otherT &other) const noexcept
	{
		_ts_strcpyn(m_data, other.data(), N);
		return *this;
	}

	const refString<T, N> &operator =(const refString<T, N> &&other) const noexcept
	{
		using std::swap;

		swap(m_data, other.m_data);
		return *this;
	}

	const refString<T, N> &operator +=(const value_type *const other) const noexcept
	{
		const size_type nLen = length();
		const size_type nOtherLen = _ts_strlen(other);
		const size_type nDiff = N - (nLen + nOtherLen);

		if (nDiff > 1U)	// > 1 to account for zero char
			_ts_strncat(m_data, other, nDiff);

		return *this;
	}
	const refString<T, N> &operator +=(const value_type &other) const noexcept
	{
		const size_type nLen = length();
		const size_type nDiff = N - (nLen + 1);

		if (nDiff > 1U)
		{
			// > 1 to account for zero char
			m_data[nLen] = other;
			m_data[nLen + 1] = value_type();
		}
		return *this;
	}
	template <std::size_t otherSize>
	const refString &operator +=(const_value_type(&other)[otherSize]) const noexcept { return (*this += &other[0]); }
	template <typename otherT, typename = std::enable_if_t<std::is_same_v<otherT::value_type, value_type> && std::is_member_function_pointer_v<decltype(&otherT::data)>> >
	const refString<T, N> &operator +=(const otherT &other) const noexcept
	{
		const size_type nLen = length();
		const size_type nOtherLen = _ts_strlen(other);
		const size_type nDiff = N - (nLen + nOtherLen);

		if (nDiff > 1U)	// > 1 to account for zero char
			_ts_strncat(m_data, other.data(), nDiff);

		return *this;
	}

	template <std::size_t otherSize>
	constexpr bool operator ==(const_value_type(&other)[otherSize]) const noexcept { return (*this == &other[0]); }
	constexpr bool operator ==(const refString<T, N> &other) const noexcept { return (*this == other.data()); }
	constexpr bool operator ==(const_pointer other) const noexcept { return (_ts_strncmp(m_data, other, N) == 0); }

	template <std::size_t otherSize>
	constexpr bool operator !=(const_value_type(&other)[otherSize]) const noexcept { return !(*this == &other[0]); }
	constexpr bool operator !=(const refString<T, N> &other) const noexcept { return !(*this == other); }
	constexpr bool operator !=(const_pointer other) const noexcept { return !(*this == other); }

	constexpr explicit operator bool() const noexcept { return !empty(); }
	constexpr operator const_pointer() const noexcept { return m_data; }

	constexpr reference operator [](const ptrdiff_type &iOffSet) const noexcept { return m_data[iOffSet]; }

	constexpr size_type length() const noexcept	{ return _ts_strnlen(m_data, N); }
	constexpr const size_type size() const noexcept { return N; }
	constexpr const_pointer data() const noexcept { return m_data; }
	constexpr bool empty() const noexcept { return (m_data == nullptr || m_data[0] == value_type()); }
	constexpr void clear() const noexcept { if (!empty()) m_data[0] = value_type(); }

	constexpr void assign(const refString<T, N> &other) const noexcept
	{
		//std::swap(m_data, other.m_data);

		const_cast<refString *>(this)->swap(other);

		//swap(other);
	}
	void swap(const refString<T, N> &other) noexcept
	{
		using std::swap;

		swap(m_data, other.m_data);
	}

private:
	mutable pointer m_data{ nullptr };
};

