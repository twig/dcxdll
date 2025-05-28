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
#include "Classes\custom\SearchHelper.h"

#include <string>

#include "Classes/tinyxml/tinyxml.h"

#ifdef DCX_USE_TESTCODE
 // enables custom drawing for treeview
#define USE_CUSTOM_TREE_DRAWING 1
#endif

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
#define TVCOLOR_SB 0x10        //!< TreeView Selection Border Color

#define TVIS_DCXMASK		0xFF0000
#define TVIS_UNDERLINE		0x010000 //!< TreeView Caption Underline Style
#define TVIS_COLOR			0x020000 //!< TreeView Caption Color Style
#define TVIS_ITALIC			0x040000 //!< TreeView Caption Italic Style
#define TVIS_BKG			0x080000 //!< TreeView Item blackground colour style.
#define TVIS_HASHITEM		0x100000 //!< TreeView item text is taken from a hash table item
#define TVIS_HASHNUMBER		0x200000 //!< TreeView item text is taken from a hash tabel item number
#define TVIS_XML			0x400000 //!< TreeView item text is taken from an xml file.
#define TVIS_EXPANDEDICON	0x800000 //!< TreeView item has an expended icon defined.

#define TVIS_UNDOC			0x1000	//!< Undocumented extended style

#ifndef TVIS_EX_HWND
#define TVIS_EX_HWND		0x04	//!< Undefined extended state
#endif

#define TVIE_EXP     0x01     //!< TreeView Expand Branch Option
#define TVIE_EXPALL  0x02     //!< TreeView Expandall Branch Option
#define TVIE_EXPPART 0x04     //!< TreeView ExpandPartial Branch Option
#define TVIE_COL     0x08     //!< TreeView Collapse Branch Option
#define TVIE_COLALL  0x10     //!< TreeView Collapseall Branch Option
#define TVIE_COLRES  0x20     //!< TreeView Collapse/Reset Branch Option
#define TVIE_TOGGLE  0x40     //!< TreeView Toogle Branch Option

#define TVIT_NORMAL 0x01      //!< TreeView Normal ImageList
#define TVIT_STATE  0x02      //!< TreeView State ImageList

struct DCXTVSORT
{
	TString		tsCustomAlias;						//!< Custom Sorting Alias
	UINT		iSortFlags{};						//!< Sorting Flags
	DcxTreeView* pthis{ nullptr };					//!< TreeView control object pointer
	TCHAR		itemtext1[MIRC_BUFFER_SIZE_CCH]{};	// Item text buffer One
	TCHAR		itemtext2[MIRC_BUFFER_SIZE_CCH]{};	// Item Text Buffer Two

	DCXTVSORT() = default;

	bool operator==(const DCXTVSORT& other) const = default;

	DCXTVSORT(const TString& tsCustomAlias, const UINT& iSortFlags, DcxTreeView* pthis)
		: tsCustomAlias(tsCustomAlias), iSortFlags(iSortFlags), pthis(pthis)
	{
	}
};
using LPDCXTVSORT = DCXTVSORT*;

 // text, background, line, insert colours alrdy set elsewhere.
struct DCXTVCOLOURS
{
	COLORREF	m_clrSelected{ CLR_INVALID };		//!< Colour of selection box.
	COLORREF	m_clrSelectionBorder{ CLR_INVALID };//!< Colour of selection boxes border.

	DCXTVCOLOURS() = default;

	bool operator==(const DCXTVCOLOURS& other) const = default;

	DCXTVCOLOURS(const COLORREF& m_clrSelected, const COLORREF& m_clrSelectionBorder) noexcept
		: m_clrSelected(m_clrSelected), m_clrSelectionBorder(m_clrSelectionBorder)
	{
	}
};

