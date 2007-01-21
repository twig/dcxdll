/*!
 * \file defines.h
 * \brief Definition Support File
 *
 * This file contains constant, alias and variable type defintions.
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.1
 *
 * \b Revisions
 *	1.1
 *		Added Visual Studio 2005 specific defines. Ook
 *
 * © ScriptsDB.org - 2006
 */

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( disable : 1195 )
#pragma warning( disable : 504 )
#pragma warning( disable : 1563 )
#else
#pragma warning( disable : 4100 ) // unreferenced formal parameter
#pragma warning( disable : 4530 )
#pragma warning( disable : 4995 ) // name was marked as #pragma deprecated
#endif

#ifndef _DEFINES_H_
#define _DEFINES_H_

//#define WIN32_LEAN_AND_MEAN //!< blah

// normal Windows XP + IE V5.01 + GDI+ 1.0
//#define _WIN32_WINNT 0x0501
//#define _WIN32_IE 0x0501
//#define GDIPVER 0x0100

// Windows XP SP2 + IE V6 + GDI+ 1.1
#define _WIN32_WINNT 0x0503
#define _WIN32_IE 0x0600
#define GDIPVER 0x0110

// Required for VS 2005
#if _MSC_VER >= 1400
#define VS2005 1
#define _CRT_SECURE_NO_DEPRECATE 1
#endif
// end of VS 2005

// Using DirectX SDK? If not, get off your arse & install it!
#define DCX_USE_DXSDK
// end of DirectX SDK

// Use GDI+
#define DCX_USE_GDIPLUS
#define DCX_MAX_GDI_ERRORS 21
//

//#include <vld.h>
#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <commctrl.h>

#include "classes/tstring/tstring.h"
#include <uxtheme.h>
#include <tmschema.h>

#ifdef DCX_USE_GDIPLUS
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")
#endif

#include "classes/dcxdialogcollection.h"

#include "AggressiveOptimize.h"

#define DLL_VERSION    1
#define DLL_SUBVERSION 3
#define DLL_BUILD      8
#ifdef NDEBUG
#ifdef DCX_DEV_BUILD
// Dev Build, enable debug output.
#define DCX_DEBUG(x,y) if (mIRCLink.m_bisDebug) DCXDebug((x), (y));
#define DLL_STATE      "Development Build"
#else
// Release Build, disable debug info.
#define DCX_DEBUG(x,y)
#define DLL_STATE      "Release Build"
#endif
#else
// Debug Build, enable debug output.
#define DCX_DEBUG(x,y) if (mIRCLink.m_bisDebug) DCXDebug((x), (y));
#define DLL_STATE      "Debug Build"
#endif

#define mIRC_ID_OFFSET 6000 //!< mIRC Dialog ID Offset

// listview stuff
#ifndef LPNMLVDISPINFO
#ifdef UNICODE
#define LPNMLVDISPINFO LPNMLVDISPINFOW
#else
#define LPNMLVDISPINFO LPNMLVDISPINFOA
#endif
#endif

