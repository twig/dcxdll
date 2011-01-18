/*!
 * \file defines.h
 * \brief Definition Support File
 *
 * This file contains constant, alias and variable type defintions.
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 *
 * © ScriptsDB.org - 2006
 */

/*
Some useful values for _MSC_VER if you need to target to a specific compiler.
http://symbiancorner.blogspot.com/2007/05/how-to-detect-version-of-ms-visual.html

#if _MSC_VER >= 1500 // Visual C++ 2008
#elif _MSC_VER >= 1400 // Visual C++ 2005
#elif _MSC_VER >= 1310 // Visual C++ .NET 2003
#elif _MSC_VER > 1300 // Visual C++ .NET 2002
#endif
*/
#if _MSC_VER > 1000
#pragma once
#endif
#ifndef _DEFINES_H_
#define _DEFINES_H_

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( disable : 1195 )
#pragma warning( disable : 504 )
#pragma warning( disable : 1563 )
#pragma warning( disable : 869 )	// remark #869: parameter "doc" was never referenced
#pragma warning( disable : 981 )	// remark #981: operands are evaluated in unspecified order
#pragma warning( disable : 1419 )	// remark #1419: external declaration in primary source file
#pragma warning( disable : 1418 )	// remark #1418: external function definition with no prior declaration http://software.intel.com/en-us/articles/cdiag1418/
#pragma warning( disable : 383 )	// remark #383: value copied to temporary, reference to temporary used
#pragma warning( disable : 2292 )	// warning #2292: destructor is declared but copy constructor and assignment operator are not
#pragma warning( disable : 654 )	// warning #654: overloaded virtual function "DcxControl::toXml" is only partially overridden in class
#pragma warning( disable : 444 )	// remark #444: destructor for base class "DWebBrowserEvents2" (declared at line 1948 of "C:\Program Files\Microsoft SDKs\Windows\v7.0\\include\exdisp.h") is not virtual
// intel compiler has problems with .def file
#define _INTEL_DLL_ __declspec(dllexport)
#else
#pragma warning( disable : 4100 ) // unreferenced formal parameter
#pragma warning( disable : 4530 )
#pragma warning( disable : 4995 ) // name was marked as #pragma deprecated
#define _INTEL_DLL_
#endif

// --------------------------------------------------
// Optional build libraries for DCX
// --------------------------------------------------
// DCX using DirectX SDK? (Required for DirectShow)
// If not, get off your arse & install it!
#define DCX_USE_DXSDK 1
// end of DirectX SDK

// DCX using GDI+? (Required for advanced graphics routines)
#define DCX_USE_GDIPLUS 1
#define DCX_MAX_GDI_ERRORS 21
// end of GDI+

// DCX using Windows SDK? (Required for Vista routines)
// If not, get off your arse & install it!
#define DCX_USE_WINSDK 1
// end of Windows SDK

// DCX Using the Boost C++ libs
// Boost is used for the regex matches when enabled.
//#define DCX_USE_BOOST 1
// end of Boost

// DCX Using the pcre lib
// PCRE is used for the regex pattern matching
// NB: CANT BE USED WITH BOOST ENABLED
//#define DCX_USE_PCRE 1
#define PCRE_STATIC 1
// end of PCRE

#ifdef DCX_USE_BOOST
#undef DCX_USE_PCRE
#endif

#if defined(DCX_USE_PCRE)
#pragma comment(lib, "pcre.lib")
#endif

// --------------------------------------------------
// Some compiler/library definitions
// --------------------------------------------------
//#define WIN32_LEAN_AND_MEAN //!< blah

// Windows 98 + IE V5.01 + GDI+ 1.0
//#define _WIN32_WINDOWS 0x0410
//#define _WIN32_WINNT 0x0410
//#define _WIN32_IE 0x0501
//#define WINVER 0x0410
//#define GDIPVER 0x0100

// Windows XP + IE V5.01 + GDI+ 1.0
//#define _WIN32_WINNT 0x0501
//#define _WIN32_IE 0x0501
//#define WINVER 0x0501
//#define GDIPVER 0x0100

// Windows XP SP1 + IE V6 + GDI+ 1.0
//#define _WIN32_WINNT 0x0502
//#define _WIN32_IE 0x0600
//#define WINVER 0x0502
//#define GDIPVER 0x0100

