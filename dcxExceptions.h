#pragma once

//
// DCX Specific exceptions
//
// Exceptions take a format string & arguments.
// Format string uses a '%' character as a placeholder for arguments, the '\' character can be used to except a '%' character so that its not taken as a place holder.
// Arguments can be any object thats supported by TString += & = operators (or that converts to a supported one)
//

//#include <stdexcept>
#include "Classes\tstring\tstring.h"
namespace Dcx {
	//namespace detail {
	//	static TString tsErr;
	//
	//	template <typename Format, typename Value, typename... Arguments>
	//	const TString &dcx_exception(const Format &fmt, const Value val, Arguments&&... args)
	//	{
	//		return _ts_sprintf(tsErr, fmt, val, args...);
	//	}
	//	template <typename Format>
	//	const TString &dcx_exception(const Format &fmt)
	//	{
	//		tsErr = fmt;
	//		return tsErr;
	//	}
	//}
	//
	//template <typename Format, typename Value, typename... Arguments>
	//void dcx_exception_InvalidArgument(const Format &fmt, const Value val, Arguments&&... args)
	//{
	//	throw std::invalid_argument(detail::dcx_exception(fmt, val, args...).c_str());
	//}
	//
	//template <typename Format>
	//void dcx_exception_InvalidArgument(const Format &fmt)
	//{
	//	throw std::invalid_argument(detail::dcx_exception(fmt).c_str());
	//}
	//
	//template <typename Format, typename Value, typename... Arguments>
	//void dcx_exception_RuntimeError(const Format &fmt, const Value val, Arguments&&... args)
	//{
	//	throw std::runtime_error(detail::dcx_exception(fmt, val, args...).c_str());
	//}
	//
	//template <typename Format>
	//void dcx_exception_RuntimeError(const Format &fmt)
	//{
	//	throw std::runtime_error(detail::dcx_exception(fmt).c_str());
	//}

	//template <typename T>
	//class dcxException
	//	: std::exception
	//{
	//private:
	//	std::basic_string<T> tErrorStr;
	//public:
	//	dcxException()
	//	{
	//
	//	}
	//	//template <std::size_t N>
	//	//dcxException(T const (&msg)[N])
	//	//{
	//	//	for (auto cnt = decltype(N){0}; cnt < N; cnt++)
	//	//	{
	//	//		tErrorStr[cnt] = msg[cnt];
	//	//	}
	//	//	tErrorStr[N] = T();
	//	//}
	//
	//	explicit dcxException(const T &msg)
	//		: tErrorStr(msg)
	//	{
	//	}
	//	template <typename Format, typename Value, typename... Arguments>
	//	dcxException(const Format &fmt, const Value val, Arguments&&... args)
	//	{
	//		_ts_sprintf(tErrorStr, fmt, val, args...);
	//	}
	//	const T *what() const noexcept { return tErrorStr.c_str(); }
	//};

	class dcxException
		: public std::exception
	{
	public:
		explicit dcxException(const char *const msg)
			: exception(msg)
		{
			tsErr = msg;
		}
		explicit dcxException(const WCHAR *const msg)
			: exception(TString(msg).c_str())
		{
			tsErr = msg;
		}
		template <typename Format, typename Value, typename... Arguments>
		dcxException(const Format &fmt, const Value val, Arguments&&... args)
			: exception(_ts_sprintf(tsErr, fmt, val, args...).c_str())
		{
			
		}
		const TCHAR *const reason() const noexcept { return !tsErr.empty() ? tsErr.to_chr() : TEXT("Unknown Exception"); }
	private:
		static TString tsErr;
	};
}

//
