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
	: DcxControl(ID, p_Dialog)
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

	if (!IsWindow(m_Hwnd))
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	if (this->m_bClosable)
	{
		//RECT rcClose;
		//GetCloseButtonRect(*rc, rcClose);
		//TabCtrl_SetPadding(m_Hwnd, (rcClose.right - rcClose.left), GetSystemMetrics(SM_CXFIXEDFRAME));

		TabCtrl_SetPadding(m_Hwnd, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYFIXEDFRAME));
	}
	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxTab::~DcxTab() noexcept
{
	ImageList_Destroy(this->getImageList());

	const auto nItems = TabCtrl_GetItemCount(m_Hwnd);
	for (auto n = decltype(nItems){0}; n < nItems; ++n)
		this->deleteLParamInfo(n);
}

/*!
 * \brief blah
 *
 * blah
 */

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
		case L"gradient"_hash:
			m_bGradient = true;
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

		if (nItem < 0 || nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw DcxExceptions::dcxInvalidItem();

		TCITEM tci{ TCIF_TEXT,0,0, szReturnValue, MIRC_BUFFER_SIZE_CCH,0,0 };

		TabCtrl_GetItem(m_Hwnd, nItem, &tci);
	}
	break;
	case L"num"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), TabCtrl_GetItemCount(m_Hwnd));
		break;
		// [NAME] [ID] [PROP] [N]
	case L"icon"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iTab = input.getnexttok().to_int() - 1;		// tok 4

		if (iTab < 0 || iTab >= TabCtrl_GetItemCount(m_Hwnd))
			throw DcxExceptions::dcxInvalidItem();

		TCITEM tci{ TCIF_IMAGE,0U,0U, nullptr, 0, 0, 0 };

		TabCtrl_GetItem(m_Hwnd, iTab, &tci);

		_ts_snprintf(szReturnValue, TEXT("%d"), tci.iImage + 1);
	}
	break;
	case L"sel"_hash:
	{
		const auto nItem = TabCtrl_GetCurSel(m_Hwnd);

		if (nItem < 0 || nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw DcxExceptions::dcxInvalidItem();

		_ts_snprintf(szReturnValue, TEXT("%d"), nItem + 1);
	}
	break;
	case L"seltext"_hash:
	{
		const auto nItem = TabCtrl_GetCurSel(m_Hwnd);

		if (nItem < 0 || nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw DcxExceptions::dcxInvalidItem();

		TCITEM tci{ TCIF_TEXT,0U,0U, szReturnValue, MIRC_BUFFER_SIZE_CCH, 0, 0 };

		TabCtrl_GetItem(m_Hwnd, nItem, &tci);
	}
	break;
	case L"childid"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 || nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw DcxExceptions::dcxInvalidItem();

		TCITEM tci{ TCIF_PARAM,0U,0U, nullptr, 0, 0, 0 };

		if (TabCtrl_GetItem(m_Hwnd, nItem, &tci))
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
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTab::parseCommandRequest(const TString& input)
{
	const XSwitchFlags xflags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (xflags[TEXT('r')])
	{
		TCITEM tci{};
		const auto nItems = TabCtrl_GetItemCount(m_Hwnd);

		for (auto n = decltype(nItems){0}; n < nItems; ++n)
		{
			ZeroMemory(&tci, sizeof(TCITEM));

			tci.mask = TCIF_PARAM;

			if (TabCtrl_GetItem(m_Hwnd, n, &tci))
			{
				if (auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam); lpdtci != nullptr && lpdtci->mChildHwnd != nullptr && IsWindow(lpdtci->mChildHwnd))
				{
					DestroyWindow(lpdtci->mChildHwnd);
					delete lpdtci;
				}
			}
		}

		TabCtrl_DeleteAllItems(m_Hwnd);
	}

	// xdid -a [NAME] [ID] [SWITCH] [N] [ICON] [TEXT][TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)[TAB](TOOLTIP)
	if (xflags[TEXT('a')])
	{
		if (numtok < 5)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		TCITEM tci{};
		tci.mask = TCIF_IMAGE | TCIF_PARAM;

		const auto data(input.getfirsttok(1, TSTABCHAR).trim());

		TString control_data;
		TString tooltip;
		const auto nToks = input.numtok(TSTABCHAR);

		if (nToks > 1)
		{
			control_data = input.getnexttok(TSTABCHAR).trim();	// tok 2

			if (nToks > 2)
				tooltip = input.getlasttoks().trim();	// tok 3, -1, TSTAB
		}

		auto nIndex = data.getfirsttok(4).to_int() - 1;

		if (nIndex < 0)
			nIndex = TabCtrl_GetItemCount(m_Hwnd);

		tci.iImage = data.getnexttok().to_int() - 1;	// tok 5

		//auto lpdtci = new DCXTCITEM;
		auto lpdtci = std::make_unique<DCXTCITEM>();

		lpdtci->tsTipText = tooltip;

		// Itemtext
		TString itemtext;
		if (data.numtok() > 5)
		{
			itemtext = data.getlasttoks();	// tok 6, -1
			tci.mask |= TCIF_TEXT;

			//if (this->m_bClosable)
			//{
			//	//itemtext += TEXT("***");	// TEXT("   ");
			//	RECT rc;
			//	GetCloseButtonRect(this->m_rc, rc);
			//	TabCtrl_SetPadding(m_Hwnd, 0, 0);
			//}

			tci.pszText = itemtext.to_chr();
		}

		if (control_data.numtok() > 5)
		{
			const DcxControl* const p_Control = this->getParentDialog()->addControl(control_data, 1,
				DcxAllowControls::ALLOW_TREEVIEW |
				DcxAllowControls::ALLOW_LISTVIEW |
				DcxAllowControls::ALLOW_RICHEDIT |
				DcxAllowControls::ALLOW_DIVIDER |
				DcxAllowControls::ALLOW_PANEL |
				DcxAllowControls::ALLOW_TAB |
				DcxAllowControls::ALLOW_REBAR |
				DcxAllowControls::ALLOW_WEBCTRL |
				DcxAllowControls::ALLOW_EDIT |
				DcxAllowControls::ALLOW_IMAGE |
				DcxAllowControls::ALLOW_LIST, m_Hwnd);

			lpdtci->mChildHwnd = p_Control->getHwnd();
		}
		tci.lParam = reinterpret_cast<LPARAM>(lpdtci.release());

		TabCtrl_InsertItem(m_Hwnd, nIndex, &tci);
		this->activateSelectedTab();
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (xflags[TEXT('c')])
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			//throw Dcx::dcxException("Invalid Item");
			throw DcxExceptions::dcxInvalidItem();

		TabCtrl_SetCurSel(m_Hwnd, nItem);
		this->activateSelectedTab();
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (xflags[TEXT('d')])
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		// if a valid item to delete
		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			//throw Dcx::dcxException("Invalid Item");
			throw DcxExceptions::dcxInvalidItem();

		const auto curSel = TabCtrl_GetCurSel(m_Hwnd);
		TCITEM tci{};

		tci.mask = TCIF_PARAM;

		if (TabCtrl_GetItem(m_Hwnd, nItem, &tci))
		{
			if (auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam); lpdtci && lpdtci->mChildHwnd && IsWindow(lpdtci->mChildHwnd))
			{
				DestroyWindow(lpdtci->mChildHwnd);
				delete lpdtci;
			}
		}

		TabCtrl_DeleteItem(m_Hwnd, nItem);

		// select the next tab item if its the current one
		if (const auto iTotal = TabCtrl_GetItemCount(m_Hwnd); ((curSel == nItem) && (iTotal > 0)))
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
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4
		const auto nIcon = input.getnexttok().to_int() - 1;	// tok 5

		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			//throw Dcx::dcxException("Invalid Item");
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
			//throw Dcx::dcxException("Insufficient parameters");
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
	// xdid -t [NAME] [ID] [SWITCH] [N] (text)
	else if (xflags[TEXT('t')])
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			//throw Dcx::dcxException("Invalid Item");
			throw DcxExceptions::dcxInvalidItem();

		TString itemtext;

		if (numtok > 4)
			itemtext = input.getlasttoks().trim();	// tok 5, -1

		TCITEM tci{};
		tci.mask = TCIF_TEXT;
		tci.pszText = itemtext.to_chr();

		TabCtrl_SetItem(m_Hwnd, nItem, &tci);
	}
	// xdid -v [DNAME] [ID] [SWITCH] [N] [POS]
	else if (xflags[TEXT('v')])
	{
		if (numtok < 5)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;		// tok 4
		const auto pos = input.getnexttok().to_int() - 1;	// tok 5
		bool adjustDelete = false;

		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			//throw Dcx::dcxException("Invalid Item");
			throw DcxExceptions::dcxInvalidItem();

		if (pos < 0 && pos >= TabCtrl_GetItemCount(m_Hwnd))
			//throw Dcx::dcxException("Invalid Item");
			throw DcxExceptions::dcxInvalidItem();

		if (nItem == pos)
			return;

		// does the nItem index get shifted after we insert
		if (nItem > pos)
			adjustDelete = true;

		auto curSel = TabCtrl_GetCurSel(m_Hwnd);
		if (curSel == nItem)
			curSel = pos;
		else if (curSel > nItem)
			curSel--;

		// get the item we're moving
		auto text = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);

		TCITEM tci{ (TCIF_IMAGE | TCIF_PARAM | TCIF_TEXT | TCIF_STATE), 0,0,text.get(), MIRC_BUFFER_SIZE_CCH, 0, 0 };

		TabCtrl_GetItem(m_Hwnd, nItem, &tci);

		// insert it into the new position
		TabCtrl_InsertItem(m_Hwnd, pos, &tci);

		// remove the old tab item
		TabCtrl_DeleteItem(m_Hwnd, (adjustDelete ? nItem + 1 : nItem));

		// select the next tab item if its the current one
		if (curSel >= 0)
		{
			TabCtrl_SetCurSel(m_Hwnd, curSel);

			this->activateSelectedTab();
		}
	}
	// xdid -w [NAME] [ID] [SWITCH] [FLAGS] [INDEX] [FILENAME]
	else if (xflags[TEXT('w')])
	{
		if (numtok < 6)
			//throw Dcx::dcxException("Insufficient parameters");
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
			throw Dcx::dcxException("Unable to get Image List");

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
	else if (xflags[TEXT('y')])
	{
		ImageList_Destroy(this->getImageList());
	}
	// xdid -m [NAME] [ID] [SWITCH] [+FLAGS] [WIDTH]
	// xdid -m -> [NAME] [ID] -m [+FLAGS] [WIDTH]
	else if (xflags[TEXT('m')])
	{
		if (numtok < 5)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		const XSwitchFlags xFlags(input.getnexttok());	// tok 4
		auto iWidth = input.getnexttok().to_int();		// tok 5

		if (iWidth < -1)
			iWidth = -1;

		TabCtrl_SetMinTabWidth(m_Hwnd, iWidth);

		this->activateSelectedTab();
	}
	else
		this->parseGlobalCommandRequest(input, xflags);
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxTab::getImageList() const noexcept
{
	return TabCtrl_GetImageList(m_Hwnd);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTab::setImageList(const HIMAGELIST himl) noexcept
{
	TabCtrl_SetImageList(m_Hwnd, himl);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTab::deleteLParamInfo(const int nItem) noexcept
{
	TCITEM tci{ TCIF_PARAM, 0,0,nullptr, 0, 0, 0 };

	if (TabCtrl_GetItem(m_Hwnd, nItem, &tci))
	{
		const auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam);

		delete lpdtci;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTab::activateSelectedTab()
{
	auto nTab = TabCtrl_GetItemCount(m_Hwnd);
	const auto nSel = TabCtrl_GetCurSel(m_Hwnd);

	if (nTab <= 0)
		return;

	RECT tabrect{};
	if (!GetWindowRect(m_Hwnd, &tabrect))
		return;

	TabCtrl_AdjustRect(m_Hwnd, FALSE, &tabrect);

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
		if (!TabCtrl_GetItem(m_Hwnd, nTab, &tci))
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

void DcxTab::getTab(const int index, const LPTCITEM tcItem) const noexcept
{
	TabCtrl_GetItem(m_Hwnd, index, tcItem);
}

int DcxTab::getTabCount() const noexcept
{
	return TabCtrl_GetItemCount(m_Hwnd);
}

//void DcxTab::GetCloseButtonRect(const RECT& rcItem, RECT& rcCloseButton)
//{
//	// ----------
//	//rcCloseButton.top = rcItem.top + 2;
//	//rcCloseButton.bottom = rcCloseButton.top + (m_iiCloseButton.rcImage.bottom - m_iiCloseButton.rcImage.top);
//	//rcCloseButton.right = rcItem.right - 2;
//	//rcCloseButton.left = rcCloseButton.right - (m_iiCloseButton.rcImage.right - m_iiCloseButton.rcImage.left);
//	// ----------
//	//rcCloseButton.top = rcItem.top + 2;
//	//rcCloseButton.bottom = rcCloseButton.top + (16);
//	//rcCloseButton.right = rcItem.right - 2;
//	//rcCloseButton.left = rcCloseButton.right - (16);
//	// ----------
//	rcCloseButton.top = rcItem.top;
//	rcCloseButton.bottom = rcCloseButton.top + GetSystemMetrics(SM_CYSMICON);
//	rcCloseButton.right = rcItem.right - GetSystemMetrics(SM_CXEDGE);
//	rcCloseButton.left = rcCloseButton.right - GetSystemMetrics(SM_CXSMICON);
//	// ----------
//}

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
	if (this->m_bGradient)
		styles.addtok(TEXT("gradient"));
	return styles;
}

void DcxTab::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());

	const auto count = this->getTabCount();
	auto buf = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);
	TCITEM tci{};

	for (auto i = decltype(count){0}; i < count; ++i)
	{
		tci.cchTextMax = MIRC_BUFFER_SIZE_CCH - 1;
		tci.pszText = buf.get();
		tci.mask |= TCIF_TEXT;
		if (TabCtrl_GetItem(m_Hwnd, i, &tci))
		{
			if (const auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam); lpdtci)
			{
				if (const auto* const ctrl = this->getParentDialog()->getControlByHWND(lpdtci->mChildHwnd); ctrl)
				{
					auto ctrlxml = ctrl->toXml();
					// we need to remove hidden style here
					if (TString styles(ctrlxml->Attribute("styles")); !styles.empty())
					{
						styles.remtok(TEXT("hidden"), 1);
						if (!styles.empty())
							ctrlxml->SetAttribute("styles", styles.c_str());
						else
							ctrlxml->RemoveAttribute("styles");
					}
					if (dcx_testflag(tci.mask, TCIF_TEXT))
						ctrlxml->SetAttribute("caption", TString(tci.pszText).c_str());
					xml->LinkEndChild(ctrlxml);
				}
			}
		}
	}
}