// Windows XP SP2 + IE V6 + GDI+ 1.1
//#define _WIN32_WINNT 0x0503
//#define _WIN32_IE 0x0600
//#define WINVER 0x0503
//#define GDIPVER 0x0110

// Windows Vista + IE V7 + GDI+ 1.1
//#define _WIN32_WINNT 0x0600
//#define _WIN32_IE 0x0700
//#define WINVER 0x0600
//#define GDIPVER 0x0110

// Windows 7 + IE V8 + GDI+ 1.1
#define _WIN32_WINNT 0x0601
#define _WIN32_IE 0x0800
#define WINVER 0x0601
#define GDIPVER 0x0110

// Required for VS 2005
#if _MSC_VER >= 1400
#define VS2005 1
#define _CRT_SECURE_NO_DEPRECATE 1
#endif
// end of VS 2005

// --------------------------------------------------
// DCX build version
// --------------------------------------------------
#define DLL_VERSION    2
#define DLL_SUBVERSION 0
#define DLL_BUILD      0
#define DLL_DEV_BUILD  4

#ifdef NDEBUG
#ifdef DCX_DEV_BUILD
// Dev Build, enable debug output.
#define DCX_DEBUG_OUTPUT 1
#define DCX_DEBUG(x,y,z) x((y), (z));
#define DLL_STATE      TEXT("Development Build")
// Link with DirectX error lib, enables extra error reporting.
#define DCX_DX_ERR	1
#else
// Release Build, disable debug info.
#define DCX_DEBUG_OUTPUT 0
#define DCX_DEBUG(x,y,z)
#define DLL_STATE      TEXT("Release Build")
#define _SECURE_SCL 0 // disables checked iterators
#endif
#else
// Debug Build, enable debug output.
#define DCX_DEBUG_OUTPUT 1
#define DCX_DEBUG(x,y,z) x((y), (z));
#define DLL_STATE      TEXT("Debug Build")
// Link with DirectX error lib, enables extra error reporting.
#define DCX_DX_ERR	1
#endif

// --------------------------------------------------
// Include files
// --------------------------------------------------
#define INITGUID
#define NOCOMM
#define NOSOUND
#define NOKANJI
#define NOHELP
#define NOMCX
//#include <vld.h>
#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <commctrl.h>

#include "Classes/TString/tstring.h"
#include "XSwitchFlags.h"

// BrowseFolder
#include <shlobj.h>

#include <uxtheme.h>
//#if _MSC_VER >= 1500 // Visual C++ 2008 (WRONG not for VS2008, for Windows SDK when compiled for Vista+)
#if DCX_USE_WINSDK && WINVER >= 0x600
#include <vssym32.h>
#else
#include <tmschema.h>
#endif

#if DCX_USE_WINSDK
#include <dwmapi.h>
#else
typedef enum _DWMWINDOWATTRIBUTE {
    DWMWA_NCRENDERING_ENABLED = 1,
    DWMWA_NCRENDERING_POLICY,
    DWMWA_TRANSITIONS_FORCEDISABLED,
    DWMWA_ALLOW_NCPAINT,
    DWMWA_CAPTION_BUTTON_BOUNDS,
    DWMWA_NONCLIENT_RTL_LAYOUT,
    DWMWA_FORCE_ICONIC_REPRESENTATION,
    DWMWA_FLIP3D_POLICY,
    DWMWA_EXTENDED_FRAME_BOUNDS,
    DWMWA_LAST
} DWMWINDOWATTRIBUTE;
#endif

#ifdef DCX_USE_GDIPLUS
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")
#endif

#ifdef DCX_USE_DXSDK
#ifdef DCX_DX_ERR
#include <Dxerr.h>
#pragma comment(lib, "DxErr.lib")
#define DX_ERR(prop,cmd,hr) this->showErrorEx((prop), (cmd), TEXT("%s: %s"), DXGetErrorString((hr)), DXGetErrorDescription((hr)))
#else
#define DX_ERR(prop,cmd,hr)
#endif
#endif

#include "classes/dcxdialogcollection.h"

#include "AggressiveOptimize.h"


