/*!
 * \file defines.h
 * \brief Definition Support File
 *
 * This file contains constant, alias and variable type defintions.
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 *
 * � ScriptsDB.org - 2006
 */

 /*
 Some useful values for _MSC_VER if you need to target to a specific compiler.
 http://symbiancorner.blogspot.com/2007/05/how-to-detect-version-of-ms-visual.html

 #if _MSC_VER >= 1930 // Visual C++ 2022
 #elif _MSC_VER >= 1911 // Visual C++ 2017.3
 #elif _MSC_VER >= 1910 // Visual C++ 2017
 #elif _MSC_VER >= 1900 // Visual C++ 2015
 #elif _MSC_VER >= 1800 // Visual C++ 2013
 #elif _MSC_VER >= 1700 // Visual C++ 2012
 #elif _MSC_VER >= 1600 // Visual C++ 2010 ?
 #elif _MSC_VER >= 1500 // Visual C++ 2008
 #elif _MSC_VER >= 1400 // Visual C++ 2005
 #elif _MSC_VER >= 1310 // Visual C++ .NET 2003
 #elif _MSC_VER > 1300 // Visual C++ .NET 2002
 #endif
 */
#pragma once

#ifndef _DEFINES_H_
#define _DEFINES_H_

 // VS2019+ only
#if !defined(_MSC_FULL_VER) || _MSC_FULL_VER < 192930145
#error "This version of DCX needs Visual Studio 2019 16.11.14 or newer"
#endif

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( disable : 383 )	// remark #383: value copied to temporary, reference to temporary used
#pragma warning( disable : 444 )	// remark #444: destructor for base class "DWebBrowserEvents2" (declared at line 1948 of "C:\Program Files\Microsoft SDKs\Windows\v7.0\\include\exdisp.h") is not virtual
#pragma warning( disable : 504 )
#pragma warning( disable : 654 )	// warning #654: overloaded virtual function "DcxControl::toXml" is only partially overridden in class
#pragma warning( disable : 869 )	// remark #869: parameter "doc" was never referenced
#pragma warning( disable : 981 )	// remark #981: operands are evaluated in unspecified order
#pragma warning( disable : 1195 )
#pragma warning( disable : 1418 )	// remark #1418: external function definition with no prior declaration http://software.intel.com/en-us/articles/cdiag1418/
#pragma warning( disable : 1419 )	// remark #1419: external declaration in primary source file
#pragma warning( disable : 1563 )
#pragma warning( disable : 2292 )	// warning #2292: destructor is declared but copy constructor and assignment operator are not
// intel compiler has problems with .def file
#define _INTEL_DLL_ __declspec(dllexport)
#else
#pragma warning( disable : 4100 ) // unreferenced formal parameter
#pragma warning( disable : 4530 )
#pragma warning( disable : 4820 ) // 'x' bytes padding added after data member '....'
#pragma warning( disable : 4995 ) // name was marked as #pragma deprecated
#define _INTEL_DLL_ __declspec(dllexport)
#endif
#pragma warning( disable : 26412 ) //  warning C26412 : Do not dereference an invalid pointer(lifetimes rule 1). '....' was invalidated at line ??? by '......'.
#pragma warning( disable : 26413 ) //  warning C26413 : Do not dereference nullptr (lifetimes rule 2). '....' was pointed to nullptr at line ???.
#pragma warning( disable : 26415 ) //  warning C26415 : Smart pointer parameter '.....' is used only to access contained pointer.Use T* or T & instead(r.30: http://go.microsoft.com/fwlink/?linkid=845475).
#pragma warning( disable : 26417 ) //  warning C26417 : Shared pointer parameter '....' is passed by reference and not reset or reassigned.Use T* or T & instead(r.35: http://go.microsoft.com/fwlink/?linkid=845488).
#pragma warning( disable : 26418 ) //  warning C26418 : Shared pointer parameter '....' is not copied or moved.Use T* or T & instead(r.36: http://go.microsoft.com/fwlink/?linkid=845489).
#pragma warning( disable : 26425 ) //  warning C26425 : Assigning 'nullptr' to a static variable.
#pragma warning( disable : 26426 ) //  warning C26426 : Global initializer calls a non - constexpr function '....' (i.22: http://go.microsoft.com/fwlink/?linkid=853919).
#pragma warning( disable : 26429 ) //  warning C26429 : Symbol '...' is never tested for nullness, it can be marked as not_null(f.23: http://go.microsoft.com/fwlink/?linkid=853921).
#pragma warning( disable : 26461 ) //  Warning C26461 : The pointer argument '...' for function '...' can be marked as a pointer to const (con.3).
#pragma warning( disable : 26462 ) //  Warning C26462 : The value pointed to by 'lpnmtb' is assigned only once, mark it as a pointer to const (con.4).
//#pragma warning( disable : 26472 ) //  warning C26472 : Don't use a static_cast for arithmetic conversions. Use brace initialization, gsl::narrow_cast or gsl::narow (type.1: http://go.microsoft.com/fwlink/p/?LinkID=620417).
#pragma warning( disable : 26481 ) //  warning C26481 : Don't use pointer arithmetic. Use span instead. (bounds.1: http://go.microsoft.com/fwlink/p/?LinkID=620413)
#pragma warning( disable : 26482 ) //  warning C26482 : Only index into arrays using constant expressions. (bounds.2: http://go.microsoft.com/fwlink/p/?LinkID=620414)
#pragma warning( disable : 26486 ) //  Warning C26486 : Don't pass a pointer that may be invalid to a function. Parameter 0 '(*hdc)' in call to 'DcxUXModule::dcxBeginBufferedPaint' may be invalid (lifetime.3).
#pragma warning( disable : 26489 ) //  Warning C26489 : Don't dereference a pointer that may be invalid: '....'. '....' may have been invalidated at line ... (lifetime.1).
#pragma warning( disable : 26490 ) //  warning C26490 : Don't use reinterpret_cast (type.1: http://go.microsoft.com/fwlink/p/?LinkID=620417).
#pragma warning( disable : 26499 ) //  warning C26499 : Could not find any lifetime tracking information for '........'
#pragma warning( disable : 26458 ) //  Warning	C26458	Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
#pragma warning( disable : 26446 ) //  Warning	C26446	Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).

