/*!
http://www.codeguru.com/cpp/controls/toolbar/article.php/c5487/
grey icons

* \file dcxtoolbar.cpp
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
#include "Classes/dcxtoolbar.h"
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

DcxToolBar::DcxToolBar(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	//We need to divide ExStyles and tbExStyles here because the first is used for
	//global transparent style and the second for arrows style

	const auto ws = parseControlStyles(styles);
	const auto ExStylesTb = parseControlStylesToolBar(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_TOOLBARCLASS,
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

	if (ExStylesTb != WindowExStyle::None)
		SendMessage(m_Hwnd, TB_SETEXTENDEDSTYLE, 0U, gsl::narrow_cast<LPARAM>(ExStylesTb));

	SendMessage(m_Hwnd, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0L);

	this->setToolTipHWND(reinterpret_cast<HWND>(SendMessage(m_Hwnd, TB_GETTOOLTIPS, NULL, NULL)));
	if (styles.istok(TEXT("balloon")) && IsWindow(getToolTipHWND()))
	{
		dcxSetWindowStyle(getToolTipHWND(), dcxGetWindowStyle(getToolTipHWND()) | TTS_BALLOON);
	}
	//SendMessage( m_Hwnd, TB_SETPARENT, (WPARAM)mParentHwnd, NULL);

	this->autoSize();

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxToolBar::~DcxToolBar() noexcept
{
	this->resetContent();

	ImageList_Destroy(this->getImageList(dcxToolBar_ImageLists::TB_IML_NORMAL));
	ImageList_Destroy(this->getImageList(dcxToolBar_ImageLists::TB_IML_DISABLE));
	ImageList_Destroy(this->getImageList(dcxToolBar_ImageLists::TB_IML_HOT));
}

/*!
* \brief blah
*
* blah
*/
WindowExStyle DcxToolBar::parseControlStylesToolBar(const TString& tsStyles)
{
	WindowExStyle ToolBarExStyles(WindowExStyle::None);

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"arrows"_hash:
			ToolBarExStyles |= TBSTYLE_EX_DRAWDDARROWS;
			break;
		case L"doublebuffer"_hash:
			ToolBarExStyles |= TBSTYLE_EX_DOUBLEBUFFER;
			break;
		case L"vertical"_hash:
			ToolBarExStyles |= TBSTYLE_EX_VERTICAL;
			break;
		case L"nopartial"_hash:
			ToolBarExStyles |= TBSTYLE_EX_HIDECLIPPEDBUTTONS;
			break;
		case L"mixedbuttons"_hash:
			ToolBarExStyles |= TBSTYLE_EX_MIXEDBUTTONS;
			break;
		case L"multicolumn"_hash:
			ToolBarExStyles |= TBSTYLE_EX_MULTICOLUMN;	// can't be used with TBSTYLE_WRAPABLE
			break;
		default:
			break;
		}
	}

	return ToolBarExStyles;
}

/*!
* \brief blah
*
* blah
*/
dcxWindowStyles DcxToolBar::parseControlStyles(const TString& tsStyles)
{
	auto ws = parseGeneralControlStyles(tsStyles);

	//Styles |= CCS_ADJUSTABLE;
	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"flat"_hash:
			ws.m_Styles |= TBSTYLE_FLAT;
			break;
		case L"check"_hash:
			ws.m_Styles |= BTNS_CHECK;
			break;
		case L"group"_hash:
			ws.m_Styles |= BTNS_GROUP;
			break;
		case L"dropdown"_hash:
			ws.m_Styles |= BTNS_DROPDOWN;
			break;
		case L"autosize"_hash:
			ws.m_Styles |= BTNS_AUTOSIZE;
			break;
		case L"noprefix"_hash:
			ws.m_Styles |= BTNS_NOPREFIX;
			break;
		case L"showtext"_hash:
			ws.m_Styles |= BTNS_SHOWTEXT;	// only applicable when used with TBSTYLE_EX_MIXEDBUTTONS
			break;
		case L"wholedropdown"_hash:
			ws.m_Styles |= BTNS_WHOLEDROPDOWN;
			break;
		case L"tooltips"_hash:
			ws.m_Styles |= TBSTYLE_TOOLTIPS;
			break;
		case L"transparent"_hash:
			ws.m_Styles |= TBSTYLE_TRANSPARENT;
			break;
		case L"nodivider"_hash:
			ws.m_Styles |= CCS_NODIVIDER;
			break;
		case L"top"_hash:
			ws.m_Styles |= CCS_TOP;
			break;
		case L"bottom"_hash:
			ws.m_Styles |= CCS_BOTTOM;
			break;
		case L"left"_hash:
			ws.m_Styles |= CCS_LEFT;
			break;
		case L"right"_hash:
			ws.m_Styles |= CCS_RIGHT;
			break;
			//case L"noresize"_hash: 
			//  Styles |= CCS_NORESIZE;
			//	break;
			//case L"noparentalign"_hash: 
			//  Styles |= CCS_NOPARENTALIGN ;
			//	break;
		case L"noauto"_hash:
			ws.m_Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
			break;
		case L"adjustable"_hash:
			ws.m_Styles |= CCS_ADJUSTABLE;
			break;
		case L"list"_hash:
			ws.m_Styles |= TBSTYLE_LIST;
			break;
		case L"wrap"_hash:
			ws.m_Styles |= TBSTYLE_WRAPABLE;
			break;
		case L"final"_hash:
			m_bOverrideTheme = true;
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
void DcxToolBar::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	const auto numtok = input.numtok();
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case L"num"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), this->getButtonCount());
		break;
		// [NAME] [ID] [PROP]
	case L"mouseitem"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), SendMessage(m_Hwnd, TB_GETHOTITEM, NULL, NULL));
		break;
		// [NAME] [ID] [PROP]
	case L"text"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iButton = input.getnexttok().to_int() - 1;	// tok 4

		if (iButton < 0 && iButton >= this->getButtonCount())
			throw DcxExceptions::dcxOutOfRange();

		if (auto lpdcxtbb = getButtonData(iButton); lpdcxtbb)
			szReturnValue = lpdcxtbb->bText.to_chr();
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"icon"_hash:
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Invalid number of arguments");
			throw DcxExceptions::dcxInvalidArguments();

		const auto iButton = input.getnexttok().to_int() - 1;	// tok 4

		if (iButton < 0 && iButton >= this->getButtonCount())
			//throw Dcx::dcxException("Out of Range");
			throw DcxExceptions::dcxOutOfRange();

		TBBUTTONINFO tbbi{ sizeof(TBBUTTONINFO),TBIF_IMAGE | TBIF_BYINDEX };

		this->getButtonInfo(iButton, &tbbi);

		_ts_snprintf(szReturnValue, TEXT("%d"), tbbi.iImage + 1);
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"state"_hash:
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Invalid number of arguments");
			throw DcxExceptions::dcxInvalidArguments();

		const auto iButton = input.getnexttok().to_int() - 1;	// tok 4

		if (iButton < 0 && iButton >= this->getButtonCount())
			//throw Dcx::dcxException("Out of Range");
			throw DcxExceptions::dcxOutOfRange();

		TBBUTTONINFO tbbi{ sizeof(TBBUTTONINFO),TBIF_STATE | TBIF_BYINDEX };

		this->getButtonInfo(iButton, &tbbi);

		szReturnValue = TEXT('+');

		if (!dcx_testflag(tbbi.fsState, TBSTATE_ENABLED))
			szReturnValue += TEXT('d');

		if (dcx_testflag(tbbi.fsState, TBSTATE_INDETERMINATE))
			szReturnValue += TEXT('i');

		if (dcx_testflag(tbbi.fsState, TBSTATE_HIDDEN))
			szReturnValue += TEXT('h');

		if (dcx_testflag(tbbi.fsState, TBSTATE_PRESSED))
			szReturnValue += TEXT('p');

		if (dcx_testflag(tbbi.fsState, TBSTATE_CHECKED))
			szReturnValue += TEXT('x');

		if (dcx_testflag(tbbi.fsState, TBSTATE_WRAP))
			szReturnValue += TEXT('w');
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"tooltip"_hash:
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Invalid number of arguments");
			throw DcxExceptions::dcxInvalidArguments();

		const auto iButton = input.getnexttok().to_int() - 1;	// tok 4

		if (iButton < 0 && iButton >= this->getButtonCount())
			//throw Dcx::dcxException("Out of Range");
			throw DcxExceptions::dcxOutOfRange();

		if (auto lpdcxtbb = getButtonData(iButton); lpdcxtbb)
			szReturnValue = lpdcxtbb->tsTipText.to_chr();
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"dropdownpoint"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iButton = input.getnexttok().to_int() - 1;	// tok 4

		if (iButton < 0 && iButton >= this->getButtonCount())
			throw DcxExceptions::dcxOutOfRange();

		RECT rc{};

		this->getItemRect(iButton, &rc);
		GSL_SUPPRESS(type.4)
			MapWindowRect(m_Hwnd, nullptr, &rc);

		_ts_snprintf(szReturnValue, TEXT("%d %d %d %d"), rc.left, rc.bottom, rc.right, rc.top);
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

