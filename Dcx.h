#pragma once
//#include "defines.h"
#include "mIRCLinker.h"
#include "DcxUXModule.h"
#include "DcxGDIModule.h"
#include "DcxDWMModule.h"
#include "DcxDPIModule.h"
#include "Classes/xpopup/XPopupMenuManager.h"
#include "Classes/xpopup/xmenubar.h"
#include "Classes/DcxDialog.h"
#include "Classes\UltraDock\dcxDock.h"

//#include <sys/stat.h> 
#include <detours.h>

#include <concepts>
#include <utility>
#include <memory>
#include <iterator>
#include <type_traits>
#include <ranges>

//#if __has_include(<codecvt>)
//#include <codecvt>
//#endif

#include "DcxConcepts.h"

//#include "DcxTextRender.h"

// should DCX use the resource wrapper classes below? set to zero to disable
#define DCX_USE_WRAPPERS 1

#ifdef DCX_USE_WRAPPERS
#pragma message ("### DCX OPTION: Use Wrappers - Enabled")
#else
#pragma message ("### DCX OPTION: Use Wrappers - Disabled")
#endif

//namespace std
//{
//	inline string to_string(const wstring& wstr)
//	{
//		//string str;
//		//str.assign(wstr.begin(), wstr.end());
//		//return str;
//
//		using convert_type = std::codecvt_utf8<wchar_t>;
//		std::wstring_convert<convert_type, wchar_t> converter;
//
//		//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
//		return converter.to_bytes(wstr);
//	}
//
//	inline wstring to_wstring(const string& str)
//	{
//		wstring wstr;
//		wstr.assign(str.begin(), str.end());
//		return wstr;
//	}
//
//	inline string to_string(const TString& wstr)
//	{
//		string str(wstr.c_str());
//		return str;
//	}
//
//	inline wstring to_wstring(const TString& str)
//	{
//		wstring wstr(str.to_wchr());
//		return wstr;
//	}
//
//	inline string to_string(const std::byte& num)
//	{
//		return to_string(gsl::narrow_cast<uint8_t>(num));
//	}
//
//	inline wstring to_wstring(const std::byte& num)
//	{
//		return to_wstring(gsl::narrow_cast<uint8_t>(num));
//	}
//}

namespace Dcx
{
	extern std::byte m_iGhostDrag;
	extern IClassFactory* m_pClassFactory;
	extern bool m_bDX9Installed;

	extern DcxDialogCollection Dialogs;
	extern DcxGDIModule GDIModule;
	extern DcxUXModule UXModule;
	extern DcxDWMModule VistaModule;
	extern DcxDPIModule DpiModule;

	/* additions */
	extern XPopupMenuManager XPopups;
	extern XMenuBar XMenubar;

	extern bool m_bErrorTriggered;

	// dragging
	extern bool m_bShowingDragImage;
	extern HIMAGELIST m_hDragImage;
	extern DcxControl* m_pDragSourceCtrl;

	// settings
	extern bool setting_bStaticColours;

	// global menu settings (only applies to currently displayed menu, used internally only)
	extern std::byte m_CurrentMenuAlpha;
	extern bool m_CurrentMenuRounded;

	/// <summary>
	/// Converts a Range to a std::vector.
	/// take from: https://timur.audio/how-to-make-a-container-from-a-c20-range
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="r">- The range to convert</param>
	/// <returns>a std::vector</returns>
	template <class T>
	auto to_vector(const T& r)
	{
		std::vector<std::ranges::range_value_t<decltype(r)>> v;

		if constexpr (std::ranges::sized_range<decltype(r)>)
			v.reserve(std::ranges::size(r));

		std::ranges::copy(r, std::back_inserter(v));
		return v;
	}
	constexpr int32_t dcxMulDiv32(const int32_t iNum, const int32_t iMul, const int32_t iDiv) noexcept
	{
		return gsl::narrow_cast<int32_t>((gsl::narrow_cast<int64_t>(iNum) * gsl::narrow_cast<int64_t>(iMul)) / iDiv);
	}
	template <DcxConcepts::IsNumeric T>
	constexpr T dcxMulDiv(const T& iNum, const T& iMul, const T& iDiv) noexcept
	{
		return gsl::narrow_cast<T>((gsl::narrow_cast<int64_t>(iNum) * gsl::narrow_cast<int64_t>(iMul)) / iDiv);
	}

	template <DcxConcepts::IsNumeric Result, DcxConcepts::IsNumeric Input>
	constexpr Result dcxRound(const Input in) noexcept
	{
		const auto t = gsl::narrow_cast<Result>(in);
		if ((in - gsl::narrow_cast<Input>(t)) > 0.5)
			return t + Result{ 1 };
		return t;
	}

	[[nodiscard("Memory Leak")]] HCURSOR dcxLoadCursorFromFile(const TString& filename);
	[[nodiscard("Memory Leak")]] HCURSOR dcxLoadCursorFromResource(const TCHAR* CursorType);

	//// determine whether _Ty is a Number type (excluding char / wchar)
	//template<class _Ty>
	//struct is_Numeric
	//	: std::integral_constant<bool,
	//		(std::is_arithmetic_v<_Ty> || std::is_same_v<_Ty, std::byte> || std::is_enum_v<_Ty>)
	//		&& !std::is_same_v<_Ty, wchar_t>
	//		&& !std::is_same_v<_Ty, char>
	//		&& !std::is_pointer_v<_Ty>
	//	>
	//{	// determine whether _Ty is a Number type (excluding char / wchar)
	//};
	//
	//// determine whether T is a Number type (excluding char / wchar) & get the value
	//template <typename T>
	//constexpr bool is_Numeric_v = is_Numeric<T>::value;

	// make_resource() function by Eric Scott Barr (http://ericscottbarr.com/blog/2014/04/c-plus-plus-14-and-sdl2-managing-resources/)

#if _MSC_VER > 1800
	template<typename Creator, typename Destructor, typename... Arguments>
	auto make_resource(Creator c, Destructor d, Arguments&&... args)
	{
		auto r = c(std::forward<Arguments>(args)...);
		if (!r) { throw std::runtime_error{ "Unable to create resource" }; }
		typedef typename std::decay<decltype(*r)>::type ResourceType;
		return std::unique_ptr<ResourceType, void(*)(ResourceType*)>(r, d);
	}
#else
	template<typename Creator, typename Destructor, typename... Arguments>
	auto make_resource(Creator c, Destructor d, Arguments&&... args)
		-> std::unique_ptr<typename std::decay<decltype(*c(std::forward<Arguments>(args)...))>::type, void(*)(typename std::decay<decltype(*c(std::forward<Arguments>(args)...))>::type*)>
	{
		auto r = c(std::forward<Arguments>(args)...);
		if (!r) { throw std::runtime_error{ "Unable to create resource" }; }
		typedef typename std::decay<decltype(*r)>::type ResourceType;
		return std::unique_ptr<ResourceType, void(*)(ResourceType*)>(r, d);
	}
#endif

	using dcxCursor_t = std::unique_ptr<HICON__, void(*)(HICON__*)>;
	using dcxIcon_t = std::unique_ptr<HICON__, void(*)(HICON__*)>;
	using dcxFile_t = std::unique_ptr<FILE, void(*)(FILE*)>;
	using dcxHandle_t = std::unique_ptr<ULONG_PTR, void(*)(ULONG_PTR*)>;
	using dcxHDC_t = std::unique_ptr<HDC__, void(*)(HDC)>;
	using dcxBitmap_t = std::unique_ptr<HBITMAP__, void(*)(HBITMAP)>;
	using dcxBSTR_t = std::unique_ptr<OLECHAR, void(*)(BSTR)>;

	/// <summary>
	/// Make a cursor resource
	/// </summary>
	/// <param name="tsFilename"></param>
	/// <returns></returns>
	inline dcxCursor_t make_cursor(const TString& tsFilename) {
		return make_resource(dcxLoadCursorFromFile, [](HCURSOR hCursor) noexcept { if (hCursor != nullptr) DestroyCursor(hCursor); }, tsFilename);
	}

