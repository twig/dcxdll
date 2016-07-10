/*!
 * \file dcxstatusbar.cpp
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
#include "Classes/dcxstatusbar.h"
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

DcxStatusBar::DcxStatusBar(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles )
: DcxControl( ID, p_Dialog )
, m_hImageList(nullptr)
, m_vParts()
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	
	parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CONTROLPARENT,
		DCX_STATUSBARCLASS,
		nullptr,
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if ( bNoTheme )
		Dcx::UXModule.dcxSetWindowTheme( m_Hwnd , L" ", L" " );

	ZeroMemory(m_iDynamicParts, sizeof(m_iDynamicParts));
	ZeroMemory(m_iFixedParts, sizeof(m_iFixedParts));

	if ((rc->bottom - rc->top) > 0)
		SendMessage(m_Hwnd,SB_SETMINHEIGHT,rc->bottom - rc->top,0);

	setControlFont( GetStockFont( DEFAULT_GUI_FONT ), FALSE );
	registreDefaultWindowProc( );
	SetProp( m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxStatusBar::~DcxStatusBar( )
{
	for (const auto pPart : m_vParts)
		delete pPart;

	cleanPartIcons( );
	ImageList_Destroy( getImageList( ) );

	unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStatusBar::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	for (const auto &tsStyle: styles)
	{
#if DCX_USE_HASHING
		switch (const_hash(tsStyle.to_chr()))
		{
			case L"grip"_hash:
				*Styles |= SBARS_SIZEGRIP;
				break;
			case L"tooltips"_hash:
				*Styles |= SBARS_TOOLTIPS;
				break;
			case L"nodivider"_hash:
				*Styles |= CCS_NODIVIDER;
				break;
			case L"top"_hash:
				{
					*Styles |= CCS_TOP;
					*Styles &= ~SBARS_SIZEGRIP; // size grip doesn't work for left or top styles.
				}
				break;
			case L"noresize"_hash:
				*Styles |= CCS_NORESIZE;
				break;
			case L"noparentalign"_hash:
				*Styles |= CCS_NOPARENTALIGN;
				break;
			case L"noauto"_hash:
				*Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
				break;
			//case L"left"_hash:
			//{ // NB: left & right styles don't render the parts vertically.
			//	*Styles |= CCS_LEFT;
			//	*Styles &= ~SBARS_SIZEGRIP;
			//}
			//break;
			//case L"right"_hash:
			//	*Styles |= CCS_RIGHT;
			//	break;
			default:
				break;
		}
#else
		if (tsStyle == TEXT("grip"))
			*Styles |= SBARS_SIZEGRIP;
		else if (tsStyle == TEXT("tooltips"))
			*Styles |= SBARS_TOOLTIPS;
		else if (tsStyle == TEXT("nodivider"))
			*Styles |= CCS_NODIVIDER;
		else if (tsStyle == TEXT("top")) {
			*Styles |= CCS_TOP;
			*Styles &= ~SBARS_SIZEGRIP; // size grip doesn't work for left or top styles.
		}
		else if (tsStyle == TEXT("noresize"))
			*Styles |= CCS_NORESIZE;
		else if (tsStyle == TEXT("noparentalign"))
			*Styles |= CCS_NOPARENTALIGN;
		else if (tsStyle == TEXT("noauto"))
			*Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
		//else if ( tsStyle == TEXT("left") )
		//{ // NB: left & right styles don't render the parts vertically.
		//	*Styles |= CCS_LEFT;
		//	*Styles &= ~SBARS_SIZEGRIP;
		//}
		//else if ( tsStyle == TEXT("right") )
		//	*Styles |= CCS_RIGHT;
#endif
	}

	parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxStatusBar::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const
{
	const auto numtok = input.numtok();
	const auto prop(input.getfirsttok(3));

	// [NAME] [ID] [PROP] [N]
	if ( prop == TEXT("text") && numtok > 3 ) {

		const int iPart = input.getnexttok( ).to_int( ) -1, nParts = getParts(DCX_STATUSBAR_MAX_PARTS, 0 );	// tok 4

		if ( iPart < 0 && iPart >= nParts )
			throw Dcx::dcxException("Invalid Part");

		const auto iFlags = getPartFlags(iPart);

		if (dcx_testflag(iFlags, SBT_OWNERDRAW)) {
			auto pPart = reinterpret_cast<LPSB_PARTINFOX>(getText(iPart, nullptr));
			if (pPart != nullptr) {
				dcx_strcpyn(szReturnValue, pPart->m_xText.to_chr(), MIRC_BUFFER_SIZE_CCH);
			}
		}
		else {
			const auto len = this->getTextLength(iPart);
			auto text = std::make_unique<WCHAR[]>(len + 1);
			getText(iPart, text.get());
			dcx_strcpyn(szReturnValue, text.get(), MIRC_BUFFER_SIZE_CCH);
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("parts") ) {

		auto parts = std::make_unique<INT[]>(DCX_STATUSBAR_MAX_PARTS);

		const auto nParts = this->getParts(DCX_STATUSBAR_MAX_PARTS, 0);

		getParts(DCX_STATUSBAR_MAX_PARTS, parts.get() );

		TString tmp((UINT)(32 * nParts));

		for (auto i = decltype(nParts){0}; i < nParts; i++)
			tmp.addtok(parts[i]);

		dcx_strcpyn(szReturnValue, tmp.to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP] [N]
	else if (prop == TEXT("tooltip") && numtok > 3) {

		const int iPart = input.getnexttok().to_int(), nParts = getParts(DCX_STATUSBAR_MAX_PARTS, 0);	// tok 4

		if (iPart < 0 && iPart >= nParts)
			throw Dcx::dcxException("Invalid Part");

		getTipText(iPart, MIRC_BUFFER_SIZE_CCH, szReturnValue);
	}
	else
		parseGlobalInfoRequest(input, szReturnValue);
}

void DcxStatusBar::deletePartInfo(const int iPart)
{
	if (!dcx_testflag(HIWORD(getTextLength(iPart)), SBT_OWNERDRAW))
		return;

	auto pPart = reinterpret_cast<LPSB_PARTINFOX>(getText(iPart, nullptr));

	if (pPart == nullptr)
		return;

	//for (auto itStart = m_vParts.begin(), itEnd = m_vParts.end(); itStart != itEnd; ++itStart)
	//{
	//	if (*itStart == pPart) {
	//		this->setText( iPart, SBT_OWNERDRAW, nullptr);
	//		delete pPart->m_xChild;
	//		delete pPart;
	//		m_vParts.erase(itStart);
	//		return;
	//	}
	//}

	if (Dcx::eraseIfFound(m_vParts, pPart))
	{
		setText(iPart, SBT_OWNERDRAW, nullptr);
		delete pPart->m_xChild;
		delete pPart;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStatusBar::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));

	const auto numtok = input.numtok();

	// xdid -k [NAME] [ID] [SWITCH] [COLOR]
	if (flags[TEXT('k')] && numtok > 3) {
		const auto col = input.getnexttok().to_<COLORREF>();	// tok 4
		if (col == CLR_INVALID)
			this->setBkColor(CLR_DEFAULT);
		else
			this->setBkColor(col);
	}
	// xdid -l -> [NAME] [ID] -l [POS [POS POS ...]]
	else if ( flags[TEXT('l')] && numtok > 3 ) {

		const auto nParts = numtok - 3;
		auto parts = std::make_unique<INT[]>(DCX_STATUSBAR_MAX_PARTS);

		auto c = 0U;
		TString p;
		for (auto i = decltype(nParts){0}; i < nParts; i++)
		{
			if (c >= 100)
				throw Dcx::dcxException("Can't Allocate Over 100% of Statusbar!");

			p = input.getnexttok( );	// tok i+4
			const auto t = p.to_int();

			if (p.right(1) == TEXT('%')) {
				m_iDynamicParts[i] = t;
				c += t;
			}
			else
				m_iFixedParts[i] = t;

			parts[i] = t;
		}
		setParts( nParts, parts.get() );
		updateParts();
	}
	// xdid -t [NAME] [ID] [SWITCH] N [+FLAGS] [#ICON] [Cell Text][TAB]Tooltip Text
	// xdid -t [NAME] [ID] [SWITCH] N [+c] [#ICON] [CID] [CTRL] [X] [Y] [W] [H] (OPTIONS)
	// xdid -t [NAME] [ID] [SWITCH] N [+f] [#ICON] (TEXT)[TAB]Tooltip Text
	else if ( flags[TEXT('t')] && numtok > 5 ) {

		const auto nPos = input.getnexttok().to_int() - 1;	// tok 4
		const auto flag(input.getnexttok());			// tok 5
		const auto icon = input.getnexttok().to_int() - 1;	// tok 6
		const auto tsTabOne(input.getfirsttok(1, TSTABCHAR).trim());
		const auto tsTabTwo(input.getlasttoks().trim());

		if ( nPos < 0 || nPos >= this->getParts(DCX_STATUSBAR_MAX_PARTS, 0 ) )
			throw Dcx::dcxException("Invalid Part");

		TString itemtext;
		TString tooltip;

		deletePartInfo(nPos); // delete custom info if any.
		DestroyIcon( getIcon( nPos ) ); // delete any icon for this part.
		setIcon( nPos, nullptr ); // set as no icon.

		const auto iFlags = parseItemFlags(flag);

		if ( tsTabOne.numtok( ) > 6 )
			itemtext = tsTabOne.gettok(7, -1).trim();

		if ( !tsTabTwo.empty() )
			tooltip = tsTabTwo;

		if (dcx_testflag(iFlags, SBT_OWNERDRAW))
		{
			auto pPart = std::make_unique<SB_PARTINFOX>();

			pPart->m_xChild = nullptr;
			pPart->m_xiIcon = icon;
			if (flag.find(TEXT('f'),0)) { // mIRC formatted text
				pPart->m_xText = itemtext;
				setTipText( nPos, tooltip.to_chr( ) );
			}
			else { // child control
				auto p_Control = m_pParentDialog->addControl(itemtext, 1,
					CTLF_ALLOW_PBAR|CTLF_ALLOW_TRACKBAR|CTLF_ALLOW_COMBOEX|/*CTLF_ALLOW_COLORCOMBO|*/
					CTLF_ALLOW_STATUSBAR|CTLF_ALLOW_TOOLBAR|
					CTLF_ALLOW_TREEVIEW|CTLF_ALLOW_LISTVIEW|CTLF_ALLOW_REBAR|
					CTLF_ALLOW_BUTTON|/*CTLF_ALLOW_RICHEDIT|*/CTLF_ALLOW_EDIT|
					CTLF_ALLOW_UPDOWN|CTLF_ALLOW_IPADDRESS|CTLF_ALLOW_WEBCTRL|
					CTLF_ALLOW_CALANDER|CTLF_ALLOW_DIVIDER|CTLF_ALLOW_PANEL|
					CTLF_ALLOW_TAB|CTLF_ALLOW_LINE|CTLF_ALLOW_BOX|CTLF_ALLOW_RADIO|
					CTLF_ALLOW_CHECK|CTLF_ALLOW_TEXT|CTLF_ALLOW_SCROLL|CTLF_ALLOW_LIST|
					CTLF_ALLOW_LINK|CTLF_ALLOW_IMAGE|CTLF_ALLOW_PAGER|CTLF_ALLOW_DATETIME|
					CTLF_ALLOW_STACKER|CTLF_ALLOW_DIRECTSHOW,
					m_Hwnd);

				pPart->m_xChild = p_Control;
				ShowWindow(p_Control->getHwnd(),SW_HIDE); // hide control untill a WM_DRAWITEM is recieved.
				SendMessage(m_Hwnd,WM_SIZE,0,0);

				//const auto ID = mIRC_ID_OFFSET + (UINT)itemtext.gettok(1).to_int();
				//
				//if (!this->m_pParentDialog->isIDValid(ID, true))
				//	throw Dcx::dcxException(TEXT("Control with ID \"%\" already exists"), ID - mIRC_ID_OFFSET);
				//
				//try {
				//	auto p_Control = DcxControl::controlFactory(this->m_pParentDialog, ID, itemtext, 2,
				//					CTLF_ALLOW_PBAR|CTLF_ALLOW_TRACKBAR|CTLF_ALLOW_COMBOEX|/*CTLF_ALLOW_COLORCOMBO|*/
				//						CTLF_ALLOW_STATUSBAR|CTLF_ALLOW_TOOLBAR|
				//						CTLF_ALLOW_TREEVIEW|CTLF_ALLOW_LISTVIEW|CTLF_ALLOW_REBAR|
				//						CTLF_ALLOW_BUTTON|/*CTLF_ALLOW_RICHEDIT|*/CTLF_ALLOW_EDIT|
				//						CTLF_ALLOW_UPDOWN|CTLF_ALLOW_IPADDRESS|CTLF_ALLOW_WEBCTRL|
				//						CTLF_ALLOW_CALANDER|CTLF_ALLOW_DIVIDER|CTLF_ALLOW_PANEL|
				//						CTLF_ALLOW_TAB|CTLF_ALLOW_LINE|CTLF_ALLOW_BOX|CTLF_ALLOW_RADIO|
				//						CTLF_ALLOW_CHECK|CTLF_ALLOW_TEXT|CTLF_ALLOW_SCROLL|CTLF_ALLOW_LIST|
				//						CTLF_ALLOW_LINK|CTLF_ALLOW_IMAGE|CTLF_ALLOW_PAGER|CTLF_ALLOW_DATETIME|
				//						CTLF_ALLOW_STACKER|CTLF_ALLOW_DIRECTSHOW,m_Hwnd);
				//
				//	// problems with colorcombo/richedit, causes odd gfx glitches & dialog slow down.
				//	this->m_pParentDialog->addControl( p_Control );
				//	pPart->m_xChild = p_Control;
				//	ShowWindow(p_Control->getHwnd(),SW_HIDE); // hide control untill a WM_DRAWITEM is recieved.
				//	SendMessage(m_Hwnd,WM_SIZE,0,0);
				//}
				//catch ( std::exception &e ) {
				//	this->showErrorEx(nullptr, TEXT("-t"), TEXT("Unable To Create Control %d (%S)"), ID - mIRC_ID_OFFSET, e.what());
				//	throw;
				//}
			}
			setPartInfo( nPos, iFlags, pPart.release() );
		}
		else {

			if ( icon != -1 )
				setIcon( nPos, ImageList_GetIcon( getImageList( ), icon, ILD_TRANSPARENT ) );

			setText( nPos, iFlags, itemtext.to_chr( ) );
			setTipText( nPos, tooltip.to_chr( ) );
		}
	}
	// xdid -v [NAME] [ID] [SWITCH] [N] (TEXT)
	else if ( flags[TEXT('v')] && numtok > 3 ) {

		const auto nPos = input.getnexttok().to_int() - 1;	// tok 4

		if (nPos < 0 || nPos >= this->getParts(DCX_STATUSBAR_MAX_PARTS, 0))
			throw Dcx::dcxException("Invalid Part");

		TString itemtext;
		if ( numtok > 4 )
			itemtext = input.getlasttoks();	// tok 5, -1

		if (dcx_testflag(HIWORD( getTextLength( nPos ) ), SBT_OWNERDRAW))
		{
			auto pPart = reinterpret_cast<LPSB_PARTINFOX>(getText(nPos, nullptr));

			if (pPart != nullptr)
			{
				pPart->m_xText = itemtext;
				setPartInfo(nPos, SBT_OWNERDRAW, pPart);
			}
		}
		else {
			auto text = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);
			setText( nPos, HIWORD( getText( nPos, text.get() ) ), itemtext.to_chr( ) );
		}
	}
	// xdid -w [NAME] [ID] [SWITCH] [FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')] && numtok > 5) {
		const auto flag(input.getnexttok());			// tok 4
		const auto index = input.getnexttok().to_int();		// tok 5
		auto filename(input.getlasttoks());				// tok 6, -1

		auto himl = getImageList();

		if (himl == nullptr) {
			himl = createImageList();

			if (himl != nullptr)
				setImageList(himl);
		}

		if (himl == nullptr)
			throw Dcx::dcxException("Unable to get imagelist");

