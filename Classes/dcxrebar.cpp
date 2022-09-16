/*!
 * \file dcxrebar.cpp
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
#include "Classes/dcxrebar.h"
#include "Classes/dcxdialog.h"
#include "Dcx.h"

 /*!
  * \brief Constructor
  *
  * \param ID Control ID
  * \param p_Dialog Parent DcxDialog Object
  * \param mParentHwnd Parent Window Handle
  * \param rc Window Rectangle
  * \param styles Window Style Tokenized List
  */

DcxReBar::DcxReBar(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WS_EX_CONTROLPARENT,
		DCX_REBARCTRLCLASS,
		ws.m_Styles | WS_CHILD,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsWindow(m_Hwnd))
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
	{
		//SendMessage( m_Hwnd, RB_SETWINDOWTHEME, 0, L" ");
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");
	}

	setNoThemed(ws.m_NoTheme);

	{
		this->setImageList(this->createImageList());
		//REBARINFO rbi;
		//rbi.cbSize = sizeof(REBARINFO);
		//rbi.fMask = 0;
		//rbi.himl = nullptr;
		//this->setBarInfo( &rbi );
	}
	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief Destructor
 *
 * blah
 */

DcxReBar::~DcxReBar() noexcept
{
	this->resetContents();

	ImageList_Destroy(this->getImageList());
}

const TString DcxReBar::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, RBS_BANDBORDERS))
		styles.addtok(TEXT("borders"));
	if (dcx_testflag(Styles, RBS_DBLCLKTOGGLE))
		styles.addtok(TEXT("dblclktoggle"));
	if (dcx_testflag(Styles, RBS_FIXEDORDER))
		styles.addtok(TEXT("fixedorder"));
	if (dcx_testflag(Styles, RBS_VARHEIGHT))
		styles.addtok(TEXT("varheight"));
	if (dcx_testflag(Styles, RBS_TOOLTIPS))
		styles.addtok(TEXT("tooltips"));
	if (dcx_testflag(Styles, RBS_VERTICALGRIPPER))
		styles.addtok(TEXT("verticalgrip"));
	if (dcx_testflag(Styles, CCS_VERT))
		styles.addtok(TEXT("vertical"));
	if (dcx_testflag(Styles, CCS_RIGHT))
		styles.addtok(TEXT("right"));
	if (dcx_testflag(Styles, CCS_BOTTOM))
		styles.addtok(TEXT("bottom"));
	if (dcx_testflag(Styles, CCS_NORESIZE))
		styles.addtok(TEXT("noresize"));
	if (dcx_testflag(Styles, CCS_NOPARENTALIGN))
		styles.addtok(TEXT("noparentalign"));
	if (dcx_testflag(Styles, (CCS_NOPARENTALIGN | CCS_NORESIZE)))
		styles.addtok(TEXT("noauto"));

	return styles;
}

void DcxReBar::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());

	if (const auto count = this->getBandCount(); count > 0)
	{
		for (auto i = decltype(count){0}; i < count; ++i)
		{
			if (const auto* const c = this->getControl(i); c)
			{
				auto subs = std::make_unique<TiXmlElement>("control");
				c->toXml(subs.get());
				xml->LinkEndChild(subs.release());
			}
		}
	}
}

TiXmlElement* DcxReBar::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