	/// <summary>
	/// Make an icon resource
	/// </summary>
	/// <param name="index"></param>
	/// <param name="filename"></param>
	/// <param name="large"></param>
	/// <param name="flags"></param>
	/// <returns></returns>
	inline dcxIcon_t make_icon(const int index, TString& filename, const bool large, const TString& flags) {
		return make_resource(dcxLoadIcon, [](HICON hIcon) noexcept { if (hIcon != nullptr) DestroyIcon(hIcon); }, index, filename, large, flags);
	}

	/// <summary>
	/// Make a file resource
	/// </summary>
	/// <param name="file"></param>
	/// <param name="modes"></param>
	/// <returns></returns>
	inline dcxFile_t make_file(const WCHAR* file, const WCHAR* modes) { return make_resource(_wfopen, [](FILE* file) noexcept { fclose(file); }, file, modes); }

	/// <summary>
	/// Make an HDC resource
	/// </summary>
	/// <param name="hdc"></param>
	/// <returns></returns>
	inline dcxHDC_t make_hdc(HDC hdc) { return make_resource(CreateCompatibleDC, [](HDC obj) noexcept { DeleteDC(obj); }, hdc); }

	/// <summary>
	/// Make a Bitmap resource
	/// </summary>
	/// <param name="hdc"></param>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <returns></returns>
	inline dcxBitmap_t make_bitmap(HDC hdc, int x, int y) { return make_resource(CreateCompatibleBitmap, [](HBITMAP obj) noexcept { DeleteBitmap(obj); }, hdc, x, y); }

	//inline ULONG_PTR *dcxCreateFile(const TCHAR *file, DWORD dAccess, DWORD dShareMode, LPSECURITY_ATTRIBUTES lpSecurity, DWORD dCreation, DWORD dflags, HANDLE templateFile) {
	//	return (ULONG_PTR *)CreateFile(file, dAccess, dShareMode, lpSecurity, dCreation, dflags, templateFile);
	//}

	/// <summary>
	/// Make a file handle resource
	/// </summary>
	/// <param name="file"></param>
	/// <param name="dAccess"></param>
	/// <param name="dShareMode"></param>
	/// <param name="lpSecurity"></param>
	/// <param name="dCreation"></param>
	/// <param name="dflags"></param>
	/// <param name="templateFile"></param>
	/// <returns></returns>
	inline dcxHandle_t make_filehandle(const TCHAR* file, DWORD dAccess, DWORD dShareMode, LPSECURITY_ATTRIBUTES lpSecurity, DWORD dCreation, DWORD dflags, HANDLE templateFile) {
		//return make_resource(dcxCreateFile, [](ULONG_PTR *file){ CloseHandle((HANDLE)file); }, file, dAccess, dShareMode, lpSecurity, dCreation, dflags, templateFile);
		return make_resource(
			[](const TCHAR* file, DWORD dAccess, DWORD dShareMode, LPSECURITY_ATTRIBUTES lpSecurity, DWORD dCreation, DWORD dflags, HANDLE templateFile) noexcept { return static_cast<ULONG_PTR*>(CreateFile(file, dAccess, dShareMode, lpSecurity, dCreation, dflags, templateFile)); },
			[](ULONG_PTR* file) noexcept { CloseHandle(file); }, file, dAccess, dShareMode, lpSecurity, dCreation, dflags, templateFile);
	}

	/// <summary>
	/// Make a BSTR resource
	/// </summary>
	/// <param name="wstr"></param>
	/// <returns></returns>
	inline dcxBSTR_t make_bstr(const WCHAR* const wstr) { return make_resource(SysAllocString, [](BSTR obj) noexcept { if (obj) SysFreeString(obj); }, wstr); }

	// NB: BaseType can be defined as some other pointer type
	// so we static_cast it later on
	template <typename Unique, typename BaseType = typename Unique::pointer>
	struct dcxResource
	{
		dcxResource() = delete;																		// no default!
		dcxResource(const dcxResource<Unique, BaseType>&) = delete;									// no copy!
		dcxResource<Unique, BaseType>& operator =(const dcxResource<Unique, BaseType>&) = delete;	// No assignments!
		dcxResource(dcxResource<Unique, BaseType>&&) = delete;									// no move!
		dcxResource<Unique, BaseType>& operator =(dcxResource<Unique, BaseType>&&) = delete;	// No assignments!

		using pointer = typename Unique::pointer;
		using element_type = typename Unique::element_type;
		using return_type = BaseType;

		constexpr dcxResource(Unique u) noexcept
			: m_uni(std::move(u))
		{
		}
		constexpr operator BaseType() const noexcept { return get(); }
		constexpr explicit operator bool() const noexcept { return (m_uni != nullptr); }
		[[nodiscard("Memory Leak")]] constexpr BaseType release() const noexcept { return static_cast<BaseType>(m_uni.release()); }
		constexpr BaseType get() const noexcept { return (m_uni.get()); }
		void reset(BaseType u) noexcept { m_uni.reset(u); }
	private:
		Unique		m_uni;
	};

	struct dcxFileResource final
		: dcxResource < dcxFile_t >
	{
		dcxFileResource() = delete;										// no default constructor!
		dcxFileResource(const dcxFileResource&) = delete;				// no copy!
		dcxFileResource& operator =(const dcxFileResource&) = delete;	// No assignments!
		dcxFileResource(dcxFileResource&&) = delete;					// no move constructor!
		dcxFileResource& operator =(dcxFileResource&&) = delete;		// No move assignments!

		// calls _wfopen()
		dcxFileResource(const WCHAR* tsFilename, const WCHAR* tsMode)
			: dcxResource(make_file(tsFilename, tsMode))
		{
		}

		// calls _wfopen()
		dcxFileResource(const TString& tsFilename, const WCHAR* tsMode)
			: dcxResource(make_file(tsFilename.to_wchr(), tsMode))
		{
		}

		/// <summary>
		/// Get the file size (32bit)
		/// </summary>
		/// <returns></returns>
		const uint32_t Size() const noexcept
		{
			// Seek End of file
			if (fseek(this->get(), 0, SEEK_END))
				return 0UL;

			// Read pointer location, because pointer is at the end, results into file size
			const auto size = gsl::narrow_cast<uint32_t>(ftell(this->get()));

			// Get back to file beginning
			if (fseek(this->get(), 0, SEEK_SET))
				return 0UL;

			return size;
		}

		/// <summary>
		/// Get the file size (64bit)
		/// </summary>
		/// <returns></returns>
		const uint64_t SizeEx() const noexcept
		{
			// Seek End of file
			if (_fseeki64(this->get(), 0, SEEK_END))
				return 0ULL;

			// Read pointer location, because pointer is at the end, results into file size
			const auto size = gsl::narrow_cast<uint64_t>(_ftelli64(this->get()));

			// Get back to file beginning
			if (_fseeki64(this->get(), 0, SEEK_SET))
				return 0ULL;

			return size;
		}
	};

	struct dcxFileHandleResource final
		: dcxResource < dcxHandle_t >
	{
		dcxFileHandleResource() = delete;											// no default!
		dcxFileHandleResource(const dcxFileHandleResource&) = delete;				// no copy!
		dcxFileHandleResource& operator =(const dcxFileHandleResource&) = delete;	// No assignments!
		dcxFileHandleResource(dcxFileHandleResource&&) = delete;				// no copy!
		dcxFileHandleResource& operator =(dcxFileHandleResource&&) = delete;	// No assignments!

		// calls CreateFile()
		dcxFileHandleResource(const TCHAR* file, DWORD dAccess, DWORD dShareMode, LPSECURITY_ATTRIBUTES lpSecurity, DWORD dCreation, DWORD dflags, HANDLE templateFile)
			: dcxResource(make_filehandle(file, dAccess, dShareMode, lpSecurity, dCreation, dflags, templateFile))
		{
		}

		/// <summary>
		/// Get the file size (32bit)
		/// </summary>
		/// <returns></returns>
		const auto Size() const noexcept
		{
			return GetFileSize(this->get(), nullptr);
		}

		/// <summary>
		/// Get the file size (64bit)
		/// </summary>
		/// <returns></returns>
		const LARGE_INTEGER SizeEx() const noexcept
		{
			LARGE_INTEGER sz{};
			GetFileSizeEx(this->get(), &sz);
			return sz;
		}
	};

