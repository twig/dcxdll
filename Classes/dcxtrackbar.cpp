/*!
http://www.codeproject.com/miscctrl/transparentslider.asp
 * \file dcxtrackbar.cpp
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
#include "defines.h"
#include "Classes/dcxtrackbar.h"
#include "Classes/dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxTrackBar::DcxTrackBar(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog),
	m_bUpdatingTooltip(false)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	this->m_Hwnd = CreateWindowEx(
		ExStyles,
		DCX_TRACKBARCLASS,
		nullptr,
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(this->m_Hwnd))
		throw std::runtime_error("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(this->m_Hwnd, L" ", L" ");
	
	this->m_hbmp[TBBMP_BACK] = nullptr;
	this->m_hbmp[TBBMP_THUMB] = nullptr;
	this->m_hbmp[TBBMP_THUMBDRAG] = nullptr;
	this->m_hbmp[TBBMP_CHANNEL] = nullptr;
	this->m_colTransparent = CLR_INVALID;

	// Keep track of the tooltip
	if (dcx_testflag(Styles, TBS_TOOLTIPS))
	{
		auto tooltip = (HWND)SendMessage(this->m_Hwnd, TBM_GETTOOLTIPS, NULL, NULL);

		if (tooltip != nullptr)
			this->m_ToolTipHWND = tooltip;
	}

	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxTrackBar::~DcxTrackBar( ) {
	if (this->m_hbmp[TBBMP_BACK] != nullptr)
		DeleteBitmap(this->m_hbmp[TBBMP_BACK]);
	if (this->m_hbmp[TBBMP_THUMB] != nullptr)
		DeleteBitmap(this->m_hbmp[TBBMP_THUMB]);
	if (this->m_hbmp[TBBMP_THUMBDRAG] != nullptr)
		DeleteBitmap(this->m_hbmp[TBBMP_THUMBDRAG]);
	if (this->m_hbmp[TBBMP_CHANNEL] != nullptr)
		DeleteBitmap(this->m_hbmp[TBBMP_CHANNEL]);

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTrackBar::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {
	*Styles |= TBS_FIXEDLENGTH;

#if TSTRING_PARTS
	for (const auto &tsStyle: styles)
	{
		if ( tsStyle == TEXT("autoticks") ) 
			*Styles |= TBS_AUTOTICKS;
		else if ( tsStyle == TEXT("both") ) 
			*Styles |= TBS_BOTH;
		else if ( tsStyle == TEXT("top") ) 
			*Styles |= TBS_TOP;		// == TBS_LEFT == 4
		else if ( tsStyle == TEXT("bottom") ) 
			*Styles |= TBS_BOTTOM;	// == TBS_RIGHT == 0, so never set.... should just remove TBS_RIGHT/TOP ?
		else if ( tsStyle == TEXT("left") ) 
			*Styles |= TBS_LEFT;	// == TBS_TOP == 4
		else if ( tsStyle == TEXT("right") ) 
			*Styles |= TBS_RIGHT;	// == TBS_BOTTOM == 0
		else if ( tsStyle == TEXT("select") ) 
			*Styles |= TBS_ENABLESELRANGE;
		else if ( tsStyle == TEXT("vertical") ) 
			*Styles |= TBS_VERT;
		else if ( tsStyle == TEXT("nothumb") ) 
			*Styles |= TBS_NOTHUMB;
		else if ( tsStyle == TEXT("noticks") ) 
			*Styles |= TBS_NOTICKS;
		else if ( tsStyle == TEXT("reversed") ) 
			*Styles |= TBS_REVERSED;
		else if ( tsStyle == TEXT("downisleft") ) 
			*Styles |= TBS_DOWNISLEFT;
		else if ( tsStyle == TEXT("tooltips") ) 
			*Styles |= TBS_TOOLTIPS;
		else if ( tsStyle == TEXT("transparentbkg") )
			*Styles |= TBS_TRANSPARENTBKGND;
	}
#else
	for (auto tsStyle(styles.getfirsttok(1)); !tsStyle.empty(); tsStyle = styles.getnexttok())
	{
		if ( tsStyle == TEXT("autoticks") ) 
			*Styles |= TBS_AUTOTICKS;
		else if ( tsStyle == TEXT("both") ) 
			*Styles |= TBS_BOTH;
		else if ( tsStyle == TEXT("top") ) 
			*Styles |= TBS_TOP;
		else if ( tsStyle == TEXT("bottom") ) 
			*Styles |= TBS_BOTTOM; // == TBS_RIGHT == 0, so never set.... should just remove TBS_RIGHT/TOP ?
		else if ( tsStyle == TEXT("left") ) 
			*Styles |= TBS_LEFT;
		else if ( tsStyle == TEXT("right") ) 
			*Styles |= TBS_RIGHT;
		else if ( tsStyle == TEXT("select") ) 
			*Styles |= TBS_ENABLESELRANGE;
		else if ( tsStyle == TEXT("vertical") ) 
			*Styles |= TBS_VERT;
		else if ( tsStyle == TEXT("nothumb") ) 
			*Styles |= TBS_NOTHUMB;
		else if ( tsStyle == TEXT("noticks") ) 
			*Styles |= TBS_NOTICKS;
		else if ( tsStyle == TEXT("reversed") ) 
			*Styles |= TBS_REVERSED;
		else if ( tsStyle == TEXT("downisleft") ) 
			*Styles |= TBS_DOWNISLEFT;
		else if ( tsStyle == TEXT("tooltips") ) 
			*Styles |= TBS_TOOLTIPS;
		else if ( tsStyle == TEXT("transparentbkg") )
			*Styles |= TBS_TRANSPARENTBKGND;
	}
#endif
	this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxTrackBar::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const
{
	const auto prop(input.getfirsttok(3));

	if ( prop == TEXT("value") )
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getPos( ) );
	else if ( prop == TEXT("range") )
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), this->getRangeMin( ), this->getRangeMax( ) );
	else if ( prop == TEXT("line") )
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getLineSize( ) );
	else if ( prop == TEXT("page") )
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getPageSize( ) );
	else if (prop == TEXT("selrange"))
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), this->getSelStart(), this->getSelEnd());
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}
/*!
* \brief blah
 *
 * \param input [NAME] [SWITCH] [ID] (OPTIONS)
 */

