/*!
 * \file dcxlink.cpp
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
#include "Classes/mirc/dcxlink.h"
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

DcxLink::DcxLink(const UINT ID, DcxDialog* const p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_LINKCLASS,
		ws.m_Styles | WindowStyle::Child,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsWindow(m_Hwnd))
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	//this->m_aColors[0] = RGB(0, 0, 255);
	//this->m_aColors[1] = RGB(255, 0, 0);
	//this->m_aColors[2] = RGB(0, 0, 255);
	//this->m_aColors[3] = RGB(128, 128, 128);

	if (p_Dialog->getToolTip())
	{
		if (styles.istok(TEXT("tooltips")))
		{
			setToolTipHWND(p_Dialog->getToolTip());
			if (!IsWindow(getToolTipHWND()))
				throw Dcx::dcxException("Unable to get ToolTips window");

			AddToolTipToolInfo(getToolTipHWND(), m_Hwnd);
		}
	}

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxLink::~DcxLink() noexcept
{
	if (this->m_hIcon)
		DestroyIcon(this->m_hIcon);
}


void DcxLink::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	const TString buf(TGetWindowText(m_Hwnd));
	xml->SetAttribute("caption", buf.c_str());
}

TiXmlElement* DcxLink::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxLink::parseControlStyles(const TString& tsStyles)
{
	//auto ws = parseGeneralControlStyles(tsStyles);
	//ws.m_Styles |= SS_NOTIFY;
	//return ws;

	dcxWindowStyles ws;

	ws.m_Styles |= SS_NOTIFY;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"nounderline"_hash:
			m_bUnderlineText = false;
			break;

		default:
			break;
		}
	}

	return parseGeneralControlStyles(tsStyles, ws);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

TString DcxLink::parseInfoRequest(const TString& input) const
{
	// [NAME] [ID] [PROP]
	if (input.gettok(3) == TEXT("text"))
		return TGetWindowText(m_Hwnd);

	return parseGlobalInfoRequest(input);
}

void DcxLink::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	// [NAME] [ID] [PROP]
	if (input.gettok(3) == TEXT("text"))
	{
		GetWindowText(m_Hwnd, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	}
	else
		parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxLink::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));		// tok 3

	const auto numtok = input.numtok();

	// xdid -l [NAME] [ID] [SWITCH] [N] [COLOR]
	if (flags[TEXT('l')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nColor = (input.getnexttok().to_<size_t>() - 1);	// tok 4

		if (nColor >= std::size(m_aColors))
			throw Dcx::dcxException("Invalid Colour Index");

		gsl::at(m_aColors, nColor) = input.getnexttok().to_<COLORREF>();	// tok 5
	}
	// xdid -q [NAME] [ID] [SWITCH] [COLOR1] ... [COLOR4]
	else if (flags[TEXT('q')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tsArgs(input.getlasttoks());			// tok 4, -1

		if (tsArgs.numtok() >= std::size(m_aColors))
			throw DcxExceptions::dcxInvalidArguments();

		UINT i = 0U;
		for (const auto& arg : tsArgs)
		{
			gsl::at(m_aColors, i) = arg.to_<COLORREF>();	// tok i+1
			++i;
		}
	}
	//xdid -t [NAME] [ID] [SWITCH] (TEXT)
	else if (flags[TEXT('t')])
	{
		const auto text(input.getlasttoks());	// tok 4, -1
		SetWindowText(m_Hwnd, text.to_chr());
		this->redrawWindow();
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto flag(input.getnexttok());		// tok 4
		const auto index = input.getnexttok().to_int();	// tok 5
		auto filename(input.getlasttoks());			// tok 6, -1

		if (this->m_hIcon)
			DestroyIcon(this->m_hIcon);

		this->m_hIcon = dcxLoadIcon(index, filename, false, flag);

		this->redrawWindow();
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxLink::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept
{
	return 0L;
}

LRESULT DcxLink::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
	{
		getParentDialog()->setMouseControl(this->getUserID());

		if (!this->m_bTracking)
		{
			TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_HOVER, m_Hwnd, HOVER_DEFAULT };
			this->m_bTracking = _TrackMouseEvent(&tme);
		}
	}
	break;

	case WM_MOUSEHOVER:
	{
		if (!this->m_bHover && this->m_bTracking)
		{
			this->m_bHover = true;
			InvalidateRect(m_Hwnd, nullptr, FALSE);
		}
	}
	break;

	case WM_MOUSELEAVE:
	{
		if (this->m_bTracking)
		{
			this->m_bHover = false;
			this->m_bTracking = FALSE;
			InvalidateRect(m_Hwnd, nullptr, FALSE);
		}
	}
	break;

	case WM_LBUTTONDOWN:
	{
		if (!this->m_bVisited)
		{
			this->m_bVisited = true;
			InvalidateRect(m_Hwnd, nullptr, FALSE);
		}
		if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			this->execAliasEx(TEXT("lbdown,%u"), getUserID());
	}
	break;

	case WM_ENABLE:
	{
		InvalidateRect(m_Hwnd, nullptr, FALSE);
	}
	break;

	case WM_SETCURSOR:
	{
		if (const auto hCtrl_Cursor = getControlCursor(); hCtrl_Cursor)
		{
			if (GetCursor() != hCtrl_Cursor)
				SetCursor(hCtrl_Cursor);
			bParsed = TRUE;
			return TRUE;
		}
		else if (Dcx::dcxLOWORD(lParam) == HTCLIENT && reinterpret_cast<HWND>(wParam) == m_Hwnd)
		{
			if (const auto hStm_Cursor = LoadCursor(nullptr, IDC_HAND); GetCursor() != hStm_Cursor)
				SetCursor(hStm_Cursor);
			bParsed = TRUE;
			return TRUE;
		}
	}
	break;

	case WM_ERASEBKGND:
	{
		if (this->isExStyle(WindowExStyle::Transparent))
		{
			bParsed = TRUE;
			return TRUE;
		}
		break;
	}

	case WM_PRINTCLIENT:
	{
		this->DrawClientArea(reinterpret_cast<HDC>(wParam));
		bParsed = TRUE;
	}
	break;
	case WM_PAINT:
	{
		bParsed = TRUE;
		PAINTSTRUCT ps{};

		auto hdc = BeginPaint(m_Hwnd, &ps);
		Auto(EndPaint(m_Hwnd, &ps));

		this->DrawClientArea(hdc);
	}
	break;

	case WM_DESTROY:
	{
		delete this;
		bParsed = TRUE;
	}
	break;

	default:
		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}
//static const char HEX2DEC[256] = 
//{
//    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
//    /* 0 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* 1 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* 2 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,
//    
//    /* 4 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* 5 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* 6 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* 7 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    
//    /* 8 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* 9 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* A */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* B */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    
//    /* C */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* D */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* E */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* F */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
//};
//    
//TString DcxLink::UriDecode(const TString & sSrc)
//{
//	// Note from RFC1630:  "Sequences which start with a percent sign
//	// but are not followed by two hexadecimal characters (0-9, A-F) are reserved
//	// for future extension"
//
//	const unsigned char * pSrc = (const unsigned char *)sSrc.to_chr();
//	const int SRC_LEN = sSrc.len();
//	const unsigned char * const SRC_END = pSrc + SRC_LEN;
//	const unsigned char * const SRC_LAST_DEC = SRC_END - 2;   // last decodable TEXT('%') 
//
//	char * const pStart = new char[SRC_LEN];
//	char * pEnd = pStart;
//
//	while (pSrc < SRC_LAST_DEC)
//	{
//		if (*pSrc == TEXT('%'))
//		{
//			char dec1, dec2;
//			if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)])
//				&& -1 != (dec2 = HEX2DEC[*(pSrc + 2)]))
//			{
//				*pEnd++ = (dec1 << 4) + dec2;
//				pSrc += 3;
//				continue;
//			}
//		}
//
//		*pEnd++ = *pSrc++;
//	}
//
//	// the last 2- chars
//	while (pSrc < SRC_END)
//		*pEnd++ = *pSrc++;
//
//	TString sResult(pStart, pEnd);
//	delete [] pStart;
//	return sResult;
//}
//
//// Only alphanum is safe.
//static const char SAFE[256] =
//{
//    /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
//    /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,
//    
//    /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
//    /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
//    /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
//    /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
//    
//    /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    
//    /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
//};
//
//TString DcxLink::UriEncode(const TString & sSrc)
//{
//	const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
//	const unsigned char * pSrc = (const unsigned char *)sSrc.to_chr();
//	const int SRC_LEN = sSrc.len();
//	unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
//	unsigned char * pEnd = pStart;
//	const unsigned char * const SRC_END = pSrc + SRC_LEN;
//
//	for (; pSrc < SRC_END; ++pSrc)
//	{
//		if (SAFE[*pSrc]) 
//			*pEnd++ = *pSrc;
//		else
//		{
//			// escape this char
//			*pEnd++ = TEXT('%');
//			*pEnd++ = DEC2HEX[*pSrc >> 4];
//			*pEnd++ = DEC2HEX[*pSrc & 0x0F];
//		}
//	}
//
//	TString sResult((char *)pStart, (char *)pEnd);
//	delete [] pStart;
//	return sResult;
//}