DcxControl* DcxReBar::getControl(const int index) const noexcept
{
	if (index > -1 && index < this->getBandCount())
	{
		REBARBANDINFO rbBand{ sizeof(REBARBANDINFO), RBBIM_CHILD };

		if (this->getBandInfo(gsl::narrow_cast<UINT>(index), &rbBand) != 0)
			return this->getParentDialog()->getControlByHWND(rbBand.hwndChild);

	}
	return nullptr;
}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxReBar::parseControlStyles(const TString& tsStyles)
{
	auto ws = parseGeneralControlStyles(tsStyles);

	ws.m_Styles |= RBS_AUTOSIZE;

	//ExStyles |= WS_EX_CONTROLPARENT;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"borders"_hash:
			ws.m_Styles |= RBS_BANDBORDERS;
			break;
		case L"dblclktoggle"_hash:
			ws.m_Styles |= RBS_DBLCLKTOGGLE;
			break;
		case L"fixedorder"_hash:
			ws.m_Styles |= RBS_FIXEDORDER;
			break;
		case L"varheight"_hash:
			ws.m_Styles |= RBS_VARHEIGHT;
			break;
		case L"tooltips"_hash:
			ws.m_Styles |= RBS_TOOLTIPS;
			break;
		case L"verticalgrip"_hash:
			ws.m_Styles |= RBS_VERTICALGRIPPER;
			break;
		case L"vertical"_hash:
			ws.m_Styles |= CCS_VERT;
			break;
		case L"right"_hash:
			ws.m_Styles |= CCS_RIGHT;
			break;
		case L"bottom"_hash:
			ws.m_Styles |= CCS_BOTTOM;
			break;
		case L"noresize"_hash:
			ws.m_Styles |= CCS_NORESIZE;
			break;
		case L"noparentalign"_hash:
			ws.m_Styles |= CCS_NOPARENTALIGN;
			break;
		case L"noauto"_hash:
			ws.m_Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
			break;
		default:
			break;
		}
	}

	return ws;
}
/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxReBar::getImageList() const noexcept
{
	REBARINFO ri{ sizeof(REBARINFO),RBIM_IMAGELIST };

	getBarInfo(&ri);

	return ri.himl;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxReBar::setImageList(HIMAGELIST himl) noexcept
{
	REBARINFO ri{ sizeof(REBARINFO), RBIM_IMAGELIST, himl };

	setBarInfo(&ri);
}

 /*!
  * \brief $xdid Parsing Function
  *
  * \param input [NAME] [ID] [PROP] (OPTIONS)
  * \param szReturnValue mIRC Data Container
  *
  * \return > void
  */

void DcxReBar::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	const auto numtok = input.numtok();

	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
	case L"num"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), this->getBandCount());
		break;
		// [NAME] [ID] [PROP] [N]
	case L"text"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4

		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_TEXT;
		rbBand.cch = MIRC_BUFFER_SIZE_CCH;
		rbBand.lpText = szReturnValue;

		getBandInfo(gsl::narrow_cast<UINT>(nIndex), &rbBand);
	}
	break;
	case L"childid"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto n = input.getnexttok().to_int() - 1; // tok 4

		if (n < 0 || n >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		if (const auto* const c = this->getControl(n); c)
			_ts_snprintf(szReturnValue, TEXT("%u"), c->getUserID());
	}
	break;
	// $xdid([NAME], [ID], [N]).[PROP]
	case L"markeditem"_hash:
	{
		const auto n = input.getnexttok().to_int() - 1;	// tok 4

		if (n < 0 || n >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		REBARBANDINFO rbi{};
		rbi.cbSize = sizeof(REBARBANDINFO);
		rbi.fMask = RBBIM_LPARAM;

		getBandInfo(gsl::narrow_cast<UINT>(n), &rbi);

		auto pdcxrbb = reinterpret_cast<LPDCXRBBAND>(rbi.lParam);

		szReturnValue = pdcxrbb->tsMarkText.to_chr();
	}
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

void DcxReBar::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -a [NAME] [ID] [SWITCH] [N] [+FLAGS] [CX] [CY] [WIDTH] [ICON] [COLOR] [Item Text][TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)[TAB]Tooltip
	if (flags[TEXT('a')])
	{
		if (numtok < 10)
			throw DcxExceptions::dcxInvalidArguments();

		REBARBANDINFO rbBand{};
		if (Dcx::VistaModule.isUseable()) // NB: when rbBand.cbSize is set to the Vista size on XP the insertband will FAIL!! fucking MS!
			rbBand.cbSize = sizeof(REBARBANDINFO);
		else
			GSL_SUPPRESS(es.47) rbBand.cbSize = REBARBANDINFO_V6_SIZE;

		rbBand.fMask = RBBIM_STYLE | RBBIM_LPARAM;

		const auto data(input.getfirsttok(1, TSTABCHAR).trim());
		TString control_data, tooltip;
		const auto nToks = input.numtok(TSTABCHAR);

		if (nToks > 1)
		{
			control_data = input.getnexttok(TSTABCHAR).trim();		// tok 2

			if (nToks > 2)
				tooltip = input.getnexttok(TSTABCHAR).trim();		// tok 3
		}
		auto nIndex = data.getfirsttok(4).to_<int>() - 1;
		rbBand.fStyle = parseBandStyleFlags(data.getnexttok());	// tok 5
		const auto cx = data.getnexttok().to_<UINT>();					// tok 6
		const auto cy = data.getnexttok().to_<UINT>();					// tok 7
		const auto width = data.getnexttok().to_<UINT>();				// tok 8
		const auto nIcon = data.getnexttok().to_<int>() - 1;			// tok 9
		const auto clrText = data.getnexttok().to_<COLORREF>();			// tok 10

		if (nIndex < -1)
			nIndex = -1;

		rbBand.cxMinChild = cx;
		rbBand.cyMinChild = cy;
		rbBand.cx = width;
		rbBand.cyIntegral = 1;
		rbBand.cyChild = cy;

		TString itemtext;
		if (data.numtok() > 10)
		{
			itemtext = data.getlasttoks().trim();	// tok 11, -1
			rbBand.fMask |= RBBIM_TEXT;
			rbBand.lpText = itemtext.to_chr();
			//rbBand.cch = itemtext.len();
		}

		// Tooltip Handling
		auto lpdcxrbb = std::make_unique<DCXRBBAND>();

		lpdcxrbb->bUline = dcx_testflag(rbBand.fStyle, RBBS_UNDERLINE);

		lpdcxrbb->bBold = dcx_testflag(rbBand.fStyle, RBBS_BOLD);

		if (dcx_testflag(rbBand.fStyle, RBBS_COLOR))
			lpdcxrbb->clrText = clrText;
		else
			lpdcxrbb->clrText = CLR_INVALID;

		if (nIcon > -1)
		{
			rbBand.iImage = nIcon;
			rbBand.fMask |= RBBIM_IMAGE;
		}

		rbBand.lParam = reinterpret_cast<LPARAM>(lpdcxrbb.get());

		DcxControl* p_Control{ nullptr };
		if (control_data.numtok() > 5)
		{
			p_Control = this->getParentDialog()->addControl(control_data, 1,
				DcxAllowControls::ALLOW_TRACKBAR |
				DcxAllowControls::ALLOW_PBAR |
				DcxAllowControls::ALLOW_COMBOEX |
				DcxAllowControls::ALLOW_TOOLBAR |
				DcxAllowControls::ALLOW_STATUSBAR |
				DcxAllowControls::ALLOW_TREEVIEW |
				DcxAllowControls::ALLOW_LISTVIEW |
				DcxAllowControls::ALLOW_COLORCOMBO |
				DcxAllowControls::ALLOW_BUTTON |
				DcxAllowControls::ALLOW_RICHEDIT |
				DcxAllowControls::ALLOW_DIVIDER |
				DcxAllowControls::ALLOW_PANEL |
				DcxAllowControls::ALLOW_TAB, m_Hwnd);

			if (const auto dct = p_Control->getControlType(); ((dct == DcxControlTypes::STATUSBAR) || (dct == DcxControlTypes::TOOLBAR)))
				p_Control->addStyle(WindowStyle::CCS_NoParentAlign | CCS_NORESIZE);

			rbBand.fMask |= RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_ID;
			rbBand.hwndChild = p_Control->getHwnd();
			rbBand.wID = p_Control->getID();
		}

		if (this->insertBand(nIndex, &rbBand) == 0L)
		{ // 0L means failed.
			this->getParentDialog()->deleteControl(p_Control);
			if (rbBand.hwndChild)
				DestroyWindow(rbBand.hwndChild);

			throw Dcx::dcxException("Unable To Add Band");
		}
		lpdcxrbb.release();
	}
	// xdid -A [NAME] [ID] [SWITCH] [N] (TEXT)
	else if (flags[TEXT('A')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto n = input.getnexttok().to_int() - 1;	// tok 4

		if (n < 0 || n >= getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		REBARBANDINFO rbi{};
		rbi.cbSize = sizeof(REBARBANDINFO);
		rbi.fMask = RBBIM_LPARAM;

		this->getBandInfo(gsl::narrow_cast<UINT>(n), &rbi);
		if (auto pdcxrbb = reinterpret_cast<LPDCXRBBAND>(rbi.lParam); pdcxrbb)
			pdcxrbb->tsMarkText = (numtok > 4 ? input.getlasttoks() : TEXT(""));	// tok 5, -1
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4

		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		this->deleteBand(gsl::narrow_cast<UINT>(nIndex));
	}
	// xdid -i [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('i')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4

		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		this->showBand(gsl::narrow_cast<UINT>(nIndex), FALSE);
	}
	// xdid -j [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('j')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4

		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		this->showBand(gsl::narrow_cast<UINT>(nIndex), TRUE);
	}
	// xdid -k [NAME] [ID] [SWITCH] [N] [ICON]
	else if (flags[TEXT('k')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_IMAGE;

		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4
		const auto nIcon = input.getnexttok().to_int() - 1;	// tok 5

		if (nIndex < 0 || nIcon < -1 || nIndex >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		// Ook: TODO add check for nIcon being valid
		rbBand.iImage = nIcon;
		this->setBandInfo(nIndex, &rbBand);
	}
	// xdid -l -> [NAME] [ID] -l [N|ALL]
	else if (flags[TEXT('l')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_STYLE;

		const auto nItems = this->getBandCount();
		const auto tsItem(input.getnexttok());	// tok 4

		if (tsItem == TEXT("all"))
		{
			for (auto i = decltype(nItems){0}; i < nItems; ++i)
			{
				if (this->getBandInfo(i, &rbBand) != 0)
				{
					rbBand.fStyle |= RBBS_NOGRIPPER;
					this->setBandInfo(i, &rbBand);
				}
			}
		}
		else {
			const auto nIndex = tsItem.to_int() - 1;

			if (nIndex < 0 || nIndex >= nItems || this->getBandInfo(nIndex, &rbBand) == 0)
				throw DcxExceptions::dcxInvalidItem();

			rbBand.fStyle |= RBBS_NOGRIPPER;
			this->setBandInfo(nIndex, &rbBand);
		}
	}
	// xdid -m [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('m')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4

		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		this->maxBand(nIndex, FALSE);
	}
	// xdid -n [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('n')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4

		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		this->minBand(nIndex, FALSE);
	}
	// xdid -q [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('q')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nRows = input.getnexttok().to_int();	// tok 4

		if (nRows > -1)
			this->m_iRowLimit = nRows;
	}
	// xdid -t [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('t')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_TEXT;

		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4
		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		TString itemtext;
		if (numtok > 4)
			itemtext = input.getlasttoks().trim();	// tok 5, -1

		rbBand.lpText = itemtext.to_chr();
		this->setBandInfo(nIndex, &rbBand);
	}
	// xdid -u [NAME] [ID] [SWITCH] [N|ALL]
	else if (flags[TEXT('u')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_STYLE;

		const auto nItems = this->getBandCount();
		const auto tsItem(input.getnexttok());	// tok 4

		if (tsItem == TEXT("all"))
		{
			for (auto i = decltype(nItems){0}; i < nItems; ++i)
			{
				if (this->getBandInfo(gsl::narrow_cast<UINT>(i), &rbBand) != 0)
				{
					rbBand.fStyle &= ~RBBS_NOGRIPPER;
					this->setBandInfo(gsl::narrow_cast<UINT>(i), &rbBand);
				}
			}
		}
		else {
			const auto nIndex = tsItem.to_int() - 1;

			if (nIndex < 0 || nIndex >= nItems || this->getBandInfo(gsl::narrow_cast<UINT>(nIndex), &rbBand) == 0)
				throw DcxExceptions::dcxInvalidItem();

			rbBand.fStyle &= ~RBBS_NOGRIPPER;
			this->setBandInfo(gsl::narrow_cast<UINT>(nIndex), &rbBand);
		}
	}
	// xdid -v [NAME] [ID] [SWITCH] [NFrom] [NTo]
	else if (flags[TEXT('v')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nIndexFrom = input.getnexttok().to_int() - 1;	// tok 4
		const auto nIndexTo = input.getnexttok().to_int() - 1;		// tok 4
		const auto nItems = this->getBandCount();

		if (nIndexFrom < 0 || nIndexFrom >= nItems || nIndexTo < 0 || nIndexTo >= nItems)
			throw DcxExceptions::dcxInvalidItem();

		if (this->moveBand(gsl::narrow_cast<UINT>(nIndexFrom), gsl::narrow_cast<UINT>(nIndexTo)) == 0)
			throw Dcx::dcxException("Move failed");

		SendMessage(m_Hwnd, WM_SIZE, 0, 0); // NB: redraw does nothing here, needs a size to force the update.
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto flag(input.getnexttok());		// tok 4
		const auto index = input.getnexttok().to_int();	// tok 5
		auto filename(input.getlasttoks());			// tok 6, -1

		auto himl = this->getImageList();

		if (!himl)
		{
			himl = this->createImageList();

			if (himl)
				this->setImageList(himl);
		}

		if (!himl)
			throw Dcx::dcxException("Unable to get imagelist");
#if DCX_USE_WRAPPERS
		const Dcx::dcxIconResource icon(index, filename, false, flag);
		ImageList_AddIcon(himl, icon.get());
#else
		if (const HICON icon = dcxLoadIcon(index, filename, false, flag); icon)
		{
			ImageList_AddIcon(himl, icon);
			DestroyIcon(icon);
		}
#endif
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if (flags[TEXT('y')])
	{
		ImageList_Destroy(this->getImageList());
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxReBar::resetContents() noexcept
{
	auto nItems = gsl::narrow_cast<UINT>(this->getBandCount());

	while (nItems--)
		this->deleteBand(nItems);
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxReBar::parseBandStyleFlags(const TString& flags) noexcept
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('b')])
		iFlags |= RBBS_BOLD;
	if (xflags[TEXT('c')])
		iFlags |= RBBS_COLOR;
	if (xflags[TEXT('e')])
		iFlags |= RBBS_CHILDEDGE;
	if (xflags[TEXT('f')])
		iFlags |= RBBS_FIXEDSIZE;
	if (xflags[TEXT('g')])
		iFlags |= RBBS_GRIPPERALWAYS;
	if (xflags[TEXT('h')])
		iFlags |= RBBS_HIDDEN;
	if (xflags[TEXT('n')])
		iFlags |= RBBS_NOGRIPPER;
	if (xflags[TEXT('o')])
		iFlags |= RBBS_USECHEVRON;
	if (xflags[TEXT('u')])
		iFlags |= RBBS_UNDERLINE;
	if (xflags[TEXT('v')])
		iFlags |= RBBS_VARIABLEHEIGHT;
	if (xflags[TEXT('w')])
		iFlags |= RBBS_BREAK;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::insertBand(const int uIndex, const LPREBARBANDINFO lprbbi) noexcept
{
	return SendMessage(m_Hwnd, RB_INSERTBAND, gsl::narrow_cast<WPARAM>(uIndex), reinterpret_cast<LPARAM>(lprbbi));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::deleteBand(const UINT uIndex) noexcept
{
	return SendMessage(m_Hwnd, RB_DELETEBAND, gsl::narrow_cast<WPARAM>(uIndex), gsl::narrow_cast<LPARAM>(0));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getBandInfo(const UINT uBand, const LPREBARBANDINFO lprbbi) const noexcept
{
	return SendMessage(m_Hwnd, RB_GETBANDINFO, gsl::narrow_cast<WPARAM>(uBand), reinterpret_cast<LPARAM>(lprbbi));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setBandInfo(const UINT uBand, const LPREBARBANDINFO lprbbi) noexcept
{
	return SendMessage(m_Hwnd, RB_SETBANDINFO, gsl::narrow_cast<WPARAM>(uBand), reinterpret_cast<LPARAM>(lprbbi));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setBarInfo(const LPREBARINFO lprbi) noexcept
{
	return SendMessage(m_Hwnd, RB_SETBARINFO, gsl::narrow_cast<WPARAM>(0), reinterpret_cast<LPARAM>(lprbi));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getBarInfo(const LPREBARINFO lprbi) const noexcept
{
	return SendMessage(m_Hwnd, RB_GETBARINFO, gsl::narrow_cast<WPARAM>(0), reinterpret_cast<LPARAM>(lprbi));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getRowCount() const noexcept
{
	return SendMessage(m_Hwnd, RB_GETROWCOUNT, gsl::narrow_cast<WPARAM>(0), gsl::narrow_cast<LPARAM>(0));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::hitTest(const LPRBHITTESTINFO lprbht) const noexcept
{
	return SendMessage(m_Hwnd, RB_HITTEST, gsl::narrow_cast<WPARAM>(0), reinterpret_cast<LPARAM>(lprbht));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getToolTips() const noexcept
{
	return SendMessage(m_Hwnd, RB_GETTOOLTIPS, gsl::narrow_cast<WPARAM>(0), gsl::narrow_cast<LPARAM>(0));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setToolTips(const HWND hwndToolTip) noexcept
{
	return SendMessage(m_Hwnd, RB_SETTOOLTIPS, reinterpret_cast<WPARAM>(hwndToolTip), gsl::narrow_cast<LPARAM>(0));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getIDToIndex(const UINT uBandID) const noexcept
{
	return SendMessage(m_Hwnd, RB_IDTOINDEX, gsl::narrow_cast<WPARAM>(uBandID), gsl::narrow_cast<LPARAM>(0));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getBandCount() const noexcept
{
	return SendMessage(m_Hwnd, RB_GETBANDCOUNT, gsl::narrow_cast<WPARAM>(0), gsl::narrow_cast<LPARAM>(0));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setReDraw(const BOOL uState) noexcept
{
	return SendMessage(m_Hwnd, WM_SETREDRAW, gsl::narrow_cast<WPARAM>(uState), gsl::narrow_cast<LPARAM>(uState));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::showBand(const UINT uBand, const BOOL fShow) noexcept
{
	return SendMessage(m_Hwnd, RB_SHOWBAND, gsl::narrow_cast<WPARAM>(uBand), gsl::narrow_cast<LPARAM>(fShow));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::moveBand(const UINT iFrom, const UINT iTo) noexcept
{
	return SendMessage(m_Hwnd, RB_MOVEBAND, gsl::narrow_cast<WPARAM>(iFrom), gsl::narrow_cast<LPARAM>(iTo));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::maxBand(const UINT uBand, const BOOL fIdeal) noexcept
{
	return SendMessage(m_Hwnd, RB_MAXIMIZEBAND, gsl::narrow_cast<WPARAM>(uBand), gsl::narrow_cast<LPARAM>(fIdeal));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::minBand(const UINT uBand, const BOOL fIdeal) noexcept
{
	return SendMessage(m_Hwnd, RB_MINIMIZEBAND, gsl::narrow_cast<WPARAM>(uBand), gsl::narrow_cast<LPARAM>(fIdeal));
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxReBar::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (!hdr)
			break;

		switch (hdr->code)
		{
		case NM_CUSTOMDRAW:
		{
			dcxlParam(LPNMCUSTOMDRAW, lpncd);

			bParsed = TRUE;

			switch (lpncd->dwDrawStage)
			{
			case CDDS_PREPAINT:
				return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW);
			case CDDS_ITEMPREPAINT:
			{
				const auto lpdcxrbb = reinterpret_cast<LPDCXRBBAND>(lpncd->lItemlParam);

				if (!lpdcxrbb)
					return CDRF_DODEFAULT;

				if (lpdcxrbb->clrText != CLR_INVALID)
					SetTextColor(lpncd->hdc, lpdcxrbb->clrText);
			}
			return (CDRF_NOTIFYPOSTPAINT | CDRF_NEWFONT);
			case CDDS_ITEMPOSTPAINT:
				return CDRF_DODEFAULT;
			default:
				return CDRF_DODEFAULT;
			}
		} // NM_CUSTOMDRAW
		break;

		case RBN_HEIGHTCHANGE:
		{
			bParsed = TRUE;

			if (RECT rc{}; GetWindowRect(m_Hwnd, &rc))
			{
				if (const UINT width = gsl::narrow_cast<UINT>(rc.right - rc.left), height = gsl::narrow_cast<UINT>(rc.bottom - rc.top); (m_iWidth != width || m_iHeight != height))
				{
					execAliasEx(TEXT("change,%u,%d,%d"), getUserID(), width, height);

					m_iWidth = width;
					m_iHeight = height;
				}
			}
		}
		break;

		case RBN_ENDDRAG:
		{
			bParsed = TRUE;
			redrawWindow();
		}
		break;

		case RBN_DELETINGBAND:
		{
			bParsed = TRUE;

			dcxlParam(LPNMREBAR, lpnmrb);

			if (!lpnmrb)
				break;

			REBARBANDINFO rbBand{};
			rbBand.cbSize = sizeof(REBARBANDINFO);
			rbBand.fMask = RBBIM_CHILD;

			if (getBandInfo(lpnmrb->uBand, &rbBand) != 0)
			{
				if (IsWindow(rbBand.hwndChild))
					getParentDialog()->deleteControl(getParentDialog()->getControlByHWND(rbBand.hwndChild));

				auto lpdcxrbb = reinterpret_cast<LPDCXRBBAND>(lpnmrb->lParam);
				delete lpdcxrbb;
			}
		}
		break;
		default:
			break;
		} // switch
	}
	break;
	default:
		break;
	}

	return 0L;
}

LRESULT DcxReBar::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	LRESULT lRes = 0L;
	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		if (dcxlParam(LPNMHDR, hdr); (hdr) && IsWindow(hdr->hwndFrom))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(hdr->hwndFrom, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_HSCROLL:
	case WM_VSCROLL:
	case WM_COMMAND:
	{
		if (IsWindow(reinterpret_cast<HWND>(lParam)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(reinterpret_cast<HWND>(lParam), TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_DELETEITEM:
	{
		if (dcxlParam(LPDELETEITEMSTRUCT, idata); (idata) && (IsWindow(idata->hwndItem)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(idata->hwndItem, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_MEASUREITEM:
	{
		if (const auto cHwnd = GetDlgItem(m_Hwnd, gsl::narrow_cast<int>(wParam)); IsWindow(cHwnd))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(cHwnd, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_DRAWITEM:
	{
		if (dcxlParam(LPDRAWITEMSTRUCT, idata); (idata) && (IsWindow(idata->hwndItem)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(idata->hwndItem, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_LBUTTONUP:
	{
		if (RBHITTESTINFO rbhi{}; GetCursorPos(&rbhi.pt))
		{
			MapWindowPoints(nullptr, m_Hwnd, &rbhi.pt, 1);

			if (const auto band = hitTest(&rbhi); (dcx_testflag(rbhi.flags, RBHT_GRABBER) || dcx_testflag(rbhi.flags, RBHT_CAPTION)) && band != -1)
			{
				m_iClickedBand = band;

				if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
					this->execAliasEx(TEXT("sclick,%u,%d"), getUserID(), band + 1);
			}
		}
	}
	break;

	case WM_CONTEXTMENU:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
		{
			if (RBHITTESTINFO rbhi{}; GetCursorPos(&rbhi.pt))
			{
				MapWindowPoints(nullptr, m_Hwnd, &rbhi.pt, 1);

				if (const auto band = this->hitTest(&rbhi); band != -1)
					this->execAliasEx(TEXT("rclick,%u,%d"), getUserID(), band + 1);
			}
		}
	}
	break;

	case WM_SIZE:
	{
		InvalidateRect(m_Hwnd, nullptr, TRUE);
	}
	break;

	case WM_DESTROY:
	{
		delete this;
		bParsed = TRUE;
	}
	break;

	default:
		lRes = this->CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return lRes;
}

LRESULT DcxReBar::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