	struct dcxStringResource final
		: dcxResource < std::unique_ptr<TCHAR[]> >
	{
		dcxStringResource() = delete;										// no default!
		dcxStringResource(const dcxStringResource&) = delete;				// no copy!
		dcxStringResource& operator =(const dcxStringResource&) = delete;	// No assignments!

		// calls std::make_unique<TCHAR[]>(uSize)
		dcxStringResource(const size_t& uSize)
			: dcxResource(std::make_unique<TCHAR[]>(uSize))
		{}
	};

	struct dcxBSTRResource final
		: dcxResource < dcxBSTR_t >
	{
		dcxBSTRResource() = delete;										// no default!
		dcxBSTRResource(const dcxBSTRResource&) = delete;				// no copy!
		dcxBSTRResource& operator =(const dcxBSTRResource&) = delete;	// No assignments!
		dcxBSTRResource(dcxBSTRResource&&) = delete;
		dcxBSTRResource& operator =(dcxBSTRResource&&) = delete;

		dcxBSTRResource(const WCHAR* const wstr)
			: dcxResource(make_bstr(wstr))
		{}
	};

	struct dcxCursorResource final
		: dcxResource < dcxCursor_t >
	{
		dcxCursorResource() = delete;										// no default!
		dcxCursorResource(const dcxCursorResource&) = delete;				// no copy!
		dcxCursorResource& operator =(const dcxCursorResource&) = delete;	// No assignments!
		dcxCursorResource(dcxCursorResource&&) = delete;
		dcxCursorResource& operator =(dcxCursorResource&&) = delete;

		// calls dcxLoadCursorFromFile()
		explicit dcxCursorResource(const TString& tsFilename)
			: dcxResource(make_cursor(tsFilename))
		{
		}

		// calls dcxLoadCursorFromResource()
		explicit dcxCursorResource(const PTCHAR CursorType)
			: dcxResource(make_resource(dcxLoadCursorFromResource, [](HCURSOR hCursor) noexcept { if (hCursor != nullptr) DestroyCursor(hCursor); }, CursorType))
		{
		}
	};

	struct dcxIconResource final
		: dcxResource < dcxIcon_t >
	{
		dcxIconResource() = delete;										// no default!
		dcxIconResource(const dcxIconResource&) = delete;				// no copy!
		dcxIconResource& operator =(const dcxIconResource&) = delete;	// No assignments!
		dcxIconResource(dcxIconResource&&) = delete;
		dcxIconResource& operator =(dcxIconResource&&) = delete;

		// calls dcxLoadIcon()
		dcxIconResource(const int index, TString& filename, const bool large, const TString& flags)
			: dcxResource(make_icon(index, filename, large, flags))
		{
		}

		// calls ExtractIconEx()
		dcxIconResource(const TString& filename, const int fIndex, const bool bLarge)
			: dcxResource(make_resource([](const TString& filename, const int fIndex, const bool bLarge) noexcept
				{
					HICON m_hIcon = nullptr;
					if (bLarge)
						ExtractIconEx(filename.to_chr(), fIndex, &m_hIcon, nullptr, 1);
					else
						ExtractIconEx(filename.to_chr(), fIndex, nullptr, &m_hIcon, 1);
					return m_hIcon;
				}, [](HICON hIcon) noexcept { if (hIcon != nullptr) DestroyIcon(hIcon); }, filename, fIndex, bLarge))
		{
		}
	};

	struct dcxHDCResource
		: dcxResource < dcxHDC_t >
	{
		dcxHDCResource() = delete;										// no default!
		dcxHDCResource(const dcxHDCResource&) = delete;				// no copy!
		dcxHDCResource& operator =(const dcxHDCResource&) = delete;	// No assignments!
		dcxHDCResource(dcxHDCResource&&) = delete;
		dcxHDCResource& operator =(dcxHDCResource&&) = delete;

		// calls CreateCompatibleDC()
		explicit dcxHDCResource(HDC hdc)
			: dcxResource(make_hdc(hdc))
		{
		}

		// calls CreateDC()
		dcxHDCResource(HDC hdc, LPCTSTR lpszDriver, LPCTSTR lpszDevice, LPCTSTR lpszOutput, const DEVMODE* lpInitData)
			: dcxResource(make_resource(CreateDC, [](HDC obj) noexcept { DeleteDC(obj); }, lpszDriver, lpszDevice, lpszOutput, lpInitData))
		{
		}

		//// calls GetDC() - broken
		//explicit dcxHDCResource(HWND hWnd)
		//	: dcxResource(make_resource(GetDC, [hWnd](HDC obj) { ReleaseDC(hWnd, obj); }, hWnd))
		//{
		//}
		//// calls GetDCEx() - broken
		//dcxHDCResource(HWND hWnd, HRGN hrgnClip, DWORD flags)
		//	: dcxResource(make_resource(GetDCEx, [hWnd](HDC obj) { ReleaseDC(hWnd, obj); }, hWnd, hrgnClip, flags))
		//{
		//}
		//// calls GetWindowDC() - broken
		//explicit dcxHDCResource(HWND hWnd)
		//	: dcxResource(make_resource(GetWindowDC, [](HDC obj) { ReleaseDC(nullptr, obj); }, hWnd))
		//{
		//}
	};

	// Wraps the HDC resource & auto handles cleanup etc..
	// calls CreateCompatableDC() & then SelectBitmap()'s the supplied bitmap into it
	// oldbitmap is saved & selected back into the hdc on destruction.
	struct dcxHDCBitmapResource final
		: dcxHDCResource
	{
		dcxHDCBitmapResource() = delete;											// no default!
		dcxHDCBitmapResource(const dcxHDCBitmapResource&) = delete;				// no copy!
		dcxHDCBitmapResource& operator =(const dcxHDCBitmapResource&) = delete;	// No assignments!
		dcxHDCBitmapResource(dcxHDCBitmapResource&&) = delete;				// no move!
		dcxHDCBitmapResource& operator =(dcxHDCBitmapResource&&) = delete;	// No assignments!

		//calls CreateCompatibleDC() then SelectBitmap()
		dcxHDCBitmapResource(HDC hdc, HBITMAP hBitmap)
			: dcxHDCResource(hdc)
		{
			if (hBitmap)
				m_hOldBitmap = SelectBitmap(this->get(), hBitmap);
		}

		~dcxHDCBitmapResource()
		{
			if (m_hOldBitmap)
				SelectBitmap(this->get(), m_hOldBitmap);
		}
	private:
		HBITMAP	m_hOldBitmap{ nullptr };
	};

	struct dcxBitmapResource final
		: dcxResource < dcxBitmap_t >
	{
		dcxBitmapResource() = delete;										// no default!
		dcxBitmapResource(const dcxBitmapResource&) = delete;				// no copy!
		dcxBitmapResource& operator =(const dcxBitmapResource&) = delete;	// No assignments!
		dcxBitmapResource(dcxBitmapResource&&) = delete;
		dcxBitmapResource& operator =(dcxBitmapResource&&) = delete;

		// calls CreateCompatibleBitmap()
		dcxBitmapResource(HDC hdc, int w, int h)
			: dcxResource(make_bitmap(hdc, w, h))
		{
		}

		//calls CreateDIBitmap();
		dcxBitmapResource(HDC hdc, const BITMAPINFOHEADER* pbmih, DWORD flInit, const void* pjBits, const BITMAPINFO* pbmi, UINT iUsage)
			: dcxResource(make_resource(CreateDIBitmap, [](HBITMAP obj) noexcept { DeleteBitmap(obj); }, hdc, pbmih, flInit, pjBits, pbmi, iUsage))
		{
		}

		//calls CreateDIBSection();
		dcxBitmapResource(HDC hdc, const BITMAPINFO* pbmi, UINT usage, void** ppvBits, HANDLE hSection, DWORD offset)
			: dcxResource(make_resource(CreateDIBSection, [](HBITMAP obj) noexcept { DeleteBitmap(obj); }, hdc, pbmi, usage, ppvBits, hSection, offset))
		{
		}
	};

