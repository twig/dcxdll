/*!
 * \file dcxbutton.cpp
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
#include "Classes/dcxbutton.h"
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
DcxButton::DcxButton(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog, DcxControlTypes::BUTTON)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_BUTTONCLASS,
		ws.m_Styles | WindowStyle::Child | BS_PUSHBUTTON,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		DcxUXModule::dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	m_aColors[0] = GetSysColor(COLOR_BTNTEXT); // normal
	m_aColors[1] = m_aColors[0]; // hover
	m_aColors[2] = m_aColors[0]; // pushed
	m_aColors[3] = GetSysColor(COLOR_GRAYTEXT); // disabled

	setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);

	if (styles.istok(TEXT("tooltips")))
	{
		if (!IsWindow(p_Dialog->getToolTipHWND()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");

		setToolTipHWND(p_Dialog->getToolTipHWND());
		AddToolTipToolInfo(getToolTipHWND(), m_Hwnd);
	}

	// fix to allow pressing enter to work.
	if (dcx_testflag(ws.m_Styles, BS_DEFPUSHBUTTON))
		SetFocus(m_Hwnd);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxButton::~DcxButton()
{
	ImageList_Destroy(getImageList());

	for (auto& x : m_aBitmaps)
		x.reset();
}

dcxWindowStyles DcxButton::parseControlStyles(const TString& tsStyles)
{
	dcxWindowStyles ws;

	ws.m_Styles |= BS_NOTIFY;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"bitmap"_hash:
			ws.m_Styles |= BS_BITMAP;
			break;
		case L"default"_hash:
			ws.m_Styles |= BS_DEFPUSHBUTTON;
			break;
		case L"split"_hash:
			ws.m_Styles |= BS_SPLITBUTTON;
			break;
		case L"commandlink"_hash:
			ws.m_Styles |= BS_COMMANDLINK;
			break;
		default:
			break;
		}
	}

	return parseGeneralControlStyles(tsStyles, ws);
}

TString DcxButton::parseInfoRequest(const TString& input) const
{
	// [NAME] [ID] [PROP]
	switch (std::hash<TString>()(input.gettok(3)))
	{
	case TEXT("text"_hash):
		return m_tsCaption;
	case TEXT("note"_hash):
		return Dcx::dcxButton_GetNote(m_Hwnd);
	break;
	default:
		break;
	}
	return parseGlobalInfoRequest(input);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */
void DcxButton::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	//// [NAME] [ID] [PROP]
	//if (input.gettok(3) == TEXT("text"))
	//{
	//	// if copy fails drop through
	//	szReturnValue = m_tsCaption.to_chr();
	//}
	//else
	//	parseGlobalInfoRequest(input, szReturnValue);

	szReturnValue = parseInfoRequest(input).to_chr();
}

