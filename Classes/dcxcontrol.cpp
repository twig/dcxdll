/*!
 * \file dcxcontrol.cpp
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
#include "Dcx.h"

#include "Classes/dcxcontrol.h"
#include "Classes/dcxdialog.h"
#include "Classes/dcxprogressbar.h"
#include "Classes/dcxtrackbar.h"
#include "Classes/dcxtrackbar.h"
#include "Classes/dcxcomboex.h"
#include "Classes/dcxcolorcombo.h"
#include "Classes/dcxstatusbar.h"
#include "Classes/dcxtreeview.h"
#include "Classes/dcxtoolbar.h"
#include "Classes/dcxlistview.h"
#include "Classes/dcxrebar.h"
#include "Classes/dcxbutton.h"
#include "Classes/dcxrichedit.h"
#include "Classes/dcxupdown.h"
#include "Classes/dcxipaddress.h"
#include "Classes/dcxwebctrl.h"
#include "Classes/dcxcalendar.h"
#include "Classes/dcxdatetime.h"
#include "Classes/dcxpager.h"
#include "Classes/dcxdivider.h"
#include "Classes/dcxpanel.h"
#include "Classes/dcxtab.h"
#include "Classes/mirc/dcxbox.h"
#include "Classes/mirc/dcxline.h"
#include "Classes/mirc/dcxline.h"
#include "Classes/mirc/dcxradio.h"
#include "Classes/mirc/dcxcheck.h"
#include "Classes/mirc/dcxtext.h"
#include "Classes/mirc/dcxedit.h"
#include "Classes/mirc/dcxscroll.h"
#include "Classes/mirc/dcxlist.h"
#include "Classes/mirc/dcxlink.h"
#include "Classes/mirc/dcximage.h"
#include "Classes/dcxmwindow.h"
#include "Classes/dcxmdialog.h"
#include "Classes/dcxstacker.h"
 //#include "Classes/DcxMultiButton.h"
#include "Classes/DcxMultiCombo.h"
#include "Classes/DcxWebControl2.h"

#ifdef DCX_USE_DXSDK
#include "Classes/dcxdirectshow.h"
#endif // DCX_USE_DXSDK

 /*!
  * \brief Constructor
  *
  * \param mID Control ID
  * \param p_Dialog Parent DcxDialog Object
  */
DcxControl::DcxControl(const UINT mID, gsl::strict_not_null<DcxDialog* const> p_Dialog) noexcept
	: DcxWindow(mID)
	, m_pParentDialog(p_Dialog)
	, m_UserID(mID - mIRC_ID_OFFSET)
	//, m_dEventMask(p_Dialog->getEventMask())	// inherit the parent dialogs event mask
	//, m_ToolTipHWND(p_Dialog->getToolTip())
{
	//if (!m_bInitialized)
	//	throw Dcx::dcxException("DCX Controls NOT Initialized!");

	// Ook: should this be a copy of the parent controls eventmask?
	m_dEventMask = p_Dialog->getEventMask();	// inherit the parent dialogs event mask
}

DcxControl::DcxControl(const UINT mID, gsl::strict_not_null<DcxDialog* const> p_Dialog, DcxControlTypes eType) noexcept
	: DcxControl(mID, p_Dialog)
{
	m_eType = eType;
}

/*!
 * \brief Destructor
 *
 * Control Interface Destructor.
 *
 * Frees global control resources.
 */
