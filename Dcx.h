#pragma once
//#include "defines.h"
#include "mIRCLinker.h"
#include "DcxUXModule.h"
#include "DcxGDIModule.h"
#include "DcxDWMModule.h"
#include "DcxDPIModule.h"
#include "DcxWebView2Module.h"
#include "DcxDCompModule.h"
#include "Classes/xpopup/XPopupMenuManager.h"
#include "Classes/xpopup/xmenubar.h"
#include "Classes/DcxDialog.h"
#include "Classes\UltraDock\dcxDock.h"

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

namespace Dcx
{
	extern std::byte m_iGhostDrag;
	extern IClassFactory* m_pClassFactory;
	extern bool m_bDX9Installed;

	extern DcxDialogCollection Dialogs;
	extern DcxGDIModule GDIModule;
	extern DcxUXModule UXModule;
	extern DcxDWMModule DwmModule;
	extern DcxDPIModule DpiModule;
	extern DcxWebView2Module WebViewModule;
	extern DcxDCompModule DCompModule;

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
	[[nodiscard]] auto to_vector(const T& r)
	{
		std::vector<std::ranges::range_value_t<decltype(r)>> v;

		if constexpr (std::ranges::sized_range<decltype(r)>)
			v.reserve(std::ranges::size(r));

		std::ranges::copy(r, std::back_inserter(v));
		return v;
	}

	/// <summary>
	/// 32bit Multiply and divide
	/// </summary>
	/// <param name="iNum"></param>
	/// <param name="iMul"></param>
	/// <param name="iDiv"></param>
	/// <returns></returns>
	[[nodiscard]] constexpr int32_t dcxMulDiv32(const int32_t iNum, const int32_t iMul, const int32_t iDiv) noexcept
	{
		return gsl::narrow_cast<int32_t>((gsl::narrow_cast<int64_t>(iNum) * gsl::narrow_cast<int64_t>(iMul)) / iDiv);
	}

	/// <summary>
	/// Multiply and divide
	/// Takes any bit width input & gives that bitwidth output. (max 64bit)
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="iNum"></param>
	/// <param name="iMul"></param>
	/// <param name="iDiv"></param>
	/// <returns></returns>
	template <DcxConcepts::IsNumeric T>
	[[nodiscard]] constexpr T dcxMulDiv(const T& iNum, const T& iMul, const T& iDiv) noexcept
	{
		return gsl::narrow_cast<T>((gsl::narrow_cast<int64_t>(iNum) * gsl::narrow_cast<int64_t>(iMul)) / iDiv);
	}

	/// <summary>
	/// Rounds a number.
	/// </summary>
	/// <typeparam name="Result"></typeparam>
	/// <typeparam name="Input"></typeparam>
	/// <param name="in"></param>
	/// <returns></returns>
	template <DcxConcepts::IsNumeric Result, DcxConcepts::IsNumeric Input>
	[[nodiscard]] constexpr Result dcxRound(const Input in) noexcept
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
	using dcxBRUSH_t = std::unique_ptr<HBRUSH__, void(*)(HBRUSH)>;

	/// <summary>
	/// Make a cursor resource
	/// </summary>
	/// <param name="tsFilename"></param>
	/// <returns></returns>
	inline dcxCursor_t make_cursor(const TString& tsFilename) {
		return make_resource(dcxLoadCursorFromFile, [](HCURSOR hCursor) noexcept { if (hCursor) DestroyCursor(hCursor); }, tsFilename);
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
		return make_resource(dcxLoadIcon, [](HICON hIcon) noexcept { if (hIcon) DestroyIcon(hIcon); }, index, filename, large, flags);
	}

	/// <summary>
	/// Make a file resource
	/// </summary>
	/// <param name="file"></param>
	/// <param name="modes"></param>
	/// <returns></returns>
	inline dcxFile_t make_file(const WCHAR* file, const WCHAR* modes) { return make_resource(_wfopen, [](FILE* file) noexcept { if (file) fclose(file); }, file, modes); }

	/// <summary>
	/// Make an HDC resource
	/// </summary>
	/// <param name="hdc"></param>
	/// <returns></returns>
	inline dcxHDC_t make_hdc(HDC hdc) { return make_resource(CreateCompatibleDC, [](HDC obj) noexcept { if (obj) DeleteDC(obj); }, hdc); }