struct DCXTVITEM
{
	TString		tsTipText;				//!< Tooltip text
	TString		tsMark;					//!< Marked item text.
	COLORREF	clrText{ CLR_INVALID };	//!< Item Caption Color
	COLORREF	clrBkg{ CLR_INVALID };	//!< Item background colour.
	HTREEITEM	hHandle{ nullptr };		//!< TreeView Item Handle (used for sorting)
	bool		bBold{ false };			//!< Is Item Caption Bold ?
	bool		bUline{ false };		//!< Is Item Caption Underlined
	bool		bItalic{ false };		//!< Is Item Caption Italicised

	DCXTVITEM() = default;

	bool operator==(const DCXTVITEM& other) const = default;

	DCXTVITEM(const TString& tsTipText, const TString& tsMark, const COLORREF& clrText, const COLORREF& clrBkg, const HTREEITEM& hHandle, bool bBold, bool bUline, bool bItalic)
		: tsTipText(tsTipText), tsMark(tsMark), clrText(clrText), clrBkg(clrBkg), hHandle(hHandle), bBold(bBold), bUline(bUline), bItalic(bItalic)
	{
	}
};
using LPDCXTVITEM = DCXTVITEM*;

class DcxTreeView final
	: public DcxControl
	, virtual public DcxSearchHelper
{
public:
	DcxTreeView() = delete;
	DcxTreeView(const DcxTreeView&) = delete;
	GSL_SUPPRESS(c.128) DcxTreeView& operator =(const DcxTreeView&) = delete;
	DcxTreeView(DcxTreeView&&) = delete;
	GSL_SUPPRESS(c.128) DcxTreeView& operator =(DcxTreeView&&) = delete;

	DcxTreeView(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxTreeView() noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;

	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;

	[[nodiscard]] HIMAGELIST getImageList(const int type) const noexcept;
	void setImageList(const HIMAGELIST himl, const int type) noexcept;
	[[nodiscard]] HIMAGELIST createImageList() noexcept;

	void insertItem(const TString& tsPath, const TString& tsData, const TString& tsTooltip);

	void getItemText(const HTREEITEM hItem, TCHAR* szBuffer, const int cchTextMax) const noexcept;
	[[nodiscard]] TString getItemText(const HTREEITEM hItem) const;
	[[nodiscard]] int getChildCount(const HTREEITEM hParent) const noexcept;
	[[nodiscard]] LPDCXTVITEM getItemParam(const HTREEITEM hItem) const noexcept;
	[[nodiscard]] int getItemImageID(const HTREEITEM hItem) const noexcept;

	void loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc) override;

	inline const TString getType() const final { return TEXT("treeview"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::TREEVIEW; }

	const TString getStyles() const final;
	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml() const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

protected:

	WNDPROC m_OrigEditProc{ nullptr }; //!< Label Edit Control Orignal Procedure

	DcxIconSizes m_iIconSize{ DcxIconSizes::SmallIcon }; //!< Icon size

	bool m_bDestroying{ false }; //!< this flag is set when the listview is about to get destroyed to avoid senseless events
	bool m_bCustomDraw{ false };

	DCXTVCOLOURS m_SelectionColours;

	/* *** */

	[[nodiscard]] HTREEITEM parsePath(const TString& path, HTREEITEM* hParent = nullptr, HTREEITEM* hInsertAt = nullptr) const;
	[[nodiscard]] TString getPathFromItem(const HTREEITEM item) const;

	[[nodiscard]] bool matchItemText(const HTREEITEM hItem, const TString& search, const DcxSearchTypes& SearchType) const;
	[[nodiscard]] std::optional<HTREEITEM> findItemText(const HTREEITEM hStart, const TString& queryText, const int n, int& matchCount, const DcxSearchTypes& SearchType) const;

	[[nodiscard]] bool matchItemText(const HTREEITEM hItem, const dcxSearchData& srch_data) const;
	[[nodiscard]] std::optional<HTREEITEM> findItemText(const HTREEITEM hStart, const int n, int& matchCount, const dcxSearchData& srch_data) const;

	void expandAllItems(const HTREEITEM hStart, const UINT expandOption) noexcept;

	[[nodiscard]] HTREEITEM cloneItem(const HTREEITEM hItem, const HTREEITEM hParentTo, const HTREEITEM hAfterTo);
	void copyAllItems(const HTREEITEM hItem, const HTREEITEM hParentTo);
	HTREEITEM copyAllItems(const TString& pathFrom, const TString& pathTo);

	[[nodiscard]] static UINT parseIconFlagOptions(const TString& flags) noexcept;
	[[nodiscard]] static UINT parseItemFlags(const TString& flags) noexcept;
	[[nodiscard]] static UINT parseSortFlags(const TString& flags) noexcept;
	[[nodiscard]] static UINT parseColorFlags(const TString& flags) noexcept;
	[[nodiscard]] static UINT parseToggleFlags(const TString& flags) noexcept;
	static int CALLBACK sortItemsEx(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	[[nodiscard]] WindowExStyle parseTreeViewExStyles(const TString& styles) const;
	static LRESULT CALLBACK EditLabelProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

	HFONT m_hItemFont{ nullptr }; // Font used for specific item changes.
	HFONT m_hOldItemFont{ nullptr }; // Font used for specific item changes.

	void DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam);
	void PreloadData() noexcept;

#ifdef DCX_USE_GDIPLUS
	void LoadGDIPlusImage(const TString& flags, TString& filename);
	void DrawGDIPlusImage(HDC hdc);

	dcxImage m_pImage;																// Background Image
	Gdiplus::CompositingQuality m_CQuality{ Gdiplus::CompositingQualityDefault };	// Image Rendering Quality
	Gdiplus::CompositingMode m_CMode{ Gdiplus::CompositingModeSourceCopy };			// Image Rendering Mode
	Gdiplus::InterpolationMode m_IMode{ Gdiplus::InterpolationModeDefault };		//
	Gdiplus::SmoothingMode m_SMode{ Gdiplus::SmoothingModeDefault };				// Image Smoothing Mode
	bool m_bTileImage{ false };						// Tile Image?
	bool m_bResizeImage{ false };					// Resize Image?
	bool m_bTransparent{ false };					// Is Control Transparent?
	int m_iXOffset{};								// Images X Offset
	int m_iYOffset{};								// Images Y Offset
#endif

	void xmlSaveTree(HTREEITEM hFromItem, const TString& name, TString& filename);
	[[nodiscard]] bool xmlGetItems(const HTREEITEM hFirstSibling, TiXmlElement* xElm, TCHAR* buf) const;
	HTREEITEM xmlLoadTree(HTREEITEM hInsertAfter, HTREEITEM hParent, const TString& name, TString& filename);
	const TiXmlElement* xmlInsertItems(HTREEITEM hParent, HTREEITEM& hInsertAfter, const TiXmlElement* xElm);

	[[nodiscard]] const size_t size() const noexcept
	{
		return Dcx::dcxTreeView_GetCount(m_Hwnd);
	}

	[[nodiscard]] HIMAGELIST TV_GetNormalImageList() noexcept
	{
		auto himl = this->getImageList(TVSIL_NORMAL);
		if (!himl)
		{
			himl = createImageList();

			if (himl)
				this->setImageList(himl, TVSIL_NORMAL);
		}
		return himl;
	}
	[[nodiscard]] HIMAGELIST TV_GetStateImageList() noexcept
	{
		auto himl = this->getImageList(TVSIL_STATE);
		if (!himl)
		{
			himl = this->createImageList();

			if (himl)
				this->setImageList(himl, TVSIL_STATE);
		}
		return himl;
	}
	void TV_RemoveNormalImageList() noexcept
	{
		if (const auto himl = this->getImageList(TVSIL_NORMAL); himl)
		{
			ImageList_Destroy(himl);
			this->setImageList(nullptr, TVSIL_NORMAL);
		}
	}
	void TV_RemoveStateImageList() noexcept
	{
		if (const auto himl = this->getImageList(TVSIL_STATE); himl)
		{
			ImageList_Destroy(himl);
			this->setImageList(nullptr, TVSIL_STATE);
		}
	}
};

#endif // _DCXTREEVIEW_H_