#pragma warning( disable : 5105) // warning C5105: macro expansion producing 'defined' has undefined behavior

// --------------------------------------------------
// Optional build libraries for DCX
// --------------------------------------------------
// DCX using DirectX SDK? (Required for DirectShow)
// This is included as part of the win10 sdk with vs2017 no separate install needed.
#define DCX_USE_DXSDK 1

#if DCX_USE_DXSDK
#pragma message ("### DCX OPTION: Use DirectX SDK - Enabled")
#else
#pragma message ("### DCX OPTION: Use DirectX SDK - Disabled")
#endif
// end of DirectX SDK

// DCX using GDI+? (Required for advanced graphics routines) (replace with Direct2D ?)
#define DCX_USE_GDIPLUS 1
constexpr auto DCX_MAX_GDI_ERRORS = 21;

#if DCX_USE_GDIPLUS
#pragma message ("### DCX OPTION: Use GDI Plus - Enabled")
#else
#pragma message ("### DCX OPTION: Use GDI Plus - Disabled")
#endif
// end of GDI+

// DCX Using C++11 regex
// NB: Can't be used with either BOOST OR PCRE enabled.
#define DCX_USE_CREGEX 0

// DCX Using PCRE2 regex
// NB: Can't be used with either BOOST OR C++11 regex enabled.
#ifdef _M_IX86
#define DCX_USE_PCRE2 1
#else
#define DCX_USE_PCRE2 0
#endif

#if DCX_USE_CREGEX
#pragma message ("### DCX OPTION: Use C++11 Regex - Enabled")
#else
#if DCX_USE_PCRE2
#pragma message ("### DCX OPTION: Use PCRE2 Regex - Enabled")
#else
#pragma message ("### DCX OPTION: Use mIRC Regex - Enabled")
#endif
#endif
// end of C++11 regex

// --------------------------------------------------
// Some compiler/library definitions
// --------------------------------------------------
//#define WIN32_LEAN_AND_MEAN //!< blah
//
// Windows 98 + IE V5.01 + GDI+ 1.0
//#define _WIN32_WINDOWS 0x0410
//#define _WIN32_WINNT 0x0410
//#define _WIN32_IE 0x0501
//#define WINVER 0x0410
//#define GDIPVER 0x0100
//
// Windows XP + IE V5.01 + GDI+ 1.0
//#define _WIN32_WINNT 0x0501
//#define _WIN32_IE 0x0501
//#define WINVER 0x0501
//#define GDIPVER 0x0100
//
// Windows XP SP1 + IE V6 + GDI+ 1.0
//#define _WIN32_WINNT 0x0502
//#define _WIN32_IE 0x0600
//#define WINVER 0x0502
//#define GDIPVER 0x0100
//
// Windows XP SP2 + IE V6 + GDI+ 1.1
//#define _WIN32_WINNT 0x0503
//#define _WIN32_IE 0x0600
//#define WINVER 0x0503
//#define GDIPVER 0x0110
//
// Windows Vista + IE V7 + GDI+ 1.1
//#define _WIN32_WINNT 0x0600
//#define _WIN32_IE 0x0700
//#define WINVER 0x0600
//#define GDIPVER 0x0110
//
// Windows 7 + IE V8 + GDI+ 1.1
//#define _WIN32_WINNT 0x0601
//#define _WIN32_IE 0x0800
//#define WINVER 0x0601
//#define GDIPVER 0x0110
//
// Windows 7 + IE V10 + GDI+ 1.1
//#define _WIN32_WINNT 0x0601
//#define _WIN32_IE 0x0A00
//#define WINVER 0x0601
//#define GDIPVER 0x0110
//
// Windows 8 + IE V10 + GDI+ 1.1
//#define _WIN32_WINNT 0x0602
//#define _WIN32_IE 0x0A00
//#define WINVER 0x0602
//#define GDIPVER 0x0110
//
// Windows 8.1 + IE V10 + GDI+ 1.1
//#define _WIN32_WINNT 0x0603
//#define _WIN32_IE 0x0A00
//#define WINVER 0x0603
//#define GDIPVER 0x0110

// Windows 10 + IE V11 + GDI+ 1.1
#define _WIN32_WINNT 0x0A00
#define _WIN32_IE 0x0A00
#define WINVER 0x0A00
#define GDIPVER 0x0110

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_SECURE_NO_WARNINGS 1
#define _SCL_SECURE_NO_WARNINGS 1
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING 1

#define _CRT_RAND_S 1

// Includes created git build info header...
#if !__has_include("gitBuild.h")
#error "gitBuild.h Missing! SourceControl project hasn't been run?"
#endif
#include "gitBuild.h"

// --------------------------------------------------
// DCX build version
// --------------------------------------------------
#define DLL_VERSION    GIT_DESCRIBE
#define DLL_BUILD      GIT_HASH
#define DLL_DEV_BUILD  "71"
#define DCXML_VERSION			1
#define DCXML_DIALOG_VERSION	2
#define DCXML_POPUP_VERSION		2

#ifdef NDEBUG
#ifdef DCX_DEV_BUILD
// Dev Build, enable debug output.
#define DCX_DEBUG_OUTPUT 1
#define DCX_DEBUG(x,y,z) x((y), (z));
#define DCX_DEBUG_ENSURES(x) Ensures((x))
#define DLL_STATE      "Development Build"
// Link with DirectX error lib, enables extra error reporting.
#define DCX_DX_ERR	1
// use test code?
#define DCX_USE_TESTCODE 1
#else
// Release Build, disable debug info.
#define DCX_DEBUG_OUTPUT 0
#define DCX_DEBUG(x,y,z)
#define DCX_DEBUG_ENSURES(x)
#define DLL_STATE      "Release Build"
#define _SECURE_SCL 0 // disables checked iterators
#endif
#else
// Debug Build, enable debug output.
#define DCX_DEBUG_OUTPUT 1
#define DCX_DEBUG(x,y,z) x((y), (z));
#define DCX_DEBUG_ENSURES(x) Ensures((x))
#define DLL_STATE      "Debug Build"
// Link with DirectX error lib, enables extra error reporting.
#define DCX_DX_ERR	1
// Use Object switch code (testing only)
#define DCX_SWITCH_OBJ 0
// use test code?
#define DCX_USE_TESTCODE 1
#endif

