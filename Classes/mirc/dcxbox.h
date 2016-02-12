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

	HWND mChildHwnd;    //!< Hwnd to child window
	HWND mBox;					//!< The Box control itself
	BOOL mState;				//!< Enable/Disable state

};
using LPDCXENUM = DCXENUM *;

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxBox : public DcxControl {

public:
	DcxBox() = delete;
	DcxBox(const DcxBox &) = delete;
	DcxBox &operator =(const DcxBox &) = delete;	// No assignments!

	DcxBox( const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxBox( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	void parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	inline const TString getType() const override { return TEXT("box"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::BOX; }

	void toXml(TiXmlElement *const xml) const override;
	const TString getStyles(void) const override;

protected:

	//static const UINT parseLayoutFlags( const TString & flags );
	static BOOL CALLBACK EnumBoxChildren(HWND hwnd,const DCXENUM *const de);
	void EraseBackground(HDC hdc);
	void DrawClientArea(HDC hdc);

	LONG m_iBoxStyles; //!< Custom Box Styles

	LayoutManager * m_pLayoutManager; //!< Layout Manager Object

	HWND m_TitleButton; //!< enable/disable button.
	HTHEME _hTheme;
};

#endif // _DCXBOX_H_
