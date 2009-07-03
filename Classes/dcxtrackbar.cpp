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

#include "dcxtrackbar.h"
#include "dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxTrackBar::DcxTrackBar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles )
: DcxControl( ID, p_Dialog ),
m_bUpdatingTooltip(false)
{
  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles,
    DCX_TRACKBARCLASS,
    NULL,
    WS_CHILD | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

	if (!IsWindow(this->m_Hwnd))
		throw "Unable To Create Window";

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->m_hbmp[TBBMP_BACK] = NULL;
	this->m_hbmp[TBBMP_THUMB] = NULL;
	this->m_hbmp[TBBMP_THUMBDRAG] = NULL;
	this->m_hbmp[TBBMP_CHANNEL] = NULL;
	this->m_colTransparent = -1;

	// Keep track of the tooltip
	if (Styles & TBS_TOOLTIPS) {
		HWND tooltip = (HWND) SendMessage(this->m_Hwnd, TBM_GETTOOLTIPS, NULL, NULL);

		if (tooltip != NULL)
			this->m_ToolTipHWND = tooltip;
	}

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxTrackBar::~DcxTrackBar( ) {
	if (this->m_hbmp[TBBMP_BACK] != NULL)
		DeleteObject(this->m_hbmp[TBBMP_BACK]);
	if (this->m_hbmp[TBBMP_THUMB] != NULL)
		DeleteObject(this->m_hbmp[TBBMP_THUMB]);
	if (this->m_hbmp[TBBMP_THUMBDRAG] != NULL)
		DeleteObject(this->m_hbmp[TBBMP_THUMBDRAG]);
	if (this->m_hbmp[TBBMP_CHANNEL] != NULL)
		DeleteObject(this->m_hbmp[TBBMP_CHANNEL]);

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTrackBar::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {
	*Styles |= TBS_FIXEDLENGTH;
	unsigned int i = 1, numtok = styles.numtok( );

	while ( i <= numtok ) {
		if ( styles.gettok( i ) == "autoticks" ) 
			*Styles |= TBS_AUTOTICKS;
		else if ( styles.gettok( i ) == "both" ) 
			*Styles |= TBS_BOTH;
		else if ( styles.gettok( i ) == "top" ) 
			*Styles |= TBS_TOP;
		else if ( styles.gettok( i ) == "bottom" ) 
			*Styles |= TBS_BOTTOM;
		else if ( styles.gettok( i ) == "left" ) 
			*Styles |= TBS_LEFT;
		else if ( styles.gettok( i ) == "right" ) 
			*Styles |= TBS_RIGHT;
		else if ( styles.gettok( i ) == "select" ) 
			*Styles |= TBS_ENABLESELRANGE;
		else if ( styles.gettok( i ) == "vertical" ) 
			*Styles |= TBS_VERT;
		else if ( styles.gettok( i ) == "nothumb" ) 
			*Styles |= TBS_NOTHUMB;
		else if ( styles.gettok( i ) == "noticks" ) 
			*Styles |= TBS_NOTICKS;
		else if ( styles.gettok( i ) == "reversed" ) 
			*Styles |= TBS_REVERSED;
		else if ( styles.gettok( i ) == "downisleft" ) 
			*Styles |= TBS_DOWNISLEFT;
		else if ( styles.gettok( i ) == "tooltips" ) 
			*Styles |= TBS_TOOLTIPS;
#ifdef DCX_USE_WINSDK
		else if ( styles.gettok( i ) == "transparentbkg" )
			*Styles |= TBS_TRANSPARENTBKGND;
#endif

		i++;
	}
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

void DcxTrackBar::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( );
	TString prop(input.gettok( 3 ));

  if ( prop == "value" ) {

    wsprintf( szReturnValue, "%d", this->getPos( ) );
    return;
  }
  else if ( prop == "range" ) {
    
    wsprintf( szReturnValue, "%d %d", this->getRangeMin( ), this->getRangeMax( ) );
    return;
  }
  else if ( prop == "line" ) {
    
    wsprintf( szReturnValue, "%d", this->getLineSize( ) );
    return;
  }
  else if ( prop == "page" ) {
    
    wsprintf( szReturnValue, "%d", this->getPageSize( ) );
    return;
  }
  else if (prop == "selrange") {
    wsprintf(szReturnValue, "%d %d", this->getSelStart(), this->getSelEnd());
    return;
  }
  else if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
    return;

  szReturnValue[0] = 0;
}
/*!
 * \brief blah
 *
 * \param input [NAME] [SWITCH] [ID] (OPTIONS)
 */

void DcxTrackBar::parseCommandRequest( TString & input ) {
	XSwitchFlags flags(input.gettok(3));
	int numtok = input.numtok( );

  // xdid -c [NAME] [ID] [SWITCH] [VALUE]
  if ( flags['c'] && numtok > 3 ) {

    LONG lPosition = (LONG)input.gettok( 4 ).to_num( );
    this->setTic( lPosition );
  }
	// xdid -g [NAME] [ID] [SWITCH] [FLAGS] [FILE]
	else if (flags['g'] && numtok > 4) {
		UINT tflags = parseImageFlags(input.gettok( 4 ));
		TString filename(input.gettok(5, -1).trim());

		// background
		if (tflags & TBCS_BACK)
			this->m_hbmp[TBBMP_BACK] = dcxLoadBitmap(this->m_hbmp[TBBMP_BACK], filename);
		// thumb
		if (tflags & TBCS_THUMB)
			this->m_hbmp[TBBMP_THUMB] = dcxLoadBitmap(this->m_hbmp[TBBMP_THUMB], filename);
		// thumb hover
		if (tflags & TBCS_THUMBDRAG)
			this->m_hbmp[TBBMP_THUMBDRAG] = dcxLoadBitmap(this->m_hbmp[TBBMP_THUMBDRAG], filename);
		// channel
		if (tflags & TBCS_CHANNEL)
			this->m_hbmp[TBBMP_CHANNEL] = dcxLoadBitmap(this->m_hbmp[TBBMP_CHANNEL], filename);

		// these dont seem to work so dont bother calling it
		//this->redrawWindow();
		//InvalidateRect(this->m_Hwnd, NULL, TRUE);
	}
  // xdid -j [NAME] [ID] [SWITCH] [MIN] [MAX]
  else if ( flags['j'] && numtok > 4 ) {

    LONG iMin = (LONG)input.gettok( 4 ).to_num( );
    LONG iMax = (LONG)input.gettok( 5 ).to_num( );
    this->setSel( iMin, iMax );
  }
  // xdid -l [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags['l'] && numtok > 3 ) {

    LONG lLineSize = (LONG)input.gettok( 4 ).to_num( );
    this->setLineSize( lLineSize );
  }
  // xdid -m [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags['m'] && numtok > 3 ) {

    LONG lPageSize = (LONG)input.gettok( 4 ).to_num( );
    this->setPageSize( lPageSize );
  }
  // xdid -n [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags['n'] && numtok > 3 ) {

    int iTicFreq = input.gettok( 4 ).to_int( );
    this->setTicFreq( iTicFreq );
  }
  // xdid -q [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags['q'] ) {

    this->clearTics( );
  }
  // xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
  else if ( flags['r'] && numtok > 4 ) {

    LONG lMinRange = (LONG)input.gettok( 4 ).to_num( );
    LONG lMaxRange = (LONG)input.gettok( 5 ).to_num( );

    this->setRangeMin( lMinRange );
    this->setRangeMax( lMaxRange );
  }
	// xdid -o [NAME] [ID] [SWITCH] [VALUE]
	else if (flags['o'] && numtok > 3) {
		m_colTransparent = (COLORREF)input.gettok( 4 ).to_num();
		this->redrawWindow();
	}
  // xdid -t [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags['t'] && numtok > 3 ) {

    TString value(input.gettok( 4 ));

    if ( value == "left" )
      this->setTipSide( TBTS_LEFT );
    else if ( value == "right" )
      this->setTipSide( TBTS_RIGHT );
    else if ( value == "bottom" )
      this->setTipSide( TBTS_BOTTOM );
    else
      this->setTipSide( TBTS_TOP );
  }
  // xdid -u [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags['u'] && numtok > 3 ) {

    LONG lLength = (LONG)input.gettok( 4 ).to_num( );

    this->setThumbLength( lLength );
  }
  // xdid -v [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags['v'] && numtok > 3 ) {

    LONG lPosition = (LONG)input.gettok( 4 ).to_num( );

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
  return SendMessage( this->m_Hwnd, TBM_GETRANGEMIN, (WPARAM) 0, (LPARAM) 0 );
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
  return SendMessage( this->m_Hwnd, TBM_GETRANGEMAX, (WPARAM) 0, (LPARAM) 0 );
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
  return SendMessage( this->m_Hwnd, TBM_GETPOS, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setTic( const LONG lPosition ) {
  return (BOOL) SendMessage( this->m_Hwnd, TBM_SETTIC, (WPARAM) 0, (LPARAM) lPosition );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setTicFreq( const LONG wFreq ) {
  return SendMessage( this->m_Hwnd, TBM_SETTICFREQ, (WPARAM) wFreq, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::clearTics( ) {
  return SendMessage( this->m_Hwnd, TBM_CLEARTICS, (WPARAM) TRUE, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setTipSide( const int fLocation ) {
  return SendMessage( this->m_Hwnd, TBM_SETTIPSIDE, (WPARAM) fLocation, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setPageSize( const LONG lPageSize ) {
  return SendMessage( this->m_Hwnd, TBM_SETPAGESIZE, (WPARAM) 0, (LPARAM) lPageSize );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::getPageSize(  ) const {
  return SendMessage( this->m_Hwnd, TBM_GETPAGESIZE, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setLineSize( const LONG lLineSize ) {
  return SendMessage( this->m_Hwnd, TBM_SETLINESIZE, (WPARAM) 0, (LPARAM) lLineSize );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::getLineSize(  ) const {
  return SendMessage( this->m_Hwnd, TBM_GETLINESIZE, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setThumbLength( const UINT iLength ) {
  return SendMessage( this->m_Hwnd, TBM_SETTHUMBLENGTH, (WPARAM) iLength, (LPARAM) 0 );
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
					this->execAliasEx("%s,%d,%d", "top", this->getUserID(), this->getPos());
					break;

				case TB_BOTTOM:
					this->execAliasEx("%s,%d,%d", "bottom", this->getUserID(), this->getPos());
					break;

				case TB_PAGEUP:
					this->execAliasEx("%s,%d,%d", "pageup", this->getUserID(), this->getPos());
					break;

				case TB_PAGEDOWN:
					this->execAliasEx("%s,%d,%d", "pagedown", this->getUserID(), this->getPos());
					break;

				case TB_LINEUP:
					this->execAliasEx("%s,%d,%d", "lineup", this->getUserID(), this->getPos());
					break;

				case TB_LINEDOWN:
					this->execAliasEx("%s,%d,%d", "linedown", this->getUserID(), this->getPos());
					break;

				case TB_THUMBPOSITION:
					this->execAliasEx("%s,%d,%d", "trackend", this->getUserID(), this->getPos());
					break;

				case SB_THUMBTRACK:
					this->execAliasEx("%s,%d,%d", "tracking", this->getUserID(), this->getPos());
					break;
			}

			break;
		}


		case WM_NOTIFY: {
			LPNMHDR hdr = (LPNMHDR) lParam;

			if (!hdr)
				break;

			switch (hdr->code) {
				case NM_CUSTOMDRAW: {
					// if (no items to draw)
					if ( !this->m_hbmp[TBBMP_BACK] &&
						!this->m_hbmp[TBBMP_THUMB] &&
						!this->m_hbmp[TBBMP_THUMBDRAG] &&
						!this->m_hbmp[TBBMP_CHANNEL])
						break;

					NMCUSTOMDRAW nmcd = *(LPNMCUSTOMDRAW) lParam;
					bParsed = TRUE;
					HDC hdc = nmcd.hdc;

					switch (nmcd.dwDrawStage) {
						case CDDS_PREPAINT:
							return CDRF_NOTIFYITEMDRAW/* | CDRF_NOTIFYPOSTPAINT*/;

						case CDDS_ITEMPREPAINT: {
							// try to make it draw the tics, doesnt work =(
							if (nmcd.dwItemSpec == TBCD_TICS) {
								return CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT;
							}
							HBITMAP oldBM = NULL;
							// channel that the trackbar control's thumb marker slides along
							if (nmcd.dwItemSpec == TBCD_CHANNEL) {
								// draw the background here
								if (this->m_hbmp[TBBMP_BACK]) {
									HDC hdcbmp = CreateCompatibleDC(hdc);

									if (hdcbmp != NULL) {
										RECT rect;
										BITMAP bmp;
										GetClientRect(this->m_Hwnd, &rect);
										GetObject(this->m_hbmp[TBBMP_BACK], sizeof(BITMAP), &bmp);
										oldBM = (HBITMAP)SelectObject(hdcbmp, this->m_hbmp[TBBMP_BACK]);

										TransparentBlt( hdc,
											rect.left, rect.top,
											rect.right - rect.left, 
											rect.bottom - rect.top, hdcbmp,
											0, 0, bmp.bmWidth, bmp.bmHeight, this->m_colTransparent);
										SelectObject(hdcbmp, oldBM);
										DeleteDC(hdcbmp);
									}
								}

								if (this->m_hbmp[TBBMP_CHANNEL]) {
									HDC hdcbmp = CreateCompatibleDC(hdc);

									if (hdcbmp != NULL) {
										BITMAP bmp;
										GetObject(this->m_hbmp[TBBMP_CHANNEL], sizeof(BITMAP), &bmp);
										oldBM = (HBITMAP)SelectObject(hdcbmp, this->m_hbmp[TBBMP_CHANNEL]);

										TransparentBlt( hdc,
											nmcd.rc.left, nmcd.rc.top,
											nmcd.rc.right - nmcd.rc.left, 
											nmcd.rc.bottom - nmcd.rc.top, hdcbmp,
											0, 0, bmp.bmWidth, bmp.bmHeight, this->m_colTransparent);
										SelectObject(hdcbmp, oldBM);
										DeleteDC(hdcbmp);
										return /*CDRF_NOTIFYPOSTPAINT | */CDRF_SKIPDEFAULT;
									}
								}

//								return CDRF_NOTIFYPOSTPAINT | CDRF_SKIPDEFAULT;
								return CDRF_DODEFAULT;// | CDRF_NOTIFYPOSTPAINT;
							}
							// trackbar control's thumb marker. This is the portion of the control
							// that the user moves.  For the pre-item-paint of the thumb, we draw 
							// everything completely here, during item pre-paint, and then tell
							// the control to skip default painting and NOT to notify 
							// us during post-paint.
							if (nmcd.dwItemSpec == TBCD_THUMB) {
								HBITMAP pBmp = NULL;

								// if thumb is selected/focussed, switch brushes
								if (nmcd.uItemState & CDIS_SELECTED)
									pBmp = this->m_hbmp[TBBMP_THUMBDRAG];
								else
									pBmp = this->m_hbmp[TBBMP_THUMB];

								// nothing custom to draw
								if (!pBmp)
									return CDRF_DODEFAULT;

								HDC hdcbmp = CreateCompatibleDC(hdc);
								if (hdcbmp != NULL) {
									BITMAP bmp;

									GetObject(pBmp, sizeof(BITMAP), &bmp);
									oldBM = (HBITMAP)SelectObject(hdcbmp, pBmp);

									TransparentBlt( hdc,
										nmcd.rc.left, nmcd.rc.top,
										nmcd.rc.right - nmcd.rc.left, 
										nmcd.rc.bottom - nmcd.rc.top, hdcbmp,
										0, 0, bmp.bmWidth, bmp.bmHeight, this->m_colTransparent);
									SelectObject(hdcbmp, oldBM);
									DeleteDC(hdcbmp);

									// don't let control draw itself, or it will un-do our work
									return CDRF_SKIPDEFAULT;
								}
							}

							// default!
							return CDRF_DODEFAULT;
						} // item prepaint

						case CDDS_ITEMPOSTPAINT: {
								return CDRF_SKIPDEFAULT;
						} // item postpaint

						default:
							return CDRF_DODEFAULT;
					} // end drawstage

					break;
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

		case WM_NOTIFY: {
			LPNMHDR hdr = (LPNMHDR) lParam;

			if (!hdr)
				break;

			switch (hdr->code) {
				// Show tooltip
				case TTN_SHOW:
					// Check if its currently being updated.
					if (this->m_bUpdatingTooltip)
						break;

					TString buff((UINT) 80);

					this->evalAliasEx(buff.to_chr(), 79, "%s,%d", "showtip", this->getPos());
					
					if (buff.len() > 0) {
						TOOLINFO ti;

						ZeroMemory(&ti, sizeof(TOOLINFO));

						ti.cbSize = sizeof(TOOLINFO);
						ti.hinst = GetModuleHandle(NULL);
						ti.hwnd = this->m_Hwnd;
						ti.uId = (UINT_PTR) this->m_Hwnd;
						ti.lpszText = buff.to_chr();

						this->m_bUpdatingTooltip = true;
						SendMessage(this->m_ToolTipHWND, TTM_UPDATETIPTEXT, NULL, (LPARAM) &ti);
						this->m_bUpdatingTooltip = false;
					}

					break;
			}
		}

		case WM_PAINT:
			{
				if (!this->m_bAlphaBlend)
					break;
				PAINTSTRUCT ps;
				HDC hdc;

				hdc = BeginPaint( this->m_Hwnd, &ps );

				LRESULT res = 0L;
				bParsed = TRUE;

				// Setup alpha blend if any.
				LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

				//if (this->isExStyle(WS_EX_TRANSPARENT))
				//	this->DrawParentsBackground(hdc);
				//else
				//	DcxControl::DrawCtrlBackground(hdc,this,&ps.rcPaint);

				res = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

				this->FinishAlphaBlend(ai);

				EndPaint( this->m_Hwnd, &ps );
				return res;
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


UINT DcxTrackBar::parseImageFlags(TString &flags) {
	INT i = 1, len = flags.len(), iFlags = 0;

	// no +sign, missing params
	if (flags[0] != '+')
		return iFlags;

	while (i < len) {
		if (flags[i] == 'b')
			iFlags |= TBCS_BACK;
		else if (flags[i] == 'c')
			iFlags |= TBCS_CHANNEL;
		else if (flags[i] == 'd')
			iFlags |= TBCS_THUMBDRAG;
		else if (flags[i] == 't')
			iFlags |= TBCS_THUMB;


		++i;
	}

	return iFlags;
}
