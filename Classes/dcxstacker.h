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

#define STACKERS_GRAD			0x01
#define STACKERS_IMAGE		0x02
#define STACKERS_ARROW		0x04
#define STACKERS_COLLAPSE	0x08

#define ST_ERR	-1

#define STACKERF_SELECTED	0x01

struct DCXSITEM {
	TString			tsTipText;			//!< Tooltip text
	TString			tsCaption;			//!< Title Buttons text
	COLORREF		clrBack;			//!< Line Background Caption Color
	COLORREF		clrText;			//!< Line Caption Color
	HFONT			hFont;				//!< Items font.
	DcxControl		*pChild;			//!< Items child control
	int				iItemImg;			//!< Items Normal Image index.
	int				iSelectedItemImg;	//!< Items Selected Image index.
	RECT			itemrc;				//!< Items Rect.
	DWORD			dFlags;				//!< Items flags.
};
using LPDCXSITEM = DCXSITEM *;

using VectorOfImages = std::vector<Gdiplus::Image *>;
using VectorOfStackerItems = std::vector<LPDCXSITEM>;

/*!
 * \brief blah
 *
 * blah
 */

class DcxStacker : public DcxControl {

public:
	DcxStacker() = delete;
	DcxStacker(const DcxStacker &) = delete;
	DcxStacker &operator =(const DcxStacker &) = delete;	// No assignments!

	DcxStacker( const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxStacker( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest( const TString & input ) override;
	void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) override;

	inline const TString getType() const override { return TEXT("stacker"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::STACKER; }

	const TString getStyles(void) const override;

protected:
	HWND m_hActive;
	DWORD m_dStyles;
	VectorOfImages m_vImageList;
	VectorOfStackerItems m_vItems;

	//
	int getItemID(void) const;
	int getSelItemID(void) const;
	DWORD getItemCount(void) const;
	LPDCXSITEM getItem(const int nPos) const;
	LPDCXSITEM getHotItem(void) const;
	void getItemRect(const int nPos, LPRECT rc) const;
	//
	//int setItem(int nPos, LPDCXSITEM item);
	//int setSelectedItem(int nPos);
	//
	//void deleteItem(int nPos);
	//
	void DrawSItem(const LPDRAWITEMSTRUCT idata);
	static void DrawAliasedTriangle(const HDC hdc, const LPRECT rc, const COLORREF clrShape);
	void DrawItemImage(const HDC hdc, Gdiplus::Image *const img, const LPRECT rc);
	//
	void clearImageList(void);
	void clearItemList(void);
	//
	void calcItemRect(LPRECT rc);
};

#endif //_DCXSTACKER_H_
