#pragma once

// v1.1 By Ook

#include <concepts>

namespace DcxConcepts
{
	// determine whether _Ty is a Number type (excluding char / wchar)
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

	// is debug mode enabled?
#ifdef NDEBUG
	template<typename T>
	concept IsDebug = false;
#else
	template<typename T>
	concept IsDebug = true;
#endif

	// can be static_cast
	template<typename T, typename U>
	concept StaticCastableTo = requires(T t)
	{
		{ static_cast<U>(t) };
	};

	//namespace detail {
	//	template< class T, class U >
	//	concept SameHelper = std::same_as<T, U>;
	//}
	//template< class T, class U >
	//concept same_as = detail::SameHelper<T, U> && detail::SameHelper<U, T>;

	template <class T>
	concept IsCharText = std::same_as<std::remove_cvref_t<std::remove_all_extents_t<T>>, char>;

	template <class T>
	concept IsWCharText = std::same_as<std::remove_cvref_t<std::remove_all_extents_t<T>>, wchar_t>;

	//template <class T>
	//concept IsTString = std::same_as<std::remove_cvref_t<std::remove_all_extents_t<T>>, TString>;

	template <class T>
	concept IsPODText = IsCharText<T> || IsWCharText<T>;

	template <class T>
	concept IsPODTextPointer = std::is_pointer_v<T> && IsPODText<std::remove_pointer_t<T>>;

	template <class T>
	concept IsNumeric = is_Numeric_v<std::remove_cvref_t<T>>;

	// has Width & Height functions
	template<typename T, typename U>
	concept ImplementsWidthAndHeightFunctions = requires(T t)
	{
		static_cast<U>(t.Width());
		static_cast<U>(t.Height());
	};

	// has data function
	template<typename T>
	concept ImplementsDataFunction = std::is_member_function_pointer_v<decltype(&T::data)>;

	// has length function
	template<typename T>
	concept ImplementsLengthFunction = requires(T t)
	{
		static_cast<std::size_t>(t.length());
	};

	// has data & length functions
	template<typename T>
	concept ImplementsDataAndLengthFunctions = ImplementsDataFunction<T> && ImplementsLengthFunction<T>;

	// has size function
	template<typename T>
	concept ImplementsSizeFunction = requires(T t)
	{
		static_cast<std::size_t>(t.size());
	};

	// container type
	template<typename Iter, typename Container>
	concept container_iterator =
		std::same_as<Iter, typename Container::iterator> ||
		std::same_as<Iter, typename Container::const_iterator>;
	template<typename T>
	concept ImplementsBeginFunction = requires(T t)
	{
		{ t.begin() } -> container_iterator<T>;
	};
	template<typename T>
	concept ImplementsEndFunction = requires(T t)
	{
		{ t.end() } -> container_iterator<T>;
	};
	template<typename T>
	concept ImplementsEraseFunction = requires(T t)
	{
		//t.erase();
		//std::is_member_function_pointer_v<decltype(&T::erase)>;
		//std::is_member_function_pointer_v<decltype(std::declval(t).erase())>;
		//std::declval(t).erase();
		{ t.erase() } -> container_iterator<T>;
	};

	template<typename T>
	concept IsContainer = ImplementsBeginFunction<T> && ImplementsEndFunction<T> /*&& ImplementsEraseFunction<T>*/;

	template<typename T>
	concept ImplementsReleaseFunction = std::is_member_function_pointer_v<decltype(&T::Release)>;

	template<class T, class U>
	concept IsDerivedFrom = std::is_base_of<U, T>::value;

	template<typename T>
	concept ImplementsTrimFunction = std::is_member_function_pointer_v<decltype(&T::trim)>;

	template<typename T>
	concept ImplementsNoTrimFunction = !std::is_member_function_pointer_v<decltype(&T::trim)>;
};
