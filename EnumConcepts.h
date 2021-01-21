#pragma once

#include <concepts>

namespace EnumConcepts
{
	template<class _Ty>
	struct is_Numeric
		: std::integral_constant<bool,
		(std::is_arithmetic_v<_Ty> || std::is_same_v<_Ty, std::byte> || std::is_enum_v<_Ty>)
		&& !std::is_same_v<_Ty, wchar_t>
		&& !std::is_same_v<_Ty, char>
		&& !std::is_pointer_v<_Ty>
		>
	{
	};

	// determine whether T is a Number type (excluding char / wchar), same as is_Numeric<T>::value
	template <typename T>
	constexpr bool is_Numeric_v = is_Numeric<T>::value;

	template <class T>
	concept IsEnum = std::is_enum_v<T>;

	template <class T>
	concept IsNumeric = is_Numeric_v<T>;
}

template <EnumConcepts::IsNumeric T, EnumConcepts::IsEnum E>
constexpr E& operator |=(E& eStyle, const T& dStyle) noexcept
{
	eStyle = static_cast<E>(static_cast<size_t>(eStyle) | static_cast<size_t>(dStyle));
	return eStyle;
}

template <EnumConcepts::IsNumeric T, EnumConcepts::IsEnum E>
constexpr E operator |(const E& eStyle, const T& dStyle) noexcept
{
	return static_cast<E>(static_cast<size_t>(eStyle) | static_cast<size_t>(dStyle));
}

template <EnumConcepts::IsNumeric T, EnumConcepts::IsEnum E>
constexpr E& operator &=(E& eStyle, const T& dStyle) noexcept
{
	eStyle = static_cast<E>(static_cast<size_t>(eStyle) & static_cast<size_t>(dStyle));
	return eStyle;
}

template <EnumConcepts::IsNumeric T, EnumConcepts::IsEnum E>
constexpr E operator &(const E& eStyle, const T& dStyle) noexcept
{
	return static_cast<E>(static_cast<size_t>(eStyle) & static_cast<size_t>(dStyle));
}

template <EnumConcepts::IsNumeric T, EnumConcepts::IsEnum E>
constexpr E operator ^(const E& eStyle, const T& dStyle) noexcept
{
	return static_cast<E>(static_cast<size_t>(eStyle) ^ static_cast<size_t>(dStyle));
}

template <EnumConcepts::IsEnum E>
constexpr E operator ~(const E& eStyle) noexcept
{
	return static_cast<E>(~static_cast<size_t>(eStyle));
}

template <EnumConcepts::IsNumeric T, EnumConcepts::IsEnum E>
constexpr E to_Enum(T t) noexcept
{
	return static_cast<E>(t);
}
