/*!
 * \file dcxlistview.h
 * \brief blah
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.0
 *
 * \b Revisions
 *
 * © ScriptsDB.org - 2006
 */

#ifndef _DCXLISTVIEW_H_
#define _DCXLISTVIEW_H_

#include "../defines.h"
#include "dcxcontrol.h"
#include "dcxprogressbar.h"

class DcxDialog;

#define LVIS_BOLD       0x0100 //!< ListView Caption Bold Style
#define LVIS_UNDERLINE  0x0200 //!< ListView Caption Underline Style
#define LVIS_COLOR      0x0400 //!< ListView Caption Color Style
#define LVIS_BGCOLOR    0x0800 //!< ListView Caption Background Color Style
#define LVIS_PBAR       0x1000 //!< ListView ProgressBar Style

#define LVSS_ASC        0x01   //!< ListView Sort Ascending Style
#define LVSS_DESC       0x02   //!< ListView Sort Descending Style
#define LVSS_NUMERIC    0x04   //!< ListView Sort Numeric Style
#define LVSS_ALPHA      0x08   //!< ListView Sort Alpha-Numeric Style
#define LVSS_CUSTOM     0x10   //!< ListView Sort Custom Style
#define LVSS_CASE       0x20   //!< ListView Sort Case-sensitive Style

#define LVCS_TEXT       0x01   //!< ListView Text Color Style
#define LVCS_BKGTEXT    0x02   //!< ListView Background Text Color Style
#define LVCS_BKG        0x04   //!< ListView Background Color Style
#define LVCS_OUTLINE    0x08   //!< ListView Selection Outline Color Style

#define LVSEARCH_W 0x01 //!< ListView WildCard Search
#define LVSEARCH_R 0x02 //!< ListView Regex Search

/*!
 * \brief blah
 *
 * blah
 */

typedef struct tagDCXLVSORT {

  TString tsCustomAlias;  //!< Custom Sorting Alias
  UINT iSortFlags;        //!< Sorting Flags
  HWND m_Hwnd;            //!< ListView Handle
  int nColumn;            //!< Sorting Column

} DCXLVSORT,*LPDCXLVSORT;

/*!
 * \brief blah
 *
 * blah
 */

typedef struct tagDCXLVITEM {

  TString tsTipText;  //!< Tooltip text
  COLORREF clrBack;   //!< Line Background Caption Color
  COLORREF clrText;   //!< Line Caption Color
  BOOL bBold;         //!< Is Line Caption Bold ?
  BOOL bUline;        //!< Is Line Caption Underlined
  DcxProgressBar *pbar;
  int iPbarCol;
} DCXLVITEM,*LPDCXLVITEM;


/*!
 * \brief blah
 *
 * blah
 */

class DcxListView : public DcxControl {

public:

  DcxListView( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxListView( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxListView( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );
  void parseListviewExStyles( TString & styles, LONG * ExStyles );

  HIMAGELIST getImageList( int iImageList );
  void setImageList( HIMAGELIST himl, int iImageList );
  HIMAGELIST createImageList( BOOL bIcons );

  BOOL isListViewStyle( long dwView );

  int getColumnCount( );

  static LRESULT CALLBACK EditLabelProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

  static int CALLBACK sortItemsEx( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );

  inline TString getType( ) { return TString( "listview" ); };

  int getTopIndex( );
  int getBottomIndex( );

protected:

  WNDPROC m_OrigEditProc; //!< Label Edit Control Orignal Procedure

  UINT parseIconFlagOptions( TString & flags );
  UINT parseItemFlags( TString & flags );
  UINT parseHeaderFlags( TString & flags );
  UINT parseHeaderFlags2( TString & flags );
  UINT parseSortFlags( TString & flags );
  UINT parseColorFlags( TString & flags );
  UINT parseImageFlags( TString & flags );
  UINT parseGroupFlags( TString & flags );

  BOOL matchItemText( int nItem, int nSubItem, TString * search, UINT SearchType );

  BOOL m_bDrag; //!< Dragging Items ?

private:
	DcxProgressBar* CreatePbar(LPLVITEM lvi, TString style);
	void ScrollPbars(int row);
	void UpdateScrollPbars();
};

#endif // _DCXLISTVIEW_H_