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
 * � ScriptsDB.org - 2006
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

DcxToolBar::DcxToolBar(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
	, m_hItemFont(nullptr)
	, m_hOldItemFont(nullptr)
	, m_bOverrideTheme(false)
	, m_bAutoStretch(false)
{
	//We need to divide ExStyles and tbExStyles here because the first is used for
	//global transparent style and the second for arrows style

	const auto[bNoTheme, Styles, ExStyles] = parseControlStyles(styles);
	const auto ExStylesTb = parseControlStylesToolBar(styles);

	m_Hwnd = dcxCreateWindow(
		ExStyles,
		DCX_TOOLBARCLASS,
		Styles | WS_CHILD,
		rc,
		mParentHwnd,
		ID);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	if (ExStylesTb != WindowExStyle::None)
		SendMessage(m_Hwnd, TB_SETEXTENDEDSTYLE, 0U, gsl::narrow_cast<LPARAM>(ExStylesTb));

	SendMessage(m_Hwnd, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0L);
	this->m_ToolTipHWND = (HWND)SendMessage(m_Hwnd, TB_GETTOOLTIPS, NULL, NULL);
	if (styles.istok(TEXT("balloon")) && this->m_ToolTipHWND != nullptr)
	{
		//SetWindowLong(this->m_ToolTipHWND, GWL_STYLE, gsl::narrow_cast<LONG>(GetWindowStyle(this->m_ToolTipHWND) | TTS_BALLOON));
		dcxSetWindowStyle(m_ToolTipHWND, dcxGetWindowStyle(m_ToolTipHWND) | TTS_BALLOON);
	}
	//SendMessage( m_Hwnd, TB_SETPARENT, (WPARAM)mParentHwnd, NULL);

	this->autoSize();

	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxToolBar::~DcxToolBar( )
{
	this->resetContent( );

	ImageList_Destroy( this->getImageList( TB_IML_NORMAL ) );
	ImageList_Destroy( this->getImageList( TB_IML_DISABLE ) );
	ImageList_Destroy( this->getImageList( TB_IML_HOT ) );

	this->unregistreDefaultWindowProc( );
}

///*!
// * \brief blah
// *
// * blah
// */
//void DcxToolBar::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
//{
//	LONG ExStylesTb = 0;
//	parseControlStyles(styles, Styles, ExStyles, &ExStylesTb, bNoTheme);
//}
//
///*!
// * \brief blah
// *
// * blah
// */
//void DcxToolBar::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, LONG * ExStylesTb, BOOL * bNoTheme )
//{
//	//*Styles |= CCS_ADJUSTABLE;
//	for (const auto &tsStyle: styles)
//	{
//#if DCX_USE_HASHING
//		switch (std::hash<TString>{}(tsStyle))
//		{
//			case L"flat"_hash:
//				*Styles |= TBSTYLE_FLAT;
//				break;
//			case L"tooltips"_hash:
//				*Styles |= TBSTYLE_TOOLTIPS;
//				break;
//			case L"transparent"_hash:
//				*Styles |= TBSTYLE_TRANSPARENT;
//				break;
//			case L"nodivider"_hash:
//				*Styles |= CCS_NODIVIDER;
//				break;
//			case L"top"_hash:
//				*Styles |= CCS_TOP;
//				break;
//			case L"bottom"_hash:
//				*Styles |= CCS_BOTTOM;
//				break;
//			case L"left"_hash:
//				*Styles |= CCS_LEFT;
//				break;
//			case L"right"_hash:
//				*Styles |= CCS_RIGHT;
//				break;
//			//case L"noresize"_hash: 
//			//  *Styles |= CCS_NORESIZE;
//			//	break;
//			//case L"noparentalign"_hash: 
//			//  *Styles |= CCS_NOPARENTALIGN ;
//			//	break;
//			case L"noauto"_hash:
//				*Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
//				break;
//			case L"adjustable"_hash:
//				*Styles |= CCS_ADJUSTABLE;
//				break;
//			case L"list"_hash:
//				*Styles |= TBSTYLE_LIST;
//				break;
//			case L"wrap"_hash:
//				*Styles |= TBSTYLE_WRAPABLE;
//				break;
//			case L"arrows"_hash:
//				*ExStylesTb |= TBSTYLE_EX_DRAWDDARROWS;
//				break;
//			case L"override"_hash:
//				m_bOverrideTheme = true;
//			default:
//				break;
//		}
//#else
//		if (tsStyle == TEXT("flat"))
//			*Styles |= TBSTYLE_FLAT;
//		else if ( tsStyle == TEXT("tooltips") )
//			*Styles |= TBSTYLE_TOOLTIPS;
//		else if ( tsStyle == TEXT("transparent") )
//			*Styles |= TBSTYLE_TRANSPARENT;
//		else if ( tsStyle == TEXT("nodivider") )
//			*Styles |= CCS_NODIVIDER;
//		else if ( tsStyle == TEXT("top") )
//			*Styles |= CCS_TOP;
//		else if ( tsStyle == TEXT("bottom") )
//			*Styles |= CCS_BOTTOM;
//		else if ( tsStyle == TEXT("left") )
//			*Styles |= CCS_LEFT;
//		else if ( tsStyle == TEXT("right") )
//			*Styles |= CCS_RIGHT;
//		//else if ( tsStyle == TEXT("noresize") ) 
//		//  *Styles |= CCS_NORESIZE;
//		//else if ( tsStyle == TEXT("noparentalign") ) 
//		//  *Styles |= CCS_NOPARENTALIGN ;
//		else if ( tsStyle == TEXT("noauto") )
//			*Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
//		else if ( tsStyle == TEXT("adjustable") )
//			*Styles |= CCS_ADJUSTABLE;
//		else if ( tsStyle == TEXT("list") )
//			*Styles |= TBSTYLE_LIST;
//		else if ( tsStyle == TEXT("wrap") )
//			*Styles |= TBSTYLE_WRAPABLE;
//		else if ( tsStyle == TEXT("arrows") )
//			*ExStylesTb |= TBSTYLE_EX_DRAWDDARROWS;
//		else if ( tsStyle == TEXT("override") )
//			this->m_bOverrideTheme = true;
//#endif
//	}
//
//	this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
//}

/*!
* \brief blah
*
* blah
*/
WindowExStyle DcxToolBar::parseControlStylesToolBar(const TString & tsStyles)
{
	WindowExStyle ToolBarExStyles(WindowExStyle::None);

	for (const auto &tsStyle : tsStyles)
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
std::tuple<NoTheme, WindowStyle, WindowExStyle> DcxToolBar::parseControlStyles(const TString & tsStyles)
{
	auto[bNoTheme, Styles, ExStyles] = parseGeneralControlStyles(tsStyles);

	//Styles |= CCS_ADJUSTABLE;
	for (const auto &tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"flat"_hash:
			Styles |= TBSTYLE_FLAT;
			break;
		case L"check"_hash:
			Styles |= BTNS_CHECK;
			break;
		case L"group"_hash:
			Styles |= BTNS_GROUP;
			break;
		case L"dropdown"_hash:
			Styles |= BTNS_DROPDOWN;
			break;
		case L"autosize"_hash:
			Styles |= BTNS_AUTOSIZE;
			break;
		case L"noprefix"_hash:
			Styles |= BTNS_NOPREFIX;
			break;
		case L"showtext"_hash:
			Styles |= BTNS_SHOWTEXT;	// only applicable when used with TBSTYLE_EX_MIXEDBUTTONS
			break;
		case L"wholedropdown"_hash:
			Styles |= BTNS_WHOLEDROPDOWN;
			break;
		case L"tooltips"_hash:
			Styles |= TBSTYLE_TOOLTIPS;
			break;
		case L"transparent"_hash:
			Styles |= TBSTYLE_TRANSPARENT;
			break;
		case L"nodivider"_hash:
			Styles |= CCS_NODIVIDER;
			break;
		case L"top"_hash:
			Styles |= CCS_TOP;
			break;
		case L"bottom"_hash:
			Styles |= CCS_BOTTOM;
			break;
		case L"left"_hash:
			Styles |= CCS_LEFT;
			break;
		case L"right"_hash:
			Styles |= CCS_RIGHT;
			break;
			//case L"noresize"_hash: 
			//  Styles |= CCS_NORESIZE;
			//	break;
			//case L"noparentalign"_hash: 
			//  Styles |= CCS_NOPARENTALIGN ;
			//	break;
		case L"noauto"_hash:
			Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
			break;
		case L"adjustable"_hash:
			Styles |= CCS_ADJUSTABLE;
			break;
		case L"list"_hash:
			Styles |= TBSTYLE_LIST;
			break;
		case L"wrap"_hash:
			Styles |= TBSTYLE_WRAPABLE;
			break;
		case L"override"_hash:
			m_bOverrideTheme = true;
		default:
			break;
		}
	}

	return { bNoTheme, Styles, ExStyles };
}
/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxToolBar::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
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
			throw Dcx::dcxException("Invalid number of arguments");

		const auto iButton = input.getnexttok().to_int() - 1;	// tok 4

		if (iButton < 0 && iButton >= this->getButtonCount())
			throw Dcx::dcxException("Out of Range");

		// This way fails to give the correct result after buttons have been removed.
		//this->getButtonText( this->getIndexToCommand( nButton ), szReturnValue ); // possible overflow, needs fixing at some point.

		TBBUTTONINFO tbbi{};
		tbbi.cbSize = sizeof(TBBUTTONINFO);
		tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;

		this->getButtonInfo(iButton, &tbbi);

		if (auto lpdcxtbb = reinterpret_cast<LPDCXTBBUTTON>(tbbi.lParam); lpdcxtbb != nullptr)
			szReturnValue = lpdcxtbb->bText.to_chr();
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"icon"_hash:
	{
		if (numtok < 4)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto iButton = input.getnexttok().to_int() - 1;	// tok 4

		if (iButton < 0 && iButton >= this->getButtonCount())
			throw Dcx::dcxException("Out of Range");

		TBBUTTONINFO tbbi{};
		tbbi.cbSize = sizeof(TBBUTTONINFO);
		tbbi.dwMask = TBIF_IMAGE | TBIF_BYINDEX;

		this->getButtonInfo(iButton, &tbbi);

		_ts_snprintf(szReturnValue, TEXT("%d"), tbbi.iImage + 1);
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"state"_hash:
	{
		if (numtok < 4)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto iButton = input.getnexttok().to_int() - 1;	// tok 4

		if (iButton < 0 && iButton >= this->getButtonCount())
			throw Dcx::dcxException("Out of Range");

		TBBUTTONINFO tbbi{};
		tbbi.cbSize = sizeof(TBBUTTONINFO);
		tbbi.dwMask = TBIF_STATE | TBIF_BYINDEX;

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
			throw Dcx::dcxException("Invalid number of arguments");

		const auto iButton = input.getnexttok().to_int() - 1;	// tok 4

		if (iButton < 0 && iButton >= this->getButtonCount())
			throw Dcx::dcxException("Out of Range");

		TBBUTTONINFO tbbi{};
		tbbi.cbSize = sizeof(TBBUTTONINFO);
		tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;
		this->getButtonInfo(iButton, &tbbi);

		if (auto lpdcxtbb = reinterpret_cast<LPDCXTBBUTTON>(tbbi.lParam); lpdcxtbb != nullptr)
			szReturnValue = lpdcxtbb->tsTipText.to_chr();
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"dropdownpoint"_hash:
	{
		if (numtok < 4)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto iButton = input.getnexttok().to_int() - 1;	// tok 4

		if (iButton < 0 && iButton >= this->getButtonCount())
			throw Dcx::dcxException("Out of Range");

		RECT rc{};

		this->getItemRect(iButton, &rc);
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

void DcxToolBar::parseCommandRequest( const TString & input )
{
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		this->resetContent();

	// xdid -a [NAME] [ID] [SWITCH] [N] [+FLAGS] [WIDTH] [#ICON] [COLOR] (Button Text) [TAB] Tooltip Text
	if ( flags[TEXT('a')] && numtok > 4 )
	{
		auto nPos = input.getnexttok().to_int() - 1;	// tok 4

		if ( nPos == -1 )
			nPos += this->getButtonCount( ) + 1;

		const auto tsFlags(input.getnexttok());						// tok 5
		const auto width = input.getnexttok().to_<WORD>();			// tok 6
		const auto icon = input.getnexttok().to_int() - 1;			// tok 7
		const auto clrText = input.getnexttok().to_<COLORREF>();	// tok 8
		const auto iNumtok = input.gettok(1, TSTABCHAR).numtok();

		TBBUTTON tbb{};

		tbb.fsState = parseButtonStateFlags( tsFlags );
		tbb.idCommand = this->getFreeButtonID( );
		auto buttonStyles = parseButtonStyleFlags(tsFlags);
		tbb.fsStyle = gsl::narrow_cast<BYTE>(buttonStyles & 0xFF);

		if (( icon == -1 ) || (iNumtok < 7))
			tbb.iBitmap = I_IMAGENONE;
		else
			tbb.iBitmap = icon;

		TString itemtext;

		if ( iNumtok > 8 )
		{
			itemtext = input.gettok( 1, TSTABCHAR ).gettok( 9, -1 ).trim();

			if ( itemtext == TEXT('-') )
			{
				tbb.fsStyle = BTNS_SEP;
				//tbb.fsState = TBSTATE_ENABLED;
				tbb.iBitmap = icon;
				tbb.dwData = 0;
				tbb.iString = 0;

				this->insertButton( nPos, &tbb );
				this->autoSize( );
				this->redrawWindow( );
				return;
			}
		}

		// Tooltip Handling
		//auto lpdcxtbb = new DCXTBBUTTON;
		auto lpdcxtbb = std::make_unique<DCXTBBUTTON>();

		if ( input.numtok( TSTABCHAR) > 1 )
			lpdcxtbb->tsTipText = input.gettok( 2, -1, TSTABCHAR ).trim();

		lpdcxtbb->bUline = dcx_testflag(buttonStyles, BTNS_UNDERLINE);

		lpdcxtbb->bBold = dcx_testflag(buttonStyles, BTNS_BOLD);

		if (dcx_testflag(buttonStyles, BTNS_COLOR))
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
		this->insertButton( nPos, &tbb );
		//if (nPos == 1) // commented out for possible fix to http://dcx.scriptsdb.org/forum/showthread.php?tid=121 http://dcx.scriptsdb.org/bug/index.php?do=details&task_id=749
		//	SendMessage(m_Hwnd, TB_CHANGEBITMAP, nPos, MAKELPARAM(I_IMAGENONE,0)); // why was this added?

		// set width of button
		if ( dcx_testflag(buttonStyles,BTNS_WIDTH ))
		{
			TBBUTTONINFO tbbi{};
			tbbi.cbSize = sizeof( TBBUTTONINFO );

			tbbi.dwMask = TBIF_SIZE;
			tbbi.cx = width;
			this->setButtonInfo( tbb.idCommand, &tbbi );
		}

		this->autoSize( );
		if ( this->m_bAutoStretch )
			this->autoStretchButtons( );
	}
	// xdid -c [NAME] [ID] [SWITCH] [N] [+FLAGS] [RGB] [+REMOVEFLAGS]
	else if ( flags[TEXT('c')] && numtok > 5 )
	{
		const auto iButton = input.getnexttok( ).to_int( ) - 1;						// tok 4
		const auto buttonStyles = parseButtonStyleFlags(input.getnexttok());		// tok 5
		const auto clrColor = input.getnexttok().to_<COLORREF>();					// tok 6
		const auto removeButtonStyles = parseButtonStyleFlags(input.getnexttok());	// tok 7

		if (iButton == -1 && this->m_ToolTipHWND != nullptr)
		{
			if (dcx_testflag(buttonStyles, BTNS_TBKGCOLOR))
				SendMessage(this->m_ToolTipHWND,TTM_SETTIPBKCOLOR, gsl::narrow_cast<WPARAM>(clrColor), NULL);
			else if (dcx_testflag(buttonStyles, BTNS_TTXTCOLOR))
				SendMessage(this->m_ToolTipHWND,TTM_SETTIPTEXTCOLOR, gsl::narrow_cast<WPARAM>(clrColor), NULL);
		}
		else {

			if (iButton < 0 && iButton >= this->getButtonCount())
				throw Dcx::dcxException("Out of Range");

			TBBUTTONINFO tbbi{};
			tbbi.cbSize = sizeof( TBBUTTONINFO );
			tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;

			if (this->getButtonInfo(iButton, &tbbi) == -1)
				throw Dcx::dcxException("Unable to get button info");

			auto lpdcxtbb = reinterpret_cast<LPDCXTBBUTTON>(tbbi.lParam);

			if ( dcx_testflag(buttonStyles, BTNS_UNDERLINE ))
				lpdcxtbb->bUline = true;

			if (dcx_testflag(buttonStyles, BTNS_BOLD))
				lpdcxtbb->bBold = true;

			if (dcx_testflag(buttonStyles, BTNS_COLOR))
				lpdcxtbb->clrText = clrColor;

			if (dcx_testflag(buttonStyles, BTNS_HIGHLIGHT_TXTCOLOR))
				lpdcxtbb->clrTextHighlight = clrColor;

			if (dcx_testflag(buttonStyles, BTNS_MARK_BKGCOLOR))
				lpdcxtbb->clrMark = clrColor;

			if (dcx_testflag(buttonStyles, BTNS_BTNCOLOR))
				lpdcxtbb->clrBtnFace = clrColor;

			if (dcx_testflag(buttonStyles, BTNS_HIGHLIGHT_BTNCOLOR))
				lpdcxtbb->clrBtnHighlight = clrColor;

			if (dcx_testflag(buttonStyles, BTNS_HOTTRACK_BTNCOLOR))
				lpdcxtbb->clrHighlightHotTrack = clrColor;

			if (dcx_testflag(removeButtonStyles, BTNS_UNDERLINE))
				lpdcxtbb->bUline = false;

			if (dcx_testflag(removeButtonStyles, BTNS_BOLD))
				lpdcxtbb->bBold = false;

			if (dcx_testflag(removeButtonStyles, BTNS_COLOR))
				lpdcxtbb->clrText = CLR_INVALID;

			if (dcx_testflag(removeButtonStyles, BTNS_HIGHLIGHT_TXTCOLOR))
				lpdcxtbb->clrTextHighlight = CLR_INVALID;

			if (dcx_testflag(removeButtonStyles, BTNS_MARK_BKGCOLOR))
				lpdcxtbb->clrMark = CLR_INVALID;

			if (dcx_testflag(removeButtonStyles, BTNS_BTNCOLOR))
				lpdcxtbb->clrBtnFace = CLR_INVALID;

			if (dcx_testflag(removeButtonStyles, BTNS_HIGHLIGHT_BTNCOLOR))
				lpdcxtbb->clrBtnHighlight = CLR_INVALID;

			if (dcx_testflag(removeButtonStyles, BTNS_HOTTRACK_BTNCOLOR))
				lpdcxtbb->clrHighlightHotTrack = CLR_INVALID;

			redrawWindow( );
		}
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('d')] && numtok > 3 )
	{
		const auto nButton = input.getnexttok().to_int() - 1;	// tok 4

		if (nButton > -1)
			this->deleteButton(nButton);
	}
	// xdid -i [NAME] [ID] [SWITCH] [N] [IMAGE]
	else if ( flags[TEXT('i')] && numtok > 4 )
	{
		const auto nButton = input.getnexttok().to_int() - 1;	// tok 4
		auto iImage = input.getnexttok().to_int() - 1;			// tok 5

		if (iImage < 0)
			iImage = I_IMAGENONE;

		TBBUTTONINFO tbbi{};
		tbbi.cbSize = sizeof( TBBUTTONINFO );
		tbbi.dwMask = TBIF_IMAGE | TBIF_BYINDEX;
		tbbi.iImage = iImage;

		this->setButtonInfo( nButton /*this->getIndexToCommand( nButton )*/, &tbbi );
	}
	// xdid -j -> [NAME] [ID] -j [MIN] [MAX]
	else if ( flags[TEXT('j')] && numtok > 4 )
	{
		const auto nMin = input.getnexttok().to_int();	// tok 4
		const auto nMax = input.getnexttok().to_int();	// tok 5	// was tok 4 should be 5

		this->setButtonWidth( nMin, nMax );
	}
	// xdid -l [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('l')] && numtok > 3)
	{
		const auto size = NumToIconSize(input.getnexttok().to_<UINT>());	// tok 4

		// these are destroyed after being removed from toolbar by setImageList() function.
		//ImageList_Destroy(this->getImageList(TB_IML_NORMAL));
		//ImageList_Destroy(this->getImageList(TB_IML_DISABLE));
		//ImageList_Destroy(this->getImageList(TB_IML_HOT));

		auto himl = createImageList(size);
		setImageList(himl, TB_IML_NORMAL);

		himl = createImageList(size);
		setImageList(himl, TB_IML_DISABLE);

		himl = createImageList(size);
		setImageList(himl, TB_IML_HOT);

		redrawWindow();
	}
	// xdid -m [NAME] [ID] [SWITCH] [1|0]
	else if ( flags[TEXT('m')] && numtok > 3 )
	{
		m_bAutoStretch = (input.getnexttok() == TEXT('1'));	// tok 4

		if (m_bAutoStretch)
			autoStretchButtons( );
	}
	// xdid -q [NAME] [ID] [SWITCH] [N] (TIPTEXT)
	else if (flags[TEXT('q')] && numtok > 3)
	{
		const auto nButton = input.getnexttok().to_int() - 1;	// tok 4

		if (nButton < 0 && nButton >= this->getButtonCount())
			throw Dcx::dcxException("Out of Range");

		//int idButton = this->getIndexToCommand(nButton);
		TBBUTTONINFO tbbi{};

		tbbi.cbSize = sizeof(TBBUTTONINFO);
		tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;

		if (this->getButtonInfo(nButton, &tbbi) < 0)
			throw Dcx::dcxException("Unable to get button info");

		if (auto lpdcxtbb = reinterpret_cast<LPDCXTBBUTTON>(tbbi.lParam); numtok > 4)	// has a new tooltip
			lpdcxtbb->tsTipText = input.getlasttoks();	// tok 5, -1
		else					// no tooltip
			lpdcxtbb->tsTipText.clear();	// = TEXT("");
	}
	// This is to avoid an invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')])
	{
	}
	// xdid -t [NAME] [ID] [SWITCH] [N] [+FLAGS]
	else if ( flags[TEXT('t')] && numtok > 4 )
	{
		const auto nButton = input.getnexttok().to_int() - 1;				// tok 4
		const auto fStates = parseButtonStateFlags(input.getnexttok());	// tok 5

		const auto idButton = this->getIndexToCommand(nButton);

		if ( idButton > 0 )
			this->setButtonState( idButton, fStates );
	}
	// xdid -u [NAME] [ID] [SWITCH] [DX] [DY]
	else if ( flags[TEXT('u')] && numtok > 4 )
	{
		const auto dxButton = input.getnexttok().to_int();		// tok 4
		const auto dyButton = input.getnexttok().to_int();		// tok 5

		this->setButtonSize( dxButton, dyButton );
	}
	// xdid -v [NAME] [ID] [SWITCH] [N] (TEXT)
	else if ( flags[TEXT('v')] && numtok > 3 )
	{
		const auto nButton = input.getnexttok().to_int() - 1;		// tok 4

		if (nButton < 0 && nButton >= this->getButtonCount())
			throw Dcx::dcxException("Out of Range");
		
		const auto nIndex = this->getIndexToCommand(nButton);
		TBBUTTONINFO tbbi{};

		tbbi.cbSize = sizeof(TBBUTTONINFO);
		tbbi.dwMask = TBIF_LPARAM;
		
		if (this->getButtonInfo(nIndex, &tbbi) < 0)
			throw Dcx::dcxException("Unable to get button info");

		if (auto lpdcxtbb = reinterpret_cast<LPDCXTBBUTTON>(tbbi.lParam); lpdcxtbb != nullptr)
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
	else if (flags[TEXT('w')] && numtok > 5)
	{
		const auto tsFlags(input.getnexttok( ));	// tok 4
		const auto iFlags = this->parseImageListFlags(tsFlags);

		if (tsFlags[0] != TEXT('+'))
			throw Dcx::dcxException("Invalid Flags");

		const auto index = input.getnexttok().to_int();	// tok 5
		auto filename(input.getlasttoks());			// tok 6, -1

		auto himl = this->getImageList(TB_IML_NORMAL);

		// load the icon
		if (himl == nullptr)
			throw Dcx::dcxException("Unable to get Normal Image List");

		int cx = 0, cy = 0;
		if (!ImageList_GetIconSize(himl, &cx, &cy))
			throw Dcx::dcxException("Unable to get Icon Size");

#if DCX_USE_WRAPPERS
		Dcx::dcxIconResource icon(index, filename, (NumToIconSize(cx) > DcxIconSizes::SmallIcon), tsFlags);

		// NORMAL IML
		if (dcx_testflag(iFlags, TB_IML_NORMAL))
		{
			himl = this->getImageList(TB_IML_NORMAL);

			if (himl != nullptr)
				ImageList_AddIcon(himl, icon);
		}

		// DISABLED IML
		if (dcx_testflag(iFlags, TB_IML_DISABLE))
		{
			himl = this->getImageList(TB_IML_DISABLE);

			if (himl != nullptr)
				ImageList_AddIcon(himl, icon);
		}

		// HOT IML
		if (dcx_testflag(iFlags, TB_IML_HOT))
		{
			himl = this->getImageList(TB_IML_HOT);

			if (himl != nullptr)
				ImageList_AddIcon(himl, icon);
		}
#else
		auto icon = dcxLoadIcon(index, filename, (NumToIconSize(cx) > DcxIconSizes::SmallIcon), tsFlags);
		
		// if there is an icon to process
		if (icon == nullptr)
			throw Dcx::dcxException(TEXT("Icon Failed To Load: %"), filename);
		
		// NORMAL IML
		if (dcx_testflag(iFlags, TB_IML_NORMAL))
		{
			himl = this->getImageList(TB_IML_NORMAL);
		
			if (himl != nullptr)
				ImageList_AddIcon(himl, icon);
		}
		
		// DISABLED IML
		if (dcx_testflag(iFlags, TB_IML_DISABLE))
		{
			himl = this->getImageList(TB_IML_DISABLE);
		
			if (himl != nullptr)
				ImageList_AddIcon(himl, icon);
		}
		
		// HOT IML
		if (dcx_testflag(iFlags, TB_IML_HOT))
		{
			himl = this->getImageList(TB_IML_HOT);
		
			if (himl != nullptr)
				ImageList_AddIcon(himl, icon);
		}
		
		DestroyIcon(icon);
#endif
	}
	else
		parseGlobalCommandRequest( input, flags );
}

/*!
* \brief blah
*
* blah
*/

BYTE DcxToolBar::parseButtonStateFlags( const TString & flags )
{
	const XSwitchFlags xflags(flags);
	BYTE iFlags = TBSTATE_ENABLED;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if ( xflags[TEXT('d')] )
		iFlags &= ~TBSTATE_ENABLED;
	if ( xflags[TEXT('h')] )
		iFlags |= TBSTATE_HIDDEN;
	if ( xflags[TEXT('i')] )
		iFlags |= TBSTATE_INDETERMINATE;
	if ( xflags[TEXT('p')] )
		iFlags |= TBSTATE_PRESSED;
	if ( xflags[TEXT('x')] )
		iFlags |= TBSTATE_CHECKED;
	if ( xflags[TEXT('w')] )
		iFlags |= TBSTATE_WRAP;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxToolBar::parseButtonStyleFlags( const TString & flags ) 
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = BTNS_BUTTON;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if (xflags[TEXT('a')])
		iFlags |= BTNS_AUTOSIZE;
	if (xflags[TEXT('b')])
		iFlags |= BTNS_BOLD;
	if (xflags[TEXT('B')])
		iFlags |= BTNS_BTNCOLOR;
	if (xflags[TEXT('c')])
		iFlags |= BTNS_COLOR;
	if (xflags[TEXT('g')])
		iFlags |= BTNS_GROUP;
	if (xflags[TEXT('h')])
		iFlags |= BTNS_HIGHLIGHT_TXTCOLOR;
	if (xflags[TEXT('H')])
		iFlags |= BTNS_HIGHLIGHT_BTNCOLOR;
	if (xflags[TEXT('k')])
		iFlags |= BTNS_CHECK;
	if (xflags[TEXT('l')])
		iFlags |= BTNS_WIDTH;
	if (xflags[TEXT('m')])
		iFlags |= BTNS_MARK_BKGCOLOR;
	if (xflags[TEXT('T')])
		iFlags |= BTNS_HOTTRACK_BTNCOLOR;
	if (xflags[TEXT('u')])
		iFlags |= BTNS_UNDERLINE;
	if (xflags[TEXT('v')])
		iFlags |= BTNS_DROPDOWN;
	if (xflags[TEXT('x')])
		iFlags |= BTNS_TBKGCOLOR;
	if (xflags[TEXT('z')])
		iFlags |= BTNS_TTXTCOLOR;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/
UINT DcxToolBar::parseImageListFlags( const TString &flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('n')])
		iFlags |= TB_IML_NORMAL;
	if (xflags[TEXT('d')])
		iFlags |= TB_IML_DISABLE;
	if (xflags[TEXT('h')])
		iFlags |= TB_IML_HOT;
	if (xflags[TEXT('g')])
		iFlags |= TB_ICO_GREY;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

HIMAGELIST DcxToolBar::getImageList( const dcxToolBar_ImageLists iImageList ) const
{
	if ( iImageList == TB_IML_NORMAL )
		return (HIMAGELIST) SendMessage( m_Hwnd, TB_GETIMAGELIST, (WPARAM) 0, (LPARAM) 0);
	else if ( iImageList == TB_IML_DISABLE )
		return (HIMAGELIST) SendMessage( m_Hwnd, TB_GETDISABLEDIMAGELIST, (WPARAM) 0, (LPARAM) 0);
	else if ( iImageList == TB_IML_HOT )
		return (HIMAGELIST) SendMessage( m_Hwnd, TB_GETHOTIMAGELIST, (WPARAM) 0, (LPARAM) 0);
	return nullptr;
}

/*!
* \brief blah
*
* blah
*/

void DcxToolBar::setImageList(HIMAGELIST himl, const dcxToolBar_ImageLists iImageList)
{
	HIMAGELIST himlOld = nullptr;

	if (iImageList == TB_IML_NORMAL)
		himlOld = (HIMAGELIST) SendMessage(m_Hwnd, TB_SETIMAGELIST, (WPARAM) nullptr, (LPARAM) himl);
	else if (iImageList == TB_IML_DISABLE)
		himlOld = (HIMAGELIST) SendMessage(m_Hwnd, TB_SETDISABLEDIMAGELIST, (WPARAM) nullptr, (LPARAM) himl);
	else if (iImageList == TB_IML_HOT)
		himlOld = (HIMAGELIST) SendMessage(m_Hwnd, TB_SETHOTIMAGELIST, (WPARAM) NULL, (LPARAM) himl);

	if (himlOld != nullptr)
		ImageList_Destroy(himlOld);
}

/*!
* \brief blah
*
* blah
*/

HIMAGELIST DcxToolBar::createImageList( const DcxIconSizes iSize )
{
	return ImageList_Create(gsl::narrow_cast<int>(iSize), gsl::narrow_cast<int>(iSize), ILC_COLOR32 | ILC_MASK, 1, 0);
}

/*!
* \brief blah
*
* blah
*/

void DcxToolBar::resetContent( )
{
	auto nButtons = this->getButtonCount( );

	while ( nButtons-- )
		this->deleteButton( nButtons );
}

/*!
* \brief blah
*
* blah
*/

void DcxToolBar::autoStretchButtons( )
{
	const auto nButtons = this->getButtonCount();
	auto nTotalButtons = nButtons;

	RECT rc{};

	if (!GetClientRect(m_Hwnd, &rc))
		return;

	auto leftwidth = rc.right - rc.left;

	TBBUTTON tbb{};

	// Find the button width
	for (auto i = decltype(nButtons){0}; i < nButtons; i++)
	{
		getButton( i, &tbb );

		if ( dcx_testflag(tbb.fsStyle, BTNS_SEP) )
		{
			RECT rcItem{};
			getItemRect(i, &rcItem);
			leftwidth -= rcItem.right - rcItem.left;
			--nTotalButtons;
		}
	}

	// Apply the new width
	TBBUTTONINFO tbbi{};
	tbbi.cbSize = sizeof( TBBUTTONINFO );
	tbbi.dwMask = TBIF_SIZE;
	//tbbi.cx = (WORD)( (double) leftwidth / (double) nTotalButtons );
	tbbi.cx = gsl::narrow_cast<WORD>(dcx_round(gsl::narrow_cast<float>(gsl::narrow_cast<double>(leftwidth) / gsl::narrow_cast<double>(nTotalButtons))));

	// Make sure the size is positive
	if ( tbbi.cx > 0 ) 
	{
		for (auto i = decltype(nButtons){0}; i < nButtons; ++i)
		{
			this->getButton( i, &tbb );

			if (dcx_testflag(tbb.fsStyle, BTNS_SEP))
				continue;

			this->setButtonInfo( this->getIndexToCommand( i ), &tbbi );
		}
	}
}

void DcxToolBar::autoPosition( const int width, const int height )
{
	if ( !this->isStyle( WindowStyle::CCS_NoParentAlign | CCS_NORESIZE ) )
		SendMessage( m_Hwnd, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::autoSize( )
{
	return SendMessage( m_Hwnd, TB_AUTOSIZE, (WPARAM) 0, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::insertButton( const int nPos, const LPTBBUTTON lpbb )
{
	return SendMessage( m_Hwnd, TB_INSERTBUTTON, gsl::narrow_cast<WPARAM>(nPos), (LPARAM) lpbb );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::hitTest( const LPPOINT lpt ) const
{
	return SendMessage( m_Hwnd, TB_HITTEST, (WPARAM) 0, (LPARAM) lpt );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getItemRect( const int iButton, LPRECT lprc ) const
{
	return SendMessage( m_Hwnd, TB_GETITEMRECT, gsl::narrow_cast<WPARAM>(iButton), (LPARAM) lprc );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getButtonCount( ) const
{
	return SendMessage( m_Hwnd, TB_BUTTONCOUNT, (WPARAM) 0, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::setButtonWidth( const int cxMin, const int cxMax )
{
	return SendMessage( m_Hwnd, TB_SETBUTTONWIDTH, (WPARAM) 0, gsl::narrow_cast<LPARAM>(MAKELONG ( cxMin, cxMax )) );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::deleteButton(const int iButton)
{
	return (LRESULT) SendMessage( m_Hwnd, TB_DELETEBUTTON, gsl::narrow_cast<WPARAM>(iButton), (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::setButtonInfo( const int idButton, LPTBBUTTONINFO lpbi )
{
	return SendMessage( m_Hwnd, TB_SETBUTTONINFO, gsl::narrow_cast<WPARAM>(idButton), (LPARAM) lpbi );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getButtonInfo( const int idButton, LPTBBUTTONINFO lpbi ) const
{
	return SendMessage(m_Hwnd, TB_GETBUTTONINFO, gsl::narrow_cast<WPARAM>(idButton), (LPARAM) lpbi);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getTooltips( ) const
{
	return SendMessage( m_Hwnd, TB_GETTOOLTIPS, (WPARAM) 0, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getButtonText( const int idButton, LPSTR lpszText ) const
{
	return (LRESULT) SendMessage( m_Hwnd, TB_GETBUTTONTEXT, gsl::narrow_cast<WPARAM>(idButton), (LPARAM) lpszText );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getButton( const int iButton, LPTBBUTTON lpb ) const
{
	return SendMessage( m_Hwnd, TB_GETBUTTON, gsl::narrow_cast<WPARAM>(iButton), (LPARAM) lpb );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getCommandToIndex( const int idButton ) const
{
	return SendMessage( m_Hwnd, TB_COMMANDTOINDEX, gsl::narrow_cast<WPARAM>(idButton), (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::moveButton( const int iButtonFrom, const int iButtonTo )
{
	return SendMessage( m_Hwnd, TB_MOVEBUTTON, gsl::narrow_cast<WPARAM>(iButtonFrom), gsl::narrow_cast<LPARAM>(iButtonTo) );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::markButton( const int iButton, const bool fHighlight )
{
	return SendMessage( m_Hwnd, TB_MARKBUTTON, gsl::narrow_cast<WPARAM>(iButton), gsl::narrow_cast<LPARAM>(MAKELONG ( fHighlight, 0 )) );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::setButtonState( const int idButton, const UINT fState )
{
	return SendMessage( m_Hwnd, TB_SETSTATE, gsl::narrow_cast<WPARAM>(idButton), gsl::narrow_cast<LPARAM>(MAKELONG ( fState, 0 )) );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::setButtonSize( const int dxButton, const int dyButton )
{
	return SendMessage( m_Hwnd, TB_SETBUTTONSIZE, (WPARAM) 0, gsl::narrow_cast<LPARAM>(MAKELONG (dxButton, dyButton)) );
}


/*!
* \brief blah
*
* blah
*/

int DcxToolBar::getFreeButtonID( ) const
{
	int i = 0, iID = 0;
	do {
		iID = this->getCommandToIndex( ++i );
	} while (iID != -1);
	return i;
}

/*!
* \brief blah
*
* blah
*/

int DcxToolBar::getIndexToCommand(const int iIndex) const
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
LRESULT DcxToolBar::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) 
{
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
		{
			if (auto iButton = gsl::narrow_cast<int>(wParam); iButton > -1)
				this->execAliasEx(TEXT("%s,%d,%d"), TEXT("sclick"), this->getUserID(), this->getCommandToIndex(iButton) + 1);
		}
		bParsed = TRUE;
	}
	break;

	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (hdr == nullptr)
			break;

		switch (hdr->code) 
		{
		case NM_RCLICK:
		{
			if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
			{
				dcxlParam(LPNMMOUSE, lpnm);

				auto pt = lpnm->pt;

				if (const auto iButton = this->hitTest(&pt); iButton > -1)
				{
					RECT rc{};
					this->getItemRect(iButton, &rc);
					MapWindowRect(m_Hwnd, nullptr, &rc);

					this->execAliasEx(TEXT("%s,%d,%d,%d,%d,%d,%d"), TEXT("rclick"), this->getUserID(), iButton + 1, rc.left, rc.bottom, rc.right, rc.top);
				}
			}
			bParsed = TRUE;
			return TRUE;
		}
		break;

		case TBN_DROPDOWN:
		{
			if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
			{
				dcxlParam(LPNMTOOLBAR, lpnmtb);

				if (const auto iButton = lpnmtb->iItem - 1; iButton > -1)
				{
					RECT rc{};
					this->getItemRect(iButton, &rc);
					MapWindowRect(m_Hwnd, nullptr, &rc);

					this->execAliasEx(TEXT("%s,%d,%d,%d,%d,%d,%d"), TEXT("dropdown"), this->getUserID(), iButton + 1, rc.left, rc.bottom, rc.right, rc.top);
				}
			}
			bParsed = TRUE;
			return TBDDRET_DEFAULT;
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

				auto lpdtbb = reinterpret_cast<LPDCXTBBUTTON>(lpntbcd->nmcd.lItemlParam);

				if (lpdtbb == nullptr)
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

				if (auto hFont = GetWindowFont(m_Hwnd); hFont != nullptr)
				{
					if (LOGFONT lf{}; GetObject(hFont, sizeof(LOGFONT), &lf) != 0)
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
						if (this->m_hItemFont != nullptr)
							this->m_hOldItemFont = SelectFont(lpntbcd->nmcd.hdc, this->m_hItemFont);
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
				if (this->m_hOldItemFont != nullptr)
				{
					SelectFont(lpntbcd->nmcd.hdc, this->m_hOldItemFont);
					this->m_hOldItemFont = nullptr;
				}
				if (this->m_hItemFont != nullptr)
				{
					DeleteFont(this->m_hItemFont);
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
			if (dcxlParam(LPNMTBGETINFOTIP, tcgit); tcgit != nullptr)
			{
				if (auto lpdtbb = reinterpret_cast<LPDCXTBBUTTON>(tcgit->lParam); ((lpdtbb != nullptr) && (tcgit->pszText != nullptr)))
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
		//		if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT)) {
		//			auto lpnmc = (LPNMCHAR) lParam;
		//			this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("keyup"), this->getUserID(), lpnmc->ch, lpnmc->dwItemPrev);
		//		}
		//		bParsed = TRUE;
		//	}
		//	break;

		} // switch
	}
	break;
	}
	return 0L;
}

LRESULT DcxToolBar::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed )
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
		if (!this->m_bAlphaBlend)
			break;

		PAINTSTRUCT ps{};
		auto hdc = BeginPaint(m_Hwnd, &ps);
		Auto(EndPaint(m_Hwnd, &ps));

		bParsed = TRUE;

		// Setup alpha blend if any.
		auto ai = this->SetupAlphaBlend(&hdc);
		Auto(this->FinishAlphaBlend(ai));

		return CallDefaultProc(m_Hwnd, uMsg, (WPARAM)hdc, lParam);
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
		tsStyles.addtok(TEXT("override"));

	if (dcx_testflag(ExStylesTb, TBSTYLE_EX_DRAWDDARROWS))
		tsStyles.addtok(TEXT("arrows"));

	return tsStyles;
}

void DcxToolBar::toXml(TiXmlElement *const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());
}

TiXmlElement * DcxToolBar::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}