#if DCX_USE_WRAPPERS
		Dcx::dcxIconResource icon(index, filename, false, flag);

		ImageList_AddIcon(himl, icon);
#else
		HICON icon = dcxLoadIcon(index, filename, false, flag);
		
		if (icon == nullptr)
			throw Dcx::dcxException("Unable to load icon");
		
		ImageList_AddIcon(himl, icon);
		DestroyIcon(icon);
#endif
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if ( flags[TEXT('y')] ) {

		ImageList_Destroy( getImageList( ) );
		setImageList(nullptr);
	}
	else
		parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxStatusBar::getImageList( ) const noexcept {

  return m_hImageList;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStatusBar::setImageList( HIMAGELIST himl )
{
  m_hImageList = himl;
}

/*!
 * \brief blah
 *
 * blah
 */

//HIMAGELIST DcxStatusBar::createImageList( )
//{
//  return ImageList_Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );
//}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxStatusBar::parseItemFlags( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if (xflags[TEXT('n')])
		iFlags |= SBT_NOBORDERS;
	if (xflags[TEXT('p')])
		iFlags |= SBT_POPOUT;
	if (xflags[TEXT('c')])
		iFlags |= SBT_OWNERDRAW;
	if (xflags[TEXT('f')])
		iFlags |= SBT_OWNERDRAW;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStatusBar::cleanPartIcons( ) {

	for (auto n = 0; n < DCX_STATUSBAR_MAX_PARTS; n++)
		DestroyIcon( getIcon( n ) );
}

const TString DcxStatusBar::getStyles(void) const {
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, SBARS_SIZEGRIP))
		styles.addtok(TEXT("grip"));
	if (dcx_testflag(Styles, SBARS_TOOLTIPS))
		styles.addtok(TEXT("tooltips"));
	if (dcx_testflag(Styles, CCS_NODIVIDER))
		styles.addtok(TEXT("nodivider"));
	if (dcx_testflag(Styles, CCS_TOP))
		styles.addtok(TEXT("top"));
	if (dcx_testflag(Styles, CCS_NORESIZE))
		styles.addtok(TEXT("noresize"));
	if (dcx_testflag(Styles, CCS_NOPARENTALIGN))
		if (dcx_testflag(Styles, CCS_NORESIZE))
			styles.addtok(TEXT("noauto"));
		else
			styles.addtok(TEXT("noparentalign"));
	return styles;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setParts( const int nParts, const LPINT aWidths ) {
  return SendMessage( m_Hwnd, SB_SETPARTS, static_cast<WPARAM>(nParts), (LPARAM) aWidths );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getParts( const int nParts, LPINT aWidths ) const {
  return SendMessage( m_Hwnd, SB_GETPARTS, static_cast<WPARAM>(nParts), (LPARAM) aWidths );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getBorders( LPINT aWidths ) const {
  return SendMessage( m_Hwnd, SB_GETBORDERS, 0U, (LPARAM) aWidths );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setBkColor( const COLORREF clrBk ) {
  return SendMessage( m_Hwnd, SB_SETBKCOLOR, 0U, (LPARAM) clrBk );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setText( const int iPart, const int Style, const PTCHAR lpstr ) {
  return SendMessage( m_Hwnd, SB_SETTEXT, static_cast<WPARAM>(iPart) | Style, (LPARAM) lpstr );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setPartInfo( const int iPart, const int Style, gsl::owner<const LPSB_PARTINFOX> pPart) {
  return SendMessage( m_Hwnd, SB_SETTEXT, static_cast<WPARAM>(iPart) | (Style | SBT_OWNERDRAW), reinterpret_cast<LPARAM>(pPart) );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getText( const int iPart, PTCHAR lpstr ) const {
  return SendMessage( m_Hwnd, SB_GETTEXT, static_cast<WPARAM>(iPart), (LPARAM) lpstr );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getTextLength( const int iPart ) const {
  return SendMessage( m_Hwnd, SB_GETTEXTLENGTH, static_cast<WPARAM>(iPart), (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setTipText( const int iPart, const PTCHAR lpstr ) {
  return SendMessage( m_Hwnd, SB_SETTIPTEXT, static_cast<WPARAM>(iPart), (LPARAM) lpstr );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getTipText( const int iPart, const int nSize, PTCHAR lpstr ) const {
  return SendMessage( m_Hwnd, SB_GETTIPTEXT, MAKEWPARAM (iPart, nSize), (LPARAM) lpstr );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getRect( const int iPart, gsl::not_null<LPRECT> lprc ) const {
  return SendMessage( m_Hwnd, SB_GETRECT, static_cast<WPARAM>(iPart), reinterpret_cast<LPARAM>(lprc.get()) );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setIcon( const int iPart, const HICON hIcon ) {
  return SendMessage( m_Hwnd, SB_SETICON, static_cast<WPARAM>(iPart), (LPARAM) hIcon );
}

/*!
 * \brief blah
 *
 * blah
 */

HICON DcxStatusBar::getIcon( const int iPart ) const {
  return reinterpret_cast<HICON>(SendMessage( m_Hwnd, SB_GETICON, static_cast<WPARAM>(iPart), (LPARAM) 0 ));
}

/*!
* \brief blah
*
* blah
*/

UINT DcxStatusBar::getPartFlags(const int iPart) const {
	return static_cast<UINT>(HIWORD(SendMessage(m_Hwnd, SB_GETTEXTLENGTH, static_cast<WPARAM>(iPart), NULL)));
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxStatusBar::hitTest( const POINT & pt ) const {

	RECT rc = { 0 };
	const auto nParts = getParts(DCX_STATUSBAR_MAX_PARTS, 0);

	for (auto n = decltype(nParts){0}; n < nParts; n++)
	{
		getRect( n, &rc );
		if ( PtInRect( &rc, pt ) )
			return n;
	}
	return -1;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxStatusBar::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg ) {
	case WM_NOTIFY:
		{
			dcxlParam(LPNMHDR, hdr);

			if (hdr == nullptr)
				break;

			switch( hdr->code ) {

			case NM_CLICK:
				{
					if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
					{
						POINT pt;
						if (GetCursorPos(&pt))
						{
							MapWindowPoints(nullptr, m_Hwnd, &pt, 1);
							const auto cell = this->hitTest(pt);

							if (cell != -1)
								this->execAliasEx(TEXT("%s,%d,%d"), TEXT("sclick"), this->getUserID(), cell + 1);
						}
					}
					bParsed = TRUE;
				}
				break;

			case NM_RCLICK:
				{
					if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
					{
						POINT pt;
						if (GetCursorPos(&pt))
						{
							MapWindowPoints(nullptr, m_Hwnd, &pt, 1);
							const auto cell = this->hitTest(pt);

							if (cell != -1)
								this->execAliasEx(TEXT("%s,%d,%d"), TEXT("rclick"), this->getUserID(), cell + 1);
						}
					}
					bParsed = TRUE;
				}
				break;

			case NM_DBLCLK: 
				{
					if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
					{
						POINT pt;
						if (GetCursorPos(&pt))
						{
							MapWindowPoints(nullptr, m_Hwnd, &pt, 1);
							const auto cell = this->hitTest(pt);

							if (cell != -1)
								this->execAliasEx(TEXT("%s,%d,%d"), TEXT("dclick"), this->getUserID(), cell + 1);
						}
					}
					bParsed = TRUE;
				}
				break;

			} // switch
		}
		break;
	case WM_DRAWITEM: // support for ownerdraw statusbar. NB: NO Delete Item msg.
		{
			dcxlParam(LPDRAWITEMSTRUCT, lpDrawItem);

			if (m_Hwnd == lpDrawItem->hwndItem) {
				bParsed = TRUE;
				auto pPart = reinterpret_cast<LPSB_PARTINFOX>(lpDrawItem->itemData);
				if (pPart != nullptr) {
					auto rc = lpDrawItem->rcItem;

					if (pPart->m_xiIcon > -1) {
						IMAGEINFO ii;
						if (ImageList_GetImageInfo(this->m_hImageList, pPart->m_xiIcon, &ii))
						{
							if (ImageList_Draw(this->m_hImageList, pPart->m_xiIcon, lpDrawItem->hDC, rc.left, rc.top + ((rc.bottom - rc.top) - (ii.rcImage.bottom - ii.rcImage.top)) / 2, ILD_TRANSPARENT))
								rc.left += (ii.rcImage.right - ii.rcImage.left) + 5;
						}
					}
					if (!pPart->m_xText.empty())
						mIRC_DrawText(lpDrawItem->hDC, pPart->m_xText, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE, false);
					else if (pPart->m_xChild != nullptr) {
						SetWindowPos(pPart->m_xChild->getHwnd(), nullptr, rc.left, rc.top,
							(rc.right - rc.left), (rc.bottom - rc.top), SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
					}
					return TRUE;
				}
			}
		}
		break;
	}
	return 0L;
}

LRESULT DcxStatusBar::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
	switch( uMsg ) {
		case WM_CONTEXTMENU:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:
			break;

		case WM_HSCROLL:
		case WM_VSCROLL:
		case WM_COMMAND:
			{
				if (IsWindow((HWND) lParam)) {
					auto c_this = reinterpret_cast<DcxControl *>(GetProp((HWND)lParam, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_COMPAREITEM:
			{
				dcxlParam(LPCOMPAREITEMSTRUCT, idata);

				if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
					auto c_this = reinterpret_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DELETEITEM:
			{
				dcxlParam(LPDELETEITEMSTRUCT, idata);

				if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
					auto c_this = reinterpret_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_MEASUREITEM:
			{
				auto cHwnd = GetDlgItem(m_Hwnd, wParam);
				if (IsWindow(cHwnd)) {
					auto c_this = reinterpret_cast<DcxControl *>(GetProp(cHwnd, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DRAWITEM:
			{
				dcxlParam(LPDRAWITEMSTRUCT, idata);

				if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
					auto c_this = reinterpret_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_NOTIFY: 
			{
				dcxlParam(LPNMHDR, hdr);

				if (hdr == nullptr)
					break;

				if (m_Hwnd != hdr->hwndFrom) {
					if (IsWindow(hdr->hwndFrom)) {
						auto c_this = reinterpret_cast<DcxControl *>(GetProp(hdr->hwndFrom, TEXT("dcx_cthis")));
						if (c_this != nullptr)
							lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
			}
			break;

		case WM_SIZE:
			{
				this->updateParts();
			}
			break;

			//case WM_PAINT:
			//	{
			//		PAINTSTRUCT ps;
			//		HDC hdc;
			//
			//		hdc = BeginPaint( m_Hwnd, &ps );
			//
			//		bParsed = TRUE;
			//
			//		// Setup alpha blend if any.
			//		LPALPHAINFO ai = this->SetupAlphaBlend(&hdc, true);
			//
			//		DcxControl::DrawCtrlBackground( hdc, this);
			//
			//		lRes = CallWindowProc( this->m_DefaultWindowProc, m_Hwnd, uMsg, (WPARAM) hdc, lParam );
			//
			//		this->FinishAlphaBlend(ai);
			//
			//		EndPaint( m_Hwnd, &ps );
			//	}
			//	break;

		case WM_DESTROY:
			{
				delete this;
				bParsed = TRUE;
			}
			break;

		default:
			lRes = this->CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return lRes;
}

void DcxStatusBar::updateParts(void) {
	const auto nParts = this->getParts(0, nullptr);

	if (nParts <= 0)
		return;

	RECT rcClient;
	if (!GetClientRect(m_Hwnd, &rcClient))
		return;

	auto pParts = std::make_unique<int[]>(nParts);
	//int borders[3];

	getParts(nParts, pParts.get());

	//getBorders(borders);

	const auto w = (rcClient.right - rcClient.left); // - (2 * borders[1]);

	for (auto i = decltype(nParts){0}; i < nParts; i++) {
		int pw = m_iFixedParts[i];

		if (m_iDynamicParts[i] != 0)
			pw = (w / 100) * m_iDynamicParts[i];

		if (i == 0)
			pParts[i] = pw;
		else {
			if (pw == -1)
				pParts[i] = -1;
			else
				pParts[i] = (pParts[i-1] + pw);
		}
	}

	setParts(nParts, pParts.get());
}
