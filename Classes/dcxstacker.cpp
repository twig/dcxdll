/*!
 * \file dcxstacker.cpp
 * \brief blah
 *
 * blah
 *
 * \author Ook
 * \version 1.0
 *
 * \b Revisions
 *	1.0
 *
 * © ScriptsDB.org - 2006
 */
#include "defines.h"
#include "Classes/dcxstacker.h"
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

DcxStacker::DcxStacker(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WS_EX_CONTROLPARENT,
		DCX_STACKERCLASS,
		ws.m_Styles | WS_CHILD,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	SendMessage(m_Hwnd, CCM_SETVERSION, gsl::narrow_cast<WPARAM>(6), gsl::narrow_cast<LPARAM>(0));

	//this->m_hBackBrush = GetSysColorBrush(COLOR_3DFACE);

	if (p_Dialog->getToolTipHWND())
	{
		if (styles.istok(TEXT("tooltips")))
		{
			setToolTipHWND(p_Dialog->getToolTipHWND());
			AddToolTipToolInfo(this->getToolTipHWND(), m_Hwnd);
		}
	}

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxStacker::~DcxStacker() noexcept
{
	this->clearImageList();
}

void DcxStacker::clearImageList(void) noexcept
{
#ifdef DCX_USE_GDIPLUS
	this->m_vImageList.clear();
#endif
}

void DcxStacker::clearItemList(void) noexcept
{
	this->m_vItems.clear();
}

dcxWindowStyles DcxStacker::parseControlStyles(const TString& tsStyles)
{
	auto ws = parseGeneralControlStyles(tsStyles);

	ws.m_Styles |= LBS_OWNERDRAWVARIABLE | LBS_NOTIFY;
	m_dStyles = STACKERS_COLLAPSE;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"vscroll"_hash:
			ws.m_Styles |= WS_VSCROLL;
			break;
		case L"gradient"_hash:
			m_dStyles |= STACKERS_GRAD;
			break;
		case L"arrows"_hash:
			m_dStyles |= STACKERS_ARROW;
			break;
		case L"nocollapse"_hash:
			m_dStyles &= ~STACKERS_COLLAPSE;
			break;
		case L"float"_hash:
			m_dStyles |= STACKERS_FLOAT;
			break;
		default:
			break;
		}
	}

	return ws;
}

void DcxStacker::loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc)
{
#ifdef DCX_USE_GDIPLUS
	auto filename(tsSrc);

	if (!IsFile(filename))
		throw Dcx::dcxException(TEXT("Unable to Access File: %"), filename);

	m_vImageList.push_back(std::make_unique<Gdiplus::Image>(filename.to_wchr()));
#endif
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxStacker::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	const auto numtok = input.numtok();
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP] [N]
	case L"text"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nSel = input.getnexttok().to_int() - 1;	// tok 4

		if (nSel < 0 && nSel >= ListBox_GetCount(m_Hwnd))
			throw DcxExceptions::dcxInvalidItem();

		auto sitem = this->getItem(nSel);
		if (!sitem || sitem == reinterpret_cast<LPDCXSITEM>(LB_ERR))
			throw Dcx::dcxException("Unable to get Item");

		szReturnValue = sitem->tsCaption.to_chr();
	}
	break;
	// [NAME] [ID] [PROP]
	case L"num"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), ListBox_GetCount(m_Hwnd));
		break;
		// [NAME] [ID] [PROP]
	case L"sel"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), ListBox_GetCurSel(m_Hwnd) + 1);
		break;
		// [NAME] [ID] [PROP] [N]
	case L"haschild"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nSel = input.getnexttok().to_int() - 1;	// tok 4

		if (nSel < 0 && nSel >= ListBox_GetCount(m_Hwnd))
			throw DcxExceptions::dcxInvalidItem();

		const auto* const sitem = this->getItem(nSel);
		if (!sitem || sitem == reinterpret_cast<LPDCXSITEM>(LB_ERR))
			throw Dcx::dcxException("Unable to get Item");

		szReturnValue = dcx_truefalse(sitem->pChild != nullptr);
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"childid"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nSel = input.getnexttok().to_int() - 1;	// tok 4

		if (nSel < 0 && nSel >= ListBox_GetCount(m_Hwnd))
			throw DcxExceptions::dcxInvalidItem();

		const auto* const sitem = this->getItem(nSel);
		if (!sitem || sitem == reinterpret_cast<LPDCXSITEM>(LB_ERR))
			throw Dcx::dcxException("Unable to get Item");

		if (sitem->pChild)
			_ts_snprintf(szReturnValue, TEXT("%u"), sitem->pChild->getUserID());
		else
			szReturnValue = TEXT('0');
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

