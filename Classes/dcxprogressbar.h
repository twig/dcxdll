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

class DcxProgressBar : public DcxControl {

public:
	DcxProgressBar() = delete;
	DcxProgressBar(const DcxProgressBar &) = delete;
	DcxProgressBar &operator =(const DcxProgressBar &) = delete;	// No assignments!

	DcxProgressBar(_In_ const UINT ID, _In_ DcxDialog *const p_Dialog, _In_ const HWND mParentHwnd, _In_ const RECT *const rc, _In_ const TString & styles);
	virtual ~DcxProgressBar( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest( const TString & input ) override;
	void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) override;

	LRESULT setPosition( const int nNewPos );
	LRESULT setRange( const int iLowLim, const int iHighLim );

	LRESULT setMarquee( const BOOL fStart, const int fTime );
	LRESULT stepIt( );
	LRESULT setStep( const int nStepInc );
	LRESULT setBarColor( const COLORREF clrBar );
	LRESULT setBKColor( const COLORREF clrBk );

	LRESULT getPosition( ) const;
	LRESULT getRange( const BOOL fWhichLimit, PPBRANGE ppBRange ) const;
	void toXml(TiXmlElement *const xml) const override;

	auto getPredictedPos(LPARAM lParam, const int iLower, const int iHigher) const
	{
		const auto nXPos = LOWORD(lParam);
		const auto nYPos = HIWORD(lParam);
		auto nPos = 0;

		RECT rc;
		if (GetClientRect(m_Hwnd, &rc))
		{
			if (this->isStyle(PBS_VERTICAL))
				nPos = iHigher - dcx_round((float)(nYPos * iHigher) / (rc.bottom - rc.top - 1));
			else
				nPos = iLower + dcx_round((float)(nXPos * iHigher) / (rc.right - rc.left - 1));
		}
		return nPos;
	}
	inline const TString getType() const override { return TEXT("pbar"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::PROGRESSBAR; }

	const TString getStyles(void) const override;

protected:

	//COLORREF m_clrText;		//!< Caption Text Color
	TString m_tsText;			//!< Caption Text
	HFONT m_hfontVertical;
	bool m_bIsAbsoluteValue;	//!< Caption Numerical Placeholder Format
	bool m_bIsGrad;				//!< Draw Gradient?
	bool m_bReserved[2];
	//COLORREF m_clrGrad;		//!< Gradients Color
private:
	int CalculatePosition(void) const;
	void DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam);
};

#endif // _DCXPROGRESSBAR_H_