DcxControl::~DcxControl() noexcept
{
	if (m_Hwnd)
		RemovePropW(m_Hwnd, TEXT("dcx_cthis"));

	// Reverse to old font
	setControlFont(nullptr, FALSE);

	// Delete background brush
	if (m_hBackBrush)
	{
		DeleteObject(m_hBackBrush);
		m_hBackBrush = nullptr;
	}
	// Delete border brush
	if (m_hBorderBrush)
	{
		DeleteObject(m_hBorderBrush);
		m_hBorderBrush = nullptr;
	}

	if (const auto pd = getParentDialog(); pd)
	{
		// check if we need to destroy the cursor (do not destroy if same cursor as parent dialog, parent will destroy this for us)
		if (m_hCursor && m_hCursor.cursor != pd->getCursor())
			DestroyCursor(m_hCursor.cursor);

		pd->deleteControl(this);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxControl::parseGeneralControlStyles(const TString& styles, dcxWindowStyles& ws)
{
	ws.m_Styles |= WS_CLIPCHILDREN | WS_VISIBLE;

	for (const auto& tsStyle : styles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"notheme"_hash:
			ws.m_NoTheme = true;
			break;
		case L"tabstop"_hash:
			ws.m_Styles |= WS_TABSTOP;
			break;
		case L"group"_hash:
			ws.m_Styles |= WS_GROUP;
			break;
		case L"disabled"_hash:
			ws.m_Styles |= WS_DISABLED;
			break;
		case L"transparent"_hash:
			ws.m_ExStyles |= WS_EX_TRANSPARENT;
			break;
		case L"hidden"_hash:
			ws.m_Styles &= ~WS_VISIBLE;
			break;
		case L"alpha"_hash:
			m_bAlphaBlend = true;
			break;
		case L"shadow"_hash:
			m_TextOptions.m_bShadow = true;
			break;
		case L"noformat"_hash:
			m_TextOptions.m_bNoCtrlCodes = true;
			break;
		case L"hgradient"_hash:
			m_bGradientFill = true;
			m_bGradientVertical = false;
			break;
		case L"vgradient"_hash:
		{
			m_bGradientFill = true;
			m_bGradientVertical = true;
		}
		break;
		default:
			break;
		}
	}
	return ws;
}

dcxWindowStyles DcxControl::parseGeneralControlStyles(const TString& styles)
{
	dcxWindowStyles ws;

	return parseGeneralControlStyles(styles, ws);
}

/*!
 * \brief blah
 *
 * NB: Possible buffer overwrite condition when returned data is longer than allocated szReturn
 */

GSL_SUPPRESS(type.3)
GSL_SUPPRESS(es.47)
GSL_SUPPRESS(lifetime.1)
GSL_SUPPRESS(f.55)
bool DcxControl::evalAliasEx(_Out_writes_z_(maxlen) TCHAR* const szReturn, _In_ const int maxlen, _In_z_ _Printf_format_string_ const TCHAR* const szFormat, ...) const
{
	if (!szReturn || !szFormat)
		return false;

	TString parms;
	va_list args = nullptr;

	va_start(args, szFormat);
	parms.tvprintf(szFormat, args);
	va_end(args);

	return getParentDialog()->evalAlias(mIRCResultString(szReturn), maxlen, parms.to_chr());
	//return getParentDialog()->evalAlias(szReturn, maxlen, parms.to_chr());
}

GSL_SUPPRESS(type.3)
GSL_SUPPRESS(es.47)
GSL_SUPPRESS(lifetime.1)
GSL_SUPPRESS(f.55)
bool DcxControl::execAliasEx(_In_z_ _Printf_format_string_ const TCHAR* const szFormat, ...) const
{
	if (!szFormat)
		return false;

	TString parms;
	va_list args = nullptr;

	va_start(args, szFormat);
	parms.tvprintf(szFormat, args);
	va_end(args);

	return getParentDialog()->execAlias(parms.to_chr());
}

/// <summary>
/// General commands
/// </summary>
/// <param name="input"> - The input for the specified command.</param>
/// <param name="flags"> - a supported command - b/C/e/E/f/F/h/J/M/p/x/U/R/s/T/z</param>
void DcxControl::parseGlobalCommandRequest(const TString& input, const XSwitchFlags& flags)
{
	const auto numtok = input.numtok();

	// xdid -f [NAME] [ID] [SWITCH] [+FLAGS] [CHARSET] [SIZE] [FONTNAME]
	// xdid -f [NAME] [ID] [SWITCH] [+FLAGS] [CHARSET] [SIZE] [FONTNAME] $chr(9) [TEXT STYLES] $chr(9) [TEXT SYLES MASK]
	if (flags[TEXT('f')])
	{
		//if (numtok < 7)
		//	throw DcxExceptions::dcxInvalidArguments();
		//if (LOGFONT lf{ }; ParseCommandToLogfont(input.gettok(4, -1), &lf))
		//	setControlFont(CreateFontIndirect(&lf), FALSE);
		//redrawWindow();

		const TString tsArgs(input.gettok(4, -1).strip());
		const TString tsFontArgs(tsArgs.getfirsttok(1, TSTABCHAR));
		const TString tsStyleArgs(tsArgs.getnexttok(TSTABCHAR));
		const TString tsStyleMaskArgs(tsArgs.getnexttok(TSTABCHAR));

		if (!tsFontArgs.empty())
		{
			if (tsFontArgs.numtok() < 4)
			throw DcxExceptions::dcxInvalidArguments();

			if (LOGFONT lf{ }; ParseCommandToLogfont(tsFontArgs, &lf))
			setControlFont(CreateFontIndirect(&lf), FALSE);
		}
		if (!tsStyleArgs.empty())
		{
			const dcxTextStyles ts = parseTextStyles(tsStyleArgs);
			const dcxTextStyles tsMask = parseTextStyles(tsStyleMaskArgs);

			if (dcx_testflag(ts, dcxTextStyles::NoCtrlCodes))
				m_TextOptions.m_bNoCtrlCodes = dcx_testflag(tsMask, dcxTextStyles::NoCtrlCodes);
			if (dcx_testflag(ts, dcxTextStyles::DblOutline))
				m_TextOptions.m_bDoubleOutline = dcx_testflag(tsMask, dcxTextStyles::DblOutline);
			if (dcx_testflag(ts, dcxTextStyles::OutlineFilled))
				m_TextOptions.m_bFilledOutline = dcx_testflag(tsMask, dcxTextStyles::OutlineFilled);
			if (dcx_testflag(ts, dcxTextStyles::Glow))
				m_TextOptions.m_bGlow = dcx_testflag(tsMask, dcxTextStyles::Glow);
			if ((dcx_testflag(ts, dcxTextStyles::HorizGradientText) || dcx_testflag(ts, dcxTextStyles::VertGradientText)))
				m_TextOptions.m_bGradientFill = (dcx_testflag(tsMask, dcxTextStyles::HorizGradientText) || dcx_testflag(tsMask, dcxTextStyles::VertGradientText));
			if ((dcx_testflag(ts, dcxTextStyles::HorizGradientOutline) || dcx_testflag(ts, dcxTextStyles::VertGradientOutline)))
				m_TextOptions.m_bGradientOutline = (dcx_testflag(tsMask, dcxTextStyles::HorizGradientOutline) || dcx_testflag(tsMask, dcxTextStyles::VertGradientOutline));
			if (dcx_testflag(ts, dcxTextStyles::HorizGradientText))
				m_TextOptions.m_bHorizGradientFill = dcx_testflag(tsMask, dcxTextStyles::HorizGradientText);
			if (dcx_testflag(ts, dcxTextStyles::HorizGradientOutline))
				m_TextOptions.m_bHorizGradientOutline = dcx_testflag(tsMask, dcxTextStyles::HorizGradientOutline);
			if (dcx_testflag(ts, dcxTextStyles::NoColours))
				m_TextOptions.m_bNoColours = dcx_testflag(tsMask, dcxTextStyles::NoColours);
			if (dcx_testflag(ts, dcxTextStyles::OutlineOnly))
				m_TextOptions.m_bOutline = dcx_testflag(tsMask, dcxTextStyles::OutlineOnly);
			if (dcx_testflag(ts, dcxTextStyles::Shadow))
				m_TextOptions.m_bShadow = dcx_testflag(tsMask, dcxTextStyles::Shadow);
			if (dcx_testflag(ts, dcxTextStyles::Transparent))
				m_TextOptions.m_bTransparent = dcx_testflag(tsMask, dcxTextStyles::Transparent);
			if (dcx_testflag(ts, dcxTextStyles::NewStyle))
				m_TextOptions.m_bUseNewStyle = dcx_testflag(tsMask, dcxTextStyles::NewStyle);
			if (dcx_testflag(ts, dcxTextStyles::AlphaBlend))
				m_TextOptions.m_bAlphaBlend = dcx_testflag(tsMask, dcxTextStyles::AlphaBlend);

			if (const auto tsPen(tsStyleArgs.wildtok(L"outlinesize=*", 1)); !tsPen.empty())
			{
				if (const auto iPen = tsPen.gettok(2, L'=').to_<UINT>(); (iPen > 0 && iPen <= 20))
				{
					m_TextOptions.m_uOutlineSize = gsl::narrow_cast<BYTE>(iPen);	// limit to 1 - 20
				}
			}
			if (const auto tsPen(tsStyleArgs.wildtok(L"shadowoffsetx=*", 1)); !tsPen.empty())
			{
				if (const auto iPen = tsPen.gettok(2, L'=').to_<UINT>(); (iPen > 0 && iPen <= 20))
				{
					m_TextOptions.m_uShadowXOffset = gsl::narrow_cast<BYTE>(iPen);	// limit to 1 - 20
				}
			}
			if (const auto tsPen(tsStyleArgs.wildtok(L"shadowoffsety=*", 1)); !tsPen.empty())
			{
				if (const auto iPen = tsPen.gettok(2, L'=').to_<UINT>(); (iPen > 0 && iPen <= 20))
				{
					m_TextOptions.m_uShadowYOffset = gsl::narrow_cast<BYTE>(iPen);	// limit to 1 - 20
				}
			}
			if (const auto tsPen(tsStyleArgs.wildtok(L"shadowalpha=*", 1)); !tsPen.empty())
			{
				if (const auto iPen = tsPen.gettok(2, L'=').to_<UINT>(); (iPen >= 0 && iPen <= 255))
				{
					m_TextOptions.m_uShadowAlpha = (gsl::narrow_cast<BYTE>(iPen) & 0xFFu);	// limit to 0 - 255
			}
			}
			if (const auto tsPen(tsStyleArgs.wildtok(L"shadowsize=*", 1)); !tsPen.empty())
			{
				if (const auto iPen = tsPen.gettok(2, L'=').to_<UINT>(); (iPen > 0 && iPen <= 20))
				{
					m_TextOptions.m_uShadowThickness = gsl::narrow_cast<BYTE>(iPen);	// limit to 1 - 20
				}
			}
			if (const auto tsPen(tsStyleArgs.wildtok(L"glowsize=*", 1)); !tsPen.empty())
			{
				if (const auto iPen = tsPen.gettok(2, L'=').to_<UINT>(); (iPen > 0 && iPen <= 20))
				{
					m_TextOptions.m_uGlowSize = gsl::narrow_cast<BYTE>(iPen);	// limit to 1 - 20
		}
			}
			if (const auto tsPen(tsStyleArgs.wildtok(L"alpha=*", 1)); !tsPen.empty())
			{
				if (const auto iPen = tsPen.gettok(2, L'=').to_<UINT>(); (iPen >= 0 && iPen <= 255))
				{
					m_TextOptions.m_uAlphaValue = (gsl::narrow_cast<BYTE>(iPen) & 0xFFu);	// limit to 0 - 255
		}
			}
		}
		redrawWindow();
	}
	// xdid -p [NAME] [ID] [SWITCH] [X] [Y] [W] [H]
	else if (flags[TEXT('p')])
	{
		if (numtok < 7)
			throw DcxExceptions::dcxInvalidArguments();

		auto x = input.getfirsttok(4).to_int();
		auto y = input.getnexttok().to_int();	// tok 5
		auto w = input.getnexttok().to_int();	// tok 6
		auto h = input.getnexttok().to_int();	// tok 7

		const Dcx::dcxWindowRect rc(m_Hwnd, GetParent(m_Hwnd));

		if (x == -1)
			x = rc.left;
		if (y == -1)
			y = rc.top;
		if (w == -1)
			w = rc.Width();
		if (h == -1)
			h = rc.Height();

		MoveWindow(m_Hwnd, x, y, w, h, TRUE);
		redrawBufferedWindow();
	}
	// xdid -x [NAME] [ID] [SWITCH] [+FLAGS]
	else if (flags[TEXT('x')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		this->removeStyle(WindowStyle::Border | WS_DLGFRAME);
		this->removeExStyle(WindowExStyle::ClientEdge | WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE);

		const auto [Styles, ExStyles, bIgnoreThis] = parseBorderStyles(input.gettok(4));

		this->addStyle(Styles);
		this->addExStyle(ExStyles);

		SetWindowPos(m_Hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		InvalidateRect(m_Hwnd, nullptr, TRUE);
		SendMessage(m_Hwnd, WM_NCPAINT, 1U, 0);
	}
	// xdid -C [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	else if (flags[TEXT('C')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iFlags = this->parseColorFlags(input.getfirsttok(4));
		const auto clrColor = input.getnexttok().to_<COLORREF>();	// tok 5

		{
			// Background colour options.
		if (dcx_testflag(iFlags, DcxColourFlags::BKGCOLOR))
		{
			if (this->m_hBackBrush)
			{
				DeleteObject(this->m_hBackBrush);
				this->m_hBackBrush = nullptr;
				this->m_clrBackground = CLR_INVALID;
			}

			if (clrColor != CLR_INVALID)
			{
				this->m_hBackBrush = CreateSolidBrush(clrColor);
				this->m_clrBackground = clrColor;
			}
		}
			if (dcx_testflag(iFlags, DcxColourFlags::GRADSTARTCOLOR))
				this->m_clrStartGradient = clrColor;

			if (dcx_testflag(iFlags, DcxColourFlags::GRADENDCOLOR))
				this->m_clrEndGradient = clrColor;
		}
		{
			// Text colour options.
		if (dcx_testflag(iFlags, DcxColourFlags::TEXTCOLOR))
				this->m_TextOptions.m_clrText = clrColor;

		if (dcx_testflag(iFlags, DcxColourFlags::TEXTBKGCOLOR))
				this->m_TextOptions.m_clrTextBackground = clrColor;

			if (dcx_testflag(iFlags, DcxColourFlags::TEXTOUTLINEGRADENDCOLOR))
				this->m_TextOptions.m_clrGradientOutlineEnd = clrColor;

			if (dcx_testflag(iFlags, DcxColourFlags::TEXTOUTLINEGRADSTARTCOLOR))
				this->m_TextOptions.m_clrGradientOutlineStart = clrColor;

			if (dcx_testflag(iFlags, DcxColourFlags::TEXTGRADENDCOLOR))
				this->m_TextOptions.m_clrGradientTextEnd = clrColor;

			if (dcx_testflag(iFlags, DcxColourFlags::TEXTGRADSTARTCOLOR))
				this->m_TextOptions.m_clrGradientTextStart = clrColor;

			if (dcx_testflag(iFlags, DcxColourFlags::TEXTOUTLINECOLOR))
				this->m_TextOptions.m_clrOutline = clrColor;

			if (dcx_testflag(iFlags, DcxColourFlags::TEXTSHADOWCOLOR))
				this->m_TextOptions.m_clrShadow = clrColor;

			if (dcx_testflag(iFlags, DcxColourFlags::TEXTGLOWCOLOR))
				this->m_TextOptions.m_clrGlow = clrColor;
		}
		if (dcx_testflag(iFlags, DcxColourFlags::BORDERCOLOR))
		{
			if (this->m_hBorderBrush)
			{
				DeleteObject(this->m_hBorderBrush);
				this->m_hBorderBrush = nullptr;
			}

			if (clrColor != CLR_INVALID)
				this->m_hBorderBrush = CreateSolidBrush(clrColor);
		}

		// force a control redraw
		this->redrawWindow();
	}
	// xdid -F [NAME] [ID] [SWITCH]
	else if (flags[TEXT('F')])
		SetFocus(m_Hwnd);
	// xdid -J [NAME] [ID] [SWITCH] [+FLAGS] [CURSOR|FILENAME]
	// xdid -J DNAME ID [+FLAGS] [CURSOR|FILENAME]
	else if (flags[TEXT('J')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const TString tsFlags(input.getfirsttok(4));
		TString tsFilename(input.getlasttoks());
		setCursor(tsFlags, tsFilename);
	}
	// xdid -M [NAME] [ID] [SWITCH] [MARK INFO]
	else if (flags[TEXT('M')])
	{
		TString info;

		if (numtok > 3)
			info = input.gettok(4, -1).trim();

		this->m_tsMark = info;
	}
	// xdid -Z [NAME] [ID] [SWITCH] [%]
	else if (flags[TEXT('Z')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto perc = gsl::narrow_cast<BYTE>(input.getfirsttok(4).to_int() & 0xFF);

		if (perc > 100)
			throw DcxExceptions::dcxInvalidArguments();

		int min = 0, max = 0;
		if (!GetScrollRange(m_Hwnd, SB_VERT, &min, &max))
			throw Dcx::dcxException("Unable to get scrollbar min/max");

		//scrollbar is defined and has visible range
		if (min > 0 || max > 0)
		{
			const auto pos = dcx_round(gsl::narrow_cast<float>((max - min)) * gsl::narrow_cast<float>(perc) / 100.0f);
			//const auto pos = Dcx::dcxRound<int>((max - min) * perc / 100.0f);

			//SCROLLINFO si;
			//ZeroMemory(&si, sizeof (SCROLLINFO));
			//si.cbSize = sizeof(SCROLLINFO);
			//si.fMask = SIF_POS;

			SCROLLINFO si{ sizeof(SCROLLINFO), SIF_POS,0,0,0,0,0 };

			if (!GetScrollInfo(m_Hwnd, SB_VERT, &si))
				throw Dcx::dcxException("Unable to get scroll info");

			si.nPos = pos;
			SetScrollInfo(m_Hwnd, SB_VERT, &si, TRUE);
			SendMessage(m_Hwnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, si.nPos), 0);
		}
	}
	// xdid -b [NAME] [ID]
	else if (flags[TEXT('b')])
		EnableWindow(m_Hwnd, FALSE);
	// xdid -e [NAME] [ID]
	else if (flags[TEXT('e')])
		EnableWindow(m_Hwnd, TRUE);
	// xdid -E [NAME] [ID] [+FLAGS] [-FLAGS]
	else if (flags[TEXT('E')])
	{
		const TString tspFlags(input.getfirsttok(4));
		const TString tsnFlags(input.getnexttok());

		this->setEventMask(tspFlags, tsnFlags);
	}
	// xdid -h [NAME] [ID] [SWITCH] (+FLAGS) (DURATION)
	else if (flags[TEXT('h')])
	{
		if (numtok > 4)
			AnimateWindow(m_Hwnd,
				input.gettok(5).to_dword(),
				gsl::narrow_cast<DWORD>((DcxDialog::getAnimateStyles(input.gettok(4)) | AW_HIDE) & ~AW_ACTIVATE));
		else
			ShowWindow(m_Hwnd, SW_HIDE);

		// shouldnt need to update layout on a hidden window....
		//RECT rc;
		//if (GetClientRect(this->getParentDialog()->getHwnd(), &rc))
		//{
		//	if (this->getParentDialog()->updateLayout(rc))
		//		this->getParentDialog()->redrawWindow(); // why do we need the redraw?
		//}
	}
	// xdid -s [NAME] [ID] [SWITCH] (+FLAGS) (DURATION)
	else if (flags[TEXT('s')])
	{
		if (numtok > 4)
		{
			AnimateWindow(m_Hwnd,
				input.gettok(5).to_dword(),
				gsl::narrow_cast<DWORD>((DcxDialog::getAnimateStyles(input.gettok(4)) | AW_ACTIVATE) & ~AW_HIDE));
		}
		else
			ShowWindow(m_Hwnd, SW_SHOW);

		if (RECT rc{}; GetClientRect(this->getParentDialog()->getHwnd(), &rc))
		{
			this->getParentDialog()->updateLayout(rc);

			//if (this->getParentDialog()->updateLayout(rc))
			//	this->getParentDialog()->redrawWindow(); // why do we need the redraw?
		}
	}
	// xdid -U [NAME] [ID]
	else if (flags[TEXT('U')])
	{
		// Box Double click Bug: the GetNextDlgtabItem() function never returns & seems to just loop forever.
		// from functions doc:
		//	If the search for the next control with the WS_TABSTOP
		//	style encounters a window with the WS_EX_CONTROLPARENT style,
		//	the system recursively searches the window's children.
		//
		//HWND hNextCtrl = GetNextDlgTabItem(this->getParentDialog()->getHwnd(), m_Hwnd, FALSE);
		//
		//if (hNextCtrl && (hNextCtrl != m_Hwnd))
		//	SendMessage(this->getParentDialog()->getHwnd(), WM_NEXTDLGCTL, (WPARAM) hNextCtrl, TRUE);
		////::PostMessage(this->getParentDialog()->getHwnd(), WM_NEXTDLGCTL, 0, FALSE);
		//else
		//	SetFocus(nullptr);

		// Just remove focus from all controls
		SetFocus(nullptr);
	}
	// xdid -T [NAME] [ID] [SWITCH] (ToolTipText)
	// xdid -T [NAME] [ID] [SWITCH] [+FLAGS] (ToolTipText)
	else if (flags[TEXT('T')])
	{
		TString tsText(input.gettok(4, -1));
		if (const XSwitchFlags xflags(tsText.getfirsttok(1)); xflags[TEXT('+')])
		{
			// close the tooltip window. NB: window may be reopened right away by control...
			if (xflags[TEXT('C')])
				this->CloseToolTip();

			// clear current tooltip text for control.
			if (xflags[TEXT('c')])
				this->m_tsToolTip.clear();

			// set the tooltip text
			if (xflags[TEXT('t')])
				this->m_tsToolTip = tsText.getlasttoks().trim();

			// open the tooltip
			if (xflags[TEXT('O')])
				this->OpenToolTip();
		}
		else
			this->m_tsToolTip = tsText.trim();

		//this->m_tsToolTip = (numtok > 3 ? input.gettok(4, -1).trim() : TEXT(""));
	}
	// xdid -R [NAME] [ID] [SWITCH] [+FLAG] [ARGS]
	else if (flags[TEXT('R')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const TString tsFlags(input.getfirsttok(4));
		const TString tsArgs(input.getlasttoks());
		setRegion(tsFlags, tsArgs);

		redrawWindow();
	}
	// invalid command
	else
		throw DcxExceptions::dcxInvalidCommand();
}

/// <summary>
/// Resizes bitmap, creating a copy & deleting the old one.
/// </summary>
/// <param name="srcBM"></param>
/// <param name="rc"></param>
/// <returns></returns>
GSL_SUPPRESS(lifetime.4)
HBITMAP DcxControl::resizeBitmap(HBITMAP srcBM, const RECT* const rc) noexcept
{
#if DCX_USE_WRAPPERS && 0

	if ((!srcBM) || (!rc))
		return nullptr;

	// set result to original bitmap incase resize fails at some point.
	auto hRes = srcBM;

	// get source bitmap info.
	BITMAP bm;
	if (GetObject(srcBM, sizeof(bm), &bm) == 0)
		return hRes;

	Dcx::dcxHDCBitmapResource srcDC(nullptr, srcBM);

	auto w = (rc->right - rc->left), h = (rc->bottom - rc->top);

	// create dest bitmap.
	Dcx::dcxHDCBitmap2Resource destDC(srcDC.get(), w, h);

	// resize bitmap, render into dest hdc/bitmap
	if (StretchBlt(destDC, 0, 0, w, h, srcDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY)) {
		GdiFlush();
		// set the return bitmap if resize worked.
		DeleteBitmap(hRes);
		hRes = destDC;
	}
	return hRes;
#else
	if ((!srcBM) || (!rc))
		return nullptr;

	// set result to original bitmap incase resize fails at some point.
	auto hRes = srcBM;

	// get source bitmap info.
	BITMAP bm{};
	if (GetObject(srcBM, sizeof(bm), &bm) == 0)
		return hRes;

	// create temp hdc to render from
	const auto srcDC = CreateCompatibleDC(nullptr);
	if (!srcDC)
		return hRes;

	// delete source hdc
	Auto(DeleteDC(srcDC));

	// select bitmap into new hdc
	const auto oldSrcBm = Dcx::dcxSelectObject<HBITMAP>(srcDC, srcBM);

	// set source hdc back to orig bitmap.
	Auto(Dcx::dcxSelectObject<HBITMAP>(srcDC, oldSrcBm));

	// create temp dest hdc to render into.
	const auto destDC = CreateCompatibleDC(srcDC);

	if (!destDC)
		return hRes;

	// delete dest hdc
	Auto(DeleteDC(destDC));

	const auto w = (rc->right - rc->left), h = (rc->bottom - rc->top);

	// create dest bitmap.
	auto newBM = CreateCompatibleBitmap(srcDC, w, h);

	if (!newBM)
		return hRes;

	Auto(DeleteObject(newBM));

	// select dest bitmap into dest hdc
	const auto oldDestBm = Dcx::dcxSelectObject<HBITMAP>(destDC, newBM);

	// set dest hdc back to orig bitmap.
	Auto(Dcx::dcxSelectObject<HBITMAP>(destDC, oldDestBm));

	// resize bitmap, render into dest hdc/bitmap
	if (StretchBlt(destDC, 0, 0, w, h, srcDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY))
	{
		GdiFlush();
		// set the return bitmap if resize worked.
		std::swap(hRes, newBM);
	}
	return hRes;
#endif
}

/// <summary>
/// Converts a string to a control type.
/// </summary>
/// <param name="t"></param>
/// <returns></returns>
DcxControlTypes DcxControl::TSTypeToControlType(const TString& t)
{
	// Ook: change to a switch statement...?

	const static std::map<std::hash<TString>::result_type, DcxControlTypes> dcxTypesMap{
		{TEXT("box"_hash), DcxControlTypes::BOX},
		{ TEXT("check"_hash), DcxControlTypes::CHECK },
		{ TEXT("edit"_hash), DcxControlTypes::EDIT },
		{ TEXT("image"_hash), DcxControlTypes::IMAGE },
		{ TEXT("line"_hash), DcxControlTypes::LINE },
		{ TEXT("link"_hash), DcxControlTypes::LINK },
		{ TEXT("list"_hash), DcxControlTypes::LIST },
		{ TEXT("radio"_hash), DcxControlTypes::RADIO },
		{ TEXT("scroll"_hash), DcxControlTypes::SCROLL },
		{ TEXT("text"_hash), DcxControlTypes::TEXT },
		{ TEXT("button"_hash), DcxControlTypes::BUTTON },
		{ TEXT("calendar"_hash), DcxControlTypes::CALENDAR },
		{ TEXT("colorcombo"_hash), DcxControlTypes::COLORCOMBO },
		{ TEXT("comboex"_hash), DcxControlTypes::COMBOEX },
		{ TEXT("datetime"_hash), DcxControlTypes::DATETIME },
		{ TEXT("directshow"_hash), DcxControlTypes::DIRECTSHOW },
		{ TEXT("divider"_hash), DcxControlTypes::DIVIDER },
		{ TEXT("ipaddress"_hash), DcxControlTypes::IPADDRESS },
		{ TEXT("listview"_hash), DcxControlTypes::LISTVIEW },
		{ TEXT("dialog"_hash), DcxControlTypes::DIALOG },
		{ TEXT("window"_hash), DcxControlTypes::WINDOW },
		{ TEXT("pager"_hash), DcxControlTypes::PAGER },
		{ TEXT("panel"_hash), DcxControlTypes::PANEL },
		{ TEXT("pbar"_hash), DcxControlTypes::PROGRESSBAR },
		{ TEXT("rebar"_hash), DcxControlTypes::REBAR },
		{ TEXT("richedit"_hash), DcxControlTypes::RICHEDIT },
		{ TEXT("stacker"_hash), DcxControlTypes::STACKER },
		{ TEXT("statusbar"_hash), DcxControlTypes::STATUSBAR },
		{ TEXT("tab"_hash), DcxControlTypes::TABB },
		{ TEXT("toolbar"_hash), DcxControlTypes::TOOLBAR },
		{ TEXT("trackbar"_hash), DcxControlTypes::TRACKBAR },
		{ TEXT("treeview"_hash), DcxControlTypes::TREEVIEW },
		{ TEXT("updown"_hash), DcxControlTypes::UPDOWN },
		{ TEXT("webctrl"_hash), DcxControlTypes::WEBCTRL },
		{ TEXT("web2ctrl"_hash), DcxControlTypes::WEB2CTRL },
		{ TEXT("multibutton"_hash), DcxControlTypes::MULTIBUTTON },
		{ TEXT("multicombo"_hash), DcxControlTypes::MULTICOMBO },
		{ TEXT("grid"_hash), DcxControlTypes::GRID }
	};

	if (const auto got = dcxTypesMap.find(std::hash<TString>()(t)); got != dcxTypesMap.end())
		return got->second;
	return DcxControlTypes::UNKNOWN;
}

/*!
 * \brief blah
 *
 * blah
 */

const DcxColourFlags DcxControl::parseColorFlags(const TString& flags) noexcept
{
	const XSwitchFlags xflags(flags);
	DcxColourFlags iFlags{};

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	// background colours
	if (xflags[TEXT('b')])
		iFlags |= DcxColourFlags::BKGCOLOR;
	if (xflags[TEXT('g')])
		iFlags |= DcxColourFlags::GRADSTARTCOLOR;
	if (xflags[TEXT('G')])
		iFlags |= DcxColourFlags::GRADENDCOLOR;

	// text colours.
	if (xflags[TEXT('k')])
		iFlags |= DcxColourFlags::TEXTBKGCOLOR;
	if (xflags[TEXT('t')])
		iFlags |= DcxColourFlags::TEXTCOLOR;
	if (xflags[TEXT('e')])
		iFlags |= DcxColourFlags::TEXTGRADSTARTCOLOR;
	if (xflags[TEXT('E')])
		iFlags |= DcxColourFlags::TEXTGRADENDCOLOR;
	if (xflags[TEXT('o')])
		iFlags |= DcxColourFlags::TEXTOUTLINECOLOR;
	if (xflags[TEXT('p')])
		iFlags |= DcxColourFlags::TEXTOUTLINEGRADSTARTCOLOR;
	if (xflags[TEXT('P')])
		iFlags |= DcxColourFlags::TEXTOUTLINEGRADENDCOLOR;
	if (xflags[TEXT('s')])
		iFlags |= DcxColourFlags::TEXTSHADOWCOLOR;
	if (xflags[TEXT('S')])
		iFlags |= DcxColourFlags::TEXTGLOWCOLOR;

	// border colour.
	if (xflags[TEXT('r')])
		iFlags |= DcxColourFlags::BORDERCOLOR;

	// for checkboxes
	// checkbox background colours.
	if (xflags[TEXT('h')])
		iFlags |= DcxColourFlags::CHECKBOXHOT;	// when control is hot.
	if (xflags[TEXT('d')])
		iFlags |= DcxColourFlags::CHECKBOXDISABLED;	// when control is disabled.
	if (xflags[TEXT('c')])
		iFlags |= DcxColourFlags::CHECKBOXBGCOLOR;
    // checkbox frame colour.
	if (xflags[TEXT('f')])
		iFlags |= DcxColourFlags::CHECKBOXFRAMECOLOR;
	// checkbox tick colour.
	if (xflags[TEXT('C')])
		iFlags |= DcxColourFlags::CHECKBOXTICKCOLOR;

	return iFlags;
}

/*!
 * \brief Get Information about a control.
 *
 * blah
 */
bool DcxControl::parseGlobalInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	//	switch (std::hash<TString>{}(input.getfirsttok(3)))
	//	{
	//	case L"hwnd"_hash:
	//	{
	//		_ts_snprintf(szReturnValue, TEXT("%lu"), (DWORD)m_Hwnd);	// can't use %p as this gives a hex result.
	//		return true;
	//	}
	//	break;
	//	case L"visible"_hash:
	//	{
	//		szReturnValue = dcx_truefalse((IsWindowVisible(m_Hwnd) != FALSE));
	//		return true;
	//	}
	//	break;
	//	case L"enabled"_hash:
	//	{
	//		szReturnValue = dcx_truefalse((IsWindowEnabled(m_Hwnd) != FALSE));
	//		return true;
	//	}
	//	break;
	//	case L"pos"_hash:
	//	{
	//		const auto rc = getWindowPosition();
	//		_ts_snprintf(szReturnValue, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	//		return true;
	//	}
	//	break;
	//	case L"dpos"_hash:
	//	{
	//#if DCX_USE_WRAPPERS
	//		const Dcx::dcxWindowRect rc(m_Hwnd, getParentDialog()->getHwnd());
	//
	//		_ts_snprintf(szReturnValue, TEXT("%d %d %d %d"), rc.left, rc.top, rc.Width(), rc.Height());
	//#else
	//		if (RECT rc{}; GetWindowRect(m_Hwnd, &rc))
	//		{
	//			MapWindowRect(nullptr, getParentDialog()->getHwnd(), &rc);
	//
	//			_ts_snprintf(szReturnValue, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	//		}
	//#endif
	//		return true;
	//	}
	//	break;
	//	case L"mark"_hash:
	//	{
	//		szReturnValue = m_tsMark.to_chr();
	//		return true;
	//	}
	//	break;
	//	case L"mouse"_hash:
	//	{
	//#if DCX_USE_WRAPPERS
	//		if (const Dcx::dcxCursorPos pt(m_Hwnd); pt)
	//		{
	//			_ts_snprintf(szReturnValue, TEXT("%d %d"), pt.x, pt.y);
	//			return true;
	//		}
	//#else
	//		if (POINT pt{}; GetCursorPos(&pt))
	//		{
	//			MapWindowPoints(nullptr, m_Hwnd, &pt, 1);
	//			_ts_snprintf(szReturnValue, TEXT("%d %d"), pt.x, pt.y);
	//			return true;
	//		}
	//#endif
	//	}
	//	break;
	//	case L"pid"_hash:
	//	{
	//#if DCX_USE_WRAPPERS
	//		auto hParent = GetParent(m_Hwnd);
	//
	//		if (const Dcx::dcxClassName sClassname(hParent); sClassname == TEXT("#32770"))
	//			szReturnValue = TEXT('0');
	//		else
	//			_ts_snprintf(szReturnValue, TEXT("%u"), getParentDialog()->getControlByHWND(hParent)->getUserID());
	//
	//		return true;
	//#else
	//		stString<257> sClassname;
	//
	//		auto hParent = GetParent(m_Hwnd);
	//		GetClassName(hParent, sClassname, gsl::narrow_cast<int>(sClassname.size()));
	//
	//		if (sClassname == TEXT("#32770"))
	//			szReturnValue = TEXT('0');
	//		else
	//			_ts_snprintf(szReturnValue, TEXT("%u"), getParentDialog()->getControlByHWND(hParent)->getUserID());
	//
	//		return true;
	//#endif
	//	}
	//	break;
	//	case L"type"_hash:
	//	{
	//		szReturnValue = getType().to_chr();
	//		return true;
	//	}
	//	break;
	//	case L"styles"_hash:
	//	{
	//		szReturnValue = getStyles().to_chr();
	//		return true;
	//	}
	//	break;
	//	case L"font"_hash:
	//	{
	//		auto hFontControl = getControlFont();
	//
	//		if (!hFontControl)
	//			hFontControl = Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT);
	//
	//		if (hFontControl)
	//		{
	//			if (auto [code, lfCurrent] = Dcx::dcxGetObject<LOGFONT>(hFontControl); code != 0)
	//			{
	//				szReturnValue = ParseLogfontToCommand(&lfCurrent).to_chr();
	//				return true;
	//			}
	//		}
	//	}
	//	break;
	//	// [NAME] [ID] [PROP]
	//	case L"tooltipbgcolor"_hash:
	//	{
	//		COLORREF cref = CLR_INVALID;
	//
	//		if (m_ToolTipHWND)
	//			cref = gsl::narrow_cast<COLORREF>(SendMessage(m_ToolTipHWND, TTM_GETTIPBKCOLOR, 0, 0));
	//
	//		_ts_snprintf(szReturnValue, TEXT("%lu"), cref);
	//		return true;
	//	}
	//	break;
	//	// [NAME] [ID] [PROP]
	//	case L"tooltiptextcolor"_hash:
	//	{
	//		COLORREF cref = CLR_INVALID;
	//
	//		if (m_ToolTipHWND)
	//			cref = gsl::narrow_cast<COLORREF>(SendMessage(m_ToolTipHWND, TTM_GETTIPTEXTCOLOR, 0, 0));
	//
	//		_ts_snprintf(szReturnValue, TEXT("%lu"), cref);
	//		return true;
	//	}
	//	// [NAME] [ID] [PROP]
	//	case L"alpha"_hash:
	//	{
	//		szReturnValue = dcx_truefalse(m_bAlphaBlend);
	//		return true;
	//	}
	//	break;
	//	// [NAME] [ID] [PROP]
	//	case L"textcolor"_hash:
	//	{
	//		_ts_snprintf(szReturnValue, TEXT("%u"), m_clrText);
	//		return true;
	//	}
	//	break;
	//	// [NAME] [ID] [PROP]
	//	case L"textbgcolor"_hash:
	//	{
	//		_ts_snprintf(szReturnValue, TEXT("%u"), m_clrBackText);
	//		return true;
	//	}
	//	break;
	//	// [NAME] [ID] [PROP]
	//	case L"bgcolor"_hash:
	//	{
	//		_ts_snprintf(szReturnValue, TEXT("%u"), m_clrBackground);
	//		return true;
	//	}
	//	default:
	//		throw Dcx::dcxException("Invalid property or number of arguments");
	//		break;
	//	}
	//	return false;

	szReturnValue = parseGlobalInfoRequest(input).to_chr();
	return true;
}

/// <summary>
/// Get general information on a control.
/// throws on error.
/// </summary>
/// <param name="input">- Property to get from control.</param>
/// <returns></returns>
TString DcxControl::parseGlobalInfoRequest(const TString& input) const
{
	TString tsResult(gsl::narrow_cast<TString::size_type>(MIRC_BUFFER_SIZE_CCH));

	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
	case L"hwnd"_hash:
	{
		tsResult += from_hwnd<size_t>(m_Hwnd);
	}
	break;
	case L"visible"_hash:
	{
		tsResult = dcx_truefalse((IsWindowVisible(m_Hwnd) != FALSE));
	}
	break;
	case L"enabled"_hash:
	{
		tsResult = dcx_truefalse((IsWindowEnabled(m_Hwnd) != FALSE));
	}
	break;
	case L"pos"_hash:
	{
		const auto rc = getWindowPosition();
		tsResult.tsprintf(TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	}
	break;
	case L"dpos"_hash:
	{
		if (const auto pd = getParentDialog(); pd)
		{
#if DCX_USE_WRAPPERS
			const Dcx::dcxWindowRect rc(m_Hwnd, pd->getHwnd());

			tsResult.tsprintf(TEXT("%d %d %d %d"), rc.left, rc.top, rc.Width(), rc.Height());
#else
			if (RECT rc{}; GetWindowRect(m_Hwnd, &rc))
			{
				MapWindowRect(nullptr, pd->getHwnd(), &rc);

				tsResult.tsprintf(TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
			}
#endif
		}
	}
	break;
	case L"mark"_hash:
	{
		tsResult = m_tsMark;
	}
	break;
	case L"mouse"_hash:
	{
#if DCX_USE_WRAPPERS
		if (const Dcx::dcxCursorPos pt(m_Hwnd); pt)
		{
			tsResult.tsprintf(TEXT("%d %d"), pt.x, pt.y);
		}
#else
		if (POINT pt{}; GetCursorPos(&pt))
		{
			MapWindowPoints(nullptr, m_Hwnd, &pt, 1);
			tsResult.tsprintf(TEXT("%d %d"), pt.x, pt.y);
		}
#endif
	}
	break;
	case L"pid"_hash:
	{
#if DCX_USE_WRAPPERS
		auto hParent = GetParent(m_Hwnd);

		if (const Dcx::dcxClassName sClassname(hParent); sClassname == TEXT("#32770"))
			tsResult = TEXT('0');
		else
			tsResult += getParentDialog()->getControlByHWND(hParent)->getUserID();
#else
		stString<257> sClassname;

		auto hParent = GetParent(m_Hwnd);
		GetClassName(hParent, sClassname, gsl::narrow_cast<int>(sClassname.size()));

		if (sClassname == TEXT("#32770"))
			tsResult = TEXT('0');
		else
			tsResult += getParentDialog()->getControlByHWND(hParent)->getUserID();
#endif
	}
	break;
	case L"type"_hash:
	{
		tsResult = getType();
	}
	break;
	case L"styles"_hash:
	{
		tsResult = getStyles();
	}
	break;
	case L"font"_hash:
	{
		tsResult = FontToCommand();
	}
	break;
	// [NAME] [ID] [PROP]
	case L"tooltipbgcolour"_hash:
	case L"tooltipbgcolor"_hash:
	{
		COLORREF cref = CLR_INVALID;

		if (getToolTipHWND())
			cref = Dcx::dcxToolTip_GetTipBkColor(getToolTipHWND());

		tsResult += cref;
	}
	break;
	// [NAME] [ID] [PROP]
	case L"tooltiptextcolour"_hash:
	case L"tooltiptextcolor"_hash:
	{
		COLORREF cref = CLR_INVALID;

		if (getToolTipHWND())
			cref = Dcx::dcxToolTip_GetTipTextColor(getToolTipHWND());

		tsResult += cref;
	}
	break;
	// [NAME] [ID] [PROP]
	case L"alpha"_hash:
	{
		tsResult = dcx_truefalse(m_bAlphaBlend);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"textcolour"_hash:
	case L"textcolor"_hash:
	{
		tsResult += m_TextOptions.m_clrText;
	}
	break;
	// [NAME] [ID] [PROP]
	case L"textbgcolour"_hash:
	case L"textbgcolor"_hash:
	{
		tsResult += m_TextOptions.m_clrTextBackground;
	}
	break;
	case L"textoutlinecolour"_hash:
	case L"textoutlinecolor"_hash:
	{
		tsResult += m_TextOptions.m_clrOutline;
	}
	break;
	case L"textoutlinegradientstartcolour"_hash:
	case L"textoutlinegradientstartcolor"_hash:
	{
		tsResult += m_TextOptions.m_clrGradientOutlineStart;
	}
	break;
	case L"textoutlinegradientendcolour"_hash:
	case L"textoutlinegradientendcolor"_hash:
	{
		tsResult += m_TextOptions.m_clrGradientOutlineEnd;
	}
	break;
	case L"textgradientstartcolour"_hash:
	case L"textgradientstartcolor"_hash:
	{
		tsResult += m_TextOptions.m_clrGradientTextStart;
	}
	break;
	case L"textgradientendcolour"_hash:
	case L"textgradientendcolor"_hash:
	{
		tsResult += m_TextOptions.m_clrGradientTextEnd;
	}
	break;
	case L"textshadowcolour"_hash:
	case L"textshadowcolor"_hash:
	{
		tsResult += m_TextOptions.m_clrShadow;
	}
	break;
	// [NAME] [ID] [PROP]
	case L"bgcolour"_hash:
	case L"bgcolor"_hash:
	{
		tsResult += m_clrBackground;
	}
	break;
	case L"dpi"_hash:
	{
		tsResult += m_uDPI;
	}
	break;
	default:
		throw Dcx::dcxException("Invalid property or number of arguments");
		break;
	}
	return tsResult;
}

void DcxControl::HandleDragDrop(int x, int y) noexcept
{
	switch (getControlType())
	{
	case DcxControlTypes::LISTVIEW:
	{
		//((DcxListView*)this))->HandleDragDrop(x, y);
		//(reinterpret_cast<DcxListView*>(this))->HandleDragDrop(x, y);
		(dynamic_cast<DcxListView*>(this))->HandleDragDrop(x, y);
	}
	break;
	default:
		break;
	}
}

void DcxControl::HandleDragMove(int x, int y) noexcept
{
	switch (getControlType())
	{
	case DcxControlTypes::LISTVIEW:
	{
		//((DcxListView*)this)->HandleDragMove(x, y);
		//(reinterpret_cast<DcxListView*>(this))->HandleDragMove(x, y);
		(dynamic_cast<DcxListView*>(this))->HandleDragMove(x, y);
	}
	break;
	default:
		break;
	}
}

LRESULT CALLBACK DcxControl::WindowProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DcxControl* pthis{};

	if (uMsg == WM_NCCREATE)
	{
		const CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);

		// no params, fail...
		if ((!cs) || (!cs->lpCreateParams))
			return FALSE; // cause CreateWindowEx() to fail.

		// get control object
		pthis = static_cast<DcxControl*>(cs->lpCreateParams);
		if (!pthis)
			return FALSE; // cause CreateWindowEx() to fail.

		// set this control objects hwnd.
		pthis->m_Hwnd = mHwnd;

		// save control object to this hwnd
		if (!Dcx::dcxSetProp(mHwnd, TEXT("dcx_cthis"), pthis))
			return FALSE;	// SetProp() failed, cause CreateWindowEx() to fail.
	}
	else
		pthis = Dcx::dcxGetProp<DcxControl*>(mHwnd, TEXT("dcx_cthis"));

	// sanity check, see that prop exists.
	if (!pthis)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	if (const auto pDialog = pthis->getParentDialog(); pDialog)
	{
		if (uMsg == WM_PAINT && pDialog->IsVistaStyle())
		{
			ValidateRect(mHwnd, nullptr);

			if (RECT rcUpdate{}; GetWindowRect(mHwnd, &rcUpdate))
				pDialog->UpdateVistaStyle(&rcUpdate);

			return 0L;
		}

		if (const auto fBlocked = (InSendMessageEx(nullptr) & (ISMEX_REPLIED | ISMEX_SEND)) == ISMEX_SEND; !fBlocked)
		{
			// If Message is blocking just call old win proc
			BOOL bParsed = FALSE;

			if ((uMsg != WM_DESTROY) && (uMsg != WM_NCDESTROY))
				pthis->incRef();

			const auto lrRes = pthis->OurMessage(uMsg, wParam, lParam, bParsed);

			if ((uMsg != WM_DESTROY) && (uMsg != WM_NCDESTROY))
				pthis->decRef();

			if (bParsed)
				return lrRes;
		}
	}
	return pthis->CallDefaultClassProc(uMsg, wParam, lParam);
}

/*!
 * \brief blah
 *
 * blah
 *
 * Input [NAME] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
 */
GSL_SUPPRESS(r.11)
DcxControl* DcxControl::controlFactory(gsl::strict_not_null<DcxDialog* const> p_Dialog, const UINT mID, const TString& tsInput, const UINT offset, const DcxAllowControls mask = DcxAllowControls::ALLOW_ALL, HWND hParent)
{
	if (!m_bInitialized)
		throw Dcx::dcxException("DCX Controls NOT Initialized!");

	const auto type(tsInput.getfirsttok(gsl::narrow_cast<int>(offset)));

	const RECT rc = TSToRect(tsInput);

	const auto styles(tsInput.getlasttoks());

	//const auto oldContext = Dcx::DpiModule.dcxSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	//Auto(Dcx::DpiModule.dcxSetThreadDpiAwarenessContext(oldContext));

	if (!hParent)
		hParent = p_Dialog->getHwnd();

	if (!hParent)
		throw Dcx::dcxException("Unable to get Parent");

	switch (DcxControl::TSTypeToControlType(type))
	{
	case DcxControlTypes::PROGRESSBAR:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_PBAR))
			return new DcxProgressBar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::TRACKBAR:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_TRACKBAR))
			return new DcxTrackBar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::COMBOEX:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_COMBOEX))
			return new DcxComboEx(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::COLORCOMBO:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_COLORCOMBO))
			return new DcxColorCombo(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::STATUSBAR:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_STATUSBAR))
			return new DcxStatusBar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::TOOLBAR:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_TOOLBAR))
			return new DcxToolBar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::TREEVIEW:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_TREEVIEW))
			return new DcxTreeView(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::LISTVIEW:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_LISTVIEW))
			return new DcxListView(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::REBAR:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_REBAR))
			return new DcxReBar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::BUTTON:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_BUTTON))
			return new DcxButton(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::RICHEDIT:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_RICHEDIT))
			return new DcxRichEdit(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::UPDOWN:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_UPDOWN))
			return new DcxUpDown(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::IPADDRESS:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_IPADDRESS))
			return new DcxIpAddress(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::WEBCTRL:
		//if (dcx_testflag(mask, DcxAllowControls::ALLOW_WEBCTRL))
		//	return new DcxWebControl(mID, p_Dialog, hParent, &rc, styles);
	{
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_WEBCTRL))
		{
			auto ctrl_p = new DcxWebControl(mID, p_Dialog, hParent, &rc, styles);

			if (!ctrl_p->InitializeInterface())
			{
				DestroyWindow(ctrl_p->getHwnd());
				throw DcxExceptions::dcxUnableToCreateWindow();
			}

			return ctrl_p;
		}
	}
	break;
	case DcxControlTypes::WEB2CTRL:
	{
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_WEB2CTRL))
		{
			auto ctrl_p = new DcxWebControl2(mID, p_Dialog, hParent, &rc, styles);

			ctrl_p->InitializeInterface();

			//if (!ctrl_p->InitializeInterface())
			//{
			//	DestroyWindow(ctrl_p->getHwnd());
			//	throw DcxExceptions::dcxUnableToCreateWindow();
			//}

			return ctrl_p;
		}
	}
	break;
	case DcxControlTypes::CALENDAR:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_CALANDER))
			return new DcxCalendar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::DATETIME:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_CALANDER))
			return new DcxDateTime(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::DIVIDER:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_DIVIDER))
			return new DcxDivider(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::MULTICOMBO:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_MULTICOMBO))
			return new DcxMultiCombo(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::PANEL:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_PANEL))
			return new DcxPanel(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::TABB:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_TAB))
			return new DcxTab(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::LINE:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_LINE))
			return new DcxLine(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::BOX:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_BOX))
			return new DcxBox(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::RADIO:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_RADIO))
			return new DcxRadio(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::CHECK:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_CHECK))
			return new DcxCheck(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::TEXT:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_TEXT))
			return new DcxText(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::EDIT:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_EDIT))
			return new DcxEdit(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::SCROLL:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_SCROLL))
			return new DcxScroll(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::LIST:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_LIST))
			return new DcxList(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::LINK:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_LINK))
			return new DcxLink(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::IMAGE:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_IMAGE))
			return new DcxImage(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::PAGER:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_PAGER))
			return new DcxPager(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::STACKER:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_STACKER))
			return new DcxStacker(mID, p_Dialog, hParent, &rc, styles);
		break;
