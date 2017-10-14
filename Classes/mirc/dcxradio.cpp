/*!
 * \file dcxradio.cpp
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
#include "Classes/mirc/dcxradio.h"
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

DcxRadio::DcxRadio(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
{
	const auto[bNoTheme, Styles, ExStyles] = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ExStyles,
		WC_BUTTON,
		Styles | WindowStyle::Child,
		rc,
		mParentHwnd,
		ID);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	this->m_bNoTheme = (bNoTheme != FALSE);

	if (p_Dialog->getToolTip() != nullptr)
	{
		if (styles.istok(TEXT("tooltips")))
		{
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			if (!IsWindow(this->m_ToolTipHWND))
				throw Dcx::dcxException("Unable to get ToolTips window");

			AddToolTipToolInfo(this->m_ToolTipHWND, m_Hwnd);
		}
	}
	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxRadio::~DcxRadio( )
{
	this->unregistreDefaultWindowProc( );
}

const TString DcxRadio::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, BS_RIGHT))
		styles.addtok(TEXT("rjustify"));
	if (dcx_testflag(Styles, BS_CENTER))
		styles.addtok(TEXT("center"));
	if (dcx_testflag(Styles, BS_LEFT))
		styles.addtok(TEXT("ljustify"));
	if (dcx_testflag(Styles, BS_RIGHTBUTTON))
		styles.addtok(TEXT("right"));
	if (dcx_testflag(Styles, BS_PUSHLIKE))
		styles.addtok(TEXT("pushlike"));

	return styles;

}

/*!
 * \brief blah
 *
 * blah
 */

//void DcxRadio::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
//{
//	*Styles |= BS_AUTORADIOBUTTON;
//
//	for (const auto &tsStyle: styles)
//	{
//#if DCX_USE_HASHING
//		switch (std::hash<TString>{}(tsStyle))
//		{
//			case L"rjustify"_hash:
//				*Styles |= BS_RIGHT;
//				break;
//			case L"center"_hash:
//				*Styles |= BS_CENTER;
//				break;
//			case L"ljustify"_hash:
//				*Styles |= BS_LEFT;
//				break;
//			case L"right"_hash:
//				*Styles |= BS_RIGHTBUTTON;
//				break;
//			case L"pushlike"_hash:
//				*Styles |= BS_PUSHLIKE;
//			default:
//				break;
//		}
//#else
//		if ( tsStyle == TEXT("rjustify") )
//			*Styles |= BS_RIGHT;
//		else if ( tsStyle == TEXT("center") )
//			*Styles |= BS_CENTER;
//		else if ( tsStyle == TEXT("ljustify") )
//			*Styles |= BS_LEFT;
//		else if ( tsStyle == TEXT("right") )
//			*Styles |= BS_RIGHTBUTTON;
//		else if ( tsStyle == TEXT("pushlike") )
//			*Styles |= BS_PUSHLIKE;
//#endif
//	}
//
//	this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
//}

std::tuple<NoTheme, WindowStyle, WindowExStyle> DcxRadio::parseControlStyles(const TString & tsStyles)
{
	WindowStyle Styles(WindowStyle::None);
	WindowExStyle ExStyles(WindowExStyle::None);

	Styles |= BS_AUTORADIOBUTTON;

	for (const auto &tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"rjustify"_hash:
			Styles |= BS_RIGHT;
			break;
		case L"center"_hash:
			Styles |= BS_CENTER;
			break;
		case L"ljustify"_hash:
			Styles |= BS_LEFT;
			break;
		case L"right"_hash:
			Styles |= BS_RIGHTBUTTON;
			break;
		case L"pushlike"_hash:
			Styles |= BS_PUSHLIKE;
		default:
			break;
		}
	}

	return parseGeneralControlStyles(tsStyles, Styles, ExStyles);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxRadio::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	const auto prop(input.getfirsttok(3));

	// [NAME] [ID] [PROP]
	if ( prop == TEXT("text") )
	{
		GetWindowText( m_Hwnd, szReturnValue, MIRC_BUFFER_SIZE_CCH );
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("state"))
	{
		dcx_ConChar(dcx_testflag(Button_GetCheck(m_Hwnd), BST_CHECKED), szReturnValue);
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxRadio::parseCommandRequest( const TString & input )
{
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	//xdid -c [NAME] [ID] [SWITCH]
	if ( flags[TEXT('c')] )
	{
		Button_SetCheck( m_Hwnd, BST_CHECKED );
	}
	//xdid -t [NAME] [ID] [SWITCH] [TEXT]
	else if ( flags[TEXT('t')] && numtok > 3 )
	{
		SetWindowText(m_Hwnd, input.getlasttoks().trim().to_chr());	// tok 4, -1
	}
	//xdid -u [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('u')] )
	{
		Button_SetCheck( m_Hwnd, BST_UNCHECKED );
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxRadio::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed )
{
	return 0L;
}

LRESULT DcxRadio::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed )
{
	switch (uMsg)
	{
	case WM_ERASEBKGND:
	{
		bParsed = TRUE;
		return TRUE;
	}
	break;
	case WM_PRINTCLIENT:
	{
		this->DrawClientArea((HDC)wParam, uMsg, lParam);
		bParsed = TRUE;
	}
	break;
	case WM_PAINT:
	{
		bParsed = TRUE;
		PAINTSTRUCT ps{};

		auto hdc = BeginPaint(m_Hwnd, &ps);
		Auto(EndPaint(m_Hwnd, &ps));

		this->DrawClientArea(hdc, uMsg, lParam);
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

void DcxRadio::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	// Setup alpha blend if any.
	auto ai = SetupAlphaBlend(&hdc);
	Auto(FinishAlphaBlend(ai));

	if (this->m_bNoTheme || !Dcx::UXModule.dcxIsThemeActive())
	{
		// get controls client area
		if (RECT rcClient{}; GetClientRect(m_Hwnd, &rcClient))
		{
			if (this->m_clrBackText != CLR_INVALID)
				SetBkColor(hdc, this->m_clrBackText);

			if (this->m_clrText != CLR_INVALID)
				SetTextColor(hdc, this->m_clrText);

			auto bWasTransp = false;
			if (this->isExStyle(WindowExStyle::Transparent))
				bWasTransp = true;

			// fill background.
			if (bWasTransp)
			{
				if (!this->m_bAlphaBlend)
					this->DrawParentsBackground(hdc, &rcClient);
			}
			else
				DcxControl::DrawCtrlBackground(hdc, this, &rcClient);

			if (!bWasTransp)
				this->addExStyle(WindowExStyle::Transparent);

			CallDefaultProc(m_Hwnd, uMsg, (WPARAM)hdc, lParam);

			if (!bWasTransp)
				this->removeExStyle(WindowExStyle::Transparent);
		}
	}
	else
		CallDefaultProc(m_Hwnd, uMsg, (WPARAM)hdc, lParam);
}

void DcxRadio::toXml(TiXmlElement *const xml) const
{
	__super::toXml(xml);

	TString wtext;
	TGetWindowText(m_Hwnd, wtext);
	xml->SetAttribute("caption", wtext.c_str());
	xml->SetAttribute("styles", getStyles().c_str());
}

TiXmlElement * DcxRadio::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}