#if DCX_SWITCH_OBJ
#if !__has_include("SwitchObjects.h")
#error "Switch Objects enabled: "SwitchObjects.h" Required!"
#endif
#endif

// enable this to enable custom menus
#define DCX_CUSTOM_MENUS 1

// --------------------------------------------------
// Include files
// --------------------------------------------------
#define INITGUID
#define NOCOMM
#define NOSOUND
#define NOKANJI
#define NOHELP
#define NOMCX
#define OEMRESOURCE
// Don't use NOMINMAX as this causes gdiplus issues.
//#define NOMINMAX

#pragma warning(push)
#pragma warning(disable: 4668)	//is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#include <windows.h>
#include <windowsx.h>
#pragma warning(pop)
#include <concepts>
#include <ranges>
#include <vector>
#include <map>
#include <unordered_map>
#include <commctrl.h>
#include <richedit.h>
#include <cguid.h>

#include <memory>
#include <filesystem>
#include <optional>

// BrowseFolder
#include <shlobj.h>

#include <uxtheme.h>

#if WINVER >= 0x600
#include <vssym32.h>
#else
#include <tmschema.h>
#endif

#include <dwmapi.h>

// GDIPlus support
#ifdef DCX_USE_GDIPLUS
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#endif

// DirectX Support
#ifdef DCX_USE_DXSDK
#include <dshow.h>
#include <initguid.h>
#include <qnetwork.h>
#include <d3d9.h>
#include <vmr9.h>
#include <evr.h>
#if __has_include(<dxerr/dxerr.h>)
#ifdef DCX_DX_ERR
#include <dxerr/dxerr.h>
#pragma comment(lib, "DxErr.lib")
#define DX_ERR(prop,cmd,hr) showError((prop), (cmd), TEXT("%: %"), DXGetErrorString((hr)), DXGetErrorDescription((hr)))
#else
#define DX_ERR(prop,cmd,hr)
#endif
#else
#if !__has_include(<Dxsdkver.h>)
#error "DirectX SDK Required!"
#endif
#include <Dxsdkver.h>
#ifdef DCX_DX_ERR
#include <Dxerr.h>
#pragma comment(lib, "DxErr.lib")
//#define DX_ERR(prop,cmd,hr) showErrorEx((prop), (cmd), TEXT("%s: %s"), DXGetErrorString((hr)), DXGetErrorDescription((hr)))
#define DX_ERR(prop,cmd,hr) showError((prop), (cmd), TEXT("%: %"), DXGetErrorString((hr)), DXGetErrorDescription((hr)))
#else
#define DX_ERR(prop,cmd,hr)
#endif
#endif
#endif

// min/max() macros causes issues with GSL, just use std::max/min instead
#undef max
#undef min
#if !__has_include(<GSL\gsl>)
#error "Microsoft GSL Required!"
#endif
#include <GSL\gsl>

namespace gsl
{
	template <class Cont>
	GSL_SUPPRESS(type.1)
		GSL_SUPPRESS(bounds.4)
		GSL_SUPPRESS(r.32)
		constexpr auto at(const std::unique_ptr<Cont[]>& cont, const index i) -> decltype(cont[0])
	{
		//Expects(i >= 0 && i < narrow_cast<index>(cont.size()));
		//using size_type = decltype(cont.size());
		//return cont[static_cast<size_type>(i)];

		return cont[static_cast<std::size_t>(i)];
	}
}

// std::is_pod is removed in C++20
namespace Dcx
{
	template<class _Ty>
	struct is_pod
		: std::integral_constant<bool, std::is_standard_layout_v<_Ty>&& std::is_trivial_v<_Ty> >
	{
	};

	template <typename T>
	constexpr bool is_pod_v = is_pod<T>::value;
}

#define UMAX_PATH static_cast<size_t>(MAX_PATH)

#include "AutoRelease.h"

#include "Classes/TString/tstring.h"
#include "simpleString.h"
#include "refString.h"
#include "Classes/tstring/hashString.h"

#include "XSwitchFlags.h"

#include "classes/dcxdialogcollection.h"
#include "Classes\tinyxml\tinyxml.h"

#include "dcxExceptions.h"
#include "Classes\WindowStyles.h"

#include "json.h"

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
#define LPNMLVDISPINFO LPNMLVDISPINFOW
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
// causes mIRC 7.33Beta+ to return more detailed error information
#define MIRCF_ENHANCEDERRORS	16
// stand alone flag used by mIRC 7.33Beta+ to get info about mIRC
#define MIRCF_INFO		-1
// mIRC v7.33Beta+ SendMessage() enhanced error codes
//The return values are: 0 = success, 1 = failure or'd with 2 = bad mapfile name, 4 = bad mapfile size, 8 = bad eventid, 16 = bad server, 32 = bad script, 64 = SendMessage() support disabled.
#define MIRCF_ERR_FAILED		1
#define MIRCF_ERR_MAP_NAME		2
#define MIRCF_ERR_MAP_SIZE		4
#define MIRCF_ERR_EVENTID		8
#define MIRCF_ERR_SERVER		16
#define MIRCF_ERR_SCRIPT		32
#define MIRCF_ERR_DISABLED		64

enum class mIRC_SendMessage_Flags
	: UINT
{
	EDITBOX = 1,
	CMD = 2,
	FLOOD = 4,
	UNI_CODE = 8,
	ENHANCEDERRORS = 16 // causes mIRC 7.33Beta+ to return more detailed error information
};

