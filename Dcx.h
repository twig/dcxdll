#pragma once
//#include "defines.h"
#include "mIRCLinker.h"
#include "DcxUXModule.h"
#include "DcxGDIModule.h"
#include "DcxDWMModule.h"
#include "Classes/xpopup/XPopupMenuManager.h"
#include "Classes/xpopup/xmenubar.h"
#include "Classes/DcxDialog.h"
#include "Classes\UltraDock\dcxDock.h"

//#include <sys/stat.h> 
#include <detours.h>

// should DCX use the resource wrapper classes below? set to zero to disable
#define DCX_USE_WRAPPERS 1

namespace std
{
	inline string to_string(const wstring &wstr)
	{
		string str;
		str.assign(wstr.begin(), wstr.end());
		return str;
	}

	inline wstring to_wstring(const string &str)
	{
		wstring wstr;
		wstr.assign(str.begin(), str.end());
		return wstr;
	}

	inline string to_string(const TString &wstr)
	{
		string str(wstr.c_str());
		return str;
	}

	inline wstring to_wstring(const TString &str)
	{
		wstring wstr(str.to_wchr());
		return wstr;
	}
}

namespace Dcx
{
	extern TString m_sLastError;
	extern std::byte m_iGhostDrag;
	extern IClassFactory *m_pClassFactory;
	extern bool m_bDX9Installed;
	extern HMODULE m_hRichEditLib;

	extern DcxDialogCollection Dialogs;
	extern DcxGDIModule GDIModule;
	extern DcxUXModule UXModule;
	extern DcxDWMModule VistaModule;

	/* additions */
	extern XPopupMenuManager XPopups;
	extern XMenuBar XMenubar;
	
	extern bool m_bErrorTriggered;

	extern bool setting_bStaticColours;

	HCURSOR dcxLoadCursorFromFile(const TString &filename);
	HCURSOR dcxLoadCursorFromResource(const TCHAR *CursorType);

	//// determine whether _Ty is a Number type (excluding char / wchar)
	//template<class _Ty>
	//struct is_Numeric
	//	: std::integral_constant<bool,
	//		std::is_arithmetic_v<_Ty>
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
		if (!r) { throw std::runtime_error { "Unable to create resource" }; }
		typedef typename std::decay<decltype(*r)>::type ResourceType;
		return std::unique_ptr<ResourceType, void(*)(ResourceType*)>(r, d);
	}
#else
	template<typename Creator, typename Destructor, typename... Arguments>
	auto make_resource(Creator c, Destructor d, Arguments&&... args)
		-> std::unique_ptr<typename std::decay<decltype(*c(std::forward<Arguments>(args)...))>::type, void(*)(typename std::decay<decltype(*c(std::forward<Arguments>(args)...))>::type*)>
	{
		auto r = c(std::forward<Arguments>(args)...);
		if (!r) { throw std::runtime_error { "Unable to create resource" }; }
		typedef typename std::decay<decltype(*r)>::type ResourceType;
		return std::unique_ptr<ResourceType, void(*)(ResourceType*)>(r, d);
	}