void DcxLink::DrawClientArea(HDC hdc)
{
	RECT rect{};
	if (!GetClientRect(m_Hwnd, &rect))
		return;

	// Setup alpha blend if any.
	const auto ai = SetupAlphaBlend(&hdc, true);
	Auto(FinishAlphaBlend(ai));

	// fill background.
	DcxControl::DrawCtrlBackground(hdc, this, &rect);

	const auto hFont = (!getControlFont()) ? Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT) : getControlFont();

	if (auto [code, lf] = Dcx::dcxGetObject<LOGFONT>(hFont); code != 0)
	{
		lf.lfUnderline = (m_bUnderlineText ? TRUE : FALSE);

		if (const auto hNewFont = CreateFontIndirect(&lf); hNewFont)
		{
			Auto(DeleteObject(hNewFont));

			const auto savedDC = SaveDC(hdc);
			Auto(RestoreDC(hdc, savedDC));

			Dcx::dcxSelectObject<HFONT>(hdc, hNewFont);

			SetBkMode(hdc, TRANSPARENT);

			if (this->m_hIcon)
			{
				const auto y = ((rect.top + rect.bottom - 16) / 2);
				DrawIconEx(hdc, rect.left, y, this->m_hIcon, 0, 0, NULL, nullptr, DI_NORMAL);

				OffsetRect(&rect, 20, 0);
			}

			if (IsWindowEnabled(m_Hwnd) == FALSE)
				setTextColor(this->m_aColors[3]);
			else if (this->m_bHover)
				setTextColor(this->m_aColors[1]);
			else if (this->m_bVisited)
				setTextColor(this->m_aColors[2]);
			else
				setTextColor(this->m_aColors[0]);

			if (m_clrText != CLR_INVALID)
				SetTextColor(hdc, m_clrText);

			const TString wtext(TGetWindowText(m_Hwnd));
			this->ctrlDrawText(hdc, wtext, &rect, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER);
		}
	}

	//RECT rect;
	//if (!GetClientRect(m_Hwnd, &rect))
	//	return;
	//
	//// Setup alpha blend if any.
	//auto ai = this->SetupAlphaBlend(&hdc, true);
	//
	//// fill background.
	//DcxControl::DrawCtrlBackground(hdc,this,&rect);
	//
	//auto hFont = this->m_hFont;
	//
	//if (hFont == nullptr)
	//	hFont = GetStockFont( DEFAULT_GUI_FONT /*SYSTEM_FONT*/ );
	//
	//LOGFONT lf;
	//if (GetObject(hFont, sizeof(LOGFONT), &lf) != 0)
	//{
	//
	//	lf.lfUnderline = TRUE;
	//
	//	auto hNewFont = CreateFontIndirect(&lf);
	//	if (hNewFont != nullptr)
	//	{
	//		auto hOldFont = SelectFont(hdc, hNewFont);
	//
	//		const auto oldMode = SetBkMode(hdc, TRANSPARENT);
	//
	//		if (this->m_hIcon != nullptr) {
	//
	//			const auto y = ((rect.top + rect.bottom - 16) / 2);
	//			DrawIconEx(hdc, rect.left, y, this->m_hIcon, 0, 0, NULL, nullptr, DI_NORMAL);
	//
	//			OffsetRect(&rect, 20, 0);
	//		}
	//
	//		if (IsWindowEnabled(m_Hwnd) == FALSE)
	//			this->m_clrText = this->m_aColors[3];
	//		else if (this->m_bHover)
	//			this->m_clrText = this->m_aColors[1];
	//		else if (this->m_bVisited)
	//			this->m_clrText = this->m_aColors[2];
	//		else
	//			this->m_clrText = this->m_aColors[0];
	//
	//		TString wtext;
	//
	//		TGetWindowText(m_Hwnd, wtext);
	//		this->ctrlDrawText(hdc, wtext, &rect, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER);
	//
	//		SetBkMode(hdc, oldMode);
	//		SelectFont(hdc, hOldFont);
	//		DeleteFont(hNewFont);
	//	}
	//}
	//this->FinishAlphaBlend(ai);
}

LRESULT DcxLink::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
