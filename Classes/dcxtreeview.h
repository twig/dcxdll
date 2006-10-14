/*!
 * \file dcxtreeview.h
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

#ifndef _DCXTREEVIEW_H_
#define _DCXTREEVIEW_H_

#include "../defines.h"
#include "dcxcontrol.h"

#include <string>

class DcxDialog;
class DcxTreeView;

#define TVSEARCH_W 0x01       //!< TreeView WildCard Search
#define TVSEARCH_R 0x02       //!< TreeView Regex Search

#define TVSS_ASC        0x01   //!< TreeView Sort Ascending Style
#define TVSS_DESC       0x02   //!< TreeView Sort Descending Style
#define TVSS_NUMERIC    0x04   //!< TreeView Sort Numeric Style
#define TVSS_ALPHA      0x08   //!< TreeView Sort Alpha-Numeric Style
#define TVSS_CUSTOM     0x10   //!< TreeView Sort Custom Style
#define TVSS_CASE       0x20   //!< TreeView Sort Case-sensitive Style
#define TVSS_SINGLE     0x40   //!< TreeView Sort Single Branch
#define TVSS_ALL        0x80   //!< TreeView Sort All Sub-branches

#define TVCOLOR_B 0x01        //!< TreeView Background Color
#define TVCOLOR_L 0x02        //!< TreeView Line Color
#define TVCOLOR_T 0x04        //!< TreeView Text Color

#define TVIS_UNDERLINE 0x0100 //!< TreeView Caption Underline Style
#define TVIS_COLOR     0x0200 //!< TreeView Caption Color Style
#define TVIS_ITALIC    0x0400 //!< TreeView Caption Italic Style

#define TVIE_EXP     0x01     //!< TreeView Expand Branch Option
#define TVIE_EXPALL  0x02     //!< TreeView Expandall Branch Option
#define TVIE_EXPPART 0x04     //!< TreeView ExpandPartial Branch Option
#define TVIE_COL     0x08     //!< TreeView Collapse Branch Option
#define TVIE_COLALL  0x10     //!< TreeView Collapseall Branch Option
#define TVIE_COLRES  0x20     //!< TreeView Collapse/Reset Branch Option
#define TVIE_TOGGLE  0x40     //!< TreeView Toogle Branch Option

#define TVIT_NORMAL 0x01      //!< TreeView Normal ImageList
#define TVIT_STATE  0x02      //!< TreeView State ImageList

/*!
 * \brief blah
 *
 * blah
 */

typedef struct tagDCXTVSORT {

  TString tsCustomAlias;  //!< Custom Sorting Alias
  UINT iSortFlags;        //!< Sorting Flags
  DcxTreeView * pthis;    //!< TreeView control object pointer

} DCXTVSORT,*LPDCXTVSORT;

/*!
 * \brief blah
 *
 * blah
 */

typedef struct tagDCXTVITEM {
	TString tsTipText;  //!< Tooltip text
	COLORREF clrText;   //!< Item Caption Color
	BOOL bBold;         //!< Is Item Caption Bold ?
	BOOL bUline;        //!< Is Item Caption Underlined
	BOOL bItalic;       //!< Is Item Caption Italicised
	HTREEITEM hHandle;  //!< TreeView Item Handle (used for sorting)
} DCXTVITEM,*LPDCXTVITEM;

/*!
 * \brief blah
 *
 * blah
 */

class DcxTreeView : public DcxControl {

public:

  //DcxTreeView( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxTreeView( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxTreeView( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  HIMAGELIST getImageList( int type );
  void setImageList( HIMAGELIST himl, int type );
  HIMAGELIST createImageList( );

  //HTREEITEM insertItem( );
  HTREEITEM insertItem( TString * path, TString * data, TString * Tooltip );

  void getItemText( HTREEITEM * hItem, char * szBuffer, int cchTextMax );
  int getItemCount( HTREEITEM * hParent );

  static LRESULT CALLBACK EditLabelProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

  static int CALLBACK sortItemsEx( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );

  inline TString getType( ) { return TString( "treeview" ); };

protected:

  WNDPROC m_OrigEditProc; //!< Label Edit Control Orignal Procedure

  UINT m_iIconSize; //!< Icon size

  /* *** */

  BOOL parsePath( TString * path, HTREEITEM * hParent, HTREEITEM * hInsertAfter, int depth = 1 );
  BOOL getPath( VectorOfInts * numPath, HTREEITEM * hStart, HTREEITEM * hItemToFind, int depth = 1 );
  BOOL correctTargetItem( HTREEITEM * hParent, HTREEITEM * hInsertAfter );

  BOOL matchItemText( HTREEITEM * hItem, TString * search, UINT SearchType );
  BOOL findItemText( HTREEITEM * hStart, HTREEITEM * hItem, TString * search, int N, int * NC, UINT SearchType );
  void expandAllItems( HTREEITEM * hStart, UINT expandOption );

  HTREEITEM cloneItem( HTREEITEM * hItem, HTREEITEM * hParentTo, HTREEITEM * hAfterTo );
  void copyAllItems( HTREEITEM *hItem, HTREEITEM * hParentTo );

  std::string getPathFromVector( VectorOfInts * numPath );

  static UINT parseIconFlagOptions( TString & flags );
  static UINT parseItemFlags( TString & flags );
  static UINT parseSortFlags( TString & flags );
  static UINT parseColorFlags( TString & flags );
  static UINT parseToggleFlags( TString & flags );
  
};

#endif // _DCXTREEVIEW_H_