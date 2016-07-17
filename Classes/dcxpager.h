/*
	* Pager Control
*/

#ifndef _DCXPAGER_H_
#define _DCXPAGER_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/layout/layoutmanager.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxPager : public DcxControl {

public:
	DcxPager() = delete;
	DcxPager(const DcxPager &) = delete;
	DcxPager &operator =(const DcxPager &) = delete;	// No assignments!

	DcxPager(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxPager( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest( const TString & input ) override;
	void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) override;

	inline const TString getType() const override { return TEXT("pager"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::PAGER; }

	const TString getStyles(void) const override;
	void toXml(TiXmlElement *const xml) const override;

	void setChild(const HWND child);
	void setBkColor(const COLORREF c);
	void setBorderSize(const int bSize);
	void setButtonSize(const int bSize);
	void reCalcSize(void) const;

protected:
	HWND m_ChildHWND;

};

#endif // _DCXPAGER_H_
