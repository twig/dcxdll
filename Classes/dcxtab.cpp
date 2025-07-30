/*!
 * \file dcxtab.cpp
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
#include "Classes/dcxtab.h"
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
DcxTab::DcxTab(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog, DcxControlTypes::TABB)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WS_EX_CONTROLPARENT,
		DCX_TABCTRLCLASS,
		ws.m_Styles | WS_CHILD,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		DcxUXModule::dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	if (this->m_bClosable)
		Dcx::dcxTabCtrl_SetPadding(m_Hwnd, DcxDPIModule::dcxGetWindowMetrics(m_Hwnd, SM_CXSMICON), DcxDPIModule::dcxGetWindowMetrics(m_Hwnd, SM_CYFIXEDFRAME));

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);

	if (m_bPeek)
		CreatePeek();
	}

DcxTab::~DcxTab() noexcept
{
	if (m_hPeek)
		DestroyWindow(m_hPeek);

	ImageList_Destroy(this->setImageList(nullptr));

	const auto nItems = getTabCount();
	for (auto n = decltype(nItems){0}; n < nItems; ++n)
		this->deleteLParamInfo(n);
}

dcxWindowStyles DcxTab::parseControlStyles(const TString& tsStyles)
{
	auto ws = parseGeneralControlStyles(tsStyles);

	//ExStyles = WS_EX_CONTROLPARENT;
	ws.m_Styles |= TCS_OWNERDRAWFIXED;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"vertical"_hash:
			ws.m_Styles |= TCS_VERTICAL;
			break;
		case L"bottom"_hash:
			ws.m_Styles |= TCS_BOTTOM;
			break;
		case L"right"_hash:
			ws.m_Styles |= TCS_RIGHT;
			break;
		case L"fixedwidth"_hash:
			ws.m_Styles |= TCS_FIXEDWIDTH;
			break;
		case L"buttons"_hash:
			ws.m_Styles |= TCS_BUTTONS;
			break;
		case L"flat"_hash:
			ws.m_Styles |= TCS_FLATBUTTONS;
			break;
		case L"hottrack"_hash:
			ws.m_Styles |= TCS_HOTTRACK;
			break;
		case L"multiline"_hash:
			ws.m_Styles |= TCS_MULTILINE;
			break;
		case L"rightjustify"_hash:
			ws.m_Styles |= TCS_RIGHTJUSTIFY;
			break;
		case L"scrollopposite"_hash:
			ws.m_Styles |= TCS_SCROLLOPPOSITE;
			break;
			//case L"tooltips"_hash:
			//  Styles |= TCS_TOOLTIPS;
			//	break;
		case L"flatseps"_hash:
			ws.m_ExStyles |= TCS_EX_FLATSEPARATORS;
			break;
		case L"forcelabelleft"_hash:
			ws.m_Styles |= TCS_FORCELABELLEFT;
			break;
		case L"closable"_hash:
		{
			m_bClosable = true;
			//Styles |= TCS_OWNERDRAWFIXED;
		}
		break;
		//case L"gradient"_hash:
		//	m_bGradient = true;
		//	break;
		case L"peek"_hash:
		{
			m_bPeek = true;
		}
		break;
		default:
			break;
		}
	}

	return ws;
}

void DcxTab::loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc)
{
	auto filename(tsSrc);

	auto himl = this->getImageList();
	if (!himl)
	{
		himl = this->createImageList();

		if (himl)
			this->setImageList(himl);
	}
	if (!himl)
		throw Dcx::dcxException("Unable to get Image List");

	Dcx::dcxLoadIconRange(himl, filename, false, tsFlags, tsIndex);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */
void DcxTab::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	const auto numtok = input.numtok();

	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
	case L"text"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 || nItem >= getTabCount())
			throw DcxExceptions::dcxInvalidItem();

		TCITEM tci{ TCIF_TEXT,0,0, szReturnValue, MIRC_BUFFER_SIZE_CCH,0,0 };

		getTab(nItem, &tci);
	}
	break;
	case L"num"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), getTabCount());
		break;
	case L"peek"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), m_bPeek);
		break;
		// [NAME] [ID] [PROP] [N]
	case L"icon"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iTab = input.getnexttok().to_int() - 1;		// tok 4

		if (iTab == -1)
		{
			if (auto himl = TabCtrl_GetImageList(m_Hwnd); himl)
			{
				_ts_snprintf(szReturnValue, TEXT("%d"), ImageList_GetImageCount(himl));
				return;
			}
		}
		if (iTab < 0 || iTab >= getTabCount())
			throw DcxExceptions::dcxInvalidItem();

		TCITEM tci{ TCIF_IMAGE,0U,0U, nullptr, 0, 0, 0 };

		getTab(iTab, &tci);

		_ts_snprintf(szReturnValue, TEXT("%d"), tci.iImage + 1);
	}
	break;
	case L"sel"_hash:
	{
		const auto nItem = Dcx::dcxTabCtrl_GetCurSel(m_Hwnd);

		if (nItem < 0 || nItem >= getTabCount())
			throw DcxExceptions::dcxInvalidItem();

		_ts_snprintf(szReturnValue, TEXT("%d"), nItem + 1);
	}
	break;
	case L"seltext"_hash:
	{
		const auto nItem = Dcx::dcxTabCtrl_GetCurSel(m_Hwnd);

		if (nItem < 0 || nItem >= getTabCount())
			throw DcxExceptions::dcxInvalidItem();

		TCITEM tci{ TCIF_TEXT,0U,0U, szReturnValue, MIRC_BUFFER_SIZE_CCH, 0, 0 };

		getTab(nItem, &tci);
	}
	break;
	case L"childid"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 || nItem >= getTabCount())
			throw DcxExceptions::dcxInvalidItem();

		TCITEM tci{ TCIF_PARAM,0U,0U, nullptr, 0, 0, 0 };

		if (getTab(nItem, &tci))
		{
			if (const auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam); lpdtci)
			{
				if (const auto* const c = this->getParentDialog()->getControlByHWND(lpdtci->mChildHwnd); c)
					_ts_snprintf(szReturnValue, TEXT("%u"), c->getUserID());
			}
		}
	}
	break;
	// [NAME] [ID] [PROP]
	case L"mouseitem"_hash:
	{
		if (const auto tab = HitTestOnItem(); tab != -1)
			_ts_snprintf(szReturnValue, TEXT("%d"), tab + 1);
		else
			throw Dcx::dcxException("Unable to get cursor position");
	}
	break;
	case L"tabrect"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 || nItem >= getTabCount())
			throw DcxExceptions::dcxInvalidItem();

		RECT rc{};
		Dcx::dcxTabCtrl_GetItemRect(m_Hwnd, nItem, &rc);
		_ts_snprintf(szReturnValue, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right, rc.bottom);
	}
	break;
	case L"tabsrect"_hash:
	{
		const auto nTotal = getTabCount();
		RECT rc{};
		Dcx::dcxTabCtrl_GetItemRect(m_Hwnd, 0, &rc);

		for (int nItem{ 1 }; nItem < nTotal; ++nItem)
		{
			RECT rcItem{};
			Dcx::dcxTabCtrl_GetItemRect(m_Hwnd, nItem, &rcItem);

			//rc.bottom += (rcItem.bottom - rcItem.top);
			rc.right += (rcItem.right - rcItem.left);
		}
		_ts_snprintf(szReturnValue, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right, rc.bottom);
	}
	break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
}

