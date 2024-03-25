/*!
 * \file dcxprogressbar.cpp
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
#include "Classes/dcxprogressbar.h"
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

DcxProgressBar::DcxProgressBar(_In_ const UINT ID, _In_ gsl::strict_not_null<DcxDialog* const> p_Dialog, _In_ const HWND mParentHwnd, _In_ const RECT* const rc, _In_ const TString& styles)
	: DcxControl(ID, p_Dialog)
	, m_tsText(TEXT("%d %%"_ts))
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WindowExStyle::ClientEdge,
		DCX_PROGRESSBARCLASS,
		ws.m_Styles | WindowStyle::Child,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	if (styles.istok(TEXT("tooltips")))
	{
		if (!IsWindow(p_Dialog->getToolTipHWND()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");

		setToolTipHWND(p_Dialog->getToolTipHWND());
		AddToolTipToolInfo(getToolTipHWND(), m_Hwnd);
	}
	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxProgressBar::~DcxProgressBar() noexcept
{
	if (m_hfontVertical)
		DeleteObject(m_hfontVertical);
}

const TString DcxProgressBar::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, PBS_SMOOTH))
		styles.addtok(TEXT("smooth"));
	if (dcx_testflag(Styles, PBS_VERTICAL))
		styles.addtok(TEXT("vertical"));
	if (dcx_testflag(Styles, PBS_MARQUEE))
		styles.addtok(TEXT("marquee"));
	if (this->m_bIsGrad)
		styles.addtok(TEXT("gradient"));
	return styles;
}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxProgressBar::parseControlStyles(const TString& tsStyles)
{
	auto ws = parseGeneralControlStyles(tsStyles);

	m_bIsGrad = false;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"smooth"_hash:
			ws.m_Styles |= PBS_SMOOTH;
			break;
		case L"vertical"_hash:
			ws.m_Styles |= PBS_VERTICAL;
			break;
		case L"marquee"_hash:
			ws.m_Styles |= PBS_MARQUEE;
			break;
		case L"gradient"_hash:
		{
			ws.m_Styles |= PBS_SMOOTH;
			m_bIsGrad = true;
		}
		break;
		default:
			break;
		}
	}
	return ws;
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxProgressBar::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
	case L"value"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), this->getPosition());
		break;
	case L"range"_hash:
	{
		PBRANGE pbr{};
		this->getRange(FALSE, &pbr);
		_ts_snprintf(szReturnValue, TEXT("%d %d"), pbr.iLow, pbr.iHigh);
	}
	break;
	case L"text"_hash:
		_ts_snprintf(szReturnValue, this->m_tsText.to_chr(), this->CalculatePosition());
		break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
}
/*!
 * \brief blah
 *
 * \param input [NAME] [SWITCH] [ID] (OPTIONS)
 */

