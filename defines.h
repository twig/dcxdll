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

#pragma warning( disable : 4100 ) // unreferenced formal parameter
#pragma warning( disable : 4530 )

#ifndef _DEFINES_H_
#define _DEFINES_H_

//#define WIN32_LEAN_AND_MEAN //!< blah

// normal Windows XP + IE V5.01
//#define _WIN32_WINNT 0x0501
//#define _WIN32_IE 0x0501

// Windows XP SP2 + IE V6
#define _WIN32_WINNT 0x0503
#define _WIN32_IE 0x0600

// Required for VS 2005
#if _MSC_VER == 1400
#define VS2005 1
#define _CRT_SECURE_NO_DEPRECATE 1
#endif
// end of VS 2005

#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <commctrl.h>

#include "classes/tstring/tstring.h"
#include <uxtheme.h>
#include <tmschema.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include "classes/dcxdialogcollection.h"

#define DLL_VERSION    1
#define DLL_SUBVERSION 3
#define DLL_BUILD      7
#define DLL_STATE      "Development Build"

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
#define DCX_PAGERCLASS       "DCXCPagerClass"       //!< DCX Panel Class Name
#define DCX_BOXCLASS         "DCXBoxClass"          //!< DCX Box Class Name
//#define DCX_RADIOCLASS       "DCXRadioClass"        //!< DCX Radio Class Name
//#define DCX_CHECKCLASS       "DCXCheckClass"        //!< DCX Check Class Name
//#define DCX_SCROLLBARCLASS   "DCXScrollBarClass"    //!< DCX ScrollBar Class Name

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
BOOL isMenuBarMenu(HMENU hMenu, HMENU hMatch);

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
#define mIRC(x) int __stdcall WINAPI x(HWND mWnd, HWND aWnd, char * data, char * parms, BOOL, BOOL)

/*! \brief Return String DLL Alias */
#define ret(x) { lstrcpy(data, x); lstrcat(data, "\0"); return 3; }


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
	HANDLE m_hFileMap; //!< Handle to the mIRC DLL File Map
	LPSTR  m_pData;    //!< Pointer to a character buffer of size 900 to send mIRC custom commands
	HWND   m_mIRCHWND; //!< mIRC Window Handle
	int    m_map_cnt;  //!< MapFile counter.
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
 * f = 5				XDID
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
void DCXError(char *cmd,char *msg);

int round(float x);
BOOL ParseCommandToLogfont(TString cmd, LPLOGFONT lf);
TString ParseLogfontToCommand(LPLOGFONT lf);
UINT parseFontFlags(TString &flags);
UINT parseFontCharSet(TString &charset);
DcxDialogCollection dcxDialogs();


char * readFile(const char * filename);
TString FileDialog(TString data, TString method, HWND pWnd);

typedef HRESULT (__stdcall *PFNSETTHEME)(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);
typedef HRESULT (__stdcall *PFNISTHEMEACTIVE)();
typedef HTHEME (WINAPI *PFNOPENTHEMEDATA)(HWND, LPCWSTR);
typedef HRESULT (WINAPI *PFNCLOSETHEMEDATA)(HTHEME);
typedef HRESULT (WINAPI *PFNDRAWTHEMEBACKGROUND)(HTHEME, HDC, int, int, const RECT*, const RECT *);
typedef HRESULT (WINAPI *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME, HDC, int, int,  const RECT *, RECT *);
typedef BOOL (WINAPI *PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)(HTHEME, int, int);
typedef HRESULT (WINAPI *PFNDRAWTHEMEPARENTBACKGROUND)(HWND, HDC, RECT*);
typedef HRESULT (WINAPI *PFNDRAWTHEMETEXT)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, DWORD, const RECT *);
HRESULT dcxSetWindowTheme(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);
BOOL dcxIsThemeActive();
BOOL isXP();

HWND GetHwndFromString(TString &str);
HWND GetHwndFromString(char *str);
HWND FindOwner(TString data, HWND defaultWnd);
BOOL CopyToClipboard(HWND owner, TString str);
HBITMAP dcxLoadBitmap(HBITMAP dest, TString &filename);
HICON CreateGrayscaleIcon(HICON hIcon);
HRGN BitmapRegion(HBITMAP hBitmap,COLORREF cTransparentColor,BOOL bIsTransparent);
bool ChangeHwndIcon(const HWND hwnd, TString *flags, const int index, TString *filename);

void AddToolTipToolInfo(HWND tiphwnd, HWND ctrl);
// UltraDock
void RemStyles(HWND hwnd,int parm,long RemStyles);
void AddStyles(HWND hwnd,int parm,long AddStyles);
void InitUltraDock(void);
void CloseUltraDock(void);
int SwitchbarPos(void);
void UltraUnDock(HWND hwnd);
void UpdatemIRC(void);

#endif // _DEFINES_H_