	struct dcxHDCBitmap2Resource
		: dcxHDCResource
	{
		dcxHDCBitmap2Resource() = delete;											// no default!
		dcxHDCBitmap2Resource(const dcxHDCBitmap2Resource&) = delete;				// no copy!
		dcxHDCBitmap2Resource& operator =(const dcxHDCBitmap2Resource&) = delete;	// No assignments!
		dcxHDCBitmap2Resource(dcxHDCBitmap2Resource&&) = delete;
		dcxHDCBitmap2Resource& operator =(dcxHDCBitmap2Resource&&) = delete;

		//calls CreateCompatibleDC() then CreateCompatibleBitmap(), then SelectBitmap()
		dcxHDCBitmap2Resource(HDC hdc, const int& w, const int& h)
			: dcxHDCResource(hdc)
			, m_hBitmap(hdc, w, h)
			, m_Width(w)
			, m_Height(h)
		{
			m_hOldBitmap = SelectBitmap(this->get(), m_hBitmap.get());
		}

		~dcxHDCBitmap2Resource()
		{
			if (m_hOldBitmap)
				SelectBitmap(this->get(), m_hOldBitmap);
		}

		/// <summary>
		/// Get the bitmaps width.
		/// </summary>
		/// <returns></returns>
		const int& getWidth() const noexcept { return m_Width; }

		/// <summary>
		/// Get the bitmaps height.
		/// </summary>
		/// <returns></returns>
		const int& getHeight() const noexcept { return m_Height; }

		/// <summary>
		/// Get a pointer to the bitmap.
		/// </summary>
		/// <returns></returns>
		const HBITMAP getBitMap() const noexcept { return m_hBitmap.get(); }
	private:
		HBITMAP	m_hOldBitmap;
		dcxBitmapResource m_hBitmap;
		int m_Width, m_Height;
	};

	//struct dcxCursor
	//{
	//	dcxCursor() = delete;
	//	dcxCursor(const dcxCursor &) = delete;
	//	dcxCursor &operator =(const dcxCursor &) = delete;	// No assignments!
	//
	//	dcxCursor(const TString &tsFilename)
	//		: m_uni(make_cursor(tsFilename))
	//	{
	//	}
	//
	//	operator HCURSOR() { return m_uni.get(); }
	//	HCURSOR release() noexcept { return m_uni.release(); }
	//private:
	//	dcxCursor_t	m_uni;
	//};
	//
	//struct dcxIcon
	//{
	//	dcxIcon() = delete;								// no default
	//	dcxIcon(const dcxIcon &) = delete;				// no copy!
	//	dcxIcon &operator =(const dcxIcon &) = delete;	// No assignments!
	//
	//	dcxIcon(const int index, TString &filename, const bool large, const TString &flags)
	//		: m_uni(make_icon(index, filename, large, flags))
	//	{
	//	}
	//	operator HICON() { return m_uni.get(); }
	//
	//private:
	//	dcxIcon_t	m_uni;
	//};
	//
	//struct dcxFile
	//{
	//	dcxFile() = delete;
	//	dcxFile(const dcxFile &) = delete;
	//	dcxFile &operator =(const dcxFile &) = delete;	// No assignments!
	//
	//	dcxFile(const TCHAR *tsFilename, const TCHAR *tsMode)
	//		: m_uni(make_file(tsFilename, tsMode))
	//	{
	//	}
	//	operator FILE *() { return m_uni.get(); }
	//
	//private:
	//	dcxFile_t	m_uni;
	//};
	//
	//struct dcxHDC
	//{
	//	dcxHDC() = delete;
	//	dcxHDC(const dcxHDC &) = delete;
	//	dcxHDC &operator =(const dcxHDC &) = delete;	// No assignments!
	//
	//	dcxHDC(HDC hdc)
	//		: m_uni(make_hdc(hdc))
	//	{
	//	}
	//	//dcxHDC(HDC hdc)
	//	//	: m_uni(make_hdc(hdcTypes::CreateCompatiableDCType, hdc, nullptr, nullptr, nullptr, nullptr))
	//	//{
	//	//}
	//	//dcxHDC(LPCTSTR lpszDriver, LPCTSTR lpszDevice, LPCTSTR lpszOutput, const DEVMODE *lpInitData)
	//	//	: m_uni(make_hdc(hdcTypes::CreateDCType, nullptr, lpszDriver, lpszDevice, lpszOutput, lpInitData))
	//	//{
	//	//}
	//	operator HDC() { return m_uni.get(); }
	//
	//private:
	//	dcxHDC_t	m_uni;
	//};
	//
	//struct dcxBitmap
	//{
	//	dcxBitmap() = delete;
	//	dcxBitmap(const dcxBitmap &) = delete;
	//	dcxBitmap &operator =(const dcxBitmap &) = delete;	// No assignments!
	//
	//	dcxBitmap(HDC hdc, int x, int y)
	//		: m_uni(make_bitmap(hdc,x,y))
	//	{
	//	}
	//	operator HBITMAP() { return m_uni.get(); }
	//
	//private:
	//	dcxBitmap_t	m_uni;
	//};
	//
	//struct dcxExtractIcon {
	//	dcxExtractIcon() = delete;
	//	dcxExtractIcon(const dcxExtractIcon &) = delete;
	//	dcxExtractIcon &operator =(const dcxExtractIcon &) = delete;
	//
	//	dcxExtractIcon(const TString &filename, const int iIndex, const bool bLarge)
	//		: m_hIcon(nullptr)
	//	{
	//		if (bLarge)
	//			ExtractIconEx(filename.to_chr(), iIndex, &m_hIcon, nullptr, 1);
	//		else
	//			ExtractIconEx(filename.to_chr(), iIndex, nullptr, &m_hIcon, 1);
//
	//	}
	//	~dcxExtractIcon()
	//	{
	//		if (m_hIcon != nullptr)
	//			DestroyIcon(m_hIcon);
	//	}
	//	operator HICON() const noexcept { return m_hIcon; }
	//	explicit operator bool() const noexcept { return (m_hIcon != nullptr); }
	//	HICON release() noexcept { HICON hTmp = m_hIcon; m_hIcon = nullptr; return hTmp; }
	//private:
	//	HICON m_hIcon;
	//};

