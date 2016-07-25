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

DcxTab::DcxTab(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles )
: DcxControl(ID, p_Dialog)
, m_bClosable(false)
, m_bGradient(false)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	m_Hwnd = CreateWindowEx(	
		ExStyles | WS_EX_CONTROLPARENT, 
		DCX_TABCTRLCLASS, 
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

	/*
	HWND hHwndTip = TabCtrl_GetToolTips( m_Hwnd );
	if ( IsWindow( hHwndTip ) ) {

	TOOLINFO ti;
	ZeroMemory( &ti, sizeof( TOOLINFO ) );
	ti.cbSize = sizeof( TOOLINFO );
	ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
	ti.hwnd = mParentHwnd;
	ti.uId = (UINT) m_Hwnd;
	ti.lpszText = LPSTR_TEXTCALLBACK;
	SendMessage( hHwndTip, TTM_ADDTOOL, (WPARAM) 0, (LPARAM) &ti );
	}
	*/
	//if (p_Dialog->getToolTip() != nullptr) {
	//	if (styles.istok(TEXT("tooltips"))) {
	//		this->m_ToolTipHWND = p_Dialog->getToolTip();
	//		TabCtrl_SetToolTips(m_Hwnd,this->m_ToolTipHWND);
	//		//AddToolTipToolInfo(this->m_ToolTipHWND, m_Hwnd);
	//	}
	//}

	if (this->m_bClosable)
	{
		//RECT rcClose;
		//GetCloseButtonRect(*rc, rcClose);
		//TabCtrl_SetPadding(m_Hwnd, (rcClose.right - rcClose.left), GetSystemMetrics(SM_CXFIXEDFRAME));
		TabCtrl_SetPadding(m_Hwnd, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYFIXEDFRAME));
	}
	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc( );
	SetProp( m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxTab::~DcxTab( ) {

	ImageList_Destroy( this->getImageList( ) );

	const auto nItems = TabCtrl_GetItemCount( m_Hwnd );
	for (auto n = decltype(nItems){0}; n < nItems; n++)
		this->deleteLParamInfo( n );

	this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTab::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

	//*ExStyles = WS_EX_CONTROLPARENT;
	*Styles |= TCS_OWNERDRAWFIXED;

	for (const auto &tsStyle : styles)
	{
#if DCX_USE_HASHING
		switch (std::hash<TString>{}(tsStyle))
		{
			case L"vertical"_hash:
				*Styles |= TCS_VERTICAL;
				break;
			case L"bottom"_hash:
				*Styles |= TCS_BOTTOM;
				break;
			case L"right"_hash:
				*Styles |= TCS_RIGHT;
				break;
			case L"fixedwidth"_hash:
				*Styles |= TCS_FIXEDWIDTH;
				break;
			case L"buttons"_hash:
				*Styles |= TCS_BUTTONS;
				break;
			case L"flat"_hash:
				*Styles |= TCS_FLATBUTTONS;
				break;
			case L"hot"_hash:
				*Styles |= TCS_HOTTRACK;
				break;
			case L"multiline"_hash:
				*Styles |= TCS_MULTILINE;
				break;
			case L"rightjustify"_hash:
				*Styles |= TCS_RIGHTJUSTIFY;
				break;
			case L"scrollopposite"_hash:
				*Styles |= TCS_SCROLLOPPOSITE;
				break;
			//case L"tooltips"_hash:
			//  *Styles |= TCS_TOOLTIPS;
			//	break;
			case L"flatseps"_hash:
				*ExStyles |= TCS_EX_FLATSEPARATORS;
				break;
			case L"closable"_hash:
				{
					m_bClosable = true;
					//*Styles |= TCS_OWNERDRAWFIXED;
				}
				break;
			case L"gradient"_hash:
				m_bGradient = true;
			default:
				break;
		}
#else
		if (tsStyle == TEXT("vertical"))
			*Styles |= TCS_VERTICAL;
		else if (tsStyle == TEXT("bottom"))
			*Styles |= TCS_BOTTOM;
		else if (tsStyle == TEXT("right"))
			*Styles |= TCS_RIGHT;
		else if (tsStyle == TEXT("fixedwidth"))
			*Styles |= TCS_FIXEDWIDTH;
		else if (tsStyle == TEXT("buttons"))
			*Styles |= TCS_BUTTONS;
		else if (tsStyle == TEXT("flat"))
			*Styles |= TCS_FLATBUTTONS;
		else if (tsStyle == TEXT("hot"))
			*Styles |= TCS_HOTTRACK;
		else if (tsStyle == TEXT("multiline"))
			*Styles |= TCS_MULTILINE;
		else if (tsStyle == TEXT("rightjustify"))
			*Styles |= TCS_RIGHTJUSTIFY;
		else if (tsStyle == TEXT("scrollopposite"))
			*Styles |= TCS_SCROLLOPPOSITE;
		//else if ( tsStyle == TEXT("tooltips") )
		//  *Styles |= TCS_TOOLTIPS;
		else if (tsStyle == TEXT("flatseps"))
			*ExStyles |= TCS_EX_FLATSEPARATORS;
		else if (tsStyle == TEXT("closable")) {
			this->m_bClosable = true;
			//*Styles |= TCS_OWNERDRAWFIXED;
		}
		else if (tsStyle == TEXT("gradient"))
			this->m_bGradient = true;
#endif
	}

	this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxTab::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
#if DCX_USE_HASHING
	const auto numtok = input.numtok();

	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
	case L"text"_hash:
	{
		if (numtok < 4)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		TCITEM tci;
		ZeroMemory(&tci, sizeof(TCITEM));

		tci.mask = TCIF_TEXT;
		tci.pszText = szReturnValue;
		tci.cchTextMax = MIRC_BUFFER_SIZE_CCH;

		TabCtrl_GetItem(m_Hwnd, nItem, &tci);
	}
	break;
	case L"num"_hash:
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), TabCtrl_GetItemCount(m_Hwnd));
		break;
		// [NAME] [ID] [PROP] [N]
	case L"icon"_hash:
	{
		if (numtok < 4)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto iTab = input.getnexttok().to_int() - 1;		// tok 4

		if (iTab < 0 && iTab >= TabCtrl_GetItemCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		TCITEM tci{ TCIF_IMAGE,0U,0U, nullptr, 0, 0, 0 };

		TabCtrl_GetItem(m_Hwnd, iTab, &tci);

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), tci.iImage + 1);
	}
	break;
	case L"sel"_hash:
	{
		const auto nItem = TabCtrl_GetCurSel(m_Hwnd);

		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), nItem + 1);
	}
	break;
	case L"seltext"_hash:
	{
		const auto nItem = TabCtrl_GetCurSel(m_Hwnd);

		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		TCITEM tci{ TCIF_TEXT,0U,0U, szReturnValue, MIRC_BUFFER_SIZE_CCH, 0, 0 };

		TabCtrl_GetItem(m_Hwnd, nItem, &tci);
	}
	break;
	case L"childid"_hash:
	{
		if (numtok < 4)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		TCITEM tci{ TCIF_PARAM,0U,0U, nullptr, 0, 0, 0 };

		TabCtrl_GetItem(m_Hwnd, nItem, &tci);

		auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam);

		auto c = this->m_pParentDialog->getControlByHWND(lpdtci->mChildHwnd);
		if (c != nullptr)
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), c->getUserID());
	}
	break;
	// [NAME] [ID] [PROP]
	case L"mouseitem"_hash:
	{
		TCHITTESTINFO tchi{};

		tchi.flags = TCHT_ONITEM;
		if (!GetCursorPos(&tchi.pt))
			throw Dcx::dcxException("Unable to get cursor position");

		MapWindowPoints(nullptr, m_Hwnd, &tchi.pt, 1);

		const auto tab = TabCtrl_HitTest(m_Hwnd, &tchi);

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), tab + 1);
	}
	break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