void DcxStacker::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));

	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		SendMessage(m_Hwnd, LB_RESETCONTENT, 0U, 0);

	//xdid -a -> [NAME] [ID] -a [N] [+FLAGS] [IMAGE] [SIMAGE] [COLOR] [BGCOLOR] Item Text [TAB] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if (flags[TEXT('a')])
	{
		if (numtok < 10)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		auto sitem = std::make_unique<DCXSITEM>();

		const auto item(input.getfirsttok(1, TSTABCHAR).trim());		// tok 1, TSTAB
		const auto ctrl(input.getnexttok(TSTABCHAR).trim());			// tok 2, TSTAB

		auto nPos = item.getfirsttok(4).to_int() - 1;				// tok 4
		const auto flag(item.getnexttok().trim());				// tok 5	?? flag never used ??

		sitem->iItemImg = item.getnexttok().to_int() - 1;			// tok 6
		sitem->iSelectedItemImg = item.getnexttok().to_int() - 1;	// tok 7
		sitem->clrText = item.getnexttok().to_<COLORREF>();		// tok 8
		sitem->clrBack = item.getnexttok().to_<COLORREF>();		// tok 9
		sitem->pChild = nullptr;
		sitem->hFont = nullptr;
		sitem->tsCaption = item.getlasttoks();						// tok 10, -1

		if (nPos < 0)
			nPos = ListBox_GetCount(m_Hwnd);
		if (nPos == LB_ERR)
			nPos = 0;

		if (!ctrl.empty())
		{
			const auto p_Control = this->getParentDialog()->addControl(ctrl, 1, DcxAllowControls::ALLOW_ALL, m_Hwnd);
			sitem->pChild = p_Control;
			ShowWindow(p_Control->getHwnd(), SW_HIDE);
			this->redrawWindow();

			//const auto ID = mIRC_ID_OFFSET + (UINT)ctrl.gettok( 1 ).to_int( );
			//
			//if (!this->getParentDialog()->isIDValid(ID, true))
			//	throw Dcx::dcxException(TEXT("Control with ID \"%\" already exists"), ID - mIRC_ID_OFFSET);
			//
			//try {
			//	auto p_Control = DcxControl::controlFactory(this->m_pParentDialog, ID, ctrl, 2, DcxAllowControls::ALLOW_ALL, m_Hwnd);
			//
			//	this->getParentDialog()->addControl( p_Control );
			//	sitem->pChild = p_Control;
			//	ShowWindow(p_Control->getHwnd(),SW_HIDE);
			//	this->redrawWindow( );
			//}
			//catch (const std::exception &e) {
			//	this->showErrorEx(nullptr, TEXT("-a"), TEXT("Unable To Create Control %d (%S)"), ID - mIRC_ID_OFFSET, e.what());
			//	throw;
			//}
		}
		//if (SendMessage(m_Hwnd,LB_INSERTSTRING, static_cast<WPARAM>(nPos), reinterpret_cast<LPARAM>(sitem.get())) < 0)
		//	throw Dcx::dcxException("Error adding item to control");

		if (ListBox_InsertString(m_Hwnd, nPos, sitem.get()) < 0)
			throw Dcx::dcxException("Error adding item to control");

		sitem.release();
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('c')])
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nPos = input.getnexttok().to_int() - 1;		// tok 4

		if (nPos < 0 && nPos >= ListBox_GetCount(m_Hwnd))
			//throw Dcx::dcxException("Invalid Item");
			throw DcxExceptions::dcxInvalidItem();

		ListBox_SetCurSel(m_Hwnd, nPos);
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nPos = input.getnexttok().to_int() - 1;	// tok 4

		if (nPos < 0 && nPos >= ListBox_GetCount(m_Hwnd))
			//throw Dcx::dcxException("Invalid Item");
			throw DcxExceptions::dcxInvalidItem();

		ListBox_DeleteString(m_Hwnd, nPos);
	}
	// This is to avoid an invalid flag message.
	//xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')])
	{
	}
	//xdid -u [NAME] [ID] [SWITCH]
	else if (flags[TEXT('u')])
	{
		ListBox_SetCurSel(m_Hwnd, -1);
	}
	// xdid -T [NAME] [ID] [SWITCH] [N] (ToolTipText)
	else if (flags[TEXT('T')])
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nPos = input.getnexttok().to_int() - 1;	// tok 4

		if (nPos < 0 && nPos >= ListBox_GetCount(m_Hwnd))
			//throw Dcx::dcxException("Invalid Item");
			throw DcxExceptions::dcxInvalidItem();

		const auto sitem = this->getItem(nPos);
		if (!sitem || sitem == reinterpret_cast<LPDCXSITEM>(LB_ERR))
			throw Dcx::dcxException("Unable to get Item");

		sitem->tsTipText = (numtok > 4 ? input.getlasttoks().trim() : TEXT(""));	// tok 5, -1
	}
	//xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [FILE]
	else if (flags[TEXT('w')])
	{
		//		if (numtok < 5)
		//			throw DcxExceptions::dcxInvalidArguments();
		//
		//#ifdef DCX_USE_GDIPLUS
		//
		//		const auto flag(input.getnexttok());		// tok 4
		//		auto filename(input.getnexttok().trim());	// tok 5
		//
		//		if (!IsFile(filename))
		//			throw Dcx::dcxException(TEXT("Unable to Access File: %"), filename);
		//
		//		m_vImageList.push_back(std::make_unique<Gdiplus::Image>(filename.to_wchr()));
		//#endif

		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto flag(input.getnexttok());		// tok 4
		auto filename(input.getnexttok().trim());	// tok 5

		this->loadIcon(flag, ""_ts, filename);
	}
	//xdid -y [NAME] [ID] [SWITCH]
	else if (flags[TEXT('y')])
	{
		clearImageList();
		redrawWindow();
	}
	else
		parseGlobalCommandRequest(input, flags);
}

