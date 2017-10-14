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

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes\custom\ListHelper.h"

#include <string>

#include "Classes/tinyxml/tinyxml.h"

class DcxDialog;
class DcxTreeView;

#ifndef TVIF_EXPANDEDIMAGE
#define TVIF_EXPANDEDIMAGE 0x0200
#endif

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
#define TVIS_UNDERLINE		0x010000 //!< TreeView Caption Underline Style
#define TVIS_COLOR			0x020000 //!< TreeView Caption Color Style
#define TVIS_ITALIC			0x040000 //!< TreeView Caption Italic Style
#define TVIS_BKG			0x080000 //!< TreeView Item blackground colour style.
#define TVIS_HASHITEM		0x100000 //!< TreeView item text is taken from a hash table item
#define TVIS_HASHNUMBER		0x200000 //!< TreeView item text is taken from a hash tabel item number
#define TVIS_XML			0x400000 //!< TreeView item text is taken from an xml file.

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

struct DCXTVSORT {
	TString		tsCustomAlias;						//!< Custom Sorting Alias
	UINT		iSortFlags;							//!< Sorting Flags
	DcxTreeView *pthis;								//!< TreeView control object pointer
	TCHAR		itemtext1[MIRC_BUFFER_SIZE_CCH];	// Item text buffer One
	TCHAR		itemtext2[MIRC_BUFFER_SIZE_CCH];	// Item Text Buffer Two

	DCXTVSORT()
		: tsCustomAlias()
		, iSortFlags(0)
		, pthis(nullptr)
		, itemtext1{}
		, itemtext2{}
	{}
};
using LPDCXTVSORT = DCXTVSORT *;

/*!
 * \brief blah
 *
 * blah
 */

struct DCXTVITEM {
	TString		tsTipText;	//!< Tooltip text
	TString		tsMark;		// Marked item text.
	COLORREF	clrText;	//!< Item Caption Color
	COLORREF	clrBkg;		//!< Item background colour.
	HTREEITEM	hHandle;	//!< TreeView Item Handle (used for sorting)
	bool		bBold;		//!< Is Item Caption Bold ?
	bool		bUline;		//!< Is Item Caption Underlined
	bool		bItalic;	//!< Is Item Caption Italicised

	DCXTVITEM()
		: tsTipText()
		, tsMark()
		, clrText(CLR_INVALID)
		, clrBkg(CLR_INVALID)
		, hHandle(nullptr)
		, bBold(false)
		, bUline(false)
		, bItalic(false)
	{}
};
using LPDCXTVITEM = DCXTVITEM *;

/*!
 * \brief blah
 *
 * blah
 */

class DcxTreeView
	: public DcxControl
	, public DcxListHelper
{
public:
	DcxTreeView() = delete;
	DcxTreeView(const DcxTreeView &) = delete;
	DcxTreeView &operator =(const DcxTreeView &) = delete;	// No assignments!

	DcxTreeView( const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxTreeView( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest( const TString & input ) override;
	//void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;
	std::tuple<NoTheme, WindowStyle, WindowExStyle> parseControlStyles(const TString & tsStyles) override;

	HIMAGELIST getImageList( const int type ) const;
	void setImageList( HIMAGELIST himl, const int type );
	HIMAGELIST createImageList( );

	void insertItem( const TString &tsPath, const TString &tsData, const TString &tsTooltip );

	void getItemText( const HTREEITEM hItem, TCHAR * szBuffer, const int cchTextMax ) const;
	int getChildCount( const HTREEITEM hParent ) const;

	inline const TString getType() const override { return TEXT("treeview"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::TREEVIEW; }

	const TString getStyles(void) const override;
	void toXml(TiXmlElement *const xml) const override;
	TiXmlElement * toXml(void) const override;

protected:

	WNDPROC m_OrigEditProc; //!< Label Edit Control Orignal Procedure

	DcxIconSizes m_iIconSize; //!< Icon size

	bool m_bDestroying; //!< this flag is set when the listview is about to get destroyed to avoid senseless events

	COLORREF m_colSelection;

	/* *** */

	HTREEITEM parsePath(const TString &path, HTREEITEM *hParent = nullptr, HTREEITEM *hInsertAt = nullptr) const;
	TString getPathFromItem(const HTREEITEM item) const;

	bool matchItemText( const HTREEITEM hItem, const TString &search, const DcxSearchTypes &SearchType ) const;
	std::pair<bool, HTREEITEM> findItemText( const HTREEITEM hStart, const TString &queryText, const int n, int &matchCount, const DcxSearchTypes &SearchType ) const;
	void expandAllItems( const HTREEITEM hStart, const UINT expandOption );

	HTREEITEM cloneItem( const HTREEITEM hItem, const HTREEITEM hParentTo, const HTREEITEM hAfterTo );
	void copyAllItems( const HTREEITEM hItem, const HTREEITEM hParentTo );
	HTREEITEM copyAllItems(const TString &pathFrom, const TString &pathTo);

	static UINT parseIconFlagOptions( const TString & flags );
	static UINT parseItemFlags( const TString & flags );
	static UINT parseSortFlags( const TString & flags );
	static UINT parseColorFlags( const TString & flags );
	static UINT parseToggleFlags( const TString & flags );
	static int CALLBACK sortItemsEx(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	//static void parseTreeViewExStyles(const TString & styles, LONG * ExStyles);
	WindowExStyle parseTreeViewExStyles(const TString & styles) const;
	static LRESULT CALLBACK EditLabelProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HFONT m_hItemFont; // Font used for specific item changes.
	HFONT m_hOldItemFont; // Font used for specific item changes.

	void DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam);
	void PreloadData();

#ifdef DCX_USE_GDIPLUS
	bool LoadGDIPlusImage(const TString &flags, TString &filename);
	void DrawGDIPlusImage(HDC hdc);

	//Gdiplus::Image *m_pImage;							// Background Image
	std::unique_ptr<Gdiplus::Image> m_pImage;							// Background Image
	Gdiplus::CompositingQuality m_CQuality;// Image Rendering Quality
	Gdiplus::CompositingMode m_CMode;			// Image Rendering Mode
	Gdiplus::InterpolationMode m_IMode;		//
	Gdiplus::SmoothingMode m_SMode;				// Image Smoothing Mode
	bool m_bTileImage;						// Tile Image?
	bool m_bResizeImage;					// Resize Image?
	bool m_bTransparent;					// Is Control Transparent?
	int m_iXOffset;								// Images X Offset
	int m_iYOffset;								// Images Y Offset
#endif

	bool xmlSaveTree(HTREEITEM hFromItem, const TString &name, TString &filename);
	bool xmlGetItems(const HTREEITEM hFirstSibling, TiXmlElement *xElm, TCHAR *buf);
	HTREEITEM xmlLoadTree(HTREEITEM hInsertAfter, HTREEITEM hParent, const TString &name, TString &filename);
	const TiXmlElement *xmlInsertItems(HTREEITEM hParent, HTREEITEM &hInsertAfter, const TiXmlElement *xElm);
};

#endif // _DCXTREEVIEW_H_
