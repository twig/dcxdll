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

	typedef std::unique_ptr<HICON__, void(*)(HICON__ *)> dcxCursor_t;
	typedef std::unique_ptr<HICON__, void(*)(HICON__ *)> dcxIcon_t;
	typedef std::unique_ptr<FILE, void(*)(FILE *)> dcxFile_t;
	typedef std::unique_ptr<HANDLE, void(*)(HANDLE)> dcxHandle_t; // <- error
	typedef std::unique_ptr<HDC__, void(*)(HDC)> dcxHDC_t;
	typedef std::unique_ptr<HBITMAP__, void(*)(HBITMAP)> dcxBitmap_t;

	inline dcxCursor_t make_cursor(TString &tsFilename) {
		return make_resource(dcxLoadCursorFromFile, [](HCURSOR hCursor){ if (hCursor != nullptr) DestroyCursor(hCursor); }, tsFilename);
	}
	inline dcxIcon_t make_icon(const int index, TString &filename, const bool large, const TString &flags) {
		return make_resource(dcxLoadIcon, [](HICON hIcon){ if (hIcon != nullptr) DestroyIcon(hIcon); }, index, filename, large, flags);
	}
	inline dcxFile_t make_file(const WCHAR *file, const WCHAR *modes) { return make_resource(_wfopen, [](FILE *file){ fclose(file); }, file, modes); }
	//enum class hdcTypes : UINT { CreateCompatiableDCType, CreateDCType };
	//inline dcxHDC_t make_hdc(hdcTypes hType, HDC hdc, LPCTSTR lpszDriver, LPCTSTR lpszDevice, LPCTSTR lpszOutput, const DEVMODE *lpInitData)
	//{
	//	switch (hType)
	//	{
	//	case hdcTypes::CreateCompatiableDCType:
	//		return make_resource(CreateCompatibleDC, [](HDC obj){ DeleteDC(obj); }, hdc);
	//	case hdcTypes::CreateDCType:
	//		return make_resource(CreateDC, [](HDC obj){ DeleteDC(obj); }, lpszDriver, lpszDevice, lpszOutput, lpInitData);
	//	}
	//}
	inline dcxHDC_t make_hdc(HDC hdc) { return make_resource(CreateCompatibleDC, [](HDC obj){ DeleteDC(obj); }, hdc); }
	inline dcxBitmap_t make_bitmap(HDC hdc, int x, int y) { return make_resource(CreateCompatibleBitmap, [](HBITMAP obj){ DeleteBitmap(obj); }, hdc, x, y); }

	//inline dcxHandle_t make_filehandle(const TCHAR *file, DWORD dAccess, DWORD dShareMode, LPSECURITY_ATTRIBUTES lpSecurity, DWORD dCreation, DWORD dflags, HANDLE templateFile) {
	//	return make_resource(CreateFile, [](HANDLE file){ CloseHandle(file); }, file, dAccess, dShareMode, lpSecurity, dCreation, dflags, templateFile);
	//}

	//template <typename obj, typename Creator, typename Destructor, typename... Arguments>
	//struct dcxResource
	//{
	//	typedef std::unique_ptr<obj, void(*)(obj *)> dcxObj_t;
	//	dcxResource(Creator c, Destructor d, Arguments &&... args)
	//		: m_uni(make_resource(c, d, args))
	//	{
	//		m_hObj = m_uni.get();
	//	}
	//	dcxResource &operator =(dcxResource &&other)
	//	{
	//		using std::swap;
	//
	//		swap(m_uni, other.m_uni);
	//		swap(m_hObj, other.m_hObj);
	//	}
	//	operator obj() { return m_hObj; }
	//	dcxObj_t	m_uni;
	//	obj			m_hObj;
	//};

	//template <typename T>
	//struct dcxTest : public dcxResource< FILE, _wfopen, [](FILE *file){ fclose(file); }, T>
	//{
	//	dcxTest(FILE *file)
	//		: dcxResource < FILE, _wfopen, [](FILE *file){ fclose(file); }, file>
	//	{
	//
	//	}
	//};

	//template <typename T>
	//struct dcxResource {
	//	operator T() { return m_hObj; }
	//protected:
	//	T		m_hObj;
	//};
	//struct dcxTest : dcxResource < FILE *>
	//{
	//	dcxTest(const TCHAR *tsFilename, const TCHAR *tsMode)
	//		: m_uni(make_file(tsFilename, tsMode))
	//	{
	//		m_hObj = m_uni.get();
	//	}
	//private:
	//	dcxFile_t	m_uni;
	//};

	struct dcxCursor
	{
		dcxCursor() = delete;
		dcxCursor(const dcxCursor &) = delete;
		dcxCursor &operator =(const dcxCursor &) = delete;	// No assignments!

		dcxCursor(TString &tsFilename)
			: m_uni(make_cursor(tsFilename))
		{
		}

		//dcxCursor &operator =(dcxCursor &&other)
		//{
		//	using std::swap;
		//
		//	swap(m_uni, other.m_uni);
		//}
		operator HCURSOR() { return m_uni.get(); }
	private:
		dcxCursor_t	m_uni;
	};

	struct dcxIcon
	{
		dcxIcon() = delete;
		dcxIcon(const dcxIcon &) = delete;
		dcxIcon &operator =(const dcxIcon &) = delete;	// No assignments!

		dcxIcon(const int index, TString &filename, const bool large, const TString &flags)
			: m_uni(make_icon(index, filename, large, flags))
		{
		}
		operator HICON() { return m_uni.get(); }

	private:
		dcxIcon_t	m_uni;
	};

	struct dcxFile
	{
		dcxFile() = delete;
		dcxFile(const dcxFile &) = delete;
		dcxFile &operator =(const dcxFile &) = delete;	// No assignments!

		dcxFile(const TCHAR *tsFilename, const TCHAR *tsMode)
			: m_uni(make_file(tsFilename, tsMode))
		{
		}
		operator FILE *() { return m_uni.get(); }

	private:
		dcxFile_t	m_uni;
	};

	struct dcxHDC
	{
		dcxHDC() = delete;
		dcxHDC(const dcxHDC &) = delete;
		dcxHDC &operator =(const dcxHDC &) = delete;	// No assignments!

		dcxHDC(HDC hdc)
			: m_uni(make_hdc(hdc))
		{
		}
		//dcxHDC(HDC hdc)
		//	: m_uni(make_hdc(hdcTypes::CreateCompatiableDCType, hdc, nullptr, nullptr, nullptr, nullptr))
		//{
		//}
		//dcxHDC(LPCTSTR lpszDriver, LPCTSTR lpszDevice, LPCTSTR lpszOutput, const DEVMODE *lpInitData)
		//	: m_uni(make_hdc(hdcTypes::CreateDCType, nullptr, lpszDriver, lpszDevice, lpszOutput, lpInitData))
		//{
		//}
		operator HDC() { return m_uni.get(); }

	private:
		dcxHDC_t	m_uni;
	};

	struct dcxBitmap
	{
		dcxBitmap() = delete;
		dcxBitmap(const dcxBitmap &) = delete;
		dcxBitmap &operator =(const dcxBitmap &) = delete;	// No assignments!

		dcxBitmap(HDC hdc, int x, int y)
			: m_uni(make_bitmap(hdc,x,y))
		{
		}
		operator HBITMAP() { return m_uni.get(); }

	private:
		dcxBitmap_t	m_uni;
	};

	typedef std::map<HCURSOR, HCURSOR> MapOfCursors;
	typedef std::map<UINT, HCURSOR> MapOfAreas;

	//template< typename T >
	//struct range_t
	//{
	//	struct iter
	//	{
	//		T operator * ()const noexcept{ return n; }
	//		iter& operator ++()noexcept{ ++n; return *this; }
	//			friend
	//			bool operator != (iter const& lhs, iter const& rhs)noexcept
	//		{ return lhs.n != rhs.n; }
	//
	//		T n;
	//	};
	//
	//	iter begin()const noexcept{ return{ b }; }
	//	iter end() const noexcept{ return{ e }; }
	//	T b, e;
	//};
	//template< typename T > range_t<T>  range(T b, T e){ return{ b, e }; }

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

	typedef HCURSOR(WINAPI *PFNSETCURSOR)(HCURSOR hCursor);

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