enum class mIRC_SendMessage_ErrorCodes
	: UINT
{
	OK,
	ERR_FAILED = 1,
	ERR_MAP_NAME = 2,
	ERR_MAP_SIZE = 4,
	ERR_EVENTID = 8,
	ERR_SERVER = 16,
	ERR_SCRIPT = 32,
	ERR_DISABLED = 64
};

// size of data buffer for mirc in characters!
#define MIRC_BUFFER_SIZE_CCH	4100
// size of same data buffer in bytes.
#define MIRC_BUFFER_SIZE_BYTES (MIRC_BUFFER_SIZE_CCH * sizeof(TCHAR))
// size of the mirc mapfile we will use in bytes.
#define MIRC_MAP_SIZE		(8192 * sizeof(TCHAR))
#define mIRC_ID_OFFSET 6000U //!< mIRC Dialog ID Offset
#define mIRC_ID_MAX (UINT_MAX -1)	//!< Highest ID allowed.
#define mIRC_MAX_CONTROLS	10000U	//!< Max number of controls allowed per dialog.
constexpr auto mIRC_PALETTE_SIZE = 100U;	// Number of colours in mIRC's palette (mIRC v7.51.212+ support 100 colours, 0 - 15 same as old mirc, 16-99 new colour palette)

#define DCX_LISTVIEWCLASS       TEXT("DCXListViewClass")     //!< DCX Listview Class Name
#define DCX_PROGRESSBARCLASS    TEXT("DCXProgressBarClass")  //!< DCX ProgressBar Class Name
#define DCX_TREEVIEWCLASS       TEXT("DCXTreeViewClass")     //!< DCX TreeView Class Name
#define DCX_TOOLBARCLASS        TEXT("DCXToolBarClass")      //!< DCX ToolBar Class Name
#define DCX_RICHEDITCLASS       TEXT("DCXRichEditClass")     //!< DCX RichEdit Class Name
#define DCX_TABCTRLCLASS        TEXT("DCXTabCtrlClass")      //!< DCX Tab Class Name
#define DCX_REBARCTRLCLASS      TEXT("DCXRebarCtrlClass")    //!< DCX Rebar Class Name
#define DCX_COMBOEXCLASS        TEXT("DCXComboExClass")      //!< DCX ComboEx Class Name
#define DCX_BUTTONCLASS         TEXT("DCXButtonClass")       //!< DCX Button Class Name
#define DCX_STATUSBARCLASS      TEXT("DCXStatusBarClass")    //!< DCX StatusBar Class Name
#define DCX_COLORCOMBOCLASS     TEXT("DCXColorComboClass")   //!< DCX ColorCombo Class Name
#define DCX_TRACKBARCLASS       TEXT("DCXTrackBarClass")     //!< DCX TrackBar Class Name
#define DCX_UPDOWNCLASS         TEXT("DCXUpDownClass")       //!< DCX Updown Class Name
#define DCX_IPADDRESSCLASS      TEXT("DCXIpAddressClass")    //!< DCX IP Address Class Name
#define DCX_DIVIDERCLASS        TEXT("DCXDividerClass")      //!< DCX Divider Class Name
#define DCX_PANELCLASS          TEXT("DCXPanelClass")        //!< DCX Panel Class Name
#define DCX_CALENDARCLASS       TEXT("DCXCalendarClass")     //!< DCX Panel Class Name
#define DCX_DATETIMECLASS		TEXT("DCXDateTimeClass")     //!< DCX DateTime Class Name
#define DCX_PAGERCLASS			TEXT("DCXPagerClass")        //!< DCX Panel Class Name
#define DCX_BOXCLASS			TEXT("DCXBoxClass")          //!< DCX Box Class Name
#define DCX_RADIOCLASS			TEXT("DCXRadioClass")        //!< DCX Radio Class Name
#define DCX_CHECKCLASS			TEXT("DCXCheckClass")        //!< DCX Check Class Name
#define DCX_SCROLLBARCLASS		TEXT("DCXScrollBarClass")    //!< DCX ScrollBar Class Name
//#define DCX_SHADOWCLASS				TEXT("DCXShadowClass")			//!< DCX Shadow Class Name
#define DCX_VISTACLASS			TEXT("DCXVistaClass")        //!< DCX Vista Dialog Class Name
#define DCX_STACKERCLASS		TEXT("DCXStackerClass")      //!< DCX Stacker Class Name
#define DCX_IMAGECLASS			TEXT("DCXImageClass")        //!< DCX Image Class Name
#define DCX_EDITCLASS			TEXT("DCXEditClass")         //!< DCX Edit Class Name
#define DCX_WEBCLASS			TEXT("DCXWebClass")          //!< DCX Web Class Name
#define DCX_WEB2CLASS			TEXT("DCXWeb2Class")         //!< DCX Web2 Class Name
#define DCX_LINECLASS			TEXT("DCXLineClass")         //!< DCX Line Class Name
#define DCX_LINKCLASS			TEXT("DCXLinkClass")         //!< DCX Link Class Name
#define DCX_LISTCLASS			TEXT("DCXListClass")         //!< DCX List Class Name
//#define DCX_SCROLLCLASS			TEXT("DCXScrollClass")    //!< DCX Text Class Name
#define DCX_TEXTCLASS			TEXT("DCXTextClass")         //!< DCX Text Class Name
#define DCX_DIRECTSHOWCLASS		TEXT("DCXDirectShowClass")   //!< DCX Text Class Name
#define DCX_MULTIBUTTONCLASS    TEXT("DCXMultiButtonClass")  //!< DCX MultiButton Class Name
#define DCX_MULTICOMBOCLASS		TEXT("DCXMultiComboClass")   //!< DCX MultiCombo Class Name
#define DCX_GRIDCLASS			TEXT("DCXGridClass")         //!< DCX Grid Class Name
//#define DCX_TOOLTIPCLASS		TEXT("DCXToolTipClass")      //!< DCX Tooltip Class Name

using mIRCResultString = refString<TCHAR, MIRC_BUFFER_SIZE_CCH>;