void DcxTab::parseCommandRequest(const TString& input)
{
	const XSwitchFlags xflags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (xflags[TEXT('r')])
	{
		TCITEM tci{};
		const auto nItems = getTabCount();

		for (auto n = decltype(nItems){0}; n < nItems; ++n)
		{
			ZeroMemory(&tci, sizeof(TCITEM));

			tci.mask = TCIF_PARAM;

			if (getTab(n, &tci))
			{
				if (auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam); lpdtci)
				{
					if (lpdtci->mChildHwnd && IsWindow(lpdtci->mChildHwnd))
						DestroyWindow(lpdtci->mChildHwnd);

					delete lpdtci;
				}
			}
		}

		Dcx::dcxTabCtrl_DeleteAllItems(m_Hwnd);
	}

	// xdid -a [NAME] [ID] [SWITCH] [N] [ICON] [TEXT][TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)[TAB](TOOLTIP)
	if (xflags[TEXT('a')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto data(input.getfirsttok(1, TSTABCHAR).trim());

		TString control_data;
		TString tooltip;

		if (const auto nToks = input.numtok(TSTABCHAR); nToks > 1)
		{
			control_data = input.getnexttok(TSTABCHAR).trim();	// tok 2

			if (nToks > 2)
				tooltip = input.getlasttoks().trim();	// tok 3, -1, TSTAB
		}

		const auto nIndex = data.getfirsttok(4).to_int() - 1;
		const auto iIcon = data.getnexttok().to_int() - 1;	// tok 5

		// Itemtext
		TString itemtext;
		if (data.numtok() > 5)
			itemtext = data.getlasttoks();	// tok 6, -1

		addTab(nIndex, iIcon, itemtext, control_data, tooltip);
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (xflags[TEXT('c')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 && nItem >= getTabCount())
			throw DcxExceptions::dcxInvalidItem();

		TabCtrl_SetCurSel(m_Hwnd, nItem);
		this->activateSelectedTab();
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (xflags[TEXT('d')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		// if a valid item to delete
		if (nItem < 0 && nItem >= getTabCount())
			throw DcxExceptions::dcxInvalidItem();

		const auto curSel = Dcx::dcxTabCtrl_GetCurSel(m_Hwnd);
		TCITEM tci{};

		tci.mask = TCIF_PARAM;

		if (getTab(nItem, &tci))
		{
			if (auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam); lpdtci)
			{
				if (lpdtci->mChildHwnd && IsWindow(lpdtci->mChildHwnd))
					DestroyWindow(lpdtci->mChildHwnd);

				delete lpdtci;
			}
		}

		Dcx::dcxTabCtrl_DeleteItem(m_Hwnd, nItem);

		// select the next tab item if its the current one
		if (const auto iTotal = getTabCount(); ((curSel == nItem) && (iTotal > 0)))
		{
			if (nItem < iTotal)
				TabCtrl_SetCurSel(m_Hwnd, nItem);
			else
				TabCtrl_SetCurSel(m_Hwnd, iTotal - 1);	// nItem -1

			this->activateSelectedTab();
		}
	}
	// xdid -l [NAME] [ID] [SWITCH] [N] [ICON]
	else if (xflags[TEXT('l')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4
		const auto nIcon = input.getnexttok().to_int() - 1;	// tok 5

		if (nItem < 0 && nItem >= getTabCount())
			throw DcxExceptions::dcxInvalidItem();

		TCITEM tci{};
		tci.mask = TCIF_IMAGE;
		tci.iImage = nIcon;

		TabCtrl_SetItem(m_Hwnd, nItem, &tci);
	}
	// xdid -m [NAME] [ID] [SWITCH] [X] [Y]
	else if (xflags[TEXT('m')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto X = input.getnexttok().to_int();	// tok 4
		const auto Y = input.getnexttok().to_int();	// tok 5

		TabCtrl_SetItemSize(m_Hwnd, X, Y);
	}
	// This it to avoid an invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (xflags[TEXT('r')])
	{
	}
	// xdid -t [NAME] [ID] [SWITCH] [N] (text) ($chr(9) (tooltip text))
	else if (xflags[TEXT('t')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 && nItem >= getTabCount())
			throw DcxExceptions::dcxInvalidItem();

		const TString tsArgs(input.getlasttoks().trim());

		TString itemtext(tsArgs.getfirsttok(1, TSTABCHAR).trim());
		const TString tsTooltip(tsArgs.getnexttok(TSTABCHAR).trim());

		TCITEM tci{};
		tci.mask = TCIF_TEXT;
		tci.pszText = itemtext.to_chr();

		TabCtrl_SetItem(m_Hwnd, nItem, &tci);

		tci.mask = TCIF_PARAM;
		if (getTab(nItem, &tci))
		{
			if (const auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam); lpdtci)
			{
				lpdtci->tsTipText = tsTooltip;
			}
		}
	}
	// xdid -v [DNAME] [ID] [SWITCH] [N] [POS]
	else if (xflags[TEXT('v')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;		// tok 4
		const auto pos = input.getnexttok().to_int() - 1;	// tok 5

		if (nItem < 0 || nItem >= getTabCount())
			throw DcxExceptions::dcxInvalidItem();

		if (pos < 0 || pos >= getTabCount())
			throw DcxExceptions::dcxInvalidItem();

		if (nItem == pos)
			return;

		auto curSel = Dcx::dcxTabCtrl_GetCurSel(m_Hwnd); // make zero based
		if (curSel == nItem)
			curSel = pos;
		else if (curSel > nItem)
			curSel--;

		// get the item we're moving
		auto text = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);

		TCITEM tci{ (TCIF_IMAGE | TCIF_PARAM | TCIF_TEXT | TCIF_STATE), 0,0,text.get(), MIRC_BUFFER_SIZE_CCH, 0, 0 };

		if (!getTab(nItem, &tci))
			throw DcxExceptions::dcxInvalidItem();

		// remove the old tab item
		if (!Dcx::dcxTabCtrl_DeleteItem(m_Hwnd, nItem))
			throw DcxExceptions::dcxInvalidItem();

		// insert it into the new position
		if (TabCtrl_InsertItem(m_Hwnd, pos, &tci) == -1)
			throw DcxExceptions::dcxInvalidItem();

		// select the next tab item if its the current one
		if (curSel >= 0)
		{
			TabCtrl_SetCurSel(m_Hwnd, curSel);

			this->activateSelectedTab();
		}
	}
	// xdid -w [NAME] [ID] [SWITCH] [FLAGS] [N,N2-N3,N4...] [FILENAME]
	else if (xflags[TEXT('w')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto flag(input.getnexttok());	// tok 4
		const auto tsIndex(input.getnexttok());	// tok 5
		auto filename(input.getlasttoks());		// tok 6, -1

		this->loadIcon(flag, tsIndex, filename);
	}
	// xdid -y [NAME] [ID] [SWITCH] ([+FLAGS] (args))
	else if (xflags[TEXT('y')])
	{
		if (input.numtok() > 3)
		{
			const XSwitchFlags xFlags(input.getnexttok());

			if (xFlags[TEXT('d')])
			{
				if (const auto i = input.getnexttokas<int>() - 1; i >= 0)
					TabCtrl_RemoveImage(m_Hwnd, i);
			}
		}
		else {
			if (auto himl = setImageList(nullptr); himl)
				ImageList_Destroy(himl);
		}
	}
	// xdid -u [NAME] [ID] [SWITCH] ([+FLAGS] (args))
	else if (xflags[TEXT('u')])
	{
		if (input.numtok() > 3)
		{
			const XSwitchFlags xFlags(input.getnexttok());

			if (xFlags[TEXT('a')])
				updateAllTab();
			if (xFlags[TEXT('s')])
				activateSelectedTab();
			if (xFlags[TEXT('t')])
				activateTab(input.getnexttokas<int>());
		}
		else {
			updateAllTab();
			activateSelectedTab();
		}
	}
	// xdid -M [NAME] [ID] [SWITCH] [+FLAGS] [WIDTH]
	// xdid -M -> [NAME] [ID] -M [+FLAGS] [WIDTH]
	else if (xflags[TEXT('M')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const XSwitchFlags xFlags(input.getnexttok());	// tok 4
		auto iWidth = input.getnexttok().to_int();		// tok 5

		if (iWidth < -1)
			iWidth = -1;

		TabCtrl_SetMinTabWidth(m_Hwnd, iWidth);

		this->activateSelectedTab();
	}
	// xdid -P [NAME] [ID] [SWITCH] [+FLAGS] [ARGS]
	// xdid -P -> [NAME] [ID] -M [+FLAGS] [ARGS]
	else if (xflags[TEXT('P')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		//if (!m_hPeek)
		//	throw DcxExceptions::dcxUnableToCreateWindow();

		// +b = background colour [ARGS] = RGB colour
		// +c = Cache bitmaps.
		// +d = Description text colour [ARGS] = RGB colour
		// +e = enable/disable peek ability [ARGS] = 1/0
		// +f = font [ARGS] = [+FLAGS] [CHARSET] [SIZE] [FONTNAME]
		// +m = min size [ARGS] = minx miny
		// +M = max size [ARGS] = maxx maxy
		// +o = hover open delay [ARGS] = delay time in milliseconds (default is system setting which is usually 400)
		// +r = rounded window [ARGS] = 1/0
		// +t = Title text colour [ARGS] = RGB colour

		const XSwitchFlags xFlags(input.getnexttok());
		const auto tsArgs(input.getlasttoks());

		pkData pkd{};

		if (xFlags[L'r'])
		{
			pkd.m_dwMask |= PCF_ROUNDED;
			pkd.m_bRoundedWindow = (tsArgs.to_<int>() > 0);
		}
		else if (xFlags[L'm'])
		{
			pkd.m_dwMask |= PCF_MIN;

			pkd.m_szMin.cx = tsArgs.getfirsttok(1).to_<LONG>();
			pkd.m_szMin.cy = tsArgs.getnexttokas<LONG>();
		}
		else if (xFlags[L'M'])
		{
			pkd.m_dwMask |= PCF_MAX;

			pkd.m_szMax.cx = tsArgs.getfirsttok(1).to_<LONG>();
			pkd.m_szMax.cy = tsArgs.getnexttokas<LONG>();
		}
		else if (xFlags[L'f'])
		{
			if (LOGFONT lf{}; ParseCommandToLogfont(tsArgs, &lf))
			{
				auto hTmp = m_hPeekFont;
				m_hPeekFont = CreateFontIndirect(&lf);
				if (m_hPeekFont)
				{
					SetWindowFont(m_hPeek, m_hPeekFont, FALSE);
					if (hTmp)
						DeleteFont(hTmp);
				}
			}
		}
		else if (xFlags[L'b'])
		{
			pkd.m_dwMask |= PCF_BKGCOLOUR;

			pkd.m_clrBkg = tsArgs.getfirsttok(1).to_<COLORREF>();
		}
		else if (xFlags[L'c'])
		{
			pkd.m_dwMask |= PCF_EXSTTYLE;

			pkd.m_uexstyle = PeekCtrl_GetExtendedStyle(m_hPeek);

			if (tsArgs.getfirsttok(1).to_<int>() > 0)
				pkd.m_uexstyle |= PCS_CACHE_BITMAPS;
			else
				pkd.m_uexstyle &= ~PCS_CACHE_BITMAPS;
		}
		else if (xFlags[L't'])
		{
			pkd.m_dwMask |= PCF_TITLECOLOUR;

			pkd.m_clrTitle = tsArgs.getfirsttok(1).to_<COLORREF>();
		}
		else if (xFlags[L'd'])
		{
			pkd.m_dwMask |= PCF_DESCCOLOUR;

			pkd.m_clrDescription = tsArgs.getfirsttok(1).to_<COLORREF>();
		}
		else if (xFlags[L'o'])
		{
			m_iHoverDelay = tsArgs.getfirsttok(1).to_<UINT>();
		}
		else if (xFlags[L'e'])
		{
			if (m_hPeek)
			{
				DestroyWindow(m_hPeek);
				m_hPeek = nullptr;
			}

			m_bPeek = (tsArgs.getfirsttok(1).to_<int>() > 0);

			if (m_bPeek)
				CreatePeek();
		}
		if (m_hPeek)
			SendMessage(m_hPeek, PC_WM_SETDATA, reinterpret_cast<WPARAM>(&pkd), 0);
	}
	else
		this->parseGlobalCommandRequest(input, xflags);
}

HIMAGELIST DcxTab::getImageList() const noexcept
{
	return TabCtrl_GetImageList(m_Hwnd);
}

HIMAGELIST DcxTab::setImageList(const HIMAGELIST himl) noexcept
{
	return TabCtrl_SetImageList(m_Hwnd, himl);
}

void DcxTab::deleteLParamInfo(const int nItem) const noexcept
{
	TCITEM tci{ TCIF_PARAM, 0,0,nullptr, 0, 0, 0 };

	if (getTab(nItem, &tci))
	{
		const auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam);

		delete lpdtci;
	}
}

void DcxTab::activateSelectedTab()
{
	HidePeek();
	const auto nSel = Dcx::dcxTabCtrl_GetCurSel(m_Hwnd);
	activateTab(nSel);
}

void DcxTab::activateTab(int nSel)
{
	auto nTab = getTabCount();
	//const auto nSel = TabCtrl_GetCurSel(m_Hwnd);

	if (nTab <= 0)
		return;

	RECT tabrect{};
	if (!GetWindowRect(m_Hwnd, &tabrect))
		return;

	Dcx::dcxTabCtrl_AdjustRect(m_Hwnd, false, &tabrect);

	RECT rc{};
	if (!GetWindowRect(m_Hwnd, &rc))
		return;

	OffsetRect(&tabrect, -rc.left, -rc.top);

	TCITEM tci{ TCIF_PARAM, 0,0,nullptr, 0, 0, 0 };

	auto hdwp = BeginDeferWindowPos(nTab + 1);

	if (!hdwp)
		throw Dcx::dcxException("activateSelectedTab() - Unable to size tabs");

	auto hTemp = hdwp;

	while (nTab-- > 0)
	{
		if (!getTab(nTab, &tci))
			continue;

		if (auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam); lpdtci)
		{
			if (auto hSelChild = lpdtci->mChildHwnd; (hSelChild && IsWindow(hSelChild)))
			{
				if (nTab == nSel)
				{
					hTemp = DeferWindowPos(hdwp, hSelChild, nullptr,
						tabrect.left, tabrect.top, tabrect.right - tabrect.left, tabrect.bottom - tabrect.top,
						SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOOWNERZORDER);

					if (!hTemp)
						break;
					hdwp = hTemp;

					hTemp = DeferWindowPos(hTemp, hSelChild, nullptr,
						tabrect.left, tabrect.top, tabrect.right - tabrect.left, tabrect.bottom - tabrect.top,
						SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_DRAWFRAME | SWP_FRAMECHANGED);
				}
				else {
					hTemp = DeferWindowPos(hdwp, hSelChild, nullptr,
						0, 0, 0, 0,
						SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
				}
				if (!hTemp)
					break;
				hdwp = hTemp;
			}
		}
	}
	if (hdwp)
		EndDeferWindowPos(hdwp);
}

void DcxTab::updateAllTab()
{
	auto nTab = getTabCount();
	//const auto nSel = TabCtrl_GetCurSel(m_Hwnd);

	if (nTab <= 0)
		return;

	RECT tabrect{};
	if (!GetWindowRect(m_Hwnd, &tabrect))
		return;

	Dcx::dcxTabCtrl_AdjustRect(m_Hwnd, false, &tabrect);

	RECT rc{};
	if (!GetWindowRect(m_Hwnd, &rc))
		return;

	OffsetRect(&tabrect, -rc.left, -rc.top);

	TCITEM tci{ TCIF_PARAM, 0,0,nullptr, 0, 0, 0 };

	auto hdwp = BeginDeferWindowPos(nTab + 1);

	if (!hdwp)
		throw Dcx::dcxException("updateAllTab() - Unable to size tabs");

	auto hTemp = hdwp;

	while (nTab-- > 0)
	{
		if (!getTab(nTab, &tci))
			continue;

		if (auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam); lpdtci)
		{
			if (auto hSelChild = lpdtci->mChildHwnd; (hSelChild && IsWindow(hSelChild)))
			{
				hTemp = DeferWindowPos(hdwp, hSelChild, nullptr,
					tabrect.left, tabrect.top, tabrect.right - tabrect.left, tabrect.bottom - tabrect.top,
					SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOOWNERZORDER);

				if (!hTemp)
					break;
				hdwp = hTemp;

				hTemp = DeferWindowPos(hTemp, hSelChild, nullptr,
					tabrect.left, tabrect.top, tabrect.right - tabrect.left, tabrect.bottom - tabrect.top,
					SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_DRAWFRAME | SWP_FRAMECHANGED);

				if (!hTemp)
					break;
				hdwp = hTemp;
			}
		}
	}
	if (hdwp)
		EndDeferWindowPos(hdwp);
}

bool DcxTab::getTab(const int index, const LPTCITEM tcItem) const noexcept
{
	if (!m_Hwnd)
		return false;

	return (TabCtrl_GetItem(m_Hwnd, index, tcItem) != FALSE);
}

int DcxTab::getTabCount() const noexcept
{
	if (!m_Hwnd)
		return -1;

	return TabCtrl_GetItemCount(m_Hwnd);
}

DcxControl* DcxTab::addTab(int nIndex, int iIcon, const TString& tsText, const TString& tsCtrl, const TString& tsTooltip)
{
	if (!m_Hwnd)
		return nullptr;

	TCITEM tci{};
	tci.mask = TCIF_IMAGE | TCIF_PARAM;
	tci.iImage = iIcon;

	if (nIndex < 0)
		nIndex = getTabCount();

	auto lpdtci = std::make_unique<DCXTCITEM>();

	lpdtci->tsTipText = tsTooltip;

	// Itemtext
	if (!tsText.empty())
	{
		tci.mask |= TCIF_TEXT;
		tci.pszText = const_cast<TCHAR*>(tsText.to_chr());
	}

	DcxControl* p_Control{};

	if (tsCtrl.numtok() > 5)
	{
		if (const auto pd = this->getParentDialog(); pd)
		{
			p_Control = pd->addControl(tsCtrl, 1,
				DcxAllowControls::ALLOW_TREEVIEW |
				DcxAllowControls::ALLOW_LISTVIEW |
				DcxAllowControls::ALLOW_RICHEDIT |
				DcxAllowControls::ALLOW_DIVIDER |
				DcxAllowControls::ALLOW_PANEL |
				DcxAllowControls::ALLOW_TAB |
				DcxAllowControls::ALLOW_REBAR |
				DcxAllowControls::ALLOW_WEBCTRL |
				DcxAllowControls::ALLOW_WEB2CTRL |
				DcxAllowControls::ALLOW_EDIT |
				DcxAllowControls::ALLOW_IMAGE |
				DcxAllowControls::ALLOW_LIST, m_Hwnd);

			lpdtci->mChildHwnd = p_Control->getHwnd();
		}
	}
	tci.lParam = reinterpret_cast<LPARAM>(lpdtci.release());

	TabCtrl_InsertItem(m_Hwnd, nIndex, &tci);

	// Ook: this fixes the incorrectly positioned controls on additional tabs.
	this->activateTab(nIndex);
	this->activateSelectedTab();

	return p_Control;
}

const TString DcxTab::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);
	const auto ExStyles = dcxGetWindowExStyle(m_Hwnd);

	if (dcx_testflag(Styles, TCS_VERTICAL))
	{
		styles.addtok(TEXT("vertical"));
		if (dcx_testflag(Styles, TCS_RIGHT))
			styles.addtok(TEXT("right"));
	}
	else if (dcx_testflag(Styles, TCS_BOTTOM))
		styles.addtok(TEXT("bottom"));
	if (dcx_testflag(Styles, TCS_FIXEDWIDTH))
		styles.addtok(TEXT("fixedwidth"));
	if (dcx_testflag(Styles, TCS_BUTTONS))
		styles.addtok(TEXT("buttons"));
	if (dcx_testflag(Styles, TCS_FLATBUTTONS))
		styles.addtok(TEXT("flat"));
	if (dcx_testflag(Styles, TCS_HOTTRACK))
		styles.addtok(TEXT("hot"));
	if (dcx_testflag(Styles, TCS_MULTILINE))
		styles.addtok(TEXT("multiline"));
	if (dcx_testflag(Styles, TCS_RIGHTJUSTIFY))
		styles.addtok(TEXT("rightjustify"));
	if (dcx_testflag(Styles, TCS_SCROLLOPPOSITE))
		styles.addtok(TEXT("scrollopposite"));
	if (dcx_testflag(ExStyles, TCS_EX_FLATSEPARATORS))
		styles.addtok(TEXT("flatseps"));
	if (this->m_bClosable)
		styles.addtok(TEXT("closable"));
	if (this->m_bPeek)
		styles.addtok(TEXT("peek"));
	//if (this->m_bGradient)
	//	styles.addtok(TEXT("gradient"));
	return styles;
}

void DcxTab::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());

	if (auto himl = this->getImageList(); himl)
		xmlSaveImageList(himl, xml, L"+B"_ts);

	const auto count = this->getTabCount();
	auto buf = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);
	TCITEM tci{};

	for (auto i = decltype(count){0}; i < count; ++i)
	{
		tci.cchTextMax = MIRC_BUFFER_SIZE_CCH - 1;
		tci.pszText = buf.get();
		tci.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
		if (getTab(i, &tci))
		{
			TiXmlElement xTab("item");

			if (dcx_testflag(tci.mask, TCIF_TEXT))
				xTab.SetAttribute("text", TString(tci.pszText).c_str());
			if (dcx_testflag(tci.mask, TCIF_IMAGE) && (tci.iImage >= 0))	// -1 means no image
				xTab.SetAttribute("icon", tci.iImage + 1);

			if (const auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam); lpdtci)
			{
				if (const auto* const ctrl = this->getParentDialog()->getControlByHWND(lpdtci->mChildHwnd); ctrl)
				{
					TiXmlElement xChild("control");

					ctrl->toXml(&xChild);

					// we need to remove hidden style here
					if (TString styles(xChild.Attribute("styles")); !styles.empty())
					{
						styles.remtok(TEXT("hidden"), 1);

						if (!styles.empty())
							xChild.SetAttribute("styles", styles.c_str());
						else
							xChild.RemoveAttribute("styles");
					}
					xTab.InsertEndChild(xChild);
				}
			}

			xml->InsertEndChild(xTab);
		}
	}
}

