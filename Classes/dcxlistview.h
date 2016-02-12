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

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/dcxprogressbar.h"
#include "Classes/tinyxml/tinyxml.h"

class DcxDialog;

#define LVIS_BOLD       0x00000100	//!< ListView Caption Bold Style
#define LVIS_UNDERLINE  0x00000200	//!< ListView Caption Underline Style
#define LVIS_COLOR      0x00000400	//!< ListView Caption Color Style
#define LVIS_BGCOLOR    0x00000800	//!< ListView Caption Background Color Style
#define LVIS_PBAR       0x00001000	//!< ListView ProgressBar Style
#define LVIS_ITALIC     0x00002000	//!< ListView Caption Italic Style
#define LVIS_HASHITEM	0x00004000	//!< ListView Item takes it's text from a hashtable. (text is `table item`) (to be replaced by LVIS_HASHTABLE)
#define LVIS_HASHNUMBER	0x00008000	//!< ListView Item takes it's text from a hashtable. (text is `table N`) (to be replaced by LVIS_HASHTABLE)
#define LVIS_XML		0x00010000	//!< ListView Item takes it's text from a xml file. (text is `dataset_name filename`) (adds all items in dataset_name)
#define LVIS_HASHTABLE	0x00020000	//!< ListView Item takes it's text from a hashtable. (text is `+flags hashtable (N|N1,N2|item)`) (can add multiple items)
#define LVIS_WINDOW		0x00040000	//!< ListView Item takes it's text from a custom window. (text is `+flags window (N|N1,N2)`) (can add multiple items)
#define LVIS_CONTROL	0x00080000	//!< ListView Item takes it's text from another DCX control. (text is `+flags dialog id (N|N1,N2)`) (can add multiple items)
#define LVIS_CENTERICON	0x00100000	//!< ListView Item displays the icon centered if no text present.

// +flags mentioned above tell the parser extra details about how the items are added.
// +	:	single item is added as text only (N)
// +a	:	items added include all info for the item not just it's text.
// +n	:	Numeric range supplied N1,N2
// +i	:	item name supplied (for hashtable)
// +A	:	adds ALL items starting at N
#define LVIMF_ALLINFO	0x01
#define LVIMF_NUMERIC	0x02
#define LVIMF_NAMED		0x04
#define LVIMF_ADDALL	0x08

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

#define LVSCW_AUTOSIZE_MAX	-3	// Max of LVSCW_AUTOSIZE & LVSCW_AUTOSIZE_USEHEADER

/*!
 * \brief blah
 *
 * blah
 */

struct DCXLVSORT {

	TString tsCustomAlias;  //!< Custom Sorting Alias
	UINT iSortFlags;        //!< Sorting Flags
	HWND m_Hwnd;            //!< ListView Handle
	int nColumn;            //!< Sorting Column
	TCHAR itemtext1[MIRC_BUFFER_SIZE_CCH];
	TCHAR itemtext2[MIRC_BUFFER_SIZE_CCH];
};
using LPDCXLVSORT = DCXLVSORT *;

struct DCXLVRENDERINFO {
	DWORD		m_dFlags;	//!< Render flags (bold etc..)
	COLORREF	m_cText;	//!< Text Colour
	COLORREF	m_cBg;		//!< Background Colour.
};
using LPDCXLVRENDERINFO = DCXLVRENDERINFO *;

using VectorOfRenderInfo = std::vector<LPDCXLVRENDERINFO>;

/*!
* \brief blah
*
* blah
*/

#define DCX_LV_COLUMNF_AUTO			1		// uses LVSCW_AUTOSIZE
#define DCX_LV_COLUMNF_AUTOHEADER	2		// uses LVSCW_AUTOSIZE_USEHEADER
#define DCX_LV_COLUMNF_AUTOMAX		4		// uses both LVSCW_AUTOSIZE & LVSCW_AUTOSIZE_USEHEADER to find the largest.
#define DCX_LV_COLUMNF_PERCENT		8		// m_iSize is a % width between 0 & 100 (zero width columns are hidden)
#define DCX_LV_COLUMNF_FIXED		16		// m_iSize is the fixed width of the column

struct DCXLVCOLUMNINFO {
	int			m_iColumn;	// the column affected by this info.
	DWORD		m_dFlags;	// size flags (autosize, % width etc..)
	int			m_iSize;	// size of column (meaning depends on flags)
};
using LPDCXLVCOLUMNINFO = DCXLVCOLUMNINFO *;

using VectorOfColumnInfo = std::vector<LPDCXLVCOLUMNINFO>;
//using ColumnWidths = std::map<int, int>;