void DcxButton::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -c [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	if (flags[TEXT('c')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iColorStyles = parseColorFlags(input.getnexttok());	// tok 4
		const auto clrColor = input.getnexttok().to_<COLORREF>();		// tok 5

		if (dcx_testflag(iColorStyles, BTNCS_NORMAL))
			m_aColors[0] = clrColor;
		if (dcx_testflag(iColorStyles, BTNCS_HOVER))
			m_aColors[1] = clrColor;
		if (dcx_testflag(iColorStyles, BTNCS_SELECTED))
			m_aColors[2] = clrColor;
		if (dcx_testflag(iColorStyles, BTNCS_DISABLED))
			m_aColors[3] = clrColor;

		redrawWindow();
	}
	// xdid -k [NAME] [ID] [SWITCH] [+FLAGS] [COLOR] [FILENAME]
	else if (flags[TEXT('k')])
	{
		//if (numtok < 6)
		//	throw DcxExceptions::dcxInvalidArguments();
		//
		//if (!isStyle(WindowStyle::BS_Bitmap) && !isStyle(WindowStyle::BS_OwnerDraw))
		//	throw Dcx::dcxException("Command not supported by this button style.");
		//
		//const auto iColorStyles = parseColorFlags(input.getnexttok());	// tok 4
		//const auto clrColor = input.getnexttok().to_<COLORREF>();		// tok 5
		//
		//auto filename(input.getlasttoks().trim());	// tok 6, -1
		//
		//if (dcx_testflag(iColorStyles, BTNCS_NORMAL))
		//{
		//	m_aBitmaps[0] = dcxLoadBitmap(m_aBitmaps[0], filename);
		//	m_aTransp[0] = clrColor;
		//}
		//if (dcx_testflag(iColorStyles, BTNCS_HOVER))
		//{
		//	m_aBitmaps[1] = dcxLoadBitmap(m_aBitmaps[1], filename);
		//	m_aTransp[1] = clrColor;
		//}
		//if (dcx_testflag(iColorStyles, BTNCS_SELECTED))
		//{
		//	m_aBitmaps[2] = dcxLoadBitmap(m_aBitmaps[2], filename);
		//	m_aTransp[2] = clrColor;
		//}
		//if (dcx_testflag(iColorStyles, BTNCS_DISABLED))
		//{
		//	m_aBitmaps[3] = dcxLoadBitmap(m_aBitmaps[3], filename);
		//	m_aTransp[3] = clrColor;
		//}
		//
		//redrawWindow();

		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		if (!isStyle(WindowStyle::BS_Bitmap) && !isStyle(WindowStyle::BS_OwnerDraw))
			throw Dcx::dcxException("Command not supported by this button style.");

		const auto iColorStyles = parseColorFlags(input.getnexttok());	// tok 4
		const auto clrColor = input.getnexttok().to_<COLORREF>();		// tok 5

		auto filename(input.getlasttoks().trim());	// tok 6, -1

		if (dcx_testflag(iColorStyles, BTNCS_NORMAL))
		{
			m_aBitmaps[0].m_hBitmap = dcxLoadBitmap(m_aBitmaps[0].m_hBitmap, filename);
			m_aBitmaps[0].m_tsFilename = filename;
			m_aTransp[0] = clrColor;
		}
		if (dcx_testflag(iColorStyles, BTNCS_HOVER))
		{
			m_aBitmaps[1].m_hBitmap = dcxLoadBitmap(m_aBitmaps[1].m_hBitmap, filename);
			m_aBitmaps[1].m_tsFilename = filename;
			m_aTransp[1] = clrColor;
		}
		if (dcx_testflag(iColorStyles, BTNCS_SELECTED))
		{
			m_aBitmaps[2].m_hBitmap = dcxLoadBitmap(m_aBitmaps[2].m_hBitmap, filename);
			m_aBitmaps[2].m_tsFilename = filename;
			m_aTransp[2] = clrColor;
		}
		if (dcx_testflag(iColorStyles, BTNCS_DISABLED))
		{
			m_aBitmaps[3].m_hBitmap = dcxLoadBitmap(m_aBitmaps[3].m_hBitmap, filename);
			m_aBitmaps[3].m_tsFilename = filename;
			m_aTransp[3] = clrColor;
		}

		redrawWindow();
	}
	// xdid -l [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('l')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		m_iIconSize = NumToIconSize(input.getnexttok().to_<int>());	// tok 4

		if (auto himl = getImageList(); himl)
		{
			ImageList_Destroy(himl);
			setImageList(nullptr);
			m_bHasIcons = false;
		}
	}
	// xdid -t [NAME] [ID] [SWITCH] ItemText
	else if (flags[TEXT('t')])
	{
		if (numtok < 3)
			throw DcxExceptions::dcxInvalidArguments();

		m_tsCaption = (numtok > 3 ? input.getlasttoks().trim() : TString());	// tok 4, -1

		//Button_SetText(m_Hwnd, m_tsCaption.to_chr());

		redrawWindow();
	}
	// xdid -w [NAME] [ID] [SWITCH] [FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tsFlags(input.getnexttok());	// tok 4
		const auto tsIndex(input.getnexttok());	// tok 5
		auto filename(input.getlasttoks());		// tok 6, -1

		// load the icon
		this->loadIcon(tsFlags, tsIndex, filename);

		redrawWindow();
	}
	// xdid -m [NAME] [ID] [SWITCH] [ENABLED]
	else if (flags[TEXT('m')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto b = input.getnexttok().to_int();	// tok 4

		m_bBitmapText = (b > 0);	// any value > 0 taken as being true
		redrawWindow();
	}
	// xdid -n [NAME] [ID] [SWITCH] +(FLAGS) Text
	else if (flags[TEXT('n')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const XSwitchFlags xFlags(input.getnexttok());	// tok 4
		const auto tsText(input.getlasttoks().trim());	// tok 5, -1

		if (!xFlags[TEXT('+')])
			throw DcxExceptions::dcxInvalidFlag();

		m_tsCaption.clear();

		if (xFlags[TEXT('n')])	// note (for command link style)
			Dcx::dcxButton_SetNote(m_Hwnd, tsText.to_chr());
		else
			Dcx::dcxButton_SetText(m_Hwnd, tsText.to_chr());

		redrawWindow();
	}
	// xdid -S [NAME] [ID] [SWITCH] +FLAGS
	else if (flags[TEXT('S')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const XSwitchFlags xFlags(input.getnexttok());	// tok 4

		if (!xFlags[TEXT('+')])
			throw DcxExceptions::dcxInvalidFlag();

		DWORD dStyle = (Dcx::dcxLOWORD(GetWindowStyle(m_Hwnd)) & ~(BS_CHECKBOX | BS_RADIOBUTTON | BS_COMMANDLINK | BS_GROUPBOX));

		// button/check/radio/commandlink/group
		if (xFlags[TEXT('b')])
		{
			// standard button
		}
		else if (xFlags[TEXT('c')])
		{
			// check button
			dStyle |= BS_CHECKBOX;
		}
		else if (xFlags[TEXT('r')])
		{
			// radio button
			dStyle |= BS_RADIOBUTTON;
		}
		else if (xFlags[TEXT('C')])
		{
			// commandlink button
			dStyle |= BS_COMMANDLINK;
		}
		else if (xFlags[TEXT('g')])
		{
			// group button
			dStyle |= BS_GROUPBOX;
		}
		Dcx::dcxButton_SetStyle(m_Hwnd, dStyle, TRUE);
	}
	else
		parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

const UINT DcxButton::parseColorFlags(const TString& flags) noexcept
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('d')])
		iFlags |= BTNCS_DISABLED;
	if (xflags[TEXT('h')])
		iFlags |= BTNCS_HOVER;
	if (xflags[TEXT('n')])
		iFlags |= BTNCS_NORMAL;
	if (xflags[TEXT('s')])
		iFlags |= BTNCS_SELECTED;
	if (xflags[TEXT('g')])
		iFlags |= BTNIS_GREY;
	if (xflags[TEXT('a')])
		iFlags |= BTNIS_ASSOC;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

const HIMAGELIST& DcxButton::getImageList() const noexcept
{
	return m_ImageList;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxButton::setImageList(const HIMAGELIST himl) noexcept
{
	m_ImageList = himl;
}

/*!
 * \brief blah
 *
 * blah
 */

GSL_SUPPRESS(bounds.3)
HIMAGELIST DcxButton::createImageList() noexcept
{
	// set initial size of image list to countof(m_aBitmaps) = normal/hover/push/disabled
	return ImageList_Create(gsl::narrow_cast<int>(m_iIconSize), gsl::narrow_cast<int>(m_iIconSize), ILC_COLOR32 | ILC_MASK, gsl::narrow_cast<int>(std::size(m_aBitmaps)), 0);
}

void DcxButton::loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc)
{
	const auto index = tsIndex.to_int();
	const auto flag = parseColorFlags(tsFlags);
	auto filename(tsSrc);

	// load the icon

#if DCX_USE_WRAPPERS
	const Dcx::dcxIconResource icon(index, filename, (m_iIconSize != DcxIconSizes::SmallIcon), tsFlags);
#else
	const auto icon = dcxLoadIcon(index, filename, (m_iIconSize != DcxIconSizes::SmallIcon), tsFlags);

	if (!icon)
		throw Dcx::dcxException("Unable to load icon");

	Auto(DestroyIcon(icon));
#endif

	// prepare the image list
	if (auto himl = getImageList(); !himl)
	{
		himl = createImageList();

		if (himl)
		{
			setImageList(himl);

			ImageList_AddIcon(himl, icon);
			ImageList_AddIcon(himl, icon);
			ImageList_AddIcon(himl, icon);
			ImageList_AddIcon(himl, icon);

			m_bHasIcons = true;
		}
	}
	else {
		if (dcx_testflag(flag, BTNCS_DISABLED))
			ImageList_ReplaceIcon(himl, 3, icon);
		if (dcx_testflag(flag, BTNCS_SELECTED))
			ImageList_ReplaceIcon(himl, 2, icon);
		if (dcx_testflag(flag, BTNCS_HOVER))
			ImageList_ReplaceIcon(himl, 1, icon);
		if (dcx_testflag(flag, BTNCS_NORMAL))
			ImageList_ReplaceIcon(himl, 0, icon);
	}
}

const TString DcxButton::getStyles(void) const
{
	auto tsStyles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, BS_BITMAP))
		tsStyles.addtok(TEXT("bitmap"));
	if (dcx_testflag(Styles, BS_DEFPUSHBUTTON))
		tsStyles.addtok(TEXT("default"));
	if (dcx_testflag(Styles, BS_COMMANDLINK))		// BS_COMMANDLINK == 1110
		tsStyles.addtok(TEXT("commandlink"));
	else if (dcx_testflag(Styles, BS_SPLITBUTTON))	// BS_SPLITBUTTON == 1100, so split shows as true when commandlink is used.
		tsStyles.addtok(TEXT("split"));

	return tsStyles;
}