	//struct dcxHDCBuffer : dcxHDCResource {
	//	dcxHDCBuffer() = delete;
	//	dcxHDCBuffer(const dcxHDCBuffer &) = delete;
	//	dcxHDCBuffer &operator = (const dcxHDCBuffer &) = delete;
//
	//	dcxHDCBuffer(const HDC hdc, RECT *rc)
	//		: dcxHDCResource(hdc)
	//		, m_hBitmap(nullptr)
	//		, m_hOldBitmap(nullptr)
	//		, m_hOldFont(nullptr)
	//	{
	//		// get size of bitmap to alloc.
	//		int x = 0, y = 0, w = 0, h = 0;
//
	//		if (rc == nullptr) {
	//			// no size specified, use hdc's bitmap size.
	//			BITMAP bm = { 0 };
	//			if (GetObject((HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP), sizeof(BITMAP), &bm) == 0)
	//				throw Dcx::dcxException("dcxHDCBuffer - Unable to get hdc's bitmap");
	//			w = bm.bmWidth;
	//			h = bm.bmHeight;
	//		}
	//		else {
	//			// use size specified.
	//			w = (rc->right - rc->left);
	//			h = (rc->bottom - rc->top);
	//			x = rc->left;
	//			y = rc->top;
	//		}
//
	//		// alloc bitmap for buffer.
	//		m_hBitmap = CreateCompatibleBitmap(hdc, w, h);
//
	//		if (m_hBitmap == nullptr)
	//			throw Dcx::dcxException("dcxHDCBuffer - Unable to create bitmap");
	//		
	//			// select bitmap into hdc
	//		m_hOldBitmap = SelectBitmap(*this, m_hBitmap);
//
	//		// copy settings from hdc to buffer's hdc.
	//		SetDCBrushColor(*this, GetDCBrushColor(hdc));
	//		SetDCPenColor(*this, GetDCPenColor(hdc));
	//		SetLayout(*this, GetLayout(hdc));
	//		m_hOldFont = SelectFont(*this, GetCurrentObject(hdc, OBJ_FONT));
	//		SetTextColor(*this, GetTextColor(hdc));
	//		SetBkColor(*this, GetBkColor(hdc));
	//		SetBkMode(*this, GetBkMode(hdc));
	//		SetROP2(*this, GetROP2(hdc));
	//		SetMapMode(*this, GetMapMode(hdc));
	//		SetPolyFillMode(*this, GetPolyFillMode(hdc));
	//		SetStretchBltMode(*this, GetStretchBltMode(hdc));
	//		SetGraphicsMode(*this, GetGraphicsMode(hdc));
//
	//		// copy contents of hdc within area to buffer.
	//		BitBlt(*this, 0, 0, w, h, hdc, x, y, SRCCOPY);
//
	//		// buffer is an exact duplicate of the hdc within the area specified.
	//	}
	//	~dcxHDCBuffer()
	//	{
	//		GdiFlush();
	//		if (*this != nullptr)
	//		{
	//			if (m_hOldFont != nullptr)
	//				SelectFont(*this, m_hOldFont);
	//			if (m_hOldBitmap != nullptr)
	//				SelectBitmap(*this, m_hOldBitmap);
	//		}
	//		if (m_hBitmap != nullptr)
	//			DeleteBitmap(m_hBitmap);
	//	}
	//private:
	//	HBITMAP m_hOldBitmap;
	//	HBITMAP m_hBitmap;
	//	HFONT m_hOldFont;
	//};

	struct dcxHDCBuffer final
		: dcxHDCBitmap2Resource
	{
		dcxHDCBuffer() = delete;
		dcxHDCBuffer(const dcxHDCBuffer&) = delete;
		dcxHDCBuffer& operator = (const dcxHDCBuffer&) = delete;
		dcxHDCBuffer(dcxHDCBuffer&&) = delete;
		dcxHDCBuffer& operator = (dcxHDCBuffer&&) = delete;

		dcxHDCBuffer(const HDC hdc, const RECT& rc)
			: dcxHDCBitmap2Resource(hdc, (rc.right - rc.left), (rc.bottom - rc.top))
		{
			// copy settings from hdc to buffer's hdc.
			SetDCBrushColor(this->get(), GetDCBrushColor(hdc));
			SetDCPenColor(this->get(), GetDCPenColor(hdc));
			SetLayout(this->get(), GetLayout(hdc));
			//m_hOldFont = SelectFont(this->get(), GetCurrentObject(hdc, OBJ_FONT));
			m_hOldFont = static_cast<HFONT>(SelectObject(this->get(), GetCurrentObject(hdc, OBJ_FONT)));
			SetTextColor(this->get(), GetTextColor(hdc));
			SetBkColor(this->get(), GetBkColor(hdc));
			SetBkMode(this->get(), GetBkMode(hdc));
			SetROP2(this->get(), GetROP2(hdc));
			SetMapMode(this->get(), GetMapMode(hdc));
			SetPolyFillMode(this->get(), GetPolyFillMode(hdc));
			SetStretchBltMode(this->get(), GetStretchBltMode(hdc));
			SetGraphicsMode(this->get(), GetGraphicsMode(hdc));

			//SetBrushOrgEx(this->get(), );
			//GetBrushOrgEx
			//SetWindowOrgEx
			//GetWindowOrgEx

			// copy contents of hdc within area to buffer.
			GSL_SUPPRESS(type.4) BitBlt(this->get(), 0, 0, getWidth(), getHeight(), hdc, rc.left, rc.top, SRCCOPY);

			// buffer is an exact duplicate of the hdc within the area specified.
		}

		~dcxHDCBuffer()
		{
			GdiFlush();
			if (m_hOldFont)
				//SelectFont(this->get(), m_hOldFont);
				SelectObject(this->get(), m_hOldFont);
		}
	private:
		HFONT m_hOldFont{ nullptr };
	};

	struct dcxVariant final
		: VARIANT
	{
		dcxVariant() noexcept
		{
			VariantInit(this);
		}

		explicit dcxVariant(const BSTR bStr) noexcept
			: dcxVariant()
		{
			vt = VT_BSTR;
			bstrVal = bStr;
		}

		explicit dcxVariant(const WCHAR* cStr) noexcept
			: dcxVariant(SysAllocString(cStr))
		{
		}

		~dcxVariant() noexcept
		{
			VariantClear(this);	// this does SysFreeString() for us.
		}

		dcxVariant(const dcxVariant&) noexcept = default;
		dcxVariant& operator = (const dcxVariant&) noexcept = default;
		dcxVariant(dcxVariant&&) noexcept = default;
		dcxVariant& operator = (dcxVariant&&) noexcept = default;
	};

	struct dcxCursorPos final
		: POINT
	{
		/// <summary>
		/// Get cursor position on screen.
		/// sets cursor position to -1,-1 on error.
		/// </summary>
		/// <returns></returns>
		dcxCursorPos() noexcept
		{
			if (GetCursorPos(this) == FALSE)
				x = y = -1;
		}

		/// <summary>
		/// Get cursor position mapped to this window.
		/// sets cursor position to -1,-1 on error. (check GetLastError() for more info)
		/// </summary>
		/// <param name="hwnd">- Window to map cusrsor position too.</param>
		/// <returns></returns>
		explicit dcxCursorPos(HWND hwnd) noexcept
			: dcxCursorPos()
		{
			SetLastError(0);
			if ((MapWindowPoints(nullptr, hwnd, this, 1) == 0) && (GetLastError() != 0))
				x = y = -1;
		}

		~dcxCursorPos() noexcept = default;

		explicit operator bool() const noexcept { return ((x != -1) && (y != -1)); };
	};

	struct dcxRect
		: RECT
	{
		/// <summary>
		/// Get the rects width.
		/// </summary>
		/// <returns></returns>
		long Width() const noexcept { return (right - left); }

		/// <summary>
		/// Get the rects height.
		/// </summary>
		/// <returns></returns>
		long Height() const noexcept { return (bottom - top); }

		/// <summary>
		/// Get a copy of the rect.
		/// </summary>
		/// <returns></returns>
		RECT CopyRect() const noexcept { return { left, top, right, bottom }; }
	};

	struct dcxWindowRect final
		: dcxRect
	{
		dcxWindowRect() = delete;

		// Gets the window rect for hwnd
		explicit dcxWindowRect(HWND hwnd) noexcept
		{
			if (!hwnd) m_ok = false;
			else m_ok = (GetWindowRect(hwnd, this) != FALSE);
		}

		// Gets the window rect for hwnd & maps it to hMap
		dcxWindowRect(HWND hwnd, HWND hMap) noexcept
			: dcxWindowRect(hwnd)
		{
			if (m_ok)
			{
				SetLastError(0U);
				MapWindowRect(nullptr, hMap, this);
				m_ok = (GetLastError() == 0U);
			}
		}

		~dcxWindowRect() noexcept = default;

		explicit operator bool() noexcept { return m_ok; }
		bool	m_ok{ false };
	};

	struct dcxClientRect final
		: dcxRect
	{
		dcxClientRect() = delete;

		// Gets the window rect for hwnd
		explicit dcxClientRect(HWND hwnd) noexcept
		{
			if (!hwnd) m_ok = false;
			else m_ok = (GetClientRect(hwnd, this) != FALSE);
		}

		// Gets the window client rect for hwnd & maps it to hMap
		dcxClientRect(HWND hwnd, HWND hMap) noexcept
			: dcxClientRect(hwnd)
		{
			if (m_ok)
			{
				SetLastError(0U);
				MapWindowRect(hwnd, hMap, this);
				m_ok = (GetLastError() == 0U);
			}
		}

		~dcxClientRect() noexcept = default;

		explicit operator bool() noexcept { return m_ok; }
		bool	m_ok{ false };
	};