#ifdef DCX_USE_DXSDK

	case DcxControlTypes::DIRECTSHOW:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_DIRECTSHOW))
			return new DcxDirectshow(mID, p_Dialog, hParent, &rc, styles);
		break;

#endif // DCX_USE_DXSDK

	case DcxControlTypes::WINDOW:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_DOCK))
		{
			if (styles.empty())
				throw Dcx::dcxException("No window name");

			const auto tsWin(styles.getfirsttok(1));

			// this helps stop '@' being passed as $window(@).hwnd == $window(-2).hwnd & usually causes a crash.
			if (tsWin.len() < 2)
				throw Dcx::dcxException(TEXT("No such window: %"), tsWin);

			auto winHwnd = to_hwnd(tsWin.to_<size_t>());
			if (!IsWindow(winHwnd))
			{
				//stString<30> windowHwnd;
				//mIRCLinker::evalex(windowHwnd, static_cast<const int>(windowHwnd.size()), TEXT("$window(%s).hwnd"), tsWin.to_chr());
				//winHwnd = (HWND)dcx_atoi(windowHwnd.data());
				//
				//stString<30> windowHwnd;
				//mIRCLinker::eval(windowHwnd, TEXT("$window(%).hwnd"), tsWin);
				//winHwnd = to_hwnd(dcx_atoi(windowHwnd.data()));

				TString tsRes;
				mIRCLinker::eval(tsRes, TEXT("$window(%).hwnd"), tsWin);
				winHwnd = to_hwnd(tsRes.to_<size_t>());

				//TString tsRes;
				//if (auto o = mIRCLinker::o_eval<TString>(TEXT("$window(%).hwnd"), tsWin); o.has_value())
				//	tsRes = o.value();
				//winHwnd = to_hwnd(tsRes.to_<size_t>());
			}

			if (!IsWindow(winHwnd))
				throw Dcx::dcxException(TEXT("No such window: %"), tsWin);

			if (p_Dialog->getControlByHWND(winHwnd))
				throw Dcx::dcxException("Window already a DCX Control");

			return new DcxMWindow(winHwnd, hParent, mID, p_Dialog, &rc, styles);
		}
		break;
	case DcxControlTypes::DIALOG:
		if (dcx_testflag(mask, DcxAllowControls::ALLOW_DOCK))
		{
			if (styles.empty())
				throw Dcx::dcxException("No dialog name");

			const auto tsDname(styles.getfirsttok(1));
			const auto winHwnd = GetHwndFromString(tsDname);

			if (!IsWindow(winHwnd))
				throw Dcx::dcxException(TEXT("No such dialog: %"), tsDname);

			if (p_Dialog->getControlByHWND(winHwnd))
				throw Dcx::dcxException(TEXT("Control already exists : %"), tsDname);

			//auto newDialog = new DcxMDialog(winHwnd, hParent, mID, p_Dialog, &rc, styles);
			auto newDialog = std::make_unique<DcxMDialog>(winHwnd, hParent, mID, p_Dialog, &rc, styles);

			// if its a dcx marked dialog, mark the parent name
			if (auto dlg = Dcx::Dialogs.getDialogByHandle(winHwnd); dlg)
				dlg->setParentName(p_Dialog->getName());

			return newDialog.release();
		}
		break;
	case DcxControlTypes::UNKNOWN:
		throw Dcx::dcxException(TEXT("Unknown Control Type: %"), type);
		break;
	}
	throw Dcx::dcxException(TEXT("Control Type not supported: %"), type);
	return nullptr;	// never gets executed, but compilers complain about its absense...
}