void DcxToolBar::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		this->resetContent();

	// xdid -a [NAME] [ID] [SWITCH] [N] [+FLAGS] [WIDTH] [#ICON] [COLOR] (Button Text) [TAB] Tooltip Text
	if (flags[TEXT('a')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		auto nPos = input.getnexttok().to_int() - 1;	// tok 4

		if (nPos == -1)
			nPos += this->getButtonCount() + 1;

		const auto tsFlags(input.getnexttok());						// tok 5
		const auto width = input.getnexttok().to_<WORD>();			// tok 6
		const auto icon = input.getnexttok().to_int() - 1;			// tok 7
		const auto clrText = input.getnexttok().to_<COLORREF>();	// tok 8
		const auto iNumtok = input.gettok(1, TSTABCHAR).numtok();

		TBBUTTON tbb{};

		tbb.fsState = parseButtonStateFlags(tsFlags);
		tbb.idCommand = this->getFreeButtonID();
		const auto buttonStyles = parseButtonStyleFlags(tsFlags);
		tbb.fsStyle = gsl::narrow_cast<BYTE>(buttonStyles & 0xFF);

		if ((icon == -1) || (iNumtok < 7))
			tbb.iBitmap = I_IMAGENONE;
		else
			tbb.iBitmap = icon;

		TString itemtext;

		if (iNumtok > 8)
		{
			itemtext = input.gettok(1, TSTABCHAR).gettok(9, -1).trim();

			if (itemtext == TEXT('-'))
			{
				tbb.fsStyle = BTNS_SEP;
				//tbb.fsState = TBSTATE_ENABLED;
				tbb.iBitmap = icon;
				tbb.dwData = 0;
				tbb.iString = 0;

				this->insertButton(nPos, &tbb);
				this->autoSize();
				this->redrawWindow();
				return;
			}
		}

		// Tooltip Handling
		auto lpdcxtbb = std::make_unique<DCXTBBUTTON>();

		if (input.numtok(TSTABCHAR) > 1)
			lpdcxtbb->tsTipText = input.gettok(2, -1, TSTABCHAR).trim();

		lpdcxtbb->bUline = dcx_testflag(buttonStyles, dcxToolBar_Styles::BTNS_UNDERLINE);

		lpdcxtbb->bBold = dcx_testflag(buttonStyles, dcxToolBar_Styles::BTNS_BOLD);

		if (dcx_testflag(buttonStyles, dcxToolBar_Styles::BTNS_COLOR))
			lpdcxtbb->clrText = clrText;
		else
			lpdcxtbb->clrText = CLR_INVALID;

		lpdcxtbb->clrBtnFace = CLR_INVALID;
		lpdcxtbb->clrBtnHighlight = CLR_INVALID;
		lpdcxtbb->clrHighlightHotTrack = CLR_INVALID;
		lpdcxtbb->clrMark = CLR_INVALID;
		lpdcxtbb->clrTextHighlight = CLR_INVALID;
		lpdcxtbb->iTextBkgMode = TRANSPARENT;
		lpdcxtbb->iTextHighlightBkgMode = TRANSPARENT;

		lpdcxtbb->bText = itemtext;
		tbb.iString = reinterpret_cast<INT_PTR>(lpdcxtbb->bText.to_chr());
		tbb.dwData = reinterpret_cast<DWORD_PTR>(lpdcxtbb.release());

		// insert button
		this->insertButton(nPos, &tbb);
		//if (nPos == 1) // commented out for possible fix to http://dcx.scriptsdb.org/forum/showthread.php?tid=121 http://dcx.scriptsdb.org/bug/index.php?do=details&task_id=749
		//	SendMessage(m_Hwnd, TB_CHANGEBITMAP, nPos, MAKELPARAM(I_IMAGENONE,0)); // why was this added?

		// set width of button
		if (dcx_testflag(buttonStyles, dcxToolBar_Styles::BTNS_WIDTH))
		{
			TBBUTTONINFO tbbi{};
			tbbi.cbSize = sizeof(TBBUTTONINFO);

			tbbi.dwMask = TBIF_SIZE;
			tbbi.cx = width;
			this->setButtonInfo(tbb.idCommand, &tbbi);
		}

		this->autoSize();
		if (this->m_bAutoStretch)
			this->autoStretchButtons();
	}
	// xdid -c [NAME] [ID] [SWITCH] [N] [+FLAGS] [RGB] [+REMOVEFLAGS]
	else if (flags[TEXT('c')])
	{
		if (numtok < 7)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iButton = input.getnexttok().to_int() - 1;						// tok 4
		const auto buttonStyles = parseButtonStyleFlags(input.getnexttok());		// tok 5
		const auto clrColor = input.getnexttok().to_<COLORREF>();					// tok 6
		const auto removeButtonStyles = parseButtonStyleFlags(input.getnexttok());	// tok 7

		if (iButton == -1 && this->m_ToolTipHWND)
		{
			if (dcx_testflag(buttonStyles, dcxToolBar_Styles::BTNS_TBKGCOLOR))
				SendMessage(this->m_ToolTipHWND, TTM_SETTIPBKCOLOR, gsl::narrow_cast<WPARAM>(clrColor), NULL);
			else if (dcx_testflag(buttonStyles, dcxToolBar_Styles::BTNS_TTXTCOLOR))
				SendMessage(this->m_ToolTipHWND, TTM_SETTIPTEXTCOLOR, gsl::narrow_cast<WPARAM>(clrColor), NULL);
		}
		else {

			if (iButton < 0 && iButton >= this->getButtonCount())
				throw DcxExceptions::dcxInvalidArguments();

			if (auto lpdcxtbb = getButtonData(iButton); lpdcxtbb)
			{
				if (dcx_testflag(buttonStyles, dcxToolBar_Styles::BTNS_UNDERLINE))
					lpdcxtbb->bUline = true;

				if (dcx_testflag(buttonStyles, dcxToolBar_Styles::BTNS_BOLD))
					lpdcxtbb->bBold = true;

				if (dcx_testflag(buttonStyles, dcxToolBar_Styles::BTNS_COLOR))
					lpdcxtbb->clrText = clrColor;

				if (dcx_testflag(buttonStyles, dcxToolBar_Styles::BTNS_HIGHLIGHT_TXTCOLOR))
					lpdcxtbb->clrTextHighlight = clrColor;

				if (dcx_testflag(buttonStyles, dcxToolBar_Styles::BTNS_MARK_BKGCOLOR))
					lpdcxtbb->clrMark = clrColor;

				if (dcx_testflag(buttonStyles, dcxToolBar_Styles::BTNS_BTNCOLOR))
					lpdcxtbb->clrBtnFace = clrColor;

				if (dcx_testflag(buttonStyles, dcxToolBar_Styles::BTNS_HIGHLIGHT_BTNCOLOR))
					lpdcxtbb->clrBtnHighlight = clrColor;

				if (dcx_testflag(buttonStyles, dcxToolBar_Styles::BTNS_HOTTRACK_BTNCOLOR))
					lpdcxtbb->clrHighlightHotTrack = clrColor;

				if (dcx_testflag(removeButtonStyles, dcxToolBar_Styles::BTNS_UNDERLINE))
					lpdcxtbb->bUline = false;

				if (dcx_testflag(removeButtonStyles, dcxToolBar_Styles::BTNS_BOLD))
					lpdcxtbb->bBold = false;

				if (dcx_testflag(removeButtonStyles, dcxToolBar_Styles::BTNS_COLOR))
					lpdcxtbb->clrText = CLR_INVALID;

				if (dcx_testflag(removeButtonStyles, dcxToolBar_Styles::BTNS_HIGHLIGHT_TXTCOLOR))
					lpdcxtbb->clrTextHighlight = CLR_INVALID;

				if (dcx_testflag(removeButtonStyles, dcxToolBar_Styles::BTNS_MARK_BKGCOLOR))
					lpdcxtbb->clrMark = CLR_INVALID;

				if (dcx_testflag(removeButtonStyles, dcxToolBar_Styles::BTNS_BTNCOLOR))
					lpdcxtbb->clrBtnFace = CLR_INVALID;

				if (dcx_testflag(removeButtonStyles, dcxToolBar_Styles::BTNS_HIGHLIGHT_BTNCOLOR))
					lpdcxtbb->clrBtnHighlight = CLR_INVALID;

				if (dcx_testflag(removeButtonStyles, dcxToolBar_Styles::BTNS_HOTTRACK_BTNCOLOR))
					lpdcxtbb->clrHighlightHotTrack = CLR_INVALID;
			}
			redrawWindow();
		}
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		//if (const auto nButton = input.getnexttok().to_int() - 1; nButton > -1)
		//	this->deleteButton(nButton);

		const TString tsButton(input.getnexttok());
		const auto HandleButton = [=](const TString& tsButtons) {
			const auto r = Dcx::make_range(tsButtons, this->getButtonCount());

			if ((r.b < 0) || (r.e < r.b))
				throw DcxExceptions::dcxOutOfRange();

			for (auto nButton : r)
				this->deleteButton(nButton);
		};
		if (tsButton.numtok(TSCOMMACHAR) > 1)
		{
			// button == 1,2,3-4
			const auto itEnd = tsButton.end();
			for (auto itStart = tsButton.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
			{
				HandleButton(*itStart);
			}
		}
		else {
			// button == 3-4 or 3
			HandleButton(tsButton);
		}
	}
	// xdid -i [NAME] [ID] [SWITCH] [N] [IMAGE]
	else if (flags[TEXT('i')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		//const auto nButton = input.getnexttok().to_int() - 1;	// tok 4
		//auto iImage = input.getnexttok().to_int() - 1;			// tok 5
		//
		//if (iImage < 0)
		//	iImage = I_IMAGENONE;
		//
		//TBBUTTONINFO tbbi{};
		//tbbi.cbSize = sizeof(TBBUTTONINFO);
		//tbbi.dwMask = TBIF_IMAGE | TBIF_BYINDEX;
		//tbbi.iImage = iImage;
		//
		//this->setButtonInfo(nButton /*this->getIndexToCommand( nButton )*/, &tbbi);

		const TString tsButton(input.getnexttok());
		auto iImage = input.getnexttok().to_int() - 1;			// tok 5
		
		if (iImage < 0)
			iImage = I_IMAGENONE;

		const auto HandleButton = [=](const TString& tsButtons) {
			const auto r = Dcx::make_range(tsButtons, this->getButtonCount());

			if ((r.b < 0) || (r.e < r.b))
				throw DcxExceptions::dcxOutOfRange();

			TBBUTTONINFO tbbi{};
			tbbi.cbSize = sizeof(TBBUTTONINFO);
			tbbi.dwMask = TBIF_IMAGE | TBIF_BYINDEX;
			tbbi.iImage = iImage;
			
			for (auto nButton: r)
				this->setButtonInfo(nButton, &tbbi);
		};
		if (tsButton.numtok(TSCOMMACHAR) > 1)
		{
			// button == 1,2,3-4
			const auto itEnd = tsButton.end();
			for (auto itStart = tsButton.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
			{
				HandleButton(*itStart);
			}
		}
		else {
			// button == 3-4 or 3
			HandleButton(tsButton);
		}
	}
	// xdid -j -> [NAME] [ID] -j [MIN] [MAX]
	else if (flags[TEXT('j')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nMin = input.getnexttok().to_int();	// tok 4
		const auto nMax = input.getnexttok().to_int();	// tok 5	// was tok 4 should be 5

		this->setButtonWidth(nMin, nMax);
	}
	// xdid -l [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('l')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto size = NumToIconSize(input.getnexttok().to_<UINT>());	// tok 4

		// these are destroyed after being removed from toolbar by setImageList() function.
		//ImageList_Destroy(this->getImageList(TB_IML_NORMAL));
		//ImageList_Destroy(this->getImageList(TB_IML_DISABLE));
		//ImageList_Destroy(this->getImageList(TB_IML_HOT));

		setImageList(createImageList(size), dcxToolBar_ImageLists::TB_IML_NORMAL);

		setImageList(createImageList(size), dcxToolBar_ImageLists::TB_IML_DISABLE);

		setImageList(createImageList(size), dcxToolBar_ImageLists::TB_IML_HOT);

		redrawWindow();
	}
	// xdid -m [NAME] [ID] [SWITCH] [1|0]
	else if (flags[TEXT('m')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		m_bAutoStretch = (input.getnexttok() == TEXT('1'));	// tok 4

		if (m_bAutoStretch)
			autoStretchButtons();
	}
	// xdid -q [NAME] [ID] [SWITCH] [N] (TIPTEXT)
	else if (flags[TEXT('q')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nButton = input.getnexttok().to_int() - 1;	// tok 4

		if (nButton < 0 && nButton >= this->getButtonCount())
			throw DcxExceptions::dcxOutOfRange();

		if (auto lpdcxtbb = getButtonData(nButton); lpdcxtbb)
		{
			if (numtok > 4)	// has a new tooltip
				lpdcxtbb->tsTipText = input.getlasttoks();	// tok 5, -1
			else					// no tooltip
				lpdcxtbb->tsTipText.clear();	// = TEXT("");
		}
	}
	// This is to avoid an invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')])
	{
	}
	// xdid -t [NAME] [ID] [SWITCH] [N] [+FLAGS]
	else if (flags[TEXT('t')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		//const auto nButton = input.getnexttok().to_int() - 1;				// tok 4
		//const auto fStates = parseButtonStateFlags(input.getnexttok());	// tok 5
		//
		//const auto idButton = this->getIndexToCommand(nButton);
		//
		//if (idButton > 0)
		//	this->setButtonState(idButton, fStates);

		const auto tsButton = input.getnexttok();						// tok 4
		const auto fStates = parseButtonStateFlags(input.getnexttok());	// tok 5
		const auto HandleButton = [=](const TString& tsButtons) {
			const auto r = Dcx::make_range(tsButtons, this->getButtonCount());

			if ((r.b < 0) || (r.e < r.b))
				throw DcxExceptions::dcxOutOfRange();

			for (auto nButton: r)
			{
				if (const auto idButton = this->getIndexToCommand(nButton); idButton > 0)
					this->setButtonState(idButton, fStates);
			}
		};

		if (tsButton.numtok(TSCOMMACHAR) > 1)
		{
			// button == 1,2,3-4....
			const auto itEnd = tsButton.end();
			for (auto itStart = tsButton.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
			{
				HandleButton(*itStart);
			}
		}
		else {
			// button = 3-4 or 3
			HandleButton(tsButton);
		}

	}
	// xdid -u [NAME] [ID] [SWITCH] [DX] [DY]
	else if (flags[TEXT('u')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto dxButton = input.getnexttok().to_int();		// tok 4
		const auto dyButton = input.getnexttok().to_int();		// tok 5

		this->setButtonSize(dxButton, dyButton);
	}
	// xdid -v [NAME] [ID] [SWITCH] [N] (TEXT)
	else if (flags[TEXT('v')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nButton = input.getnexttok().to_int() - 1;		// tok 4

		if (nButton < 0 && nButton >= this->getButtonCount())
			throw DcxExceptions::dcxOutOfRange();

		const auto nIndex = this->getIndexToCommand(nButton);
		TBBUTTONINFO tbbi{};

		tbbi.cbSize = sizeof(TBBUTTONINFO);
		tbbi.dwMask = TBIF_LPARAM;

		if (this->getButtonInfo(nIndex, &tbbi) < 0)
			throw Dcx::dcxException("Unable to get button info");

		if (auto lpdcxtbb = reinterpret_cast<LPDCXTBBUTTON>(tbbi.lParam); lpdcxtbb)
		{
			if (numtok > 4)
				lpdcxtbb->bText = input.getlasttoks();	// tok 5, -1
			else
				lpdcxtbb->bText.clear();	// = TEXT("");

			ZeroMemory(&tbbi, sizeof(TBBUTTONINFO));
			tbbi.cbSize = sizeof(TBBUTTONINFO);
			tbbi.dwMask = TBIF_TEXT;
			tbbi.pszText = lpdcxtbb->bText.to_chr();
			this->setButtonInfo(nIndex, &tbbi);
		}
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tsFlags(input.getnexttok());	// tok 4
		const auto iFlags = this->parseImageListFlags(tsFlags);

		if (tsFlags[0] != TEXT('+'))
			throw DcxExceptions::dcxInvalidFlag();

		const auto index = input.getnexttok().to_int();	// tok 5
		auto filename(input.getlasttoks());			// tok 6, -1

		int cx = 0, cy = 0;
		if (!ImageList_GetIconSize(this->getImageList(dcxToolBar_ImageLists::TB_IML_NORMAL), &cx, &cy))
			throw Dcx::dcxException("Unable to get Icon Size");

		// load the icon
#if DCX_USE_WRAPPERS
		const Dcx::dcxIconResource icon(index, filename, (NumToIconSize(cx) > DcxIconSizes::SmallIcon), tsFlags);

		// NORMAL IML
		if (dcx_testflag(iFlags, dcxToolBar_ImageLists::TB_IML_NORMAL))
		{
			if (auto himl = this->getImageList(dcxToolBar_ImageLists::TB_IML_NORMAL); himl)
				ImageList_AddIcon(himl, icon.get());
		}

		// DISABLED IML
		if (dcx_testflag(iFlags, dcxToolBar_ImageLists::TB_IML_DISABLE))
		{
			if (auto himl = this->getImageList(dcxToolBar_ImageLists::TB_IML_DISABLE); himl)
				ImageList_AddIcon(himl, icon.get());
		}

		// HOT IML
		if (dcx_testflag(iFlags, dcxToolBar_ImageLists::TB_IML_HOT))
		{
			if (auto himl = this->getImageList(dcxToolBar_ImageLists::TB_IML_HOT); himl)
				ImageList_AddIcon(himl, icon.get());
		}
#else
		const auto icon = dcxLoadIcon(index, filename, (NumToIconSize(cx) > DcxIconSizes::SmallIcon), tsFlags);

		// if there is an icon to process
		if (!icon)
			throw Dcx::dcxException(TEXT("Icon Failed To Load: %"), filename);

		Auto(DestroyIcon(icon));

		// NORMAL IML
		if (dcx_testflag(iFlags, TB_IML_NORMAL))
		{
			if (auto himl = this->getImageList(TB_IML_NORMAL); himl)
				ImageList_AddIcon(himl, icon);
		}

		// DISABLED IML
		if (dcx_testflag(iFlags, TB_IML_DISABLE))
		{
			if (auto himl = this->getImageList(TB_IML_DISABLE); himl)
				ImageList_AddIcon(himl, icon);
		}

		// HOT IML
		if (dcx_testflag(iFlags, TB_IML_HOT))
		{
			if (auto himl = this->getImageList(TB_IML_HOT); himl)
				ImageList_AddIcon(himl, icon);
		}
#endif
	}
	else
		parseGlobalCommandRequest(input, flags);
}

/*!
* \brief blah
*
* blah
*/

BYTE DcxToolBar::parseButtonStateFlags(const TString& flags) noexcept
{
	const XSwitchFlags xflags(flags);
	BYTE iFlags = TBSTATE_ENABLED;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('d')])
		iFlags &= ~TBSTATE_ENABLED;
	if (xflags[TEXT('h')])
		iFlags |= TBSTATE_HIDDEN;
	if (xflags[TEXT('i')])
		iFlags |= TBSTATE_INDETERMINATE;
	if (xflags[TEXT('p')])
		iFlags |= TBSTATE_PRESSED;
	if (xflags[TEXT('x')])
		iFlags |= TBSTATE_CHECKED;
	if (xflags[TEXT('w')])
		iFlags |= TBSTATE_WRAP;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxToolBar::parseButtonStyleFlags(const TString& flags) noexcept
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = BTNS_BUTTON;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('a')])
		iFlags |= BTNS_AUTOSIZE;
	if (xflags[TEXT('b')])
		iFlags |= dcxToolBar_Styles::BTNS_BOLD;
	if (xflags[TEXT('B')])
		iFlags |= dcxToolBar_Styles::BTNS_BTNCOLOR;
	if (xflags[TEXT('c')])
		iFlags |= dcxToolBar_Styles::BTNS_COLOR;
	if (xflags[TEXT('g')])
		iFlags |= BTNS_GROUP;
	if (xflags[TEXT('h')])
		iFlags |= dcxToolBar_Styles::BTNS_HIGHLIGHT_TXTCOLOR;
	if (xflags[TEXT('H')])
		iFlags |= dcxToolBar_Styles::BTNS_HIGHLIGHT_BTNCOLOR;
	if (xflags[TEXT('k')])
		iFlags |= BTNS_CHECK;
	if (xflags[TEXT('l')])
		iFlags |= dcxToolBar_Styles::BTNS_WIDTH;
	if (xflags[TEXT('m')])
		iFlags |= dcxToolBar_Styles::BTNS_MARK_BKGCOLOR;
	if (xflags[TEXT('T')])
		iFlags |= dcxToolBar_Styles::BTNS_HOTTRACK_BTNCOLOR;
	if (xflags[TEXT('u')])
		iFlags |= dcxToolBar_Styles::BTNS_UNDERLINE;
	if (xflags[TEXT('v')])
		iFlags |= BTNS_DROPDOWN;
	if (xflags[TEXT('x')])
		iFlags |= dcxToolBar_Styles::BTNS_TBKGCOLOR;
	if (xflags[TEXT('z')])
		iFlags |= dcxToolBar_Styles::BTNS_TTXTCOLOR;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/
UINT DcxToolBar::parseImageListFlags(const TString& flags) noexcept
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('n')])
		iFlags |= dcxToolBar_ImageLists::TB_IML_NORMAL;
	if (xflags[TEXT('d')])
		iFlags |= dcxToolBar_ImageLists::TB_IML_DISABLE;
	if (xflags[TEXT('h')])
		iFlags |= dcxToolBar_ImageLists::TB_IML_HOT;
	if (xflags[TEXT('g')])
		iFlags |= dcxToolBar_ImageLists::TB_ICO_GREY;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

HIMAGELIST DcxToolBar::getImageList(const dcxToolBar_ImageLists iImageList) const noexcept
{
	if (iImageList == dcxToolBar_ImageLists::TB_IML_NORMAL)
		return reinterpret_cast<HIMAGELIST>(SendMessage(m_Hwnd, TB_GETIMAGELIST, 0U, 0));
	else if (iImageList == dcxToolBar_ImageLists::TB_IML_DISABLE)
		return reinterpret_cast<HIMAGELIST>(SendMessage(m_Hwnd, TB_GETDISABLEDIMAGELIST, 0U, 0));
	else if (iImageList == dcxToolBar_ImageLists::TB_IML_HOT)
		return reinterpret_cast<HIMAGELIST>(SendMessage(m_Hwnd, TB_GETHOTIMAGELIST, 0U, 0));
	return nullptr;
}

/*!
* \brief blah
*
* blah
*/

void DcxToolBar::setImageList(const HIMAGELIST himl, const dcxToolBar_ImageLists iImageList) noexcept
{
	HIMAGELIST himlOld{ nullptr };

	if (iImageList == dcxToolBar_ImageLists::TB_IML_NORMAL)
		himlOld = reinterpret_cast<HIMAGELIST>(SendMessage(m_Hwnd, TB_SETIMAGELIST, 0U, reinterpret_cast<LPARAM>(himl)));
	else if (iImageList == dcxToolBar_ImageLists::TB_IML_DISABLE)
		himlOld = reinterpret_cast<HIMAGELIST>(SendMessage(m_Hwnd, TB_SETDISABLEDIMAGELIST, 0U, reinterpret_cast<LPARAM>(himl)));
	else if (iImageList == dcxToolBar_ImageLists::TB_IML_HOT)
		himlOld = reinterpret_cast<HIMAGELIST>(SendMessage(m_Hwnd, TB_SETHOTIMAGELIST, 0U, reinterpret_cast<LPARAM>(himl)));

	if (himlOld)
		ImageList_Destroy(himlOld);
}

/*!
* \brief blah
*
* blah
*/

HIMAGELIST DcxToolBar::createImageList(const DcxIconSizes iSize) noexcept
{
	return ImageList_Create(gsl::narrow_cast<int>(iSize), gsl::narrow_cast<int>(iSize), ILC_COLOR32 | ILC_MASK, 1, 0);
}

/*!
* \brief blah
*
* blah
*/

void DcxToolBar::resetContent() noexcept
{
	auto nButtons = this->getButtonCount();

	while (nButtons--)
		this->deleteButton(nButtons);
}

/*!
* \brief blah
*
* blah
*/

void DcxToolBar::autoStretchButtons() noexcept
{
	const auto nButtons = this->getButtonCount();
	auto nTotalButtons = nButtons;

	RECT rc{};

	if (!GetClientRect(m_Hwnd, &rc))
		return;

	auto leftwidth = rc.right - rc.left;

	TBBUTTON tbb{};

	// Find the button width
	for (auto i = decltype(nButtons){0}; i < nButtons; ++i)
	{
		getButton(i, &tbb);

		if (dcx_testflag(tbb.fsStyle, BTNS_SEP))
		{
			//RECT rcItem{};
			//getItemRect(i, &rcItem);

			if (auto [code, rcItem] = getItemRect(i); code)
			{
				leftwidth -= rcItem.right - rcItem.left;
			}
			--nTotalButtons;
		}
	}

	// Apply the new width
	TBBUTTONINFO tbbi{};
	tbbi.cbSize = sizeof(TBBUTTONINFO);
	tbbi.dwMask = TBIF_SIZE;
	//tbbi.cx = (WORD)( (double) leftwidth / (double) nTotalButtons );
	tbbi.cx = gsl::narrow_cast<WORD>(dcx_round(gsl::narrow_cast<float>(gsl::narrow_cast<double>(leftwidth) / gsl::narrow_cast<double>(nTotalButtons))));

	// Make sure the size is positive
	if (tbbi.cx > 0)
	{
		for (auto i = decltype(nButtons){0}; i < nButtons; ++i)
		{
			this->getButton(i, &tbb);

			if (dcx_testflag(tbb.fsStyle, BTNS_SEP))
				continue;

			this->setButtonInfo(this->getIndexToCommand(i), &tbbi);
		}
	}
}

void DcxToolBar::autoPosition(const int width, const int height) noexcept
{
	if (!this->isStyle(WindowStyle::CCS_NoParentAlign | CCS_NORESIZE))
		SendMessage(m_Hwnd, WM_SIZE, 0U, 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::autoSize() noexcept
{
	return SendMessage(m_Hwnd, TB_AUTOSIZE, 0U, 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::insertButton(const int nPos, LPCTBBUTTON lpbb) noexcept
{
	return SendMessage(m_Hwnd, TB_INSERTBUTTON, gsl::narrow_cast<WPARAM>(nPos), reinterpret_cast<LPARAM>(lpbb));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::hitTest(const LPPOINT lpt) const noexcept
{
	return SendMessage(m_Hwnd, TB_HITTEST, 0U, reinterpret_cast<LPARAM>(lpt));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getItemRect(const int iButton, LPCRECT lprc) const noexcept
{
	return SendMessage(m_Hwnd, TB_GETITEMRECT, gsl::narrow_cast<WPARAM>(iButton), reinterpret_cast<LPARAM>(lprc));
}

Dcx::CodeValue<RECT> DcxToolBar::getItemRect(const int iButton) const noexcept
{
	RECT rc{};
	return { SendMessage(m_Hwnd, TB_GETITEMRECT, gsl::narrow_cast<WPARAM>(iButton), reinterpret_cast<LPARAM>(std::addressof(rc))), rc };
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getButtonCount() const noexcept
{
	return SendMessage(m_Hwnd, TB_BUTTONCOUNT, 0U, 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::setButtonWidth(const int cxMin, const int cxMax) noexcept
{
	return SendMessage(m_Hwnd, TB_SETBUTTONWIDTH, 0U, gsl::narrow_cast<LPARAM>(MAKELONG(cxMin, cxMax)));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::deleteButton(const int iButton) noexcept
{
	return static_cast<LRESULT>(SendMessage(m_Hwnd, TB_DELETEBUTTON, gsl::narrow_cast<WPARAM>(iButton), 0));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::setButtonInfo(const int idButton, const LPTBBUTTONINFO lpbi) noexcept
{
	return SendMessage(m_Hwnd, TB_SETBUTTONINFO, gsl::narrow_cast<WPARAM>(idButton), reinterpret_cast<LPARAM>(lpbi));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getButtonInfo(const int idButton, const LPTBBUTTONINFO lpbi) const noexcept
{
	return SendMessage(m_Hwnd, TB_GETBUTTONINFO, gsl::narrow_cast<WPARAM>(idButton), reinterpret_cast<LPARAM>(lpbi));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getTooltips() const noexcept
{
	return SendMessage(m_Hwnd, TB_GETTOOLTIPS, 0U, 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getButtonText(const int idButton, const LPSTR lpszText) const noexcept
{
	return static_cast<LRESULT>(SendMessage(m_Hwnd, TB_GETBUTTONTEXT, gsl::narrow_cast<WPARAM>(idButton), reinterpret_cast<LPARAM>(lpszText)));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getButton(const int iButton, const LPTBBUTTON lpb) const noexcept
{
	return SendMessage(m_Hwnd, TB_GETBUTTON, gsl::narrow_cast<WPARAM>(iButton), reinterpret_cast<LPARAM>(lpb));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getCommandToIndex(const int idButton) const noexcept
{
	return SendMessage(m_Hwnd, TB_COMMANDTOINDEX, gsl::narrow_cast<WPARAM>(idButton), 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::moveButton(const int iButtonFrom, const int iButtonTo) noexcept
{
	return SendMessage(m_Hwnd, TB_MOVEBUTTON, gsl::narrow_cast<WPARAM>(iButtonFrom), gsl::narrow_cast<LPARAM>(iButtonTo));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::markButton(const int iButton, const bool fHighlight) noexcept
{
	//return SendMessage(m_Hwnd, TB_MARKBUTTON, gsl::narrow_cast<WPARAM>(iButton), gsl::narrow_cast<LPARAM>(MAKELONG(fHighlight, 0)));
	//return SendMessage(m_Hwnd, TB_MARKBUTTON, gsl::narrow_cast<WPARAM>(iButton), MAKELPARAM(fHighlight, 0));
	return SendMessage(m_Hwnd, TB_MARKBUTTON, gsl::narrow_cast<WPARAM>(iButton), Dcx::dcxMAKELPARAM(gsl::narrow_cast<uint8_t>(fHighlight), gsl::narrow_cast<uint8_t>(0)));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::setButtonState(const int idButton, const UINT fState) noexcept
{
	//return SendMessage(m_Hwnd, TB_SETSTATE, gsl::narrow_cast<WPARAM>(idButton), gsl::narrow_cast<LPARAM>(MAKELONG(fState, 0)));
	//return SendMessage(m_Hwnd, TB_SETSTATE, gsl::narrow_cast<WPARAM>(idButton), MAKELPARAM(fState, 0));
	return SendMessage(m_Hwnd, TB_SETSTATE, gsl::narrow_cast<WPARAM>(idButton), Dcx::dcxMAKELPARAM(gsl::narrow_cast<UINT>(fState), gsl::narrow_cast<UINT>(0)));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::setButtonSize(const int dxButton, const int dyButton) noexcept
{
	//return SendMessage(m_Hwnd, TB_SETBUTTONSIZE, 0U, gsl::narrow_cast<LPARAM>(MAKELONG(dxButton, dyButton)));
	//return SendMessage(m_Hwnd, TB_SETBUTTONSIZE, 0U, MAKELPARAM(dxButton, dyButton));
	return SendMessage(m_Hwnd, TB_SETBUTTONSIZE, 0U, Dcx::dcxMAKELPARAM(dxButton, dyButton));
}

LPDCXTBBUTTON DcxToolBar::getButtonData(const int idButton) const
{
	TBBUTTONINFO tbbi{ sizeof(TBBUTTONINFO),TBIF_LPARAM | TBIF_BYINDEX };

	if (this->getButtonInfo(idButton, &tbbi) < 0)
		throw Dcx::dcxException("Unable to get button info");

	return reinterpret_cast<LPDCXTBBUTTON>(tbbi.lParam);
}


/*!
* \brief blah
*
* blah
*/

int DcxToolBar::getFreeButtonID() const noexcept
{
	int i = 0, iID = 0;
	do {
		iID = this->getCommandToIndex(++i);
	} while (iID != -1);
	return i;
}

/*!
* \brief blah
*
* blah
*/

int DcxToolBar::getIndexToCommand(const int iIndex) const noexcept
{
	const auto nButtons = this->getButtonCount();

	for (auto i = decltype(nButtons){1}; i <= nButtons; ++i)
	{
		if (this->getCommandToIndex(i) == iIndex)
			return i;
	}

	return 0;
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxToolBar::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		if (Dcx::dcxHIWORD(wParam) == BN_CLICKED)
		{
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			{
				if (const auto iButton = gsl::narrow_cast<int>(Dcx::dcxLOWORD(wParam)); iButton > -1)
					this->execAliasEx(TEXT("sclick,%u,%d"), getUserID(), this->getCommandToIndex(iButton) + 1);
			}
		}
		bParsed = TRUE;
	}
	break;

	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (!hdr)
			break;

		switch (hdr->code)
		{
			//case NM_CLICK:	// WM_COMMAND is used instead, keep for reference
			//{
			//	if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			//	{
			//		dcxlParam(LPNMCLICK, lpnm);
			//		this->execAliasEx(TEXT("nm_sclick,%u,%u"), getUserID(), lpnm->dwItemSpec);
			//	}
			//	bParsed = TRUE;
			//	return TRUE;
			//}
			//break;

		case NM_RCLICK:
		{
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			{
				dcxlParam(LPNMCLICK, lpnm);

				const auto iIndex = this->getCommandToIndex(lpnm->dwItemSpec);
				if (auto [code, rc] = this->getItemRect(iIndex); code)
				{
					MapWindowRect(m_Hwnd, nullptr, &rc);
					this->execAliasEx(TEXT("rclick,%u,%u,%d,%d,%d,%d"), getUserID(), iIndex + 1, rc.left, rc.bottom, rc.right, rc.top);
				}
			}
			bParsed = TRUE;
			return TRUE;
		}
		break;

		case TBN_DROPDOWN:
		{
			LRESULT lRes = TBDDRET_DEFAULT;
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			{
				dcxlParam(LPNMTOOLBAR, lpnmtb);

				const auto iIndex = this->getCommandToIndex(lpnmtb->iItem);

				if (auto [code, rc] = this->getItemRect(iIndex); code)
				{
					MapWindowRect(m_Hwnd, nullptr, &rc);

					// return false to have it treat the dropdown as a normal button press.
					if (!this->execAliasEx(TEXT("dropdown,%u,%d,%d,%d,%d,%d"), getUserID(), iIndex + 1, rc.left, rc.bottom, rc.right, rc.top))
						lRes = TBDDRET_TREATPRESSED;
					//else
					//	lRes = TBDDRET_NODEFAULT;
				}
			}
			bParsed = TRUE;
			return lRes;
		}
		break;

		case NM_CUSTOMDRAW:
		{
			dcxlParam(LPNMTBCUSTOMDRAW, lpntbcd);

			bParsed = TRUE;
			switch (lpntbcd->nmcd.dwDrawStage)
			{
			case CDDS_PREPAINT:
				return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW);

			case CDDS_ITEMPREPAINT:
			{

				const auto lpdtbb = reinterpret_cast<LPDCXTBBUTTON>(lpntbcd->nmcd.lItemlParam);

				if (!lpdtbb)
					return CDRF_DODEFAULT;

				DWORD dFlags = (CDRF_NOTIFYPOSTPAINT | CDRF_NEWFONT);

				if (lpdtbb->clrBtnFace != CLR_INVALID)
					lpntbcd->clrBtnFace = lpdtbb->clrBtnFace;
				if (lpdtbb->clrBtnHighlight != CLR_INVALID)
					lpntbcd->clrBtnHighlight = lpdtbb->clrBtnHighlight;
				if (lpdtbb->clrHighlightHotTrack != CLR_INVALID) {
					lpntbcd->clrHighlightHotTrack = lpdtbb->clrHighlightHotTrack;
					dFlags |= TBCDRF_HILITEHOTTRACK;
				}
				if (lpdtbb->clrMark != CLR_INVALID)
					lpntbcd->clrMark = lpdtbb->clrMark;
				if (lpdtbb->clrText != CLR_INVALID)
					lpntbcd->clrText = lpdtbb->clrText;
				if (lpdtbb->clrTextHighlight != CLR_INVALID)
					lpntbcd->clrTextHighlight = lpdtbb->clrTextHighlight;

				if (const auto hFont = GetWindowFont(m_Hwnd); hFont)
				{
					if (auto [code, lf] = Dcx::dcxGetObject<LOGFONT>(hFont); code != 0)
					{
						if (lpdtbb->bBold)
							lf.lfWeight |= FW_BOLD;
						else
							lf.lfWeight &= ~FW_BOLD;
						if (lpdtbb->bUline)
							lf.lfUnderline = TRUE;
						else
							lf.lfUnderline = FALSE;

						this->m_hItemFont = CreateFontIndirect(&lf);
						if (this->m_hItemFont)
							this->m_hOldItemFont = Dcx::dcxSelectObject(lpntbcd->nmcd.hdc, this->m_hItemFont);
					}
				}
				// allows custom colours even when control is themed. (Vista Only)
				if (this->m_bOverrideTheme)
					dFlags |= TBCDRF_USECDCOLORS;

				return gsl::narrow_cast<LRESULT>(dFlags);
			}

			case CDDS_ITEMPOSTPAINT:
				//return CDRF_DODEFAULT;
			{
				if (this->m_hOldItemFont)
				{
					Dcx::dcxSelectObject(lpntbcd->nmcd.hdc, this->m_hOldItemFont);
					this->m_hOldItemFont = nullptr;
				}
				if (this->m_hItemFont)
				{
					DeleteObject(this->m_hItemFont);
					this->m_hItemFont = nullptr;
				}
				return CDRF_DODEFAULT;
			}

			default:
				return CDRF_DODEFAULT;
			}
		}
		break;

		case TBN_GETINFOTIP:
		{
			if (dcxlParam(LPNMTBGETINFOTIP, tcgit); tcgit)
			{
				if (auto lpdtbb = reinterpret_cast<LPDCXTBBUTTON>(tcgit->lParam); ((lpdtbb) && (tcgit->pszText)))
					dcx_strcpyn(tcgit->pszText, lpdtbb->tsTipText.to_chr(), tcgit->cchTextMax);
			}
			bParsed = TRUE;
		}
		break;

		//case TTN_GETDISPINFO:
		//	{
		//		LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam;
		//
		//		idButton = lpttt->hdr.idFrom;
		//	}
		//	break;

		case TBN_DELETINGBUTTON:
		{
			dcxlParam(LPNMTOOLBAR, lpnmtb);

			TBBUTTON tbb{};

			this->getButton(this->getCommandToIndex(lpnmtb->iItem), &tbb);

			auto lpBut = reinterpret_cast<LPDCXTBBUTTON>(tbb.dwData);

			delete lpBut;

			bParsed = TRUE;
		}
		break;

		// NM_CHAR code all works, but do we want it?
		//case NM_CHAR:
		//	{
		//		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT)) {
		//			auto lpnmc = (LPNMCHAR) lParam;
		//			this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("keyup"), this->getUserID(), lpnmc->ch, lpnmc->dwItemPrev);
		//		}
		//		bParsed = TRUE;
		//	}
		//	break;

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

LRESULT DcxToolBar::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_CONTEXTMENU:
	case WM_LBUTTONUP:
		break;

	case WM_SIZE:
	{
		if (this->m_bAutoStretch)
			this->autoStretchButtons();
	}
	break;

	case WM_PAINT:
	{
		if (!this->IsAlphaBlend())
			break;

		PAINTSTRUCT ps{};
		auto hdc = BeginPaint(m_Hwnd, &ps);
		Auto(EndPaint(m_Hwnd, &ps));

		bParsed = TRUE;

		// Setup alpha blend if any.
		auto ai = this->SetupAlphaBlend(&hdc);
		Auto(this->FinishAlphaBlend(ai));

		return CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
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

const TString DcxToolBar::getStyles(void) const
{
	auto tsStyles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);
	const auto ExStylesTb = SendMessage(m_Hwnd, TB_GETEXTENDEDSTYLE, 0U, 0L);

	if (dcx_testflag(Styles, TBSTYLE_FLAT))
		tsStyles.addtok(TEXT("flat"));
	if (dcx_testflag(Styles, TBSTYLE_TOOLTIPS))
		tsStyles.addtok(TEXT("tooltips"));
	if (dcx_testflag(Styles, TBSTYLE_TRANSPARENT))
		tsStyles.addtok(TEXT("transparent"));
	if (dcx_testflag(Styles, CCS_NODIVIDER))
		tsStyles.addtok(TEXT("nodivider"));
	if (dcx_testflag(Styles, CCS_TOP))
		tsStyles.addtok(TEXT("top"));
	if (dcx_testflag(Styles, CCS_BOTTOM))
		tsStyles.addtok(TEXT("bottom"));
	if (dcx_testflag(Styles, CCS_LEFT))
		tsStyles.addtok(TEXT("left"));
	if (dcx_testflag(Styles, CCS_RIGHT))
		tsStyles.addtok(TEXT("right"));
	if (dcx_testflag(Styles, CCS_NOPARENTALIGN | CCS_NORESIZE))
		tsStyles.addtok(TEXT("noauto"));
	if (dcx_testflag(Styles, CCS_ADJUSTABLE))
		tsStyles.addtok(TEXT("adjustable"));
	if (dcx_testflag(Styles, TBSTYLE_LIST))
		tsStyles.addtok(TEXT("list"));
	if (dcx_testflag(Styles, TBSTYLE_WRAPABLE))
		tsStyles.addtok(TEXT("wrap"));

	if (m_bOverrideTheme)
		tsStyles.addtok(TEXT("final"));

	if (dcx_testflag(ExStylesTb, TBSTYLE_EX_DRAWDDARROWS))
		tsStyles.addtok(TEXT("arrows"));

	return tsStyles;
}

void DcxToolBar::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());
}

TiXmlElement* DcxToolBar::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

LRESULT DcxToolBar::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
