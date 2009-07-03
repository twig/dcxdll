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

#include "tinyxml/tinyxml.h"

class DcxDialog;
class DcxTreeView;

#ifndef TVIF_EXPANDEDIMAGE
#define TVIF_EXPANDEDIMAGE 0x0200
#endif

#define TVSEARCH_W 0x01       //!< TreeView WildCard Search
#define TVSEARCH_R 0x02       //!< TreeView Regex Search
#define TVSEARCH_E 0x04       //!< TreeView Exact Match

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
#define TVCOLOR_S 0x08        //!< TreeView Selection Color

#define TVIS_DCXMASK		0xFF0000
#define TVIS_UNDERLINE	0x010000 //!< TreeView Caption Underline Style
#define TVIS_COLOR			0x020000 //!< TreeView Caption Color Style
#define TVIS_ITALIC			0x040000 //!< TreeView Caption Italic Style
#define TVIS_BKG				0x080000 //!< TreeView Item blackground colour style.
#define TVIS_HASHITEM		0x100000 //!< TreeView item text is taken from a hash table item
#define TVIS_HASHNUMBER	0x200000 //!< TreeView item text is taken from a hash tabel item number
#define TVIS_XML				0x400000 //!< TreeView item text is taken from an xml file.

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
	COLORREF clrBkg;		//!< Item background colour.
	BOOL bBold;         //!< Is Item Caption Bold ?
	BOOL bUline;        //!< Is Item Caption Underlined
	BOOL bItalic;       //!< Is Item Caption Italicised
	HTREEITEM hHandle;  //!< TreeView Item Handle (used for sorting)
	TString tsMark;		// Marked item text.
} DCXTVITEM,*LPDCXTVITEM;

/*!
 * \brief blah
 *
 * blah
 */

class DcxTreeView : public DcxControl {

public:

	DcxTreeView( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles );
	virtual ~DcxTreeView( );

	LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

	void parseInfoRequest( TString & input, char * szReturnValue );
	void parseCommandRequest( TString & input );
	void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );
#ifdef DCX_USE_WINSDK
	static void parseTreeViewExStyles( const TString & styles, LONG * ExStyles );
#endif

	HIMAGELIST getImageList( const int type );
	void setImageList( HIMAGELIST himl, const int type );
	HIMAGELIST createImageList( );

	//HTREEITEM insertItem( );
	void insertItem( const TString * path, const TString * data, const TString * Tooltip );

	void getItemText( HTREEITEM * hItem, char * szBuffer, const int cchTextMax ) const;
	int getChildCount( HTREEITEM * hParent ) const;

	static LRESULT CALLBACK EditLabelProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	static int CALLBACK sortItemsEx( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );

	inline TString getType( ) { return TString( "treeview" ); };

protected:

	WNDPROC m_OrigEditProc; //!< Label Edit Control Orignal Procedure

	UINT m_iIconSize; //!< Icon size

	COLORREF m_colSelection;

	/* *** */

	HTREEITEM parsePath(const TString *path, HTREEITEM *hParent = NULL, HTREEITEM *hInsertAt = NULL);
	TString getPathFromItem(HTREEITEM *item);


	BOOL matchItemText( HTREEITEM * hItem, const TString * search, const UINT SearchType ) const;
	BOOL findItemText( HTREEITEM * hStart, HTREEITEM * hItem, const TString * search, const int N, int * NC, const UINT SearchType ) const;
	void expandAllItems( HTREEITEM * hStart, const UINT expandOption );

	HTREEITEM cloneItem( HTREEITEM * hItem, HTREEITEM * hParentTo, HTREEITEM * hAfterTo );
	void copyAllItems( HTREEITEM *hItem, HTREEITEM * hParentTo );

	static UINT parseIconFlagOptions( const TString & flags );
	static UINT parseItemFlags( const TString & flags );
	static UINT parseSortFlags( const TString & flags );
	static UINT parseColorFlags( const TString & flags );
	static UINT parseToggleFlags( const TString & flags );

	HFONT m_hItemFont; // Font used for specific item changes.
	HFONT m_hOldItemFont; // Font used for specific item changes.

	void DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam);
	void PreloadData();

#ifdef DCX_USE_GDIPLUS
	bool LoadGDIPlusImage(const TString &flags, TString &filename);
	void DrawGDIPlusImage(HDC hdc);
	Image *m_pImage;							// Background Image
	CompositingQuality m_CQuality;// Image Rendering Quality
	CompositingMode m_CMode;			// Image Rendering Mode
	InterpolationMode m_IMode;		//
	SmoothingMode m_SMode;				// Image Smoothing Mode
	bool m_bTileImage;						// Tile Image?
	bool m_bResizeImage;					// Resize Image?
	int m_iXOffset;								// Images X Offset
	int m_iYOffset;								// Images Y Offset
	bool m_bTransparent;					// Is Control Transparent?
#endif

	bool xmlSaveTree(HTREEITEM hFromItem, const TString &name, TString &filename);
	bool xmlGetItems(HTREEITEM hFirstSibling, TiXmlElement *xElm, TCHAR *buf);
	HTREEITEM xmlLoadTree(HTREEITEM hInsertAfter, HTREEITEM hParent, const TString &name, TString &filename);
	TiXmlElement *xmlInsertItems(HTREEITEM hParent, HTREEITEM &hInsertAfter, TiXmlElement *xElm);
	TString getStyles(void);
};

#endif // _DCXTREEVIEW_H_
