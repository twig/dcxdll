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

class DcxCalendar final
	: public DcxControl
{
public:
	DcxCalendar() = delete;
	DcxCalendar(const DcxCalendar &) = delete;
	GSL_SUPPRESS(c.128) DcxCalendar &operator =(const DcxCalendar &) = delete;	// No assignments!
	DcxCalendar(DcxCalendar &&) = delete;
	GSL_SUPPRESS(c.128) DcxCalendar &operator =(DcxCalendar &&) = delete;

	DcxCalendar( const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxCalendar( ) noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("calendar"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::CALENDAR; }

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	const TString getStyles(void) const final;

	const TString getValue(void) const;
	long getCurrentCTime() const;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

private:
	MONTHDAYSTATE m_MonthDayStates[12];
};

#endif // _DCXCALENDAR_H_