	/// <summary>
	/// Make a Bitmap resource
	/// </summary>
	/// <param name="hdc"></param>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <returns></returns>
	inline dcxBitmap_t make_bitmap(HDC hdc, int x, int y) { return make_resource(CreateCompatibleBitmap, [](HBITMAP obj) noexcept { if (obj) DeleteBitmap(obj); }, hdc, x, y); }

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
			[](ULONG_PTR* file) noexcept { if ((file) && (file != INVALID_HANDLE_VALUE)) CloseHandle(file); }, file, dAccess, dShareMode, lpSecurity, dCreation, dflags, templateFile);
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
		dcxResource() = delete;									// no default!
		dcxResource(const dcxResource&) = delete;				// no copy!
		dcxResource& operator =(const dcxResource&) = delete;	// No assignments!
		dcxResource(dcxResource&&) = delete;					// no move!
		dcxResource& operator =(dcxResource&&) = delete;		// No assignments!

		//dcxResource(const Unique& m_uni) noexcept
		//	: m_uni(m_uni)
		//{
		//}
		bool operator==(const dcxResource& other) const = default;

		using pointer = typename Unique::pointer;
		using element_type = typename Unique::element_type;
		using return_type = BaseType;

		constexpr dcxResource(Unique u) noexcept
			: m_uni(std::move(u))
		{
		}
		constexpr operator return_type() const noexcept { return get(); }
		constexpr explicit operator bool() const noexcept { return (m_uni != nullptr); }
		[[nodiscard("Memory Leak")]] constexpr BaseType release() const noexcept { return static_cast<BaseType>(m_uni.release()); }
		constexpr return_type get() const noexcept { return (m_uni.get()); }
		void reset(BaseType u) noexcept { m_uni.reset(u); }
	private:
		Unique		m_uni;
	};

	struct dcxFileResource final
		: dcxResource < dcxFile_t >
	{
		dcxFileResource() = delete;										// no default constructor!
		dcxFileResource(const dcxFileResource&) = delete;				// no copy!
		//dcxFileResource& operator =(const dcxFileResource&) = delete;	// No assignments!
		dcxFileResource(dcxFileResource&&) = delete;					// no move constructor!
		//dcxFileResource& operator =(dcxFileResource&&) = delete;		// No move assignments!

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
		using dcxResource::operator==;
		//bool operator==(const dcxFileResource& other) const = default;

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
		//dcxFileHandleResource& operator =(const dcxFileHandleResource&) = delete;	// No assignments!
		dcxFileHandleResource(dcxFileHandleResource&&) = delete;				// no copy!
		//dcxFileHandleResource& operator =(dcxFileHandleResource&&) = delete;	// No assignments!

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
		GSL_SUPPRESS(c.128) dcxStringResource& operator =(const dcxStringResource&) = delete;	// No assignments!

		// calls std::make_unique<TCHAR[]>(uSize)
		dcxStringResource(const size_t uSize)
			: dcxResource(std::make_unique<TCHAR[]>(uSize))
		{}
	};

	struct dcxBSTRResource final
		: dcxResource < dcxBSTR_t >
	{
		dcxBSTRResource() = delete;										// no default!
		dcxBSTRResource(const dcxBSTRResource&) = delete;				// no copy!
		GSL_SUPPRESS(c.128) dcxBSTRResource& operator =(const dcxBSTRResource&) = delete;	// No assignments!
		dcxBSTRResource(dcxBSTRResource&&) = delete;
		GSL_SUPPRESS(c.128) dcxBSTRResource& operator =(dcxBSTRResource&&) = delete;

		dcxBSTRResource(const WCHAR* const wstr)
			: dcxResource(make_bstr(wstr))
		{}
	};

	struct dcxCursorResource final
		: dcxResource < dcxCursor_t >
	{
		dcxCursorResource() = delete;										// no default!
		dcxCursorResource(const dcxCursorResource&) = delete;				// no copy!
		GSL_SUPPRESS(c.128) dcxCursorResource& operator =(const dcxCursorResource&) = delete;	// No assignments!
		dcxCursorResource(dcxCursorResource&&) = delete;
		GSL_SUPPRESS(c.128) dcxCursorResource& operator =(dcxCursorResource&&) = delete;

		// calls dcxLoadCursorFromFile()
		explicit dcxCursorResource(const TString& tsFilename)
			: dcxResource(make_cursor(tsFilename))
		{
		}

		// calls dcxLoadCursorFromResource()
		explicit dcxCursorResource(const PTCHAR CursorType)
			: dcxResource(make_resource(dcxLoadCursorFromResource, [](HCURSOR hCursor) noexcept { if (hCursor) DestroyCursor(hCursor); }, CursorType))
		{
		}
	};

	struct dcxIconResource final
		: dcxResource < dcxIcon_t >
	{
		dcxIconResource() = delete;										// no default!
		dcxIconResource(const dcxIconResource&) = delete;				// no copy!
		GSL_SUPPRESS(c.128) dcxIconResource& operator =(const dcxIconResource&) = delete;	// No assignments!
		dcxIconResource(dcxIconResource&&) = delete;
		GSL_SUPPRESS(c.128) dcxIconResource& operator =(dcxIconResource&&) = delete;

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
				}, [](HICON hIcon) noexcept { if (hIcon) DestroyIcon(hIcon); }, filename, fIndex, bLarge))
		{
		}
	};

	struct dcxHDCResource
		: dcxResource < dcxHDC_t >
	{
		dcxHDCResource() = delete;									// no default!
		dcxHDCResource(const dcxHDCResource&) = delete;				// no copy!
		GSL_SUPPRESS(c.128) dcxHDCResource& operator =(const dcxHDCResource&) = delete;	// No assignments!
		dcxHDCResource(dcxHDCResource&&) = delete;
		GSL_SUPPRESS(c.128) dcxHDCResource& operator =(dcxHDCResource&&) = delete;

		// calls CreateCompatibleDC()
		explicit dcxHDCResource(HDC hdc)
			: dcxResource(make_hdc(hdc))
		{
		}

		// calls CreateDC()
		dcxHDCResource(HDC hdc, LPCTSTR lpszDriver, LPCTSTR lpszDevice, LPCTSTR lpszOutput, const DEVMODE* lpInitData)
			: dcxResource(make_resource(CreateDC, [](HDC obj) noexcept { if (obj) DeleteDC(obj); }, lpszDriver, lpszDevice, lpszOutput, lpInitData))
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
		GSL_SUPPRESS(c.128) dcxHDCBitmapResource& operator =(const dcxHDCBitmapResource&) = delete;	// No assignments!
		dcxHDCBitmapResource(dcxHDCBitmapResource&&) = delete;				// no move!
		GSL_SUPPRESS(c.128) dcxHDCBitmapResource& operator =(dcxHDCBitmapResource&&) = delete;	// No assignments!

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
		GSL_SUPPRESS(c.128) dcxBitmapResource& operator =(const dcxBitmapResource&) = delete;	// No assignments!
		dcxBitmapResource(dcxBitmapResource&&) = delete;
		GSL_SUPPRESS(c.128) dcxBitmapResource& operator =(dcxBitmapResource&&) = delete;

		// calls CreateCompatibleBitmap()
		dcxBitmapResource(HDC hdc, int w, int h)
			: dcxResource(make_bitmap(hdc, w, h))
		{
		}

		//calls CreateDIBitmap();
		dcxBitmapResource(HDC hdc, const BITMAPINFOHEADER* pbmih, DWORD flInit, const void* pjBits, const BITMAPINFO* pbmi, UINT iUsage)
			: dcxResource(make_resource(CreateDIBitmap, [](HBITMAP obj) noexcept { if (obj) DeleteBitmap(obj); }, hdc, pbmih, flInit, pjBits, pbmi, iUsage))
		{
		}

		//calls CreateDIBSection();
		dcxBitmapResource(HDC hdc, const BITMAPINFO* pbmi, UINT usage, void** ppvBits, HANDLE hSection, DWORD offset)
			: dcxResource(make_resource(CreateDIBSection, [](HBITMAP obj) noexcept { if (obj) DeleteBitmap(obj); }, hdc, pbmi, usage, ppvBits, hSection, offset))
		{
		}
	};

	struct dcxHDCBitmap2Resource
		: dcxHDCResource
	{
		dcxHDCBitmap2Resource() = delete;											// no default!
		dcxHDCBitmap2Resource(const dcxHDCBitmap2Resource&) = delete;				// no copy!
		GSL_SUPPRESS(c.128) dcxHDCBitmap2Resource& operator =(const dcxHDCBitmap2Resource&) = delete;	// No assignments!
		dcxHDCBitmap2Resource(dcxHDCBitmap2Resource&&) = delete;
		GSL_SUPPRESS(c.128) dcxHDCBitmap2Resource& operator =(dcxHDCBitmap2Resource&&) = delete;

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

	struct dcxHDCBuffer final
		: dcxHDCBitmap2Resource
	{
		dcxHDCBuffer() = delete;
		dcxHDCBuffer(const dcxHDCBuffer&) = delete;
		GSL_SUPPRESS(c.128) dcxHDCBuffer& operator = (const dcxHDCBuffer&) = delete;
		dcxHDCBuffer(dcxHDCBuffer&&) = delete;
		GSL_SUPPRESS(c.128) dcxHDCBuffer& operator = (dcxHDCBuffer&&) = delete;

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
			SetTextAlign(this->get(), GetTextAlign(hdc));

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

		explicit dcxVariant(bool val) noexcept
			: dcxVariant()
		{
			vt = VT_BOOL;
			boolVal = gsl::narrow_cast<VARIANT_BOOL>(val);
		}

		explicit dcxVariant(int val) noexcept
			: dcxVariant()
		{
			vt = VT_I4;
			intVal = gsl::narrow_cast<INT>(val);
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
		GSL_SUPPRESS(c.128) dcxVariant& operator = (const dcxVariant&) noexcept = default;
		dcxVariant(dcxVariant&&) noexcept = default;
		GSL_SUPPRESS(c.128) dcxVariant& operator = (dcxVariant&&) noexcept = default;
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

		bool operator==(const dcxCursorPos& other) const = default;
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
			iter& operator --() noexcept { --n; return *this; }
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
			//HTREEITEM operator * () const noexcept { return itr_Item; }
			dcxTreeItem operator * () const noexcept { return dcxTreeItem{ itr_Window,itr_Item }; }
			iter& operator ++() noexcept
			{
				itr_Item = TreeView_GetNextSibling(itr_Window, itr_Item);
				return *this;
			}
			iter& operator --() noexcept
			{
				itr_Item = TreeView_GetPrevSibling(itr_Window, itr_Item);
				return *this;
			}
			iter operator ++(int) noexcept
			{
				iter res{ *this };
				itr_Item = TreeView_GetNextSibling(itr_Window, itr_Item);
				return res;
			}
			iter operator --(int) noexcept
			{
				iter res{ *this };
				itr_Item = TreeView_GetPrevSibling(itr_Window, itr_Item);
				return res;
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

		dcxTreeItem GetChild() const noexcept
		{
			if (!m_Window || !m_Item)
				return { nullptr, nullptr };

			return { m_Window, TreeView_GetChild(m_Window, m_Item) };
		}
		dcxTreeItem GetParent() const noexcept
		{
			if (!m_Window || !m_Item)
				return { nullptr, nullptr };

			return { m_Window, TreeView_GetParent(m_Window, m_Item) };
		}
		dcxTreeItem GetPrev() const noexcept
		{
			if (!m_Window || !m_Item)
				return { nullptr, nullptr };

			return { m_Window, TreeView_GetPrevSibling(m_Window, m_Item) };
		}
		dcxTreeItem GetNext() const noexcept
		{
			if (!m_Window || !m_Item)
				return { nullptr, nullptr };

			return { m_Window, TreeView_GetNextSibling(m_Window, m_Item) };
		}
		TVITEMEX GetItemEx(UINT uMask) const noexcept
		{
			TVITEMEX item{};
			if (!m_Window || !m_Item)
				return item;

			item.hItem = m_Item;
			// exclude text flag as no buffer set.
			item.mask = (uMask & ~TVIF_TEXT) | TVIF_HANDLE;

			TreeView_GetItem(m_Window, &item);

			return item;
		}
		bool SetItemEx(const LPTVITEMEX pItem) noexcept
		{
			if (!pItem)
				return false;
			if (!m_Window || !m_Item)
				return false;

			TVITEMEX item{ *pItem };
			item.hItem = m_Item;
			item.mask |= TVIF_HANDLE;

			return (TreeView_SetItem(m_Window, &item) != FALSE);
		}
		TString GetItemText() const
		{
			if (!m_Window || !m_Item)
				return {};

			//NB: item text can be any length BUT treeview control only displays first 260 characters.
			TString tsRes(gsl::narrow_cast<UINT>(mIRCLinker::c_mIRC_Buffer_Size_cch));
			TVITEMEX item{};
			item.hItem = m_Item;
			item.pszText = tsRes.to_chr();
			item.cchTextMax = tsRes.capacity_cch();
			item.mask = TVIF_TEXT | TVIF_HANDLE;
			if (TreeView_GetItem(m_Window, &item))
				return TString(item.pszText);

			return tsRes;
		}
		void SetItemText(const TString& tsStr) noexcept
		{
			if (!m_Window || !m_Item)
				return;

			//NB: item text can be any length BUT treeview control only displays first 260 characters.
			TVITEMEX item{};
			item.hItem = m_Item;
			GSL_SUPPRESS(type.3) item.pszText = const_cast<TCHAR*>(tsStr.to_chr());
			item.cchTextMax = tsStr.len();
			item.mask = TVIF_TEXT | TVIF_HANDLE;

			TreeView_SetItem(m_Window, &item);
		}
		template <class T>
		T GetItemParam() const noexcept
		{
			if (!m_Window || !m_Item)
				return {};

			TVITEMEX item{};
			item.hItem = m_Item;
			item.mask = TVIF_PARAM | TVIF_HANDLE;
			if (TreeView_GetItem(m_Window, &item))
			{
				if constexpr (std::is_same_v<T, LPARAM>)		// if we ant an LPARAM, just return it
					return item.lParam;
				else if constexpr (std::is_pointer_v<T>)
					return reinterpret_cast<T>(item.lParam);	// if we want a pointer, cast it
				else
					return gsl::narrow_cast<T>(item.lParam);	// otherwise its a number type conversion.
			}
			return {};
		}
		template <class T>
		void SetItemParam(T obj) noexcept
		{
			if (!m_Window || !m_Item)
				return;

			TVITEMEX item{};
			item.hItem = m_Item;
			item.mask = TVIF_PARAM | TVIF_HANDLE;

			if constexpr (std::is_same_v<T, LPARAM>)			// if we are an LPARAM, just assign it
				item.lParam = obj;
			else if constexpr (std::is_pointer_v<T>)
				item.lParam = reinterpret_cast<LPARAM>(obj);	// if we are a pointer, cast it
			else
				item.lParam = gsl::narrow_cast<LPARAM>(obj);	// otherwise its a number type conversion.

			TreeView_SetItem(m_Window, &item);
		}
		void SetItemImages(int iImage, int iSelected, int iExpanded) noexcept
		{
			if (!m_Window || !m_Item)
				return;

			//NB: item text can be any length BUT treeview control only displays first 260 characters.
			TVITEMEX item{};
			item.hItem = m_Item;
			item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_EXPANDEDIMAGE | TVIF_HANDLE;

			item.iImage = (iImage < 0) ? I_IMAGENONE : iImage;
			item.iSelectedImage = (iSelected < 0) ? item.iImage : iSelected;
			item.iExpandedImage = (iExpanded < 0) ? item.iImage : iExpanded;

			TreeView_SetItem(m_Window, &item);
		}
		RECT GetItemRect(bool bTextAreaOnly) const noexcept
		{
			RECT rc{};
			if (!m_Window || !m_Item)
				return rc;

			TreeView_GetItemRect(m_Window, m_Item, &rc, bTextAreaOnly);

			return rc;
		}

		HWND		m_Window{};
		HTREEITEM	m_Item{};
	};

	struct dcxBrush final
	{
		dcxBrush() noexcept = default;
		dcxBrush(dcxBrush&& other) = default;
		dcxBrush(const dcxBrush&) = delete;

		dcxBrush(const HBRUSH& m_data) noexcept
			: m_data(m_data)
		{
		}

		dcxBrush& operator =(dcxBrush& other) = delete;
		dcxBrush& operator =(dcxBrush&& other) noexcept = default;

		~dcxBrush()
		{
			if (m_data)
				DeleteBrush(m_data);
		}

		bool operator==(const dcxBrush& other) const noexcept = default;

		operator HBRUSH () const noexcept
		{
			return m_data;
		}

		explicit operator bool () const noexcept
		{
			return (m_data != nullptr);
		}

		dcxBrush& operator =(HBRUSH other) noexcept
		{
			if (m_data)
				DeleteBrush(m_data);

			m_data = other;

			return *this;
		}

	private:
		HBRUSH m_data{};
	};

	struct dcxBrushResource final
		: dcxResource < dcxBRUSH_t >
	{
		dcxBrushResource() = delete;														// no default!
		dcxBrushResource(const dcxBrushResource&) = delete;									// no copy!
		GSL_SUPPRESS(c.128) dcxBrushResource& operator =(const dcxBrushResource&) = delete;	// No copy assignments!
		dcxBrushResource(dcxBrushResource&&) = delete;										// no move
		GSL_SUPPRESS(c.128) dcxBrushResource& operator =(dcxBrushResource&&) = delete;		// no move assignment

		//calls CreateSolidBrush()
		dcxBrushResource(COLORREF clr)
			: dcxResource(make_resource(CreateSolidBrush, [](HBRUSH obj) noexcept { if (obj) DeleteBrush(obj); }, clr))
		{
		}

		//calls CreatePatternBrush()
		dcxBrushResource(HBITMAP hbm)
			: dcxResource(make_resource(CreatePatternBrush, [](HBRUSH obj) noexcept { if (obj) DeleteBrush(obj); }, hbm))
		{
		}

		//calls CreateHatchBrush()
		dcxBrushResource(int iHatch, COLORREF clr)
			: dcxResource(make_resource(CreateHatchBrush, [](HBRUSH obj) noexcept { if (obj) DeleteBrush(obj); }, iHatch, clr))
		{
		}

		//calls CreateBrushIndirect()
		dcxBrushResource(LOGBRUSH* plbrush)
			: dcxResource(make_resource(CreateBrushIndirect, [](HBRUSH obj) noexcept { if (obj) DeleteBrush(obj); }, plbrush))
		{
		}

		//calls CreateDIBPatternBrush()
		dcxBrushResource(HGLOBAL h, UINT iUsage)
			: dcxResource(make_resource(CreateDIBPatternBrush, [](HBRUSH obj) noexcept { if (obj) DeleteBrush(obj); }, h, iUsage))
		{
		}
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
		const T iRes{ nItemCnt - nAdjust };
		return{ std::min(iStart, iRes), std::min(iEnd, iRes) };
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

	std::pair<int, int> getItemRange(const TString& tsItems, const int nItemCnt);
	range_t<int> getItemRange2(const TString& tsItems, const int nItemCnt);

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
	void dcxLoadIconRange(HIMAGELIST himl, TString& tsFilename, const bool bLarge, const TString& tsFlags, const range_t<int>& Range);
	void dcxLoadIconRange(HIMAGELIST himl, TString& tsFilename, const bool bLarge, const TString& tsFlags, const TString& tsRanges);

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

	}

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

		CodeValue() = default;

		bool operator==(const CodeValue& other) const = default;

		CodeValue(const CodeType& code, const T& value) noexcept(std::is_nothrow_copy_constructible_v<CodeType>&& std::is_nothrow_copy_constructible_v<T>)
			: code(code), value(value)
		{
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

	inline bool dcxSetProp(_In_ HWND hwnd, _In_z_ const TCHAR* const str, _In_ DcxControl* data) noexcept
	{
		return SetPropW(hwnd, str, data);
	}

	template <class T>
	GSL_SUPPRESS(lifetime)
		inline auto dcxGetProp(_In_ HWND hwnd, _In_z_ const TCHAR* const str) noexcept
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
		inline auto dcxGetProp(_In_ Window hwnd, _In_z_ const TCHAR* const str) noexcept
	{
		if constexpr (std::is_integral_v<T>)
			return reinterpret_cast<T>(GetProp(reinterpret_cast<HWND>(hwnd), str));
		else if constexpr (std::is_enum_v<T>)
			return static_cast<T>(reinterpret_cast<DWORD>(GetProp(reinterpret_cast<HWND>(hwnd), str)));
		else
			return static_cast<T>(GetProp(reinterpret_cast<HWND>(hwnd), str));
	}

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
	COLORREF BrushToColour(HBRUSH hBrush) noexcept;
}