int DcxStacker::getItemID(void) const noexcept
{
#if DCX_USE_WRAPPERS
	const Dcx::dcxCursorPos pt(m_Hwnd);
#else
	POINT pt{};
	if (!GetCursorPos(&pt))
		return -1;

	MapWindowPoints(nullptr, m_Hwnd, &pt, 1);
#endif
	return gsl::narrow_cast<int>(Dcx::dcxLOWORD(SendMessage(m_Hwnd, LB_ITEMFROMPOINT, 0, Dcx::dcxMAKELPARAM(pt.x, pt.y))) + 1);
}

int DcxStacker::getSelItemID(void) const noexcept
{
	return ListBox_GetCurSel(m_Hwnd) + 1;
}

DWORD DcxStacker::getItemCount(void) const noexcept
{
	return gsl::narrow_cast<DWORD>(ListBox_GetCount(m_Hwnd));
}

LPDCXSITEM DcxStacker::getItem(const int nPos) const noexcept
{
	return reinterpret_cast<LPDCXSITEM>(ListBox_GetItemData(m_Hwnd, nPos));
}

LPDCXSITEM DcxStacker::getHotItem(void) const noexcept
{
	return getItem(this->getItemID() - 1);
}

void DcxStacker::getItemRect(const int nPos, LPCRECT rc) const noexcept
{
	ListBox_GetItemRect(m_Hwnd, nPos, rc);
}