void DcxTrackBar::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -c [NAME] [ID] [SWITCH] [VALUE]
	if ( flags[TEXT('c')] && numtok > 3 ) {
#if TSTRING_TEMPLATES
		const auto lPosition = input.getnexttok().to_<long>();	// tok 4
#else
		const auto lPosition = (LONG)input.getnexttok().to_num();	// tok 4
#endif
		this->setTic( lPosition );
	}
	// xdid -g [NAME] [ID] [SWITCH] [FLAGS] [FILE]
	else if (flags[TEXT('g')] && numtok > 4) {
		const auto tflags = parseImageFlags(input.getnexttok());	// tok 4
		auto filename(input.getlasttoks().trim());				// tok 5, -1

		// background
		if (dcx_testflag(tflags, TBCS_BACK))
			this->m_hbmp[TBBMP_BACK] = dcxLoadBitmap(this->m_hbmp[TBBMP_BACK], filename);
		// thumb
		if (dcx_testflag(tflags, TBCS_THUMB))
			this->m_hbmp[TBBMP_THUMB] = dcxLoadBitmap(this->m_hbmp[TBBMP_THUMB], filename);
		// thumb hover
		if (dcx_testflag(tflags, TBCS_THUMBDRAG))
			this->m_hbmp[TBBMP_THUMBDRAG] = dcxLoadBitmap(this->m_hbmp[TBBMP_THUMBDRAG], filename);
		// channel
		if (dcx_testflag(tflags, TBCS_CHANNEL))
			this->m_hbmp[TBBMP_CHANNEL] = dcxLoadBitmap(this->m_hbmp[TBBMP_CHANNEL], filename);

		// these dont seem to work so dont bother calling it
		//this->redrawWindow();
		//InvalidateRect(this->m_Hwnd, nullptr, TRUE);
	}
	// xdid -j [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if ( flags[TEXT('j')] && numtok > 4 ) {