// Win2000+ stuff for Win98+ compat (only used during testing)
//#ifndef ICC_STANDARD_CLASSES
//#define ICC_STANDARD_CLASSES		0x00004000
//#endif
//#ifndef ICC_LINK_CLASS
//#define ICC_LINK_CLASS					0x00008000
//#endif
//#ifndef WS_EX_COMPOSITED
//#define WS_EX_COMPOSITED        0x02000000L
//#endif
//#ifndef WS_EX_NOACTIVATE
//#define WS_EX_NOACTIVATE        0x08000000L
//#endif
//#ifndef WS_EX_LAYERED
//#define WS_EX_LAYERED           0x00080000
//#endif
//#ifndef PWINDOWINFO
//#define PWINDOWINFO LPVOID
//#endif
//#ifndef PFLASHWINFO
//#define PFLASHWINFO LPVOID
//#endif
#ifndef WM_DWMCOMPOSITIONCHANGED
#define WM_DWMCOMPOSITIONCHANGED 0x031E
#endif

// --------------------------------------------------
// Listview stuff
// --------------------------------------------------
#ifndef LPNMLVDISPINFO
#ifdef UNICODE
#define LPNMLVDISPINFO LPNMLVDISPINFOW
#else
#define LPNMLVDISPINFO LPNMLVDISPINFOA
#endif
#endif

// --------------------------------------------------
// DCX defines
// --------------------------------------------------
#define WM_MCOMMAND		(WM_USER + 200)
#define WM_MEVALUATE	(WM_USER + 201)
#define MIRCF_EDITBOX	1
#define MIRCF_CMD		2
#define MIRCF_FLOOD		4
#define MIRCF_UNICODE	8
// size of data buffer for mirc in characters!
#define MIRC_BUFFER_SIZE_CCH	4100
// size of same data buffer in bytes.
#define MIRC_BUFFER_SIZE_BYTES (MIRC_BUFFER_SIZE_CCH * sizeof(TCHAR))
// size of the mirc mapfile we will use in bytes.
#define MIRC_MAP_SIZE		(8192 * sizeof(TCHAR))
#define mIRC_ID_OFFSET 6000 //!< mIRC Dialog ID Offset

#define DCX_LISTVIEWCLASS    TEXT("DCXListViewClass")     //!< DCX Listview Class Name
#define DCX_PROGRESSBARCLASS TEXT("DCXProgressBarClass")  //!< DCX ProgressBar Class Name
#define DCX_TREEVIEWCLASS    TEXT("DCXTreeViewClass")     //!< DCX TreeView Class Name
#define DCX_TOOLBARCLASS     TEXT("DCXToolBarClass")      //!< DCX ToolBar Class Name
#define DCX_RICHEDITCLASS    TEXT("DCXRichEditClass")     //!< DCX RichEdit Class Name
#define DCX_TABCTRLCLASS     TEXT("DCXTabCtrlClass")      //!< DCX Tab Class Name
#define DCX_REBARCTRLCLASS   TEXT("DCXRebarCtrlClass")    //!< DCX Rebar Class Name
#define DCX_COMBOEXCLASS     TEXT("DCXComboExClass")      //!< DCX ComboEx Class Name
#define DCX_BUTTONCLASS      TEXT("DCXButtonClass")       //!< DCX Button Class Name
#define DCX_STATUSBARCLASS   TEXT("DCXStatusBarClass")    //!< DCX StatusBar Class Name
#define DCX_COLORCOMBOCLASS  TEXT("DCXColorComboClass")   //!< DCX ColorCombo Class Name
#define DCX_TRACKBARCLASS    TEXT("DCXTrackBarClass")     //!< DCX TrackBar Class Name
#define DCX_UPDOWNCLASS      TEXT("DCXUpDownClass")       //!< DCX Updown Class Name
#define DCX_IPADDRESSCLASS   TEXT("DCXIpAddressClass")    //!< DCX IP Address Class Name
#define DCX_DIVIDERCLASS     TEXT("DCXDividerClass")      //!< DCX Divider Class Name
#define DCX_PANELCLASS       TEXT("DCXPanelClass")        //!< DCX Panel Class Name
#define DCX_CALENDARCLASS    TEXT("DCXCalendarClass")     //!< DCX Panel Class Name
#define DCX_DATETIMECLASS    TEXT("DCXDateTimeClass")     //!< DCX DateTime Class Name
#define DCX_PAGERCLASS       TEXT("DCXCPagerClass")       //!< DCX Panel Class Name
#define DCX_BOXCLASS         TEXT("DCXBoxClass")          //!< DCX Box Class Name
//#define DCX_RADIOCLASS       TEXT("DCXRadioClass")        //!< DCX Radio Class Name
//#define DCX_CHECKCLASS       TEXT("DCXCheckClass")        //!< DCX Check Class Name
//#define DCX_SCROLLBARCLASS   TEXT("DCXScrollBarClass")    //!< DCX ScrollBar Class Name
#define DCX_SHADOWCLASS				TEXT("DCXShadowClass")			//!< DCX Shadow Class Name
#define DCX_VISTACLASS				TEXT("DCXVistaClass")				//!< DCX Vista Dialog Class Name
#define DCX_STACKERCLASS			TEXT("DCXStackerClass")			//!< DCX Stacker Class Name

