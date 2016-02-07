/*!
* \file dcxcomboex.cpp
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
#include "Classes/dcxcomboex.h"
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

DcxComboEx::DcxComboEx(const UINT ID, DcxDialog *const  p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
	, m_tsSelected("")
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	this->m_Hwnd = CreateWindowEx(
		ExStyles,
		DCX_COMBOEXCLASS,
		nullptr,
		WS_CHILD | CBS_AUTOHSCROLL | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(this->m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme) {
		Dcx::UXModule.dcxSetWindowTheme(this->m_Hwnd, L" ", L" ");
		//SendMessage( this->m_Hwnd, CBEM_SETWINDOWTHEME, NULL, (LPARAM)(LPCWSTR)L" "); // do this instead?
	}

	this->m_EditHwnd = (HWND) this->getEditControl();

	if (IsWindow(this->m_EditHwnd)) {
		if (bNoTheme)
			Dcx::UXModule.dcxSetWindowTheme(this->m_EditHwnd, L" ", L" ");

		this->m_exEdit.cHwnd = this->m_Hwnd;
		this->m_exEdit.pHwnd = mParentHwnd;

		//SetWindowLong( this->m_EditHwnd, GWL_STYLE, GetWindowLong( this->m_EditHwnd, GWL_STYLE ));// | ES_AUTOHSCROLL );
		this->m_exEdit.OldProc = SubclassWindow(this->m_EditHwnd, DcxComboEx::ComboExEditProc);
		SetWindowLongPtr(this->m_EditHwnd, GWLP_USERDATA, (LONG)&this->m_exEdit);
	}

	this->m_hComboHwnd = (HWND)SendMessage(this->m_Hwnd, CBEM_GETCOMBOCONTROL, 0, 0);
	if (IsWindow(this->m_hComboHwnd)) {
		if (bNoTheme)
			Dcx::UXModule.dcxSetWindowTheme(this->m_hComboHwnd, L" ", L" ");

		COMBOBOXINFO cbi = { 0 };
		cbi.cbSize = sizeof(cbi);
		if (!GetComboBoxInfo(this->m_hComboHwnd, &cbi))
			throw Dcx::dcxException("Unable to get Combo Box Info");

		if (styles.istok(TEXT("sort"))) { // doesnt work atm.
			if (IsWindow(cbi.hwndList))
				AddStyles(cbi.hwndList, GWL_STYLE, LBS_SORT);
		}
		if (styles.istok(TEXT("hscroll"))) {
			//if (IsWindow(cbi.hwndCombo))
			//AddStyles(cbi.hwndCombo, GWL_STYLE, WS_HSCROLL);
			if (IsWindow(cbi.hwndList))
				AddStyles(cbi.hwndList, GWL_STYLE, WS_HSCROLL);
		}
	}
	//if (p_Dialog->getToolTip() != nullptr) {
	//	if (styles.istok(TEXT("tooltips"))) {
	//		this->m_ToolTipHWND = p_Dialog->getToolTip();
	//		AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
	//		AddToolTipToolInfo(this->m_ToolTipHWND, this->m_EditHwnd);
	//	}
	//}

	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);

	DragAcceptFiles(this->m_Hwnd, TRUE);

	// fix bug with disabled creation
	// todo: fix this properly
	if (dcx_testflag(Styles, WS_DISABLED)) {
		EnableWindow(this->m_Hwnd, TRUE);
		EnableWindow(this->m_Hwnd, FALSE);
	}
}

/*!
* \brief blah
*
* blah
*/

DcxComboEx::~DcxComboEx( ) {

	ImageList_Destroy( this->getImageList( ) );

	this->unregistreDefaultWindowProc( );
}

/*!
* \brief blah
*
* blah
*/