TiXmlElement* DcxTab::toXml(void) const
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
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			{
				if (const auto tab = HitTestOnItem(); tab != -1)
					this->execAliasEx(TEXT("rclick,%u,%d"), getUserID(), tab + 1);
			}

			bParsed = TRUE;
			break;
		}

		case NM_CLICK:
		{
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
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

		RECT rect{};
		const auto nTabIndex = gsl::narrow_cast<int>(idata->itemID);

		CopyRect(&rect, &idata->rcItem);

		const auto savedDC = SaveDC(idata->hDC);
		Auto(RestoreDC(idata->hDC, savedDC));

		//DcxControl::DrawCtrlBackground(idata->hDC, this, &rect);

		if (this->m_bGradient)
		{
			if (this->m_clrBackText == CLR_INVALID)
				// Gives a nice silver/gray gradient
				XPopupMenuItem::DrawGradient(idata->hDC, &rect, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNFACE), TRUE);
			else
				XPopupMenuItem::DrawGradient(idata->hDC, &rect, GetSysColor(COLOR_BTNHIGHLIGHT), this->m_clrBackText, TRUE);
		}
		else
			DcxControl::DrawCtrlBackground(idata->hDC, this, &rect);

		rect.left += 1 + GetSystemMetrics(SM_CXEDGE); // move in past border.
		rect.top += 1 + GetSystemMetrics(SM_CYEDGE); //4;

		TCHAR szLabel[MIRC_BUFFER_SIZE_CCH]{};

		TCITEM tci{ TCIF_TEXT | TCIF_IMAGE | TCIF_STATE, 0, TCIS_HIGHLIGHTED, &szLabel[0], MIRC_BUFFER_SIZE_CCH, 0, NULL };

		if (!TabCtrl_GetItem(getHwnd(), nTabIndex, &tci))
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
			rect.left += GetSystemMetrics(SM_CXEDGE);	//5 add padding for left of text, make this a settable option for text alignment.
		}
		else {
			// center text on control (default)
			uDrawFlags |= DT_CENTER;
		}
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

	case WM_SIZE:
	{
		this->activateSelectedTab();
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_SIZE))
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
		brush.SetBlend(&blendFactors[0], &blendPos[0], std::size(blendFactors));
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
	return { ((rcItem.right - GetSystemMetrics(SM_CXEDGE)) - GetSystemMetrics(SM_CXSMICON)), rcItem.top, (rcItem.right - GetSystemMetrics(SM_CXEDGE)), rcItem.top + GetSystemMetrics(SM_CYSMICON) };
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
			if (RECT rc{}; TabCtrl_GetItemRect(m_Hwnd, iTab, &rc))
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

			if (RECT rc{}; TabCtrl_GetItemRect(m_Hwnd, iTab, &rc))
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

	// Setup alpha blend if any.
	const auto ai = this->SetupAlphaBlend(&hdc);
	Auto(this->FinishAlphaBlend(ai));

	return CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
}