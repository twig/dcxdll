/*!
 * \file defines.h
 * \brief Definition Support File
 *
 * This file contains constant, alias and variable type defintions.
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.0
 *
 * \b Revisions
 *
 * © ScriptsDB.org - 2006
 */

#pragma warning( disable : 4530 )

#ifndef _DEFINES_H_
#define _DEFINES_H_

//#define WIN32_LEAN_AND_MEAN //!< blah

#define _WIN32_WINNT 0x0501
#define _WIN32_IE 0x0501

#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <commctrl.h>
//#include <gdiplus.h>

//using namespace Gdiplus;

#include "classes/tstring/tstring.h"
//#include <uxtheme.h>
//#include "dialog.h"

#define DLL_VERSION    1
#define DLL_SUBVERSION 3
#define DLL_BUILD      0
#define DLL_STATE "Development Build"

#define mIRC_ID_OFFSET 6000 //!< mIRC Dialog ID Offset

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

// XPopup Stuff

#define XPOPUPMENUCLASS "XPopupMenu32" //!< XPopupMenu Window Class Name

LRESULT CALLBACK mIRCSubClassWinProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL isMenuBarMenu( HMENU hMenu, HMENU hMatch );

/*! \brief mIRC Function Alias */
#define mIRC( x ) int __stdcall WINAPI x( HWND mWnd, HWND aWnd, char * data, char * parms, BOOL, BOOL )

/*! \brief Return String DLL Alias */
#define ret( x ) { lstrcpy( data, x ); lstrcat( data, "\0" ); return 3; }

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
  LPSTR m_pData;     //!< Pointer to a character buffer of size 900 to send mIRC custom commands
  HWND m_mIRCHWND;   //!< mIRC Window Handle

} mIRCDLL;

/*!
 * \brief Switch Parameters Container
 *
 * This structure is used to parse /xdid or /xdialog command -switches. It contains two arrays
 * populated by 26 values from a/A-z/Z and of value 1 indicating the switch was in the command or 0 if not.
 *
 * a = 0
 * b = 1
 * c = 2
 * d = 3
 * e = 4
 * f = 5
 * g = 6
 * h = 7
 * i = 8
 * j = 9
 * k = 10
 * l = 11
 * m = 12
 * n = 13
 * o = 14
 * p = 15
 * q = 16
 * r = 17
 * s = 18
 * t = 19
 * u = 20
 * v = 21
 * w = 22
 * x = 23
 * y = 24
 * z = 25
 */

typedef struct {
  int switch_flags[26];     //!< Lowercase switches a-z
  int switch_cap_flags[26]; //!< Uppercase switches A-Z
} XSwitchFlags;


typedef std::vector<int> VectorOfInts; //<! Vector of int

void mIRCSignal( const char * data );
void mIRCError( const char * data );
void mIRCeval( const char * text, char * res );
void mIRCcom( const char * data );

int round( float x );

char * readFile( const char * filename );

typedef HRESULT (__stdcall *PFNSETTHEME)( HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList );
HRESULT SetWindowTheme( HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList );
BOOL isXP( );

#endif // _DEFINES_H_