/*!
 * \brief blah
 *
 * blah
 */

struct DCXLVITEM {
	TString tsTipText;	//!< Tooltip text
	TString tsMark;		// Marked text
	DcxControl *pbar;
	//DcxProgressBar *pbar;
	int iPbarCol;
	VectorOfRenderInfo	vInfo;	//!< Render Info for each colum
};
using LPDCXLVITEM = DCXLVITEM *;

/*!
 * \brief blah
 *
 * blah
 */

class DcxListView : public DcxControl {

public:
	DcxListView() = delete;
	DcxListView(const DcxListView &) = delete;
	DcxListView &operator =(const DcxListView &) = delete;	// No assignments!

	DcxListView(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxListView( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	HIMAGELIST getImageList(const int iImageList) const;
	void setImageList( const HIMAGELIST himl, const int iImageList );

	bool isListViewStyle( const DWORD dwView ) const;

	const int &getColumnCount( ) const;

	inline const TString getType() const override { return TEXT("listview"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::LISTVIEW; }

	int getTopIndex( ) const;
	int getBottomIndex( ) const;
	const TString getStyles(void) const override;

protected:

	WNDPROC m_OrigEditProc; //!< Label Edit Control Orignal Procedure

	static UINT parseIconFlagOptions( const TString & flags );
	static UINT parseItemFlags( const TString & flags );
	static UINT parseHeaderFlags( const TString & flags );
	static INT parseHeaderFlags2( const TString & flags );
	static INT parseHeaderFlags2( const XSwitchFlags & xflags );
	static UINT parseSortFlags( const TString & flags );
	static UINT parseColorFlags( const TString & flags );
	static UINT parseImageFlags( const TString & flags );
	static UINT parseGroupFlags( const TString & flags );
	static UINT parseGroupState( const TString & flags );

	bool matchItemText(const int nItem, const int nSubItem, const TString &search, const DcxSearchTypes &SearchType) const;

	void autoSize(const int nColumn, const TString &flags);
	void autoSize(const int nColumn, const int iFlags , const int iWidth = 0);

	//bool m_bDrag; //!< Dragging Items ?

private:
	DcxControl* CreatePbar(LPLVITEM lvi, const TString &style);
	void ScrollPbars(const int row, const int nCols, const int iTop, const int iBottom, LPLVITEM lvi);
	void UpdateScrollPbars(void);
	HIMAGELIST initImageList(const int iImageList);
	bool xmlLoadListview(const int nPos, const TString &dataset, TString &filename);
	void xmlSetItem(const int nItem, const int nSubItem, const TiXmlElement *xNode, LPLVITEM lvi, LPDCXLVITEM lpmylvi, TString &tsBuf);
	bool ctrlLoadListview(const int nPos, const TString &tsData);
	bool xLoadListview(const int nPos, const TString &tsData, const TCHAR *sTest, const TCHAR *sCount, const TCHAR *sGet, const TCHAR *sGetNamed);
	void massSetItem(const int nPos, const TString &input);
	void setHeaderStyle(HWND h, const int nCol, const TString &info);
	void DeleteColumns(const int nColumn);
	TString ItemToString(int nItem, int iColumns);
	bool xSaveListview(const int nStartPos, const int nEndPos, const TString & tsData, const TCHAR * sTestCommand, const TCHAR * sStoreCommand);

	// static functions.
	static void getItemRange(const TString &tsItems, const int nItemCnt, int *iStart_range, int *iEnd_range);
	static UINT parseMassItemFlags( const TString & flags );
	static void parseText2Item(const TString & tsTxt, TString & tsItem, const TString &tsData);
	static int CALLBACK sortItemsEx(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static LRESULT CALLBACK EditLabelProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static HIMAGELIST createImageList(const BOOL bIcons);
	static void parseListviewExStyles(const TString & styles, LONG * ExStyles);

	//
	HFONT m_hItemFont;					// Font used for specific item changes.
	HFONT m_hOldItemFont;				// Font used for specific item changes.
	int m_iSelectedItem;				// Items currently selected.
	int m_iSelectedSubItem;				// SubItems currently selected.
	mutable int m_iColumnCount;			// the number of columns in the listview, a -1 value mean "dont know"
	VectorOfColumnInfo	m_vWidths;		// column widths for dynamic sizing of columns.
	bool m_bHasPBars;					// true if listview has pbars at all, if it does, a slower update is used that checks & moves pbars. (better system needed)
	bool m_bReserved[3];
};

#endif // _DCXLISTVIEW_H_