// --------------------------------------------------
// CLA constants
// --------------------------------------------------
enum class CLATypes
	: UINT
{
	LAYOUTFIXED		= 0x001,  //!< Layout Cell Fixed Type
	LAYOUTFILL		= 0x002,  //!< Layout Cell Fill Type
	LAYOUTPANE		= 0x004,  //!< Layout Cell Pane Type
	LAYOUTID		= 0x008,  //!< Layout Control ID is Valid
	LAYOUTDIM		= 0x010,  //!< Layout Control Dimensions are Valid (Only works with LayoutCellFixed)
	LAYOUTVERT		= 0x020,  //!< Layout Cell Vertical Style (LayoutCellPane and LayoutCellFixed)
	LAYOUTHORZ		= 0x040,  //!< Layout Cell Horizontal Style (LayoutCellPane and LayoutCellFixed)
	LAYOUTFIXEDPOS	= 0x080,  //!< Layout Cell Fixed Position Style (LayoutCellPane and LayoutCellFixed)
	LAYOUTMIN		= 0x100
};


// --------------------------------------------------
// DCX Font stuff
// --------------------------------------------------
enum class dcxFontFlags
	: UINT
{
	DCF_ANTIALIASE = 0x01,	//!< Control Font Anti-Aliase Style
	DCF_BOLD = 0x02,		//!< Control Font Bold Style
	DCF_ITALIC = 0x04,		//!< Control Font Italic Style
	DCF_STRIKEOUT = 0x08,	//!< Control Font Strikeout Style
	DCF_UNDERLINE = 0x10,	//!< Control Font Underline Style
	DCF_DEFAULT = 0x20,		//!< Control Font Default Style
	DCF_LIGHT = 0x40		//!< Control Font Light Style (opposite of bold)
};

// --------------------------------------------------
// XPopup stuff
// --------------------------------------------------
#define XPOPUPMENUCLASS TEXT("XPopupMenu32") //!< XPopupMenu Window Class Name

// --------------------------------------------------
// Ultradock stuff
// --------------------------------------------------

enum class SwitchBarPos
	: UINT
{
	SWB_NONE = 0,
	SWB_LEFT,
	SWB_RIGHT,
	SWB_TOP,
	SWB_BOTTOM
};

// --------------------------------------------------
// DLL stuff
// --------------------------------------------------
// mIRC Function Alias
#define mIRC(x) _INTEL_DLL_ int WINAPI x(_In_ HWND mWnd, _In_ HWND aWnd, _Inout_z_ TCHAR *const data, _In_z_ const TCHAR *const parms, _In_ BOOL, _In_ BOOL)

// Return String DLL Alias (data is limited to mIRCLinker::m_dwBytes)
#define ret(x) { if (ts_strcpyn(data, (x), mIRCLinker::m_dwCharacters) == nullptr) data[0] = 0; return 3; }

#define PACKVERSION(major,minor) MAKELONG(minor,major)

// mIRC Signal structure
struct SIGNALSWITCH
{
	bool xdock{ false };
	bool xstatusbar{ true };
	bool xtray{ true };
};
using LPSIGNALSWITCH = SIGNALSWITCH*;

using VectorOfInts = std::vector<int>; //<! Vector of int

struct clrCheckBox
{
	COLORREF m_clrBackground{ RGB(255, 128, 0) };
	COLORREF m_clrFrame{};
	COLORREF m_clrTick{};
	COLORREF m_clrDisabledBackground{ RGB(200, 200, 200) };
	COLORREF m_clrDisabledFrame{ RGB(200, 200, 200) };
	COLORREF m_clrDisabledTick{ RGB(128, 128, 128) };
	COLORREF m_clrHotBackground{ RGB(255, 128, 123) };
	COLORREF m_clrHotFrame{ RGB(0,0,255) };
	COLORREF m_clrHotTick{ RGB(0,0,255) };
};

TString queryEvalAttribute(const TiXmlElement* element, const char* attribute, const char* defaultValue = "");
[[nodiscard("Memory Leak")]] HBITMAP dcxLoadBitmap(HBITMAP dest, TString& filename);

struct dcxImage
{
	TString m_tsFilename;
	HBITMAP m_hBitmap{};
	HICON m_hIcon{};
#ifdef DCX_USE_GDIPLUS
	Gdiplus::Image* m_pImage{ nullptr }; //!< GDI+ Image Object

	explicit operator bool() const noexcept { return (!m_tsFilename.empty() && (m_hBitmap || m_hIcon || m_pImage)); }
#else
	explicit operator bool() const noexcept { return (!m_tsFilename.empty() && (m_hBitmap || m_hIcon)); }
#endif

	void reset() noexcept
	{
#ifdef DCX_USE_GDIPLUS
		delete m_pImage;
		m_pImage = nullptr;
#endif

		if (m_hBitmap)
			DeleteBitmap(m_hBitmap);
		m_hBitmap = nullptr;

		if (m_hIcon)
			DeleteObject(m_hIcon);
		m_hIcon = nullptr;

		m_tsFilename.clear();
	}

	void toXml(TiXmlElement* xml) const
	{
		if (!xml)
			return;

		xml->SetAttribute("eval", "0"); // diable eval
		xml->SetAttribute("src", m_tsFilename.c_str());
	}

	TiXmlElement* toXml() const
	{
		auto xml = std::make_unique<TiXmlElement>("image");
		toXml(xml.get());
		return xml.release();
	}

	void fromXml(const TiXmlElement* xml)
	{
		if (!xml)
			return;

		reset();
		m_tsFilename = queryEvalAttribute(xml, "src");
		if (!m_tsFilename.empty())
			m_hBitmap = dcxLoadBitmap(m_hBitmap, m_tsFilename);
	}
};

// id > class > text
struct xmlStyle
{
	TString tsID;
	TString tsClass;
	TString tsType;
	// style data can be any attributes the target will take.

	const TiXmlElement* xStyle{}; // pointer to this style in the xml document, ONLY valid while doc exists!
};
using VectorOfStyles = std::vector<xmlStyle>; //!< Vector of XML styles in dcxml

struct xmlTemplate
{
	TString tsName;
	const TiXmlElement* xTemplate{}; // pointer to this template in the xml document, ONLY valid while doc exists!
};
using VectorOfTemplates = std::vector<xmlTemplate>; //!< Vector of XML templates in dcxml