void DcxButton::toXml(TiXmlElement* const xml) const
{
	if (!xml || !m_Hwnd)
		return;

	__super::toXml(xml);

	const TString tsStyles(getStyles());

	if (const auto bCmd = tsStyles.istok(TEXT("commandlink")); bCmd || tsStyles.istok(TEXT("split")))
	{
		xml->SetAttribute("text", TGetWindowText(m_Hwnd).c_str());
		if (bCmd)
		{
			if (Dcx::dcxButton_GetNoteLength(m_Hwnd) > 0)
			{
				//TCHAR szBuf[MIRC_BUFFER_SIZE_CCH]{};
				//DWORD l{ MIRC_BUFFER_SIZE_CCH - 1 };
				//Dcx::dcxButton_GetNote(m_Hwnd, &szBuf[0], &l); // NB: two POINTERS
				//xml->SetAttribute("note", TString(szBuf).c_str());

				const auto tsBuf(Dcx::dcxButton_GetNote(m_Hwnd));
				xml->SetAttribute("note", tsBuf.c_str());
			}
		}
	}
	else {
		if (!m_tsCaption.empty())
			xml->SetAttribute("caption", m_tsCaption.c_str());
		else
			xml->SetAttribute("text", TGetWindowText(m_Hwnd).c_str());
	}

	xml->SetAttribute("styles", tsStyles.c_str());
	if (m_bBitmapText)
		xml->SetAttribute("bitmaptext", "1");

	if (this->m_iIconSize != DcxIconSizes::SmallIcon) // small is the default.
		xml->SetAttribute("iconsize", gsl::narrow_cast<int>(this->m_iIconSize));

	{
		// save colours
		TiXmlElement xColours("colours");
		setColourAttribute(&xColours, "normal", m_aColors[0]);
		setColourAttribute(&xColours, "hover", m_aColors[1]);
		setColourAttribute(&xColours, "pushed", m_aColors[2]);
		setColourAttribute(&xColours, "disabled", m_aColors[3]);

		xml->InsertEndChild(xColours);
	}
	{
		// save transparentcolours (used with images)
		TiXmlElement xColours("transparentcolours");
		setColourAttribute(&xColours, "normal", m_aTransp[0]);
		setColourAttribute(&xColours, "hover", m_aTransp[1]);
		setColourAttribute(&xColours, "pushed", m_aTransp[2]);
		setColourAttribute(&xColours, "disabled", m_aTransp[3]);

		xml->InsertEndChild(xColours);
	}

	{
		// images
		TiXmlElement xImages("images");
		if (m_aBitmaps[0].m_hBitmap)
			xImages.SetAttribute("normalsrc", m_aBitmaps[0].m_tsFilename.c_str());
		if (m_aBitmaps[1].m_hBitmap)
			xImages.SetAttribute("hoversrc", m_aBitmaps[1].m_tsFilename.c_str());
		if (m_aBitmaps[2].m_hBitmap)
			xImages.SetAttribute("pushedsrc", m_aBitmaps[2].m_tsFilename.c_str());
		if (m_aBitmaps[3].m_hBitmap)
			xImages.SetAttribute("disabledsrc", m_aBitmaps[3].m_tsFilename.c_str());
		if (m_aBitmaps[0].m_hBitmap || m_aBitmaps[1].m_hBitmap || m_aBitmaps[2].m_hBitmap || m_aBitmaps[3].m_hBitmap)
			xml->InsertEndChild(xImages);
	}
	// icons...
	if (auto himl = this->getImageList(); himl)
		xmlSaveImageList(himl, xml, L"+B"_ts);
}