/// <summary>
/// Setup a control based on some dcxml.
/// </summary>
/// <param name="xDcxml"> - the base dcxml tag</param>
/// <param name="xCtrl"> - the current controls xml</param>
void DcxControl::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xCtrl)
{
	if (!xDcxml || !xCtrl || !m_Hwnd)
		return;

	// set any styles that apply
	xmlSetStyles();

	// set control icons if any. ???
	xmlLoadIcons(xCtrl);

	// check <control> for any style info
	xmlSetStyle(xCtrl);

	if (const auto clr = gsl::narrow_cast<DWORD>(queryIntAttribute(xCtrl, "eventmask", -1)); clr != MAXDWORD)
		this->m_dEventMask = clr;

	if (const auto clr = gsl::narrow_cast<BYTE>(queryIntAttribute(xCtrl, "alpha", 255)); clr != 255)
		this->m_iAlphaLevel = clr;

	if (auto tmp = queryAttribute(xCtrl, "mark"); !_ts_isEmpty(tmp))
		this->m_tsMark = tmp;
	if (auto tmp = queryAttribute(xCtrl, "tooltip"); !_ts_isEmpty(tmp))
		this->m_tsToolTip = tmp;

	if (auto xCursor = xCtrl->FirstChildElement("cursor"); xCursor)
	{
		TString tsCursor(queryEvalAttribute(xCursor, "src", "arrow"));
		const TString tsFlags(queryAttribute(xCursor, "flags", "+r"));

		setCursor(tsFlags, tsCursor);
	}

	// Load region data
	// NB: cant save region data yet.
	if (auto xRegion = xCtrl->FirstChildElement("region"); xRegion)
	{
		TString tsRegion(queryEvalAttribute(xRegion, "src"));
		const TString tsFlags(queryAttribute(xRegion, "flags", "+"));

		if (!tsRegion.empty())
			setRegion(tsFlags, tsRegion);
	}
}

GSL_SUPPRESS(type.4)
LRESULT DcxControl::setFont(const HFONT hFont, const BOOL fRedraw) noexcept
{
	return SendMessage(m_Hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), MAKELPARAM(fRedraw, 0));
}

void DcxControl::setCursor(const TString& tsFlags, TString& tsFilename)
{
	const auto iFlags = this->parseCursorFlags(tsFlags);
	const auto* const CursorType = this->parseCursorType(tsFilename);

	// if previous cursor was the dialogs cursor, just set as blank
	if (m_hCursor.cursor == getParentDialog()->getCursor())
		m_hCursor.cursor = nullptr;

	m_hCursor.src = tsFilename;
	m_hCursor.flags = tsFlags;

	m_hCursor.cursor = Dcx::dcxLoadCursor(iFlags, CursorType, m_hCursor.enabled, m_hCursor.cursor, tsFilename);
}

void DcxControl::setRegion(const TString& tsFlags, const TString& tsArgs)
{
	const XSwitchFlags xflags(tsFlags);

	if (!xflags[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	HRGN hRegion = nullptr;
	auto RegionMode = 0;
	auto noRegion = false;

#if DCX_USE_WRAPPERS
	const Dcx::dcxWindowRect rc(m_Hwnd);
#else
	RECT rc = { 0 };
	if (!GetWindowRect(m_Hwnd, &rc))
		throw Dcx::dcxException("Unable to get window rect!");
#endif

	if (xflags[TEXT('o')])
		RegionMode = RGN_OR;
	else if (xflags[TEXT('a')])
		RegionMode = RGN_AND;
	else if (xflags[TEXT('i')])
		RegionMode = RGN_DIFF;
	else if (xflags[TEXT('x')])
		RegionMode = RGN_XOR;
	else if (xflags[TEXT('c')])
		RegionMode = RGN_COPY;

	if (xflags[TEXT('f')]) // image file - [COLOR] [FILE]
	{
		if (tsArgs.numtok() < 2)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tCol = tsArgs.getfirsttok(1).to_<COLORREF>();
		auto filename(tsArgs.getlasttoks());

		auto bitmapRgn = dcxLoadBitmap(nullptr, filename);

		if (!bitmapRgn)
			throw Dcx::dcxException("Unable To Load Image file.");
		Auto(DeleteObject(bitmapRgn));

		if (xflags[TEXT('R')]) // now resize image to match control.
			bitmapRgn = resizeBitmap(bitmapRgn, &rc);
		hRegion = BitmapRegion(bitmapRgn, tCol, (tCol != CLR_INVALID));
	}
	else if (xflags[TEXT('r')]) // rounded rect - radius args (optional)
	{
		const auto radius = (!tsArgs.empty()) ? tsArgs.to_int() : 20;

		hRegion = CreateRoundRectRgn(0, 0, rc.right - rc.left, rc.bottom - rc.top, radius, radius);
	}
	else if (xflags[TEXT('c')]) // circle - radius arg (optional)
	{
		if (!tsArgs.empty())
		{
			auto radius = tsArgs.to_int();
			if (radius < 1)
				radius = 100; // handle cases where arg isnt a number or is a negative.
			const auto cx = ((rc.right - rc.left) / 2);
			const auto cy = ((rc.bottom - rc.top) / 2);
			hRegion = CreateEllipticRgn(cx - radius, cy - radius, cx + radius, cy + radius);
		}
		else
			hRegion = CreateEllipticRgn(0, 0, rc.right - rc.left, rc.bottom - rc.top);
	}
	else if (xflags[TEXT('p')]) // polygon
	{
		// u need at least 3 points for a shape
		if (tsArgs.numtok() < 3)
			throw DcxExceptions::dcxInvalidArguments();

		const auto strPoints(tsArgs);
		const auto tPoints = strPoints.numtok();

		if (tPoints < 3)
			throw Dcx::dcxException("Invalid Points: At least 3 points required.");

		GSL_SUPPRESS(r.5) auto pnts = std::make_unique<POINT[]>(tPoints);
		UINT cnt = 0;

		for (const auto& strPoint : strPoints)
		{
			gsl::at(pnts, cnt).x = strPoint.getfirsttok(1, TSCOMMACHAR).to_<LONG>();
			gsl::at(pnts, cnt).y = strPoint.getnexttok(TSCOMMACHAR).to_<LONG>();
			++cnt;
		}

		hRegion = CreatePolygonRgn(pnts.get(), gsl::narrow_cast<int>(tPoints), WINDING);
	}
	else if (xflags[TEXT('b')])
	{ // alpha [1|0] [level]
		noRegion = true;
		if (tsArgs.numtok() != 2)
			throw DcxExceptions::dcxInvalidArguments();

		m_bAlphaBlend = (tsArgs.getfirsttok(1).to_int() > 0);

		const auto alpha = gsl::narrow_cast<BYTE>(tsArgs.getnexttokas<int>() & 0xFF);

		if (alpha == 255U)
			m_bAlphaBlend = false;
		m_iAlphaLevel = alpha;
	}
	else {
		if (!xflags[TEXT('n')]) // none, no args
			throw DcxExceptions::dcxInvalidFlag();

		noRegion = true;
		SetWindowRgn(m_Hwnd, nullptr, FALSE);	// redraw at end
	}

	if (!noRegion)
	{
		if (!hRegion)
			throw Dcx::dcxException("Unable to create region.");

		if (RegionMode != 0)
		{
			if (auto wrgn = CreateRectRgn(0, 0, 0, 0); wrgn)
			{
				Auto(DeleteRgn(wrgn));
				if (GetWindowRgn(m_Hwnd, wrgn) != ERROR)
					CombineRgn(hRegion, hRegion, wrgn, RegionMode);
			}
		}
		SetWindowRgn(m_Hwnd, hRegion, FALSE);	// redraw at end
	}
}

GSL_SUPPRESS(lifetime)
HFONT DcxControl::getFont() const noexcept
{
	return (m_Hwnd ? GetWindowFont(m_Hwnd) : nullptr);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxControl::setRedraw(const BOOL fView) noexcept
{
	return SendMessage(m_Hwnd, WM_SETREDRAW, gsl::narrow_cast<WPARAM>(fView), LPARAM{ 0 });
}

/*!
 * \brief blah
 *
 * blah
 */

GSL_SUPPRESS(type.4)
void DcxControl::setControlFont(const HFONT hFont, const BOOL fRedraw) noexcept
{
	//if (auto hControlFont = this->getFont(); hControlFont != Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT))
	//{
	//	if (hControlFont)
	//	{
	//		DeleteFont(hControlFont);
	//		this->m_hFont = nullptr;
	//	}
	//	else if (this->m_hFont)
	//	{
	//		DeleteFont(this->m_hFont);
	//		this->m_hFont = nullptr;
	//	}
	//}
	//
	//this->setFont(hFont, fRedraw);
	//this->m_hFont = hFont;

	//if (auto hControlFont = this->getFont(); hControlFont != Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT))
	//{
	//	if (hControlFont)
	//	{
	//		DeleteFont(hControlFont);
	//	}
	//
	//	if (this->m_hFont == hControlFont)
	//		this->m_hFont = nullptr;
	//}
	//
	//if (this->m_hFont)
	//{
	//	DeleteFont(this->m_hFont);
	//	this->m_hFont = nullptr;
	//}
	//
	//this->setFont(hFont, fRedraw);
	//this->m_hFont = hFont;

	if (auto hControlFont = this->getFont(); hControlFont != Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT))
	{
		if ((hControlFont) && (this->m_hFont != hControlFont))
			DeleteFont(hControlFont);
	}

	if (this->m_hFont)
	{
		// its OK to delete stock font.
		DeleteFont(this->m_hFont);
		this->m_hFont = nullptr;
	}

	this->setFont(hFont, fRedraw);
	this->m_hFont = hFont;
}

/*!
* \brief blah
*
* blah
*/

const RECT DcxControl::getWindowPosition() const noexcept
{
#if DCX_USE_WRAPPERS
	const Dcx::dcxWindowRect rc(m_Hwnd, GetParent(m_Hwnd));
	return rc.CopyRect();
#else
	RECT rc{};
	GetWindowRectParent(m_Hwnd, &rc);
	return rc;
#endif
}

TString DcxControl::FontToCommand() const
{
	TString tsResult;

	auto hFontControl = getControlFont();

	if (!hFontControl)
		hFontControl = Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT);

	if (hFontControl)
	{
		if (auto [code, lfCurrent] = Dcx::dcxGetObject<LOGFONT>(hFontControl); code != 0)
		{
			tsResult = ParseLogfontToCommand(&lfCurrent);
		}
	}
	return tsResult;
}

dcxTextStyles DcxControl::parseTextStyles(const TString& tsStyles)
{
	dcxTextStyles ts{};

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"shadow"_hash:
			ts |= dcxTextStyles::Shadow;
			break;
		case L"noformat"_hash:
			ts |= dcxTextStyles::NoCtrlCodes;
			break;
		case L"vgtext"_hash:
			ts |= dcxTextStyles::VertGradientText;
			break;
		case L"hgtext"_hash:
			ts |= dcxTextStyles::HorizGradientText;
			break;
		case L"outlineonly"_hash:
			ts |= dcxTextStyles::OutlineOnly;
			break;
		case L"filledoutline"_hash:
			ts |= dcxTextStyles::OutlineFilled;
			break;
		case L"vgoutline"_hash:
			ts |= dcxTextStyles::VertGradientOutline;
			break;
		case L"hgoutline"_hash:
			ts |= dcxTextStyles::HorizGradientOutline;
			break;
		case L"nocolours"_hash:
		case L"nocolors"_hash:
			ts |= dcxTextStyles::NoColours;
			break;
		case L"glow"_hash:
			ts |= dcxTextStyles::Glow;
			break;
		case L"dbloutline"_hash:
			ts |= dcxTextStyles::DblOutline;
			break;
		case L"transparent"_hash:
			ts |= dcxTextStyles::Transparent;
			break;
		case L"newstyle"_hash:
			ts |= dcxTextStyles::NewStyle;
			break;
		case L"alpha"_hash:
			ts |= dcxTextStyles::AlphaBlend;
			break;
		default:
			break;
		}
	}
	return ts;
}

void DcxControl::updateParentCtrl() noexcept
{
	// find the host control, if any.
	this->m_pParentHWND = GetParent(m_Hwnd);
}

void DcxControl::DrawCtrlBackground(const HDC hdc, const DcxControl* const p_this, const RECT* const rwnd, HTHEME hTheme, const int iPartId, const int iStateId) noexcept
{
	if ((!hdc) || (!p_this))
		return;

	// if transparent, do nothing...
	if (p_this->isExStyle(WindowExStyle::Transparent))
		return;

	// fill background.
	RECT rc{};
	if (!rwnd)
	{
		if (!GetClientRect(p_this->getHwnd(), &rc))
			return;
	}
	else
		CopyRect(&rc, rwnd);

	if (!IsWindowEnabled(p_this->m_Hwnd))
	{// use disabled colouring when windows disabled.
		//TMT_BACKGROUND
		//TMT_FILLCOLOR
		//TMT_BTNFACE

		//if (hTheme && p_this->IsThemed() && DcxUXModule::dcxIsThemeActive())
		//{
		//	if (DcxUXModule::dcxIsThemeBackgroundPartiallyTransparent(hTheme, iPartId, iStateId))
		//		DcxUXModule::dcxDrawThemeParentBackground(p_this->m_Hwnd, hdc, &rc);
		//	DcxUXModule::dcxDrawThemeBackground(hTheme, hdc, iPartId, iStateId, &rc, nullptr);
		//}
		//else
		//	FillRect(hdc, &rc, GetSysColorBrush(COLOR_3DFACE));

		COLORREF clr{ CLR_INVALID };
		if (hTheme && p_this->IsThemed() && DcxUXModule::dcxIsThemeActive())
			DcxUXModule::dcxGetThemeColor(hTheme, iPartId, iStateId, TMT_BTNFACE, &clr);

		if (clr == CLR_INVALID)
			clr = GetSysColor(COLOR_3DFACE);

		Dcx::FillRectColour(hdc, &rc, clr);
		}
	else if (p_this->m_bGradientFill)
	{
		const auto clrStart = (p_this->getStartGradientColor() != CLR_INVALID) ? p_this->getStartGradientColor() : GetSysColor(COLOR_3DFACE);
		const auto clrEnd = (p_this->getEndGradientColor() != CLR_INVALID) ? p_this->getEndGradientColor() : GetSysColor(COLOR_GRADIENTACTIVECAPTION);

		XPopupMenuItem::DrawGradient(hdc, &rc, clrStart, clrEnd, !p_this->m_bGradientVertical);
	}
	else {
		auto hBrush = p_this->getBackClrBrush();
		if (!hBrush)
		{
			if (hTheme && p_this->IsThemed() && DcxUXModule::dcxIsThemeActive())
			{
			//	//if (DcxUXModule::dcxIsThemeBackgroundPartiallyTransparent(hTheme, iPartId, iStateId))
			//	//	DcxUXModule::dcxDrawThemeParentBackground(p_this->m_Hwnd, hdc, &rc);
			//	//DcxUXModule::dcxDrawThemeBackground(hTheme, hdc, iPartId, iStateId, &rc, nullptr);

				COLORREF clr{ CLR_INVALID };
				DcxUXModule::dcxGetThemeColor(hTheme, iPartId, iStateId, TMT_BTNFACE, &clr);

				if (clr == CLR_INVALID)
					clr = GetSysColor(COLOR_3DFACE);

				Dcx::FillRectColour(hdc, &rc, clr);
				return;
			}
			else
				hBrush = GetSysColorBrush(COLOR_3DFACE);
		}
		if (hBrush)
			FillRect(hdc, &rc, hBrush);
	}
}

void DcxControl::DrawControl(HDC hDC, HWND hwnd) const
{
	if (!hDC || !hwnd)
		return;

#if DCX_USE_WRAPPERS
	// if window matches this one, don't draw (loop condition)
	if (hwnd == m_Hwnd)
		return;

	// if window isn't visible, don't draw.
	if (!IsWindowVisible(hwnd))
		return;

	// if window is within a background paint of it's own, don't draw. (loop condition)
	if (const auto* const p_ctrl = this->getParentDialog()->getControlByHWND(hwnd); (p_ctrl && p_ctrl->m_bInPrint))
		return;

	const Dcx::dcxWindowRect rc(hwnd, GetParent(hwnd));

	// if window isn't within the client area of the control who's background we are drawing, don't draw.
	if (!RectVisible(hDC, &rc))
		return;

	const auto w = rc.Width(), h = rc.Height();

	const Dcx::dcxBitmapResource hBitmap(hDC, w, h);

	const Dcx::dcxHDCBitmapResource hdcMemory(hDC, hBitmap.get());

	::SendMessage(hwnd, WM_ERASEBKGND, reinterpret_cast<WPARAM>(hdcMemory.get()), 1L); // HACK: using 1L instead of zero as a workaround for stacker.
	::SendMessage(hwnd, WM_PRINT, reinterpret_cast<WPARAM>(hdcMemory.get()), gsl::narrow_cast<LPARAM>(PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE /*| PRF_ERASEBKGND*/));

	BitBlt(hDC, rc.left, rc.top, w, h, hdcMemory.get(), 0, 0, SRCCOPY);
#else
	// if window matches this one, don't draw (loop condition)
	if (hwnd == m_Hwnd)
		return;

	// if window isnTEXT('t visible, don')t draw.
	if (!IsWindowVisible(hwnd))
		return;

	// if window is within a background paint of it's own, don't draw. (loop condition)
	if (const auto* const p_ctrl = this->getParentDialog()->getControlByHWND(hwnd); (p_ctrl && p_ctrl->m_bInPrint))
		return;

	//RECT rc;
	//if (!GetWindowRect(hwnd, &rc))
	//	return;
	//
	//MapWindowRect(nullptr, GetParent(hwnd), &rc);

	RECT rc{};
	if (!GetWindowRectParent(hwnd, &rc))
		return;

	// if window isn't within the client area of the control who's background we are drawing, don't draw.
	if (!RectVisible(hDC, &rc))
		return;

	auto hdcMemory = ::CreateCompatibleDC(hDC);

	if (!hdcMemory)
		return;

	Auto(DeleteDC(hdcMemory));

	auto w = (rc.right - rc.left), h = (rc.bottom - rc.top);
	auto hBitmap = ::CreateCompatibleBitmap(hDC, w, h);

	if (!hBitmap)
		return;

	Auto(DeleteObject(hBitmap));

	const auto hbmpOld = Dcx::dcxSelectObject<HBITMAP>(hdcMemory, hBitmap);

	Auto(Dcx::dcxSelectObject<HBITMAP>(hdcMemory, hbmpOld));

	::SendMessage(hwnd, WM_ERASEBKGND, reinterpret_cast<WPARAM>(hdcMemory), 1L); // HACK: using 1L instead of zero as a workaround for stacker.
	::SendMessage(hwnd, WM_PRINT, reinterpret_cast<WPARAM>(hdcMemory), gsl::narrow_cast<LPARAM>(PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE /*| PRF_ERASEBKGND*/));

	BitBlt(hDC, rc.left, rc.top, w, h, hdcMemory, 0, 0, SRCCOPY);
#endif

}