TiXmlElement* DcxTab::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxTab::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;
	const auto pd = getParentDialog();
	if (!pd)
		return;

	__super::fromXml(xDcxml, xThis);

	int nIndex{};

	// <item >
	for (auto xItem = xThis->FirstChildElement("item"); xItem; xItem = xItem->NextSiblingElement("item"))
	{
		const TString tsText(queryAttribute(xItem, "text"));
		const TString tsFlags(queryAttribute(xItem, "flags", "+"));
		const TString tsTooltip(queryAttribute(xItem, "tooltip"));
		const auto iImage = queryIntAttribute(xItem, "icon") - 1;
		TString tsCtrl;

		// <control ...>
		if (auto xCtrl = xItem->FirstChildElement("control"); xCtrl)
		{
			// found control for tab
			TString tsID(queryAttribute(xCtrl, "id"));
			auto szType = queryAttribute(xCtrl, "type");
			auto szStyles = queryAttribute(xCtrl, "styles");
			auto iWidth = queryIntAttribute(xCtrl, "width");
			auto iHeight = queryIntAttribute(xCtrl, "height");

			// ID is NOT a number!
			if (tsID.empty()) // no id, generate one.
				tsID.addtok(getParentDialog()->getUniqueID());

			// fixed position control, no cla
			//xdid -a DNAME ID [N] [ICON] (TEXT) [TAB] [CID] [CONTROL] [X] [Y] [W] [H] (OPTIONS) [TAB] (TOOLTIP)
			_ts_sprintf(tsCtrl, TEXT("% % 0 0 % % %"), tsID, szType, iWidth, iHeight, szStyles);

			if (auto pCtrl = addTab(nIndex, iImage, tsText, tsCtrl, tsTooltip); pCtrl)
			{
				pCtrl->fromXml(xDcxml, xCtrl);
			}
		}
		else
			addTab(nIndex, iImage, tsText, tsCtrl, tsTooltip);

		++nIndex;
	}
}