#else
	const auto numtok = input.numtok();
	const auto prop(input.getfirsttok(3));

	if ( prop == TEXT("text") && numtok > 3 ) {

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		TCITEM tci;
		ZeroMemory( &tci, sizeof( TCITEM ) );

		tci.mask = TCIF_TEXT;
		tci.pszText = szReturnValue;
		tci.cchTextMax = MIRC_BUFFER_SIZE_CCH;

		TabCtrl_GetItem( m_Hwnd, nItem, &tci );
	}
	else if ( prop == TEXT("num") ) {

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), TabCtrl_GetItemCount( m_Hwnd ) );
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("icon") && numtok > 3 ) {

		const auto iTab = input.getnexttok().to_int() - 1;		// tok 4

		if (iTab < 0 && iTab >= TabCtrl_GetItemCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		TCITEM tci;
		ZeroMemory( &tci, sizeof( TCITEM ) );

		tci.mask = TCIF_IMAGE;

		TabCtrl_GetItem( m_Hwnd, iTab, &tci );

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), tci.iImage + 1 );
	}
	else if ( prop == TEXT("sel") ) {

		const auto nItem = TabCtrl_GetCurSel(m_Hwnd);

		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), nItem + 1 );
	}
	else if ( prop == TEXT("seltext") ) {

		const auto nItem = TabCtrl_GetCurSel(m_Hwnd);

		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		TCITEM tci;
		ZeroMemory( &tci, sizeof( TCITEM ) );

		tci.mask = TCIF_TEXT;
		tci.pszText = szReturnValue;
		tci.cchTextMax = MIRC_BUFFER_SIZE_CCH;

		TabCtrl_GetItem( m_Hwnd, nItem, &tci );
	}
	else if ( prop == TEXT("childid") && numtok > 3 ) {

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		TCITEM tci;
		ZeroMemory( &tci, sizeof( TCITEM ) );

		tci.mask = TCIF_PARAM;
		TabCtrl_GetItem( m_Hwnd, nItem, &tci );

		auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam);

		auto c = this->m_pParentDialog->getControlByHWND(lpdtci->mChildHwnd);
		if ( c != nullptr ) 
			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), c->getUserID( ) );
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("mouseitem")) {
		TCHITTESTINFO tchi;

		tchi.flags = TCHT_ONITEM;
		if (!GetCursorPos(&tchi.pt))
			throw Dcx::dcxException("Unable to get cursor position");

		MapWindowPoints(nullptr, m_Hwnd, &tchi.pt, 1);

		const auto tab = TabCtrl_HitTest(m_Hwnd, &tchi);

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), tab + 1);
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
#endif
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTab::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')]) {
		TCITEM tci = { 0 };
		const auto nItems = TabCtrl_GetItemCount(m_Hwnd);

		for (auto n = decltype(nItems){0}; n < nItems; n++)
		{
			ZeroMemory(&tci, sizeof(TCITEM));

			tci.mask = TCIF_PARAM;

			if (TabCtrl_GetItem(m_Hwnd, n, &tci)) {
				auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam);

				if (lpdtci != nullptr && lpdtci->mChildHwnd != nullptr && IsWindow(lpdtci->mChildHwnd)) {
					DestroyWindow(lpdtci->mChildHwnd);
					delete lpdtci;
				}
			}
		}

		TabCtrl_DeleteAllItems(m_Hwnd);
	}

	// xdid -a [NAME] [ID] [SWITCH] [N] [ICON] [TEXT][TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)[TAB](TOOLTIP)
	if ( flags[TEXT('a')] && numtok > 4 ) {
		TCITEM tci;
		ZeroMemory( &tci, sizeof( TCITEM ) );
		tci.mask = TCIF_IMAGE | TCIF_PARAM;

		const auto data(input.getfirsttok(1, TSTABCHAR).trim());

		TString control_data;
		TString tooltip;
		const auto nToks = input.numtok(TSTABCHAR);

		if ( nToks > 1 ) {
			control_data = input.getnexttok( TSTABCHAR).trim();	// tok 2

			if ( nToks > 2 )
				tooltip = input.getlasttoks().trim();	// tok 3, -1, TSTAB
		}

		auto nIndex = data.getfirsttok(4).to_int() - 1;

		if ( nIndex == -1 )
			nIndex += TabCtrl_GetItemCount( m_Hwnd ) + 1;

		tci.iImage = data.getnexttok( ).to_int( ) - 1;	// tok 5

		//auto lpdtci = new DCXTCITEM;
		auto lpdtci = std::make_unique<DCXTCITEM>();

		lpdtci->tsTipText = tooltip;

		// Itemtext
		TString itemtext;
		if ( data.numtok( ) > 5 ) {
			itemtext = data.getlasttoks();	// tok 6, -1
			tci.mask |= TCIF_TEXT;

			//if (this->m_bClosable)
			//{
			//	//itemtext += TEXT("***");	// TEXT("   ");
			//	RECT rc;
			//	GetCloseButtonRect(this->m_rc, rc);
			//	TabCtrl_SetPadding(m_Hwnd, 0, 0);
			//}

			tci.pszText = itemtext.to_chr( );
		}

		if ( control_data.numtok( ) > 5 ) {
			auto p_Control = this->m_pParentDialog->addControl(control_data, 1,
				CTLF_ALLOW_TREEVIEW |
				CTLF_ALLOW_LISTVIEW |
				CTLF_ALLOW_RICHEDIT |
				CTLF_ALLOW_DIVIDER |
				CTLF_ALLOW_PANEL |
				CTLF_ALLOW_TAB |
				CTLF_ALLOW_REBAR |
				CTLF_ALLOW_WEBCTRL |
				CTLF_ALLOW_EDIT |
				CTLF_ALLOW_IMAGE |
				CTLF_ALLOW_LIST
				, m_Hwnd);

			lpdtci->mChildHwnd = p_Control->getHwnd( );

			//const auto ID = mIRC_ID_OFFSET + (UINT)control_data.gettok(1).to_int();
			//
			//if (!this->m_pParentDialog->isIDValid(ID, true))
			//	throw Dcx::dcxException(TEXT("Control with ID \"%\" already exists"), ID - mIRC_ID_OFFSET);
			//
			//try {
			//	auto p_Control = DcxControl::controlFactory(this->m_pParentDialog, ID, control_data, 2,
			//		CTLF_ALLOW_TREEVIEW |
			//		CTLF_ALLOW_LISTVIEW |
			//		CTLF_ALLOW_RICHEDIT |
			//		CTLF_ALLOW_DIVIDER |
			//		CTLF_ALLOW_PANEL |
			//		CTLF_ALLOW_TAB |
			//		CTLF_ALLOW_REBAR |
			//		CTLF_ALLOW_WEBCTRL |
			//		CTLF_ALLOW_EDIT |
			//		CTLF_ALLOW_IMAGE |
			//		CTLF_ALLOW_LIST
			//		,m_Hwnd);
			//
			//	lpdtci->mChildHwnd = p_Control->getHwnd( );
			//	this->m_pParentDialog->addControl( p_Control );
			//}
			//catch (std::exception &e) {
			//	this->showErrorEx(nullptr, TEXT("-a"), TEXT("Unable To Create Control %d (%S)"), ID - mIRC_ID_OFFSET, e.what());
			//	throw;
			//}
		}
		tci.lParam = (LPARAM)lpdtci.release();

		TabCtrl_InsertItem( m_Hwnd, nIndex, &tci );
		this->activateSelectedTab( );
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('c')] && numtok > 3 ) {
		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if ( nItem < 0 && nItem >= TabCtrl_GetItemCount( m_Hwnd ) )
			throw Dcx::dcxException("Invalid Item");
		
		TabCtrl_SetCurSel(m_Hwnd, nItem);
		this->activateSelectedTab( );
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('d')] && numtok > 3 ) {
		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		// if a valid item to delete
		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");
		
		const auto curSel = TabCtrl_GetCurSel(m_Hwnd);
		TCITEM tci;
		ZeroMemory( &tci, sizeof( TCITEM ) );

		tci.mask = TCIF_PARAM;

		if (TabCtrl_GetItem(m_Hwnd, nItem, &tci)) {
			auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam);

			if ( lpdtci != nullptr && lpdtci->mChildHwnd != nullptr && IsWindow( lpdtci->mChildHwnd ) ) {
				DestroyWindow( lpdtci->mChildHwnd );
				delete lpdtci;
			}
		}

		TabCtrl_DeleteItem( m_Hwnd, nItem );

		// select the next tab item if its the current one
		const auto iTotal = TabCtrl_GetItemCount(m_Hwnd);
		if ((curSel == nItem) && (iTotal > 0)) {
			if (nItem < iTotal)
				TabCtrl_SetCurSel(m_Hwnd, nItem);
			else
				TabCtrl_SetCurSel(m_Hwnd, iTotal -1);	// nItem -1

			this->activateSelectedTab( );
		}
	}
	// xdid -l [NAME] [ID] [SWITCH] [N] [ICON]
	else if ( flags[TEXT('l')] && numtok > 4 ) {
		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4
		const auto nIcon = input.getnexttok().to_int() - 1;	// tok 5

		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");
		
		TCITEM tci;
		ZeroMemory( &tci, sizeof( TCITEM ) );
		tci.mask = TCIF_IMAGE;
		tci.iImage = nIcon;

		TabCtrl_SetItem( m_Hwnd, nItem, &tci );
	}
	// xdid -m [NAME] [ID] [SWITCH] [X] [Y]
	else if ( flags[TEXT('m')] && numtok > 4 ) {

		const auto X = input.getnexttok( ).to_int( );	// tok 4
		const auto Y = input.getnexttok().to_int();	// tok 5

		TabCtrl_SetItemSize( m_Hwnd, X, Y );
	}
	// This it to avoid an invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('r')] ) {
	}
	// xdid -t [NAME] [ID] [SWITCH] [N] (text)
	else if ( flags[TEXT('t')] && numtok > 3 ) {

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		TString itemtext;

		TCITEM tci;
		ZeroMemory( &tci, sizeof( TCITEM ) );
		tci.mask = TCIF_TEXT;

		if ( numtok > 4 )
			itemtext = input.getlasttoks().trim();	// tok 5, -1

		tci.pszText = itemtext.to_chr( );

		TabCtrl_SetItem( m_Hwnd, nItem, &tci );
	}

	// xdid -v [DNAME] [ID] [SWITCH] [N] [POS]
	else if (flags[TEXT('v')] && numtok > 4) {
		const auto nItem = input.getnexttok().to_int() - 1;		// tok 4
		const auto pos = input.getnexttok().to_int() - 1;	// tok 5
		BOOL adjustDelete = FALSE;

		if (nItem < 0 && nItem >= TabCtrl_GetItemCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		if (pos < 0 && pos >= TabCtrl_GetItemCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		if (nItem == pos)
			return;

		// does the nItem index get shifted after we insert
		if (nItem > pos)
			adjustDelete = TRUE;

		auto curSel = TabCtrl_GetCurSel(m_Hwnd);
		if (curSel == nItem)
			curSel = pos;
		else if (curSel > nItem)
			curSel--;

		// get the item we're moving
		//TCHAR* text = new TCHAR[MIRC_BUFFER_SIZE_CCH];
		auto text = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);

		TCITEM tci;
		ZeroMemory(&tci, sizeof(TCITEM));

		tci.pszText = text.get();
		tci.cchTextMax = MIRC_BUFFER_SIZE_CCH;
		tci.mask = TCIF_IMAGE | TCIF_PARAM | TCIF_TEXT | TCIF_STATE;

		TabCtrl_GetItem(m_Hwnd, nItem, &tci);

		// insert it into the new position
		TabCtrl_InsertItem(m_Hwnd, pos, &tci);

		// remove the old tab item
		TabCtrl_DeleteItem(m_Hwnd, (adjustDelete ? nItem + 1 : nItem));

		// select the next tab item if its the current one
		if (curSel >= 0) {
			TabCtrl_SetCurSel(m_Hwnd, curSel);

			this->activateSelectedTab();
		}
	}

	// xdid -w [NAME] [ID] [SWITCH] [FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')] && numtok > 5) {
		const auto flag(input.getnexttok());		// tok 4
		const auto index = input.getnexttok().to_int();	// tok 5
		auto filename(input.getlasttoks());			// tok 6, -1

		auto himl = this->getImageList();

		if (himl == nullptr) {
			himl = this->createImageList();

			if (himl != nullptr)
				this->setImageList(himl);
		}
		if (himl == nullptr)
			throw Dcx::dcxException("Unable to get Image List");

		//HICON icon = dcxLoadIcon(index, filename, false, flag);
		//
		//if (icon != nullptr) {
		//	ImageList_AddIcon(himl, icon);
		//	DestroyIcon(icon);
		//}

		Dcx::dcxIconResource icon(index, filename, false, flag);

		ImageList_AddIcon(himl, icon);
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if ( flags[TEXT('y')] ) {

		ImageList_Destroy( this->getImageList( ) );
	}
	// xdid -m [NAME] [ID] [SWITCH] [+FLAGS] [WIDTH]
	// xdid -m -> [NAME] [ID] -m [+FLAGS] [WIDTH]
	else if (flags[TEXT('m')]) {
		const XSwitchFlags xFlags(input.getnexttok());	// tok 4
		auto iWidth = input.getnexttok().to_int();		// tok 5

		if (iWidth < -1)
			iWidth = -1;

		TabCtrl_SetMinTabWidth(m_Hwnd, iWidth);

		this->activateSelectedTab();
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxTab::getImageList(  ) const {

  return TabCtrl_GetImageList( m_Hwnd );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTab::setImageList( HIMAGELIST himl ) {

  TabCtrl_SetImageList( m_Hwnd, himl );
}

/*!
 * \brief blah
 *
 * blah
 */

//HIMAGELIST DcxTab::createImageList( ) {
//
//  return ImageList_Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );
//}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTab::deleteLParamInfo(const int nItem)
{
	TCITEM tci;
	ZeroMemory(&tci, sizeof(TCITEM));

	tci.mask = TCIF_PARAM;

	if (TabCtrl_GetItem(m_Hwnd, nItem, &tci)) {

		auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam);

		delete lpdtci;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTab::activateSelectedTab( ) {

	auto nTab = TabCtrl_GetItemCount(m_Hwnd);
	const auto nSel = TabCtrl_GetCurSel(m_Hwnd);

	if (nTab <= 0)
		return;

	RECT tabrect;
	if (!GetWindowRect(m_Hwnd, &tabrect))
		return;

	TabCtrl_AdjustRect(m_Hwnd, FALSE, &tabrect);

	RECT rc;
	if (!GetWindowRect(m_Hwnd, &rc))
		return;

	OffsetRect(&tabrect, -rc.left, -rc.top);

	TCITEM tci;
	ZeroMemory(&tci, sizeof(TCITEM));
	tci.mask = TCIF_PARAM;

	auto hdwp = BeginDeferWindowPos(nTab + 1);

	if (hdwp == nullptr)
		throw Dcx::dcxException("activateSelectedTab() - Unable to size tabs");

	//HWND hSelChild = nullptr;
	//
	//while (nTab-- > 0) {
	//
	//	TabCtrl_GetItem(m_Hwnd, nTab, &tci);
	//	LPDCXTCITEM lpdtci = (LPDCXTCITEM)tci.lParam;
	//
	//	if (lpdtci->mChildHwnd != nullptr && IsWindow(lpdtci->mChildHwnd)) {
	//
	//		if (nTab == nSel) {
	//			hSelChild = lpdtci->mChildHwnd;
	//
	//			//hdwp = DeferWindowPos( hdwp, lpdtci->mChildHwnd, nullptr, 
	//			//	tabrect.left, tabrect.top, tabrect.right-tabrect.left, tabrect.bottom-tabrect.top, 
	//			//	SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOOWNERZORDER |SWP_DRAWFRAME|SWP_FRAMECHANGED);
	//			//hdwp = DeferWindowPos(hdwp, lpdtci->mChildHwnd, nullptr, 0, 0, 0, 0,
	//			//	SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
	//			hdwp = DeferWindowPos(hdwp, hSelChild, nullptr,
	//				tabrect.left, tabrect.top, tabrect.right - tabrect.left, tabrect.bottom - tabrect.top,
	//				SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOOWNERZORDER);
	//			hdwp = DeferWindowPos(hdwp, hSelChild, nullptr,
	//				tabrect.left, tabrect.top, tabrect.right - tabrect.left, tabrect.bottom - tabrect.top,
	//				SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_DRAWFRAME | SWP_FRAMECHANGED);
	//		}
	//		else {
	//			hdwp = DeferWindowPos(hdwp, lpdtci->mChildHwnd, nullptr, 0, 0, 0, 0,
	//				SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
	//		}
	//		if (hdwp == nullptr)
	//			break;
	//	}
	//}
	//if (hdwp != nullptr)
	//	EndDeferWindowPos(hdwp);
	//
	////if (hSelChild != nullptr) {
	////	InvalidateRect(hSelChild, nullptr, TRUE);
	////	SetWindowPos(hSelChild, nullptr, tabrect.left, tabrect.top, tabrect.right - tabrect.left, tabrect.bottom - tabrect.top, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_DRAWFRAME | SWP_FRAMECHANGED);
	////	//UpdateWindow(hSelChild);
	////}

	auto hTemp = hdwp;

	while (nTab-- > 0) {

		if (!TabCtrl_GetItem(m_Hwnd, nTab, &tci))
			continue;

		auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam);
		if (lpdtci != nullptr)
		{
			auto hSelChild = lpdtci->mChildHwnd;

			if (hSelChild != nullptr && IsWindow(hSelChild)) {

				if (nTab == nSel) {

					hTemp = DeferWindowPos(hdwp, hSelChild, nullptr,
						tabrect.left, tabrect.top, tabrect.right - tabrect.left, tabrect.bottom - tabrect.top,
						SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOOWNERZORDER);

					if (hTemp == nullptr)
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
				if (hTemp == nullptr)
					break;
				hdwp = hTemp;
			}
		}
	}
	if (hdwp != nullptr)
		EndDeferWindowPos(hdwp);
}

void DcxTab::getTab(const int index, LPTCITEM tcItem) const {
	TabCtrl_GetItem(m_Hwnd, index, tcItem);
}

int DcxTab::getTabCount() const {
	return TabCtrl_GetItemCount(m_Hwnd);
}

void DcxTab::GetCloseButtonRect(const RECT& rcItem, RECT& rcCloseButton)
{
	// ----------
	//rcCloseButton.top = rcItem.top + 2;
	//rcCloseButton.bottom = rcCloseButton.top + (m_iiCloseButton.rcImage.bottom - m_iiCloseButton.rcImage.top);
	//rcCloseButton.right = rcItem.right - 2;
	//rcCloseButton.left = rcCloseButton.right - (m_iiCloseButton.rcImage.right - m_iiCloseButton.rcImage.left);
	// ----------
	//rcCloseButton.top = rcItem.top + 2;
	//rcCloseButton.bottom = rcCloseButton.top + (16);
	//rcCloseButton.right = rcItem.right - 2;
	//rcCloseButton.left = rcCloseButton.right - (16);
	// ----------
	rcCloseButton.top = rcItem.top;
	rcCloseButton.bottom = rcCloseButton.top + GetSystemMetrics(SM_CYSMICON);
	rcCloseButton.right = rcItem.right - GetSystemMetrics(SM_CXEDGE);
	rcCloseButton.left = rcCloseButton.right - GetSystemMetrics(SM_CXSMICON);
	// ----------
}

const TString DcxTab::getStyles(void) const {
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(m_Hwnd);
	const auto ExStyles = GetWindowExStyle(m_Hwnd);

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

void DcxTab::toXml(TiXmlElement *const xml) const {
	__super::toXml(xml);
	const auto count = this->getTabCount();
	auto buf = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);
	TCITEM tci = { 0 };

	for (auto i = decltype(count){0}; i < count; i++) {
		tci.cchTextMax = MIRC_BUFFER_SIZE_CCH -1;
		tci.pszText = buf.get();
		tci.mask |= TCIF_TEXT;
		if(TabCtrl_GetItem(m_Hwnd, i, &tci)) {
			auto lpdtci = reinterpret_cast<LPDCXTCITEM>(tci.lParam);
			auto ctrl = this->m_pParentDialog->getControlByHWND(lpdtci->mChildHwnd);
			if (ctrl != nullptr) {
				auto ctrlxml = ctrl->toXml();
				// we need to remove hidden style here
				TString styles(ctrlxml->Attribute("styles"));
				if (!styles.empty()) {
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

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxTab::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed)
{
	switch (uMsg) {
		case WM_NOTIFY : 
			{
				dcxlParam(LPNMHDR, hdr);

				if (hdr == nullptr)
					break;

				switch (hdr->code) {
					case NM_RCLICK:
					{
						if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
						{
							TCHITTESTINFO tchi;

							tchi.flags = TCHT_ONITEM;
							if (GetCursorPos(&tchi.pt))
							{
								MapWindowPoints(nullptr, m_Hwnd, &tchi.pt, 1);

								const auto tab = TabCtrl_HitTest(m_Hwnd, &tchi);
								//TabCtrl_GetCurSel(m_Hwnd);

								if (tab != -1)
									this->execAliasEx(TEXT("%s,%d,%d"), TEXT("rclick"), this->getUserID(), tab + 1);
							}
						}

						bParsed = TRUE;
						break;
					}

					case NM_CLICK:
						{
							if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
							{
								const auto tab = TabCtrl_GetCurFocus(m_Hwnd);
								//int tab = TabCtrl_GetCurSel(m_Hwnd);

								if (tab != -1) {
									if (this->m_bClosable) {
										POINT pt;
										if (GetCursorPos(&pt))
										{
											RECT rc = { 0 };

											MapWindowPoints(nullptr, m_Hwnd, &pt, 1);
											if (TabCtrl_GetItemRect(m_Hwnd, tab, &rc))
											{
												RECT rcCloseButton = { 0 };
												GetCloseButtonRect(rc, rcCloseButton);

												if (PtInRect(&rcCloseButton, pt)) {
													this->execAliasEx(TEXT("%s,%d,%d"), TEXT("closetab"), this->getUserID(), tab + 1);
													break;
												}
											}
										}
									}
									this->execAliasEx(TEXT("%s,%d,%d"), TEXT("sclick"), this->getUserID(), tab +1);
								}
							}
						}
					// fall through.
					case TCN_SELCHANGE:
						{
							this->activateSelectedTab();
							bParsed = TRUE;
						}
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

				if ((idata == nullptr) || (!IsWindow(idata->hwndItem)))
					break;

				RECT rect = { 0 };
				const auto nTabIndex = idata->itemID;

				CopyRect(&rect, &idata->rcItem);

				// if themes are active use them.
				// call default WndProc(), DrawThemeParentBackgroundUx() is only temporary
				DcxControl::DrawCtrlBackground(idata->hDC, this, &rect);
				//DrawThemeParentBackgroundUx(m_Hwnd, idata->hDC, &rect);
				//CopyRect(&rect, &idata->rcItem);
				if (this->m_bGradient) {
					if (this->m_clrBackText == CLR_INVALID)
						// Gives a nice silver/gray gradient
						XPopupMenuItem::DrawGradient(idata->hDC, &rect, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNFACE), TRUE);
					else
						XPopupMenuItem::DrawGradient(idata->hDC, &rect, GetSysColor(COLOR_BTNHIGHLIGHT), this->m_clrBackText, TRUE);
				}
				rect.left += 1+ GetSystemMetrics(SM_CXEDGE); // move in past border.
				rect.top += 1 + GetSystemMetrics(SM_CYEDGE); //4;

				// TODO: (twig) Ook can u take a look at this plz? string stuff isnt my forte
				TCHAR szLabel[MIRC_BUFFER_SIZE_CCH];
				szLabel[0] = TEXT('\0');

				//TCITEM tci;

				//tci.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_STATE;
				//tci.pszText = szLabel;
				//tci.cchTextMax = MIRC_BUFFER_SIZE_CCH;
				//tci.dwStateMask = TCIS_HIGHLIGHTED;

				TCITEM tci{ TCIF_TEXT | TCIF_IMAGE | TCIF_STATE, 0, TCIS_HIGHLIGHTED, szLabel, MIRC_BUFFER_SIZE_CCH, 0, NULL };

				if (!TabCtrl_GetItem(getHwnd(), nTabIndex, &tci)) {
					showError(nullptr, TEXT("DcxTab Fatal Error"), TEXT("Invalid item"));
					break;
				}

				const TString label(tci.pszText);	// copy buffer, this may not be the same buffer as provided by us.

				// fill the rect so it appears to "merge" with the tab page content
				//if (!dcxIsThemeActive())
				//FillRect(idata->hDC, &rect, GetSysColorBrush(COLOR_BTNFACE));

				// set transparent so text background isnt annoying
				const auto iOldBkMode = SetBkMode(idata->hDC, TRANSPARENT);
				Auto(SetBkMode(idata->hDC, iOldBkMode));

				// Draw icon on left side if the item has an icon
				if (tci.iImage != -1) {
					if (ImageList_DrawEx(getImageList(), tci.iImage, idata->hDC, rect.left, rect.top, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT))
					{
						//IMAGEINFO ii;
						//if (ImageList_GetImageInfo(this->getImageList(), tci.iImage, &ii))
						//	rect.left += (ii.rcImage.right - ii.rcImage.left);
						int iSizeX = 0, iSizeY = 0;
						if (ImageList_GetIconSize(getImageList(), &iSizeX, &iSizeY))
							rect.left += iSizeX;
					}
				}
				// Draw 'Close button' at right side
				if (m_bClosable) {
					RECT rcCloseButton = { 0 };
					GetCloseButtonRect(rect, rcCloseButton);
					// Draw systems close button ? or do you want a custom close button?
					DrawFrameControl(idata->hDC, &rcCloseButton, DFC_CAPTION, DFCS_CAPTIONCLOSE | DFCS_FLAT | DFCS_TRANSPARENT);
					//MoveToEx( idata->hDC, rcCloseButton.left, rcCloseButton.top, nullptr );
					//LineTo( idata->hDC, rcCloseButton.right, rcCloseButton.bottom );
					//MoveToEx( idata->hDC, rcCloseButton.right, rcCloseButton.top, nullptr );
					//LineTo( idata->hDC, rcCloseButton.left, rcCloseButton.bottom );

					rect.right = rcCloseButton.left - 2;
				}
				COLORREF crOldColor = CLR_INVALID;

				if (dcx_testflag(tci.dwState, TCIS_HIGHLIGHTED))
					crOldColor = SetTextColor(idata->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));

				//rect.top += 1+ GetSystemMetrics(SM_CYEDGE); //4;

				//DrawText(idata->hDC, label.to_chr(), label.len(), &rect, DT_SINGLELINE | DT_TOP | DT_NOPREFIX);
				// allow mirc formatted text.
				//mIRC_DrawText(idata->hDC, label, &rect, DT_SINGLELINE | DT_TOP | DT_NOPREFIX, false, this->m_bUseUTF8);
				//if (!this->m_bCtrlCodeText) {
				//	if (this->m_bShadowText)
				//		dcxDrawShadowText(idata->hDC, label.to_wchr(this->m_bUseUTF8), label.wlen(),&rect, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE, GetTextColor(idata->hDC), 0, 5, 5);
				//	else
				//		DrawTextW( idata->hDC, label.to_wchr(this->m_bUseUTF8), label.wlen( ), &rect, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE );
				//}
				//else
				//	mIRC_DrawText( idata->hDC, label, &rect, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE, this->m_bShadowText, this->m_bUseUTF8);

				this->ctrlDrawText(idata->hDC, label, &rect, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE);

				if (dcx_testflag(tci.dwState, TCIS_HIGHLIGHTED))
					SetTextColor(idata->hDC, crOldColor);
				break;
			}
			// NB: WM_MEASUREITEM is never called here as its called before we subclass the control, so we miss it.
			//		This needs fixed, must change how we subclass the controls...
			//case WM_MEASUREITEM:
			//{
			//					   //if (!m_bClosable)
			//						  // break;
			//					   auto mis = (LPMEASUREITEMSTRUCT)lParam;
			//					   mis->itemHeight = mis->itemHeight;
			//}
	}

	return 0L;
}

LRESULT DcxTab::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed )
{
	LRESULT lRes = 0L;
	switch( uMsg ) {

		case WM_CONTEXTMENU:
		case WM_LBUTTONUP:
			break;

		case WM_NOTIFY : 
			{
				dcxlParam(LPNMHDR, hdr);

				if (hdr == nullptr)
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

				if (IsWindow(hdr->hwndFrom)) {
					auto c_this = reinterpret_cast<DcxControl *>(GetProp(hdr->hwndFrom, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
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

		case WM_SIZE:
			{
				this->activateSelectedTab( );
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_SIZE))
					this->execAliasEx(TEXT("%s,%d"), TEXT("sizing"), this->getUserID( ) );
			}
			break;

		case WM_ERASEBKGND:
			{
				if (this->isExStyle(WS_EX_TRANSPARENT))
					this->DrawParentsBackground((HDC)wParam);
				else
					DcxControl::DrawCtrlBackground((HDC) wParam,this);
				bParsed = TRUE;
				return TRUE;
			}
			break;

		case WM_PAINT:
			{
				if (!this->m_bAlphaBlend)
					break;

				PAINTSTRUCT ps;
				auto hdc = BeginPaint(m_Hwnd, &ps);

				bParsed = TRUE;

				// Setup alpha blend if any.
				auto ai = this->SetupAlphaBlend(&hdc);

				lRes = CallWindowProc( this->m_DefaultWindowProc, m_Hwnd, uMsg, (WPARAM) hdc, lParam );

				this->FinishAlphaBlend(ai);

				EndPaint( m_Hwnd, &ps );
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
			lRes = this->CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return lRes;
}