struct xmlIcon
{
	TString tsID;		// UserID
	TString tsClass;
	TString tsType;		// control type
	TString tsFlags;
	TString tsSrc;
	const TiXmlElement* xIcon{}; // pointer to this template in the xml document, ONLY valid while doc exists!
};
using VectorOfIcons = std::vector<xmlIcon>; //!< Vector of XML templates in dcxml

// UNICODE/ANSI wrappers
#define dcx_atoi(x) ts_atoi(x)
#define dcx_atoi64(x) ts_atoi64(x)
#define dcx_atof(x) ts_atof(x)
#define dcx_fopen(x,y) _wfopen(x,y)
#define dcx_strstr(x,y) ts_strstr((x),(y))
#define dcx_strncmp(x,y,z) ts_strncmp((x),(y),(z))
#define dcx_itoa(x,y,z) _ts_itoa((x), (y), (z))

//#define dcx_strcpyn(x, y, z) { if (lstrcpyn((x), (y), gsl::narrow_cast<int>((z))) == nullptr) (x)[0] = 0; }

template <typename T>
inline void dcx_strcpyn(TCHAR* const sDest, const TCHAR* sSrc, const T& iSize) noexcept { if (ts_strcpyn(sDest, sSrc, iSize) == nullptr) sDest[0] = 0; }

constexpr const TCHAR* const dcx_truefalse(const bool& x) noexcept { return (x) ? &(TEXT("$true"))[0] : &(TEXT("$false")[0]); }

#define dcx_Con(x,y) dcx_strcpyn((y), dcx_truefalse((x)), mIRCLinker::m_dwCharacters);

#define dcx_ConChar(x,y) { \
if ((x)) (y)[0] = TEXT('1'); \
	else (y)[0] = TEXT('0'); \
	(y)[1] = 0; \
}

//#define dcx_testflag(x,y) (((x) & (y)) == (y))
//template <typename T, typename M>
//constexpr bool dcx_testflag(T x, M y) noexcept { return ((x & gsl::narrow_cast<T>(y)) == gsl::narrow_cast<T>(y)); }
template <DcxConcepts::IsNumeric T, DcxConcepts::IsNumeric M>
constexpr bool dcx_testflag(T x, M y) noexcept {
	if constexpr (sizeof(T) >= sizeof(M)) return ((x & gsl::narrow_cast<T>(y)) == gsl::narrow_cast<T>(y));
	else return ((gsl::narrow_cast<M>(x) & y) == y);
}

#define dcxlParam(x,y) const auto y = reinterpret_cast<x>(lParam)
#define dcxwParam(x,y) auto y = reinterpret_cast<x>(wParam)

template <DcxConcepts::IsNumeric T = size_t>
constexpr HWND to_hwnd(T t) noexcept { return reinterpret_cast<HWND>(t); }

template <DcxConcepts::IsNumeric T = size_t>
constexpr T from_hwnd(HWND t) noexcept { return reinterpret_cast<T>(t); }

// --------------------------------------------------
// DLL routines
// --------------------------------------------------

int dcx_round(const float x) noexcept;

bool ParseCommandToLogfont(const TString& cmd, LPLOGFONT lf);
TString ParseLogfontToCommand(const LPLOGFONT lf);
dcxFontFlags parseFontFlags(const TString& flags) noexcept;
UINT parseFontCharSet(const TString& charset);

auto readFile(const TString& filename) -> std::unique_ptr<BYTE[]>;
TString readTextFile(const TString& tFile);
bool SaveDataToFile(const TString& tsFile, const TString& tsData);
TString FileDialog(const TString& data, const TString& method, const HWND pWnd);
std::pair<bool, int> SaveClipboardToFile(const XSwitchFlags& xFlags, const TString& tsFile);
//Dcx::BoolValue<int> SaveClipboardToFile(const XSwitchFlags& xFlags, const TString& tsFile);

int CALLBACK BrowseFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) noexcept;
[[nodiscard("Memory Leak")]] gsl::owner<LPITEMIDLIST> GetFolderFromCSIDL(const int nCsidl) noexcept;

HWND GetHwndFromString(const TString& str);
HWND FindOwner(const TString& data, const gsl::not_null<HWND>& defaultWnd);
std::optional<HWND> FindOwner(const TString& data);
bool CopyToClipboard(const HWND owner, const TString& str) noexcept;

//[[nodiscard("Memory Leak")]] HBITMAP dcxLoadBitmap(HBITMAP dest, TString& filename);
[[nodiscard("Memory Leak")]] HICON dcxLoadIcon(const int index, TString& filename, const bool large, const TString& flags);
[[nodiscard("Memory Leak")]] HICON CreateGrayscaleIcon(HICON hIcon) noexcept;
[[nodiscard("Memory Leak")]] HRGN BitmapRegion(HBITMAP hBitmap, const COLORREF cTransparentColor, const bool bIsTransparent);

HFONT CopyHDCSettings(_In_ HDC hSrc, _In_ HDC hDst) noexcept;

void ChangeHwndIcon(const HWND hwnd, const TString& flags, const int index, TString& filename);
void AddFileIcons(HIMAGELIST himl, TString& filename, const bool bLarge, const int iIndex, const int iStart = 0, int iEnd = -1);
int dcxPickIconDlg(const HWND hwnd, LPWSTR pszIconPath, const UINT& cchIconPath, int* piIconIndex) noexcept;

BOOL dcxGetWindowRect(const HWND hWnd, const LPRECT lpRect) noexcept;
bool GetWindowRectParent(const HWND hwnd, RECT* rcWin) noexcept;

SYSTEMTIME MircTimeToSystemTime(const long mircTime);
long SystemTimeToMircTime(const LPSYSTEMTIME pst);

void AddToolTipToolInfo(const HWND tiphwnd, const HWND ctrl) noexcept;
#ifdef DCX_USE_GDIPLUS
constexpr const TCHAR* GetLastStatusStr(Gdiplus::Status status) noexcept;
#endif