LRESULT DcxTab::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
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
		case NM_RCLICK:
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			{
				if (const auto tab = HitTestOnItem(); tab != -1)
					this->execAliasEx(TEXT("rclick,%u,%d"), getUserID(), tab + 1);
			}

			bParsed = TRUE;
			break;
		}

		case NM_CLICK:
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			{
				if (const auto tab = TabCtrl_GetCurFocus(m_Hwnd); tab != -1)
				{
					if (CloseButtonHitTest(tab))
						execAliasEx(TEXT("closetab,%u,%d"), getUserID(), tab + 1);
					else
						execAliasEx(TEXT("sclick,%u,%d"), getUserID(), tab + 1);
				}
			}
		}
		// fall through.
		[[fallthrough]];
		case TCN_SELCHANGE:
		{
			this->activateSelectedTab();
			bParsed = TRUE;
		}
		break;
		default:
			break;
		}
		break;
	}

	// Original source based on code from eMule 0.47 source code available at http://www.emule-project.net
	case WM_DRAWITEM:
	{
		//if (!m_bClosable)
		//	break;

		dcxlParam(LPDRAWITEMSTRUCT, idata);

		if ((!idata) || (!IsWindow(idata->hwndItem)))
			break;

		if (!idata->hDC)
			break;

		RECT rect{};
		const auto nTabIndex = gsl::narrow_cast<int>(idata->itemID);
		const bool bCurSel = (Dcx::dcxTabCtrl_GetCurSel(m_Hwnd) == nTabIndex);

		CopyRect(&rect, &idata->rcItem);

		const auto savedDC = SaveDC(idata->hDC);
		Auto(RestoreDC(idata->hDC, savedDC));

		if (!bCurSel && !isStyle(WindowStyle::TCS_Buttons))
		{
			// Ook: This is a hack to fix the gap left between a non-selected tab & the tab area when NOT TCS_BUTTONS
			rect.bottom += DcxDPIModule::dcxGetWindowMetrics(m_Hwnd, SM_CYEDGE); // 2
		}

		if (this->m_bGradientFill)
		{
			if (this->m_TextOptions.m_clrTextBackground == CLR_INVALID)
				// Gives a nice silver/gray gradient
				XPopupMenuItem::DrawGradient(idata->hDC, &rect, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNFACE), !this->m_bGradientVertical);
			else
				XPopupMenuItem::DrawGradient(idata->hDC, &rect, GetSysColor(COLOR_BTNHIGHLIGHT), this->m_TextOptions.m_clrTextBackground, !this->m_bGradientVertical);
		}
		else
			DcxControl::DrawCtrlBackground(idata->hDC, this, &rect);

		rect.left += 1 + DcxDPIModule::dcxGetWindowMetrics(m_Hwnd, SM_CXEDGE); // move in past border.
		rect.top += 1 + DcxDPIModule::dcxGetWindowMetrics(m_Hwnd, SM_CYEDGE); //4;

		TCHAR szLabel[MIRC_BUFFER_SIZE_CCH]{};

		TCITEM tci{ TCIF_TEXT | TCIF_IMAGE | TCIF_STATE, 0, TCIS_HIGHLIGHTED, &szLabel[0], MIRC_BUFFER_SIZE_CCH, 0, 0 };

		if (!getTab(nTabIndex, &tci))
		{
			showError(nullptr, TEXT("DcxTab Fatal Error"), TEXT("Invalid item"));
			break;
		}

		const TString label(tci.pszText);	// copy buffer, this may not be the same buffer as provided by us.

		// set transparent so text background isnt annoying
		SetBkMode(idata->hDC, TRANSPARENT);

		// Draw icon on left side if the item has an icon
		if (tci.iImage != -1)
		{
			if (ImageList_DrawEx(getImageList(), tci.iImage, idata->hDC, rect.left, rect.top, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT))
			{
				int iSizeX = 0, iSizeY = 0;
				if (ImageList_GetIconSize(getImageList(), &iSizeX, &iSizeY))
					rect.left += iSizeX;
			}
		}
		// Draw 'Close button' at right side
		if (m_bClosable)
		{
			RECT rcCloseButton(GetCloseButtonRect(rect));

			// Draw systems close button ? or do you want a custom close button?
			DrawFrameControl(idata->hDC, &rcCloseButton, DFC_CAPTION, DFCS_CAPTIONCLOSE | DFCS_FLAT | DFCS_TRANSPARENT);

			rect.right = rcCloseButton.left - 2;
		}

		//DrawGlow(nTabIndex, idata->hDC, rect);

		if (dcx_testflag(tci.dwState, TCIS_HIGHLIGHTED))
			SetTextColor(idata->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));

		uint32_t uDrawFlags = DT_WORD_ELLIPSIS | DT_SINGLELINE;

		if (isStyle(WindowStyle::TCS_ForceLeftAlign))
		{
			// force text to be left aligned
			rect.left += DcxDPIModule::dcxGetWindowMetrics(m_Hwnd, SM_CXEDGE);	//5 add padding for left of text, make this a settable option for text alignment.
		}
		else {
			// center text on control (default)
			uDrawFlags |= DT_CENTER;
		}
		//if (dcx_testflag(dcxGetWindowStyle(m_Hwnd), TCS_HOTTRACK))
		//	m_TextOptions.m_bGlow = (nTabIndex == this->m_iHotItem);

		this->ctrlDrawText(idata->hDC, label, &rect, uDrawFlags);
		break;
	}