const TString DcxStacker::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, WS_VSCROLL))
		styles.addtok(TEXT("vscroll"));
	if (dcx_testflag(this->m_dStyles, STACKERS_GRAD))
		styles.addtok(TEXT("gradient"));
	if (dcx_testflag(this->m_dStyles, STACKERS_ARROW))
		styles.addtok(TEXT("arrows"));
	if (!dcx_testflag(this->m_dStyles, STACKERS_COLLAPSE))
		styles.addtok(TEXT("nocollapse"));
	if (dcx_testflag(this->m_dStyles, STACKERS_FLOAT))
		styles.addtok(TEXT("float"));
	return styles;
}

void DcxStacker::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());
}

TiXmlElement* DcxStacker::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxStacker::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);
}

void DcxStacker::DrawAliasedTriangle(const HDC hdc, const LPCRECT rc, const COLORREF clrShape)
{
#ifdef DCX_USE_GDIPLUS
	if (!Dcx::GDIModule.isUseable() || !hdc || !rc)
		return;

	Gdiplus::Graphics gfx(hdc);

	gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	gfx.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
	//gfx.SetCompositingQuality
	Gdiplus::SolidBrush blackBrush(Gdiplus::Color(128, GetRValue(clrShape), GetGValue(clrShape), GetBValue(clrShape)));
	// Create an array of Point objects that define the polygon.
	//Point point1(rc->left, rc->top);
	//Point point2(rc->right, rc->top);
	//Point point3(rc->left + (rc->right - rc->left)/2, rc->bottom);
	//Point points[3] = {point1, point2, point3};
	Gdiplus::Point points[3];
	points[0].X = rc->left;
	points[0].Y = rc->top;
	points[1].X = rc->right;
	points[1].Y = rc->top;
	points[2].X = (rc->left + (rc->right - rc->left) / 2);
	points[2].Y = rc->bottom;
	// Fill the polygon.
	gfx.FillPolygon(&blackBrush, &points[0], gsl::narrow_cast<INT>(std::size(points)));
#endif
}

#ifdef DCX_USE_GDIPLUS
void DcxStacker::DrawItemImage(const HDC hdc, Gdiplus::Image* const img, const LPCRECT rc)
{
	if (!Dcx::GDIModule.isUseable() || !img || !rc || !hdc)
		return;

	Gdiplus::Graphics grphx(hdc);
	grphx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	grphx.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
	const int w = (rc->right - rc->left), h = (rc->bottom - rc->top);
	if ((img->GetWidth() == 1) || (img->GetHeight() == 1))
	{
		// This fixes a GDI+ bug when resizing 1 px images
		// http://www.devnewsgroups.net/group/microsoft.public.dotnet.framework.windowsforms/topic11515.aspx
		grphx.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
		grphx.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
	}
	grphx.DrawImage(img, rc->left, rc->top, w, h);
}
#endif

