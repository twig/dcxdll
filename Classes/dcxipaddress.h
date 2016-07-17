/*!
 * \file dcxipaddress.h
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

#ifndef _DCXIPADDRESS_H_
#define _DCXIPADDRESS_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxIpAddress : public DcxControl {

public:
	DcxIpAddress() = delete;
	DcxIpAddress(const DcxIpAddress &) = delete;
	DcxIpAddress &operator =(const DcxIpAddress &) = delete;	// No assignments!

	DcxIpAddress(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxIpAddress( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	LRESULT setRange( const int nField, const BYTE iMin, const BYTE iMax );
	LRESULT setFocus( const int nField );
	LRESULT setAddress( const DWORD dwIpAddress );
	LRESULT getAddress( LPDWORD lpdwIpAddress ) const;
	LRESULT clearAddress( );

	inline const TString getType() const override { return TEXT("ipaddress"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::IPADDRESS; }

	void toXml(TiXmlElement *const xml) const override;

protected:

};

#endif // _DCXIPADDRESS_H_
