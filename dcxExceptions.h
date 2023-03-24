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
	class dcxException final
		: public std::exception
	{
	public:
		explicit dcxException(const char *const msg)
			: exception(msg)
		{
			tsErr = msg;
		}
		explicit dcxException(const wchar_t *const msg)
			: exception(TString(msg).c_str())
		{
			tsErr = msg;
		}
		template <typename Format, typename Value, typename... Arguments>
		dcxException(const Format &fmt, const Value &val, Arguments&&... args)
			: exception(_ts_sprintf(tsErr, fmt, val, args...).c_str())
		{
			
		}
		const TCHAR *const reason() const noexcept { return !tsErr.empty() ? tsErr.to_chr() : &sUnknown_Err[0]; }
	private:
		inline static TString tsErr;
		static constexpr TCHAR sUnknown_Err[] = TEXT("Unknown Exception");
	};


	class dcxException2 final
	{
	public:
		explicit dcxException2(const TCHAR* const msg)
		{
			tsErr.clear();
			if (msg)
				tsErr = msg;
		}
		explicit dcxException2(const std::wstring& msg)
		{
			tsErr.clear();
			if (!msg.empty())
				tsErr = msg;
		}

		//template <class... Arguments>
		//dcxException2(std::format_string<Arguments...> fmt, Arguments&&... args)
		//{
		//	auto s = std::format(fmt, std::forward<Arguments>(args)...);
		//}

		//template <class... Arguments>
		//dcxException2(std::wformat_string<Arguments...> fmt, Arguments&&... args)
		//{
		//	tsErr = std::format(fmt, std::forward<Arguments>(args)...);
		//}
		const TCHAR* const reason() const noexcept { return !tsErr.empty() ? tsErr.c_str() : &sUnknown_Err[0]; }
	private:
		inline static std::basic_string<TCHAR> tsErr;
		static constexpr TCHAR sUnknown_Err[] = TEXT("Unknown Exception");
	};
}

namespace DcxExceptions
{
	class dcxUnableToCreateWindow final
		: public std::exception
	{
	public:
		dcxUnableToCreateWindow() noexcept
			: exception("Unable To Create Window")
		{}
	};
	class dcxInvalidArguments final
		: public std::exception
	{
	public:
		dcxInvalidArguments() noexcept
			: exception("Invalid Arguments")
		{}
	};
	class dcxInvalidFilename final
		: public std::exception
	{
	public:
		dcxInvalidFilename() noexcept
			: exception("Invalid Filename")
		{}
	};
	class dcxInvalidFlag final
		: public std::exception
	{
	public:
		dcxInvalidFlag() noexcept
			: exception("Invalid Flag")
		{}
	};
	class dcxInvalidPath final
		: public std::exception
	{
	public:
		dcxInvalidPath() noexcept
			: exception("Invalid Path")
		{}
		dcxInvalidPath(const char *szPath) noexcept
			: exception(szPath)
		{}
	};
	class dcxInvalidCommand final
		: public std::exception
	{
	public:
		dcxInvalidCommand() noexcept
			: exception("Invalid Command")
		{}
	};
	class dcxInvalidItem final
		: public std::exception
	{
	public:
		dcxInvalidItem() noexcept
			: exception("Invalid Item")
		{}
	};
	class dcxOutOfRange final
		: public std::exception
	{
	public:
		dcxOutOfRange() noexcept
			: exception("Out Of Range")
		{}
	};
	class dcxIDAlreadyExists final
		: public std::exception
	{
	public:
		dcxIDAlreadyExists() noexcept
			: exception("Invalid ID, already exists")
		{}
	};
	class dcxUnableToCreateImageList final
		: public std::exception
	{
	public:
		dcxUnableToCreateImageList() noexcept
			: exception("Unable To Create ImageList")
		{}
	};
	class dcxUnableToLoadIcon final
		: public std::exception
	{
	public:
		dcxUnableToLoadIcon() noexcept
			: exception("Unable To Load Icon")
		{}
	};
	class dcxUnableToAddImage final
		: public std::exception
	{
	public:
		dcxUnableToAddImage() noexcept
			: exception("Unable To Add Image to ImageList")
		{}
	};
	class dcxUnableToGetItem final
		: public std::exception
	{
	public:
		dcxUnableToGetItem() noexcept
			: exception("Unable To Get Item")
		{}
	};
}

//