// DCX Stuff
#define DCX_LISTVIEWCLASS    "DCXListViewClass"     //!< DCX Listview Class Name
#define DCX_PROGRESSBARCLASS "DCXProgressBarClass"  //!< DCX ProgressBar Class Name
#define DCX_TREEVIEWCLASS    "DCXTreeViewClass"     //!< DCX TreeView Class Name
#define DCX_TOOLBARCLASS     "DCXToolBarClass"      //!< DCX ToolBar Class Name
#define DCX_RICHEDITCLASS    "DCXRichEditClass"     //!< DCX RichEdit Class Name
#define DCX_TABCTRLCLASS     "DCXTabCtrlClass"      //!< DCX Tab Class Name
#define DCX_REBARCTRLCLASS   "DCXRebarCtrlClass"    //!< DCX Rebar Class Name
#define DCX_COMBOEXCLASS     "DCXComboExClass"      //!< DCX ComboEx Class Name
#define DCX_BUTTONCLASS      "DCXButtonClass"       //!< DCX Button Class Name
#define DCX_STATUSBARCLASS   "DCXStatusBarClass"    //!< DCX StatusBar Class Name
#define DCX_COLORCOMBOCLASS  "DCXColorComboClass"   //!< DCX ColorCombo Class Name
#define DCX_TRACKBARCLASS    "DCXTrackBarClass"     //!< DCX TrackBar Class Name
#define DCX_UPDOWNCLASS      "DCXUpDownClass"       //!< DCX Updown Class Name
#define DCX_IPADDRESSCLASS   "DCXIpAddressClass"    //!< DCX IP Address Class Name
#define DCX_DIVIDERCLASS     "DCXDividerClass"      //!< DCX Divider Class Name
#define DCX_PANELCLASS       "DCXPanelClass"        //!< DCX Panel Class Name
#define DCX_CALENDARCLASS    "DCXCalendarClass"     //!< DCX Panel Class Name
#define DCX_DATETIMECLASS    "DCXDateTimeClass"     //!< DCX DateTime Class Name
#define DCX_PAGERCLASS       "DCXCPagerClass"       //!< DCX Panel Class Name
#define DCX_BOXCLASS         "DCXBoxClass"          //!< DCX Box Class Name
//#define DCX_RADIOCLASS       "DCXRadioClass"        //!< DCX Radio Class Name
//#define DCX_CHECKCLASS       "DCXCheckClass"        //!< DCX Check Class Name
//#define DCX_SCROLLBARCLASS   "DCXScrollBarClass"    //!< DCX ScrollBar Class Name
#define DCX_SHADOWCLASS				"DCXShadowClass"				//!< DCX Box Class Name

// Layout Constants
#define LAYOUTFIXED 0x01  //!< Layout Cell Fixed Type
#define LAYOUTFILL  0x02  //!< Layout Cell Fill Type
#define LAYOUTPANE  0x04  //!< Layout Cell Pane Type
#define LAYOUTID    0x08  //!< Layout Control ID is Valid
#define LAYOUTDIM   0x10  //!< Layout Control Dimensions are Valid (Only works with fixed)
#define LAYOUTVERT  0x20  //!< Layout Cell Vertical Style (LayoutCellPane and LayoutCellFixed)
#define LAYOUTHORZ  0x40  //!< Layout Cell Horizontal Style (LayoutCellPane and LayoutCellFixed)

// DCX Font stuff
#define DCF_ANTIALIASE    0x01 //!< Control Font Anti-Aliase Style
#define DCF_BOLD          0x02 //!< Control Font Bold Style
#define DCF_ITALIC        0x04 //!< Control Font Italic Style
#define DCF_STRIKEOUT     0x08 //!< Control Font Strikeout Style
#define DCF_UNDERLINE     0x10 //!< Control Font Underline Style
#define DCF_DEFAULT       0x20 //!< Control Font Default Style

// XPopup Stuff
#define XPOPUPMENUCLASS "XPopupMenu32" //!< XPopupMenu Window Class Name

LRESULT CALLBACK mIRCSubClassWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL isMenuBarMenu(const HMENU hMenu, const HMENU hMatch);

// UltraDock Stuff
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


/*! \brief mIRC Function Alias */
#define mIRC(x) int WINAPI x(HWND mWnd, HWND aWnd, char * data, char * parms, BOOL, BOOL)

/*! \brief Return String DLL Alias (data is limited to 900) */
#define ret(x) { lstrcpyn(data, (x), 900); return 3; }

/*!
 * \brief mIRC DLL Loading Structure
 */
typedef struct {
	DWORD  mVersion; //!< mIRC Version
	HWND   mHwnd;    //!< mIRC Hwnd 
	BOOL   mKeep;    //!< mIRC variable stating to keep DLL in memory
} LOADINFO;

/*!
 * \brief DCX DLL mIRC Information Structure
 */