void DcxStacker::DrawSItem(const LPDRAWITEMSTRUCT idata)
{
	if (!idata || idata->itemID == -1)
		return;

	auto sitem = reinterpret_cast<LPDCXSITEM>(idata->itemData);
	if (!sitem)
		return;

	RECT rcWin{};
	if (!GetClientRect(idata->hwndItem, &rcWin))
		return;

	auto hFont = sitem->hFont;

	if (!hFont)
		hFont = GetWindowFont(idata->hwndItem);

	auto [code, lf] = Dcx::dcxGetObject<LOGFONT>(hFont);
	if (code == 0)
		return;

	// Create temp HDC as drawing buffer.
	// if temp HDC or its bitmap fails to create, use supplied HDC without buffer.
	const auto hBuffer = CreateHDCBuffer(idata->hDC, &rcWin);
	Auto(DeleteHDCBuffer(hBuffer));

	HDC memDC = idata->hDC;

	if (hBuffer)
		memDC = *hBuffer;

	auto rcText = idata->rcItem;
	auto button_base = 0;
	auto Redraw = false;
	auto h = std::max(gsl::narrow_cast<LONG>(MIN_STACK_HEIGHT), lf.lfHeight * 20);

	rcText.bottom = rcText.top + h;
	//rcText.right -= 1;
	if (rcText.bottom > idata->rcItem.bottom)
		rcText.bottom = idata->rcItem.bottom;
	button_base = rcText.bottom;

	// draw button for this item.
	UINT style = DFCS_BUTTONPUSH | DFCS_ADJUSTRECT;
	if (dcx_testflag(idata->itemState, ODS_DISABLED))
		style |= DFCS_INACTIVE;
	if (dcx_testflag(idata->itemState, ODS_SELECTED))
		style |= DFCS_PUSHED;
	//if (dcx_testflag(idata->itemState, ODS_HOTLIGHT))
	//	style |= DFCS_HOT;

	DrawFrameControl(memDC, &rcText, DFC_BUTTON, style);
	//rcText.right -= GetSystemMetrics(SM_CXEDGE); // move in right side past border
	rcText.right -= Dcx::DpiModule.dcxGetWindowMetrics(m_Hwnd, SM_CXEDGE); // move in right side past border

	// fill background colour if any.
	if (dcx_testflag(this->m_dStyles, STACKERS_GRAD))
	{
		auto clrStart = this->m_clrStartGradient;
		auto clrEnd = this->m_clrEndGradient;

		if (clrEnd == CLR_INVALID)
			clrEnd = GetSysColor(COLOR_GRADIENTACTIVECAPTION);
		if (clrStart == CLR_INVALID)
			clrStart = sitem->clrBack;

		if (clrStart == CLR_INVALID)
			clrStart = GetSysColor(COLOR_BTNFACE);

		XPopupMenuItem::DrawGradient(memDC, &rcText, clrStart, clrEnd, true);
	}
	else if (sitem->clrBack != CLR_INVALID)
	{
		SetBkColor(memDC, sitem->clrBack);
		ExtTextOut(memDC, rcText.left, rcText.top, ETO_CLIPPED | ETO_OPAQUE, &rcText, TEXT(""), 0, nullptr);
	}

#ifdef DCX_USE_GDIPLUS
	// draw GDI+ image if any, we draw the image after the  colour fill to allow for alpha in pics.
	if (dcx_testflag(idata->itemState, ODS_SELECTED) && sitem->iSelectedItemImg > -1 && sitem->iSelectedItemImg < gsl::narrow_cast<int>(this->m_vImageList.size()))
		DrawItemImage(memDC, gsl::at(m_vImageList, sitem->iSelectedItemImg).get(), &rcText);
	else if (sitem->iItemImg > -1 && sitem->iItemImg < gsl::narrow_cast<int>(this->m_vImageList.size()))
		DrawItemImage(memDC, gsl::at(m_vImageList, sitem->iItemImg).get(), &rcText);
#endif

	// draw text if any
	if (!sitem->tsCaption.empty())
	{
		const auto oldFont = Dcx::dcxSelectObject(memDC, hFont);
		Auto(Dcx::dcxSelectObject(memDC, oldFont));

		// get text colour.
		auto clrText = sitem->clrText, oldClr = gsl::narrow_cast<COLORREF>(CLR_INVALID);
		if (clrText == CLR_INVALID)
			clrText = GetSysColor(COLOR_BTNTEXT);
		// draw the text
		if (clrText != CLR_INVALID)
			oldClr = SetTextColor(memDC, clrText);

		ctrlDrawText(memDC, sitem->tsCaption, &rcText, DT_CENTER | DT_END_ELLIPSIS);

		if (oldClr != CLR_INVALID)
			SetTextColor(memDC, oldClr);
	}
	// draw arrows if wanted.
	if (dcx_testflag(this->m_dStyles, STACKERS_ARROW))
	{
		auto rcArrow = rcText;
		rcArrow.left = rcArrow.right - 10;
		rcArrow.top += 3;
		rcArrow.bottom -= 3;
		DrawAliasedTriangle(memDC, &rcArrow, 0);
	}
	if (idata->hDC != memDC)
	{ // if using temp buffer HDC, render completed item to main HDC & cleanup buffer.
		BitBlt(idata->hDC, idata->rcItem.left, idata->rcItem.top, (idata->rcItem.right - idata->rcItem.left), h, memDC, idata->rcItem.left, idata->rcItem.top, SRCCOPY);
	}

	// position child control if any.
	if (sitem->pChild)
	{
		if (const auto sWnd = sitem->pChild->getHwnd(); IsWindow(sWnd))
		{
			if (dcx_testflag(idata->itemState, ODS_SELECTED))
			{
				if (RECT rc{}; GetWindowRect(sWnd, &rc))
				{
					SetWindowPos(sWnd, nullptr, idata->rcItem.left, button_base, (idata->rcItem.right - idata->rcItem.left), (rc.bottom - rc.top), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
					h += (rc.bottom - rc.top);
				}
				if (sWnd != this->m_hActive)
				{
					if (IsWindow(this->m_hActive))
						Redraw = true;
				}
				this->m_hActive = sWnd;
			}
			else
				SetWindowPos(sWnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
		}
	}
	else if (IsWindow(m_hActive) && dcx_testflag(idata->itemState, ODS_SELECTED))
	{
		m_hActive = nullptr;
		Redraw = true;
	}
	if (h != (idata->rcItem.bottom - idata->rcItem.top))
	{
		ListBox_SetItemHeight(idata->hwndItem, idata->itemID, h);

		//MEASUREITEMSTRUCT mi;
		//SCROLLINFO sc;
		//UINT scroll_height = 0, cnt, items = this->getItemCount(), minitemheight = -1;
		//BOOL bParsed = FALSE;
		//cnt = items;
		//mi.CtlID = this->getID();
		//mi.CtlType = ODT_LISTBOX;
		//while (cnt > 0) {
		//	mi.itemID = cnt-1;
		//	mi.itemData = (ULONG_PTR)this->getItem(cnt-1);
		//	this->ParentMessage(WM_MEASUREITEM, (WPARAM)mi.CtlID, (LPARAM)&mi, bParsed);
		//	if (bParsed) {
		//		if (mi.itemHeight < minitemheight)
		//			minitemheight = mi.itemHeight;
		//		scroll_height += mi.itemHeight;
		//		bParsed = FALSE;
		//	}
		//	cnt--;
		//}
		//if (minitemheight == 0)
		//	minitemheight = 1;

		//ZeroMemory(&sc, sizeof(sc));
		//sc.cbSize = sizeof(sc);
		//sc.fMask = SIF_RANGE;
		//sc.nMin = 0;
		//sc.nMax = items + (h / minitemheight) -1;
		//SetScrollInfo(m_Hwnd, SB_VERT, &sc, FALSE);
		Redraw = true;
	}
	if (Redraw)
		redrawWindow();
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxStacker::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	LRESULT lRes = 0L;
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		bParsed = TRUE;
		switch (Dcx::dcxHIWORD(wParam))
		{
		case LBN_DBLCLK:
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
				execAliasEx(TEXT("dclick,%u,%d"), getUserID(), getSelItemID());
		}
		break;
		case LBN_SELCHANGE:
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
				this->execAliasEx(TEXT("sclick,%u,%d"), getUserID(), getSelItemID());
		}
		break;
		default:
			break;
		}
	}
	break;
	case WM_COMPAREITEM:
	{
		bParsed = TRUE;
		//LPCOMPAREITEMSTRUCT idata = (LPCOMPAREITEMSTRUCT)lParam;
	}
	break;

	case WM_DELETEITEM:
	{
		dcxlParam(LPDELETEITEMSTRUCT, idata);
		if (!idata)
			break;

		auto sitem = reinterpret_cast<LPDCXSITEM>(idata->itemData);
		if (!sitem)
			break;

		bParsed = TRUE;
		lRes = TRUE;
		if (sitem->pChild)
		{
			if (const auto dct = sitem->pChild->getControlType(); dct == DcxControlTypes::DIALOG || dct == DcxControlTypes::WINDOW)
				delete sitem->pChild;
			else { //if ( sitem->pChild->getRefCount( ) == 0 ) {
				if (const auto pd = this->getParentDialog(); pd)
					pd->deleteControl(sitem->pChild); // remove from internal list!
				if (IsWindow(sitem->pChild->getHwnd()))
					DestroyWindow(sitem->pChild->getHwnd());
			}
		}
		delete sitem;
	}
	break;

	case WM_MEASUREITEM:
	{
		dcxlParam(LPMEASUREITEMSTRUCT, idata);
		if (!idata)
			break;

		auto sitem = reinterpret_cast<LPDCXSITEM>(idata->itemData);
		if (!sitem)
			break;

		bParsed = TRUE;
		lRes = TRUE;

		auto hFont = sitem->hFont;

		RECT rcText{};
		if (!GetClientRect(m_Hwnd, &rcText))
			break;

		auto h = gsl::narrow_cast<long>(MIN_STACK_HEIGHT), w = (rcText.right - rcText.left);

		if (!hFont)
			hFont = GetWindowFont(m_Hwnd);

		auto [code, lf] = Dcx::dcxGetObject<LOGFONT>(hFont);
		if (code == 0)
			break;

		h = std::max(gsl::narrow_cast<LONG>(h), lf.lfHeight * 20);

		if (sitem->pChild)
		{
			if (const auto win = sitem->pChild->getHwnd(); IsWindow(win) && IsWindowVisible(win))
			{
				if (RECT rc{}; GetWindowRect(win, &rc))
				{
					h += (rc.bottom - rc.top);
					//w = max(w,(rc.right - rc.left));
				}
			}
		}
		idata->itemHeight = gsl::narrow_cast<UINT>(h);
		idata->itemWidth = gsl::narrow_cast<UINT>(w);
	}
	break;

	case WM_DRAWITEM:
	{
		bParsed = TRUE;
		lRes = TRUE;
		DrawSItem(reinterpret_cast<LPDRAWITEMSTRUCT>(lParam));
	}
	break;
	default:
		break;
	}
	return lRes;
}