bool IsFile(TString& filename);

struct dcxTextOptions
{
	bool m_bUseNewStyle{};
	bool m_bGradientFill{};
	bool m_bHorizGradientFill{};
	bool m_bOutline{};
	bool m_bFilledOutline{};
	bool m_bDoubleOutline{};
	bool m_bGradientOutline{};
	bool m_bHorizGradientOutline{};
	bool m_bShadow{};
	bool m_bNoCtrlCodes{};
	bool m_bTransparent{};
	bool m_bGlow{};
	bool m_bNoColours{};
	bool m_bAlphaBlend{};

	COLORREF m_clrText{ CLR_INVALID };
	COLORREF m_clrTextBackground{ CLR_INVALID };
	COLORREF m_clrGradientTextStart{ CLR_INVALID };
	COLORREF m_clrGradientTextEnd{ CLR_INVALID };
	COLORREF m_clrGradientOutlineStart{ CLR_INVALID };
	COLORREF m_clrGradientOutlineEnd{ CLR_INVALID };
	COLORREF m_clrOutline{ CLR_INVALID };
	COLORREF m_clrShadow{ RGB(0, 0, 0) };
	COLORREF m_clrGlow{ RGB(255,255,255) };

	BYTE m_uOutlineSize{ 1 };
	BYTE m_uShadowXOffset{ 5 };
	BYTE m_uShadowYOffset{ 5 };
	BYTE m_uShadowAlpha{ 192 };
	BYTE m_uShadowThickness{ 1 };
	BYTE m_uGlowSize{ 12 };
	BYTE m_uAlphaValue{ 255 };
};

struct dcxTextBreakdown
{
	LOGFONT m_log{};
	COLORREF m_clrText{ CLR_INVALID };
	COLORREF m_clrBack{ CLR_INVALID };
	TString m_str;
	bool m_bTransparent{ true };
};
std::vector<dcxTextBreakdown> dcxBreakdownmIRCText(const TString& txt);
std::vector<dcxTextBreakdown> dcxBreakdownmIRCText(const LPCWSTR txt, UINT len);
RECT dcxBreakdownCalcRect(HDC hdc, const std::vector<dcxTextBreakdown>& vec, LPCRECT rc, const UINT uStyle, const dcxTextOptions& dTO) noexcept;
void mIRC_DrawBreakdown(HDC hdc, const std::vector<dcxTextBreakdown>& vec, LPRECT rc, const UINT uStyle, const dcxTextOptions& dTO) noexcept;

void dcxDrawShadowText(HDC hdc, LPCWSTR pszText, UINT cch, RECT* pRect, DWORD dwFlags, COLORREF crText, COLORREF crShadow, int ixOffset, int iyOffset) noexcept;
void mIRC_DrawText(HDC hdc, const TString& txt, LPRECT rc, const UINT style, const bool shadow);
void mIRC_DrawTextNoBuffer(HDC hdc, const TString& txt, LPRECT rc, const UINT style, const bool shadow, const dcxTextOptions* dTO = nullptr);
void DrawRotatedText(const TString& strDraw, const LPCRECT rc, const HDC hDC, const int nAngleLine = 0, const bool bEnableAngleChar = false, const int nAngleChar = 0) noexcept;
void dcxDrawGradientText(HDC hdc, LPCWSTR txt, int len, LPRECT pRC, UINT fmt, const dcxTextOptions& dTO);
void dcxDrawGradientTextMasked(HDC hdc, LPCWSTR txt, int len, LPRECT pRC, UINT fmt, const dcxTextOptions& dTO);
void dcxDrawOutlineText(HDC hdc, LPCWSTR txt, int len, LPRECT pRC, UINT fmt, const dcxTextOptions& dTO);
void dcxDrawTextOptions(HDC hdc, LPCWSTR txt, int len, LPRECT pRC, UINT mStyle, const dcxTextOptions& dTO);

[[nodiscard("Memory Leak")]] gsl::owner<HDC*> CreateHDCBuffer(HDC hdc, const LPRECT rc);
void DeleteHDCBuffer(gsl::owner<HDC*> hBuffer) noexcept;
[[nodiscard("Memory Leak")]] HANDLE CreateHDCBufferOptions(_In_ HDC hdc, _Out_ HDC* hdcOut, _In_ bool bCopySettings, _In_ bool bCopyContents, _In_ bool bAlpha, _In_ BYTE uAlpha) noexcept;
void DeleteHDCBufferOptions(_In_opt_ HANDLE hBuf, _In_ bool bDraw) noexcept;

int TGetWindowText(HWND hwnd, TString& txt);
TString TGetWindowText(HWND hwnd);
void FreeOSCompatibility(void) noexcept;

const char* queryAttribute(const TiXmlElement* element, const char* attribute, const char* defaultValue = "") noexcept;
//gsl::not_null<const char *> queryAttribute(gsl::not_null<const TiXmlElement *> element, gsl::not_null<const char *> attribute, gsl::not_null<const char *> defaultValue = gsl::not_null<const char *>("")) noexcept;
//std::optional<const char *> queryAttribute(gsl::not_null<const TiXmlElement *> element, gsl::not_null<const char *> attribute) noexcept;
//TString queryEvalAttribute(const TiXmlElement* element, const char* attribute, const char* defaultValue = "");
COLORREF queryColourAttribute(const TiXmlElement* element, const char* attribute, COLORREF defaultValue = CLR_INVALID) noexcept;
void setColourAttribute(TiXmlElement* element, const char* attribute, COLORREF Value);
int queryIntAttribute(const TiXmlElement* element, const char* attribute, const int defaultValue = 0) noexcept;
//int queryIntAttribute(gsl::not_null<const TiXmlElement *> element, gsl::not_null<const char *> attribute, const int defaultValue = 0);
//std::optional<int> queryIntAttribute(gsl::not_null<const TiXmlElement *> element, gsl::not_null<const char *> attribute);
double queryDoubleAttribute(const TiXmlElement* element, const char* attribute, const double defaultValue = 0.0) noexcept;
float queryFloatAttribute(const TiXmlElement* element, const char* attribute, const float defaultValue = 0.0f) noexcept;