void DcxComboEx::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	//*ExStyles |= CBES_EX_NOSIZELIMIT;

	for (const auto &tsStyle: styles)
	{
		if ( tsStyle == TEXT("simple") )
			*Styles |= CBS_SIMPLE;
		else if ( tsStyle == TEXT("dropdown") )
			*Styles |= CBS_DROPDOWNLIST;
		else if ( tsStyle == TEXT("dropedit") )
			*Styles |= CBS_DROPDOWN;
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

void DcxComboEx::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const
{
	const auto numtok = input.numtok();

	const auto prop(input.getfirsttok(3));

	// [NAME] [ID] [PROP] [N]
	if ( prop == TEXT("text") && numtok > 3 ) {

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if ( nItem > -1 ) {

			COMBOBOXEXITEM cbi;
			ZeroMemory( &cbi, sizeof( COMBOBOXEXITEM ) );

			cbi.mask = CBEIF_TEXT;
			cbi.iItem = nItem;
			cbi.pszText = szReturnValue;
			cbi.cchTextMax = MIRC_BUFFER_SIZE_CCH;

			this->getItem( &cbi );
		}
		else {
			if (nItem != -1 || (!this->isStyle(CBS_DROPDOWN) && !this->isStyle(CBS_SIMPLE)))
				throw Dcx::dcxException("Invalid Item");

			//auto hEdit = (HWND)this->getEditControl();
			//if (IsWindow(hEdit))
			//	GetWindowText(hEdit, szReturnValue, MIRC_BUFFER_SIZE_CCH);

			dcx_strcpyn(szReturnValue, m_tsSelected.to_chr(), MIRC_BUFFER_SIZE_CCH);
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("seltext") ) {

		const auto nItem = this->getCurSel();

		if ( nItem > -1 )
			throw Dcx::dcxException("Invalid Item");

		COMBOBOXEXITEM cbi;
		ZeroMemory( &cbi, sizeof( COMBOBOXEXITEM ) );

		cbi.mask = CBEIF_TEXT;
		cbi.iItem = nItem;
		cbi.pszText = szReturnValue;
		cbi.cchTextMax = MIRC_BUFFER_SIZE_CCH;

		this->getItem( &cbi );
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("sel") ) {

		const auto nItem = this->getCurSel();

		if ( nItem < 0 )
			throw Dcx::dcxException("Invalid Item");

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), nItem + 1 );
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("num") ) {

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getCount( ) );
	}
	// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N]
	else if ( prop == TEXT("find") && numtok > 5 ) {

		const auto matchtext(input.getfirsttok(2, TSTAB).trim());
		const auto params(input.getnexttok(TSTAB).trim());	// tok 3

		if ( !matchtext.empty() ) {

			auto SearchType = DcxSearchTypes::SEARCH_E;
			const auto tsSearchType((params++)[0]);

			//if ( params.getfirsttok( 1 ) == TEXT("R") )
			if (tsSearchType == TEXT('R'))
				SearchType = DcxSearchTypes::SEARCH_R;
			else if (tsSearchType == TEXT('W'))
				SearchType = DcxSearchTypes::SEARCH_W;

			const auto N = params++.to_<UINT>();	// tok 2

			const auto nItems = this->getCount();
			auto count = decltype(N){0};

			// count total
			if ( N == 0 ) {
				for (auto i = decltype(nItems){0}; i < nItems; i++)
				{
					if ( this->matchItemText( i, matchtext, SearchType ) )
						count++;
				}

				wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), count );
			}
			// find Nth matching
			else {
				for (auto i = decltype(nItems){0}; i < nItems; i++)
				{
					if ( this->matchItemText( i, matchtext, SearchType ) )
						count++;

					if ( count == N ) {

						wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), i + 1 );
						return;
					}
				}
			} // else
		}
	}
	// [NAME] [ID] [PROP] [ROW]
	else if (prop == TEXT("markeditem") && numtok == 4) {

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		COMBOBOXEXITEM cbi;
		ZeroMemory(&cbi, sizeof(COMBOBOXEXITEM));

		cbi.mask = CBEIF_LPARAM;
		cbi.iItem = nItem;

		this->getItem(&cbi);

		auto mycbi = reinterpret_cast<LPDCXCBITEM>(cbi.lParam);

		if (mycbi == nullptr)
			throw Dcx::dcxException("Unable to get DCX Item");

		dcx_strcpyn(szReturnValue, mycbi->tsMark.to_chr(), MIRC_BUFFER_SIZE_CCH);

	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

/*!
* \brief blah
*
* blah
*/

void DcxComboEx::parseCommandRequest( const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));

	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		this->resetContent();

	// xdid -a [NAME] [ID] [SWITCH] [N] [INDENT] [ICON] [STATE] [OVERLAY] Item Text
	// [NAME] [ID] -a [N] [INDENT] [ICON] [STATE] [OVERLAY] Item Text
	if (flags[TEXT('a')] && numtok > 8) {
#if TSTRING_TESTCODE
		auto nPos = input.getnexttokas<int>() - 1;		// tok 4
		const auto indent = input.getnexttokas<int>();	// tok 5
		const auto icon = input.getnexttokas<int>() - 1;	// tok 6
		const auto state = input.getnexttokas<int>() - 1;	// tok 7
		const auto overlay = input.getnexttokas<int>();		// tok 8		(never used, here for spacing only atm)
#else
		auto nPos = input.getnexttok().to_int() - 1;		// tok 4
		const auto indent = input.getnexttok().to_int();	// tok 5
		const auto icon = input.getnexttok().to_int() - 1;	// tok 6
		const auto state = input.getnexttok().to_int() - 1;	// tok 7
		const auto overlay = input.getnexttok().to_int();	// tok 8		(never used, here for spacing only atm)
#endif
		const auto itemtext(input.getlasttoks());			// tok 9, -1

		if (nPos < -1) {
			if (IsWindow(this->m_EditHwnd))
				SetWindowText(this->m_EditHwnd, itemtext.to_chr());

			//SendMessage(this->m_EditHwnd, WM_SETTEXT,0, (LPARAM)itemtext.to_chr());
		}
		else {
			COMBOBOXEXITEM cbi;

			ZeroMemory(&cbi, sizeof(COMBOBOXEXITEM));

			cbi.mask = CBEIF_TEXT | CBEIF_INDENT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
			cbi.iIndent = indent;
			cbi.iImage = icon;
			cbi.iSelectedImage = state;
			cbi.iOverlay = overlay;
			cbi.pszText = const_cast<TCHAR *>(itemtext.to_chr());
			cbi.iItem = nPos;
			cbi.lParam = (LPARAM)new DCXCBITEM;

			//nPos = this->insertItem(&cbi);
			if (this->insertItem(&cbi) == -1)
			{
				delete reinterpret_cast<DCXCBITEM *>(cbi.lParam);
				throw Dcx::dcxException("Unable to add item.");
			}
			// Now update the horizontal scroller
			auto combo = (HWND)SendMessage(this->m_Hwnd, CBEM_GETCOMBOCONTROL, NULL, NULL);

			if (IsWindow(combo)) {
				// Get Font sizes (best way i can find atm, if you know something better then please let me know)

				//int nMaxStrlen = itemtext.len();
				//const int nHorizExtent = (int)SendMessage( combo, CB_GETHORIZONTALEXTENT, NULL, NULL );
				//
				//HDC hdc = GetDC( this->m_Hwnd );
				//TEXTMETRIC tm;
				//HFONT hFont = this->getFont();
				//
				//HFONT hOldFont = SelectFont(hdc, hFont);
				//
				//GetTextMetrics(hdc, &tm);
				//
				//SelectFont(hdc, hOldFont);
				//
				//ReleaseDC( this->m_Hwnd, hdc);
				//
				//// Multiply max str len by font average width + 1
				//nMaxStrlen *= (tm.tmAveCharWidth + tm.tmOverhang);
				//// Add 2 * chars as spacer.
				//nMaxStrlen += (tm.tmAveCharWidth * 2);
				//
				//if (nMaxStrlen > nHorizExtent)
				//	SendMessage(combo, CB_SETHORIZONTALEXTENT, nMaxStrlen, NULL);

				const auto nHorizExtent = (int)SendMessage(combo, CB_GETHORIZONTALEXTENT, NULL, NULL);

				auto hdc = GetDC(this->m_Hwnd);

				if (hdc != nullptr)
				{
					Auto(ReleaseDC(this->m_Hwnd, hdc));

					HFONT hFont = this->getFont(), hOldFont = nullptr;
					SIZE sz = { 0 };

					if (hFont != nullptr)
						hOldFont = SelectFont(hdc, hFont);

					if (GetTextExtentPoint32(hdc, itemtext.to_chr(), itemtext.len(), &sz))
					{
						if (sz.cx > nHorizExtent)
							SendMessage(combo, CB_SETHORIZONTALEXTENT, sz.cx, NULL);
					}

					if (hFont != nullptr)
						SelectFont(hdc, hOldFont);
				}
			}
		}
	}
	// xdid -A [NAME] [ID] [ROW] [+FLAGS] [INFO]
	// [NAME] [ID] -A [ROW] [+FLAGS] [INFO]
	else if (flags[TEXT('A')]) {
		if (numtok < 5)
			throw Dcx::dcxException("Insufficient parameters");

		auto nRow = input.getnexttok().to_int();	// tok 4

		// We're currently checking 1-based indexes.
		if ((nRow < 1) || (nRow > this->getCount()))
			throw Dcx::dcxException(TEXT("Invalid row index %."), nRow);

		// Convert to 0-based index.
		nRow--;

		COMBOBOXEXITEM cbei;
		ZeroMemory(&cbei, sizeof(COMBOBOXEXITEM));

		cbei.mask = CBEIF_LPARAM;
		cbei.iItem = nRow;

		// Couldn't retrieve info
		if (!this->getItem(&cbei))
			throw Dcx::dcxException("Unable to get item.");

		auto cbiDcx = reinterpret_cast<LPDCXCBITEM>(cbei.lParam);

		if (cbiDcx == nullptr)
			throw Dcx::dcxException("Unable to get Item Info");
		
		const XSwitchFlags xflags(input.getnexttok());	// tok 5
		const auto info(input.getlasttoks());		// tok 6, -1

		if (!xflags[TEXT('M')])
			throw Dcx::dcxException(TEXT("Unknown flags %"), input.gettok(5));
		
		cbiDcx->tsMark = info;
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('c')] && numtok > 3) {
		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem > -1)
			this->setCurSel(nItem);
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	//xdid -d -> [NAME] [ID] -d [N](,[N],[N1]-N2],...)
	//xdid -d -> [NAME] [ID] -d [N] [+flags] [match text]
	else if (flags[TEXT('d')] && numtok > 3) {
		const auto Ns(input.getnexttok());			// tok 4
		const XSwitchFlags xFlags(input.getnexttok());	// tok 5
		const auto nItems = this->getCount();

		if (xFlags[TEXT('+')])
		{
			// have flags, so its a match text delete
			const auto tsMatchText(input.getnexttok());
			auto SearchType = DcxSearchTypes::SEARCH_E;	// plain text exact match delete

			if (xFlags[TEXT('w')])
				SearchType = DcxSearchTypes::SEARCH_W;	// wildcard delete
			else if (xFlags[TEXT('r')])
				SearchType = DcxSearchTypes::SEARCH_R;	// regex delete

			for (auto nPos = Ns.to_int(); nPos < nItems; nPos++) {

				if (this->matchItemText(nPos, tsMatchText, SearchType))
					this->deleteItem(nPos--);		// NB: we do nPos-- here as a lines just been removed so we have to check the same nPos again
			}
		}
		else {
			for (auto itStart = Ns.begin(TSCOMMA), itEnd = Ns.end(); itStart != itEnd; ++itStart)
			{
				auto iStart = 0, iEnd = 0;
				const TString tsLine(*itStart);
				this->getItemRange(tsLine, nItems, &iStart, &iEnd);
				if ((iStart < 0) || (iEnd < 0) || (iStart >= nItems) || (iEnd >= nItems))
					throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

				for (auto nPos = iStart; nPos <= iEnd; nPos++)
					this->deleteItem(nPos);
			}
		}

		if (!this->getCount())
			this->redrawWindow();
	}
	// This is to avoid invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')]) {
		//this->resetContent();
	}
	// xdid -u [NAME] [ID] [SWITCH]
	else if (flags[TEXT('u')]) {
		this->setCurSel(-1);
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')] && numtok > 5) {
		const auto flag(input++);			// tok 4
		const auto index = input++.to_<int>();		// tok 5
		auto filename(input++);				// tok 6, -1

		auto himl = this->getImageList();

		if (himl == nullptr) {
			himl = this->createImageList();

			if (himl != nullptr)
				this->setImageList(himl);
		}

		if (himl != nullptr) {
			//auto icon = dcxLoadIcon(index, filename, false, flag);
			//
			//if (icon != nullptr) {
			//	ImageList_AddIcon(himl, icon);
			//	DestroyIcon(icon);
			//}

			Dcx::dcxIconResource icon(index, filename, false, flag);

			ImageList_AddIcon(himl, icon);
		}
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if (flags[TEXT('y')]) {
		ImageList_Destroy(this->getImageList());
		this->setImageList(nullptr);
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
* \brief blah
*
* blah
*/

HIMAGELIST DcxComboEx::getImageList(  ) const {

	return (HIMAGELIST) SendMessage( this->m_Hwnd, CBEM_GETIMAGELIST, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

void DcxComboEx::setImageList( HIMAGELIST himl ) {

	SendMessage( this->m_Hwnd, CBEM_SETIMAGELIST, (WPARAM) 0U, (LPARAM) himl );
}

/*!
* \brief blah
*
* blah
*/

HIMAGELIST DcxComboEx::createImageList( ) {

	return ImageList_Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );
}

/*!
* \brief blah
*
* blah
*/

bool DcxComboEx::matchItemText(const int nItem, const TString &search, const DcxSearchTypes &SearchType) const
{
	auto itemtext = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);

	COMBOBOXEXITEM cbi;
	ZeroMemory( &cbi, sizeof( COMBOBOXEXITEM ) );

	cbi.mask = CBEIF_TEXT;
	cbi.iItem = nItem;
	cbi.pszText = itemtext.get();
	cbi.cchTextMax = MIRC_BUFFER_SIZE_CCH;

	this->getItem( &cbi );

	switch (SearchType)
	{
	case DcxSearchTypes::SEARCH_R:
		return isRegexMatch(itemtext.get(), search.to_chr());
	case DcxSearchTypes::SEARCH_W:
		return TString(itemtext).iswm(search);
	case DcxSearchTypes::SEARCH_E:
		return (lstrcmp(search.to_chr(), itemtext.get()) == 0); // must be a zero check not a !
	}
	return false;
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::insertItem( const PCOMBOBOXEXITEM lpcCBItem ) {
	return SendMessage( this->m_Hwnd, CBEM_INSERTITEM, (WPARAM) 0U, (LPARAM) lpcCBItem );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::getItem( PCOMBOBOXEXITEM lpcCBItem ) const {
	return SendMessage( this->m_Hwnd, CBEM_GETITEM, (WPARAM) 0U, (LPARAM) lpcCBItem );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::getEditControl( ) const {
	return SendMessage( this->m_Hwnd, CBEM_GETEDITCONTROL, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::deleteItem( const int iIndex ) {
	return SendMessage( this->m_Hwnd, CBEM_DELETEITEM, (WPARAM) iIndex, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::setCurSel( const int iIndex ) {
	return SendMessage( this->m_Hwnd, CB_SETCURSEL, (WPARAM) iIndex, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::getCurSel( ) const {
	return SendMessage( this->m_Hwnd, CB_GETCURSEL, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::getLBText( const int iIndex, LPSTR lps ) {
	return SendMessage( this->m_Hwnd, CB_GETLBTEXT, (WPARAM) iIndex, (LPARAM) lps );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::resetContent( ) {
	return SendMessage( this->m_Hwnd, CB_RESETCONTENT, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::getCount( ) const {
	return SendMessage( this->m_Hwnd, CB_GETCOUNT, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::limitText( const int iLimit ) {
	return SendMessage( this->m_Hwnd, CB_LIMITTEXT, (WPARAM) iLimit, (LPARAM) 0U );
}

void DcxComboEx::getItemRange(const TString &tsItems, const int nItemCnt, int *iStart_range, int *iEnd_range)
{
	int iStart, iEnd;
	if (tsItems.numtok(TEXT('-')) == 2) {
		iStart = tsItems.getfirsttok(1, TEXT("-")).to_int() - 1;
		iEnd = tsItems.getnexttok(TEXT("-")).to_int() - 1;

		if (iEnd == -1)	// special case
			iEnd = nItemCnt - 1;
	}
	else {
		iEnd = tsItems.to_int() - 1;

		if (iEnd == -1)	// special case
			iStart = iEnd = nItemCnt - 1;
		else
			iStart = iEnd;
	}
	*iStart_range = iStart;
	*iEnd_range = iEnd;
}

const TString DcxComboEx::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(this->m_Hwnd);

	if (dcx_testflag(Styles, CBS_SIMPLE))
		styles.addtok(TEXT("simple"));
	if (dcx_testflag(Styles, CBS_DROPDOWNLIST)) 
		styles.addtok(TEXT("dropdown"));
	if (dcx_testflag(Styles, CBS_DROPDOWN)) 
		styles.addtok(TEXT("dropedit"));

	return styles;
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxComboEx::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch ( uMsg )
	{
	case WM_COMMAND: 
		switch ( HIWORD( wParam ) )
		{
		case CBN_DBLCLK:
			if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
				this->execAliasEx(TEXT("%s,%d,%d"), TEXT("dclick"), this->getUserID( ), this->getCurSel( ) + 1 );
			bParsed = TRUE;
			return TRUE;
			break;

		case CBN_SELENDOK:
			if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
				this->execAliasEx(TEXT("%s,%d,%d"), TEXT("sclick"), this->getUserID( ), this->getCurSel( ) + 1 );

			TCHAR itemtext[MIRC_BUFFER_SIZE_CCH];
			itemtext[0] = TEXT('\0');
			COMBOBOXEXITEM cbex;
			ZeroMemory(&cbex, sizeof(COMBOBOXEXITEM));
			cbex.mask = CBEIF_TEXT;
			cbex.pszText = itemtext;
			cbex.cchTextMax = MIRC_BUFFER_SIZE_CCH;
			cbex.iItem = this->getCurSel();
			this->getItem(&cbex);
			if (IsWindow(this->m_EditHwnd))
				SetWindowText(this->m_EditHwnd, itemtext);
			m_tsSelected = itemtext;

			bParsed = TRUE;
			return TRUE;
			break;
		case CBN_EDITCHANGE:
			if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT))
				this->execAliasEx(TEXT("%s,%d"), TEXT("edit"), this->getUserID( ) );

			bParsed = TRUE;
			return TRUE;
			break;
		} // switch
		bParsed = TRUE;
		break;
	case WM_NOTIFY:
		dcxlParam(LPNMHDR,nhmdr);

		switch (nhmdr->code) 
		{
		case CBEN_ENDEDIT:
			{
				dcxlParam(LPNMCBEENDEDIT,endedit);

				if (endedit == nullptr)
					break;

				if (endedit->iWhy == CBENF_RETURN) {
					if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT))
						this->execAliasEx(TEXT("%s,%d"), TEXT("return"), this->getUserID( ) );
					bParsed = TRUE;
				}
			}
			break;
		case CBEN_DELETEITEM:
			{
				dcxlParam(PNMCOMBOBOXEX,lpcb);

				if (lpcb == nullptr)
					break;

				auto lpdcxcbi = reinterpret_cast<LPDCXCBITEM>(lpcb->ceItem.lParam);
				lpcb->ceItem.lParam = NULL;

				delete lpdcxcbi;

				bParsed = TRUE; // message has been handled
			}
			break;
		}
		break;
	}
	return 0L;
}

LRESULT DcxComboEx::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {
	case WM_LBUTTONUP:
		{
			if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
				this->execAliasEx(TEXT("%s,%d"), TEXT("lbup"), this->getUserID( ) );
		}
		break;
	case WM_LBUTTONDBLCLK:
	case WM_CONTEXTMENU:
		break;

	case WM_MOUSEACTIVATE:
		{
			switch (HIWORD(lParam))
			{
			case WM_RBUTTONDOWN:
				{
					// NB: rclick doesnt change selection!
					if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
						this->execAliasEx(TEXT("%s,%d,%d"), TEXT("rclick"), this->getUserID( ), this->getCurSel( ) + 1 );
				}
				break;
			}
			//TODO: Add `ownmenu` setting or similar to stop default edit menu & replace with own.
			// this could be done with most if not all controls.
			//bParsed = TRUE;
			//return MA_ACTIVATE;
		}
		break;

	case WM_NCDESTROY:
		{
			delete this;
			bParsed = TRUE;
		}
		break;
	default:
		return this->CommonMessage( uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}

/*!
* \brief blah
*
* blah
*/

LRESULT CALLBACK DcxComboEx::ComboExEditProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {

	auto lpce = reinterpret_cast<LPDCXCOMBOEXEDIT>(GetWindowLongPtr(mHwnd, GWLP_USERDATA));
	if (lpce == nullptr)
		return DefWindowProc( mHwnd, uMsg, wParam, lParam );

	switch( uMsg ) {

		// This message added to allow the control to return a msg when 'return' is pressed.
		case WM_GETDLGCODE:
			return DLGC_WANTALLKEYS | CallWindowProc( lpce->OldProc, mHwnd, uMsg, wParam, lParam );
			break;
/*
		case WM_KEYDOWN:
		{
		if ( wParam == VK_RETURN ) {

		DcxControl * pthis = (DcxControl *) GetProp( lpce->cHwnd, TEXT("dcx_cthis") );

		if ( pthis != nullptr ) {
		pthis->callAliasEx( nullptr, TEXT("%s,%d"), TEXT("return"), pthis->getUserID( ) );
		}
		}
		}
		break;
		//case WM_NOTIFY:
		//	{
		//      LPNMHDR hdr = (LPNMHDR) lParam;
		//      if (!hdr)
		//        break;

		//      switch( hdr->code ) {
		//		case TTN_GETDISPINFO:
		//			{
		//         DcxControl * pthis = (DcxControl *) GetProp( lpce->cHwnd, TEXT("dcx_cthis") );
		//         if ( pthis != nullptr ) {
		//					LPNMTTDISPINFO di = (LPNMTTDISPINFO)lParam;
		//					di->lpszText = TEXT("test");
		//					di->hinst = nullptr;
		//				}
		//				return 0L;
		//			}
		//			break;
		//		case TTN_LINKCLICK:
		//			{
		//         DcxControl * pthis = (DcxControl *) GetProp( lpce->cHwnd, TEXT("dcx_cthis") );
		//         if ( pthis != nullptr ) {
		//					pthis->callAliasEx( nullptr, TEXT("%s,%d"), TEXT("tooltiplink"), pthis->getUserID( ) );
		//				}
		//				return 0L;
		//			}
		//			break;
		//		}
		//	}
		//	break;*/
		case LB_GETITEMRECT:
		{
			// This fixes the lockup on clicking the comboex editbox. (why?)
			// this is not a real fix, but a workaround.
			// NB: when we dont replace the editbox wndproc this crash doesnt happen.
			// mIRC sends this message to it's child windows, no idea why
			return LB_ERR;
		}
		break;
	}
	return CallWindowProc( lpce->OldProc, mHwnd, uMsg, wParam, lParam );
}
