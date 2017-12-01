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
		dcxException(const Format &fmt, const Value &val, Arguments&&... args)
			: exception(_ts_sprintf(tsErr, fmt, val, args...).c_str())
		{
			
		}
		const TCHAR *const reason() const noexcept { return !tsErr.empty() ? tsErr.to_chr() : sUnknown_Err; }
	private:
		static TString tsErr;
		static constexpr TCHAR *sUnknown_Err = TEXT("Unknown Exception");
	};
}

//