	struct dcxClassName final
		: stString<257>
	{
		dcxClassName() = delete;

		// Gets the class name for hwnd
		explicit dcxClassName(HWND hwnd) noexcept
		{
			if (hwnd)
				GetClassName(hwnd, this->data(), gsl::narrow_cast<int32_t>(this->size()));
		}

		~dcxClassName() = default;
	};

	struct dcxPalette final
	{
		dcxPalette(bool bForce = false) noexcept
		{
			try {
				getmIRCPalette(gsl::span<COLORREF>(m_Palette), bForce); // get mIRC palette
			}
			catch (...)
			{
			}
		}

		/// <summary>
		/// Get a pointer to the palette.
		/// </summary>
		/// <returns></returns>
		explicit operator COLORREF* () noexcept
		{
			return &m_Palette[0];
		}

		/// <summary>
		/// Get the size of the palette.
		/// </summary>
		/// <returns></returns>
		constexpr const size_t& size() const noexcept { return m_size; }

		struct iter
		{
			COLORREF operator * () const noexcept { if (!itr_Palette) return CLR_INVALID; return itr_Palette[n]; }
			iter& operator ++() noexcept { ++n; return *this; }
			friend
				bool operator != (iter const& lhs, iter const& rhs) noexcept
			{
				return lhs.n != rhs.n;
			}

			size_t	n;
			const COLORREF* const itr_Palette;
		};

		iter begin() const noexcept { return{ 0, &m_Palette[0] }; }
		iter end() const noexcept { return{ size(), nullptr }; }

		constexpr static const size_t m_size = mIRC_PALETTE_SIZE;
		COLORREF m_Palette[m_size]{ CLR_INVALID };
	};

	struct dcxTreeItem final
	{
		dcxTreeItem(HWND hWin, HTREEITEM hParent) noexcept
			: m_Window{ hWin }
			, m_Item{ TreeView_GetChild(hWin, hParent) }
		{
		}
		GSL_SUPPRESS(es.47)
			explicit dcxTreeItem(HWND hWin) noexcept
			: m_Window{ hWin }
			, m_Item{ TreeView_GetRoot(hWin) }
		{
		}

		explicit operator HTREEITEM () noexcept
		{
			return m_Item;
		}

		struct iter
		{
			HTREEITEM operator * () const noexcept { return itr_Item; }
			iter& operator ++() noexcept
			{
				itr_Item = TreeView_GetNextSibling(itr_Window, itr_Item);
				return *this;
			}
			friend
				bool operator != (iter const& lhs, iter const& rhs) noexcept
			{
				return lhs.itr_Item != rhs.itr_Item;
			}

			HWND		itr_Window{};
			HTREEITEM	itr_Item{};
		};

		GSL_SUPPRESS(lifetime.4) iter begin() const noexcept { return{ m_Window, TreeView_GetChild(m_Window, m_Item) }; }
		GSL_SUPPRESS(lifetime.4) iter end() const noexcept { return{ m_Window, nullptr }; }

		HWND		m_Window{};
		HTREEITEM	m_Item{};
	};

	using MapOfCursors = std::map<HCURSOR, HCURSOR>;
	using MapOfAreas = std::map<UINT, HCURSOR>;

	template< typename T >
	struct range_t
	{
		struct iter
		{
			T operator * () const noexcept { return n; }
			iter& operator ++() noexcept { ++n; return *this; }
			friend
				bool operator != (iter const& lhs, iter const& rhs) noexcept
			{
				return lhs.n != rhs.n;
			}
			friend
				bool operator <= (iter const& lhs, iter const& rhs) noexcept
			{
				return lhs.n <= rhs.n;
			}

			T n;
		};
		struct reverse_iter
		{
			T operator * () const noexcept { return n; }
			reverse_iter& operator ++() noexcept { --n; return *this; }
			friend
				bool operator != (reverse_iter const& lhs, reverse_iter const& rhs) noexcept
			{
				return lhs.n != rhs.n;
			}
			friend
				bool operator <= (reverse_iter const& lhs, reverse_iter const& rhs) noexcept
			{
				return lhs.n >= rhs.n;
			}

			T n;
		};

		iter begin() const noexcept { return{ b }; }
		iter end() const noexcept { return{ e + 1 }; }
		reverse_iter rbegin() const noexcept { return{ e }; }
		reverse_iter rend() const noexcept { return{ b }; }

		T length() const noexcept { return (e - b) + 1; }
		bool inRange(T f) const noexcept
		{
			return ((b <= f) && (e >= f));
		}

		T b, e;
	};
	template< DcxConcepts::IsNumeric T > range_t<T>  make_range(T b, T e) noexcept { return{ b, e }; }

	template< DcxConcepts::IsNumeric T > range_t<T>  make_range(const TString& tsItems, T nItemCnt, T nAdjust)
	{
		T iStart{}, iEnd{};
		if (tsItems.numtok(TEXT('-')) == 2)
		{
			iStart = tsItems.getfirsttok(1, TEXT('-')).to_<T>() - nAdjust;
			iEnd = tsItems.getnexttok(TEXT('-')).to_<T>() - nAdjust;

			if (iEnd == -1)	// special case
				iEnd = nItemCnt - nAdjust;
		}
		else {
			iEnd = tsItems.to_<T>() - nAdjust;

			if (iEnd == -1)	// special case
				iStart = iEnd = nItemCnt - nAdjust;
			else
				iStart = iEnd;
		}
		return{ std::min(iStart, nItemCnt - nAdjust), std::min(iEnd, nItemCnt - nAdjust) };
	}
	template< DcxConcepts::IsNumeric T > range_t<T>  make_range(const TString& tsItems, T nItemCnt)
	{
		T iStart{}, iEnd{};
		if (tsItems.numtok(TEXT('-')) == 2)
		{
			iStart = tsItems.getfirsttok(1, TEXT('-')).to_<T>();
			iEnd = tsItems.getnexttok(TEXT('-')).to_<T>();

			if (iEnd == -1)	// special case
				iEnd = nItemCnt;
		}
		else {
			iEnd = tsItems.to_<T>();

			if (iEnd == -1)	// special case
				iStart = iEnd = nItemCnt;
			else
				iStart = iEnd;
		}
		return{ std::min(iStart, nItemCnt), std::min(iEnd, nItemCnt) };
	}
	template< DcxConcepts::IsNumeric T > range_t<T> adjust_range(const range_t<T>& r, int nAdjustment) noexcept
	{
		return { r.b + nAdjustment, r.e + nAdjustment };
	}

	// not for use in code, just for testing
	template <DcxConcepts::IsNumeric T>
	struct dcxNumber final
	{
		constexpr operator std::make_unsigned_t<T>() const noexcept { return gsl::narrow_cast<std::make_unsigned_t<T>>(m_nValue); }
		constexpr operator std::make_signed_t<T>() const noexcept { return gsl::narrow_cast<std::make_signed_t<T>>(m_nValue); }
		constexpr explicit operator bool() const noexcept { return (m_nValue != T()); }
		T& operator =(const std::make_unsigned_t<T>& other) noexcept
		{
			m_nValue = T(other);
			return m_nValue;
		}
		T& operator =(const std::make_signed_t<T>& other) noexcept
		{
			m_nValue = T(other);
			return m_nValue;
		}
		bool operator ==(const std::make_unsigned_t<T>& other) const noexcept { return m_nValue == T(other); }
		bool operator ==(const std::make_signed_t<T>& other) const noexcept { return m_nValue == T(other); }

		T m_nValue;
	};

	void setupOSCompatibility(void);
	void freeOSCompatibility(void) noexcept;

	IClassFactory* const getClassFactory() noexcept;
	const TCHAR* getLastError() noexcept;
	const std::byte& getGhostDrag() noexcept;
	bool setGhostDrag(const std::byte newAlpha) noexcept;
	const bool& isDX9Installed() noexcept;
	bool isUnloadSafe() noexcept;

