/*!
 * \file xpopupmenuitem.cpp
 * \brief blah
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.0
 *
 * \b Revisions
 *
 * � ScriptsDB.org - 2006
 */
#include "defines.h"
#include "Classes/xpopup/xpopupmenuitem.h"
#include "Classes/xpopup/xpopupmenu.h"
#include "Dcx.h"

 /// <summary>
 /// Construct a menu item.
 /// </summary>
 /// <param name="Parent"></param>
 /// <param name="bSep"></param>
 /// <param name="dwDataBackup"></param>
XPopupMenuItem::XPopupMenuItem(XPopupMenu* Parent, const bool bSep, ULONG_PTR dwDataBackup) noexcept
	: m_pXParentMenu(Parent), m_bSep(bSep), m_nIcon(-1), m_dwItemDataBackup(dwDataBackup)
{
}

/// <summary>
/// Construct a menu item.
/// </summary>
/// <param name="Parent"></param>
/// <param name="tsItemText"></param>
/// <param name="bSubMenu"></param>
/// <param name="dwDataBackup"></param>
XPopupMenuItem::XPopupMenuItem(XPopupMenu* Parent, const TString& tsItemText, const bool bSubMenu, ULONG_PTR dwDataBackup)
	: m_pXParentMenu(Parent), m_tsItemText(tsItemText), m_bSubMenu(bSubMenu), m_dwItemDataBackup(dwDataBackup)
{
	m_tsItemText.trim();

	parseItemText();
}

/// <summary>
/// Construct a menu item.
/// </summary>
/// <param name="Parent"></param>
/// <param name="tsItemText"></param>
/// <param name="nIcon"></param>
/// <param name="bSubMenu"></param>
/// <param name="dwDataBackup"></param>
XPopupMenuItem::XPopupMenuItem(XPopupMenu* Parent, const TString& tsItemText, const int nIcon, const bool bSubMenu, ULONG_PTR dwDataBackup)
	: m_pXParentMenu(Parent), m_tsItemText(tsItemText), m_nIcon(nIcon), m_bSubMenu(bSubMenu), m_dwItemDataBackup(dwDataBackup)
{
	m_tsItemText.trim();

	parseItemText();
}

/// <summary>
/// Construct a menu item.
/// </summary>
/// <param name="Parent"></param>
/// <param name="tsItemText"></param>
/// <param name="tsTooltip"></param>
/// <param name="nIcon"></param>
/// <param name="bSubMenu"></param>
/// <param name="dwDataBackup"></param>
XPopupMenuItem::XPopupMenuItem(XPopupMenu* Parent, const TString& tsItemText, const TString& tsTooltip, const int nIcon, const bool bSubMenu, ULONG_PTR dwDataBackup)
	: m_pXParentMenu(Parent), m_tsItemText(tsItemText), m_tsTooltipText(tsTooltip), m_nIcon(nIcon), m_bSubMenu(bSubMenu), m_dwItemDataBackup(dwDataBackup)
{
	m_tsItemText.trim();
	m_tsTooltipText.trim();

	parseItemText();
}

/// <summary>
/// Set this items submenu.
/// </summary>
/// <param name="bSubMenu"></param>
void XPopupMenuItem::setSubMenu(const bool bSubMenu) noexcept
{
	this->m_bSubMenu = bSubMenu;
}

/// <summary>
/// Set this items text.
/// </summary>
/// <param name="tsItemText"></param>
void XPopupMenuItem::setItemText(const TString& tsItemText)
{
	if (!this->m_bSep)
		this->m_tsItemText = tsItemText;
}

/// <summary>
/// Set this items icon.
/// </summary>
/// <param name="nIcon"></param>
void XPopupMenuItem::setItemIcon(const int nIcon) noexcept
{
	if (!this->m_bSep)
		this->m_nIcon = nIcon;
}

/// <summary>
/// Set this items overriding style.
/// </summary>
/// <param name="uStyle"></param>
void XPopupMenuItem::setOverrideStyle(UINT uStyle) noexcept
{
	m_eStyleOverride = uStyle;
}

/// <summary>
/// Get this items text.
/// </summary>
/// <returns></returns>
const TString& XPopupMenuItem::getItemText() const noexcept
{
	return this->m_tsItemText;
}

/// <summary>
/// Get this items icon.
/// </summary>
/// <returns></returns>
const int& XPopupMenuItem::getItemIcon() const noexcept
{
	return this->m_nIcon;
}

/// <summary>
/// Get this items current style.
/// </summary>
/// <returns></returns>
auto XPopupMenuItem::getStyle() const noexcept
{
	if (m_eStyleOverride != gsl::narrow_cast<UINT>(XPopupMenu::MenuStyle::XPMS_None))
		return gsl::narrow_cast<XPopupMenu::MenuStyle>(m_eStyleOverride);

	if (m_pXParentMenu)
		return m_pXParentMenu->getStyle();

	return XPopupMenu::MenuStyle::XPMS_None;
}

UINT XPopupMenuItem::getStyle2() const noexcept
{
	if (m_eStyleOverride != gsl::narrow_cast<UINT>(XPopupMenu::MenuStyle::XPMS_None))
		return m_eStyleOverride;

	if (m_pXParentMenu)
		return gsl::narrow_cast<UINT>(m_pXParentMenu->getStyle());

	return gsl::narrow_cast<UINT>(XPopupMenu::MenuStyle::XPMS_None);
}

/// <summary>
/// Is tooltips enabled for this menu?
/// </summary>
/// <returns></returns>
bool XPopupMenuItem::IsTooltipsEnabled() const noexcept
{
	if (!m_pXParentMenu)
		return false;
	return m_pXParentMenu->IsToolTipsEnabled();
}