void DcxControl::DrawParentsBackground(const HDC hdc, const RECT* const rcBounds, const HWND dHwnd)
{
	if (!hdc || !IsValidWindow())
		return;

	// fill in parent bg
	RECT rcClient{};
	auto hwnd = m_Hwnd;

	if (dHwnd)
		hwnd = dHwnd;

	if (!rcBounds)
	{
		if (!GetClientRect(hwnd, &rcClient)) // get controls client area
			return;
	}
	else
		rcClient = *rcBounds;

	// if themes are active use them.
//	if (dcxIsThemeActive() && !this->m_bNoTheme) {
//#ifdef DCX_USE_WINSDK
//		if (DrawThemeParentBackgroundExUx != nullptr)
//			DrawThemeParentBackgroundExUx(hwnd, hdc, 0, &rcClient); // Vista only, does basicly the same as below.
//		else
//#endif
//			DrawThemeParentBackgroundUx(hwnd, hdc, &rcClient); // XP+
//		return;
//	}

	/*
		The following code draws the parents background & client area,
		followed by all child controls covered by this one.
	*/
	updateParentCtrl(); // find the host control, if any.

	const auto pDialog = getParentDialog();
	if (!pDialog)
		return;

	//If in Vista mode
	if (pDialog->IsVistaStyle())
	{
		// Check if the hdc to render too is the main hdc, if so bkg is already drawn so just return
		if (hdc == pDialog->GetVistaHDC())
			return;
		// Check if parent is dialog.
		if (m_pParentHWND == pDialog->getHwnd())
		{
			// When in vista mode dialog has already been drawn
			// So just grab image from windows DC.
			auto hdcParent = pDialog->GetVistaHDC();
			auto rcWin = rcClient;
			pDialog->MapVistaRect(hwnd, &rcWin);
			BitBlt(hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top), hdcParent, rcWin.left, rcWin.top, SRCCOPY);
			return;
		}
	}
	if (pDialog->isExStyle(WindowExStyle::Composited))
	{
		// When in composited mode underling controls have already been drawn
		// So just grab image from windows DC.

		if (auto hdcParent = GetDC(m_pParentHWND); hdcParent)
		{
			Auto(ReleaseDC(m_pParentHWND, hdcParent));

			auto rcWin = rcClient;
			MapWindowRect(hwnd, m_pParentHWND, &rcWin);

			BitBlt(hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
				hdcParent, rcWin.left, rcWin.top, SRCCOPY);
		}
		return;
	}

	//// make a new HDC for background rendering
	//HDC hdcbkg = CreateCompatibleDC( hdc );
	//if (hdcbkg != nullptr) {
	//	//RECT rcParentWin, rcParentClient, rcWin;
	//	RECT rcParentWin, rcWin;
	//	// get parent windows client area.
	//	GetClientRect(this->m_pParentHWND,&rcParentWin);
	//	// make a bitmap for rendering to.
	//	HBITMAP memBM = CreateCompatibleBitmap ( hdc, rcParentWin.right - rcParentWin.left, rcParentWin.bottom - rcParentWin.top );
	//	if (memBM != nullptr) {
	//		// get this controls x & y pos within its parent.
	//		rcWin = rcClient;
	//		MapWindowPoints(hwnd,this->m_pParentHWND, (LPPOINT)&rcWin, 2);
	//		// associate bitmap with HDC
	//		HBITMAP oldBM = (HBITMAP)SelectObject ( hdcbkg, memBM );
	//		HRGN clipRgn = CreateRectRgnIndirect(&rcWin); // clip parents drawing to this controls rect.
	//		if (clipRgn != nullptr) {
	//			SelectClipRgn(hdcbkg, clipRgn);
	//			DeleteObject(clipRgn);
	//		}
	//		// Sending WM_ERASEBKGND followed by WM_PRINTCLIENT emulates the method used by DrawThemeParentBackgroundEx() on vista.
	//		this->m_bInPrint = true; // this helps prevent long drawing loops
	//		// fill in the parents image
	//		::SendMessage(this->m_pParentHWND, WM_ERASEBKGND, (WPARAM)hdcbkg,1L); // HACK: using 1L instead of nullptr as a workaround for stacker.
	//		::SendMessage(this->m_pParentHWND, WM_PRINTCLIENT, (WPARAM)hdcbkg,PRF_CLIENT);
	//		// now draw all child controls within area of this control.
	//		// NB: AVOID EnumChildWindows()
	//		HWND child = GetWindow(m_Hwnd, GW_HWNDPREV);
	//		while (child != nullptr) {
	//			this->DrawControl(hdcbkg, child);
	//			child = GetWindow(child, GW_HWNDPREV);
	//		}
	//		this->m_bInPrint = false;
	//		// draw background to main hdc
	//		BitBlt( hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
	//			hdcbkg, rcWin.left, rcWin.top, SRCCOPY);
	//		DeleteObject(SelectObject( hdcbkg, oldBM ));
	//	}
	//	DeleteDC( hdcbkg );
	//}

	// make a new HDC for background rendering
	RECT rcParentWin{}, rcWin{};
	if (!GetClientRect(m_pParentHWND, &rcParentWin))
		return;

#if DCX_USE_WRAPPERS
	const Dcx::dcxHDCBuffer hdcbkg(hdc, rcParentWin);

	// get this controls x & y pos within its parent.
	rcWin = rcClient;
	MapWindowRect(hwnd, m_pParentHWND, &rcWin);

	if (auto clipRgn = CreateRectRgnIndirect(&rcWin); /* clip parents drawing to this controls rect. */ clipRgn)
	{
		SelectClipRgn(hdcbkg.get(), clipRgn);
		DeleteObject(clipRgn);
	}

	// Sending WM_ERASEBKGND followed by WM_PRINTCLIENT emulates the method used by DrawThemeParentBackgroundEx() on vista.
	m_bInPrint = true; // this helps prevent long drawing loops
	// fill in the parents image
	::SendMessage(m_pParentHWND, WM_ERASEBKGND, reinterpret_cast<WPARAM>(hdcbkg.get()), 1L); // HACK: using 1L instead of zero as a workaround for stacker.
	::SendMessage(m_pParentHWND, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdcbkg.get()), PRF_CLIENT);

	for (auto hChild = GetWindow(m_Hwnd, GW_HWNDPREV); hChild; hChild = GetWindow(hChild, GW_HWNDPREV))
		DrawControl(hdcbkg.get(), hChild);

	m_bInPrint = false;
	// draw background to main hdc
	BitBlt(hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top), hdcbkg.get(), rcWin.left, rcWin.top, SRCCOPY);
#else
	auto hdcbkg = CreateHDCBuffer(hdc, &rcParentWin);
	if (!hdcbkg)
		return;
	Auto(DeleteHDCBuffer(hdcbkg));

	// get this controls x & y pos within its parent.
	rcWin = rcClient;
	MapWindowRect(hwnd, this->m_pParentHWND, &rcWin);

	if (auto clipRgn = CreateRectRgnIndirect(&rcWin); /* clip parents drawing to this controls rect. */ clipRgn)
	{
		SelectClipRgn(*hdcbkg, clipRgn);
		DeleteObject(clipRgn);
	}

	// Sending WM_ERASEBKGND followed by WM_PRINTCLIENT emulates the method used by DrawThemeParentBackgroundEx() on vista.
	this->m_bInPrint = true; // this helps prevent long drawing loops
	// fill in the parents image
	::SendMessage(this->m_pParentHWND, WM_ERASEBKGND, reinterpret_cast<WPARAM>(*hdcbkg), 1L); // HACK: using 1L instead of zero as a workaround for stacker.
	::SendMessage(this->m_pParentHWND, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(*hdcbkg), PRF_CLIENT);

	//::SendMessage(this->m_pParentHWND, WM_PRINT, (WPARAM)*hdcbkg,PRF_CLIENT|PRF_ERASEBKGND);
	// now draw all child controls within area of this control.
	// NB: AVOID EnumChildWindows()
	//HWND child = GetWindow(m_Hwnd, GW_HWNDPREV);
	//while (child != nullptr) {
	//	this->DrawControl(*hdcbkg, child);
	//	child = GetWindow(child, GW_HWNDPREV);
	//}

	for (auto hChild = GetWindow(m_Hwnd, GW_HWNDPREV); hChild; hChild = GetWindow(hChild, GW_HWNDPREV))
		this->DrawControl(*hdcbkg, hChild);

	this->m_bInPrint = false;
	// draw background to main hdc
	BitBlt(hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
		*hdcbkg, rcWin.left, rcWin.top, SRCCOPY);
#endif
}