typedef struct {
	HANDLE	m_hFileMap; //!< Handle to the mIRC DLL File Map
	LPSTR		m_pData;    //!< Pointer to a character buffer of size 900 to send mIRC custom commands
	HWND		m_mIRCHWND; //!< mIRC Window Handle
	int			m_map_cnt;  //!< MapFile counter.
	int			m_bDoGhostDrag; //!< Ghost window while dragging.
	bool		m_bGhosted; //!< Is Window Currently ghosted (as a result of drag ghost).
	bool		m_bisDebug;    // is mIRC is using /debug upon DCX LoadDLL().
	bool		m_bUseGDIPlus; // we can use GDI+ functions.
	bool		m_bmIRCSixPointTwoZero; // Is this mIRC V6.20
	HWND		m_hSwitchbar; // The Switchbars HWND
	HWND		m_hToolbar; // The Toolbars HWND
	HWND		m_hMDI; // The MDIClients HWND
	HWND		m_hTreebar; // The Treebars HWND
	HWND		m_hTreeView; // The TreeView control child of the Treebar.
	HFONT		m_hTreeFont; // The Treebars original font.
	HIMAGELIST m_hTreeImages; // The Treebars original image list.
} mIRCDLL;

/*!
 * \brief Switch Parameters Container
 *
 * This structure is used to parse /xdid or /xdialog command -switches. It contains two arrays
 * populated by 26 values from a/A-z/Z and of value 1 indicating the switch was in the command or 0 if not.
 *
 * a = 0		
 * b = 1		xdid
 * c = 2				XDID
 * d = 3
 * e = 4		xdid
 * f = 5		xdid	XDID
 * g = 6
 * h = 7		xdid
 * i = 8
 * j = 9				XDID
 * k = 10
 * l = 11
 * m = 12			XDID
 * n = 13
 * o = 14
 * p = 15	xdid
 * q = 16
 * r = 17			XDID
 * s = 18	xdid
 * t = 19			XDID
 * u = 20			XDID
 * v = 21
 * w = 22
 * x = 23	xdid
 * y = 24
 * z = 25			XDID
 */

typedef struct {
	int switch_flags[26];     //!< Lowercase switches a-z
	int switch_cap_flags[26]; //!< Uppercase switches A-Z
} XSwitchFlags;

typedef std::vector<int> VectorOfInts; //<! Vector of int

#define DOCKF_NORMAL	0x01	//!< No special flags.
#define DOCKF_AUTOH		0x02	//!< Auto Horizontal size.
#define DOCKF_AUTOV		0x04	//!< Auto Vertical size.
#define DOCKF_SIZE		0x08	//!< Auto Horizontal & Vertical size.
#define DOCKF_LEFT		0x10  //!< Dock to left. (UltraDock)
#define DOCKF_RIGHT		0x20	//!< Dock to right. (UltraDock)
#define DOCKF_TOP			0x40	//!< Dock to top. (UltraDock)
#define DOCKF_BOTTOM	0x80	//!< Dock to bottom. (UltraDock)

typedef struct tagDCXULTRADOCK {
	HWND hwnd;
	DWORD flags;
	DWORD old_styles;
	DWORD old_exstyles;
	RECT rc;
} DCXULTRADOCK, *LPDCXULTRADOCK;

typedef std::vector<LPDCXULTRADOCK> VectorOfDocks;

void mIRCDebug(const char *szFormat, ...);
void mIRCSignalDCX(const char *szFormat, ...);
void mIRCSignal(const char *data);
void mIRCError(const char *data);
void mIRCeval(const char *text, char *res);
void mIRCcom(const char *data);

void dcxInfoError(const char *ctrl, const char *functn, const char *dlg, const int ctrlid, const char *msg);
void DCXError(const char *cmd,const char *msg);
void DCXDebug(const char *cmd,const char *msg);

int round(const float x);
BOOL ParseCommandToLogfont(const TString& cmd, LPLOGFONT lf);
TString ParseLogfontToCommand(const LPLOGFONT lf);
UINT parseFontFlags(TString &flags);
UINT parseFontCharSet(TString &charset);
DcxDialogCollection dcxDialogs();


char * readFile(const char * filename);
TString FileDialog(const TString & data, TString method, const HWND pWnd);