	void load(mIRCLinker::LOADINFO* const lInfo);
	void unload(void);
	const bool& initDirectX();
	const bool& initDirectX(TCHAR* dxResult, int dxSize);
	void error(const TCHAR* const cmd, const TCHAR* const msg) noexcept;
	template <typename Format, typename Value, typename... Arguments>
	void error(const TCHAR* const cmd, _Printf_format_string_ const Format& fmt, const Value& val, Arguments&&... args)
	{
		TString tsErr;
		error(cmd, _ts_sprintf(tsErr, fmt, val, args...).to_chr());
	}

	void mark(const TString& tsDName, const TString& tsCallbackName);
	LRESULT CALLBACK mIRCSubClassWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	GSL_SUPPRESS(lifetimes.1) const TCHAR* const parseCursorType(const TString& cursor);
	GSL_SUPPRESS(lifetimes.1) const TCHAR* const parseCursorType(const std::hash<TString>::result_type& cursor);
	const DWORD parseSystemCursorType(const TString& cursor);
	const DWORD parseSystemCursorType(const std::hash<TString>::result_type& cursor);
	const DWORD parseAreaType(const TString& tsArea);
	const DWORD parseAreaType(const std::hash<TString>::result_type& tsArea);
	HCURSOR dcxLoadCursor(const DcxResourceFlags iFlags, const TCHAR* CursorType, bool& bCursorFromFile, const HCURSOR oldCursor, TString& filename);
	void setAreaCursor(const HCURSOR hCursor, const UINT iType);
	void deleteAreaCursor(const UINT iType);
	HCURSOR SystemToCustomCursor(const HCURSOR hCursor);
	HCURSOR AreaToCustomCursor(const UINT iType);
	void setCursor(const HCURSOR hSystemCursor, const HCURSOR hCustomCursor);
	void deleteCursor(const HCURSOR hCursor);
	HCURSOR WINAPI XSetCursor(HCURSOR hCursor);
	PVOID PatchAPI(const char* const c_szDllName, const char* const c_szApiName, PVOID newfPtr) noexcept;
	void RemovePatch(PVOID fPtr, PVOID newfPtr) noexcept;
	void FillRectColour(HDC hdc, LPCRECT prc, COLORREF clr) noexcept;

	//typedef HCURSOR(WINAPI *PFNSETCURSOR)(HCURSOR hCursor);
	using PFNSETCURSOR = HCURSOR(WINAPI*)(HCURSOR hCursor);

	extern PFNSETCURSOR SetCursorUx;

	// parse_string, taken from somewhere on stackoverflow (sorry can't remember where) & modified for our use.
	template <DcxConcepts::IsNumeric RETURN_TYPE, typename STRING_TYPE>
	RETURN_TYPE parse_string(const STRING_TYPE* str)
	{
		static_assert(DcxConcepts::IsPODText<STRING_TYPE>, "Only WCHAR, CHAR, or TCHAR type supported!");

		std::basic_stringstream<STRING_TYPE> buf;
		buf << str;
		RETURN_TYPE val{};
		buf >> val;
		return val;

		//std::from_chars();
	}

	namespace details
	{
		//	// Casts any numeric type as another numeric type.
		//	template < typename tResult, typename tInput >
		//	std::enable_if_t<is_Numeric_v<tInput>, inline constexpr tResult> numeric_cast(tInput &in) noexcept
		//	{
		//		static_assert(is_Numeric_v<tResult>, "A Numeric return type is required");
		//		return gsl::narrow_cast<tResult>(in);
		//	}
	//
		//	// Casts any string as a number
		//	template < typename tResult, typename tInput >
		//	std::enable_if_t<!is_Numeric_v<tInput> && !std::is_convertible_v<tInput, tResult>, tResult> numeric_cast(tInput &in)
		//	{
		//		static_assert(is_Numeric_v<tResult>, "A Numeric return type is required");
		//		std::stringstream buf;
		//		buf << in;
		//		tResult result;
		//		buf >> result;
		//		return result;
		//	}
	//
		//	// Converts any object to a numeric using implicit-conversion.
		//	template < typename tResult, typename tInput >
		//	std::enable_if_t<!is_Numeric_v<tInput> && std::is_convertible_v<tInput, tResult>, inline constexpr tResult> numeric_cast(tInput &in)
		//	{
		//		static_assert(is_Numeric_v<tResult>, "A Numeric return type is required");
		//		return in;
		//	}
		//}
	//
		//// Converts any object to a numeric.
		////template < typename tResult, typename tInput >
		////inline constexpr tResult numeric_cast(tInput &in)
		////{
		////	static_assert(is_Numeric_v<tResult>, "A Numeric return type is required");
		////	return details::numeric_cast<tResult>(in);
		////}
	//
		//template < typename tResult, typename tInput >
		//inline constexpr tResult numeric_cast(tInput &&in)
		//{
		//	static_assert(is_Numeric_v<tResult>, "A Numeric return type is required");
		//	return details::numeric_cast<tResult>(std::forward<tInput>(in));
		//}
	//
		//// Converts any pointer to a numeric.
		//template < typename tResult, typename tInput, typename = std::enable_if_t<!std::is_same_v<std::remove_cv_t<tInput>,char> && !std::is_same_v<std::remove_cv_t<tInput>,wchar_t> > >
		//inline constexpr tResult numeric_cast(tInput *in)
		//{
		//	static_assert(is_Numeric_v<tResult>, "A Numeric return type is required");
		//	return reinterpret_cast<tResult>(in);
		//}
	//
		////// Converts any calculation to a numeric.
		////template < typename tResult, typename tInput, typename = std::enable_if_t<std::is_arithmetic_v<tInput> > >
		////inline constexpr tResult numeric_cast(tInput in)
		////{
		////	static_assert(is_Numeric_v<tResult>, "A Numeric return type is required");
		////	return gsl::narrow_cast<tResult>(in);
		////}

		template < DcxConcepts::IsNumeric tResult, typename tInput >
		inline constexpr tResult numeric_cast(tInput& in) noexcept
		{
			static_assert(is_Numeric_v<tResult>, "A Numeric return type is required");
			if constexpr (is_Numeric_v<tInput>)
			{
				// Casts any numeric type as another numeric type.
				//static_assert(in == gsl::narrow_cast<tInput>(gsl::narrow_cast<tResult>(in)), "A Numeric return type is required");
				return gsl::narrow_cast<tResult>(in);
			}
			else if constexpr (!std::is_convertible_v<tInput, tResult>)
			{
				// Casts any string as a number
				std::stringstream buf;
				if constexpr (std::is_array_v<tInput>)
					buf << &in[0];
				else
					buf << in;
				tResult result{};
				buf >> result;
				return result;
			}
			else {
				// Converts any object to a numeric using implicit-conversion.
				return in;
			}
		}

		//template < typename tResult, typename tInput >
		//requires requires(tResult out, tInput in)
		//{
		//	{is_Numeric_v<tResult>};
		//	{is_Numeric_v<tInput>};
		//	{std::is_convertible_v<tInput, tResult>};
		//}
		//inline constexpr tResult numeric_cast(tInput& in) noexcept
		//{
		//	// Casts any numeric type as another numeric type.
		//	//static_assert(in == gsl::narrow_cast<tInput>(gsl::narrow_cast<tResult>(in)), "A Numeric return type is required");
		//	return gsl::narrow_cast<tResult>(in);
		//}
		//template < typename tResult, typename tInput >
		//requires requires(tResult out, tInput in)
		//{
		//	{is_Numeric_v<tResult>};
		//	{!is_Numeric_v<tInput>};
		//	{!std::is_convertible_v<tInput, tResult>};
		//}
		//inline constexpr tResult numeric_cast(tInput& in) noexcept
		//{
		//	// Casts any string as a number
		//	std::stringstream buf;
		//	if constexpr (std::is_array_v<tInput>)
		//		buf << &in[0];
		//	else
		//		buf << in;
		//	tResult result{};
		//	buf >> result;
		//	return result;
		//}
	}

	// Converts any object to a numeric.
	//template < typename tResult, typename tInput >
	//inline constexpr tResult numeric_cast(tInput &in)
	//{
	//	static_assert(is_Numeric_v<tResult>, "A Numeric return type is required");
	//	return details::numeric_cast<tResult>(in);
	//}