GSL_SUPPRESS(lifetime.4)
LPALPHAINFO DcxControl::SetupAlphaBlend(HDC* hdc, const bool DoubleBuffer)
{
	if ((!hdc) || (!*hdc) || (!this->IsAlphaBlend() && !DoubleBuffer))
		return nullptr;

	auto ai = std::make_unique<ALPHAINFO>(m_Hwnd);

	/*
		1: draw parents bg to temp hdc
		2: copy bg to temp hdcbkg for later alpha
		3: draw button to temp hdc, over parents bg
		4: alpha blend temp hdc to hdc
	*/

	if (DcxUXModule::IsBufferedPaintSupported())
	{
		BP_PAINTPARAMS paintParams{ sizeof(BP_PAINTPARAMS),BPPF_ERASE, nullptr, nullptr };
		if (this->IsAlphaBlend())
		{
			// we are doing alpha
			// create buffer to hold alpha blended image (this buffer will be drawn to the orig hdc)
			ai->ai_BufferAlpha = DcxUXModule::dcxBeginBufferedPaint(*hdc, &ai->ai_rcClient, BPBF_TOPDOWNDIB /*BPBF_COMPATIBLEBITMAP*/, &paintParams, &ai->ai_hdcAlpha);
			if (!ai->ai_BufferAlpha)
				return nullptr;

			// fill alpha buffer with bkg image.
			this->DrawParentsBackground(ai->ai_hdcAlpha, &ai->ai_rcClient);

			// create buffer to hold image than needs to be alpha blended.
			ai->ai_bf.SourceConstantAlpha = this->m_iAlphaLevel; // 0x7f half of 0xff = 50% transparency
			paintParams.pBlendFunction = &ai->ai_bf;
			ai->ai_Buffer = DcxUXModule::dcxBeginBufferedPaint(ai->ai_hdcAlpha, &ai->ai_rcClient, BPBF_TOPDOWNDIB /*BPBF_COMPATIBLEBITMAP*/, &paintParams, &ai->ai_hdc);
			if (!ai->ai_Buffer)
				return nullptr;

			// copy parent bkg to this buffer too
			BitBlt(ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, ai->ai_rcClient.right - ai->ai_rcClient.left, ai->ai_rcClient.bottom - ai->ai_rcClient.top, ai->ai_hdcAlpha, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);

			// return new buffered hdc.
			ai->ai_Oldhdc = *hdc;
			*hdc = ai->ai_hdc;
			return ai.release();
		}
		else {
			// buffering only.
			// create a buffer to hold all drawing.
			ai->ai_Buffer = DcxUXModule::dcxBeginBufferedPaint(*hdc, &ai->ai_rcClient, BPBF_TOPDOWNDIB /*BPBF_COMPATIBLEBITMAP*/, &paintParams, &ai->ai_hdc);
		if (ai->ai_Buffer)
		{
				// first fill with parents image.
			this->DrawParentsBackground(ai->ai_hdc, &ai->ai_rcClient);
			ai->ai_Oldhdc = *hdc;
			*hdc = ai->ai_hdc;
			return ai.release();
		}
	}
	}
	// if vista method failed, fall through to our own method.

	//{
	//	// create a new HDC for alpha blending.
	//	ai->ai_hdc = CreateCompatibleDC( *hdc );
	//	ai->ai_bkg = nullptr;
	//	if (ai->ai_hdc != nullptr) {
	//		//GetClientRect(m_Hwnd,&ai->ai_rcWin);
	//		GetWindowRect(m_Hwnd,&ai->ai_rcWin);
	//		// create a bitmap to render to
	//		ai->ai_bitmap = CreateCompatibleBitmap ( *hdc, ai->ai_rcWin.right - ai->ai_rcWin.left, ai->ai_rcWin.bottom - ai->ai_rcWin.top );
	//		if (ai->ai_bitmap != nullptr) {
	//			GetClientRect(m_Hwnd,&ai->ai_rcClient);
	//			// associate bitmap with hdc
	//			ai->ai_oldBM = (HBITMAP)SelectObject ( ai->ai_hdc, ai->ai_bitmap );
	//			// fill in parent bg
	//			this->DrawParentsBackground(ai->ai_hdc, &ai->ai_rcClient);
	//			// If alpha blending, make a background bitmap & fill it.
	//			if (this->IsAlphaBlend()) {
	//				// avoid doing the whole background rendering again by simply copying the one we just did.
	//				HDC hdcbkg = CreateCompatibleDC( *hdc );
	//				if (hdcbkg != nullptr) {
	//					ai->ai_bkg = CreateCompatibleBitmap ( *hdc, ai->ai_rcWin.right - ai->ai_rcWin.left, ai->ai_rcWin.bottom - ai->ai_rcWin.top );
	//					if (ai->ai_bkg != nullptr) {
	//						HBITMAP oldBM = (HBITMAP)SelectObject ( hdcbkg, ai->ai_bkg );
	//
	//						BitBlt( hdcbkg, ai->ai_rcClient.left, ai->ai_rcClient.top, ai->ai_rcClient.right - ai->ai_rcClient.left, ai->ai_rcClient.bottom - ai->ai_rcClient.top, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
	//
	//						SelectObject(hdcbkg, oldBM);
	//					}
	//					DeleteDC(hdcbkg);
	//				}
	//			}
	//			ai->ai_Oldhdc = *hdc;
	//			*hdc = ai->ai_hdc;
	//		}
	//	}
	//}
	// create a new HDC Buffer for alpha blending.

	ai->ai_hdcBuffer = CreateHDCBuffer(*hdc, &ai->ai_rcWin);
	ai->ai_bkg = nullptr;
	if (ai->ai_hdcBuffer)
	{
		// assign hdc for easy refrence & compat with previous code.
		ai->ai_hdc = *ai->ai_hdcBuffer;
		// fill in parent bg
		DrawParentsBackground(ai->ai_hdc, &ai->ai_rcClient);
		// If alpha blending, make a background bitmap & fill it.
		if (IsAlphaBlend())
		{
			// avoid doing the whole background rendering again by simply copying the one we just did.
			if (auto hdcbkg = CreateCompatibleDC(*hdc); hdcbkg)
			{
				Auto(DeleteDC(hdcbkg));

				ai->ai_bkg = CreateCompatibleBitmap(*hdc, ai->ai_rcWin.right - ai->ai_rcWin.left, ai->ai_rcWin.bottom - ai->ai_rcWin.top);
				if (ai->ai_bkg)
				{
					auto oldBM = Dcx::dcxSelectObject<HBITMAP>(hdcbkg, ai->ai_bkg);
					Auto(Dcx::dcxSelectObject<HBITMAP>(hdcbkg, oldBM));

					BitBlt(hdcbkg, ai->ai_rcClient.left, ai->ai_rcClient.top, ai->ai_rcClient.right - ai->ai_rcClient.left, ai->ai_rcClient.bottom - ai->ai_rcClient.top, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
				}
			}

			//Dcx::dcxHDCBitmap2Resource hdcbkg(*hdc, (ai->ai_rcWin.right - ai->ai_rcWin.left),(ai->ai_rcWin.bottom - ai->ai_rcWin.top));
			//BitBlt(hdcbkg, ai->ai_rcClient.left, ai->ai_rcClient.top, ai->ai_rcClient.right - ai->ai_rcClient.left, ai->ai_rcClient.bottom - ai->ai_rcClient.top, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
		}
		ai->ai_Oldhdc = *hdc;
		*hdc = ai->ai_hdc;
	}
	return ai.release();
}

GSL_SUPPRESS(type.4)
GSL_SUPPRESS(i.11)
GSL_SUPPRESS(r.11)
void DcxControl::FinishAlphaBlend(LPALPHAINFO ai) noexcept
{
	if (!ai)
		return;
	Auto(delete ai);

	if (ai->ai_Buffer)
	{
		DcxUXModule::dcxEndBufferedPaint(ai->ai_Buffer, TRUE);

		if (ai->ai_BufferAlpha)
			DcxUXModule::dcxEndBufferedPaint(ai->ai_BufferAlpha, TRUE);

		return;
	}
	//// if we can't do Vista method, try do our own
	//if (ai->ai_hdc != nullptr) {
	//	if (ai->ai_bitmap != nullptr) {
	//		int w = (ai->ai_rcClient.right - ai->ai_rcClient.left), h = (ai->ai_rcClient.bottom - ai->ai_rcClient.top);
	//		if (this->IsAlphaBlend()) {
	//			if (ai->ai_bkg != nullptr) {
	//				// create a new HDC for alpha blending. (doing things this way avoids any flicker)
	//				HDC hdcbkg = CreateCompatibleDC( ai->ai_Oldhdc);
	//				if (hdcbkg != nullptr) {
	//					// associate bitmap with hdc
	//					HBITMAP oldBM = (HBITMAP)SelectObject ( hdcbkg, ai->ai_bkg );
	//					// alpha blend finished button with parents background
	//					BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)this->m_iAlphaLevel, 0 }; // 0x7f half of 0xff = 50% transparency
	//					AlphaBlend(hdcbkg,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, w, h,bf);
	//					// draw final image to windows hdc.
	//					BitBlt(ai->ai_Oldhdc,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,hdcbkg,ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
	//
	//					SelectObject( hdcbkg, oldBM);
	//					DeleteDC(hdcbkg);
	//				}
	//				DeleteBitmap(ai->ai_bkg);
	//			}
	//		}
	//		else
	//			BitBlt(ai->ai_Oldhdc,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,ai->ai_hdc,ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
	//
	//		SelectObject (ai->ai_hdc, ai->ai_oldBM );
	//		DeleteObject(ai->ai_bitmap);
	//	}
	//	DeleteDC( ai->ai_hdc );
	//}
	//delete ai;

	// if we can't do Vista method, try do our own
	if (ai->ai_hdcBuffer)
	{
		Auto(DeleteHDCBuffer(ai->ai_hdcBuffer));

		if (const auto w = (ai->ai_rcClient.right - ai->ai_rcClient.left), h = (ai->ai_rcClient.bottom - ai->ai_rcClient.top); this->IsAlphaBlend())
		{
			if (ai->ai_bkg)
			{
				Auto(DeleteObject(ai->ai_bkg));

				// create a new HDC for alpha blending. (doing things this way avoids any flicker)
				if (const auto hdcbkg = CreateCompatibleDC(ai->ai_Oldhdc); hdcbkg)
				{
					Auto(DeleteDC(hdcbkg));

					// associate bitmap with hdc
					const auto oldBM = Dcx::dcxSelectObject<HBITMAP>(hdcbkg, ai->ai_bkg);
					Auto(Dcx::dcxSelectObject<HBITMAP>(hdcbkg, oldBM));

					// alpha blend finished button with parents background
					ai->ai_bf.AlphaFormat = AC_SRC_OVER;
					ai->ai_bf.SourceConstantAlpha = this->m_iAlphaLevel; // 0x7f half of 0xff = 50% transparency
					AlphaBlend(hdcbkg, ai->ai_rcClient.left, ai->ai_rcClient.top, w, h, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, w, h, ai->ai_bf);
					// draw final image to windows hdc.
					BitBlt(ai->ai_Oldhdc, ai->ai_rcClient.left, ai->ai_rcClient.top, w, h, hdcbkg, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
				}
			}
		}
		else
			BitBlt(ai->ai_Oldhdc, ai->ai_rcClient.left, ai->ai_rcClient.top, w, h, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
	}
}

void DcxControl::showError(const TCHAR* const prop, const TCHAR* const cmd, const TCHAR* const err) const
{
	if (!getParentDialog())
		return;

	if (getParentDialog()->IsVerbose())
	{
		TString res;
		if (prop)
			_ts_sprintf(res, TEXT("D_IERROR %(%, %).%: %"), getType(), getParentDialog()->getName(), getUserID(), prop, err);
		else
			_ts_sprintf(res, TEXT("D_CERROR (%) xdid % % %: %"), getType(), cmd, getParentDialog()->getName(), getUserID(), err);
		Dcx::error(cmd, res.to_chr());
	}
	//constexpr TCHAR None[] = TEXT("none");
	//if (!getParentDialog()->getAliasName().empty())
	//	const_cast<DcxControl*>(this)->execAliasEx(TEXT("error,%u,%s,%s,%s,%s"), getUserID(), getType().to_chr(), (prop ? prop : &None[0]), (cmd ? cmd : &None[0]), err);

	constexpr TCHAR None[] = TEXT("none");
	if (!getParentDialog()->getAliasName().empty())
		this->execAliasEx(TEXT("error,%u,%s,%s,%s,%s"), getUserID(), getType().to_chr(), (prop ? prop : &None[0]), (cmd ? cmd : &None[0]), err);
}

LRESULT DcxControl::CommonMessage(const UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	LRESULT lRes = 0L;
	switch (uMsg)
	{
	case WM_HELP:
	{
		if (dcx_testflag(getEventMask(), DCX_EVENT_HELP))
			execAliasEx(TEXT("help,%u"), getUserID());
		bParsed = TRUE;
		lRes = TRUE;
	}
	break;

	case WM_DPICHANGED_AFTERPARENT:
	{
		// win10+ only
		if (dcx_testflag(getEventMask(), DCX_EVENT_THEME))
			execAliasEx(TEXT("dpichanged,%u,afterparent"), getUserID());

		//m_uDPI = Dcx::DpiModule.dcxGetDpiForWindow(m_Hwnd);
		m_uDPI = this->getParentDialog()->getDPI();
	}
	break;

	case WM_DPICHANGED:
	{
		// win8.1+ only
		dcxlParam(LPCRECT, pRc);

		bool bResize{ true };
		if (dcx_testflag(getEventMask(), DCX_EVENT_THEME))
		{
			//stString<MIRC_BUFFER_SIZE_CCH> stRes; // 8k of stack
			//evalAliasEx(stRes, gsl::narrow_cast<int>(stRes.capacity_cch()), TEXT("dpichanged,%u,%d,%d,%d,%d,%d,%u"), getUserID(), Dcx::dcxLOWORD(wParam), pRc->top, pRc->bottom, pRc->left, pRc->right, m_uDPI);
			//bResize = (stRes != TEXT("noresize"));

			TString stRes(gsl::narrow_cast<TString::size_type>(MIRC_BUFFER_SIZE_CCH));
			evalAliasEx(stRes.to_chr(), gsl::narrow_cast<int>(stRes.capacity_cch()), TEXT("dpichanged,%u,%d,%d,%d,%d,%d,%u"), getUserID(), Dcx::dcxLOWORD(wParam), pRc->top, pRc->bottom, pRc->left, pRc->right, m_uDPI);
			bResize = (stRes != TEXT("noresize"));
		}

		if (!IsValidWindow())
			break;

		m_uDPI = Dcx::dcxLOWORD(wParam);

		bParsed = TRUE;

		if (bResize)
			SetWindowPos(m_Hwnd,
				nullptr,
				pRc->left,
				pRc->top,
				pRc->right - pRc->left,
				pRc->bottom - pRc->top,
				SWP_NOZORDER | SWP_NOACTIVATE);
		break;
	}

	case WM_SETCURSOR:
	{
		if ((Dcx::dcxLOWORD(lParam) == HTCLIENT) && (to_hwnd(wParam) == m_Hwnd) && (m_hCursor.cursor))
		{
			if (GetCursor() != m_hCursor.cursor)
				SetCursor(m_hCursor.cursor);
			bParsed = TRUE;
			return TRUE;
		}
	}
	break;

	case WM_MOUSEMOVE:
	{
		getParentDialog()->setMouseControl(getUserID());
	}
	break;

	case WM_SETFOCUS:
	{
		getParentDialog()->setFocusControl(getUserID());
	}
	break;

	case WM_CTLCOLORDLG:
	{
		bParsed = TRUE;
		return reinterpret_cast<LRESULT>(getBackClrBrush());
	}
	break;

	case WM_CTLCOLORBTN:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORSCROLLBAR:
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLOREDIT:
	{
		if (const auto* const p_Control = this->getParentDialog()->getControlByHWND(to_hwnd(lParam)); p_Control)
		{
			const auto clrText = p_Control->getTextColor();
			const auto clrBackText = p_Control->getBackTextColor();
			auto hBackBrush = p_Control->getBackClrBrush();

			bParsed = TRUE;

			//lRes = p_Control->CallDefaultClassProc(uMsg, wParam, lParam);	// error, parent window handles this, NOT the child control
			lRes = CallDefaultClassProc(uMsg, wParam, lParam);

			auto hdc = reinterpret_cast<HDC>(wParam);

			if (!hdc)
				break;

			if (clrText != CLR_INVALID)
				SetTextColor(hdc, clrText);

			if (clrBackText != CLR_INVALID)
				SetBkColor(hdc, clrBackText);

			if (p_Control->isExStyle(WindowExStyle::Transparent))
			{
				// when transparent set as no bkg brush & default transparent drawing.
				SetBkMode(hdc, TRANSPARENT);
				hBackBrush = Dcx::dcxGetStockObject<HBRUSH>(HOLLOW_BRUSH);
			}

			if (hBackBrush)
				lRes = reinterpret_cast<LRESULT>(hBackBrush);
		}
	}
	break;

	case WM_LBUTTONDOWN:
	{
		if (dcx_testflag(this->getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("lbdown,%u"), getUserID());
	}
	break;

	case WM_LBUTTONUP:
	{
		if (dcx_testflag(this->getEventMask(), DCX_EVENT_CLICK))
		{
			execAliasEx(TEXT("lbup,%u"), getUserID());
			execAliasEx(TEXT("sclick,%u"), getUserID());
		}
	}
	break;

	case WM_LBUTTONDBLCLK:
	{
		if (dcx_testflag(this->getEventMask(), DCX_EVENT_CLICK))
		{
			execAliasEx(TEXT("dclick,%u"), getUserID());
			execAliasEx(TEXT("lbdblclk,%u"), getUserID());
		}
	}
	break;

	case WM_RBUTTONDOWN:
	{
		if (dcx_testflag(this->getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("rbdown,%u"), getUserID());
	}
	break;

	case WM_RBUTTONUP:
	{
		if (dcx_testflag(this->getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("rbup,%u"), getUserID());
	}
	break;

	case WM_RBUTTONDBLCLK:
	{
		if (dcx_testflag(this->getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("rdclick,%u"), getUserID());
	}
	break;

	case WM_CONTEXTMENU:
	{
		if (dcx_testflag(this->getEventMask(), DCX_EVENT_CLICK))
		{
			//if (auto hwnd = to_hwnd(wParam); hwnd == m_Hwnd)
			//{
			//	if (execAliasEx(TEXT("rclick,%u"), getUserID()))
			//		lRes = CallDefaultClassProc(uMsg, wParam, lParam);	// this allows the display of local context menus (like copy/paste with edit control)
			//}

			if (execAliasEx(TEXT("rclick,%u"), getUserID()))
				lRes = CallDefaultClassProc(uMsg, wParam, lParam);	// this allows the display of local context menus (like copy/paste with edit control)

			// default menu is NOT displayed when rclick event returns $false
			// this allows the display of custom menus...
		}
		bParsed = TRUE; // stops event being passed down to parent controls
	}
	break;
	case WM_DROPFILES:
	{
		dcxwParam(HDROP, files);

		if (!files)
			break;

		bParsed = TRUE;

		const stString<MIRC_BUFFER_SIZE_CCH> sFilename;

		if (const auto count = DragQueryFile(files, 0xFFFFFFFF, sFilename, gsl::narrow_cast<UINT>(std::size(sFilename))); count > 0)
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_DRAG))
			{
				const stString<20> sRet;

				evalAliasEx(sRet, gsl::narrow_cast<int>(std::size(sRet)), TEXT("dragbegin,%u,%u"), getUserID(), count);

				// cancel drag drop event
				if (sRet == TEXT("cancel"))
				{
					DragFinish(files);
					return 0L;
				}

				// for each file, send callback message
				for (auto i = decltype(count){0}; i < count; ++i)
				{
					if (DragQueryFile(files, i, sFilename, gsl::narrow_cast<UINT>(std::size(sFilename))))
					{
						//execAliasEx(TEXT("dragfile,%u,%s"), getUserID(), sFilename.data());

						if (const auto pd = getParentDialog(); pd)
						{
							TString tsBuf;
							mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_text %"), sFilename.data());
							mIRCLinker::eval(tsBuf, TEXT("$%(%,dragfile,%,%dcx_text)"), pd->getAliasName(), pd->getName(), getUserID());
						}
					}
				}

				execAliasEx(TEXT("dragfinish,%u"), getUserID());
			}
		}

		DragFinish(files);
		break;
	}
	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (!hdr)
			break;

		switch (hdr->code)
		{
		case TTN_GETDISPINFO:
		{
			if (!this->m_tsToolTip.empty())
			{
				dcxlParam(LPNMTTDISPINFO, di);

				if (!di)
					break;

				di->lpszText = this->m_tsToolTip.to_chr();
				di->hinst = nullptr;
				bParsed = TRUE;
			}
		}
		break;
		case TTN_LINKCLICK:
		{
			bParsed = TRUE;
			execAliasEx(TEXT("tooltiplink,%u"), getUserID());
		}
		break;
		default:
			break;
		}
	}
	break;
	// Default WM_PRINTCLIENT method that handles alpha for us.
	// This Message is required for AnimateWindow() to work (also used by new transparency/alpha code)
	case WM_PRINTCLIENT:
	{
		dcxwParam(HDC, hdc);

		bParsed = TRUE;

		// Setup alpha blend if any.
		const auto ai = SetupAlphaBlend(&hdc);
		Auto(FinishAlphaBlend(ai));

		lRes = CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
	}
	break;
	case WM_PRINT:
	{
		if (m_bInPrint) // avoid a drawing loop.
			bParsed = TRUE;
	}
	break;

	// redraw the control if the theme has changed
	case WM_THEMECHANGED:
		redrawWindow();
		break;

	case WM_CLOSE:
	{
		if (GetKeyState(VK_ESCAPE) != 0) // don't allow the window to close if escape is pressed. Needs looking into for a better method.
			bParsed = TRUE;
	}
	break;
	case WM_KEYDOWN:
	{
		if (dcx_testflag(getEventMask(), DCX_EVENT_EDIT))
		{
			if (wParam == VK_RETURN)
				execAliasEx(TEXT("return,%u"), getUserID());

			//if ((this->m_bIgnoreRepeat) && (lParam & 0x40000000)) // ignore repeats
			// break;

			execAliasEx(TEXT("keydown,%u,%zu"), getUserID(), wParam);
		}
		break;
	}
	case WM_KEYUP:
	{
		if (dcx_testflag(getEventMask(), DCX_EVENT_EDIT))
			execAliasEx(TEXT("keyup,%u,%zu"), getUserID(), wParam);
		break;
	}

	//case WM_WINDOWPOSCHANGING:
	//{
	//	if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_MOVE)) {
	//		WINDOWPOS *wp = (WINDOWPOS *) lParam;
//
		//		// break if nomove & nosize specified, since thats all we care about.
		//		if ((dcx_testflag(wp->flags, SWP_NOMOVE)) && (dcx_testflag(wp->flags, SWP_NOSIZE)))
		//			break;
//
		//		TCHAR ret[256];
//
		//		this->callAliasEx(ret, "changing,%d,%d,%d,%d,%d,%d", this->getUserID(), (dcx_testflag(wp->flags, 3)),wp->x, wp->y, wp->cx, wp->cy);
//
		//		if (wp != nullptr) {
		//			if (ts_strcmp(TEXT("nosize"), ret) == 0)
		//				wp->flags |= SWP_NOSIZE;
		//			else if (ts_strcmp(TEXT("nomove"), ret) == 0)
		//				wp->flags |= SWP_NOMOVE;
		//			else if (ts_strcmp(TEXT("nochange"), ret) == 0)
		//				wp->flags |= SWP_NOSIZE | SWP_NOMOVE;
		//		}
		//	}
		//	break;
		//}
	default:
		break;
	}
	return lRes;
}

void DcxControl::HandleChildControlSize()
{
	HandleChildSizing(SizingTypes::ReBar | SizingTypes::Status | SizingTypes::Toolbar);

	if (dcx_testflag(getEventMask(), DCX_EVENT_SIZE))
		execAliasEx(TEXT("sizing,%u"), getUserID());
}

// Invalidate controls area in parent.
void DcxControl::InvalidateParentRect(HWND hwnd) noexcept
{
	if (!hwnd)
		return;

#if DCX_USE_WRAPPERS
	const auto parent = GetParent(hwnd);
	const Dcx::dcxWindowRect rc(hwnd, parent);

	InvalidateRect(parent, &rc, TRUE);
#else
	RECT rc{};
	const auto parent = GetParent(hwnd);

	if (!GetWindowRectParent(hwnd, &rc))
		return;

	InvalidateRect(parent, &rc, TRUE);
#endif
}

void DcxControl::calcTextRect(HDC hdc, const TString& txt, LPRECT rc, const UINT style) const
{
	if (!hdc || !rc)
		return;

	auto t(txt);
	if (!this->IsControlCodeTextDisabled())
		t.strip();
	if (this->IsShadowTextEnabled())
		dcxDrawShadowText(hdc, t.to_wchr(), gsl::narrow_cast<UINT>(t.len()), rc, style | DT_CALCRECT, this->m_TextOptions.m_clrText, this->m_TextOptions.m_clrShadow, this->m_TextOptions.m_uShadowXOffset, this->m_TextOptions.m_uShadowYOffset);
	else
		DrawTextW(hdc, t.to_wchr(), gsl::narrow_cast<int>(t.len()), rc, style | DT_CALCRECT);
}

void DcxControl::ctrlDrawText(HDC hdc, const TString& txt, const LPRECT rc, const UINT style) const
{
	if (!hdc || !rc || txt.empty())
		return;

	//Ook: This version causes issues when control is disabled, text isnt drawn disabled.
	//const auto oldClr = SetTextColor(hdc, m_TextOptions.m_clrText);
	//Auto(SetTextColor(hdc, oldClr));
	//if (this->IsControlCodeTextDisabled())
	//{
	//	const auto oldBkgMode = SetBkMode(hdc, TRANSPARENT);
	//	Auto(SetBkMode(hdc, oldBkgMode));
	//
	//	if (this->IsShadowTextEnabled())
	//		dcxDrawShadowText(hdc, txt.to_chr(), txt.len(), rc, style, m_TextOptions.m_clrText, m_TextOptions.m_clrShadow, 5, 5);
	//	else
	//		DrawText(hdc, txt.to_chr(), gsl::narrow_cast<int>(txt.len()), rc, style);
	//}
	//else
	//	mIRC_DrawText(hdc, txt, rc, style, this->IsShadowTextEnabled());

	dcxTextOptions dTO = this->m_TextOptions;
	dTO.m_bTransparent = dTO.m_bNoCtrlCodes;
	//dTO.m_clrTextBackground = this->getBackTextColor();

	// If ctrl is disabled then no colours or ctrl codes in text, & set colour to grayed.
	if (!IsWindowEnabled(m_Hwnd))
	{
		dTO.m_bNoCtrlCodes = true;
		dTO.m_clrText = GetSysColor(COLOR_GRAYTEXT);
		//dTO.m_clrTextBackground = GetSysColor(COLOR_BACKGROUND);
	}

	if (dTO.m_bGradientFill)
	{
		//dTO.m_bCtrlCodes = false;
		dTO.m_bNoColours = true;
	}
	if (dTO.m_bUseNewStyle)
	{
	dcxDrawTextOptions(hdc, txt.to_wchr(), gsl::narrow_cast<int>(txt.len()), rc, style, dTO);

		//dcxDrawGDIPlusTextOptions(hdc, txt.to_wchr(), gsl::narrow_cast<int>(txt.len()), rc, style, dTO);

	//dcxDrawGradientTextMasked(hdc, txt.to_wchr(), gsl::narrow_cast<int>(txt.len()), rc, style, dTO);

	//dcxDrawGradientText(hdc, txt.to_wchr(), gsl::narrow_cast<int>(txt.len()), rc, style, dTO);
	}
	else {
		const auto oldClr = SetTextColor(hdc, dTO.m_clrText);
		Auto(SetTextColor(hdc, oldClr));

		if (dTO.m_bNoCtrlCodes || dTO.m_bNoColours)
		{
			const auto oldBkgMode = SetBkMode(hdc, TRANSPARENT);
			Auto(SetBkMode(hdc, oldBkgMode));
		 
			if (dTO.m_bGlow)
				dcxDrawShadowText(hdc, txt.to_wchr(), gsl::narrow_cast<UINT>(txt.len()), rc, style, dTO.m_clrText, dTO.m_clrGlow, 0, 0);
			else if (dTO.m_bShadow)
				dcxDrawShadowText(hdc, txt.to_wchr(), gsl::narrow_cast<UINT>(txt.len()), rc, style, dTO.m_clrText, dTO.m_clrShadow, dTO.m_uShadowXOffset, dTO.m_uShadowYOffset);
			else
				DrawTextW(hdc, txt.to_wchr(), gsl::narrow_cast<int>(txt.len()), rc, style);
		}
		else
			mIRC_DrawText(hdc, txt, rc, style, dTO.m_bShadow);
	}

	//if (this->IsControlCodeTextDisabled())
	//{
	//	const auto oldBkgMode = SetBkMode(hdc, TRANSPARENT);
	//	Auto(SetBkMode(hdc, oldBkgMode));
	// 
	//	if (this->IsShadowTextEnabled())
	//		dcxDrawShadowText(hdc, txt.to_wchr(), gsl::narrow_cast<UINT>(txt.len()), rc, style, GetTextColor(hdc), dTO.m_clrShadow, 5, 5);
	//	else
	//		DrawTextW(hdc, txt.to_wchr(), gsl::narrow_cast<int>(txt.len()), rc, style);
	//}
	//else
	//	mIRC_DrawText(hdc, txt, rc, style, this->IsShadowTextEnabled());
	//}
	//if (this->IsControlCodeTextDisabled())
	//{
	//	const auto oldBkgMode = SetBkMode(hdc, TRANSPARENT);
	//	Auto(SetBkMode(hdc, oldBkgMode));
	//
	//	if (this->IsShadowTextEnabled())
	//		dcxDrawShadowText(hdc, txt.to_wchr(), gsl::narrow_cast<UINT>(txt.len()), rc, style, GetTextColor(hdc), 0, 5, 5);
	//	else
	//		DrawTextW(hdc, txt.to_wchr(), gsl::narrow_cast<int>(txt.len()), rc, style);
	//}
	//else
	//	mIRC_DrawText(hdc, txt, rc, style, this->IsShadowTextEnabled());
}

void DcxControl::loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc)
{
	// place holder only, does nothing...
}

const TString DcxControl::getStyles(void) const
{
	TString result;
	if (!m_Hwnd)
		return result;

	const auto exStyles = dcxGetWindowExStyle(m_Hwnd);
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (!this->IsThemed())
		result = TEXT("notheme");
	if (dcx_testflag(Styles, WS_TABSTOP))
		result.addtok(TEXT("tabstop"));
	if (dcx_testflag(Styles, WS_GROUP))
		result.addtok(TEXT("group"));
	if (dcx_testflag(Styles, WS_DISABLED))
		result.addtok(TEXT("disabled"));
	if (dcx_testflag(exStyles, WS_EX_TRANSPARENT))
		result.addtok(TEXT("transparent"));
	if (!dcx_testflag(Styles, WS_VISIBLE))
		result.addtok(TEXT("hidden"));
	if (this->IsAlphaBlend())
		result.addtok(TEXT("alpha"));
	if (this->IsShadowTextEnabled())
		result.addtok(TEXT("shadow"));
	if (this->IsControlCodeTextDisabled())
		result.addtok(TEXT("noformat"));
	if (this->IsGradientFillEnabled())
	{
		if (this->IsGradientFillVertical())
			result.addtok(TEXT("vgradient"));
		else
			result.addtok(TEXT("hgradient"));
	}
	if (this->getToolTipHWND())
		result.addtok(TEXT("tooltips"));
	return result;
}

void DcxControl::toXml(TiXmlElement* const xml) const
{
	if (!xml)
		return;

	const auto styles(getStyles());
	if (const auto pd = this->getParentDialog(); pd)
	{
		if (const auto tsID(pd->IDToName(getID())); !tsID.empty())
			xml->SetAttribute("id", tsID.c_str());
		else
			xml->SetAttribute("id", gsl::narrow_cast<int>(getUserID()));
	}
	else
		xml->SetAttribute("id", gsl::narrow_cast<int>(getUserID()));
	xml->SetAttribute("type", getType().c_str());
	if (!styles.empty())
		xml->SetAttribute("styles", styles.c_str());
	xml->SetAttribute("border", getBorderStyles().c_str());
	if (this->m_clrBackground != CLR_INVALID)
		setColourAttribute(xml, "bgcolour", this->m_clrBackground);
	if (this->m_hBorderBrush)
		setColourAttribute(xml, "bordercolour", Dcx::BrushToColour(this->m_hBorderBrush));
	if (this->m_TextOptions.m_clrText != CLR_INVALID)
		setColourAttribute(xml, "textcolour", this->m_TextOptions.m_clrText);
	if (this->m_TextOptions.m_clrTextBackground != CLR_INVALID)
		setColourAttribute(xml, "textbgcolour", this->m_TextOptions.m_clrTextBackground);
	if (this->m_clrStartGradient != CLR_INVALID)
		setColourAttribute(xml, "gradientstart", this->m_clrStartGradient);
	if (this->m_clrEndGradient != CLR_INVALID)
		setColourAttribute(xml, "gradientend", this->m_clrEndGradient);
	if (this->m_colTransparentBg != CLR_INVALID)
		setColourAttribute(xml, "transparentbg", this->m_colTransparentBg);
	if (!this->m_tsMark.empty())
		xml->SetAttribute("mark", m_tsMark.c_str());
	if (!this->m_tsToolTip.empty())
		xml->SetAttribute("tooltip", m_tsToolTip.c_str());
	if (auto hTip = getToolTipHWND(); hTip)
	{
		if (const auto tmp = Dcx::dcxToolTip_GetTipBkColor(hTip); tmp != CLR_INVALID)
			setColourAttribute(xml, "tooltipbgcolour", tmp);
		if (const auto tmp = Dcx::dcxToolTip_GetTipTextColor(hTip); tmp != CLR_INVALID)
			setColourAttribute(xml, "tooltiptextcolour", tmp);
	}
	xml->SetAttribute("eventmask", this->m_dEventMask);
	if (m_bAlphaBlend && (this->m_iAlphaLevel < 255U))
		xml->SetAttribute("alpha", gsl::narrow_cast<int>(this->m_iAlphaLevel));
	if (const auto tsFont = FontToCommand(); !tsFont.empty())
		xml->SetAttribute("font", tsFont.c_str());
	//cursors
	if (this->m_hCursor.cursor)
	{
		TiXmlElement xCursor("cursor");

		xCursor.SetAttribute("src", (this->m_hCursor.src.empty() ? "arrow" : this->m_hCursor.src.c_str()));
		xCursor.SetAttribute("flags", (this->m_hCursor.flags.empty() ? "+r" : this->m_hCursor.flags.c_str()));

		xml->InsertEndChild(xCursor);
	}

	{
		TiXmlElement xTextOptions("textoptions");

		xTextOptions.SetAttribute("noformat", m_TextOptions.m_bNoCtrlCodes);
		xTextOptions.SetAttribute("dbloutline", m_TextOptions.m_bDoubleOutline);
		xTextOptions.SetAttribute("filloutline", m_TextOptions.m_bFilledOutline);
		if (m_TextOptions.m_bGradientFill)
		{
			if (m_TextOptions.m_bHorizGradientFill)
				xTextOptions.SetAttribute("gtext", "Horizontal");
			else
				xTextOptions.SetAttribute("gtext", "Vertical");
}
		if (m_TextOptions.m_bGradientOutline)
		{
			if (m_TextOptions.m_bHorizGradientOutline)
				xTextOptions.SetAttribute("goutline", "Horizontal");
			else
				xTextOptions.SetAttribute("goutline", "Vertical");
		}
		xTextOptions.SetAttribute("nocolours", m_TextOptions.m_bNoColours);
		xTextOptions.SetAttribute("outlineonly", m_TextOptions.m_bOutline);
		xTextOptions.SetAttribute("transparent", m_TextOptions.m_bTransparent);

		{
			TiXmlElement xItem("shadows");

			xItem.SetAttribute("enable", m_TextOptions.m_bShadow);
			xItem.SetAttribute("alpha", m_TextOptions.m_uShadowAlpha);
			xItem.SetAttribute("size", m_TextOptions.m_uShadowThickness);
			xItem.SetAttribute("xoffset", m_TextOptions.m_uShadowXOffset);
			xItem.SetAttribute("yoffset", m_TextOptions.m_uShadowYOffset);

			xTextOptions.InsertEndChild(xItem);
		}

		{
			TiXmlElement xItem("glow");

			xItem.SetAttribute("enable", m_TextOptions.m_bGlow);
			xItem.SetAttribute("size", m_TextOptions.m_uGlowSize);

			xTextOptions.InsertEndChild(xItem);
		}

		{
			TiXmlElement xTextColours("colours");

			setColourAttribute(&xTextColours, "glow", m_TextOptions.m_clrGlow);
			setColourAttribute(&xTextColours, "goutlinestart", m_TextOptions.m_clrGradientOutlineStart);
			setColourAttribute(&xTextColours, "goutlineend", m_TextOptions.m_clrGradientOutlineEnd);
			setColourAttribute(&xTextColours, "gtextstart", m_TextOptions.m_clrGradientOutlineStart);
			setColourAttribute(&xTextColours, "gtextend", m_TextOptions.m_clrGradientOutlineEnd);
			setColourAttribute(&xTextColours, "outline", m_TextOptions.m_clrOutline);
			setColourAttribute(&xTextColours, "shadow", m_TextOptions.m_clrShadow);
			setColourAttribute(&xTextColours, "text", m_TextOptions.m_clrText);
			setColourAttribute(&xTextColours, "textbkg", m_TextOptions.m_clrTextBackground);

			xTextOptions.InsertEndChild(xTextColours);
		}
		xml->InsertEndChild(xTextOptions);
	}
}

TiXmlElement* DcxControl::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxControl::xmlParseElements(const TString& tsPath, const TiXmlElement* xParent, const TiXmlElement* xTemplate)
{
	if (!xParent || tsPath.empty())
		return;

	const auto pd = getParentDialog();
	if (!pd)
		return;

	TString tsCurrentPath(tsPath);

	int iCLA{ 1 };
	const TiXmlElement* xBase{ xParent };
	if (xTemplate)
		xBase = xTemplate;

	// parse all child elements
	for (auto xElement = xBase->FirstChildElement(); xElement; xElement = xElement->NextSiblingElement())
	{
		switch (std::hash<const char*>()(xElement->Value()))
		{
		case "pane"_hash:
		{
			// its a pane
			TString tsTok;
			tsTok.addtok(iCLA);
			tsCurrentPath.puttok(tsTok, tsCurrentPath.numtok());

			xmlAddPane(tsPath, tsCurrentPath, xElement, xTemplate);

			++iCLA;
		}
		break;
		case "control"_hash:
		{
			xmlAddControl(tsPath, tsCurrentPath, xParent, xElement);
		}
		break;
		case "calltemplate"_hash:
		{
			xmlCallTemplate(tsCurrentPath, xParent, xElement);
		}
		break;
		default:
			break;
		}
	}
}

void DcxControl::xmlAddPane(const TString& tsParentPath, const TString& tsCurrentPath, const TiXmlElement* xElement, const TiXmlElement* xTemplate)
{
	if (!xElement || tsParentPath.empty())
		return;

	const auto pd = getParentDialog();
	if (!pd)
		return;

	// cascade margin weight
	auto szCascade = queryAttribute(xElement, "cascade");
	auto szMargin = queryAttribute(xElement, "margin", "0 0 0 0");
	const auto iWidth = queryIntAttribute(xElement, "width");
	const auto iHeight = queryIntAttribute(xElement, "height");
	const auto iWeight = queryIntAttribute(xElement, "weight", 1);

	// xdid -l dname id root $chr(9) +p id weight w h
	// name switch options
	TString tsInput;
	_ts_sprintf(tsInput, L"% % -l cell % \t +p% 0 % % %", pd->getName(), this->getUserID(), tsParentPath, szCascade, iWeight, iWidth, iHeight);
	this->parseCommandRequest(tsInput);
	_ts_sprintf(tsInput, L"% % -l space % \t + %", pd->getName(), this->getUserID(), tsCurrentPath, szMargin);
	this->parseCommandRequest(tsInput);

	xmlParseElements(tsCurrentPath, xElement, xTemplate);
}

bool DcxControl::xmlAddControl(const TString& tsParentPath, const TString& tsCurrentPath, const TiXmlElement* xParent, const TiXmlElement* xCtrl)
{
	if (!xParent || !xCtrl || tsParentPath.empty())
		return false;
	const auto pd = getParentDialog();
	if (!pd)
		return false;

	auto szX = queryAttribute(xCtrl, "x", "0");
	auto szY = queryAttribute(xCtrl, "y", "0");
	const auto iWidth = queryIntAttribute(xCtrl, "width");
	const auto iHeight = queryIntAttribute(xCtrl, "height");
	TString tsID(queryAttribute(xCtrl, "id"));
	auto szType = queryAttribute(xCtrl, "type");
	auto szStyles = queryAttribute(xCtrl, "styles");

	// ID is NOT a number!
	if (tsID.empty()) // no id, generate one.
		tsID.addtok(pd->getUniqueID());

	// fixed position control, no cla
	// xdid -c dname id [newid] [type] [x] [y] [width] [height] [styles...]
	TString tsInput;
	_ts_sprintf(tsInput, TEXT("% % % % % % %"), tsID, szType, szX, szY, iWidth, iHeight, szStyles);
	if (auto ctrl = pd->addControl(tsInput, 1, DcxAllowControls::ALLOW_ALL, m_Hwnd); ctrl)
	{
		ctrl->fromXml(xParent, xCtrl);

		// x & y makes this a fixed control, not cla
		if (!xCtrl->Attribute("x") && !xCtrl->Attribute("y") && !xParent->Attribute("nocla"))
		{
			// assume its cla now.
			const auto iWeight = queryIntAttribute(xCtrl, "weight", 1);
			TString tsFlags("i"); // id included
			if (xCtrl->Attribute("width"))
			{
				tsFlags += L'f'; // fixed size
				if (xCtrl->Attribute("height"))
					tsFlags += L'w'; // both
				else
					tsFlags += L'h'; // horizontal

			}
			else if (xCtrl->Attribute("height"))
				tsFlags += L"fv"; // fixed vertical
			else
				tsFlags += L'l'; // fill

			{
				_ts_sprintf(tsInput, L"% % -l cell % \t +% % % % %", pd->getName(), this->getUserID(), tsParentPath, tsFlags, tsID, iWeight, iWidth, iHeight);
				this->parseCommandRequest(tsInput);
				return true;
			}
		}
	}
	return false;
}

void DcxControl::xmlCallTemplate(const TString& tsCurrentPath, const TiXmlElement* xParent, const TiXmlElement* xCallTemplate)
{
	const auto pd = getParentDialog();
	if (!pd)
		return;

	auto& xmlTemplates = pd->xmlGetTemplates();

	if (!xCallTemplate || xmlTemplates.empty())
		return;

	const TString tsName(queryAttribute(xCallTemplate, "name"));

	const auto itEnd = xmlTemplates.cend();
	const auto it = std::find_if(xmlTemplates.cbegin(), itEnd, [tsName](const auto& a) noexcept {
		return (a.tsName == tsName);
		});

	if (it != itEnd)
		xmlParseElements(tsCurrentPath, xParent, it->xTemplate);

	// should we throw an error when no match?
}

void DcxControl::xmlSetStyle(const TiXmlElement* xStyle)
{
	if (!xStyle)
		return;

	// font & colours...
	{
		TString tsFont;

		if (auto tmp = queryAttribute(xStyle, "font"); !_ts_isEmpty(tmp))
		{
			// single font item
			// + charset size fontname
			tsFont = tmp;
		}
		else {
			// split font parts
			// fontstyle charset fontsize fontname
			auto fontsize = queryAttribute(xStyle, "fontsize", "8");
			auto charset = queryAttribute(xStyle, "chatset", "ansi");
			auto fontstyle = queryAttribute(xStyle, "fontstyle", "d");
			auto fontname = queryAttribute(xStyle, "fontname");

			// fontname is the only one without a default & is required.
			GSL_SUPPRESS(lifetime.1) if (!_ts_isEmpty(fontname))
				_ts_sprintf(tsFont, "+% % % %", fontstyle, charset, fontsize, fontname);
		}
		if (!tsFont.empty())
		{
			if (LOGFONT lf{ }; ParseCommandToLogfont(tsFont, &lf))
				setControlFont(CreateFontIndirect(&lf), FALSE);
		}
	}

	if (const auto clr = queryColourAttribute(xStyle, "bgcolour"); clr != CLR_INVALID)
	{
		this->m_clrBackground = clr;
		if (this->m_hBackBrush)
			DeleteObject(this->m_hBackBrush);
		this->m_hBackBrush = CreateSolidBrush(clr);
	}
	if (const auto clr = queryColourAttribute(xStyle, "bordercolour"); clr != CLR_INVALID)
	{
		if (this->m_hBorderBrush)
			DeleteObject(this->m_hBorderBrush);
		this->m_hBorderBrush = CreateSolidBrush(clr);
	}
	if (const auto clr = queryColourAttribute(xStyle, "textcolour"); clr != CLR_INVALID)
		this->m_TextOptions.m_clrText = clr;
	if (const auto clr = queryColourAttribute(xStyle, "textbgcolour"); clr != CLR_INVALID)
		this->m_TextOptions.m_clrTextBackground = clr;
	if (const auto clr = queryColourAttribute(xStyle, "gradientstart"); clr != CLR_INVALID)
		this->m_clrStartGradient = clr;
	if (const auto clr = queryColourAttribute(xStyle, "gradientend"); clr != CLR_INVALID)
		this->m_clrEndGradient = clr;
	if (const auto clr = queryColourAttribute(xStyle, "transparentbg"); clr != CLR_INVALID)
		this->m_colTransparentBg = clr;
	if (auto hTip = getToolTipHWND(); hTip)
	{
		if (const auto clr = queryColourAttribute(xStyle, "tooltipbgcolour"); clr != CLR_INVALID)
			Dcx::dcxToolTip_SetTipBkColor(hTip, clr);
		if (const auto clr = queryColourAttribute(xStyle, "tooltiptextcolour"); clr != CLR_INVALID)
			Dcx::dcxToolTip_SetTipTextColor(hTip, clr);
	}
	if (auto xTextOptions = xStyle->FirstChildElement("textoptions"); xTextOptions)
	{
		m_TextOptions.m_bNoCtrlCodes = (xTextOptions->Attribute("noformat") != nullptr);
		m_TextOptions.m_bDoubleOutline = (xTextOptions->Attribute("dbloutline") != nullptr);
		m_TextOptions.m_bFilledOutline = (xTextOptions->Attribute("filledoutline") != nullptr);
		m_TextOptions.m_bGlow = (xTextOptions->Attribute("glow") != nullptr);
		if (const TString tmp(queryAttribute(xTextOptions, "gtext")); !_ts_isEmpty(tmp))
		{
			m_TextOptions.m_bHorizGradientFill = (tmp == L"Horizontal");
			m_TextOptions.m_bGradientFill = true;
		}
		if (const TString tmp(queryAttribute(xTextOptions, "goutline")); !_ts_isEmpty(tmp))
		{
			m_TextOptions.m_bHorizGradientOutline = (tmp == L"Horizontal");
			m_TextOptions.m_bGradientOutline = true;
		}
		m_TextOptions.m_bNoColours = (queryIntAttribute(xTextOptions, "nocolours") == 0);
		m_TextOptions.m_bOutline = (queryIntAttribute(xTextOptions, "outlineonly") == 0);
		m_TextOptions.m_bTransparent = (queryIntAttribute(xTextOptions, "transparent") == 0);

		if (auto xItem = dynamic_cast<const TiXmlElement*>(xTextOptions->FirstChild("shadows")); xItem)
		{
			m_TextOptions.m_bShadow = (queryIntAttribute(xItem, "enable") == 0);

			m_TextOptions.m_uShadowAlpha = gsl::narrow_cast<BYTE>(queryIntAttribute(xItem, "alpha", m_TextOptions.m_uShadowAlpha));
			m_TextOptions.m_uShadowThickness = gsl::narrow_cast<BYTE>(queryIntAttribute(xItem, "size", m_TextOptions.m_uShadowThickness));
			m_TextOptions.m_uShadowXOffset = gsl::narrow_cast<BYTE>(queryIntAttribute(xItem, "xoffset", m_TextOptions.m_uShadowXOffset));
			m_TextOptions.m_uShadowYOffset = gsl::narrow_cast<BYTE>(queryIntAttribute(xItem, "yoffset", m_TextOptions.m_uShadowYOffset));
		}

		if (auto xItem = dynamic_cast<const TiXmlElement*>(xTextOptions->FirstChild("glow")); xItem)
		{
			m_TextOptions.m_bGlow = (queryIntAttribute(xItem, "enable") == 0);

			m_TextOptions.m_uGlowSize = gsl::narrow_cast<BYTE>(queryIntAttribute(xItem, "size", m_TextOptions.m_uGlowSize));
		}

		if (auto xColours = dynamic_cast<const TiXmlElement*>(xTextOptions->FirstChild("colours")); xColours)
		{
			m_TextOptions.m_clrGlow = queryColourAttribute(xColours, "glow", m_TextOptions.m_clrGlow);
			m_TextOptions.m_clrGradientOutlineEnd = queryColourAttribute(xColours, "goutlineend", m_TextOptions.m_clrGradientOutlineEnd);
			m_TextOptions.m_clrGradientOutlineStart = queryColourAttribute(xColours, "goutlinestart", m_TextOptions.m_clrGradientOutlineStart);
			m_TextOptions.m_clrGradientTextEnd = queryColourAttribute(xColours, "gtextend", m_TextOptions.m_clrGradientTextEnd);
			m_TextOptions.m_clrGradientTextStart = queryColourAttribute(xColours, "gtextstart", m_TextOptions.m_clrGradientTextStart);
			m_TextOptions.m_clrOutline = queryColourAttribute(xColours, "outline", m_TextOptions.m_clrOutline);
			m_TextOptions.m_clrShadow = queryColourAttribute(xColours, "shadow", m_TextOptions.m_clrShadow);
			// these two may have been set by old option above.
			m_TextOptions.m_clrText = queryColourAttribute(xColours, "text", m_TextOptions.m_clrText);
			m_TextOptions.m_clrTextBackground = queryColourAttribute(xColours, "textbkg", m_TextOptions.m_clrTextBackground);
		}
	}
}

void DcxControl::xmlSetStyles()
{
	// set style...
	const auto pd = this->getParentDialog();
	if (!pd)
		return;

	for (const auto& a : pd->xmlGetStyles())
	{
		if (a.tsType.empty() && a.tsClass.empty() && a.tsID.empty()) // an <all> tag
			xmlSetStyle(a.xStyle);
		else if (a.tsType == this->getType())
			xmlSetStyle(a.xStyle);
		else if (const TString tmpid(queryAttribute(a.xStyle, "id")); a.tsID == tmpid)
			xmlSetStyle(a.xStyle);
		else if (const TString tmp(queryAttribute(a.xStyle, "class")); a.tsClass == tmp)
			xmlSetStyle(a.xStyle);
	}
}

void DcxControl::xmlLoadIcons(const TiXmlElement* xThis)
{
	const auto pd = this->getParentDialog();
	if (!pd)
		return;

	for (const auto& a : pd->xmlGetIcons())
	{
		bool bGood = false;
		// if type matches, use the icon.
		if (a.tsType == this->getType())
			bGood = true;
		// if id matches, load the icon, id can be id or id,id,id...
		if (!a.tsID.empty())
		{
			const auto HandleIDRange = [this, pd](const TString& tsID) {
				UINT id_start = 0, id_end = 0;
				if (tsID.numtok(TEXT('-')) == 2)
				{
					id_start = tsID.getfirsttok(1, TEXT('-')).to_<UINT>();
					id_end = tsID.getnexttok(TEXT('-')).to_<UINT>();
				}
				else
					id_start = id_end = pd->NameToUserID(tsID);

				if ((id_end < id_start) || (id_start == 0) || (id_end == 0))
					return false;

				return (id_start <= this->getUserID()) && (id_end >= this->getUserID());
			};

			if (const auto nTok = a.tsID.numtok(TSCOMMACHAR); nTok > 1)
			{
				// multiple id ranges...
				const auto itEnd = a.tsID.end();
				for (auto itStart = a.tsID.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
				{
					bGood = HandleIDRange(*itStart);
				}
			}
			else {
				// single id range
				bGood = HandleIDRange(a.tsID);
			}
		}
		// if class matches, load the icon
		if (!a.tsClass.empty() && (a.tsClass == queryAttribute(xThis, "class")))
			bGood = true;
		if (bGood)
		{
			const TString tsFlags(queryAttribute(a.xIcon, "flags", "+"));
			TString tsSrc(queryEvalAttribute(a.xIcon, "src"));
			TString tsIndex(queryAttribute(a.xIcon, "index"));
			if (tsIndex.empty())
			{
				// no index attribute, so look for indexmin & indexmax (both default to zero)
				const auto MinIndex(queryIntAttribute(a.xIcon, "indexmin"));
				const auto MaxIndex(queryIntAttribute(a.xIcon, "indexmax"));
				if (MinIndex == 0 && MaxIndex == 0)
					tsIndex = L"0";
				else
					_ts_sprintf(tsIndex, L"%-%", MinIndex, MaxIndex);
			}
			if (!tsFlags.empty() && !tsIndex.empty() && !tsSrc.empty())
				loadIcon(tsFlags, tsIndex, tsSrc);
		}
	}
}

void DcxControl::xmlSaveImageList(HIMAGELIST himl, TiXmlElement* xml, const TString& tsFlags) const
{
	if (!himl || !xml || tsFlags.empty())
		return;

	const auto pd = this->getParentDialog();
	if (!pd)
		return;

	//pd->xmlSaveImageList(himl, xml, tsFlags);

	if (const auto cnt = ImageList_GetImageCount(himl); cnt > 0)
	{
		if (!xml->Attribute("iconsize"))
		{
			if (int cx{}, cy{}; ImageList_GetIconSize(himl, &cx, &cy))
			{
				xml->SetAttribute("iconsize", cx);
			}
		}
	
		xmlIcon xIcon;
	
		//xIcon.tsType = this->getType();
		xIcon.tsID = pd->IDToName(this->getID());
		xIcon.tsFlags = tsFlags;
	
		for (int i{}; i < cnt; ++i)
		{
			if (auto hIcon = ImageList_GetIcon(himl, i, ILD_TRANSPARENT); hIcon)
			{
				Auto(DestroyIcon(hIcon));
	
				xIcon.tsSrc = IconToBase64(hIcon);
	
				pd->xmlGetIcons().emplace_back(xIcon);
			}
		}
	}
}

// Convert a number into the closest icon size
DcxIconSizes DcxControl::NumToIconSize(const int& num) noexcept
{
	// if size is > max allowed, return max
	if (num >= gsl::narrow_cast<int>(DcxIconSizes::MaxSize))
		return DcxIconSizes::MaxSize;
	// we now know that size is < max allowed, so if its > smallest size, return medium
	if (num > gsl::narrow_cast<int>(DcxIconSizes::SmallIcon))
		return DcxIconSizes::MediumIcon;
	// otherwise return small
	return DcxIconSizes::SmallIcon;
}

void DcxControl::InitializeDcxControls()
{
	if (m_bInitialized)
		return;

	// Patch ScrollWindowEx() function for List/TreeView custom backgrounds...
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Patch ScrollWindowEx..."));
	ScrollWindowExUx = static_cast<decltype(::ScrollWindowEx)*>(Dcx::PatchAPI("User32.dll", "ScrollWindowEx", DcxControl::XScrollWindowEx));

	// Custom ProgressBar
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering ProgressBar..."));
	dcxRegisterClass<DcxProgressBar>(PROGRESS_CLASS, DCX_PROGRESSBARCLASS);

	// Custom TreeView
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering TreeView..."));
	dcxRegisterClass<DcxTreeView>(WC_TREEVIEW, DCX_TREEVIEWCLASS);

	// Custom Toolbar
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering ToolBar..."));
	dcxRegisterClass<DcxToolBar>(TOOLBARCLASSNAME, DCX_TOOLBARCLASS);

	// Custom StatusBar
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering StatusBar..."));
	dcxRegisterClass<DcxStatusBar>(STATUSCLASSNAME, DCX_STATUSBARCLASS);

	// Custom ListView
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Listview..."));
	dcxRegisterClass<DcxListView>(WC_LISTVIEW, DCX_LISTVIEWCLASS);

	// Custom ComboEx
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering ComboEx..."));
	dcxRegisterClass<DcxComboEx>(WC_COMBOBOXEX, DCX_COMBOEXCLASS);

	// Custom TrackBar
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering TrackBar..."));
	dcxRegisterClass<DcxTrackBar>(TRACKBAR_CLASS, DCX_TRACKBARCLASS);

	// Custom RichEdit
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering RichEdit..."));
	dcxRegisterClass<DcxRichEdit>(RICHEDIT_CLASS, DCX_RICHEDITCLASS);

	// Custom RebarCtrl
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering ReBar..."));
	dcxRegisterClass<DcxReBar>(REBARCLASSNAME, DCX_REBARCTRLCLASS);

	// Custom Color Combo
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering ColorCombo..."));
	dcxRegisterClass<DcxColorCombo>(WC_COMBOBOX, DCX_COLORCOMBOCLASS);

	// Custom TabCtrl
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Tab..."));
	dcxRegisterClass<DcxTab>(WC_TABCONTROL, DCX_TABCTRLCLASS);

	// Custom UpDown
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering UpDown..."));
	dcxRegisterClass<DcxUpDown>(UPDOWN_CLASS, DCX_UPDOWNCLASS);

	// Custom IpAddress
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering IpAddress..."));
	dcxRegisterClass<DcxIpAddress>(WC_IPADDRESS, DCX_IPADDRESSCLASS);

	// Custom Button
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Button..."));
	dcxRegisterClass<DcxButton>(WC_BUTTON, DCX_BUTTONCLASS);

	// Custom Check
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Check..."));
	dcxRegisterClass<DcxCheck>(WC_BUTTON, DCX_CHECKCLASS);

	// Custom Radio
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Radio..."));
	dcxRegisterClass<DcxRadio>(WC_BUTTON, DCX_RADIOCLASS);

	// Custom Calendar
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Calendar..."));
	dcxRegisterClass<DcxCalendar>(MONTHCAL_CLASS, DCX_CALENDARCLASS);

	// Custom DateTime
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering DateTime..."));
	dcxRegisterClass<DcxDateTime>(DATETIMEPICK_CLASS, DCX_DATETIMECLASS);

	// Custom Pager
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Pager..."));
	dcxRegisterClass<DcxPager>(WC_PAGESCROLLER, DCX_PAGERCLASS);

	// Custom Image
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Image..."));
	dcxRegisterClass<DcxImage>(WC_STATIC, DCX_IMAGECLASS);

	// Custom Edit
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Edit..."));
	dcxRegisterClass<DcxEdit>(WC_EDIT, DCX_EDITCLASS);

	{
		const auto hHandle = GetModuleHandle(nullptr);

		// Custom Divider
		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Divider..."));

		{
			const WNDCLASSEX wc{ sizeof(WNDCLASSEX),0, DcxControl::WindowProc, 0, 0, hHandle, nullptr, nullptr, reinterpret_cast<HBRUSH>(COLOR_3DFACE + 1), nullptr, DCX_DIVIDERCLASS, nullptr };
			DcxDivider::m_hDefaultClassProc = DividerWndProc;
			RegisterClassEx(&wc);
		}

		// Custom MultiCombo
		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering MultiCombo..."));

		{
			const WNDCLASSEX wc{ sizeof(WNDCLASSEX), CS_PARENTDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, DcxControl::WindowProc, 0, 0, hHandle, nullptr, LoadCursor(hHandle, IDC_ARROW), nullptr, nullptr, DCX_MULTICOMBOCLASS, nullptr };
			DcxMultiCombo::m_hDefaultClassProc = MultiComboWndProc;
			RegisterClassEx(&wc);
		}

		// Custom Panel
		{
			DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Panel..."));

			const WNDCLASSEX wc{ sizeof(WNDCLASSEX), CS_PARENTDC, DcxControl::WindowProc, 0, 0, hHandle, nullptr, LoadCursor(hHandle, IDC_ARROW), nullptr, nullptr, DCX_PANELCLASS, nullptr };
			DcxPanel::m_hDefaultClassProc = DefWindowProc;
			RegisterClassEx(&wc);
		}

		// Custom Box
		{
			DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Box..."));
			const WNDCLASSEX wc{ sizeof(WNDCLASSEX), CS_PARENTDC, DcxControl::WindowProc, 0, 0, hHandle, nullptr, LoadCursor(hHandle, IDC_ARROW), nullptr, nullptr, DCX_BOXCLASS, nullptr };
			DcxBox::m_hDefaultClassProc = DefWindowProc;
			RegisterClassEx(&wc);
		}
	}

	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Web..."));
	dcxRegisterClass<DcxWebControl>(WC_STATIC, DCX_WEBCLASS);

	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Line..."));
	dcxRegisterClass<DcxLine>(WC_STATIC, DCX_LINECLASS);

	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Link..."));
	dcxRegisterClass<DcxLink>(WC_STATIC, DCX_LINKCLASS);
	//dcxRegisterClass<DcxLink>(WC_LINK, DCX_LINKCLASS);

	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering List..."));
	dcxRegisterClass<DcxList>(WC_LISTBOX, DCX_LISTCLASS);

	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Scroll..."));
	dcxRegisterClass<DcxScroll>(WC_SCROLLBAR, DCX_SCROLLBARCLASS);

	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Text..."));
	dcxRegisterClass<DcxText>(WC_STATIC, DCX_TEXTCLASS);

	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering DirectShow..."));
	dcxRegisterClass<DcxDirectshow>(WC_STATIC, DCX_DIRECTSHOWCLASS);

	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Stacker..."));
	dcxRegisterClass<DcxStacker>(WC_LISTBOX, DCX_STACKERCLASS);

	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Web2..."));
	dcxRegisterClass<DcxWebControl2>(WC_STATIC, DCX_WEB2CLASS);

	m_bInitialized = true;

	return;
}

void DcxControl::UnInitializeDcxControls() noexcept
{
	m_bInitialized = false;

	UnregisterClass(DCX_PROGRESSBARCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_TREEVIEWCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_TOOLBARCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_STATUSBARCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_LISTVIEWCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_COMBOEXCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_TRACKBARCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_RICHEDITCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_REBARCTRLCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_COLORCOMBOCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_TABCTRLCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_UPDOWNCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_IPADDRESSCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_BUTTONCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_CHECKCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_RADIOCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_CALENDARCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_DATETIMECLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_PAGERCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_IMAGECLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_EDITCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_DIVIDERCLASS, GetModuleHandle(nullptr));
	UnregisterClass(MCOMBO_DROPCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_MULTICOMBOCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_PANELCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_BOXCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_WEBCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_WEB2CLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_LINECLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_LINKCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_LISTCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_SCROLLBARCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_TEXTCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_DIRECTSHOWCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_STACKERCLASS, GetModuleHandle(nullptr));
	//UnregisterClass(DCX_GRIDCLASS, GetModuleHandle(nullptr));
	//UnregisterClass(DCX_MULTIBUTTONCLASS, GetModuleHandle(nullptr));

	if (DcxWindow::m_hZeroRgn)
	{
		DeleteRgn(DcxWindow::m_hZeroRgn);
		DcxWindow::m_hZeroRgn = nullptr;
	}

	Dcx::RemovePatch(DcxControl::ScrollWindowExUx, DcxControl::XScrollWindowEx);
}

// patch function to allow buffered scrolling for list & treeview controls.
int WINAPI DcxControl::XScrollWindowEx(_In_ HWND hWnd, _In_ int dx, _In_ int dy, _In_ const RECT* prcScroll, _In_ const RECT* prcClip, _In_ HRGN hrgnUpdate, _Out_ LPRECT prcUpdate, _In_ UINT flags) noexcept
{
	if (!ScrollWindowExUx)
		return ERROR;

	if (const auto pthis = Dcx::dcxGetProp<DcxControl*>(hWnd, TEXT("dcx_cthis")); pthis)
	{
		switch (pthis->getGeneralControlType())
		{
		default:
			break;
		case DcxControlTypes::LIST:
		{
			if (pthis->isExStyle(WindowExStyle::Transparent) || pthis->IsAlphaBlend())
			{
				InvalidateRect(hWnd, nullptr, /*TRUE*/ FALSE);
				return SIMPLEREGION;
			}
		}
		break;
		case DcxControlTypes::TREEVIEW:
		{
			if (const auto pTree = dynamic_cast<DcxTreeView*>(pthis); pTree->isExStyle(WindowExStyle::Transparent) || pTree->IsAlphaBlend() || pTree->isBkgImage())
			{
				InvalidateRect(hWnd, nullptr, /*TRUE*/ FALSE);
				return SIMPLEREGION;
			}
		}
		break;
		}
	}

	return ScrollWindowExUx(hWnd, dx, dy, prcScroll, prcClip, hrgnUpdate, prcUpdate, flags);
}
