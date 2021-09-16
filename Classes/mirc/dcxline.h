/*!
 * \file dcxline.h
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

#ifndef _DCXLINE_H_
#define _DCXLINE_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxLine final
	: public DcxControl
{
public:
	DcxLine() = delete;
	DcxLine(const DcxLine &) = delete;
	DcxLine &operator =(const DcxLine &) = delete;	// No assignments!
	DcxLine(DcxLine &&) = delete;
	DcxLine &operator =(DcxLine &&) = delete;

	DcxLine( const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxLine( ) noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) noexcept final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("line"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::LINE; }

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	const TString getStyles(void) const final;

	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;
	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure

private:
	void DrawClientArea(HDC hdc);

	TString m_sText;
	bool m_bVertical{ false };
	bool m_bSimple{ false };
};

#endif // _DCXLINE_H_