/// <summary>
/// Parse the item text for any embeded commands/effects.
/// </summary>
/// <returns></returns>
bool XPopupMenuItem::parseItemText()
{
	if (const auto typeHash = m_pXParentMenu->getNameHash(); ((typeHash != TEXT("mirc"_hash)) && (typeHash != TEXT("mircbar"_hash)) && (typeHash != TEXT("dialog"_hash))))
		return false;

	// handle icons
	if (constexpr TCHAR sepChar = TEXT('\v'); m_tsItemText.numtok(sepChar) > 1)	// 11
	{
		m_nIcon = m_tsItemText.getfirsttok(1, sepChar).to_int() - 1;	// tok 1, TEXT('\v')	get embeded icon number if any

		if (m_tsItemText.numtok(sepChar) > 2)	// 11
		{
			// second \v token taken to be style info for item. Overrides the menus style.
			if (const TString tsStyle(m_tsItemText.getnexttok(sepChar).trim()); !tsStyle.empty())
				setOverrideStyle(gsl::narrow_cast<UINT>(getParentMenu()->parseStyle(tsStyle)));
		}
		m_tsItemText = m_tsItemText.getlasttoks().trim();				// tok last, TEXT('\v')	get real item text
	}
	// handles tooltips (conflict with right hand text tab(9) char, changed to char(1))
	if (constexpr TCHAR sepChar = 1; m_tsItemText.numtok(sepChar) > 1)
	{
		const TString tsTmp(m_tsItemText);						// copy item text
		m_tsItemText = tsTmp.getfirsttok(1, sepChar).trim();	// tok 1, get real item text
		m_tsTooltipText = tsTmp.getlasttoks().trim();			// tok 2-, get tooltip text
	}
	// check for $chr(14), this sets the item as a special item that can be selected without closing the menu.
	// this can optionally be followed by an ID number which is used in the callback alias.
	if (const auto nPos = m_tsItemText.find(L'\x0e', 1); nPos != -1)	// $chr(14)
	{
		ptrdiff_t nEnd{ nPos + 1 };
		while (m_tsItemText[nEnd] >= L'0' && m_tsItemText[nEnd] <= L'9')
		{
			++nEnd;
		}
		if ((nEnd - nPos) > 1)
		{
			auto tsID(m_tsItemText.sub(nPos, nEnd));
			m_tsItemText.remove_range(nPos, nEnd);
			tsID.remove(L'\x0e');
			if (!tsID.empty())
				setCommandID(tsID.to_<UINT>());
		}
		else
			m_tsItemText.remove(L'\x0e');
		m_tsItemText.trim();

		setCheckToggle(true);

	}
	//check for $chr(12), if so then the item is a radio check item.
	if (const auto nPos = m_tsItemText.find(L'\x0c', 1); nPos != -1)	// $chr(12)
	{
		// remove $chr(12) from text and trim whitespaces
		m_tsItemText.remove(L'\x0c').trim();

		setRadioCheck(true);
	}
	// check for $chr(16), if found then next chars should be a number 0-100.
	if (const auto nPos = m_tsItemText.find(L'\x10', 1); nPos != -1)	// $chr(16)
	{
		ptrdiff_t nEnd{ nPos + 1 };
		while (m_tsItemText[nEnd] >= L'0' && m_tsItemText[nEnd] <= L'9')
		{
			++nEnd;
		}
		if ((nEnd - nPos) > 1)
		{
			auto tsID(m_tsItemText.sub(nPos, nEnd));
			m_tsItemText.remove_range(nPos, nEnd);
			tsID.remove(L'\x10');
			if (!tsID.empty())
				this->m_uProgressValue = std::clamp(tsID.to_<UINT>(),0u,100u);
		}
		else
			m_tsItemText.remove(L'\x10');
		m_tsItemText.trim();
	}
	return true;
}

void XPopupMenuItem::toXml(VectorOfIcons& vIcons, TiXmlElement* const xml) const
{
}

TiXmlElement* XPopupMenuItem::toXml(VectorOfIcons& vIcons) const
{
	auto xml = std::make_unique<TiXmlElement>("item");
	toXml(vIcons, xml.get());
	return xml.release();
}

void XPopupMenuItem::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis, const VectorOfIcons& vIcons)
{
}

void XPopupMenuItem::DrawButton(_In_ const LPDRAWITEMSTRUCT lpdis, _In_ const XPMENUCOLORS* const lpcol, _In_ bool bReversed, _In_ LPCRECT rc, _In_ bool bThemed) noexcept
{
	const auto bGrayed = dcx_testflag(lpdis->itemState, ODS_GRAYED);
	const auto bSelected = dcx_testflag(lpdis->itemState, ODS_SELECTED);

	bool bDoDraw{ true };

	if (bThemed)
	{
		if (auto hCurrentMenu = reinterpret_cast<HMENU>(lpdis->hwndItem); hCurrentMenu)
		{
			if (const auto hMenuWin = Dcx::XPopups.getHWNDfromHMENU(hCurrentMenu); IsWindow(hMenuWin))
			{
				if (auto hMenuStyleTheme = Dcx::UXModule.dcxOpenThemeData(hMenuWin, VSCLASS_BUTTON); hMenuStyleTheme)
				{
					int iStyle{ PBS_NORMAL };
					if (bReversed)
						iStyle = PBS_PRESSED;
					if (bSelected)
						iStyle = PBS_HOT;	// DrawItemSelection() not needed as this state handles it.
					if (bGrayed)
						iStyle = PBS_DISABLED;

					Dcx::UXModule.dcxDrawThemeBackground(hMenuStyleTheme, lpdis->hDC, BP_PUSHBUTTON, iStyle, rc, nullptr);

					Dcx::UXModule.dcxCloseThemeData(hMenuStyleTheme);

					bDoDraw = false;
				}
			}
		}
	}

	if (bDoDraw)
	{
		const RECT rcSaved = lpdis->rcItem;

		UINT uiStyle = DFCS_BUTTONPUSH | DFCS_ADJUSTRECT;

		if (bSelected)
			uiStyle |= DFCS_PUSHED;
		if (bGrayed)
			uiStyle |= DFCS_INACTIVE;

		if (bReversed)
		{
			if (bSelected)
			{
				DrawFrameControl(lpdis->hDC, &lpdis->rcItem, DFC_BUTTON, uiStyle);
				this->DrawItemSelection(lpdis, lpcol, bGrayed, false);
			}
		}
		else
		{
			DrawFrameControl(lpdis->hDC, &lpdis->rcItem, DFC_BUTTON, uiStyle);

			if (bSelected)
				this->DrawItemSelection(lpdis, lpcol, bGrayed, false);
		}

		lpdis->rcItem = rcSaved;
	}
}

void XPopupMenuItem::DrawProgress(_In_ HDC hdc, _In_ HMENU hCurrentMenu, _In_ const XPMENUCOLORS* const lpcol, _In_ int iPos, _In_ LPCRECT rc, _In_ bool bThemed) noexcept
{
	bool bDoDraw{ true };
	//WindowFromDC

	if (bThemed)
	{
		if (const auto hMenuWin = Dcx::XPopups.getHWNDfromHMENU(hCurrentMenu); IsWindow(hMenuWin))
		{
			if (auto hMenuStyleTheme = Dcx::UXModule.dcxOpenThemeData(hMenuWin, VSCLASS_PROGRESS); hMenuStyleTheme)
			{
				Auto(Dcx::UXModule.dcxCloseThemeData(hMenuStyleTheme));

				constexpr int iStyle{ PBFS_NORMAL };

				Dcx::UXModule.dcxDrawThemeBackground(hMenuStyleTheme, hdc, PP_BAR, iStyle, rc, nullptr);

				RECT rcContents{};
				Dcx::UXModule.dcxGetThemeBackgroundContentRect(hMenuStyleTheme, hdc, PP_BAR, iStyle, rc, &rcContents);

				rcContents.right = rcContents.left + gsl::narrow_cast<LONG>(((rcContents.right - rcContents.left) / 100.0) * std::clamp(iPos, 0, 100));
				Dcx::UXModule.dcxDrawThemeBackground(hMenuStyleTheme, hdc, PP_FILL, iStyle, &rcContents, nullptr);

				bDoDraw = false;
			}
		}
	}

	if (bDoDraw)
	{
		dcxDrawRect(hdc, rc, lpcol->m_clrBack, lpcol->m_clrBorder, false);

		RECT rcContents{ *rc };
		rcContents.top++;
		rcContents.bottom--;
		rcContents.left++;
		rcContents.right = rcContents.left + gsl::narrow_cast<LONG>(((rcContents.right - rcContents.left) / 100.0) * std::clamp(iPos, 0, 100));
		dcxDrawRect(hdc, &rcContents, lpcol->m_clrLightBox, lpcol->m_clrLightBox, false);
	}
}