LRESULT DcxStacker::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	LRESULT lRes = 0L;
	switch (uMsg)
	{
	case WM_GETDLGCODE:
	{
		bParsed = TRUE;
		return DLGC_WANTALLKEYS;
	}
	break;

	case WM_HSCROLL:
		[[fallthrough]];
	case WM_VSCROLL:
		[[fallthrough]];
		//{
		//	if (((HWND) lParam == nullptr) && this->IsAlphaBlend()) {
		//		bParsed = TRUE;
		//		//SendMessage(m_Hwnd,WM_SETREDRAW,FALSE,nullptr);
		//		LRESULT res = CallWindowProc( this->m_DefaultWindowProc, m_Hwnd, uMsg, wParam, lParam );
		//		//SendMessage(m_Hwnd,WM_SETREDRAW,TRUE,nullptr);
		//		//InvalidateRect(m_Hwnd, nullptr, FALSE);
		//		//this->redrawWindow();
		//		//RECT rc;
		//		//HWND hParent = GetParent(m_Hwnd);
		//		//GetWindowRect(m_Hwnd, &rc);
		//		//MapWindowPoints(nullptr, hParent, (LPPOINT)&rc, 2);
		//		//InvalidateRect(hParent, &rc, FALSE);
		//		//RedrawWindow(m_Hwnd,0,0,RDW_INVALIDATE|RDW_ALLCHILDREN|RDW_NOERASE);
		//		//RedrawWindow(m_Hwnd,0,0,RDW_INTERNALPAINT|RDW_FRAME|RDW_INVALIDATE|RDW_ALLCHILDREN);
		//		return res;
		//	}
		//}

	case WM_COMMAND:
	{
		if (IsWindow(to_hwnd(lParam)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(to_hwnd(lParam), TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_COMPAREITEM:
	{
		if (dcxlParam(LPCOMPAREITEMSTRUCT, idata); (idata) && (IsWindow(idata->hwndItem)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(idata->hwndItem, TEXT("dcx_cthis")); c_this)
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

	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (!hdr)
			break;

		if (hdr->hwndFrom == getToolTipHWND())
		{
			switch (hdr->code)
			{
			case TTN_GETDISPINFO:
			{
				dcxlParam(LPNMTTDISPINFO, di);
				if (!di)
					break;

				const auto sitem = getHotItem();
				if (!sitem || sitem == reinterpret_cast<LPDCXSITEM>(LB_ERR))
					break;

				di->lpszText = sitem->tsTipText.to_chr();
				di->hinst = nullptr;
				bParsed = TRUE;
			}
			break;
			case TTN_LINKCLICK:
			{
				bParsed = TRUE;
				this->execAliasEx(TEXT("tooltiplink,%u,%d"), getUserID(), getItemID());
			}
			break;
			default:
				break;
			}
		}
		else if (m_Hwnd != hdr->hwndFrom)
		{
			if (IsWindow(hdr->hwndFrom))
			{
				if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(hdr->hwndFrom, TEXT("dcx_cthis")); c_this)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
	}
	break;

	case WM_LBUTTONDBLCLK:
		break;

	case WM_LBUTTONUP:
	{
		if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			this->execAliasEx(TEXT("lbup,%u,%d"), getUserID(), this->getItemID());
	}
	break;

	case WM_ENABLE:
	{
		this->redrawWindow();
	}
	break;

	case WM_ERASEBKGND:
	{
		bParsed = TRUE;
		// fill background.
		auto lastID = getItemCount();
		if (lastID == LB_ERR)
			break;
		lastID--;
		if ((lastID == 0) || (lParam != 0))
			DrawParentsBackground(reinterpret_cast<HDC>(wParam));
		else {
			if (RECT rcClient{}; GetClientRect(m_Hwnd, &rcClient))
			{
				RECT rcItem{};
				getItemRect(lastID, &rcItem);
				if (rcItem.bottom < rcClient.bottom)
				{
					rcClient.top = rcItem.bottom;
					DrawParentsBackground(reinterpret_cast<HDC>(wParam), &rcClient);
				}

				//lastID = this->getSelItemID();
				//if (lastID != LB_ERR) {
				//	getItemRect(lastID, &rcItem);
				//	DrawParentsBackground((HDC) wParam, &rcItem);
				//}
			}
		}
		return TRUE;
	}
	break;

	case WM_PAINT:
	{
		if (!IsAlphaBlend())
			break;

		PAINTSTRUCT ps{};
		auto hdc = BeginPaint(m_Hwnd, &ps);
		Auto(EndPaint(m_Hwnd, &ps));

		bParsed = TRUE;

		// Setup alpha blend if any.
		auto ai = SetupAlphaBlend(&hdc);
		Auto(FinishAlphaBlend(ai));

		lRes = CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
	}
	break;

	case WM_DESTROY:
	{
		delete this;
		bParsed = TRUE;
	}
	break;
	default:
		lRes = CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return lRes;
}

LRESULT DcxStacker::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