#if TSTRING_TEMPLATES
		const auto iMin = input.getnexttok().to_<long>();	// tok 4
		const auto iMax = input.getnexttok().to_<long>();	// tok 5
#else
		const auto iMin = (LONG)input.getnexttok().to_num();	// tok 4
		const auto iMax = (LONG)input.getnexttok().to_num();	// tok 5
#endif
		this->setSel( iMin, iMax );
	}
	// xdid -l [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('l')] && numtok > 3 ) {

#if TSTRING_TEMPLATES
		const auto lLineSize = input.getnexttok().to_<long>();	// tok 4
#else
		const auto lLineSize = (LONG)input.getnexttok().to_num();	// tok 4
#endif
		this->setLineSize( lLineSize );
	}
	// xdid -m [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('m')] && numtok > 3 ) {

#if TSTRING_TEMPLATES
		const auto lPageSize = input.getnexttok().to_<long>();	// tok 4
#else
		const auto lPageSize = (LONG)input.getnexttok().to_num();	// tok 4
#endif
		this->setPageSize( lPageSize );
	}
	// xdid -n [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('n')] && numtok > 3 ) {

#if TSTRING_TEMPLATES
		const auto iTicFreq = input.getnexttok().to_<long>();	// tok 4
#else
		const auto iTicFreq = input.getnexttok().to_int();	// tok 4
#endif
		this->setTicFreq( iTicFreq );
	}
	// xdid -q [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('q')] ) {

		this->clearTics( );
	}
	// xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if ( flags[TEXT('r')] && numtok > 4 ) {

#if TSTRING_TEMPLATES
		const auto lMinRange = input.getnexttok().to_<long>();	// tok 4
		const auto lMaxRange = input.getnexttok().to_<long>();	// tok 5
#else
		const auto lMinRange = (LONG)input.getnexttok().to_num();	// tok 4
		const auto lMaxRange = (LONG)input.getnexttok().to_num();	// tok 5
#endif

		this->setRangeMin( lMinRange );
		this->setRangeMax( lMaxRange );
	}
	// xdid -o [NAME] [ID] [SWITCH] [VALUE]
	else if (flags[TEXT('o')] && numtok > 3) {
#if TSTRING_TEMPLATES
		m_colTransparent = input.getnexttok().to_<COLORREF>();	// tok 4
#else
		m_colTransparent = (COLORREF)input.getnexttok( ).to_num();	// tok 4
#endif
		this->redrawWindow();
	}
	// xdid -t [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('t')] && numtok > 3 ) {

		const auto value(input.getnexttok());	// tok 4

		if ( value == TEXT("left") )
			this->setTipSide( TBTS_LEFT );
		else if ( value == TEXT("right") )
			this->setTipSide( TBTS_RIGHT );
		else if ( value == TEXT("bottom") )
			this->setTipSide( TBTS_BOTTOM );
		else
			this->setTipSide( TBTS_TOP );
	}
	// xdid -u [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('u')] && numtok > 3 ) {

#if TSTRING_TEMPLATES
		const auto lLength = input.getnexttok().to_<long>();	// tok 4
#else
		const auto lLength = (LONG)input.getnexttok().to_num();	// tok 4
#endif

		this->setThumbLength( lLength );
	}
	// xdid -v [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('v')] && numtok > 3 ) {

#if TSTRING_TEMPLATES
		const auto lPosition = input.getnexttok().to_<long>();	// tok 4
#else
		const auto lPosition = (LONG)input.getnexttok().to_num();	// tok 4