void XPopupMenuItem::DrawTrackbar(_In_ HDC hdc, _In_ HMENU hCurrentMenu, _In_ UINT uState, _In_ const XPMENUCOLORS* const lpcol, _In_ int iPos, _In_ LPCRECT rc, _In_ bool bThemed) noexcept
{
	bool bDoDraw{ true };

	const auto bGrayed = dcx_testflag(uState, ODS_GRAYED);
	const auto bSelected = dcx_testflag(uState, ODS_SELECTED);

	if (bThemed)
	{
		if (const auto hMenuWin = Dcx::XPopups.getHWNDfromHMENU(hCurrentMenu); IsWindow(hMenuWin))
		{
			if (auto hMenuStyleTheme = Dcx::UXModule.dcxOpenThemeData(hMenuWin, VSCLASS_TRACKBAR); hMenuStyleTheme)
			{
				Auto(Dcx::UXModule.dcxCloseThemeData(hMenuStyleTheme));

				int iStyle{ TKS_NORMAL };

				Dcx::UXModule.dcxDrawThemeBackground(hMenuStyleTheme, hdc, TKP_TRACK, iStyle, rc, nullptr);

				RECT rcContents{};
				Dcx::UXModule.dcxGetThemeBackgroundContentRect(hMenuStyleTheme, hdc, TKP_TRACK, iStyle, rc, &rcContents);

				iStyle = TUBS_NORMAL;
				if (bGrayed)
					iStyle = TUBS_DISABLED;
				else if (bSelected)
					iStyle = TUBS_HOT;

				const auto offset = gsl::narrow_cast<LONG>(((rcContents.right - rcContents.left) / 100.0) * std::clamp(iPos, 0, 100));

				rcContents.left += offset - 5;
				rcContents.right = rcContents.left + 10;
				Dcx::UXModule.dcxDrawThemeBackground(hMenuStyleTheme, hdc, TKP_THUMB, iStyle, &rcContents, rc);

				bDoDraw = false;
			}
		}
	}

	if (bDoDraw)
	{
		dcxDrawRect(hdc, rc, lpcol->m_clrBack, lpcol->m_clrBorder, false);

		RECT rcThumb{ *rc };
		const auto offset = gsl::narrow_cast<LONG>(((rcThumb.right - rcThumb.left) / 100.0) * std::clamp(iPos, 0, 100));
		rcThumb.left += offset - 5;
		rcThumb.right = rcThumb.left + 10;

		if (bGrayed)
			dcxDrawRect(hdc, &rcThumb, lpcol->m_clrDisabledSelection, lpcol->m_clrBorder, false);
		else if (bSelected)
			dcxDrawRect(hdc, &rcThumb, lpcol->m_clrSelection, lpcol->m_clrSelectionBorder, false);
		else
			dcxDrawRect(hdc, &rcThumb, lpcol->m_clrBox, lpcol->m_clrBorder, false);
	}
}

