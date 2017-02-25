/*!
 * \file dcxcalendar.h
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

#ifndef _DCXCALENDAR_H_
#define _DCXCALENDAR_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

#define BOLDDAY(ds,iDay) if(iDay>0 && iDay<32) (ds)|=(0x00000001<<(iDay-1))

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxCalendar : public DcxControl {

public:
	DcxCalendar() = delete;
	DcxCalendar(const DcxCalendar &) = delete;
	DcxCalendar &operator =(const DcxCalendar &) = delete;	// No assignments!

	DcxCalendar( const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxCalendar( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	inline const TString getType() const override { return TEXT("calendar"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::CALENDAR; }

	void toXml(TiXmlElement *const xml) const override;
	TiXmlElement * toXml(void) const override;
	const TString getStyles(void) const override;
	const TString getValue(void) const;

private:
	MONTHDAYSTATE m_MonthDayStates[12];
};

#endif // _DCXCALENDAR_H_
