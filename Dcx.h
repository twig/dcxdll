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

#include <sys/stat.h> 
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
	extern BYTE m_iGhostDrag;
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

	HCURSOR dcxLoadCursorFromFile(const TString &filename);
	HCURSOR dcxLoadCursorFromResource(const PTCHAR CursorType);

	template<class _Ty>
	struct is_Numeric
		: std::_Cat_base<std::is_arithmetic<_Ty>::value
		&& !std::is_same<_Ty, wchar_t>::value
		&& !std::is_same<_Ty, char>::value
		&& !std::is_pointer<_Ty>::value>
	{	// determine whether _Ty is a Number type (excluding char / wchar)
	};

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
	using dcxHandle_t = std::unique_ptr<ULONG, void(*)(ULONG_PTR)>; // <- error
	using dcxHDC_t = std::unique_ptr<HDC__, void(*)(HDC)>;
	using dcxBitmap_t = std::unique_ptr<HBITMAP__, void(*)(HBITMAP)>;

	inline dcxCursor_t make_cursor(const TString &tsFilename) {
		return make_resource(dcxLoadCursorFromFile, [](HCURSOR hCursor){ if (hCursor != nullptr) DestroyCursor(hCursor); }, tsFilename);
	}
	inline dcxIcon_t make_icon(const int index, TString &filename, const bool large, const TString &flags) {
		return make_resource(dcxLoadIcon, [](HICON hIcon){ if (hIcon != nullptr) DestroyIcon(hIcon); }, index, filename, large, flags);
	}
	inline dcxFile_t make_file(const WCHAR *file, const WCHAR *modes) { return make_resource(_wfopen, [](FILE *file){ fclose(file); }, file, modes); }
	inline dcxHDC_t make_hdc(HDC hdc) { return make_resource(CreateCompatibleDC, [](HDC obj){ DeleteDC(obj); }, hdc); }
	inline dcxBitmap_t make_bitmap(HDC hdc, int x, int y) { return make_resource(CreateCompatibleBitmap, [](HBITMAP obj){ DeleteBitmap(obj); }, hdc, x, y); }

	//inline dcxHandle_t make_filehandle(const TCHAR *file, DWORD dAccess, DWORD dShareMode, LPSECURITY_ATTRIBUTES lpSecurity, DWORD dCreation, DWORD dflags, HANDLE templateFile) {
	//	return make_resource(CreateFile, [](HANDLE file){ CloseHandle(file); }, file, dAccess, dShareMode, lpSecurity, dCreation, dflags, templateFile);
	//}

	// NB: BaseType can be defined as some other pointer type
	// so we static_cast it later on
	template <typename Unique, typename BaseType = Unique::pointer>
	struct dcxResource {
		dcxResource() = delete;									// no default!
		dcxResource(const dcxResource<Unique,BaseType> &) = delete;				// no copy!
		dcxResource<Unique, BaseType> &operator =(const dcxResource<Unique, BaseType> &) = delete;	// No assignments!

		using pointer = typename Unique::pointer;
		using element_type = typename Unique::element_type;
		using return_type = BaseType;

		dcxResource(Unique u)
			: m_uni(std::move(u))
		{
		}
		operator BaseType() const noexcept { return static_cast<BaseType>(m_uni.get()); }
		explicit operator bool() const noexcept { return (m_uni != nullptr); }
		BaseType release() noexcept { return static_cast<BaseType>(m_uni.release()); }
	private:
		Unique		m_uni;
	};

	struct dcxFileResource : dcxResource < dcxFile_t >
	{
		//dcxFileResource() = delete;									// no default!
		//dcxFileResource(const dcxFileResource &) = delete;				// no copy!
		//dcxFileResource &operator =(const dcxFileResource &) = delete;	// No assignments!

		// calls _wfopen()
		dcxFileResource(const WCHAR *tsFilename, const WCHAR *tsMode)
			: dcxResource(make_file(tsFilename, tsMode))
		{
		}
	};

	struct dcxCursorResource : dcxResource < dcxCursor_t >
	{
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
		// calls dcxLoadIcon()
		dcxIconResource(const int index, TString &filename, const bool large, const TString &flags)
			: dcxResource(make_icon(index, filename, large, flags))
		{
		}

		// calls ExtractIconEx()
		dcxIconResource(const TString &filename, const int fIndex, const bool bLarge)
			: dcxResource(make_resource([](const TString &filename, const int fIndex, const bool bLarge) {
			HICON m_hIcon;
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
		//	: dcxResource(make_resource(GetDC, [](HDC obj) { ReleaseDC(nullptr, obj); }, hWnd))
		//{
		//}
		//// calls GetDCEx() - broken
		//explicit dcxHDCResource(HWND hWnd)
		//	: dcxResource(make_resource(GetDCEx, [](HDC obj) { ReleaseDC(nullptr, obj); }, hWnd))
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
		//calls CreateCompatibleDC() then SelectBitmap()
		dcxHDCBitmapResource(HDC hdc, HBITMAP hBitmap)
			: dcxHDCResource(hdc)
			, m_hOldBitmap(nullptr)
			//, m_hNewBitmap(nullptr)
		{
			if (hBitmap != nullptr)
				m_hOldBitmap = SelectBitmap(*this, hBitmap);
		}
		////calls CreateCompatibleDC() then CreateCompatibleBitmap() then SelectBitmap()
		//dcxHDCBitmapResource(HDC hdc, const int w, const int h)
		//	: dcxHDCResource(hdc)
		//	, m_hOldBitmap(nullptr)
		//	, m_hNewBitmap(nullptr)
		//{
		//	if (hBitmap != nullptr)
		//		m_hOldBitmap = SelectBitmap(hdc, hBitmap);
		//}
		~dcxHDCBitmapResource()
		{
			if (m_hOldBitmap != nullptr)
				SelectBitmap(*this, m_hOldBitmap);
		}
	private:
		HBITMAP	m_hOldBitmap;
		//HBITMAP	m_hNewBitmap;
	};

	struct dcxBitmapResource : dcxResource < dcxBitmap_t >
	{
		// calls CreateCompatibleBitmap()
		dcxBitmapResource(HDC hdc, int x, int y)
			: dcxResource(make_bitmap(hdc, x, y))
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

	struct dcxHDCBuffer : dcxHDCResource {
		dcxHDCBuffer() = delete;
		dcxHDCBuffer(const dcxHDCBuffer &) = delete;
		dcxHDCBuffer &operator = (const dcxHDCBuffer &) = delete;

		dcxHDCBuffer(const HDC hdc, RECT *rc)
			: dcxHDCResource(hdc)
			, m_hBitmap(nullptr)
			, m_hOldBitmap(nullptr)
			, m_hOldFont(nullptr)
		{
			// get size of bitmap to alloc.
			BITMAP bm = { 0 };
			int x, y;

			if (rc == nullptr) {
				// no size specified, use hdc's bitmap size.
				if (GetObject((HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP), sizeof(BITMAP), &bm) == 0)
					throw Dcx::dcxException("dcxHDCBuffer - Unable to get hdc's bitmap");
				x = 0;
				y = 0;
			}
			else {
				// use size specified.
				bm.bmWidth = (rc->right - rc->left);
				bm.bmHeight = (rc->bottom - rc->top);
				x = rc->left;
				y = rc->top;
			}

			// alloc bitmap for buffer.
			m_hBitmap = CreateCompatibleBitmap(hdc, bm.bmWidth, bm.bmHeight);

			if (m_hBitmap == nullptr)
				throw Dcx::dcxException("dcxHDCBuffer - Unable to create bitmap");
			
				// select bitmap into hdc
			m_hOldBitmap = SelectBitmap(*this, m_hBitmap);

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
			BitBlt(*this, 0, 0, bm.bmWidth, bm.bmHeight, hdc, x, y, SRCCOPY);

			// buffer is an exact duplicate of the hdc within the area specified.
		}
		~dcxHDCBuffer()
		{
			GdiFlush();
			if (*this != nullptr)
			{
				if (m_hOldFont != nullptr)
					SelectFont(*this, m_hOldFont);
				if (m_hOldBitmap != nullptr)
					SelectBitmap(*this, m_hOldBitmap);
				if (m_hBitmap != nullptr)
					DeleteBitmap(m_hBitmap);
			}
		}
	private:
		HBITMAP m_hOldBitmap;
		HBITMAP m_hBitmap;
		HFONT m_hOldFont;
	};

	using MapOfCursors = std::map<HCURSOR, HCURSOR>;
	using MapOfAreas = std::map<UINT, HCURSOR>;

	// example code to get a range (purely for testing)
	//template< typename T >
	//struct range_t
	//{
	//	struct iter
	//	{
	//		T operator * ()const noexcept { return n; }
	//		iter& operator ++()noexcept { ++n; return *this; }
	//		friend
	//			bool operator != (iter const& lhs, iter const& rhs)noexcept
	//		{
	//			return lhs.n != rhs.n;
	//		}
	//
	//		T n;
	//	};
	//
	//	iter begin()const noexcept { return{ b }; }
	//	iter end() const noexcept { return{ e }; }
	//	T b, e;
	//};
	//template< typename T > range_t<T>  range(T b, T e) { return{ b, e }; }

	void setupOSCompatibility(void);
	void freeOSCompatibility(void);

	IClassFactory* getClassFactory() noexcept;
	const TCHAR * getLastError() noexcept;
	const BYTE &getGhostDrag() noexcept;
	bool setGhostDrag(const BYTE newAlpha);
	const bool &isDX9Installed() noexcept;
	bool isUnloadSafe();
	bool isFile(const WCHAR *const file);
	bool isFile(LPCSTR const file);

	void load(LOADINFO *const lInfo);
	void unload(void);
	const bool &initDirectX();
	const bool &initDirectX(TCHAR *dxResult, int dxSize);
	void error(const TCHAR *const cmd, const TCHAR *const msg);
	void errorex(const TCHAR *const cmd, const TCHAR *const szFormat, ...);
	int mark(TCHAR *const data, const TString & tsDName, const TString & tsCallbackName);
	LRESULT CALLBACK mIRCSubClassWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	const char *const dcxGetFormattedString(const TCHAR *const fmt, ...);
	const PTCHAR parseCursorType(const TString & cursor);
	const DWORD parseSystemCursorType(const TString & cursor);
	const DWORD parseAreaType(const TString &tsArea);
	HCURSOR dcxLoadCursor(const UINT iFlags, const PTCHAR CursorType, bool &bCursorFromFile, const HCURSOR oldCursor, TString &filename);
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
	RETURN_TYPE parse_string(const STRING_TYPE *str) {
		static_assert(std::is_same<WCHAR, STRING_TYPE>::value || std::is_same<CHAR, STRING_TYPE>::value, "Only WCHAR, CHAR, or TCHAR type supported!");

		std::basic_stringstream<STRING_TYPE> buf;
		buf << str;
		RETURN_TYPE val;
		buf >> val;
		return val;
	}

	namespace details {

		// Casts any numeric type as another numeric type.
		template < typename tResult, typename tInput >
		std::enable_if_t<is_Numeric<tInput>::value, inline constexpr tResult> numeric_cast(tInput &in) noexcept
		{
			static_assert(is_Numeric<tResult>::value, "A Numeric return type is required");
			return static_cast<tResult>(in);
		}

		// Casts any string as a number
		template < typename tResult, typename tInput >
		std::enable_if_t<!is_Numeric<tInput>::value && !std::is_convertible<tInput, tResult>::value, tResult> numeric_cast(tInput &in)
		{
			static_assert(is_Numeric<tResult>::value, "A Numeric return type is required");
			std::stringstream buf;
			buf << in;
			tResult result;
			buf >> result;
			return result;
		}

		// Converts any object to a numeric using implicit-conversion.
		template < typename tResult, typename tInput >
		std::enable_if_t<!is_Numeric<tInput>::value && std::is_convertible<tInput, tResult>::value, inline constexpr tResult> numeric_cast(tInput &in)
		{
			static_assert(is_Numeric<tResult>::value, "A Numeric return type is required");
			return in;
		}
	}

	// Converts any object to a numeric.
	template < typename tResult, typename tInput >
	inline constexpr tResult numeric_cast(tInput &in)
	{
		static_assert(is_Numeric<tResult>::value, "A Numeric return type is required");
		return details::numeric_cast<tResult>(in);
	}

	// Converts any pointer to a numeric.
	template < typename tResult, typename tInput, typename = std::enable_if_t<!std::is_same<std::remove_cv_t<tInput>,char>::value && !std::is_same<std::remove_cv_t<tInput>,wchar_t>::value> >
	inline constexpr tResult numeric_cast(tInput *in)
	{
		static_assert(is_Numeric<tResult>::value, "A Numeric return type is required");
		return reinterpret_cast<tResult>(in);
	}

	//// Converts any calculation to a numeric.
	//template < typename tResult, typename tInput, typename = std::enable_if_t<std::is_arithmetic<tInput>::value> >
	//inline constexpr tResult numeric_cast(tInput in)
	//{
	//	static_assert(is_Numeric<tResult>::value, "A Numeric return type is required");
	//	return static_cast<tResult>(in);
	//}

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
}
