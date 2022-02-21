#pragma once

// v1.2 By Ook
//

#include <concepts>
#include <gsl/gsl>

namespace EnumConcepts
{
	template <class T>
	concept IsEnum = std::is_enum_v<T>;

	template<class _Ty>
	struct is_Numeric
		: std::integral_constant<bool,
		(std::is_arithmetic_v<_Ty> || std::is_same_v<_Ty, std::byte> || std::is_enum_v<_Ty>)
		&& !std::is_same_v<_Ty, wchar_t>
		&& !std::is_same_v<_Ty, char>
		&& !std::is_pointer_v<_Ty>
		//&& !std::is_enum_v<_Ty>
		>
	{
	};

	// determine whether T is a Number type (excluding char / wchar), same as is_Numeric<T>::value
	template <typename T>
	constexpr bool is_Numeric_v = is_Numeric<T>::value;

	template <class T>
	concept IsNumeric = is_Numeric_v<T>;
}

//template <EnumConcepts::IsEnum E>
//constexpr E& operator |=(E& eStyle, const E& dStyle) noexcept
//{
//	if constexpr (sizeof(E) <= sizeof(size_t))
//		eStyle = static_cast<E>(static_cast<size_t>(eStyle) | static_cast<size_t>(dStyle));
//	else
//		eStyle = static_cast<E>(static_cast<uint64_t>(eStyle) | static_cast<uint64_t>(dStyle));
//	return eStyle;
//}

template <EnumConcepts::IsNumeric T, EnumConcepts::IsEnum E>
constexpr E& operator |=(E& eStyle, const T& dStyle) noexcept
{
	if constexpr(sizeof(E) <= sizeof(size_t) && sizeof(T) <= sizeof(size_t))
		eStyle = static_cast<E>(static_cast<size_t>(eStyle) | static_cast<size_t>(dStyle));
	else
		eStyle = static_cast<E>(static_cast<uint64_t>(eStyle) | static_cast<uint64_t>(dStyle));
	return eStyle;
}

//template <EnumConcepts::IsNumeric T, EnumConcepts::IsEnum E>
//constexpr T& operator |=(T& dStyle, const E& eStyle) noexcept
//{
//	//if constexpr (sizeof(E) <= sizeof(size_t) && sizeof(T) <= sizeof(size_t))
//	//	dStyle = static_cast<T>(static_cast<size_t>(eStyle) | static_cast<size_t>(dStyle));
//	//else
//	//	dStyle = static_cast<T>(static_cast<uint64_t>(eStyle) | static_cast<uint64_t>(dStyle));
//	//return dStyle;
//
//	dStyle |= static_cast<T>(eStyle);
//
//	return dStyle;
//}

template <EnumConcepts::IsNumeric T, EnumConcepts::IsEnum E>
constexpr E operator |(const E& eStyle, const T& dStyle) noexcept
{
	if constexpr (sizeof(E) <= sizeof(size_t) && sizeof(T) <= sizeof(size_t))
		return static_cast<E>(static_cast<size_t>(eStyle) | static_cast<size_t>(dStyle));
	else
		return static_cast<E>(static_cast<uint64_t>(eStyle) | static_cast<uint64_t>(dStyle));
}

template <EnumConcepts::IsNumeric T, EnumConcepts::IsEnum E>
constexpr E& operator &=(E& eStyle, const T& dStyle) noexcept
{
	if constexpr (sizeof(E) <= sizeof(size_t) && sizeof(T) <= sizeof(size_t))
		eStyle = static_cast<E>(static_cast<size_t>(eStyle) & static_cast<size_t>(dStyle));
	else
		eStyle = static_cast<E>(static_cast<uint64_t>(eStyle) & static_cast<uint64_t>(dStyle));
	return eStyle;
}

template <EnumConcepts::IsNumeric T, EnumConcepts::IsEnum E>
constexpr E operator &(const E& eStyle, const T& dStyle) noexcept
{
	if constexpr (sizeof(E) <= sizeof(size_t) && sizeof(T) <= sizeof(size_t))
		return static_cast<E>(static_cast<size_t>(eStyle) & static_cast<size_t>(dStyle));
	else
		return static_cast<E>(static_cast<uint64_t>(eStyle) & static_cast<uint64_t>(dStyle));
}

template <EnumConcepts::IsNumeric T, EnumConcepts::IsEnum E>
constexpr E operator ^(const E& eStyle, const T& dStyle) noexcept
{
	if constexpr (sizeof(E) <= sizeof(size_t) && sizeof(T) <= sizeof(size_t))
		return static_cast<E>(static_cast<size_t>(eStyle) ^ static_cast<size_t>(dStyle));
	else
		return static_cast<E>(static_cast<uint64_t>(eStyle) ^ static_cast<uint64_t>(dStyle));
}

template <EnumConcepts::IsEnum E>
constexpr E operator ~(const E& eStyle) noexcept
{
	if constexpr (sizeof(E) <= sizeof(size_t))
		return static_cast<E>(~static_cast<size_t>(eStyle));
	else
		return static_cast<E>(~static_cast<uint64_t>(eStyle));
}

////template <EnumConcepts::IsEnum E, EnumConcepts::IsNumeric T = std::underlying_type_t<E> >
//template <class E, class T = std::underlying_type_t<E> >
//constexpr E to_Enum(T &&t) noexcept
//{
//	return static_cast<E>(std::forward<T>(t));
//}
//
////template <EnumConcepts::IsEnum T, EnumConcepts::IsNumeric E = std::underlying_type_t<T> >
//template <class T, class E = std::underlying_type_t<T> >
//constexpr E from_Enum(T &&t) noexcept
//{
//	return static_cast<E>(std::forward<T>(t));
//}

template <EnumConcepts::IsEnum E, EnumConcepts::IsNumeric T = std::underlying_type_t<E> >
constexpr E to_Enum(T t) noexcept
{
	return static_cast<E>(t);
}

template <EnumConcepts::IsEnum T, EnumConcepts::IsNumeric E = std::underlying_type_t<T> >
constexpr E from_Enum(T t) noexcept
{
	return static_cast<E>(t);
}