#if DCX_DEBUG_OUTPUT
	// NB: WM_MEASUREITEM is only called for ownerdrawn fixed width tabs (& only called once)
	case WM_MEASUREITEM:
	{
		//if (!m_bClosable)
		   // break;
		auto mis = (LPMEASUREITEMSTRUCT)lParam;
		mis->itemHeight = mis->itemHeight;
	}
	break;
#endif

	default:
		break;
	}

	return 0L;
}

LRESULT DcxTab::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	LRESULT lRes = 0L;
	switch (uMsg)
	{
	case WM_CONTEXTMENU:
	case WM_LBUTTONUP:
		break;

	case WM_MOUSEMOVE:
	{
		if (const auto pDialog = getParentDialog(); pDialog)
			pDialog->setMouseControl(getUserID());

		//m_bHover = false;
		m_iHoverItem = -1;

		if (!m_bTracking)
			m_bTracking = TrackMouseEvents(TME_LEAVE | TME_HOVER, m_iHoverDelay);
		if (!m_bHot)
		{
			m_bHot = true;
			//InvalidateRect(m_Hwnd, nullptr, FALSE);
		}
		const POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		if (const auto iTab = Dcx::dcxTabCtrl_GetPointItem(m_Hwnd, pt); iTab >= 0)
		{
			if (iTab != this->m_iHotItem)
			{
				RECT rcItem{};
				//if (this->m_iHotItem >= 0)
				//{
				//	Dcx::dcxTabCtrl_GetItemRect(m_Hwnd, this->m_iHotItem, &rcItem);
				//	InvalidateRect(m_Hwnd, &rcItem, FALSE);
				//}
				this->m_iHotItem = iTab;

				Dcx::dcxTabCtrl_GetItemRect(m_Hwnd, iTab, &rcItem);
				//InvalidateRect(m_Hwnd, &rcItem, FALSE);

				SetPeekSource(iTab, Dcx::dcxTabCtrl_GetCurSel(m_Hwnd), &rcItem);

				if (m_bHover)
				{
					RECT rcPeek{ rcItem };
					OffsetRect(&rcPeek, 0, (rcPeek.bottom - rcPeek.top));
					MapWindowRect(m_Hwnd, nullptr, &rcPeek);
					ShowPeek(rcPeek.left, rcPeek.top);
				}
			}
		}
	}
	break;

	case WM_MOUSEHOVER:
	{
		if (!m_bHover && m_bTracking)
		{
			m_bHover = true;
			m_bTracking = false;

			//InvalidateRect(m_Hwnd, nullptr, FALSE);

			const POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			if (const auto iTab = Dcx::dcxTabCtrl_GetPointItem(m_Hwnd, pt); iTab >= 0)
			{
				if (iTab != this->m_iHoverItem)
				{
					this->m_iHoverItem = iTab;

					RECT rcItem{};
					Dcx::dcxTabCtrl_GetItemRect(m_Hwnd, iTab, &rcItem);

					// dropdown peek window...
					PeekCtrl_ResetCache(m_hPeek);

					RECT rcPeek{ rcItem };
					OffsetRect(&rcPeek, 0, (rcPeek.bottom - rcPeek.top));
					MapWindowRect(m_Hwnd, nullptr, &rcPeek);
					ShowPeek(rcPeek.left, rcPeek.top);
				}
			}
		}
	}
	break;

	case WM_MOUSELEAVE:
	{
		if (m_bTracking)
		{
			m_bHover = false;
			m_bTracking = false;
			m_bHot = false;

			if (const auto iTab = m_iHotItem; iTab >= 0)
			{
				RECT rcItem{};
				Dcx::dcxTabCtrl_GetItemRect(m_Hwnd, iTab, &rcItem);
				InvalidateRect(m_Hwnd, &rcItem, FALSE);
			}

			m_iHotItem = -1;
			m_iHoverItem = -1;
			//InvalidateRect(m_Hwnd, nullptr, FALSE);

			PeekCtrl_ResetCache(m_hPeek);
		}
		HidePeek();
	}
	break;

	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (!hdr)
			break;

		//if (hdr->hwndFrom == this->m_ToolTipHWND) {
		//	switch(hdr->code) {
		//	case TTN_GETDISPINFO:
		//		{
		//			auto di = (LPNMTTDISPINFO)lParam;
		//			di->lpszText = this->m_tsToolTip.to_chr();
		//			di->hinst = nullptr;
		//			bParsed = TRUE;
		//		}
		//		break;
		//	case TTN_LINKCLICK:
		//		{
		//			bParsed = TRUE;
		//			this->execAliasEx(TEXT("%s,%d"), TEXT("tooltiplink"), this->getUserID());
		//		}
		//		break;
		//	default:
		//		break;
		//	}
		//}

		if (IsWindow(hdr->hwndFrom))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(hdr->hwndFrom, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_HSCROLL:
	case WM_VSCROLL:
	{
		//if (!IsWindow(to_hwnd(lParam)))
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
				this->execAliasEx(TEXT("scroll,%u,%u"), getUserID(), wParam);
		}
	}
	[[fallthrough]];
	case WM_COMMAND:
	{
		if (IsWindow(to_hwnd(lParam)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(to_hwnd(lParam), TEXT("dcx_cthis")); c_this)
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

	case WM_SIZE:
	{
		this->activateSelectedTab();
		if (dcx_testflag(getEventMask(), DCX_EVENT_SIZE))
			this->execAliasEx(TEXT("sizing,%u"), getUserID());
	}
	break;

	case WM_ERASEBKGND:
	{
		if (this->isExStyle(WindowExStyle::Transparent))
			this->DrawParentsBackground(reinterpret_cast<HDC>(wParam));
		else
			DcxControl::DrawCtrlBackground(reinterpret_cast<HDC>(wParam), this);
		bParsed = TRUE;
		return TRUE;
	}
	break;

	case WM_PAINT:
	{
		//if (!this->IsAlphaBlend())
		//	break;
		//
		//PAINTSTRUCT ps{};
		//auto hdc = BeginPaint(m_Hwnd, &ps);
		//Auto(EndPaint(m_Hwnd, &ps));
		//
		//bParsed = TRUE;
		//
		//// Setup alpha blend if any.
		//const auto ai = this->SetupAlphaBlend(&hdc);
		//Auto(this->FinishAlphaBlend(ai));
		//
		//lRes = CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);

		//CleanUpParentCache();

		if (!this->IsAlphaBlend())
			break;

		bParsed = TRUE;

		if (!wParam)
		{
			PAINTSTRUCT ps{};

			auto hdc = BeginPaint(m_Hwnd, &ps);
			Auto(EndPaint(m_Hwnd, &ps));

			return DrawClientArea(hdc, uMsg, lParam);
		}
		else
			return DrawClientArea(reinterpret_cast<HDC>(wParam), uMsg, lParam);
	}
	break;

	//case WM_CLOSE:
	//	{
	//		if (GetKeyState(VK_ESCAPE) != 0) // don't allow the window to close if escape is pressed. Needs looking into for a better method.
	//			bParsed = TRUE;
	//	}
	//	break;

	case WM_DESTROY:
	{
		this->CallDefaultClassProc(uMsg, wParam, lParam);

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

#if DCX_USE_GDIPLUS
void DcxTab::DrawGlow(const int nTabIndex, HDC hDC, const RECT& rect) const
{
	//const Dcx::dcxCursorPos cPos(m_Hwnd);
	//TCHITTESTINFO hInfo{ {cPos.x, cPos.y}, TCHT_ONITEM };

	//if ((TabCtrl_HitTest(m_Hwnd, &hInfo) == nTabIndex) /*&& isStyle(TCS_HOTTRACK) &&*/ && (TabCtrl_GetCurSel(m_Hwnd) != nTabIndex))

	//if ((HitTestOnItem() == nTabIndex) /*&& isStyle(TCS_HOTTRACK) &&*/ && (TabCtrl_GetCurSel(m_Hwnd) != nTabIndex))

	if ((HitTestOnItem() == nTabIndex))
	{
		Gdiplus::Graphics graphics(hDC);
		Gdiplus::GraphicsPath graphicsPath;
		const Gdiplus::Rect gRect(rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top));
		constexpr int radius = 5;

		//rect - for a bounding rect
		//radius - for how 'rounded' the glow will look
		constexpr int diameter = radius * 2;

		graphicsPath.AddArc(Gdiplus::Rect(gRect.X, gRect.Y, diameter, diameter), 180.0f, 90.0f);
		graphicsPath.AddArc(Gdiplus::Rect(gRect.X + gRect.Width - diameter, gRect.Y, diameter, diameter), 270.0f, 90.0f);
		graphicsPath.AddArc(Gdiplus::Rect(gRect.X + gRect.Width - diameter, gRect.Y + gRect.Height - diameter, diameter, diameter), 0.0f, 90.0f);
		graphicsPath.AddArc(Gdiplus::Rect(gRect.X, gRect.Y + gRect.Height - diameter, diameter, diameter), 90.0f, 90.0f);
		graphicsPath.CloseFigure();

		Gdiplus::PathGradientBrush brush(&graphicsPath);
		brush.SetCenterColor(Gdiplus::Color(255, 20, 20, 234)); //would be some shade of blue, following your example
		int colCount = 1;
		const Gdiplus::Color clrGlow(0, 20, 20, 234);
		brush.SetSurroundColors(&clrGlow, &colCount); //same as your center color, but with the alpha channel set to 0

		//play with these numbers to get the glow effect you want
		const Gdiplus::REAL blendFactors[] = { 0.0f, 0.1f, 0.3f, 1.0f };
		const Gdiplus::REAL blendPos[] = { 0.0f, 0.4f, 0.6f, 1.0f };
		//sets how transition toward the center is shaped
		brush.SetBlend(&blendFactors[0], &blendPos[0], gsl::narrow_cast<int>(std::size(blendFactors)));
		//sets the scaling on the center. you may want to have it elongated in the x-direction
		brush.SetFocusScales(0.2f, 0.2f);

		graphics.FillPath(&brush, &graphicsPath);
	}
}
#endif

int DcxTab::HitTestOnItem() const noexcept
{
	if (TCHITTESTINFO tchi{ {0,0},TCHT_ONITEM }; GetCursorPos(&tchi.pt))
	{
		MapWindowPoints(nullptr, m_Hwnd, &tchi.pt, 1);

		return TabCtrl_HitTest(m_Hwnd, &tchi);
	}
	return 0;
}

RECT DcxTab::GetCloseButtonRect(const RECT& rcItem) noexcept
{
	//return { ((rcItem.right - GetSystemMetrics(SM_CXEDGE)) - GetSystemMetrics(SM_CXSMICON)), rcItem.top, (rcItem.right - GetSystemMetrics(SM_CXEDGE)), rcItem.top + GetSystemMetrics(SM_CYSMICON) };
	const auto dpi = DcxDPIModule::dcxGetDpiForSystem();
	const auto xEdge = gsl::narrow_cast<long>(DcxDPIModule::dcxGetSystemMetricsForDpi(SM_CXEDGE, dpi));
	const auto szXIcon = gsl::narrow_cast<long>(DcxDPIModule::dcxGetSystemMetricsForDpi(SM_CXSMICON, dpi));
	const auto szYIcon = gsl::narrow_cast<long>(DcxDPIModule::dcxGetSystemMetricsForDpi(SM_CYSMICON, dpi));
	return { ((rcItem.right - xEdge) - szXIcon), rcItem.top, (rcItem.right - xEdge), rcItem.top + szYIcon };
}

LRESULT DcxTab::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}

bool DcxTab::CloseButtonHitTest(const int iTab) const noexcept
{
#if DCX_USE_WRAPPERS
	if (m_bClosable)
	{
		if (const Dcx::dcxCursorPos pt(m_Hwnd); pt)
		{
			if (RECT rc{}; Dcx::dcxTabCtrl_GetItemRect(m_Hwnd, iTab, &rc))
			{
				const RECT rcCloseButton(GetCloseButtonRect(rc));

				return (PtInRect(&rcCloseButton, pt) != FALSE);
			}
		}
	}
	return false;
#else
	if (m_bClosable)
	{
		if (POINT pt{}; GetCursorPos(&pt))
		{
			MapWindowPoints(nullptr, m_Hwnd, &pt, 1);

			if (RECT rc{}; Dcx::dcxTabCtrl_GetItemRect(m_Hwnd, iTab, &rc))
			{
				const RECT rcCloseButton(GetCloseButtonRect(rc));

				return (PtInRect(&rcCloseButton, pt) != FALSE);
			}
		}
	}
	return false;
#endif
}

LRESULT DcxTab::DrawClientArea(HDC hdc, UINT uMsg, LPARAM lParam)
{
	if (!hdc)
		return 0L;

	//CleanUpParentCache();

	// Setup alpha blend if any.
	const auto ai = this->SetupAlphaBlend(&hdc);
	Auto(this->FinishAlphaBlend(ai));

	return CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
}

void DcxTab::CreatePeek() noexcept
{
	auto hHandle = GetModuleHandle(nullptr);

	const RECT rcPeek{ 0,0,200,200 };
	m_hPeek = CreateWindowExW(WS_EX_NOACTIVATE, PEEK_CLASS, nullptr, WS_POPUP | WS_CLIPCHILDREN, rcPeek.left, rcPeek.top, (rcPeek.right - rcPeek.left), (rcPeek.bottom - rcPeek.top), m_Hwnd, nullptr, hHandle, nullptr);
	if (m_hPeek)
	{
		//PeekCtrl_SetExtendedStyle(m_hPeek, PCS_CACHE_BITMAPS);

		const SIZE szMin{ 100, 30 };
		const SIZE szMax{ 200, 200 };
		PeekCtrl_SetMinMax(m_hPeek, &szMin, &szMax);
	}
}

void DcxTab::SetPeekSource(int iTab, _In_ int iTabSel, LPCRECT rcItem) const noexcept
{
	if (!m_bPeek || !m_hPeek || !rcItem || iTab < 0)
		return;

	//SetPeekPos((iTab != iTabSel), rcItem);

	pkData pkd{};

	pkd.m_dwMask = PCF_HWND | PCF_DESC | PCF_TITLE;

	TCITEM tci{};
	tci.mask = TCIF_PARAM | TCIF_TEXT;
	tci.pszText = &pkd.m_Title[0];
	tci.cchTextMax = gsl::narrow_cast<decltype(tci.cchTextMax)>(std::size(pkd.m_Title));

	if (this->getTab(iTab, &tci))
	{
		if (const auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam); lpdtci)
		{
			//auto hFont = GetWindowFont(m_Hwnd);
			if (auto hFont = this->getFont(); hFont)
				SetWindowFont(m_hPeek, hFont, FALSE);

			if (tci.pszText)
				_ts_strcpyn(&pkd.m_Title[0], tci.pszText, std::size(pkd.m_Title));

			_ts_strcpyn(&pkd.m_Description[0], lpdtci->tsTipText.to_chr(), std::size(pkd.m_Description));

			if (iTab != iTabSel)
				pkd.m_hSrc = lpdtci->mChildHwnd;

			PeekCtrl_SetData(m_hPeek, &pkd);
		}
	}
}

void DcxTab::ShowPeek(int x, int y) const noexcept
{
	if (!m_bPeek || !m_hPeek)
		return;

	PeekCtrl_Show(m_hPeek, true, x, y);
}

void DcxTab::HidePeek() const noexcept
{
	if (!m_hPeek)
		return;

	ShowWindow(m_hPeek, SW_HIDE);
	PeekCtrl_RemoveSource(m_hPeek);
}