#endif

	using dcxCursor_t = std::unique_ptr<HICON__, void(*)(HICON__ *)>;
	using dcxIcon_t = std::unique_ptr<HICON__, void(*)(HICON__ *)>;
	using dcxFile_t = std::unique_ptr<FILE, void(*)(FILE *)>;
	using dcxHandle_t = std::unique_ptr<ULONG_PTR, void(*)(ULONG_PTR *)>;
	using dcxHDC_t = std::unique_ptr<HDC__, void(*)(HDC)>;
	using dcxBitmap_t = std::unique_ptr<HBITMAP__, void(*)(HBITMAP)>;
	using dcxBSTR_t = std::unique_ptr<OLECHAR, void(*)(BSTR)>;

	inline dcxCursor_t make_cursor(const TString &tsFilename) {
		return make_resource(dcxLoadCursorFromFile, [](HCURSOR hCursor){ if (hCursor != nullptr) DestroyCursor(hCursor); }, tsFilename);
	}
	inline dcxIcon_t make_icon(const int index, TString &filename, const bool large, const TString &flags) {
		return make_resource(dcxLoadIcon, [](HICON hIcon){ if (hIcon != nullptr) DestroyIcon(hIcon); }, index, filename, large, flags);
	}
	inline dcxFile_t make_file(const WCHAR *file, const WCHAR *modes) { return make_resource(_wfopen, [](FILE *file){ fclose(file); }, file, modes); }
	inline dcxHDC_t make_hdc(HDC hdc) { return make_resource(CreateCompatibleDC, [](HDC obj){ DeleteDC(obj); }, hdc); }
	inline dcxBitmap_t make_bitmap(HDC hdc, int x, int y) { return make_resource(CreateCompatibleBitmap, [](HBITMAP obj){ DeleteBitmap(obj); }, hdc, x, y); }

	//inline ULONG_PTR *dcxCreateFile(const TCHAR *file, DWORD dAccess, DWORD dShareMode, LPSECURITY_ATTRIBUTES lpSecurity, DWORD dCreation, DWORD dflags, HANDLE templateFile) {
	//	return (ULONG_PTR *)CreateFile(file, dAccess, dShareMode, lpSecurity, dCreation, dflags, templateFile);
	//}
	inline dcxHandle_t make_filehandle(const TCHAR *file, DWORD dAccess, DWORD dShareMode, LPSECURITY_ATTRIBUTES lpSecurity, DWORD dCreation, DWORD dflags, HANDLE templateFile) {
		//return make_resource(dcxCreateFile, [](ULONG_PTR *file){ CloseHandle((HANDLE)file); }, file, dAccess, dShareMode, lpSecurity, dCreation, dflags, templateFile);
		return make_resource(
			[](const TCHAR *file, DWORD dAccess, DWORD dShareMode, LPSECURITY_ATTRIBUTES lpSecurity, DWORD dCreation, DWORD dflags, HANDLE templateFile) { return (ULONG_PTR *)CreateFile(file, dAccess, dShareMode, lpSecurity, dCreation, dflags, templateFile); },
			[](ULONG_PTR *file) { CloseHandle((HANDLE)file); }, file, dAccess, dShareMode, lpSecurity, dCreation, dflags, templateFile);
	}
	inline dcxBSTR_t make_bstr(const WCHAR *const wstr) { return make_resource(SysAllocString, [](BSTR obj) { if (obj != nullptr) SysFreeString(obj); }, wstr); }

	// NB: BaseType can be defined as some other pointer type
	// so we static_cast it later on
	template <typename Unique, typename BaseType = Unique::pointer>
	struct dcxResource {
		dcxResource() = delete;																		// no default!
		dcxResource(const dcxResource<Unique,BaseType> &) = delete;									// no copy!
		dcxResource<Unique, BaseType> &operator =(const dcxResource<Unique, BaseType> &) = delete;	// No assignments!

		using pointer = typename Unique::pointer;
		using element_type = typename Unique::element_type;
		using return_type = BaseType;

		constexpr dcxResource(Unique u)
			: m_uni(std::move(u))
		{
		}
		constexpr operator BaseType() const noexcept { return get(); }
		constexpr explicit operator bool() const noexcept { return (m_uni != nullptr); }
		constexpr BaseType release() const noexcept { return static_cast<BaseType>(m_uni.release()); }
		constexpr BaseType get() const noexcept { return static_cast<BaseType>(m_uni.get()); }
	private:
		Unique		m_uni;
	};

	struct dcxFileResource : dcxResource < dcxFile_t >
	{
		dcxFileResource() = delete;										// no default!
		dcxFileResource(const dcxFileResource &) = delete;				// no copy!
		dcxFileResource &operator =(const dcxFileResource &) = delete;	// No assignments!

		// calls _wfopen()
		dcxFileResource(const WCHAR *tsFilename, const WCHAR *tsMode)
			: dcxResource(make_file(tsFilename, tsMode))
		{
		}

		// calls _wfopen()
		dcxFileResource(const TString &tsFilename, const WCHAR *tsMode)
			: dcxResource(make_file(tsFilename.to_wchr(), tsMode))
		{
		}

		const uint32_t Size() const noexcept
		{
			// Seek End of file
			if (fseek(*this, 0, SEEK_END))
				return 0UL;

			// Read pointer location, because pointer is at the end, results into file size
			auto size = gsl::narrow_cast<uint32_t>(ftell(*this));

			// Get back to file beginning
			if (fseek(*this, 0, SEEK_SET))
				return 0UL;

			return size;
		}

		const uint64_t SizeEx() const noexcept
		{
			// Seek End of file
			if (_fseeki64(*this, 0, SEEK_END))
				return 0ULL;

			// Read pointer location, because pointer is at the end, results into file size
			auto size = gsl::narrow_cast<uint64_t>(_ftelli64(*this));

			// Get back to file beginning
			if (_fseeki64(*this, 0, SEEK_SET))
				return 0ULL;

			return size;
		}
	};

	struct dcxFileHandleResource : dcxResource < dcxHandle_t >
	{
		dcxFileHandleResource() = delete;											// no default!
		dcxFileHandleResource(const dcxFileHandleResource &) = delete;				// no copy!
		dcxFileHandleResource &operator =(const dcxFileHandleResource &) = delete;	// No assignments!

		// calls CreateFile()
		dcxFileHandleResource(const TCHAR *file, DWORD dAccess, DWORD dShareMode, LPSECURITY_ATTRIBUTES lpSecurity, DWORD dCreation, DWORD dflags, HANDLE templateFile)
			: dcxResource(make_filehandle(file, dAccess, dShareMode, lpSecurity, dCreation, dflags, templateFile))
		{
		}

		const auto Size() const noexcept
		{
			return GetFileSize(*this, nullptr);
		}

		const LARGE_INTEGER SizeEx() const noexcept
		{
			LARGE_INTEGER sz{};
			GetFileSizeEx(*this, &sz);
			return sz;
		}
	};

	struct dcxStringResource : dcxResource < std::unique_ptr<TCHAR[]> >
	{
		dcxStringResource() = delete;										// no default!
		dcxStringResource(const dcxStringResource &) = delete;				// no copy!
		dcxStringResource &operator =(const dcxStringResource &) = delete;	// No assignments!

		// calls std::make_unique<TCHAR[]>(uSize)
		dcxStringResource(const size_t &uSize)
			: dcxResource(std::make_unique<TCHAR[]>(uSize))
		{}
	};

	struct dcxBSTRResource : dcxResource < dcxBSTR_t >
	{
		dcxBSTRResource() = delete;										// no default!
		dcxBSTRResource(const dcxBSTRResource &) = delete;				// no copy!
		dcxBSTRResource &operator =(const dcxBSTRResource &) = delete;	// No assignments!

		dcxBSTRResource(const WCHAR *const wstr)
			: dcxResource(make_bstr(wstr))
		{}
	};

	struct dcxCursorResource : dcxResource < dcxCursor_t >
	{
		dcxCursorResource() = delete;										// no default!
		dcxCursorResource(const dcxCursorResource &) = delete;				// no copy!
		dcxCursorResource &operator =(const dcxCursorResource &) = delete;	// No assignments!

		// calls dcxLoadCursorFromFile()
		explicit dcxCursorResource(const TString &tsFilename)
			: dcxResource(make_cursor(tsFilename))
		{
		}
		// calls dcxLoadCursorFromResource()
		explicit dcxCursorResource(const PTCHAR CursorType)
			: dcxResource(make_resource(dcxLoadCursorFromResource, [](HCURSOR hCursor) { if (hCursor != nullptr) DestroyCursor(hCursor); }, CursorType))
		{
		}
	};

	struct dcxIconResource : dcxResource < dcxIcon_t >
	{
		dcxIconResource() = delete;										// no default!
		dcxIconResource(const dcxIconResource &) = delete;				// no copy!
		dcxIconResource &operator =(const dcxIconResource &) = delete;	// No assignments!

		// calls dcxLoadIcon()
		dcxIconResource(const int index, TString &filename, const bool large, const TString &flags)
			: dcxResource(make_icon(index, filename, large, flags))
		{
		}

		// calls ExtractIconEx()
		dcxIconResource(const TString &filename, const int fIndex, const bool bLarge)
			: dcxResource(make_resource([](const TString &filename, const int fIndex, const bool bLarge) {
			HICON m_hIcon = nullptr;
			if (bLarge)
				ExtractIconEx(filename.to_chr(), fIndex, &m_hIcon, nullptr, 1);
			else
				ExtractIconEx(filename.to_chr(), fIndex, nullptr, &m_hIcon, 1);
			return m_hIcon;
		}, [](HICON hIcon) { if (hIcon != nullptr) DestroyIcon(hIcon); }, filename, fIndex, bLarge))
		{
		}
	};

	struct dcxHDCResource : dcxResource < dcxHDC_t >
	{
		dcxHDCResource() = delete;										// no default!
		dcxHDCResource(const dcxHDCResource &) = delete;				// no copy!
		dcxHDCResource &operator =(const dcxHDCResource &) = delete;	// No assignments!

		// calls CreateCompatibleDC()
		explicit dcxHDCResource(HDC hdc)
			: dcxResource(make_hdc(hdc))
		{
		}
		// calls CreateDC()
		dcxHDCResource(HDC hdc, LPCTSTR lpszDriver, LPCTSTR lpszDevice, LPCTSTR lpszOutput, const DEVMODE *lpInitData)
			: dcxResource(make_resource(CreateDC, [](HDC obj) { DeleteDC(obj); }, lpszDriver, lpszDevice, lpszOutput, lpInitData))
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
	struct dcxHDCBitmapResource : dcxHDCResource
	{
		dcxHDCBitmapResource() = delete;											// no default!
		dcxHDCBitmapResource(const dcxHDCBitmapResource &) = delete;				// no copy!
		dcxHDCBitmapResource &operator =(const dcxHDCBitmapResource &) = delete;	// No assignments!

		//calls CreateCompatibleDC() then SelectBitmap()
		dcxHDCBitmapResource(HDC hdc, HBITMAP hBitmap)
			: dcxHDCResource(hdc)
			, m_hOldBitmap(nullptr)
		{
			if (hBitmap != nullptr)
				m_hOldBitmap = SelectBitmap(*this, hBitmap);
		}
		~dcxHDCBitmapResource()
		{
			if (m_hOldBitmap != nullptr)
				SelectBitmap(*this, m_hOldBitmap);
		}
	private:
		HBITMAP	m_hOldBitmap;
	};

	struct dcxBitmapResource : dcxResource < dcxBitmap_t >
	{
		dcxBitmapResource() = delete;										// no default!
		dcxBitmapResource(const dcxBitmapResource &) = delete;				// no copy!
		dcxBitmapResource &operator =(const dcxBitmapResource &) = delete;	// No assignments!

		// calls CreateCompatibleBitmap()
		dcxBitmapResource(HDC hdc, int w, int h)
			: dcxResource(make_bitmap(hdc, w, h))
		{
		}
		
		//calls CreateDIBitmap();
		dcxBitmapResource(HDC hdc, const BITMAPINFOHEADER *pbmih, DWORD flInit, const void *pjBits, const BITMAPINFO *pbmi, UINT iUsage)
			: dcxResource(make_resource(CreateDIBitmap, [](HBITMAP obj) { DeleteBitmap(obj); }, hdc, pbmih, flInit, pjBits, pbmi, iUsage))
		{
		}

		//calls CreateDIBSection();
		dcxBitmapResource(HDC hdc,const BITMAPINFO *pbmi, UINT usage,void **ppvBits, HANDLE hSection, DWORD offset)
			: dcxResource(make_resource(CreateDIBSection, [](HBITMAP obj) { DeleteBitmap(obj); }, hdc, pbmi, usage, ppvBits, hSection, offset))
		{
		}
	};

	struct dcxHDCBitmap2Resource
		: dcxHDCResource
	{
		dcxHDCBitmap2Resource() = delete;											// no default!
		dcxHDCBitmap2Resource(const dcxHDCBitmap2Resource &) = delete;				// no copy!
		dcxHDCBitmap2Resource &operator =(const dcxHDCBitmap2Resource &) = delete;	// No assignments!

		//calls CreateCompatibleDC() then CreateCompatibleBitmap(), then SelectBitmap()
		dcxHDCBitmap2Resource(HDC hdc, const int &w, const int &h)
			: dcxHDCResource(hdc)
			, m_hBitmap(hdc, w, h)
			, m_Width(w)
			, m_Height(h)
		{
			m_hOldBitmap = SelectBitmap(*this, m_hBitmap);
		}
		~dcxHDCBitmap2Resource()
		{
			if (m_hOldBitmap != nullptr)
				SelectBitmap(*this, m_hOldBitmap);
		}
		const int &getWidth() const noexcept { return m_Width; }
		const int &getHeight() const noexcept { return m_Height; }
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

	struct dcxHDCBuffer : dcxHDCBitmap2Resource {
		dcxHDCBuffer() = delete;
		dcxHDCBuffer(const dcxHDCBuffer &) = delete;
		dcxHDCBuffer &operator = (const dcxHDCBuffer &) = delete;

		dcxHDCBuffer(const HDC hdc, const RECT &rc)
			: dcxHDCBitmap2Resource(hdc, (rc.right - rc.left), (rc.bottom - rc.top))
			, m_hOldFont(nullptr)
		{
			// copy settings from hdc to buffer's hdc.
			SetDCBrushColor(*this, GetDCBrushColor(hdc));
			SetDCPenColor(*this, GetDCPenColor(hdc));
			SetLayout(*this, GetLayout(hdc));
			m_hOldFont = SelectFont(*this, GetCurrentObject(hdc, OBJ_FONT));
			SetTextColor(*this, GetTextColor(hdc));
			SetBkColor(*this, GetBkColor(hdc));
			SetBkMode(*this, GetBkMode(hdc));
			SetROP2(*this, GetROP2(hdc));
			SetMapMode(*this, GetMapMode(hdc));
			SetPolyFillMode(*this, GetPolyFillMode(hdc));
			SetStretchBltMode(*this, GetStretchBltMode(hdc));
			SetGraphicsMode(*this, GetGraphicsMode(hdc));

			// copy contents of hdc within area to buffer.
			BitBlt(*this, 0, 0, getWidth(), getHeight(), hdc, rc.left, rc.top, SRCCOPY);

			// buffer is an exact duplicate of the hdc within the area specified.
		}
		~dcxHDCBuffer()
		{
			GdiFlush();
			if (m_hOldFont != nullptr)
				SelectFont(*this, m_hOldFont);
		}
	private:
		HFONT m_hOldFont;
	};

	struct dcxVariant
		: VARIANT
	{
		dcxVariant()
		{
			VariantInit(this);
		}
		explicit dcxVariant(const BSTR bStr)
			: dcxVariant()
		{
			vt = VT_BSTR;
			bstrVal = bStr;
		}
		explicit dcxVariant(const WCHAR *cStr)
			: dcxVariant(SysAllocString(cStr))
		{
		}
		~dcxVariant()
		{
			VariantClear(this);	// this does SysFreeString() for us.
		}
	};

	struct dcxCursorPos
		: POINT
	{
		dcxCursorPos()
		{
			if (GetCursorPos(this) == FALSE)
				throw Dcx::dcxException(TEXT("Unable to get Cursor Position"));
		}
		explicit dcxCursorPos(HWND hwnd)
			: dcxCursorPos()
		{
			MapWindowPoints(nullptr, hwnd, this, 1);
		}
		~dcxCursorPos() = default;
	};

	struct dcxWindowRect
		: RECT
	{
		dcxWindowRect() = delete;
		// Gets the window rect for hwnd
		explicit dcxWindowRect(HWND hwnd)
		{
			if (GetWindowRect(hwnd, this) == FALSE)
				throw Dcx::dcxException(TEXT("Unable to get Window Rect"));
		}

		// Gets the window rect for hwnd & maps it to hMap
		dcxWindowRect(HWND hwnd, HWND hMap)
			: dcxWindowRect(hwnd)
		{
			SetLastError(0U);
			MapWindowRect(nullptr, hMap, this);
			if (GetLastError() != 0U)
				throw Dcx::dcxException(TEXT("Unable to Map Window Rect"));
		}
		~dcxWindowRect() = default;

		// get the rect's width
		long Width() const noexcept { return (right - left); }
		// get the rect's height
		long Height() const noexcept { return (bottom - top); }
	};

	struct dcxClassName
		: stString<257>
	{
		dcxClassName() = delete;
		// Gets the class name for hwnd
		explicit dcxClassName(HWND hwnd)
		{
			GetClassName(hwnd, this->data(), gsl::narrow_cast<int32_t>(this->size()));
		}
		~dcxClassName() = default;
	};

	struct dcxPalette
	{
		dcxPalette()
			: m_Palette{ CLR_INVALID }
		{
			getmIRCPalette(&m_Palette[0], m_size); // get mIRC palette
		}
		explicit operator COLORREF *() noexcept
		{
			return &m_Palette[0];
		}
		constexpr size_t size() const noexcept { return m_size; }

		struct iter
		{
			COLORREF operator * () const noexcept { return itr_Palette[n]; }
			iter& operator ++() noexcept { ++n; return *this; }
			friend
				bool operator != (iter const& lhs, iter const& rhs) noexcept
			{
				return lhs.n != rhs.n;
			}
		
			size_t	n;
			const COLORREF	*const itr_Palette;
		};
		
		iter begin() const noexcept { return{ 0, &m_Palette[0] }; }
		iter end() const noexcept { return{ size(), nullptr }; }

		constexpr static const ptrdiff_t m_size = mIRC_PALETTE_SIZE;
		COLORREF m_Palette[m_size];
	};
	using MapOfCursors = std::map<HCURSOR, HCURSOR>;
	using MapOfAreas = std::map<UINT, HCURSOR>;

	// example code to get a range (purely for testing)
	//template< typename T >
	//struct range_t
	//{
	//	struct iter
	//	{
	//		T operator * () const noexcept { return n; }
	//		iter& operator ++() noexcept { ++n; return *this; }
	//		friend
	//			bool operator != (iter const& lhs, iter const& rhs) noexcept
	//		{
	//			return lhs.n != rhs.n;
	//		}
	//
	//		T n;
	//	};
	//
	//	iter begin() const noexcept { return{ b }; }
	//	iter end() const noexcept { return{ e }; }
	//	T b, e;
	//};
	//template< typename T > range_t<T>  range(T b, T e) { return{ b, e }; }

	template <typename T>
	struct dcxNumber {

		constexpr operator std::make_unsigned_t<T>() const noexcept { return gsl::narrow_cast<std::make_unsigned_t<T>>(m_nValue); }
		constexpr operator std::make_signed_t<T>() const noexcept { return gsl::narrow_cast<std::make_signed_t<T>>(m_nValue); }
		constexpr explicit operator bool() const noexcept { return (m_nValue != T()); }
		T &operator =(const std::make_unsigned_t<T> &other)
		{
			m_nValue = T(other);
			return m_nValue;
		}
		T &operator =(const std::make_signed_t<T> &other)
		{
			m_nValue = T(other);
			return m_nValue;
		}
		bool operator ==(const std::make_unsigned_t<T> &other) { return m_nValue == T(other); }
		bool operator ==(const std::make_signed_t<T> &other) { return m_nValue == T(other); }

		T m_nValue;
	};

	void setupOSCompatibility(void);
	void freeOSCompatibility(void);

	IClassFactory *const getClassFactory() noexcept;
	const TString &getLastError() noexcept;
	const std::byte &getGhostDrag() noexcept;
	bool setGhostDrag(const std::byte newAlpha) noexcept;
	const bool &isDX9Installed() noexcept;
	bool isUnloadSafe();
	//bool isFile(const WCHAR *const file);
	//bool isFile(LPCSTR const file);

	void load(mIRCLinker::LOADINFO *const lInfo);
	void unload(void);
	const bool &initDirectX();
	const bool &initDirectX(TCHAR *dxResult, int dxSize);
	void error(const TCHAR *const cmd, const TCHAR *const msg);
	//void errorex(const TCHAR *const cmd, const TCHAR *const szFormat, ...);
	template <typename Format, typename Value, typename... Arguments>
	void error(const TCHAR *const cmd, const Format &fmt, const Value &val, Arguments&&... args)
	{
		TString tsErr;
		error(cmd, _ts_sprintf(tsErr, fmt, val, args...).to_chr());
	}

	//int mark(TCHAR *const data, const TString & tsDName, const TString & tsCallbackName);
	int mark(const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &data, const TString & tsDName, const TString & tsCallbackName);
	LRESULT CALLBACK mIRCSubClassWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	//const char *const dcxGetFormattedString(const TCHAR *const fmt, ...);
	const TCHAR *const parseCursorType(const TString & cursor);
	const TCHAR *const parseCursorType(const std::hash<TString>::result_type & cursor);
	const DWORD parseSystemCursorType(const TString & cursor);
	const DWORD parseSystemCursorType(const std::hash<TString>::result_type & cursor);
	const DWORD parseAreaType(const TString &tsArea);
	const DWORD parseAreaType(const std::hash<TString>::result_type &tsArea);
	HCURSOR dcxLoadCursor(const UINT iFlags, const TCHAR *CursorType, bool &bCursorFromFile, const HCURSOR oldCursor, TString &filename);
	void setAreaCursor(const HCURSOR hCursor, const UINT iType);
	void deleteAreaCursor(const UINT iType);
	HCURSOR SystemToCustomCursor(const HCURSOR hCursor);
	HCURSOR AreaToCustomCursor(const UINT iType);
	void setCursor(const HCURSOR hSystemCursor, const HCURSOR hCustomCursor);
	void deleteCursor(const HCURSOR hCursor);
	HCURSOR WINAPI XSetCursor(HCURSOR hCursor);
	PVOID PatchAPI(const char *const c_szDllName, const char *const c_szApiName, PVOID newfPtr);
	void RemovePatch(PVOID fPtr, PVOID newfPtr);

	//typedef HCURSOR(WINAPI *PFNSETCURSOR)(HCURSOR hCursor);
	using PFNSETCURSOR = HCURSOR(WINAPI *)(HCURSOR hCursor);

	extern PFNSETCURSOR SetCursorUx;

	// parse_string, taken from somewhere on stackoverflow (sorry can't remember where) & modified for our use.
	template <typename RETURN_TYPE, typename STRING_TYPE>
	RETURN_TYPE parse_string(const STRING_TYPE *str)
	{
		static_assert(std::is_same_v<WCHAR, STRING_TYPE> || std::is_same_v<CHAR, STRING_TYPE>, "Only WCHAR, CHAR, or TCHAR type supported!");

		std::basic_stringstream<STRING_TYPE> buf;
		buf << str;
		RETURN_TYPE val;
		buf >> val;
		return val;
	}

	namespace details {

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

		template < typename tResult, typename tInput >
		inline constexpr tResult numeric_cast(tInput &in) noexcept
		{
			static_assert(is_Numeric_v<tResult>, "A Numeric return type is required");
			if constexpr(is_Numeric_v<tInput>)
			{
				// Casts any numeric type as another numeric type.
				return gsl::narrow_cast<tResult>(in);
			}
			else if constexpr(!std::is_convertible_v<tInput, tResult>)
			{
				// Casts any string as a number
				std::stringstream buf;
				if constexpr(std::is_array_v<tInput>)
					buf << &in[0];
				else
					buf << in;
				tResult result;
				buf >> result;
				return result;
			}
			else {
				// Converts any object to a numeric using implicit-conversion.
				return in;
			}
		}
	}

	// Converts any object to a numeric.
	//template < typename tResult, typename tInput >
	//inline constexpr tResult numeric_cast(tInput &in)
	//{
	//	static_assert(is_Numeric_v<tResult>, "A Numeric return type is required");
	//	return details::numeric_cast<tResult>(in);
	//}

	template < typename tResult, typename tInput >
	inline constexpr tResult numeric_cast(tInput &&in) noexcept
	{
		static_assert(is_Numeric_v<tResult>, "A Numeric return type is required");
		return details::numeric_cast<tResult>(std::forward<tInput>(in));
	}

	// Converts any pointer to a numeric.
	template < typename tResult, typename tInput, typename = std::enable_if_t<!std::is_same_v<std::remove_cv_t<tInput>, char> && !std::is_same_v<std::remove_cv_t<tInput>, wchar_t> > >
	inline constexpr tResult numeric_cast(tInput *in) noexcept
	{
		static_assert(is_Numeric_v<tResult>, "A Numeric return type is required");
		//if constexpr(std::is_same_v<std::remove_cv_t<tInput>,void>)
		//	return static_cast<tResult>(in);
		//else
			return reinterpret_cast<tResult>(in);
	}

	// find() - takes an array object, & something to compare against. (same as std::find(begin(),end(),val) )
	template <typename Res, typename obj, typename _Val>
	Res find(obj &SourceObject, _Val &val)
	{
		return std::find(SourceObject.begin(), SourceObject.end(), val);
	}

	// find(obj &SourceObject, _Val &val)
	// - takes an array object, & something to compare against. (same as std::find(begin(),end(),val) != end() )
	// Result
	//	true - If val is found in object otherwise false
	template <typename obj, typename _Val>
	bool find(obj &SourceObject, _Val &val)
	{
		auto itEnd = SourceObject.end();
		return (std::find(SourceObject.begin(), itEnd, val) != itEnd);
	}

	// C++11 Better countof() for arrays, taken from http://www.g-truc.net/post-0708.html
	template <typename T, std::size_t N>
	constexpr std::size_t countof(T const (&)[N]) noexcept
	{
		return N;
	}

	// countof() for container objects
	template <class C>
	std::size_t countof(C const & c)
	{
		return c.size();
	}

	// Erase an item from a container if its found.
	template <typename Cont, typename Val>
	bool eraseIfFound(Cont &con, Val &v)
	{
		//if constexpr(std::is_member_function_pointer_v<decltype(&Cont::find)>)
		//{
		//	const auto itEnd = con.end();
		//
		//	if (const auto itGot = con.find(v); itGot != itEnd)
		//	{
		//		con.erase(itGot);
		//		return true;
		//	}
		//	return false;
		//}
		//else {
			const auto itEnd = con.end();

			if (const auto itGot = std::find(con.begin(), itEnd, v); itGot != itEnd)
			{
				con.erase(itGot);
				return true;
			}
			return false;
		//}
	}

	template <class T>
	bool IsFileEx(const T &filename)
	{
		return (std::experimental::filesystem::exists(filename) && std::experimental::filesystem::is_regular_file(filename));
	}
}