SIZE XPopupMenuItem::getItemSize(const HWND mHwnd)
{
	if ((this->m_bSep) || (!m_pXParentMenu))
		return{ XPMI_BOXLPAD + XPMI_BOXWIDTH + XPMI_BOXRPAD, XPMI_SEPHEIGHT };

	if (!parseItemText())
	{
		mIRCLinker::eval(m_tsItemText, m_tsItemText);

		////check if the first char is $chr(12), if so then the text is utf8 (this is kept for compatability with old script only)
		//if (m_tsItemText[0] == 12)
		//{
		//	// remove $chr(12) from text and trim whitespaces
		//	m_tsItemText = m_tsItemText.right(-1).trim();
		//	setRadioCheck(true);
		//}
	}

	SIZE size{ XPMI_BOXLPAD + XPMI_BOXWIDTH + XPMI_BOXRPAD, XPMI_HEIGHT };

	if (!mHwnd)
		return size;

	if (const auto hdc = GetDC(mHwnd); hdc)
	{
		Auto(ReleaseDC(mHwnd, hdc));

		RECT rc{};
		TString tsStripped(m_tsItemText);
		tsStripped.strip();
		DrawTextW(hdc, tsStripped.to_wchr(), gsl::narrow_cast<int>(tsStripped.len()), &rc, DT_CALCRECT | DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		size.cx += std::max<long>((rc.right - rc.left), XPMI_MINSTRING);

		//size.cx = XPMI_BOXLPAD + XPMI_BOXRPAD + std::max<long>((rc.right - rc.left), XPMI_MINSTRING);
		//if (m_bSubMenu)
		//	size.cx += XPMI_SUBARROW_WIDTH + XPMI_SUBARROW_XPAD;

		//// Odd error in size returned by GetTextExtentPoint32() when dealing with utf text, length is cut short for some reason...
		//TString tsStripped(m_tsItemText);
		//tsStripped.strip();
		//tsStripped.Normalize();
		//GetTextExtentPoint32(hdc, tsStripped.to_chr(), tsStripped.len(), &size);
		//size.cx += std::max<long>(size.cx, XPMI_MINSTRING);

		//SIZE sz{};
		//TString tsStripped(m_tsItemText);
		//tsStripped.strip();
		//GetTextExtentPointW(hdc, tsStripped.to_wchr(), gsl::narrow_cast<int>(tsStripped.len()), &sz);
		//size.cx += std::max<long>(sz.cx, XPMI_MINSTRING);
	}

	return size;
}

void XPopupMenuItem::DrawItem(const LPDRAWITEMSTRUCT lpdis)
{
	if ((!lpdis) || (!this->m_pXParentMenu))
		return;

	const auto lpcol = &this->m_pXParentMenu->getColors();
	const auto iItemStyle = this->m_pXParentMenu->getItemStyle();
	const auto bGrayed = dcx_testflag(lpdis->itemState, ODS_GRAYED);
	const auto bSelected = dcx_testflag(lpdis->itemState, ODS_SELECTED);
	const auto bChecked = dcx_testflag(lpdis->itemState, ODS_CHECKED);

	// All Items
	this->DrawItemBackground(lpdis, lpcol);
	this->DrawItemBox(lpdis, lpcol);

	if (dcx_testflag(iItemStyle, XPS_VERTICALSEP))
		DrawItemVerticalSeparator(lpdis, lpcol);

	// Separator
	if (this->m_bSep)
		this->DrawItemSeparator(lpdis, lpcol);
	// Regular Item
	else {
		// Item is selected
		switch (const auto eStyle = this->getStyle(); eStyle)
		{
		case XPopupMenu::MenuStyle::XPMS_BUTTON:
		case XPopupMenu::MenuStyle::XPMS_BUTTON_REV:
		case XPopupMenu::MenuStyle::XPMS_BUTTON_THEMED:
		case XPopupMenu::MenuStyle::XPMS_BUTTON_REV_THEMED:
		case XPopupMenu::MenuStyle::XPMS_TRACK:
		case XPopupMenu::MenuStyle::XPMS_TRACK_THEMED:
			break;
		default:
		{
			if (bSelected)
			{
				if (bGrayed)
				{
					// only draw selection box if XPS_DISABLEDSEL
					if (dcx_testflag(iItemStyle, XPS_DISABLEDSEL))
						this->DrawItemSelection(lpdis, lpcol, true, this->m_pXParentMenu->IsRoundedSelector());
				}
				else
					this->DrawItemSelection(lpdis, lpcol, false, this->m_pXParentMenu->IsRoundedSelector());
			}
		}
		break;
		}

		if (bChecked)
		{
			if (this->IsRadioCheck())
				this->DrawItemRadioCheck(lpdis, lpcol, bGrayed, this->m_pXParentMenu->IsRoundedSelector());
			else
				this->DrawItemCheckBox(lpdis, lpcol, bGrayed, this->m_pXParentMenu->IsRoundedSelector());
		}

		this->DrawItemText(lpdis, lpcol, bGrayed);

		if (!bChecked || this->m_nIcon > -1)
			this->DrawItemIcon(lpdis, lpcol, iItemStyle, bSelected, bGrayed);

		if (this->m_bSubMenu)
			this->DrawItemSubArrow(lpdis, lpcol, bGrayed);
	}
}

/*!
 * \brief Draws the background of the actual item where text resides
 *
 * blah
 */
void XPopupMenuItem::DrawItemBackground(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol)
{
	if (!lpdis || !lpcol || !lpdis->hDC || !this->m_pXParentMenu)
		return;

	switch (this->getStyle())
	{
	case XPopupMenu::MenuStyle::XPMS_ICY:
		this->DrawGradient(lpdis->hDC, &lpdis->rcItem, lpcol->m_clrLightBox, lpcol->m_clrBox, false);
		break;

	case XPopupMenu::MenuStyle::XPMS_ICY_REV:
		this->DrawGradient(lpdis->hDC, &lpdis->rcItem, lpcol->m_clrBox, lpcol->m_clrLightBox, false);
		break;

	case XPopupMenu::MenuStyle::XPMS_GRADE:
		this->DrawGradient(lpdis->hDC, &lpdis->rcItem, lpcol->m_clrBox, lpcol->m_clrLightBox, true);
		break;

	case XPopupMenu::MenuStyle::XPMS_GRADE_REV:
		this->DrawGradient(lpdis->hDC, &lpdis->rcItem, lpcol->m_clrLightBox, lpcol->m_clrBox, true);
		break;

	case XPopupMenu::MenuStyle::XPMS_CUSTOM:
	case XPopupMenu::MenuStyle::XPMS_CUSTOMBIG:
	{
		if (this->DrawMenuBitmap(lpdis, (this->getStyle() == XPopupMenu::MenuStyle::XPMS_CUSTOMBIG), this->m_pXParentMenu->getBackBitmap()))
			break;
	}
	// fall through when unable to draw bitmap (no bitmap or error)
	[[fallthrough]];
	//case XPopupMenu::MenuStyle::XPMS_OFFICEXP:
	//case XPopupMenu::MenuStyle::XPMS_NORMAL:
	//case XPopupMenu::MenuStyle::XPMS_OFFICE2003_REV:
	//case XPopupMenu::MenuStyle::XPMS_OFFICE2003:
	//case XPopupMenu::MenuStyle::XPMS_BUTTON:
	//case XPopupMenu::MenuStyle::XPMS_BUTTON_REV:
	//case XPopupMenu::MenuStyle::XPMS_BUTTON_THEMED:
	//case XPopupMenu::MenuStyle::XPMS_BUTTON_REV_THEMED:
	//case XPopupMenu::MenuStyle::XPMS_VERTICAL:
	//case XPopupMenu::MenuStyle::XPMS_VERTICAL_REV:
	default:
	{
		Dcx::FillRectColour(lpdis->hDC, &lpdis->rcItem, lpcol->m_clrBack);
	}
	break;
	}
}

/*!
 * \brief Draws the area on the left side of the menu for checks/radios/icons
 *
 * blah
 */
void XPopupMenuItem::DrawItemBox(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol)
{
	if (!lpdis || !lpcol || !lpdis->hDC || !this->m_pXParentMenu)
		return;

	bool bThemed{ false };
	bool bRev{ false };

	switch (this->getStyle())
	{
	case XPopupMenu::MenuStyle::XPMS_OFFICE2003_REV:
	{
		const RECT rc{ XPMI_BOXLPAD, lpdis->rcItem.top, XPMI_BOXLPAD + XPMI_BOXWIDTH, lpdis->rcItem.bottom };
		XPopupMenuItem::DrawGradient(lpdis->hDC, &rc, lpcol->m_clrBox, lpcol->m_clrLightBox, true);
		break;
	}

	case XPopupMenu::MenuStyle::XPMS_OFFICEXP:
	{
		const RECT rc{ XPMI_BOXLPAD, lpdis->rcItem.top, XPMI_BOXLPAD + XPMI_BOXWIDTH, lpdis->rcItem.bottom };

		Dcx::FillRectColour(lpdis->hDC, &rc, lpcol->m_clrBox);
		break;
	}

	case XPopupMenu::MenuStyle::XPMS_VERTICAL_REV:
	{
		bRev = true;
	}
	[[fallthrough]];
	case XPopupMenu::MenuStyle::XPMS_VERTICAL:
		XPopupMenuItem::DrawVerticalBar(lpdis, lpcol, bRev);
		break;

	case XPopupMenu::MenuStyle::XPMS_ICY:
	case XPopupMenu::MenuStyle::XPMS_ICY_REV:
	case XPopupMenu::MenuStyle::XPMS_GRADE:
	case XPopupMenu::MenuStyle::XPMS_GRADE_REV:
	case XPopupMenu::MenuStyle::XPMS_CUSTOM:
	case XPopupMenu::MenuStyle::XPMS_CUSTOMBIG:
	case XPopupMenu::MenuStyle::XPMS_NORMAL:
		break;

	case XPopupMenu::MenuStyle::XPMS_BUTTON_REV_THEMED:
	{
		bRev = true;
	}
	[[fallthrough]];
	case XPopupMenu::MenuStyle::XPMS_BUTTON_THEMED:
	{
		bThemed = true;
	}
	[[fallthrough]];
	case XPopupMenu::MenuStyle::XPMS_BUTTON:
	{
		if (this->m_bSep)
			break;

		RECT rc{ lpdis->rcItem };
		if (const auto iItemStyle = this->m_pXParentMenu->getItemStyle(); dcx_testflag(iItemStyle, XPS_VERTICALSEP))
			rc.left += XPMI_BOXLPAD + XPMI_BOXWIDTH;

		this->DrawButton(lpdis, lpcol, bRev, &rc, bThemed);
	}
	break;

	case XPopupMenu::MenuStyle::XPMS_BUTTON_REV:
	{
		if (this->m_bSep)
			break;

		RECT rc{ lpdis->rcItem };
		if (const auto iItemStyle = this->m_pXParentMenu->getItemStyle(); dcx_testflag(iItemStyle, XPS_VERTICALSEP))
			rc.left += XPMI_BOXLPAD + XPMI_BOXWIDTH;

		this->DrawButton(lpdis, lpcol, true, &rc, bThemed);
	}
	break;

	case XPopupMenu::MenuStyle::XPMS_PROGRESS_THEMED:
	{
		bThemed = true;
	}
	[[fallthrough]];
	case XPopupMenu::MenuStyle::XPMS_PROGRESS:
	{
		if (this->m_bSep)
			break;

		RECT rc{ lpdis->rcItem };
		if (const auto iItemStyle = this->m_pXParentMenu->getItemStyle(); dcx_testflag(iItemStyle, XPS_VERTICALSEP))
			rc.left += XPMI_BOXLPAD + XPMI_BOXWIDTH;

		this->DrawProgress(lpdis->hDC, reinterpret_cast<HMENU>(lpdis->hwndItem), lpcol, m_uProgressValue, &rc, bThemed);
	}
	break;

	case XPopupMenu::MenuStyle::XPMS_TRACK_THEMED:
	{
		bThemed = true;
	}
	[[fallthrough]];
	case XPopupMenu::MenuStyle::XPMS_TRACK:
	{
		if (this->m_bSep)
			break;

		RECT rc{ lpdis->rcItem };
		if (const auto iItemStyle = this->m_pXParentMenu->getItemStyle(); dcx_testflag(iItemStyle, XPS_VERTICALSEP))
			rc.left += XPMI_BOXLPAD + XPMI_BOXWIDTH;

		this->DrawTrackbar(lpdis->hDC, reinterpret_cast<HMENU>(lpdis->hwndItem), lpdis->itemState, lpcol, m_uProgressValue, &rc, bThemed);
	}
	break;

	case XPopupMenu::MenuStyle::XPMS_OFFICE2003:
	default:
	{
		const RECT rc{ XPMI_BOXLPAD, lpdis->rcItem.top, XPMI_BOXLPAD + XPMI_BOXWIDTH, lpdis->rcItem.bottom };

		XPopupMenuItem::DrawGradient(lpdis->hDC, &rc, lpcol->m_clrLightBox, lpcol->m_clrBox, true);
		break;
	}
	}
}

void XPopupMenuItem::DrawItemSelection(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol, const bool bDis, const bool bRounded) noexcept
{
	if (!lpdis || !lpcol || !lpdis->hDC)
		return;

	if (!dcxDrawTranslucentRect(lpdis->hDC, std::addressof(lpdis->rcItem), bDis ? lpcol->m_clrDisabledSelection : lpcol->m_clrSelection, lpcol->m_clrSelectionBorder, bRounded))
		dcxDrawRect(lpdis->hDC, std::addressof(lpdis->rcItem), bDis ? lpcol->m_clrDisabledSelection : lpcol->m_clrSelection, lpcol->m_clrSelectionBorder, bRounded);
}

void XPopupMenuItem::DrawItemCheckBox(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol, const bool bDis, const bool bRounded) noexcept
{
	if (!lpdis || !lpcol || !lpdis->hDC)
		return;

	dcxDrawCheckBox(lpdis->hDC, std::addressof(lpdis->rcItem), std::addressof(lpcol->m_clrCheckBox), lpdis->itemState, true, bRounded);
}

void XPopupMenuItem::DrawItemRadioCheck(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol, const bool bDis, const bool bRounded) noexcept
{
	if (!lpdis || !lpcol || !lpdis->hDC)
		return;

	dcxDrawRadioBox(lpdis->hDC, std::addressof(lpdis->rcItem), std::addressof(lpcol->m_clrCheckBox), lpdis->itemState, true, bRounded);
}

void XPopupMenuItem::DrawItemText(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol, const bool bDis)
{
	if (!lpdis || !lpcol || !lpdis->hDC)
		return;

	const auto oldClr = SetTextColor(lpdis->hDC, (bDis ? lpcol->m_clrDisabledText : ((dcx_testflag(lpdis->itemState, ODS_SELECTED)) ? lpcol->m_clrSelectedText : lpcol->m_clrText)));
	Auto(SetTextColor(lpdis->hDC, oldClr));

	const auto oldBkg = SetBkMode(lpdis->hDC, TRANSPARENT);
	Auto(SetBkMode(lpdis->hDC, oldBkg));

	RECT rc{ lpdis->rcItem };
	rc.left += XPMI_BOXLPAD + XPMI_BOXWIDTH + XPMI_BOXRPAD;

	if (m_tsItemText.numtok(TSTABCHAR) > 1)
	{
		const auto lefttext(m_tsItemText.getfirsttok(1, TSTABCHAR).trim());
		const auto righttext(m_tsItemText.getnexttok(TSTABCHAR).trim());

		//DrawTextEx( lpdis->hDC, lefttext.to_chr( ), lefttext.len( ), &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, nullptr );
		mIRC_DrawText(lpdis->hDC, lefttext, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, false);

		if (!righttext.empty())
		{
			rc.right -= 15;
			//DrawTextEx( lpdis->hDC, (TCHAR *)righttext.to_chr( ), righttext.len( ), &rc, DT_RIGHT | DT_SINGLELINE | DT_VCENTER, nullptr );
			mIRC_DrawText(lpdis->hDC, righttext, &rc, DT_RIGHT | DT_SINGLELINE | DT_VCENTER, false);
		}
	}
	else {
		//DrawTextEx( lpdis->hDC, this->m_tsItemText.to_chr( ), this->m_tsItemText.len( ), &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, nullptr );
		mIRC_DrawText(lpdis->hDC, m_tsItemText, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, false);
	}
}

void XPopupMenuItem::DrawItemIcon(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol, const UINT iExStyles, const bool bSel, const bool bDis) noexcept
{
	if (!this->m_pXParentMenu || (this->m_nIcon < 0) || !lpdis || !lpdis->hDC)
		return;

	const auto himl = this->m_pXParentMenu->getImageList();
	if (!himl)
		return;

	if (this->m_nIcon >= ImageList_GetImageCount(himl))
		return;

	constexpr auto x = (XPMI_BOXLPAD + XPMI_BOXLPAD + XPMI_BOXWIDTH - XPMI_ICONSIZE) / 2;
	const auto y = (lpdis->rcItem.top + lpdis->rcItem.bottom - XPMI_ICONSIZE) / 2;

	// Selected Item
	if (bSel)
	{
		// Disabled
		if (bDis)
			ImageList_DrawEx(himl, this->m_nIcon, lpdis->hDC, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT | ILD_BLEND50);
		else {

			if (dcx_testflag(iExStyles, XPS_ICON3DSHADOW))
			{
				ImageList_DrawEx(himl, this->m_nIcon, lpdis->hDC, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT | ILD_BLEND25);
				ImageList_DrawEx(himl, this->m_nIcon, lpdis->hDC, x - 1, y - 1, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT);

			}
			else if (dcx_testflag(iExStyles, XPS_ICON3D))
				ImageList_DrawEx(himl, this->m_nIcon, lpdis->hDC, x - 1, y - 1, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT);
			else
				ImageList_DrawEx(himl, this->m_nIcon, lpdis->hDC, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT);
		}
	}
	// Not selected
	else {

		if (bDis)
			ImageList_DrawEx(himl, this->m_nIcon, lpdis->hDC, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT | ILD_BLEND50);
		else
			ImageList_DrawEx(himl, this->m_nIcon, lpdis->hDC, x, y, 0, 0, CLR_NONE, RGB(0, 0, 0), ILD_TRANSPARENT);
	}
}

void XPopupMenuItem::DrawItemSubArrow(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol, const bool bDis) noexcept
{
	if (!lpdis || !lpdis->hDC || !lpcol)
		return;

	//#ifdef DCX_USE_GDIPLUS
	//	if (!Dcx::GDIModule.isUseable())
	//		dcxDrawArrow(lpdis->hDC, &lpdis->rcItem, bDis ? lpcol->m_clrDisabledText : lpcol->m_clrText);
	//	else {
	//		Gdiplus::Graphics gfx( lpdis->hDC );
	//		const COLORREF clrShape = bDis?lpcol->m_clrDisabledText:lpcol->m_clrText;
	//		const auto x = lpdis->rcItem.right - XPMI_SUBARROW_WIDTH;
	//		const auto y = ((lpdis->rcItem.bottom + lpdis->rcItem.top) / 2) - (XPMI_SUBARROW_HEIGHT / 2);
	//		const RECT rc{ x,y,x + 5,y + 8 };
	//
	//		gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias8x8);
	//		//gfx.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
	//		gfx.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
	//		gfx.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
	//		gfx.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
	//		Gdiplus::SolidBrush blackBrush(Gdiplus::Color(128, GetRValue(clrShape), GetGValue(clrShape), GetBValue(clrShape)));
	//		// Create an array of Point objects that define the polygon.
	//		Gdiplus::Point points[3];
	//		points[0].X = rc.left;
	//		points[0].Y = rc.top;
	//		points[1].X = rc.right;
	//		points[1].Y = (rc.top + ((rc.bottom - rc.top)/2));
	//		points[2].X = rc.left;
	//		points[2].Y = rc.bottom;
	//		// Fill the polygon.
	//		gfx.FillPolygon(&blackBrush, &points[0], 3);
	//	}
	//	ExcludeClipRect( lpdis->hDC, lpdis->rcItem.right - (XPMI_SUBARROW_WIDTH + XPMI_SUBARROW_XPAD), lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom );
	//#else

	if (dcx_testflag(lpdis->itemState, CDIS_SELECTED))
		dcxDrawArrow(lpdis->hDC, &lpdis->rcItem, bDis ? lpcol->m_clrDisabledText : lpcol->m_clrSelectedText, dcxArrowFlags::Right /*| dcxArrowFlags::Themed*/);
	else
		dcxDrawArrow(lpdis->hDC, &lpdis->rcItem, bDis ? lpcol->m_clrDisabledText : lpcol->m_clrText, dcxArrowFlags::Down /*| dcxArrowFlags::Themed*/);

	ExcludeClipRect(lpdis->hDC, lpdis->rcItem.right - (XPMI_SUBARROW_WIDTH + XPMI_SUBARROW_XPAD), lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom);
	//#endif
}

void XPopupMenuItem::DrawItemSeparator(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol) noexcept
{
	if (!this->m_pXParentMenu || !lpdis || !lpcol || !lpdis->hDC)
		return;

	auto x1 = lpdis->rcItem.left;
	const auto x2 = lpdis->rcItem.right;
	const auto y = (lpdis->rcItem.bottom + lpdis->rcItem.top) / 2;
	const auto& iItemStyle = this->m_pXParentMenu->getItemStyle();

	switch (this->getStyle())
	{
	case XPopupMenu::MenuStyle::XPMS_ICY:
	case XPopupMenu::MenuStyle::XPMS_ICY_REV:
	case XPopupMenu::MenuStyle::XPMS_NORMAL:
	case XPopupMenu::MenuStyle::XPMS_BUTTON:
	case XPopupMenu::MenuStyle::XPMS_BUTTON_REV:
	case XPopupMenu::MenuStyle::XPMS_BUTTON_THEMED:
	case XPopupMenu::MenuStyle::XPMS_BUTTON_REV_THEMED:
	{
		if (dcx_testflag(iItemStyle, XPS_VERTICALSEP))
			x1 = XPMI_BOXLPAD + XPMI_BOXWIDTH + XPMI_BOXRPAD;
	}
	break;

	case XPopupMenu::MenuStyle::XPMS_OFFICEXP:
	case XPopupMenu::MenuStyle::XPMS_OFFICE2003_REV:
	case XPopupMenu::MenuStyle::XPMS_OFFICE2003:
	case XPopupMenu::MenuStyle::XPMS_GRADE:
	case XPopupMenu::MenuStyle::XPMS_GRADE_REV:
	case XPopupMenu::MenuStyle::XPMS_CUSTOM:
	case XPopupMenu::MenuStyle::XPMS_CUSTOMBIG:
	case XPopupMenu::MenuStyle::XPMS_VERTICAL:
	case XPopupMenu::MenuStyle::XPMS_VERTICAL_REV:
	default:
		x1 = XPMI_BOXLPAD + XPMI_BOXWIDTH + XPMI_BOXRPAD;
		break;
	}

	{
		const auto hPen = CreatePen(PS_SOLID, 1, lpcol->m_clrSeparatorLine);

		if (!hPen)
			return;
		Auto(DeleteObject(hPen));

		const auto oldPen = SelectObject(lpdis->hDC, hPen);
		Auto(SelectObject(lpdis->hDC, oldPen));

		dcxDrawLine(lpdis->hDC, x1, y, x2, y);
	}

	if (dcx_testflag(iItemStyle, XPS_DOUBLESEP))
	{
		const auto hContrastPen = CreatePen(PS_SOLID, 1, GetContrastColour(lpcol->m_clrSeparatorLine));
		if (!hContrastPen)
			return;
		Auto(DeleteObject(hContrastPen));

		const auto oldPen = SelectObject(lpdis->hDC, hContrastPen);
		Auto(SelectObject(lpdis->hDC, oldPen));

		dcxDrawLine(lpdis->hDC, x1, y + 1, x2, y + 1);
	}
}

void XPopupMenuItem::DrawItemVerticalSeparator(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol) noexcept
{
	if (!this->m_pXParentMenu || !lpdis || !lpcol || !lpdis->hDC)
		return;

	const LONG xPos{ (lpdis->rcItem.left + XPMI_BOXLPAD + XPMI_BOXWIDTH) };

	{
		const auto hPen = CreatePen(PS_SOLID, 1, lpcol->m_clrVerticalSeparatorLine);

		if (!hPen)
			return;
		Auto(DeleteObject(hPen));

		const auto oldPen = SelectObject(lpdis->hDC, hPen);
		Auto(SelectObject(lpdis->hDC, oldPen));

		dcxDrawLine(lpdis->hDC, xPos, lpdis->rcItem.top, xPos, lpdis->rcItem.bottom);
	}

	if (dcx_testflag(this->m_pXParentMenu->getItemStyle(), XPS_DOUBLESEP))
	{
		const auto hContrastPen = CreatePen(PS_SOLID, 1, GetContrastColour(lpcol->m_clrVerticalSeparatorLine));
		if (!hContrastPen)
			return;
		Auto(DeleteObject(hContrastPen));

		const auto oldPen = SelectObject(lpdis->hDC, hContrastPen);
		Auto(SelectObject(lpdis->hDC, oldPen));

		dcxDrawLine(lpdis->hDC, xPos + 1, lpdis->rcItem.top, xPos + 1, lpdis->rcItem.bottom);
	}
}

void XPopupMenuItem::DrawGradient(const HDC hdc, const RECT* const lprc, const COLORREF clrStart, const COLORREF clrEnd, const bool bHorz) noexcept
{
	if (!hdc || !lprc)
		return;

	const auto StartRed = GetRValue(clrStart);
	const auto StartGreen = GetGValue((clrStart & 0xFFFF));
	const auto StartBlue = GetBValue(clrStart);

	const auto EndRed = GetRValue(clrEnd);
	const auto EndGreen = GetGValue((clrEnd & 0xFFFF));
	const auto EndBlue = GetBValue(clrEnd);

	//TRIVERTEX        vert[2] = { 0 };
	//
	//vert [0] .x      = lprc->left;
	//vert [0] .y      = lprc->top;
	//vert [0] .Red    = (COLOR16)((StartRed & 0xff) << 8);
	//vert [0] .Green  = (COLOR16)((StartGreen & 0xff) << 8);
	//vert [0] .Blue   = (COLOR16)((StartBlue & 0xff) << 8);
	//vert [0] .Alpha  = 0xff00;
	//
	//vert [1] .x      = lprc->right;
	//vert [1] .y      = lprc->bottom;
	//vert [1] .Red    = (COLOR16)((EndRed & 0xff) << 8);
	//vert [1] .Green  = (COLOR16)((EndGreen & 0xff) << 8);
	//vert [1] .Blue   = (COLOR16)((EndBlue & 0xff) << 8);
	//vert [1] .Alpha  = 0xff00;
	////vert [1] .Alpha  = 0x0C00;

	TRIVERTEX vert[2] = {
		{ lprc->left, lprc->top, gsl::narrow_cast<COLOR16>((StartRed & 0xff) << 8), gsl::narrow_cast<COLOR16>((StartGreen & 0xff) << 8), gsl::narrow_cast<COLOR16>((StartBlue & 0xff) << 8), 0xff00 },
		{ lprc->right, lprc->bottom, gsl::narrow_cast<COLOR16>((EndRed & 0xff) << 8), gsl::narrow_cast<COLOR16>((EndGreen & 0xff) << 8), gsl::narrow_cast<COLOR16>((EndBlue & 0xff) << 8), 0xff00 }
	};

	GRADIENT_RECT    gRect{ 0,1 };

	const ULONG gMode = (bHorz) ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V;

	if (!GradientFill(hdc, &vert[0], gsl::narrow_cast<ULONG>(std::size(vert)), &gRect, 1, gMode))
	{
		// if GradientFill fails do our own method.
		constexpr auto dy = 2;
		const auto n = (bHorz) ? lprc->bottom - lprc->top - dy : lprc->right - lprc->left - dy;

		RECT rc{};

		for (auto dn = decltype(n){0}; dn <= n; dn += dy)
		{
			const auto Red = gsl::narrow_cast<BYTE>(Dcx::dcxMulDiv32(gsl::narrow_cast<int>(EndRed) - StartRed, dn, n) + StartRed);
			const auto Green = gsl::narrow_cast<BYTE>(Dcx::dcxMulDiv32(gsl::narrow_cast<int>(EndGreen) - StartGreen, dn, n) + StartGreen);
			const auto Blue = gsl::narrow_cast<BYTE>(Dcx::dcxMulDiv32(gsl::narrow_cast<int>(EndBlue) - StartBlue, dn, n) + StartBlue);

			if (bHorz)
				SetRect(&rc, lprc->left, lprc->top + dn, lprc->right, lprc->top + dn + dy);
			else
				SetRect(&rc, lprc->left + dn, lprc->top, lprc->left + dn + dy, lprc->bottom);

			Dcx::FillRectColour(hdc, &rc, RGB(Red, Green, Blue));
		}
	}
}

void XPopupMenuItem::DrawVerticalBar(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol, const bool bReversed)
{
	if (!lpdis || !lpcol || !lpdis->hDC)
		return;

	// Working code: Calculates height of complete menu, and draws gradient to fill. Samples taken off complete gradient when needed.
	///*
	// Get height of all menu items
			//int menuH = 0;
			//int i = 0;
	// GetMenuItemRect() calls here cause submenus to fails to render on first opening for some reason.
			//while (GetMenuItemRect(mIRCLinker::getHWND(), (HMENU) lpdis->hwndItem, i, &rc) != FALSE) {
			//	menuH += (rc.bottom - rc.top);
			//	i++;
			//}
			//RECT rcIntersect;
			//RECT rcBar;

			//// Fix to remove black line at the bottom
			//menuH++;
	// using bitmap height like this fixes that.
	//BITMAP bm{};
	//if (GetObject((HBITMAP)GetCurrentObject(lpdis->hDC, OBJ_BITMAP), sizeof(BITMAP), &bm) == 0)
	//	return;

	auto [code, bm] = Dcx::dcxGetObject<BITMAP>(Dcx::dcxGetCurrentObject<HBITMAP>(lpdis->hDC, OBJ_BITMAP));
	if (code == 0)
		return;

	// get the size of the bar on the left
	const RECT rcBar{ XPMI_BOXLPAD, 0, (XPMI_BOXLPAD + XPMI_BOXWIDTH), bm.bmHeight };

	// get the rect of the box which will draw JUST the box (prevents redraw over items already done)
	const RECT rcIntersect{ XPMI_BOXLPAD, lpdis->rcItem.top, (XPMI_BOXLPAD + XPMI_BOXWIDTH), lpdis->rcItem.bottom };

#if DCX_USE_WRAPPERS
	// set up a buffer to draw the whole gradient bar
	const Dcx::dcxHDCBuffer hdcBuffer(lpdis->hDC, rcBar);

	// draw the gradient into the buffer
	if (bReversed)
		XPopupMenuItem::DrawGradient(hdcBuffer, &rcBar, lpcol->m_clrBox, lpcol->m_clrLightBox, false);
	else
		XPopupMenuItem::DrawGradient(hdcBuffer, &rcBar, lpcol->m_clrLightBox, lpcol->m_clrBox, false);

	// copy the box we want from the whole gradient bar
	BitBlt(lpdis->hDC, rcIntersect.left, rcIntersect.top, rcIntersect.right - rcIntersect.left, rcIntersect.bottom - rcIntersect.top, hdcBuffer, rcIntersect.left, rcIntersect.top, SRCCOPY);
#else
	// set up a buffer to draw the whole gradient bar
	if (auto hdcBuffer = CreateHDCBuffer(lpdis->hDC, &rcBar); hdcBuffer)
	{
		Auto(DeleteHDCBuffer(hdcBuffer));

		// draw the gradient into the buffer
		if (bReversed)
			XPopupMenuItem::DrawGradient(*hdcBuffer, &rcBar, lpcol->m_clrBox, lpcol->m_clrLightBox, TRUE);
		else
			XPopupMenuItem::DrawGradient(*hdcBuffer, &rcBar, lpcol->m_clrLightBox, lpcol->m_clrBox, TRUE);

		// copy the box we want from the whole gradient bar
		BitBlt(lpdis->hDC, rcIntersect.left, rcIntersect.top, rcIntersect.right - rcIntersect.left, rcIntersect.bottom - rcIntersect.top, *hdcBuffer, rcIntersect.left, rcIntersect.top, SRCCOPY);
	}
	else {
		// buffer create failed, try unbuffered.
		if (bReversed)
			XPopupMenuItem::DrawGradient(lpdis->hDC, &rcIntersect, lpcol->m_clrBox, lpcol->m_clrLightBox, TRUE);
		else
			XPopupMenuItem::DrawGradient(lpdis->hDC, &rcIntersect, lpcol->m_clrLightBox, lpcol->m_clrBox, TRUE);
}
#endif
}

/*!
 * \brief blah
 *
 * blah
 */

COLORREF XPopupMenuItem::LightenColor(const UINT iScale, const COLORREF clrColor) noexcept
{
	const auto nScale = gsl::narrow_cast<int>(iScale);
	const auto R = Dcx::dcxMulDiv32(255 - GetRValue(clrColor), nScale, 255) + GetRValue(clrColor);
	const auto G = Dcx::dcxMulDiv32(255 - GetGValue((clrColor & 0xFFFF)), nScale, 255) + GetGValue((clrColor & 0xFFFF));
	const auto B = Dcx::dcxMulDiv32(255 - GetBValue(clrColor), nScale, 255) + GetBValue(clrColor);

	return RGB(R, G, B);
}

/*!
* \brief blah
*
* blah
*/

COLORREF XPopupMenuItem::DarkenColor(const UINT iScale, const COLORREF clrColor) noexcept
{
	const auto nScale = gsl::narrow_cast<int>(iScale);
	const auto R = Dcx::dcxMulDiv32(GetRValue(clrColor), (255 - nScale), 255);
	const auto G = Dcx::dcxMulDiv32(GetGValue((clrColor & 0xFFFF)), (255 - nScale), 255);
	const auto B = Dcx::dcxMulDiv32(GetBValue(clrColor), (255 - nScale), 255);

	return RGB(R, G, B);
}

bool XPopupMenuItem::DrawMenuBitmap(const LPDRAWITEMSTRUCT lpdis, const bool bBigImage, const HBITMAP bmImage)
{
	if ((!lpdis) || (!bmImage) || (!lpdis->hDC))
		return false;

#if DCX_USE_WRAPPERS
	if (!bBigImage)
	{
		const Dcx::dcxHDCBitmapResource hdcbmp(lpdis->hDC, bmImage);

		if (auto [code, bmp] = Dcx::dcxGetObject<BITMAP>(bmImage); code != 0)
		{
			const int oldMode = SetStretchBltMode(lpdis->hDC, STRETCH_HALFTONE);
			SetBrushOrgEx(lpdis->hDC, 0, 0, nullptr);
			StretchBlt(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right - lpdis->rcItem.left, lpdis->rcItem.bottom - lpdis->rcItem.top, hdcbmp.get(), 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
			SetStretchBltMode(lpdis->hDC, oldMode);
		}
	}
	else {

		// TODO: Move the whole menu draw elsewhere so its only done once, instead of for each item.

		const RECT rcBar = dcxGetCurrentBitmapRect(lpdis->hDC);
		if (IsRectEmpty(&rcBar))
			return false;

		// get the rect of the box which will draw JUST the box (prevents redraw over items already done)
		const RECT rcIntersect{ 0, lpdis->rcItem.top, rcBar.right, lpdis->rcItem.bottom };

		// set up a buffer to draw the whole whole menus background.
		const Dcx::dcxHDCBuffer hdcBuffer(lpdis->hDC, rcBar);

		if (const SIZE sz = dcxGetBitmapDimensions(bmImage); ((sz.cx > 0) && (sz.cy > 0)))
		{
			// draw into the buffer
			const Dcx::dcxHDCBitmapResource hdcbmp(lpdis->hDC, bmImage);

			SetStretchBltMode(hdcBuffer, STRETCH_HALFTONE);
			SetBrushOrgEx(hdcBuffer, 0, 0, nullptr);
			StretchBlt(hdcBuffer, rcBar.left, rcBar.top, rcBar.right - rcBar.left, rcBar.bottom - rcBar.top, hdcbmp, 0, 0, sz.cx, sz.cy, SRCCOPY);

			// test code to alpha blend the image (works)
			//if (auto p_Item = reinterpret_cast<XPopupMenuItem*>(lpdis->itemData); p_Item)
			//{
			//	if (p_Item->m_pXParentMenu)
			//		Dcx::FillRectColour(hdcBuffer, &rcBar, p_Item->m_pXParentMenu->getColor(XPopupMenu::MenuColours::XPMC_BACKGROUND));
			//	const BLENDFUNCTION blend{ AC_SRC_OVER, 0, 0x7f, 0 };
			//	AlphaBlend(hdcBuffer, rcBar.left, rcBar.top, rcBar.right - rcBar.left, rcBar.bottom - rcBar.top, hdcbmp, 0, 0, bm.bmWidth, bm.bmHeight, blend);
			//}
			//else
			//	StretchBlt(hdcBuffer, rcBar.left, rcBar.top, rcBar.right - rcBar.left, rcBar.bottom - rcBar.top, hdcbmp, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
		}
		// copy the box we want from the whole image
		BitBlt(lpdis->hDC, rcIntersect.left, rcIntersect.top, rcIntersect.right - rcIntersect.left, rcIntersect.bottom - rcIntersect.top, hdcBuffer, rcIntersect.left, rcIntersect.top, SRCCOPY);
	}
	return true;
#else
	if (!bBigImage)
	{
		const auto hdcbmp = CreateCompatibleDC(lpdis->hDC);

		if (!hdcbmp)
			return false;
		Auto(DeleteDC(hdcbmp));

		if (BITMAP bmp{}; GetObject(bmImage, sizeof(BITMAP), &bmp) != 0)
		{
			const auto hOldBm = SelectObject(hdcbmp, bmImage);
			Auto(SelectObject(hdcbmp, hOldBm));

			const auto oldMode = SetStretchBltMode(lpdis->hDC, STRETCH_HALFTONE);
			SetBrushOrgEx(lpdis->hDC, 0, 0, nullptr);
			StretchBlt(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right - lpdis->rcItem.left, lpdis->rcItem.bottom - lpdis->rcItem.top, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
			SetStretchBltMode(lpdis->hDC, oldMode);
		}
	}
	else {
		BITMAP bm{};
		if (GetObject(GetCurrentObject(lpdis->hDC, OBJ_BITMAP), sizeof(BITMAP), &bm) == 0)
			return false;

		// get the size of the whole menu.
		const RECT rcBar{ 0, 0, bm.bmWidth, bm.bmHeight };

		// get the rect of the box which will draw JUST the box (prevents redraw over items already done)
		const RECT rcIntersect{ 0, lpdis->rcItem.top, rcBar.right, lpdis->rcItem.bottom };

		// set up a buffer to draw the whole whole menus background.
		const auto hdcBuffer = CreateHDCBuffer(lpdis->hDC, &rcBar);

		if (!hdcBuffer)
			return false;
		Auto(DeleteHDCBuffer(hdcBuffer));

		// draw into the buffer
		if (auto hdcbmp = CreateCompatibleDC(lpdis->hDC); hdcbmp)
		{
			Auto(DeleteDC(hdcbmp));

			BITMAP bmp{};

			if (GetObject(bmImage, sizeof(BITMAP), &bmp) != 0)
			{
				const auto hOldBm = SelectObject(hdcbmp, bmImage);
				Auto(SelectObject(hdcbmp, hOldBm));

				SetStretchBltMode(*hdcBuffer, STRETCH_HALFTONE);
				SetBrushOrgEx(*hdcBuffer, 0, 0, nullptr);

				StretchBlt(*hdcBuffer, rcBar.left, rcBar.top, rcBar.right - rcBar.left, rcBar.bottom - rcBar.top, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
			}

			// copy the box we want from the whole gradient bar
			BitBlt(lpdis->hDC, rcIntersect.left, rcIntersect.top, rcIntersect.right - rcIntersect.left, rcIntersect.bottom - rcIntersect.top, *hdcBuffer, rcIntersect.left, rcIntersect.top, SRCCOPY);
		}
	}
	return true;
#endif
}