void DcxProgressBar::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -c name ID $rgb(color)
	if (flags[TEXT('c')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		setBarColor(input.getnexttok().to_<COLORREF>());	// tok 4
	}
	//// xdid -g name ID [1|0]
	//else if ( flags[TEXT('g')] ) {
	//this->m_bIsGrad = input.gettok( 4 ).to_<bool>( );
	//}
	// xdid -i name ID (TEXT)
	else if (flags[TEXT('i')])
	{
		if (numtok > 3)
			m_tsText = input.getlasttoks();	// tok 4, -1
		else
			m_tsText.clear();

		redrawWindow();
	}
	// xdid -j name ID [a|p]
	else if (flags[TEXT('j')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		m_bIsAbsoluteValue = (input.getnexttok() == TEXT('a'));

		redrawWindow();
	}
	// xdid -k name ID $rgb(color)
	else if (flags[TEXT('k')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		setBKColor(input.getnexttok().to_<COLORREF>());	// tok 4
	}
	// xdid -m(o|g) name ID N
	else if (flags[TEXT('m')])
	{
		// -mo
		if (flags[TEXT('o')])
		{
			if (numtok < 4)
				throw DcxExceptions::dcxInvalidArguments();

			setMarquee(TRUE, input.getnexttok().to_int());	// tok 4
		}
		// -mg
		else if (flags[TEXT('g')])
			setMarquee(FALSE, 0);
	}
	// xdid -q name ID [COLOR]
	else if (flags[TEXT('q')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		setTextColor(input.getnexttok().to_<COLORREF>());	// tok 4
		redrawWindow();
	}
	// xdid -r name ID RLow RHigh
	else if (flags[TEXT('r')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iLow = input.getnexttok().to_int();
		const auto iHigh = input.getnexttok().to_int();

		setRange(iLow, iHigh);
	}
	// xdid -t name ID
	else if (flags[TEXT('t')])
	{
		stepIt();
	}
	// xdid -u name ID N
	else if (flags[TEXT('u')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		setStep(input.getnexttok().to_int());	// tok 4
	}
	// xdid -v name ID N
	else if (flags[TEXT('v')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		setPosition(input.getnexttok().to_int());	// tok 4
	}
	// xdid [-o] [NAME] [ID] [ANGLE]
	// vertical fonts [1|0]
	else if (flags[TEXT('o')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto angle = input.getnexttok().to_int();	// tok 4

		//TODO: let user specify angle of text?
		if (angle > 0)
		{
			auto [code, lfCurrent] = Dcx::dcxGetObject<LOGFONT>(getControlFont());
			if (code == 0)
				throw Dcx::dcxException("Unable to get LOGFONT");

			// input is angle based, expected angle = *10
			//lfCurrent.lfEscapement = angle * 10;
			//lfCurrent.lfOrientation = angle * 10;
			lfCurrent.lfEscapement = 900;
			lfCurrent.lfOrientation = 900;

			if (m_hfontVertical)
				DeleteObject(m_hfontVertical);

			m_hfontVertical = CreateFontIndirect(&lfCurrent);
		}
		else if (m_hfontVertical)
		{
			DeleteObject(m_hfontVertical);
			m_hfontVertical = nullptr;
		}

		redrawWindow();
	}
	else
		parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setPosition(const int nNewPos) noexcept
{
	return SendMessage(m_Hwnd, PBM_SETPOS, gsl::narrow_cast<WPARAM>(nNewPos), 0);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setRange(const int iLowLim, const int iHighLim) noexcept
{
	return SendMessage(m_Hwnd, PBM_SETRANGE32, gsl::narrow_cast<WPARAM>(iLowLim), gsl::narrow_cast<LPARAM>(iHighLim));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::getPosition() const noexcept
{
	return SendMessage(m_Hwnd, PBM_GETPOS, gsl::narrow_cast<WPARAM>(0), 0);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::getRange(const BOOL fWhichLimit, const PPBRANGE ppBRange) const noexcept
{
	return SendMessage(m_Hwnd, PBM_GETRANGE, gsl::narrow_cast<WPARAM>(fWhichLimit), reinterpret_cast<LPARAM>(ppBRange));
}

int DcxProgressBar::getStep() const noexcept
{
	return gsl::narrow_cast<int>(SendMessage(m_Hwnd, PBM_GETSTEP, 0U, 0));
}

COLORREF DcxProgressBar::getBarColor() const noexcept
{
	return gsl::narrow_cast<COLORREF>(SendMessage(m_Hwnd, PBM_GETBARCOLOR, 0U, 0));
}

COLORREF DcxProgressBar::getBKColor() const noexcept
{
	return gsl::narrow_cast<COLORREF>(SendMessage(m_Hwnd, PBM_GETBKCOLOR, 0U, 0));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setMarquee(const BOOL fStart, const int fTime) noexcept
{
	return SendMessage(m_Hwnd, PBM_SETMARQUEE, gsl::narrow_cast<WPARAM>(fStart), gsl::narrow_cast<LPARAM>(fTime));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::stepIt() noexcept
{
	return SendMessage(m_Hwnd, PBM_STEPIT, gsl::narrow_cast<WPARAM>(0), 0);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setStep(const int nStepInc) noexcept
{
	return SendMessage(m_Hwnd, PBM_SETSTEP, gsl::narrow_cast<WPARAM>(nStepInc), 0);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setBarColor(const COLORREF clrBar) noexcept
{
	setStartGradientColor(clrBar);
	return SendMessage(m_Hwnd, PBM_SETBARCOLOR, 0U, gsl::narrow_cast<LPARAM>(clrBar));
}
/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setBKColor(const COLORREF clrBk) noexcept
{
	setEndGradientColor(clrBk);
	return SendMessage(m_Hwnd, PBM_SETBKCOLOR, 0U, gsl::narrow_cast<LPARAM>(clrBk));
}

void DcxProgressBar::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());
	xml->SetAttribute("caption", this->m_tsText.c_str());

	{
		PBRANGE pbr{};
		this->getRange(FALSE, &pbr);
		xml->SetAttribute("min", pbr.iLow);
		xml->SetAttribute("max", pbr.iHigh);
	}
	xml->SetAttribute("pos", gsl::narrow_cast<int>(this->getPosition()));
	xml->SetAttribute("step", this->getStep());
	if ((this->m_clrStartGradient == CLR_INVALID) && (this->m_clrEndGradient == CLR_INVALID))
	{
		if (const auto tmp = this->getBarColor(); tmp != CLR_INVALID)
			setColourAttribute(xml, "barcolour", tmp);

		if (const auto tmp = this->getBKColor(); tmp != CLR_INVALID)
			setColourAttribute(xml, "bkcolour", tmp);
	}
	if (m_bIsAbsoluteValue)
		xml->SetAttribute("absolute", "1");
}

TiXmlElement* DcxProgressBar::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxProgressBar::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

	this->m_tsText = queryAttribute(xThis, "caption");
	{
		const auto iLow = queryIntAttribute(xThis, "min");
		const auto iHigh = queryIntAttribute(xThis, "max", 100);
		this->setRange(iLow, iHigh);
	}
	this->m_bIsAbsoluteValue = (queryIntAttribute(xThis, "absolute") > 0);
	this->setStep(queryIntAttribute(xThis, "step", 1));
	this->setPosition(queryIntAttribute(xThis, "pos"));
	if (const auto tmp = queryColourAttribute(xThis, "barcolour"); tmp != CLR_INVALID)
		this->setBarColor(tmp);
	if (const auto tmp = queryColourAttribute(xThis, "bkcolour"); tmp != CLR_INVALID)
		this->setBKColor(tmp);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxProgressBar::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept
{
	return 0L;
}

LRESULT DcxProgressBar::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_ERASEBKGND:
	{
		if (isExStyle(WindowExStyle::Transparent))
		{
			DrawParentsBackground(reinterpret_cast<HDC>(wParam));
			bParsed = TRUE;
			return TRUE;
		}
	}
	break;

	case WM_PRINTCLIENT:
	{
		DrawClientArea(reinterpret_cast<HDC>(wParam), uMsg, lParam);
		bParsed = TRUE;
	}
	break;
	case WM_PAINT:
	{
		bParsed = TRUE;
		if (!wParam)
		{
			PAINTSTRUCT ps{};

			auto hdc = BeginPaint(m_Hwnd, &ps);
			Auto(EndPaint(m_Hwnd, &ps));

			DrawClientArea(hdc, uMsg, lParam);
		}
		else
			DrawClientArea(reinterpret_cast<HDC>(wParam), uMsg, lParam);
	}
	break;

	case WM_LBUTTONUP:
	{
		if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
		{
			const auto iLower = gsl::narrow_cast<int>(getRange(TRUE, nullptr));
			const auto iHigher = gsl::narrow_cast<int>(getRange(FALSE, nullptr));

			execAliasEx(TEXT("sclick,%u,%d,%d,%d,%zd"), getUserID(), getPredictedPos(lParam, iLower, iHigher), iLower, iHigher, getPosition());
		}
	}
	break;

	//case WM_RBUTTONUP:
	//{
	//	if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
	//	{
	//		const auto iLower = getRange(TRUE, nullptr);
	//		const auto iHigher = getRange(FALSE, nullptr);
	//
	//		execAliasEx(TEXT("rclick,%u,%d,%d,%d,%d"), getUserID(), getPredictedPos(lParam, iLower, iHigher), iLower, iHigher, getPosition());
	//	}
	//}
	//break;
	//case WM_CONTEXTMENU:	// stops second rclick event
	//	break;

	case WM_CONTEXTMENU:
	{
		if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
		{
			const auto iLower = gsl::narrow_cast<int>(getRange(TRUE, nullptr));
			const auto iHigher = gsl::narrow_cast<int>(getRange(FALSE, nullptr));

			execAliasEx(TEXT("rclick,%u,%d,%d,%d,%zd"), getUserID(), getPredictedPos(lParam, iLower, iHigher), iLower, iHigher, getPosition());
		}
	}
	break;

	case WM_MOUSEMOVE:
	{
		this->getParentDialog()->setMouseControl(this->getUserID());

		if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
		{
			if (wParam == MK_LBUTTON)
			{
				const auto iLower = gsl::narrow_cast<int>(this->getRange(TRUE, nullptr));
				const auto iHigher = gsl::narrow_cast<int>(this->getRange(FALSE, nullptr));

				this->execAliasEx(TEXT("mousebar,%u,%d,%d,%d,%zd"), getUserID(), getPredictedPos(lParam, iLower, iHigher), iLower, iHigher, getPosition());
			}
		}
	}
	break;

	case WM_DESTROY:
	{
		delete this;
		bParsed = TRUE;
	}
	break;

	default:
		return CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}


int DcxProgressBar::CalculatePosition() const noexcept
{
	const auto iPos = getPosition();

	if (m_bIsAbsoluteValue)
		return gsl::narrow_cast<int>(iPos);

	const auto iLower = getRange(TRUE, nullptr);
	const auto iHigher = getRange(FALSE, nullptr);

	return dcx_round(gsl::narrow_cast<float>((iPos - iLower) * 100) / (iHigher - iLower));
}

void DcxProgressBar::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	RECT rc{};
	// A window may receive internal paint messages as a result of calling RedrawWindow with the RDW_INTERNALPAINT flag set.
	// In this case, the window may not have an update region.
	// An application should call the GetUpdateRect function to determine whether the window has an update region.
	// If GetUpdateRect returns zero, the application should not call the BeginPaint and EndPaint functions.
	//if (GetUpdateRect(m_Hwnd,&rc,FALSE) == 0) return;

	if (!GetClientRect(m_Hwnd, &rc))
		return;

	// Setup alpha blend if any.
	auto ai = SetupAlphaBlend(&hdc);
	Auto(FinishAlphaBlend(ai));

	if (m_bIsGrad)
	{
		DcxControl::DrawCtrlBackground(hdc, this, &rc);

		auto clrStart = getStartGradientColor();
		auto clrEnd = getEndGradientColor();

		if (clrEnd == CLR_INVALID)
			clrEnd = GetSysColor(COLOR_GRADIENTACTIVECAPTION);
		if (clrStart == CLR_INVALID)
			clrStart = XPopupMenuItem::DarkenColor(100, clrEnd);

		auto rcGrad = rc;

		if (isStyle(WindowStyle::PBS_Vertical))
		{
			rcGrad.top += (rc.bottom - rc.top) - (CalculatePosition() * (rc.bottom - rc.top)) / 100;
			XPopupMenuItem::DrawGradient(hdc, &rcGrad, clrStart, clrEnd, TRUE);
		}
		else {
			rcGrad.right = (CalculatePosition() * rc.right) / 100;
			XPopupMenuItem::DrawGradient(hdc, &rcGrad, clrStart, clrEnd, FALSE);
		}
	}
	else
		CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);

	if (!m_tsText.empty())
	{
		const auto oldMode = SetBkMode(hdc, TRANSPARENT);
		Auto(SetBkMode(hdc, oldMode));

		COLORREF oldColour{ CLR_INVALID };

		if (const auto clr = getTextColor(); clr != CLR_INVALID)
			oldColour = SetTextColor(hdc, clr);

		// used to calc text value on pbar
		TString text;
		const auto iPos = CalculatePosition();

		text.tsprintf(m_tsText.to_chr(), iPos);

		HFONT oldfont{ nullptr };
		// NEEDS FIXED: font selection needs looked at
		if (const auto f = getControlFont(); f)
			oldfont = Dcx::dcxSelectObject(hdc, f);

		// rect for text
		auto rcText = rc;
		//DrawText(hdc, text.to_chr(), text.len(), &rcText, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_CALCRECT);
		calcTextRect(hdc, text, &rcText, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP);

		const auto w = rcText.right - rcText.left;
		const auto h = rcText.bottom - rcText.top;

		// reposition the new text area to be at the center
		if (m_hfontVertical)
		{
			rc.left = ((rc.right - rc.left) - h) / 2;
			// added a +w +h as well to as text is drawn ABOVE the damn rect
			rc.top = ((rc.bottom - rc.top) + w + h) / 2;
			rc.right = rc.left + h;
			rc.bottom = rc.top + w;

			if (auto hTmp = Dcx::dcxSelectObject(hdc, m_hfontVertical); !oldfont)
				oldfont = hTmp;
		}
		else {
			rc.left = ((rc.right - rc.left) - w) / 2;
			rc.top = ((rc.bottom - rc.top) - h) / 2;
			rc.right = rc.left + w;
			rc.bottom = rc.top + h;
		}

		ctrlDrawText(hdc, text, &rc, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP);

		if (oldfont)
			Dcx::dcxSelectObject(hdc, oldfont);
		if (oldColour != CLR_INVALID)
			SetTextColor(hdc, oldColour);
	}
}

LRESULT DcxProgressBar::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