typedef HRESULT (__stdcall *PFNSETTHEME)(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);
typedef HRESULT (__stdcall *PFNISTHEMEACTIVE)();
typedef HTHEME (WINAPI *PFNOPENTHEMEDATA)(HWND, LPCWSTR);
typedef HRESULT (WINAPI *PFNCLOSETHEMEDATA)(HTHEME);
typedef HRESULT (WINAPI *PFNDRAWTHEMEBACKGROUND)(HTHEME, HDC, int, int, const RECT*, const RECT *);
typedef HRESULT (WINAPI *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME, HDC, int, int,  const RECT *, RECT *);
typedef BOOL (WINAPI *PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)(HTHEME, int, int);
typedef HRESULT (WINAPI *PFNDRAWTHEMEPARENTBACKGROUND)(HWND, HDC, RECT*);
typedef HRESULT (WINAPI *PFNDRAWTHEMETEXT)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, DWORD, const RECT *);
typedef BOOL (WINAPI *PFNUPDATELAYEREDWINDOW)(HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags);
typedef BOOL (WINAPI *PFNSETLAYEREDWINDOWATTRIBUTES)(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
typedef int (WINAPI *PFNDRAWSHADOWTEXT)(HDC hdc, LPCWSTR pszText, UINT cch, const RECT *pRect, DWORD dwFlags, COLORREF crText, COLORREF crShadow, int ixOffset, int iyOffset);

HRESULT dcxSetWindowTheme(const HWND hwnd, const LPCWSTR pszSubAppName, const LPCWSTR pszSubIdList);
BOOL dcxIsThemeActive();
BOOL isXP();

HWND GetHwndFromString(const TString &str);
HWND GetHwndFromString(const char *str);
HWND FindOwner(const TString & data, const HWND defaultWnd);
BOOL CopyToClipboard(const HWND owner, const TString & str);
HBITMAP dcxLoadBitmap(HBITMAP dest, TString &filename);
HICON dcxLoadIcon(const int index, TString &filename, const bool large = false);
HICON CreateGrayscaleIcon(HICON hIcon);
HRGN BitmapRegion(HBITMAP hBitmap,COLORREF cTransparentColor,BOOL bIsTransparent);
bool ChangeHwndIcon(const HWND hwnd, const TString *flags, const int index, const TString *filename);

SYSTEMTIME MircTimeToSystemTime(const long mircTime);
long SystemTimeToMircTime(const LPSYSTEMTIME pst);

void AddToolTipToolInfo(const HWND tiphwnd, const HWND ctrl);
void dcxDrawShadowText(HDC hdc, LPCWSTR pszText, UINT cch, const RECT *pRect, DWORD dwFlags, COLORREF crText, COLORREF crShadow, int ixOffset, int iyOffset);
#ifdef DCX_USE_GDIPLUS
const char *GetLastStatusStr(Status status);
#endif
bool IsFile(const TString &filename);

// UltraDock
void RemStyles(HWND hwnd,int parm,long RemStyles);
void AddStyles(HWND hwnd,int parm,long AddStyles);
void InitUltraDock(void);
void CloseUltraDock(void);
int SwitchbarPos(const int type);
void UltraUnDock(const HWND hwnd);
void UpdatemIRC(void);

extern PFNSETTHEME SetWindowThemeUx;  //!< blah
extern PFNISTHEMEACTIVE IsThemeActiveUx;
extern PFNOPENTHEMEDATA OpenThemeDataUx;
extern PFNCLOSETHEMEDATA CloseThemeDataUx;
extern PFNDRAWTHEMEBACKGROUND DrawThemeBackgroundUx;
extern PFNGETTHEMEBACKGROUNDCONTENTRECT GetThemeBackgroundContentRectUx;
extern PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT IsThemeBackgroundPartiallyTransparentUx;
extern PFNDRAWTHEMEPARENTBACKGROUND DrawThemeParentBackgroundUx;
extern PFNDRAWTHEMETEXT DrawThemeTextUx;
extern PFNUPDATELAYEREDWINDOW UpdateLayeredWindowUx;
extern PFNSETLAYEREDWINDOWATTRIBUTES SetLayeredWindowAttributesUx;
extern PFNDRAWSHADOWTEXT DrawShadowTextUx;
extern mIRCDLL mIRCLink;

#endif // _DEFINES_H_