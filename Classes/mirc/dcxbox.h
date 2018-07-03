/*!
 * \file dcxbox.h
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

#ifndef _DCXBOX_H_
#define _DCXBOX_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/layout/layoutmanager.h"

#define DCX_BOXTEXTSPACING 10

#define BOXS_LEFT		0x00 //!< Box Left Style
#define BOXS_CENTER		0x01 //!< Box Center Style
#define BOXS_RIGHT		0x02 //!< Box Right Style
#define BOXS_BOTTOM		0x04 //!< Box Bottom Style
#define BOXS_NONE		0x08 //!< Box No Border Style
#define BOXS_ROUNDED	0x10 //!< Box has rounded corners.
#define BOXS_CHECK		0x20 //!< Box has check button in title to enable/disable contents.
#define BOXS_RADIO		0x40 //!< Box has radio button in title to enable/disable contents.

struct DCXENUM {

	HWND mChildHwnd;	//!< Hwnd to child window
	HWND mBox;			//!< The Box control itself
	BOOL mState;		//!< Enable/Disable state

	//DCXENUM() noexcept
	//	: mChildHwnd(nullptr)
	//	, mBox(nullptr)
	//	, mState(FALSE)
	//{}
};
using LPDCXENUM = DCXENUM *;

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxBox
	: public DcxControl
{
public:
	DcxBox() = delete;
	DcxBox(const DcxBox &) = delete;
	DcxBox &operator =(const DcxBox &) = delete;
	DcxBox(DcxBox &&) = delete;
	DcxBox &operator =(DcxBox &&) = delete;

	//DCX_DELETE_CONTROL_METHODS(DcxBox);

	DcxBox( const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxBox( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) noexcept final;
	
	//void parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("box"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::BOX; }

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	const TString getStyles(void) const final;

	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;
	static WNDPROC m_hDefaultClassProc;	//!< Default window procedure

private:
	//static const UINT parseLayoutFlags( const TString & flags );
	static BOOL CALLBACK EnumBoxChildren(HWND hwnd,const DCXENUM *const de) noexcept;
	void EraseBackground(HDC hdc);
	void DrawClientArea(HDC hdc);
	void DrawBorder(HDC hdc, RECT &rc) noexcept;

	LONG m_iBoxStyles{}; //!< Custom Box Styles

	std::unique_ptr<LayoutManager> m_pLayoutManager; //!< Layout Manager Object

	//HWND m_TitleButton{ nullptr }; //!< enable/disable button.
	bool m_bTitleChecked{ false };
	HTHEME _hTheme{ nullptr };

};

#endif // _DCXBOX_H_