// --------------------------------------------------
// CLA constants
// --------------------------------------------------
#define LAYOUTFIXED 0x01  //!< Layout Cell Fixed Type
#define LAYOUTFILL  0x02  //!< Layout Cell Fill Type
#define LAYOUTPANE  0x04  //!< Layout Cell Pane Type
#define LAYOUTID    0x08  //!< Layout Control ID is Valid
#define LAYOUTDIM   0x10  //!< Layout Control Dimensions are Valid (Only works with fixed)
#define LAYOUTVERT  0x20  //!< Layout Cell Vertical Style (LayoutCellPane and LayoutCellFixed)
#define LAYOUTHORZ  0x40  //!< Layout Cell Horizontal Style (LayoutCellPane and LayoutCellFixed)

// --------------------------------------------------
// DCX Font stuff
// --------------------------------------------------
#define DCF_ANTIALIASE    0x01 //!< Control Font Anti-Aliase Style
#define DCF_BOLD          0x02 //!< Control Font Bold Style
#define DCF_ITALIC        0x04 //!< Control Font Italic Style
#define DCF_STRIKEOUT     0x08 //!< Control Font Strikeout Style
#define DCF_UNDERLINE     0x10 //!< Control Font Underline Style
#define DCF_DEFAULT       0x20 //!< Control Font Default Style

// --------------------------------------------------
// XPopup stuff
// --------------------------------------------------
#define XPOPUPMENUCLASS TEXT("XPopupMenu32") //!< XPopupMenu Window Class Name

// --------------------------------------------------
// Ultradock stuff
// --------------------------------------------------
#define SWB_NONE    0
#define SWB_LEFT    1
#define SWB_RIGHT   2
#define SWB_TOP     3
#define SWB_BOTTOM  4

// Dialog info structure
typedef struct tagMYDCXWINDOW {
	RECT rc;
	DWORD old_styles;
	DWORD old_exstyles;
} MYDCXWINDOW,*LPMYDCXWINDOW;

// --------------------------------------------------
// DLL stuff
// --------------------------------------------------
// mIRC Function Alias
#define mIRC(x) _INTEL_DLL_ int WINAPI x(HWND mWnd, HWND aWnd, TCHAR * data, TCHAR * parms, BOOL, BOOL)

// Return String DLL Alias (data is limited to MIRC_BUFFER_SIZE_CCH)
#define ret(x) { lstrcpyn(data, (x), MIRC_BUFFER_SIZE_CCH); return 3; }

#define PACKVERSION(major,minor) MAKELONG(minor,major)

// mIRC DLL Loading Structure
typedef struct {
	DWORD  mVersion; //!< mIRC Version
	HWND   mHwnd;    //!< mIRC Hwnd 
	BOOL   mKeep;    //!< mIRC variable stating to keep DLL in memory
	BOOL   mUnicode; //!< mIRC V7+ unicode enabled dll.
} LOADINFO;

// mIRC Signal structure
typedef struct SIGNALSWITCH {
	bool xdock;
	bool xstatusbar;
	bool xtray;
} *LPSIGNALSWITCH;

typedef std::vector<int> VectorOfInts; //<! Vector of int