TiXmlElement* DcxButton::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxButton::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	// NB: needs to be before __super::toXml() to size is set when icons are loaded.
	m_iIconSize = NumToIconSize(queryIntAttribute(xThis, "iconsize", gsl::narrow_cast<int>(DcxIconSizes::SmallIcon)));

	__super::fromXml(xDcxml, xThis);

	{
		m_tsCaption = queryAttribute(xThis, "caption");
		const TString tsText(queryAttribute(xThis, "text"));

		const TString tsStyles(getStyles());
		if (auto bCmd = tsStyles.istok(TEXT("commandlink")); bCmd || tsStyles.istok(TEXT("split")))
		{
			Button_SetText(m_Hwnd, tsText.to_chr());
			if (bCmd)
			{
				if (const TString tsNote(queryAttribute(xThis, "note")); !tsNote.empty())
					Button_SetNote(m_Hwnd, tsNote.to_chr());
			}
		}
		else if (!tsText.empty())
		{
			Button_SetText(m_Hwnd, tsText.to_chr());
		}
	}

	m_bBitmapText = (queryIntAttribute(xThis, "bitmaptext") > 0);

	if (auto xColours = xThis->FirstChildElement("colours"); xColours)
	{
		m_aColors[0] = queryColourAttribute(xColours, "normal");
		m_aColors[1] = queryColourAttribute(xColours, "hover");
		m_aColors[2] = queryColourAttribute(xColours, "pushed");
		m_aColors[3] = queryColourAttribute(xColours, "disabled");
	}
	if (auto xColours = xThis->FirstChildElement("transparentcolours"); xColours)
	{
		m_aTransp[0] = queryColourAttribute(xColours, "normal");
		m_aTransp[1] = queryColourAttribute(xColours, "hover");
		m_aTransp[2] = queryColourAttribute(xColours, "pushed");
		m_aTransp[3] = queryColourAttribute(xColours, "disabled");
	}
	if (auto xImages = xThis->FirstChildElement("images"); xImages)
	{
		this->m_aBitmaps[0].m_tsFilename = queryAttribute(xImages, "normalsrc");
		if (!this->m_aBitmaps[0].m_tsFilename.empty())
			m_aBitmaps[0].m_hBitmap = dcxLoadBitmap(m_aBitmaps[0].m_hBitmap, this->m_aBitmaps[0].m_tsFilename);
		this->m_aBitmaps[1].m_tsFilename = queryAttribute(xImages, "hoversrc");
		if (!this->m_aBitmaps[1].m_tsFilename.empty())
			m_aBitmaps[1].m_hBitmap = dcxLoadBitmap(m_aBitmaps[1].m_hBitmap, this->m_aBitmaps[1].m_tsFilename);
		this->m_aBitmaps[2].m_tsFilename = queryAttribute(xImages, "pushedsrc");
		if (!this->m_aBitmaps[2].m_tsFilename.empty())
			m_aBitmaps[2].m_hBitmap = dcxLoadBitmap(m_aBitmaps[2].m_hBitmap, this->m_aBitmaps[2].m_tsFilename);
		this->m_aBitmaps[3].m_tsFilename = queryAttribute(xImages, "disabledsrc");
		if (!this->m_aBitmaps[3].m_tsFilename.empty())
			m_aBitmaps[3].m_hBitmap = dcxLoadBitmap(m_aBitmaps[3].m_hBitmap, this->m_aBitmaps[3].m_tsFilename);
	}
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxButton::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	//const auto pDialog = getParentDialog();
	//if (!pDialog)
	//	return 0L;	// something went very wrong...

	switch (uMsg)
	{
	case WM_COMMAND:
	{
		switch (Dcx::dcxHIWORD(wParam))
		{
		case BN_CLICKED:
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
				execAliasEx(TEXT("sclick,%u"), getUserID());
		}
		break;
		case BN_DBLCLK:
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
				execAliasEx(TEXT("dclick,%u"), getUserID());
		}
		break;
		default:
			break;
		}
	}
	break;
	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, pnmh);

		if (!pnmh)
			break;

		switch (pnmh->code)
		{
		case BCN_HOTITEMCHANGE:
		{
			if (dcxlParam(LPNMBCHOTITEM, item); dcx_testflag(getEventMask(), DCX_EVENT_FOCUS))
			{
				if (dcx_testflag(item->dwFlags, HICF_ENTERING))
					execAliasEx(TEXT("hotchange,%u,entering"), getUserID());
				else
					execAliasEx(TEXT("hotchange,%u,leaving"), getUserID());
			}
		}
		break;
		case BCN_DROPDOWN:
		{
			if (dcxlParam(LPNMBCDROPDOWN, item); dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			{
				execAliasEx(TEXT("dropdown,%u,%d,%d,%d,%d"), getUserID(), item->rcButton.top, item->rcButton.left, item->rcButton.bottom, item->rcButton.right);
			}
		}
		break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}
	return 0L;
}