	template < DcxConcepts::IsNumeric tResult, typename tInput >
	inline constexpr tResult numeric_cast(tInput&& in) noexcept
	{
		static_assert(is_Numeric_v<tResult>, "A Numeric return type is required");
		return details::numeric_cast<tResult>(std::forward<tInput>(in));
	}

	// Converts any pointer to a numeric.
	template < DcxConcepts::IsNumeric tResult, typename tInput, typename = std::enable_if_t<!std::is_same_v<std::remove_cv_t<tInput>, char> && !std::is_same_v<std::remove_cv_t<tInput>, wchar_t> > >
	inline constexpr tResult numeric_cast(tInput* in) noexcept
	{
		static_assert(is_Numeric_v<tResult>, "A Numeric return type is required");
		//if constexpr(std::is_same_v<std::remove_cv_t<tInput>,void>)
		//	return static_cast<tResult>(in);
		//else
		return reinterpret_cast<tResult>(in);
	}

	// find() - takes an array object, & something to compare against. (same as std::find(begin(),end(),val) )
	template <typename Res, DcxConcepts::IsContainer obj, typename _Val>
	Res find(obj& SourceObject, _Val& val)
	{
		return std::find(SourceObject.begin(), SourceObject.end(), val);
	}

	// find(obj &SourceObject, _Val &val)
	// - takes an array object, & something to compare against. (same as std::find(begin(),end(),val) != end() )
	// Result
	//	true - If val is found in object otherwise false
	template <DcxConcepts::IsContainer obj, typename _Val>
	bool find(obj& SourceObject, _Val& val) noexcept
	{
		auto itEnd = SourceObject.end();
		return (std::find(SourceObject.begin(), itEnd, val) != itEnd);
	}

	// C++11 Better countof() for arrays, taken from http://www.g-truc.net/post-0708.html
	// replaced by std::size() in code.
	template <typename T, std::size_t N>
	constexpr std::size_t countof(T const (&)[N]) noexcept
	{
		return N;
	}

	// countof() for container objects
	template <DcxConcepts::ImplementsSizeFunction C>
	constexpr std::size_t countof(C const& c) noexcept
	{
		return c.size();
	}

	// Erase an item from a container if its found.
	template <DcxConcepts::IsContainer Cont, typename Val>
	bool eraseIfFound(Cont& con, Val& v) noexcept(std::is_nothrow_move_assignable_v<Cont::value_type>)
	{
		if (const auto itEnd = con.end(), itGot = std::find(con.begin(), itEnd, v); itGot != itEnd)
		{
			con.erase(itGot);
			return true;
		}
		return false;

		//  Ook: is this OK? needs testing...
		//const auto itEnd = con.end();
		//return (con.erase(std::find(con.begin(), itEnd, v)) != itEnd);
	}

	/// <summary>
	/// Test if a file exists.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="filename"></param>
	/// <returns></returns>
	template <class T>
	bool IsFileEx(const T& filename) noexcept
	{
		return (std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename));
	}

	template <class T, class CodeType = int>
	struct CodeValue final
	{
		CodeType	code{};
		T			value{};

		explicit operator bool() const noexcept
		{
			if constexpr (std::is_same_v<CodeType, bool>)
				return code;
			else
				return (code > 0);
		}
	};
	template <class T>
	using BoolValue = CodeValue<T, bool >;

	template <class T>
	auto dcxGetObject(_In_ HANDLE h) noexcept
	{
		CodeValue<T> v;
		v.code = GetObject(h, sizeof(T), &v.value);
		return v;
	}

	template <class T>
	inline T dcxGetStockObject(_In_ int i) noexcept
	{
		return static_cast<T>(GetStockObject(i));
	}

	template <class T>
	inline T dcxGetCurrentObject(_In_ HDC hdc, _In_ UINT type) noexcept
	{
		return static_cast<T>(GetCurrentObject(hdc, type));
	}

	GSL_SUPPRESS(lifetime)
		inline WNDPROC dcxGetWindowProc(_In_ HWND hwnd) noexcept
	{
		return reinterpret_cast<WNDPROC>(GetWindowLongPtr(hwnd, GWLP_WNDPROC));
	}

	template <class T>
	inline T dcxSelectObject(_In_ HDC hdc, _In_ T obj) noexcept
	{
		return static_cast<T>(SelectObject(hdc, obj));
	}

	template <DcxConcepts::IsNumeric T>
	constexpr inline WORD dcxLOWORD(T&& data) noexcept
	{
		return gsl::narrow_cast<WORD>(gsl::narrow_cast<DWORD_PTR>(data) & 0xFFFF);
	}

	template <DcxConcepts::IsNumeric T>
	constexpr inline WORD dcxHIWORD(T&& data) noexcept
	{
		return gsl::narrow_cast<WORD>((gsl::narrow_cast<DWORD_PTR>(data) >> 16) & 0xFFFF);
	}

	template <DcxConcepts::IsNumeric T>
	constexpr inline LPARAM dcxMAKELPARAM(T&& l, T&& h) noexcept
	{
		//return gsl::narrow_cast<LPARAM>(gsl::narrow_cast<WORD>(gsl::narrow_cast<DWORD_PTR>(l) & 0xFFFF) | (gsl::narrow_cast<DWORD>(gsl::narrow_cast<WORD>(gsl::narrow_cast<DWORD_PTR>(h) & 0xFFFF)) << 16));
		return gsl::narrow_cast<LPARAM>(dcxLOWORD(l) | (gsl::narrow_cast<DWORD>(dcxLOWORD(h)) << 16));
	}

	template <class T>
	GSL_SUPPRESS(lifetime)
		inline auto dcxGetProp(_In_ HWND hwnd, _In_ const TCHAR* const str) noexcept
	{
		if constexpr (std::is_integral_v<T>)
			return reinterpret_cast<T>(GetProp(hwnd, str));
		else if constexpr (std::is_enum_v<T>)
			return static_cast<T>(reinterpret_cast<DWORD>(GetProp(hwnd, str)));
		else
			return static_cast<T>(GetProp(hwnd, str));
	}

	template <class T, class Window>
	GSL_SUPPRESS(lifetime)
		inline auto dcxGetProp(_In_ Window hwnd, _In_ const TCHAR* const str) noexcept
	{
		if constexpr (std::is_integral_v<T>)
			return reinterpret_cast<T>(GetProp(reinterpret_cast<HWND>(hwnd), str));
		else if constexpr (std::is_enum_v<T>)
			return static_cast<T>(reinterpret_cast<DWORD>(GetProp(reinterpret_cast<HWND>(hwnd), str)));
		else
			return static_cast<T>(GetProp(reinterpret_cast<HWND>(hwnd), str));
	}

	//template <class T, std::size_t N>
	//inline T dcxGetProp(HWND hwnd, const TCHAR(&str)[N])
	//{
	//	return dcxGetProp<T>(hwnd, &str[0]);
	//}

	//template <DcxConcepts::ImplementsDataFunction T>
	//GSL_SUPPRESS(lifetime)
	//	inline auto dcxGetWindowText(HWND hwnd)
	//{
	//	T txt;
	//
	//	if (!hwnd)
	//		return txt;
	//
	//	// NB: needs to include space for end 0
	//	if (const auto nText = GetWindowTextLength(hwnd) + 2; nText > 2)
	//	{
	//		txt.reserve(gsl::narrow_cast<UINT>(nText));
	//		GetWindowText(hwnd, txt.to_chr(), nText);
	//	}
	//	return txt;
	//}

#if DCX_USE_CREGEX
	template <class Input>
	std::basic_regex<TCHAR> make_mirc_regex(const Input& str)
	{
		// need to remove the // from the pattern so /^blah$/ becomes ^blah$
		const auto patlen = _ts_strlen(str);
		auto tmp = std::make_unique<TCHAR[]>(patlen);
		auto rType = std::regex::awk;

		//if ()
		_ts_strcpyn(tmp.get(), pattern + 1, patlen - 2);
		return std::basic_regex<TCHAR>(tmp.get(), rType);

}
#endif

}