void getmIRCPalette(bool bForce);
void getmIRCPalette(COLORREF* const Palette, const UINT PaletteItems, bool bForce);
void getmIRCPalette(gsl::span<COLORREF> Palette, bool bForce);
void getmIRCPaletteMask(COLORREF* const Palette, const UINT PaletteItems, uint16_t uMask, bool bForce);
int unfoldColor(const WCHAR* color) noexcept;

// UltraDock
void RemStyles(const HWND hwnd, const int parm, const long RemStyles) noexcept;
void AddStyles(const HWND hwnd, const int parm, const long AddStyles) noexcept;

// DirectX
HRESULT GetDXVersion(DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion);

TString MakeTextmIRCSafe(const TString& tsStr);
TString MakeTextmIRCSafe(const TCHAR* const tString);
TString MakeTextmIRCSafe(const TCHAR* const tString, const std::size_t len);

void dcxDrawLine(HDC hdc, LONG x1, LONG y1, LONG x2, LONG y2) noexcept;
void dcxDrawCheckBox(HDC hDC, const LPCRECT rcBox, const clrCheckBox* lpcol, const DWORD dState, const bool bTicked, const bool bRounded) noexcept;
void dcxDrawRadioBox(HDC hDC, const LPCRECT rcBox, const clrCheckBox* lpcol, const DWORD dState, const bool bTicked, const bool bRounded) noexcept;
void dcxDrawEdge(HDC hdc, const LPRECT rc, COLORREF clr) noexcept;
void dcxDrawBorder(HDC hdc, LPCRECT lprc, DWORD dwBorder, COLORREF clr) noexcept;

enum class dcxArrowFlags
	: UINT
{
	Left =		0x01,
	Right =		0x02,
	Up =		0x04,
	Down =		0x08,
	Themed =	0x10
};
void dcxDrawArrow(_In_ HDC hdc, _In_ LPCRECT lprc, _In_ COLORREF clr, _In_ dcxArrowFlags eFlags = dcxArrowFlags::Right) noexcept;
HWND dcxGetRealParent(HWND hWnd) noexcept;
HBITMAP dcxCreateMask(HDC hdc, LPRECT rc, COLORREF clrMask) noexcept;

/// <summary>
/// Draws a solid rect of the specified size and colour
/// </summary>
/// <param name="hDC">- The drawing surface</param>
/// <param name="rc">- The bounding rectangle</param>
/// <param name="clr">- the fill colour</param>
/// <param name="clrBorder">- The border colour</param>
/// <param name="bRounded">- Should the rectangle have rounded corners?</param>
/// <returns></returns>
bool dcxDrawRect(HDC hDC, LPCRECT rc, COLORREF clr, COLORREF clrBorder, bool bRounded) noexcept;

/// <summary>
/// Draws a translucent rect of the specified size and colour
/// </summary>
/// <param name="hDC">- The drawing surface</param>
/// <param name="rc">- The bounding rectangle</param>
/// <param name="clr">- the fill colour</param>
/// <param name="clrBorder">- The border colour</param>
/// <param name="bRounded">- Should the rectangle have rounded corners?</param>
/// <returns></returns>
bool dcxDrawTranslucentRect(HDC hDC, LPCRECT rc, COLORREF clr, COLORREF clrBorder, bool bRounded) noexcept;

bool dcxDrawBitMap(HDC hdc, LPCRECT prc, HBITMAP hbm, bool bStretch, bool bAlpha) noexcept;

int dcxSetStretchModeHalfTone(HDC hdc) noexcept;
void dcxStretchHalfToneBlt(HDC hdcDest, const RECT& rcDest, HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc) noexcept;
void dcxTransparentHalfToneBlt(HDC hdcDest, const RECT& rcDest, HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc, COLORREF clrTrans) noexcept;
BITMAP dcxGetCurrentBitmap(HDC hdc) noexcept;
SIZE dcxGetCurrentBitmapDimensions(HDC hdc) noexcept;
RECT dcxGetCurrentBitmapRect(HDC hdc) noexcept;
SIZE dcxGetBitmapDimensions(HBITMAP hBM) noexcept;
RECT dcxGetBitmapRect(HBITMAP hBM) noexcept;
bool CopyBitmapToHDC(HDC hdc, LONG xDest, LONG yDest, LONG wDest, LONG hDest, HBITMAP hBm, LONG xSrc, LONG ySrc, bool bHasAlpha = false) noexcept;
bool CopyHDCToBitmap(HBITMAP hBm, LONG xDest, LONG yDest, LONG wDest, LONG hDest, HDC hdc, LONG xSrc, LONG ySrc) noexcept;

/// <summary>
/// Get a colour that contrasts nicely with the supplied colour.
/// </summary>
/// <param name="sRGB"></param>
/// <returns></returns>
COLORREF GetContrastColour(COLORREF sRGB) noexcept;

/// <summary>
/// Get a colour that contrasts nicely with the supplied colour. Maintains alpha value.
/// </summary>
/// <param name="sRGB"></param>
/// <returns></returns>
RGBQUAD GetContrastColour(RGBQUAD sRGB) noexcept;

TString BitmapToBase64(HBITMAP hBMP);
TString IconToBase64(HICON hIcon);
HBITMAP Base64ToBitmap(const char* vData, long w, long h);
HICON Base64ToIcon(const char* hbmData, const char* maskData, long sz);
HBITMAP Base64ToBitmap(const wchar_t* vData, long w, long h);
HICON Base64ToIcon(const wchar_t* hbmData, const wchar_t* maskData, long sz);
#ifdef DCX_USE_GDIPLUS
bool SavePNGFile(TString& tsFile, Gdiplus::Image& img);
#endif

HMODULE getDllModule(HINSTANCE hMod = nullptr) noexcept;

extern SIGNALSWITCH dcxSignal;
extern COLORREF staticPalette[mIRC_PALETTE_SIZE];

#include "Dcx.h"

#endif // _DEFINES_H_