LRESULT DcxButton::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	//const auto pDialog = getParentDialog();
	//if (!pDialog)
	//	return 0L;	// something went very wrong...

	switch (uMsg)
	{
	case WM_MOUSEMOVE:
	{
		if (const auto pDialog = getParentDialog(); pDialog)
			pDialog->setMouseControl(getUserID());

		m_bHover = false;

		if (!m_bTracking)
			m_bTracking = TrackMouseEvents(TME_LEAVE | TME_HOVER);

		if (!m_bTouched)
		{
			m_bTouched = true;
			InvalidateRect(m_Hwnd, nullptr, FALSE);
		}
	}
	break;

	case WM_MOUSEHOVER:
	{
		if (!m_bHover && m_bTracking)
			m_bHover = true;
	}
	break;

	case WM_MOUSELEAVE:
	{
		if (m_bTracking)
		{
			m_bHover = false;
			m_bTracking = false;
			m_bTouched = false;
			m_bSelected = false;
			InvalidateRect(m_Hwnd, nullptr, FALSE);
		}
	}
	break;

	case WM_LBUTTONDOWN:
	{
		if (!m_bSelected)
		{
			m_bSelected = true;
			InvalidateRect(m_Hwnd, nullptr, FALSE);
		}
		if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("lbdown,%u"), getUserID());
	}
	break;

	case WM_LBUTTONUP:
	{
		m_bSelected = false;
		if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("lbup,%u"), getUserID());
	}
	break;

	case WM_ENABLE:
	{
		InvalidateRect(m_Hwnd, nullptr, TRUE);
	}
	break;

	case WM_UPDATEUISTATE:
	{
		InvalidateRect(m_Hwnd, nullptr, TRUE);
	}
	break;

	case WM_ERASEBKGND:
	{
		bParsed = TRUE;
		return TRUE;
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
		PAINTSTRUCT ps{};

		auto hdc = BeginPaint(m_Hwnd, &ps);
		Auto(EndPaint(m_Hwnd, &ps));

		DrawClientArea(hdc, uMsg, lParam);
	}
	break;

	case WM_DESTROY:
	{
		this->CallDefaultClassProc(uMsg, wParam, lParam);

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

GSL_SUPPRESS(bounds.3)
void DcxButton::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	if (!hdc)
		return;

	RECT rcClient{};

	// get controls client area
	if (!GetClientRect(m_Hwnd, &rcClient))
		return;

	// get controls width & height.
	const auto w = (rcClient.right - rcClient.left), h = (rcClient.bottom - rcClient.top);

	int nState{}; // get buttons state.

	if (IsWindowEnabled(m_Hwnd) == FALSE)
		nState = 3;
	else if (m_bSelected)
		nState = 2;
	else if (m_bTouched)
		nState = 1;

	// Setup alpha blend if any.
	const auto ai = SetupAlphaBlend(&hdc);
	Auto(FinishAlphaBlend(ai));

	if (!hdc)
		return;

	gsl::owner<HTHEME> hTheme{ nullptr };
	int iStateId{};
	if (IsThemed() && DcxUXModule::dcxIsThemeActive())
	{
		hTheme = DcxUXModule::dcxOpenThemeData(m_Hwnd, VSCLASS_BUTTON);
		//hTheme = DcxUXModule::dcxGetWindowTheme(m_Hwnd);

		// this allows the theme buttons to have a transparent background like the normal ones
		switch (nState)
		{
		case 1:
			iStateId = PBS_HOT;
			break;
		case 2:
			iStateId = PBS_PRESSED;
			break;
		case 3:
			iStateId = PBS_DISABLED;
			break;
		default:
			iStateId = PBS_NORMAL;
			break;
		}
	}

	// fill background.
	if (isExStyle(WindowExStyle::Transparent))
	{ // fill with parent image
		if (!IsAlphaBlend())
			DrawParentsBackground(hdc, &rcClient);
	}
	else // normal bkg
		DcxControl::DrawCtrlBackground(hdc, this, &rcClient, hTheme, BP_PUSHBUTTON, iStateId);

	const bool isBitmap = isStyle(WindowStyle::BS_Bitmap);

	// Bitmapped button
	if (isBitmap)
	{
		// create a new HDC for background rendering
#if DCX_USE_WRAPPERS
		const Dcx::dcxHDCBitmapResource hdcbmp(hdc, gsl::at(m_aBitmaps, nState).m_hBitmap);

		// get bitmaps info.
		if (auto [code, bmp] = Dcx::dcxGetObject<BITMAP>(gsl::at(m_aBitmaps, nState).m_hBitmap); code != 0)
			TransparentBlt(hdc, rcClient.left, rcClient.top, w, h, hdcbmp.get(), 0, 0, bmp.bmWidth, bmp.bmHeight, gsl::at(m_aTransp, nState));

#else
		if (auto hdcbmp = CreateCompatibleDC(hdc); hdcbmp)
		{
			Auto(DeleteDC(hdcbmp));

			// get bitmaps info.
			if (BITMAP bmp{}; GetObject(m_aBitmaps[nState].m_hBitmap, sizeof(BITMAP), &bmp) != 0)
			{
				// associate bitmap with HDC
				const auto oldbm = SelectBitmap(hdcbmp, m_aBitmaps[nState].m_hBitmap);
				Auto(SelectBitmap(hdcbmp, oldbm)); // got to put the old bm back.

				TransparentBlt(hdc, rcClient.left, rcClient.top, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, m_aTransp[nState]);
			}
		}
#endif
	}

	// Regular button
	if ((!isBitmap) || (m_bBitmapText))
	{
		// draw default window bg
		if (!isBitmap)
		{
			if (hTheme)
			{
				// This method causes the theme bkg to re-appear during resize, but button is otherwise drawn correctly.
				if (uMsg != WM_PRINTCLIENT)
				{
					if (HRGN hRgn{ nullptr }; DcxUXModule::dcxGetThemeBackgroundRegion(hTheme, hdc, BP_PUSHBUTTON, iStateId, &rcClient, &hRgn) == S_OK)
					{
						if (hRgn)
						{
							if ((m_hZeroRgn) && (!EqualRgn(hRgn, m_hZeroRgn)))
								SelectClipRgn(hdc, hRgn);
							DeleteObject(hRgn);
						}
					}
				}
				CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
			}
			else {
				auto rc = rcClient;
				UINT iState = DFCS_BUTTONPUSH | DFCS_ADJUSTRECT;
				switch (nState)
				{
				case 1:
					iState |= DFCS_HOT;
					break;
				case 2:
					iState |= DFCS_PUSHED;
					break;
				case 3:
					iState |= DFCS_INACTIVE;
					break;
				default:
					break;
				}
				DrawFrameControl(hdc, &rc, DFC_BUTTON, iState);
				if (IsGradientFillEnabled())
				{
					const auto clrStart = (getStartGradientColor() != CLR_INVALID) ? getStartGradientColor() : GetSysColor(COLOR_3DFACE);
					const auto clrEnd = (getEndGradientColor() != CLR_INVALID) ? getEndGradientColor() : GetSysColor(COLOR_GRADIENTACTIVECAPTION);

					XPopupMenuItem::DrawGradient(hdc, &rc, clrStart, clrEnd, !IsGradientFillVertical());
				}
			}
		}

		const auto hFontOld = Dcx::dcxSelectObject<HFONT>(hdc, getControlFont());
		Auto(Dcx::dcxSelectObject<HFONT>(hdc, hFontOld));

		const auto oldbkg = SetBkMode(hdc, TRANSPARENT);
		Auto(SetBkMode(hdc, oldbkg));

		RECT rcTxt{ BUTTON_XPAD,BUTTON_YPAD,(rcClient.right - BUTTON_XPAD),(rcClient.bottom - BUTTON_YPAD) };

		if (!m_tsCaption.empty())
		{
			//calcTextRect(hdc, this->m_tsCaption, &rcTxt, DT_WORD_ELLIPSIS | DT_SINGLELINE);
			auto t(m_tsCaption);
			if (!IsControlCodeTextDisabled())
				t.strip();
			if (!m_bSelected && IsShadowTextEnabled())
				dcxDrawShadowText(hdc, t.to_wchr(), gsl::narrow_cast<UINT>(t.len()), &rcTxt, DT_WORD_ELLIPSIS | DT_SINGLELINE | DT_CALCRECT, gsl::at(m_aColors, nState), 0, 5, 5);
			else
				DrawTextW(hdc, t.to_chr(), gsl::narrow_cast<int>(t.len()), &rcTxt, DT_WORD_ELLIPSIS | DT_SINGLELINE | DT_CALCRECT);
		}

		const auto iCenter = w / 2;
		const auto iVCenter = h / 2;
		const auto iTextW = (rcTxt.right - rcTxt.left);
		const auto iTextH = (rcTxt.bottom - rcTxt.top);

		// If there is an icon
		if (auto himl = getImageList(); himl && m_bHasIcons)
		{
			auto iIconLeft = (iCenter - ((gsl::narrow_cast<int>(m_iIconSize) + ICON_XPAD + iTextW) / 2));
			auto iIconTop = (iVCenter - (gsl::narrow_cast<int>(m_iIconSize) / 2));

			if (iIconLeft < BUTTON_XPAD)
				iIconLeft = BUTTON_XPAD;

			if (iIconTop < BUTTON_YPAD)
				iIconTop = BUTTON_YPAD;

			rcTxt.left = iIconLeft + gsl::narrow_cast<int>(m_iIconSize) + ICON_XPAD;

			if (nState == 3) // disabled
				ImageList_Draw(himl, nState, hdc, iIconLeft, iIconTop, ILD_TRANSPARENT | ILD_BLEND50);
			else
				ImageList_Draw(himl, nState, hdc, iIconLeft, iIconTop, ILD_TRANSPARENT);
		}
		else {
			rcTxt.left = (iCenter - (iTextW / 2));
			if (rcTxt.left < BUTTON_XPAD)
				rcTxt.left = BUTTON_XPAD;
		}

		if (iTextW > 0)
		{
			rcTxt.top = iVCenter - iTextH / 2;

			if (rcTxt.top < BUTTON_YPAD)
				rcTxt.top = BUTTON_YPAD;

			rcTxt.right = (rcClient.right - BUTTON_XPAD);
			rcTxt.bottom = (rcClient.bottom - BUTTON_YPAD);

			const auto oldClr = SetTextColor(hdc, gsl::at(m_aColors, nState));
			Auto(SetTextColor(hdc, oldClr));

			//ctrlDrawText(hdc, m_tsCaption, &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE);

			if (IsControlCodeTextDisabled())
			{
				if (!m_bSelected && IsShadowTextEnabled())
					dcxDrawShadowText(hdc, m_tsCaption.to_wchr(), gsl::narrow_cast<UINT>(m_tsCaption.len()), &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE, gsl::at(m_aColors, nState), 0, 5, 5);
				else
					DrawTextW(hdc, m_tsCaption.to_wchr(), gsl::narrow_cast<int>(m_tsCaption.len()), &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE);
			}
			else
				mIRC_DrawText(hdc, m_tsCaption, &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE, (!m_bSelected && IsShadowTextEnabled()));
		}
	}

	if (hTheme)
		DcxUXModule::dcxCloseThemeData(hTheme);
}

LRESULT DcxButton::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
