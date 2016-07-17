/*!
 * \file dcxscroll.cpp
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
#include "Classes/mirc/dcxscroll.h"
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

DcxScroll::DcxScroll(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
	, m_nPage(5)
	, m_nLine(1)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	m_Hwnd = CreateWindowEx(
		(DWORD)ExStyles,
		TEXT("SCROLLBAR"),
		nullptr,
		(DWORD)(WS_CHILD | Styles),
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS | SIF_RANGE;
	si.nPos = 0;
	si.nMin = 0;
	si.nMax = 100;
	SetScrollInfo(m_Hwnd, SB_CTL, &si, TRUE);

	this->registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxScroll::~DcxScroll( ) {

	this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxScroll::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	//for (auto tsStyle(styles.getfirsttok(1)); !tsStyle.empty(); tsStyle = styles.getnexttok())
	//{
	//	if ( tsStyle == TEXT("vertical") )
	//		*Styles |= SBS_VERT;
	//}

	if (styles.istok(TEXT("vertical")))
		*Styles |= SBS_VERT;

	this->parseGeneralControlStyles(styles, Styles, ExStyles, bNoTheme);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxScroll::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
#if DCX_USE_HASHING
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case L"value"_hash:
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS;
		if (!GetScrollInfo(m_Hwnd, SB_CTL, &si))
			throw Dcx::dcxException("Unable to get scroll info");

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), si.nPos);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"range"_hash:
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE;
		if (!GetScrollInfo(m_Hwnd, SB_CTL, &si))
			throw Dcx::dcxException("Unable to get scroll info");

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), si.nMin, si.nMax);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"line"_hash:
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->m_nLine);
		break;
		// [NAME] [ID] [PROP]
	case L"page"_hash:
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->m_nPage);
		break;
	default:
		this->parseGlobalInfoRequest(input, szReturnValue);
		break;
	}
#else
	const auto prop(input.getfirsttok(3));

	// [NAME] [ID] [PROP]
	if ( prop == TEXT("value") ) {

		SCROLLINFO si;
		si.cbSize = sizeof( SCROLLINFO );
		si.fMask = SIF_POS;
		if (!GetScrollInfo(m_Hwnd, SB_CTL, &si))
			throw Dcx::dcxException("Unable to get scroll info");

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), si.nPos );
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("range") ) {

		SCROLLINFO si;
		si.cbSize = sizeof( SCROLLINFO );
		si.fMask = SIF_RANGE;
		if (!GetScrollInfo(m_Hwnd, SB_CTL, &si))
			throw Dcx::dcxException("Unable to get scroll info");

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), si.nMin, si.nMax);
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("line") ) {

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->m_nLine );
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("page")) {

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->m_nPage);
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
#endif
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxScroll::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	//xdid -l [NAME] [ID] [SWITCH] [N]
	if ( flags[TEXT('l')] && numtok > 3 ) {

		const auto nLine = input.getnexttok().to_int();	// tok 4

		if ( nLine > 0 )
			this->m_nLine = nLine;
	}
	//xdid -m [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('m')] && numtok > 3 ) {

		const auto nPage = input.getnexttok().to_int();	// tok 4

		if ( nPage > 0 )
			this->m_nPage = nPage;
	}
	//xdid -r [NAME] [ID] [SWITCH] [L] [R]
	else if ( flags[TEXT('r')] && numtok > 4 ) {

		const auto L = input.getnexttok().to_int();	// tok 4
		const auto R = input.getnexttok().to_int();	// tok 5

		SCROLLINFO si;
		si.cbSize = sizeof( SCROLLINFO );
		si.fMask = SIF_RANGE;
		si.nMin = L;
		si.nMax = R;
		SetScrollInfo( m_Hwnd, SB_CTL, &si, TRUE );
	}
	//xdid -v [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('v')] && numtok > 3 ) {

		const auto pos = input.getnexttok().to_int();	// tok 4

		SCROLLINFO si;
		si.cbSize = sizeof( SCROLLINFO );
		si.fMask = SIF_POS;
		si.nPos = pos;
		SetScrollInfo( m_Hwnd, SB_CTL, &si, TRUE );
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

const TString DcxScroll::getStyles(void) const
{
	auto tsStyles(__super::getStyles());
	const auto Styles = GetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, SBS_VERT))
		tsStyles.addtok(TEXT("vertical"));

	return tsStyles;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxScroll::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) {
	switch (uMsg) {
		case WM_HSCROLL: 
		{
			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
			if (!GetScrollInfo(m_Hwnd, SB_CTL, &si))
				break;

			switch (LOWORD(wParam)) {
				case SB_TOP:
				{
					si.nPos = si.nMin;
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("top"), this->getUserID( ), si.nPos );
					break;
				}

				case SB_BOTTOM:
				{
					si.nPos = si.nMax;
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("bottom"), this->getUserID( ), si.nPos );
					break;
				}

				//case SB_ENDTRACK:
				//  CallAliasEx( p_Dialog, ret, TEXT("%s,%d,%d"), TEXT("sclick"), 
				//               this->getUserID( ), p_DcxTrackBar->getPos( ) );
				//  break;

				case SB_PAGEUP:
				{
					if ((si.nPos - this->m_nPage) >= si.nMin)
						si.nPos -= this->m_nPage;
					else
						si.nPos = si.nMin;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("pageup"), this->getUserID(), si.nPos);
					break;
				}

				case SB_PAGEDOWN:
				{
					if (si.nPos + this->m_nPage <= si.nMax)
						si.nPos += this->m_nPage;
					else
						si.nPos = si.nMax;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("pagedown"), this->getUserID(), si.nPos);
					break;
				}

				case SB_LINEUP:
				{
					if (si.nPos - this->m_nLine >= si.nMin)
						si.nPos -= this->m_nLine;
					else
						si.nPos = si.nMin;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("lineup"), this->getUserID( ), si.nPos );
					break;
				}

				case SB_LINEDOWN:
				{
					if (si.nPos + this->m_nLine <= si.nMax)
						si.nPos += this->m_nLine;
					else
						si.nPos = si.nMax;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("linedown"), this->getUserID(), si.nPos);
					break;
				}

				case SB_THUMBPOSITION:
				{
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("trackend"), this->getUserID(), si.nPos);
					break;
				}

				case SB_THUMBTRACK:
				{
					si.nPos = si.nTrackPos;
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("tracking"), this->getUserID(), si.nTrackPos);
					break;
				}
			}

			bParsed = TRUE;
			si.fMask = SIF_POS;
			SetScrollInfo(m_Hwnd, SB_CTL, &si, TRUE);
			break;
		}

		case WM_VSCROLL:
		{
			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
			if (!GetScrollInfo(m_Hwnd, SB_CTL, &si))
				break;

			switch (LOWORD(wParam)) {
				case SB_TOP:
				{
					si.nPos = si.nMin;
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("top"), this->getUserID(), si.nPos);
					break;
				}

				case SB_BOTTOM:
				{
					si.nPos = si.nMax;
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("bottom"), this->getUserID(), si.nPos);
					break;
				}

				//case SB_ENDTRACK:
				//  CallAliasEx( p_Dialog, ret, TEXT("%s,%d,%d"), TEXT("sclick"), 
				//               this->getUserID( ), p_DcxTrackBar->getPos( ) );
				//  break;

				case SB_PAGEUP:
				{
					if (si.nPos - this->m_nPage >= si.nMin)
						si.nPos -= this->m_nPage;
					else
						si.nPos = si.nMin;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("pageup"), this->getUserID(), si.nPos);
					break;
				}
				
				case SB_PAGEDOWN:
				{
					if (si.nPos + this->m_nPage <= si.nMax)
						si.nPos += this->m_nPage;
					else
						si.nPos = si.nMax;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("pagedown"), this->getUserID(), si.nPos);
					break;
				}
				
				case SB_LINEUP:
				{
					if (si.nPos - this->m_nLine >= si.nMin)
						si.nPos -= this->m_nLine;
					else
						si.nPos = si.nMin;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("lineup"), this->getUserID(), si.nPos);
					break;
				}

				case SB_LINEDOWN:
				{
					if ( si.nPos + this->m_nLine <= si.nMax )
						si.nPos += this->m_nLine;
					else
						si.nPos = si.nMax;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("linedown"), this->getUserID( ), si.nPos );
					break;
				}

				case SB_THUMBPOSITION:
				{
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("trackend"), this->getUserID(), si.nPos);
					break;
				}

				case SB_THUMBTRACK:
				{
					si.nPos = si.nTrackPos;
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("tracking"), this->getUserID(), si.nTrackPos);
					break;
				}
			}

			bParsed = TRUE;
			si.fMask = SIF_POS;
			SetScrollInfo(m_Hwnd, SB_CTL, &si, TRUE);
			break;
		}
	}

	return 0L;
}

LRESULT DcxScroll::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {

		case WM_PAINT:
			{
				if (!this->m_bAlphaBlend)
					break;

				PAINTSTRUCT ps;

				auto hdc = BeginPaint(m_Hwnd, &ps);

				bParsed = TRUE;

				// Setup alpha blend if any.
				auto ai = this->SetupAlphaBlend(&hdc);

				auto res = CallWindowProc(this->m_DefaultWindowProc, m_Hwnd, uMsg, (WPARAM)hdc, lParam);

				this->FinishAlphaBlend(ai);

				EndPaint( m_Hwnd, &ps );
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
