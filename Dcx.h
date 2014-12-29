#pragma once
//#include "defines.h"
#include "mIRCLinker.h"
#include "DcxUXModule.h"
#include "DcxGDIModule.h"
#include "DcxDWMModule.h"
#include "Classes/xpopup/XPopupMenuManager.h"
#include "Classes/xpopup/xmenubar.h"
#include "Classes/DcxDialog.h"
#include <sys/stat.h> 

// is it just me or is this class just a namespace?

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
	//template<typename Creator, typename Destructor, typename... Arguments>
	//auto make_resource(Creator c, Destructor d, Arguments&&... args)
	//{
	//	auto r = c(std::forward<Arguments>(args)...);
	//	if (!r) { throw std::runtime_error{ "Unable to create resource" }; }
	//	typedef typename std::decay<decltype(*r)>::type ResourceType;
	//	return std::unique_ptr<ResourceType, void(*)(ResourceType*)>(r, d);
	//}
	
	template<typename Creator, typename Destructor, typename... Arguments>
	auto make_resource(Creator c, Destructor d, Arguments&&... args)
		-> std::unique_ptr<typename std::decay<decltype(*c(std::forward<Arguments>(args)...))>::type, void(*)(typename std::decay<decltype(*c(std::forward<Arguments>(args)...))>::type*)>
	{
		auto r = c(std::forward<Arguments>(args)...);
		if (!r) { throw std::runtime_error{ "Unable to create resource" }; }
		typedef typename std::decay<decltype(*r)>::type ResourceType;
		return std::unique_ptr<ResourceType, void(*)(ResourceType*)>(r, d);
	}

	typedef std::unique_ptr<HICON__, void(*)(HICON__ *)> dcxCursor_t;
	typedef std::unique_ptr<HICON__, void(*)(HICON__ *)> dcxIcon_t;
	typedef std::unique_ptr<FILE, void(*)(FILE *)> dcxFile_t;
	typedef std::unique_ptr<HANDLE, void(*)(HANDLE)> dcxHandle_t;

	//inline dcxCursor_t make_cursor(TString &tsFilename) { return make_resource(dcxLoadCursorFromFile, dcxDestroyCursor, tsFilename); }
	//inline dcxIcon_t make_icon(const int index, TString &filename, const bool large, const TString &flags) { return make_resource(dcxLoadIcon, dcxDestroyIcon, index,filename,large,flags); }
	//inline dcxFile_t make_file(const WCHAR *file, const WCHAR *modes) { return make_resource(_wfopen, fclose, file, modes); }
	inline dcxCursor_t make_cursor(TString &tsFilename) {
		return make_resource(dcxLoadCursorFromFile, [](HCURSOR hCursor){ if (hCursor != nullptr) DestroyCursor(hCursor); }, tsFilename);
	}
	inline dcxIcon_t make_icon(const int index, TString &filename, const bool large, const TString &flags) {
		return make_resource(dcxLoadIcon, [](HICON hIcon){ if (hIcon != nullptr) DestroyIcon(hIcon); }, index, filename, large, flags);
	}
	inline dcxFile_t make_file(const WCHAR *file, const WCHAR *modes) { return make_resource(_wfopen, [](FILE *file){ fclose(file); }, file, modes); }
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
		dcxCursor(TString &tsFilename)
			: m_uni(make_cursor(tsFilename))
		{
			m_hObj = m_uni.get();
		}
		dcxCursor &operator =(dcxCursor &&other)
		{
			using std::swap;

			swap(m_uni, other.m_uni);
			swap(m_hObj, other.m_hObj);
		}
		operator HCURSOR() { return m_hObj; }
	private:
		dcxCursor_t	m_uni;
		HCURSOR		m_hObj;
	};
	struct dcxIcon
	{
		dcxIcon(const int index, TString &filename, const bool large, const TString &flags)
			: m_uni(make_icon(index, filename, large, flags))
		{
			m_hObj = m_uni.get();
		}
		dcxIcon &operator =(dcxIcon &&other)
		{
			using std::swap;

			swap(m_uni, other.m_uni);
			swap(m_hObj, other.m_hObj);
		}
		operator HICON() { return m_hObj; }

	private:
		dcxIcon_t	m_uni;
		HICON		m_hObj;
	};
	struct dcxFile
	{
		//dcxFile(TString &tsFilename, const TString &tsMode)
		//	: m_uni(make_file(tsFilename.to_chr(), tsMode.to_chr()))
		//{
		//	m_File = m_uni.get();
		//}
		dcxFile(const TCHAR *tsFilename, const TCHAR *tsMode)
			: m_uni(make_file(tsFilename, tsMode))
		{
			m_hObj = m_uni.get();
		}
		dcxFile &operator =(dcxFile &&other)
		{
			using std::swap;

			swap(m_uni, other.m_uni);
			swap(m_hObj, other.m_hObj);
		}
		operator FILE *() { return m_hObj; }

	private:
		dcxFile_t	m_uni;
		FILE		*m_hObj;
	};

	//struct dcxFileHandle
	//{
	//	dcxFileHandle(const TCHAR *tsFilename, const TCHAR *tsMode)
	//		: m_uni(make_file(tsFilename, tsMode))
	//	{
	//		m_hObj = m_uni.get();
	//	}
	//	dcxFileHandle &operator =(dcxFileHandle &&other)
	//	{
	//		using std::swap;

	//		swap(m_uni, other.m_uni);
	//		swap(m_hObj, other.m_hObj);
	//	}
	//	operator FILE *() { return m_hObj; }

	//private:
	//	dcxFile_t	m_uni;
	//	FILE		*m_hObj;
	//};

	typedef struct tagCursor_Data {
		HCURSOR		m_hCursor;
		bool		m_bNoDestroy;
	} Cursor_Data, *LPCursor_Data;
	
	//struct Cursor_Data {
	//	Cursor_Data() : m_hCursor(nullptr) {}
	//	Cursor_Data(TString &tsFilename)
	//		: m_hCursor(dcxLoadCursorFromFile(tsFilename))
	//	{

	//	}
	//	~Cursor_Data()
	//	{
	//		if (m_hCursor != nullptr)
	//			DestroyCursor(m_hCursor);
	//	}
	//	
	//	HCURSOR		m_hCursor;
	//};

	typedef std::map<UINT, Cursor_Data> MapOfCursors;
	
	extern MapOfCursors	m_vMapOfCursors;

	void setupOSCompatibility(void);
	void freeOSCompatibility(void);

	IClassFactory* getClassFactory();
	const TCHAR * getLastError();
	const BYTE &getGhostDrag();
	bool setGhostDrag(const BYTE newAlpha);
	const bool &isDX9Installed();
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
	const DWORD parseSystemCursorType(const TString & cursor);
	const DWORD parseAreaType(const TString &tsArea);
	HCURSOR dcxLoadCursor(const UINT iFlags, const PTCHAR CursorType, bool &bCursorFromFile, const HCURSOR oldCursor, TString &filename);
	HCURSOR getCursor(const UINT iType);
	void setCursor(const Cursor_Data &hCursor, const UINT iType);
	void deleteCursor(const UINT iType);

	// parse_string, taken from somewhere on stackoverflow (sorry can't remember where) & modified for our use.
	template <typename RETURN_TYPE, typename STRING_TYPE>
	RETURN_TYPE parse_string(const STRING_TYPE *str) {
		static_assert(std::is_same<WCHAR, std::remove_pointer_t<STRING_TYPE>>::value || std::is_same<CHAR, std::remove_pointer_t<STRING_TYPE>>::value, "Only WCHAR, CHAR, or TCHAR type supported!");

		std::basic_stringstream<std::remove_pointer_t<STRING_TYPE>> buf;
		buf << str;
		RETURN_TYPE val;
		buf >> val;
		return val;
	}
}