// UNICODE/ANSI wrappers
#if UNICODE
#define dcx_atoi(x) _wtoi(x)
#define dcx_atoi64(x) _wtoi64(x)
#define dcx_atof(x) _wtof(x)
#define dcx_fopen(x,y) _wfopen(x,y)
#else
#define dcx_atoi(x) atoi(x)
#define dcx_atoi64(x) _atoi64(x)
#define dcx_atof(x) _atof(x)
#define dcx_fopen(x,y) fopen(x,y)
#endif
// --------------------------------------------------
// DLL routines
// --------------------------------------------------

int round(const float x);
BOOL ParseCommandToLogfont(const TString& cmd, LPLOGFONT lf);
TString ParseLogfontToCommand(const LPLOGFONT lf);
UINT parseFontFlags(const TString &flags);
UINT parseFontCharSet(const TString &charset);

BYTE *readFile(const PTCHAR filename);
TString FileDialog(const TString & data, const TString &method, const HWND pWnd);

int CALLBACK BrowseFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
LPITEMIDLIST GetFolderFromCSIDL(const int nCsidl);

HWND GetHwndFromString(const TString &str);
HWND GetHwndFromString(const TCHAR *str);
HWND FindOwner(const TString & data, const HWND defaultWnd);
BOOL CopyToClipboard(const HWND owner, const TString & str);
HBITMAP dcxLoadBitmap(HBITMAP dest, TString &filename);
HICON dcxLoadIcon(const int index, TString &filename, const bool large, const TString &flags);
HICON CreateGrayscaleIcon(HICON hIcon);
HRGN BitmapRegion(HBITMAP hBitmap,COLORREF cTransparentColor,BOOL bIsTransparent);
bool ChangeHwndIcon(const HWND hwnd, const TString &flags, const int index, TString &filename);
bool AddFileIcons(HIMAGELIST himl, TString &filename, const bool bLarge, const int iIndex);
BOOL dcxGetWindowRect(HWND hWnd, LPRECT lpRect);
int dcxPickIconDlg(HWND hwnd, LPWSTR pszIconPath, UINT cchIconPath, int *piIconIndex);

SYSTEMTIME MircTimeToSystemTime(const long mircTime);
long SystemTimeToMircTime(const LPSYSTEMTIME pst);

void AddToolTipToolInfo(const HWND tiphwnd, const HWND ctrl);
void dcxDrawShadowText(HDC hdc, LPCWSTR pszText, UINT cch, const RECT *pRect, DWORD dwFlags, COLORREF crText, COLORREF crShadow, int ixOffset, int iyOffset);
#ifdef DCX_USE_GDIPLUS
const TCHAR *GetLastStatusStr(Status status);
#endif
bool IsFile(TString &filename);
#if UNICODE
void calcStrippedRect(HDC hdc, const TString &txt, const UINT style, LPRECT rc, const bool ignoreleft);
void mIRC_DrawText(HDC hdc, const TString &txt, const LPRECT rc, const UINT style, const bool shadow);
#else
void calcStrippedRect(HDC hdc, const TString &txt, const UINT style, LPRECT rc, const bool ignoreleft, const bool tryutf8);
void mIRC_DrawText(HDC hdc, const TString &txt, const LPRECT rc, const UINT style, const bool shadow, const bool tryutf8);
#endif
HDC *CreateHDCBuffer(HDC hdc, const LPRECT rc);
void DeleteHDCBuffer(HDC *hBuffer);
int TGetWindowText(HWND hwnd, TString &txt);
void FreeOSCompatibility(void);
BOOL isRegexMatch(const TCHAR *matchtext, const TCHAR *pattern);

// UltraDock
void RemStyles(HWND hwnd,int parm,long RemStyles);
void AddStyles(HWND hwnd,int parm,long AddStyles);
void InitUltraDock(void);
void CloseUltraDock(void);
int SwitchbarPos(const int type);
void UpdatemIRC(void);

// CustomDock
#ifndef NDEBUG
bool InitCustomDock(void);
#endif

// DirectX
HRESULT GetDXVersion( DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion );

extern SIGNALSWITCH dcxSignal;

#include "Dcx.h"

#endif // _DEFINES_H_
