/*!
 * \file dcxprogressbar.h
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

#ifndef _DCXPROGRESSBAR_H_
#define _DCXPROGRESSBAR_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/xpopup/xpopupmenuitem.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxProgressBar final
	: public DcxControl
{
public:
	DcxProgressBar() = delete;
	DcxProgressBar(const DcxProgressBar &) = delete;
	GSL_SUPPRESS(c.128) DcxProgressBar &operator =(const DcxProgressBar &) = delete;	// No assignments!
	DcxProgressBar(DcxProgressBar &&) = delete;
	GSL_SUPPRESS(c.128) DcxProgressBar &operator =(DcxProgressBar &&) = delete;

	DcxProgressBar(_In_ const UINT ID, _In_ gsl::strict_not_null<DcxDialog* const> p_Dialog, _In_ const HWND mParentHwnd, _In_ const RECT *const rc, _In_ const TString & styles);
	~DcxProgressBar( ) noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) noexcept final;

	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest( const TString & input ) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	LRESULT setPosition( const int nNewPos ) noexcept;
	LRESULT setRange( const int iLowLim, const int iHighLim ) noexcept;

	LRESULT setMarquee( const BOOL fStart, const int fTime ) noexcept;
	LRESULT stepIt( ) noexcept;
	LRESULT setStep( const int nStepInc ) noexcept;
	LRESULT setBarColor( const COLORREF clrBar ) noexcept;
	LRESULT setBKColor( const COLORREF clrBk ) noexcept;

	LRESULT getPosition( ) const noexcept;
	LRESULT getRange( const BOOL fWhichLimit, const PPBRANGE ppBRange ) const noexcept;

	int getStep() const noexcept;
	COLORREF getBarColor() const noexcept;
	COLORREF getBKColor() const noexcept;

	const TString getStyles(void) const final;

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

	auto getPredictedPos(LPARAM lParam, const int iLower, const int iHigher) const noexcept
	{
		const auto nXPos = Dcx::dcxLOWORD(lParam);
		const auto nYPos = Dcx::dcxHIWORD(lParam);
		auto nPos = 0;

		if (!m_Hwnd)
			return nPos;

		if (RECT rc{}; GetClientRect(m_Hwnd, &rc))
		{
			if (this->isStyle(WindowStyle::PBS_Vertical))
				nPos = iHigher - dcx_round(gsl::narrow_cast<float>((nYPos * iHigher)) / (rc.bottom - rc.top - 1));
			else
				nPos = iLower + dcx_round(gsl::narrow_cast<float>((nXPos * iHigher)) / (rc.right - rc.left - 1));
		}
		return nPos;
	}

	inline const TString getType() const final { return TEXT("pbar"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::PROGRESSBAR; }

protected:

	TString m_tsText;					//!< Caption Text
	HFONT m_hfontVertical{ nullptr };
	bool m_bIsAbsoluteValue{ false };	//!< Caption Numerical Placeholder Format
	bool m_bIsGrad{ false };			//!< Draw Gradient?
	bool m_bReserved[2]{ false };

private:
	int CalculatePosition(void) const noexcept;
	void DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam);
};

#endif // _DCXPROGRESSBAR_H_