#endif

		this->setPos( lPosition );
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setRangeMin( const LONG iLowLim ) {
	return SendMessage( this->m_Hwnd, TBM_SETRANGEMIN, (WPARAM) TRUE, (LPARAM) iLowLim );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getRangeMin(  ) const {
	return SendMessage( this->m_Hwnd, TBM_GETRANGEMIN, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setRangeMax( const LONG iHighLim ) {
	return SendMessage( this->m_Hwnd, TBM_SETRANGEMAX, (WPARAM) TRUE, (LPARAM) iHighLim );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getRangeMax(  ) const {
	return SendMessage( this->m_Hwnd, TBM_GETRANGEMAX, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setRange( const LONG iLowLim, const LONG iHighLim ) {
	return SendMessage( this->m_Hwnd, TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG ( iLowLim, iHighLim ) );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setPos( const LONG lPosition ) {
	return SendMessage( this->m_Hwnd, TBM_SETPOS, (WPARAM) TRUE, (LPARAM) lPosition );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getPos(  ) const {
	return SendMessage( this->m_Hwnd, TBM_GETPOS, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

void DcxTrackBar::setTic( const LONG lPosition ) {
	SendMessage( this->m_Hwnd, TBM_SETTIC, (WPARAM) 0U, (LPARAM) lPosition );
}

/*!
* \brief blah
*
* blah
*/

void DcxTrackBar::setTicFreq( const LONG wFreq ) {
	SendMessage( this->m_Hwnd, TBM_SETTICFREQ, (WPARAM) wFreq, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::clearTics( ) {
	return SendMessage( this->m_Hwnd, TBM_CLEARTICS, (WPARAM) TRUE, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setTipSide( const int fLocation ) {
	return SendMessage( this->m_Hwnd, TBM_SETTIPSIDE, (WPARAM) fLocation, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setPageSize( const LONG lPageSize ) {
	return SendMessage( this->m_Hwnd, TBM_SETPAGESIZE, (WPARAM) 0U, (LPARAM) lPageSize );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getPageSize(  ) const {
	return SendMessage( this->m_Hwnd, TBM_GETPAGESIZE, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setLineSize( const LONG lLineSize ) {
	return SendMessage( this->m_Hwnd, TBM_SETLINESIZE, (WPARAM) 0U, (LPARAM) lLineSize );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getLineSize(  ) const {
	return SendMessage( this->m_Hwnd, TBM_GETLINESIZE, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setThumbLength( const UINT iLength ) {
	return SendMessage( this->m_Hwnd, TBM_SETTHUMBLENGTH, (WPARAM) iLength, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setSel( const LONG iLowLim, const LONG iHighLim ) {
	return SendMessage( this->m_Hwnd, TBM_SETSEL, (WPARAM) TRUE, (LPARAM) MAKELONG( iLowLim, iHighLim ) );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getSelStart(  ) const {
	return SendMessage( this->m_Hwnd, TBM_GETSELSTART, (WPARAM) 0, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getSelEnd(  ) const {
	return SendMessage( this->m_Hwnd, TBM_GETSELEND, (WPARAM) 0, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxTrackBar::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg ) {
		case WM_VSCROLL:
		case WM_HSCROLL:
			{
				switch (LOWORD(wParam)) {
				case TB_TOP:
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("top"), this->getUserID(), this->getPos());
					break;

				case TB_BOTTOM:
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("bottom"), this->getUserID(), this->getPos());
					break;

				case TB_PAGEUP:
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("pageup"), this->getUserID(), this->getPos());
					break;

				case TB_PAGEDOWN:
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("pagedown"), this->getUserID(), this->getPos());
					break;

				case TB_LINEUP:
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("lineup"), this->getUserID(), this->getPos());
					break;

				case TB_LINEDOWN:
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("linedown"), this->getUserID(), this->getPos());
					break;

				case TB_THUMBPOSITION:
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("trackend"), this->getUserID(), this->getPos());
					break;

				case SB_THUMBTRACK:
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("tracking"), this->getUserID(), this->getPos());
					break;
				}

				break;
			}


		case WM_NOTIFY: {
			dcxlParam(LPNMHDR, hdr);

			if (hdr == nullptr)
				break;

			switch (hdr->code) {
				case NM_CUSTOMDRAW: {
					// if (no items to draw)
					if (
						(this->m_hbmp[TBBMP_BACK] == nullptr) &&
						(this->m_hbmp[TBBMP_THUMB] == nullptr) &&
						(this->m_hbmp[TBBMP_THUMBDRAG] == nullptr) &&
						(this->m_hbmp[TBBMP_CHANNEL] == nullptr)
						)
						break;

					dcxlParam(LPNMCUSTOMDRAW, nmcd);
					if (nmcd == nullptr)
						break;

					auto hdc = nmcd->hdc;
					if (hdc == nullptr)
						break;

					bParsed = TRUE;

					switch (nmcd->dwDrawStage) {
						case CDDS_PREPAINT:
							return CDRF_NOTIFYITEMDRAW/* | CDRF_NOTIFYPOSTPAINT*/;

						case CDDS_ITEMPREPAINT: {
							// try to make it draw the tics, doesnt work =(
							if (nmcd->dwItemSpec == TBCD_TICS)
								return CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT;

							// channel that the trackbar control's thumb marker slides along
							if (nmcd->dwItemSpec == TBCD_CHANNEL) {
								// draw the background here
								DrawTrackBarPart(hdc, TBBMP_BACK);

								if (DrawTrackBarPart(hdc, TBBMP_CHANNEL, &nmcd->rc))
									return /*CDRF_NOTIFYPOSTPAINT | */CDRF_SKIPDEFAULT;

								//								return CDRF_NOTIFYPOSTPAINT | CDRF_SKIPDEFAULT;
								return CDRF_DODEFAULT;// | CDRF_NOTIFYPOSTPAINT;
							}
							// trackbar control's thumb marker. This is the portion of the control
							// that the user moves.  For the pre-item-paint of the thumb, we draw 
							// everything completely here, during item pre-paint, and then tell
							// the control to skip default painting and NOT to notify 
							// us during post-paint.
							if (nmcd->dwItemSpec == TBCD_THUMB) {
								auto iPartId = TBBMP_THUMB;

								// if thumb is selected/focussed, switch brushes
								if (dcx_testflag(nmcd->uItemState, CDIS_SELECTED))
									iPartId = TBBMP_THUMBDRAG;

								if (DrawTrackBarPart(hdc, iPartId, &nmcd->rc))
									return CDRF_SKIPDEFAULT;
							}

							// default!
							return CDRF_DODEFAULT;
						} // item prepaint

						case CDDS_ITEMPOSTPAINT:
							return CDRF_SKIPDEFAULT;
						// item postpaint

						default:
							return CDRF_DODEFAULT;
					} // end drawstage

					//break; // unreachable
				} // end NM_CUSTOMDRAW

			} // end SWITCH
		} // end notify
	}
	return 0L;
}

LRESULT DcxTrackBar::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {

		//case WM_ERASEBKGND:
		//	{
		//		if (this->isExStyle(WS_EX_TRANSPARENT))
		//			this->DrawParentsBackground((HDC)wParam);
		//		else {
		//			RECT rect;
		//			GetClientRect( this->m_Hwnd, &rect );
		//			DcxControl::DrawCtrlBackground((HDC) wParam,this,&rect);
		//		}
		//		bParsed = TRUE;
		//		return TRUE;
		//	}
		//	break;

		case WM_NOTIFY:
			{
				dcxlParam(LPNMHDR, hdr);

				if (hdr == nullptr)
					break;

				switch (hdr->code) {
					// Show tooltip
					case TTN_SHOW:
						// Check if its currently being updated.
						if (this->m_bUpdatingTooltip)
							break;

						TString buff((UINT) 80);

						this->evalAliasEx(buff.to_chr(), 79, TEXT("%s,%d,%d"), TEXT("showtip"), this->getUserID(), this->getPos());

						if (!buff.empty()) {
							TOOLINFO ti;

							ZeroMemory(&ti, sizeof(TOOLINFO));

							ti.cbSize = sizeof(TOOLINFO);
							ti.hinst = GetModuleHandle(nullptr);
							ti.hwnd = this->m_Hwnd;
							ti.uId = (UINT_PTR) this->m_Hwnd;
							ti.lpszText = buff.to_chr();

							this->m_bUpdatingTooltip = true;
							SendMessage(this->m_ToolTipHWND, TTM_UPDATETIPTEXT, NULL, (LPARAM) &ti);
							this->m_bUpdatingTooltip = false;
						}
						break; // case TTN_SHOW
				}
				break; // case WM_NOTIFY
			}

		case WM_PAINT:
			{
				if (!this->m_bAlphaBlend)
					break;

				PAINTSTRUCT ps;
				auto hdc = BeginPaint(this->m_Hwnd, &ps);
				Auto(EndPaint(this->m_Hwnd, &ps));

				bParsed = TRUE;

				// Setup alpha blend if any.
				auto ai = this->SetupAlphaBlend(&hdc);
				Auto(this->FinishAlphaBlend(ai));

				//if (this->isExStyle(WS_EX_TRANSPARENT))
				//	this->DrawParentsBackground(hdc);
				//else
				//	DcxControl::DrawCtrlBackground(hdc,this,&ps.rcPaint);

				return CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM)hdc, lParam);

				//if (!this->m_bAlphaBlend)
				//	break;
				//
				//PAINTSTRUCT ps;
				//auto hdc = BeginPaint(this->m_Hwnd, &ps);
				//
				//LRESULT res = 0L;
				//bParsed = TRUE;
				//
				//// Setup alpha blend if any.
				//auto ai = this->SetupAlphaBlend(&hdc);
				//
				////if (this->isExStyle(WS_EX_TRANSPARENT))
				////	this->DrawParentsBackground(hdc);
				////else
				////	DcxControl::DrawCtrlBackground(hdc,this,&ps.rcPaint);
				//
				//res = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );
				//
				//this->FinishAlphaBlend(ai);
				//
				//EndPaint( this->m_Hwnd, &ps );
				//return res;
			}
			break;

		case WM_DESTROY:
			{
				delete this;
				bParsed = TRUE;
			}
			break;

		default:
			return this->CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return 0L;
}


UINT DcxTrackBar::parseImageFlags(const TString &flags) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('b')])
		iFlags |= TBCS_BACK;
	if (xflags[TEXT('c')])
		iFlags |= TBCS_CHANNEL;
	if (xflags[TEXT('d')])
		iFlags |= TBCS_THUMBDRAG;
	if (xflags[TEXT('t')])
		iFlags |= TBCS_THUMB;

	return iFlags;
}

bool DcxTrackBar::DrawTrackBarPart(HDC hdc, const TrackBarParts iPartId, const LPRECT rc)
{
	if (this->m_hbmp[iPartId] == nullptr)
		return false;

	RECT rect;
	BITMAP bmp;
	if (rc == nullptr) {
		if (!GetClientRect(this->m_Hwnd, &rect))
			return false;
	}
	else
		CopyRect(&rect, rc);

	if (GetObject(this->m_hbmp[iPartId], sizeof(BITMAP), &bmp) == 0)
		return false;

	auto hdcbmp = CreateCompatibleDC(hdc);

	if (hdcbmp == nullptr)
		return false;
	Auto(DeleteDC(hdcbmp));

	auto oldBM = SelectBitmap(hdcbmp, this->m_hbmp[iPartId]);
	Auto(SelectBitmap(hdcbmp, oldBM));

	TransparentBlt(hdc,
		rect.left, rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top, hdcbmp,
		0, 0, bmp.bmWidth, bmp.bmHeight, this->m_colTransparent);

	return true;
}
