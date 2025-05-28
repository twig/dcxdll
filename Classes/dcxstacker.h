/*!
 * \file dcxstacker.h
 * \brief blah
 *
 * blah
 *
 * \author Ook
 * \version 1.0
 *
 * \b Revisions
 *
 * © ScriptsDB.org - 2006
 */

#ifndef _DCXSTACKER_H_
#define _DCXSTACKER_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/xpopup/xpopupmenuitem.h"

#define MIN_STACK_HEIGHT 20 //!< Min height for a stacker item.

#define STACKERS_GRAD		0x01
#define STACKERS_IMAGE		0x02
#define STACKERS_ARROW		0x04
#define STACKERS_COLLAPSE	0x08
#define STACKERS_FLOAT		0x10

#define ST_ERR	-1

#define STACKERF_SELECTED	0x01

struct DCXSITEM
{
	TString			tsTipText;			//!< Tooltip text
	TString			tsCaption;			//!< Title Buttons text
	COLORREF		clrBack{ CLR_INVALID };			//!< Line Background Caption Color
	COLORREF		clrText{ CLR_INVALID };			//!< Line Caption Color
	HFONT			hFont{ nullptr };				//!< Items font.
	DcxControl* pChild{ nullptr };			//!< Items child control
	int				iItemImg{ -1 };			//!< Items Normal Image index.
	int				iSelectedItemImg{ -1 };	//!< Items Selected Image index.
	RECT			itemrc{};				//!< Items Rect.
	DWORD			dFlags{};				//!< Items flags.
};
using LPDCXSITEM = DCXSITEM *;

using VectorOfImages = std::vector<std::unique_ptr<Gdiplus::Image>>;
using VectorOfStackerItems = std::vector<LPDCXSITEM>;

class DcxStacker final
	: public DcxControl
{
public:
	DcxStacker() = delete;
	DcxStacker(const DcxStacker&) = delete;
	GSL_SUPPRESS(c.128) DcxStacker& operator =(const DcxStacker&) = delete;
	DcxStacker(DcxStacker&&) = delete;
	GSL_SUPPRESS(c.128) DcxStacker& operator =(DcxStacker&&) = delete;

	DcxStacker(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxStacker() noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;

	void loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc) override;

	inline const TString getType() const final { return TEXT("stacker"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::STACKER; }

	const TString getStyles(void) const final;
	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml(void) const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

protected:
	HWND m_hActive{ nullptr };
	DWORD m_dStyles{};
	VectorOfImages m_vImageList;
	VectorOfStackerItems m_vItems;

	//
	int getItemID(void) const noexcept;
	int getSelItemID(void) const noexcept;
	DWORD getItemCount(void) const noexcept;
	LPDCXSITEM getItem(const int nPos) const noexcept;
	LPDCXSITEM getHotItem(void) const noexcept;
	void getItemRect(const int nPos, LPCRECT rc) const noexcept;

	//
	//int setItem(int nPos, LPDCXSITEM item);
	//int setSelectedItem(int nPos);
	//
	//void deleteItem(int nPos);
	//

	void DrawSItem(const LPDRAWITEMSTRUCT idata);
	static void DrawAliasedTriangle(const HDC hdc, const LPCRECT rc, const COLORREF clrShape);
#ifdef DCX_USE_GDIPLUS
	void DrawItemImage(const HDC hdc, Gdiplus::Image* const img, const LPCRECT rc);
#endif
	//
	void clearImageList(void) noexcept;
	void clearItemList(void) noexcept;
	//
	void calcItemRect(LPRECT rc);
};

#endif //_DCXSTACKER_H_
