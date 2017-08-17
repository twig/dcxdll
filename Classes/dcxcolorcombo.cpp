/*!
 * \file dcxcolorcombo.cpp
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
#include "Classes/dcxcolorcombo.h"
#include "Classes/dcxtrackbar.h"
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

DcxColorCombo::DcxColorCombo(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	m_Hwnd = CreateWindowEx(
		static_cast<DWORD>(ExStyles),
		DCX_COLORCOMBOCLASS,
		nullptr,
		WS_CHILD | static_cast<DWORD>(Styles),
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	this->registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxColorCombo::~DcxColorCombo() {

	this->resetContent();
	this->unregistreDefaultWindowProc();
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxColorCombo::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme)
{
	*Styles |= CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED;

	this->parseGeneralControlStyles(styles, Styles, ExStyles, bNoTheme);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxColorCombo::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
#if DCX_USE_HASHING
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case L"num"_hash:
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), getCount());
		break;
		// [NAME] [ID] [PROP] [N]
	case L"color"_hash:
	{
		if (input.numtok() < 4)
			throw Dcx::dcxException("Invalid number of argumnets");

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 || nItem >= getCount())
			throw Dcx::dcxException("Invalid Item");

		auto lpdcxcci = reinterpret_cast<LPDCXCCOMBOITEM>(getItemData(nItem));

		if (lpdcxcci == nullptr)
			throw Dcx::dcxException("Unable to get item data");

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), lpdcxcci->clrItem);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"sel"_hash:
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), static_cast<int>(getCurSel()) + 1);
		break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
		break;
	}
#else
	const auto numtok = input.numtok();

	const auto prop(input.getfirsttok(3));

	// [NAME] [ID] [PROP]
	if ( prop == TEXT("num") ) {

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getCount( ) );
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("color") && numtok > 3 ) {

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 || nItem >= this->getCount())
			throw Dcx::dcxException("Invalid Item");

		auto lpdcxcci = reinterpret_cast<LPDCXCCOMBOITEM>(this->getItemData( nItem ));

		if (lpdcxcci == nullptr)
			throw Dcx::dcxException("Unable to get item data");

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), lpdcxcci->clrItem );
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("sel")) {

		const auto nItem = static_cast<int>(this->getCurSel());

		// commented out to allow a zero result (CB_ERR + 1) to mean none selected.
		//if (nItem != CB_ERR ) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), nItem + 1);
		//}
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
void DcxColorCombo::parseCommandRequest( const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		this->resetContent();

	// xdid -a [NAME] [ID] [SWITCH] [N] [RGB]
	if (flags[TEXT('a')] && numtok > 4) {
		auto nItem = input.getnexttok().to_int() - 1;	// tok 4
		const auto clrItem = input.getnexttok().to_<COLORREF>();	// tok 5

		if (nItem >= this->getCount())
			nItem = -1;

		if (nItem > -2) {
			auto lpdcxcci = new DCXCCOMBOITEM;

			lpdcxcci->clrItem = clrItem;
			//lpmycci->itemtext = "";
			this->insertItem(nItem, (LPARAM)lpdcxcci);
		}
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('c')] && numtok > 3) {
		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if ((nItem < -1) || (nItem >= this->getCount()))
			throw Dcx::dcxException("Item out of range");
	
		this->setCurSel(nItem);
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('d')] && numtok > 3) {
		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if ((nItem < -1) || (nItem >= this->getCount()))
			throw Dcx::dcxException("Item out of range");
		
		this->deleteItem(nItem);
	}
	// xdid -m [NAME] [ID] [SWITCH]
	else if (flags[TEXT('m')]) {
		this->setmIRCPalette();
	}
	// xdid -o [NAME] [ID] [SWITCH] [N] [RGB]
	else if (flags[TEXT('o')] && numtok > 4) {
		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4
		const auto clrItem = input.getnexttok().to_<COLORREF>();	// tok 5

		if ((nItem < -1) || (nItem >= this->getCount()))
			throw Dcx::dcxException("Item out of range");

		auto lpdcxcci = reinterpret_cast<LPDCXCCOMBOITEM>(this->getItemData(nItem));

		if (lpdcxcci != nullptr)
			lpdcxcci->clrItem = clrItem;
	}
	// This is to avoid invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')]) {
		//this->resetContent();
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxColorCombo::setmIRCPalette( ) {

	static const TCHAR com[] = TEXT("$color(0) $color(1) $color(2) $color(3) $color(4) $color(5) $color(6) $color(7) $color(8) $color(9) $color(10) $color(11) $color(12) $color(13) $color(14) $color(15)");
	TString cols;
	//mIRCLinker::tsEval( cols, &com[0] );
	mIRCLinker::eval(cols, &com[0]);

	for (const auto &col: cols)
	{
		auto lpdcxcci = std::make_unique<DCXCCOMBOITEM>();

		lpdcxcci->clrItem = col.to_<COLORREF>();
		//lpmycci->itemtext = "";
		this->insertItem( -1, (LPARAM) lpdcxcci.release() );
	}
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::insertItem(  const int nPos, const LPARAM lParam ) {
  return SendMessage( m_Hwnd, CB_INSERTSTRING, static_cast<WPARAM>(nPos), lParam );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::getCount( ) const {
  return SendMessage( m_Hwnd, CB_GETCOUNT, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::setCurSel( const int nPos ) {
  return SendMessage( m_Hwnd, CB_SETCURSEL, static_cast<WPARAM>(nPos), (LPARAM) 0U );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::getItemData( const int nItem ) const {
  return SendMessage( m_Hwnd, CB_GETITEMDATA, static_cast<WPARAM>(nItem), (LPARAM) 0U );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::getCurSel( ) const {
  return SendMessage( m_Hwnd, CB_GETCURSEL, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::deleteItem( const int nItem ) {
  return SendMessage( m_Hwnd, CB_DELETESTRING, static_cast<WPARAM>(nItem), (LPARAM) 0U );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::resetContent( ) {
  return SendMessage( m_Hwnd, CB_RESETCONTENT, (WPARAM) 0U, (LPARAM) 0U );
}


/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxColorCombo::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg ) {
	case WM_COMMAND:
		{
			if ( HIWORD( wParam ) == CBN_SELENDOK ) {
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
					execAliasEx(TEXT("sclick,%u,%d"), getUserID( ), getCurSel( ) + 1 );
				bParsed = TRUE;
				return 0L;
			}
		}
		break;
	case WM_DELETEITEM:
		{
			dcxlParam(PDELETEITEMSTRUCT, delis);

			if (delis == nullptr)
				break;

			auto lpdcxcci = reinterpret_cast<LPDCXCCOMBOITEM>(delis->itemData);

			delete lpdcxcci;

			bParsed = TRUE;
			return TRUE;
		}
		break;

	case WM_DRAWITEM:
		{
			dcxlParam(LPDRAWITEMSTRUCT, lpdis);

			if (lpdis == nullptr || lpdis->itemID == -1)
				break;

			auto lpdcxcci = reinterpret_cast<LPDCXCCOMBOITEM>(lpdis->itemData);

			if (lpdcxcci == nullptr)
				break;

			auto hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
			Auto(DeletePen(hPen));

			const auto oldPen = SelectPen(lpdis->hDC, hPen);
			Auto(SelectPen(lpdis->hDC, oldPen));

			auto rcItem = lpdis->rcItem;

			// draw selection indicator
			if (dcx_testflag(lpdis->itemState, ODS_COMBOBOXEDIT))
				SetBkColor(lpdis->hDC, GetSysColor(COLOR_WINDOW));
			else if (dcx_testflag(lpdis->itemState, ODS_SELECTED))
				SetBkColor(lpdis->hDC, GetSysColor(COLOR_MENUHILIGHT));
			else
				SetBkColor(lpdis->hDC, GetSysColor(COLOR_WINDOW));
			ExtTextOut( lpdis->hDC, rcItem.left, rcItem.top, ETO_CLIPPED | ETO_OPAQUE, &rcItem, TEXT(""), NULL, nullptr );

			InflateRect( &rcItem, -4, -2 );

			SetBkColor( lpdis->hDC, lpdcxcci->clrItem );

			ExtTextOut( lpdis->hDC, rcItem.left, rcItem.top, ETO_CLIPPED | ETO_OPAQUE, &rcItem, TEXT(""), NULL, nullptr );

			MoveToEx( lpdis->hDC, rcItem.left, rcItem.top, nullptr );
			LineTo( lpdis->hDC, rcItem.right, rcItem.top );

			MoveToEx( lpdis->hDC, rcItem.right, rcItem.top, nullptr );
			LineTo( lpdis->hDC, rcItem.right, rcItem.bottom );

			MoveToEx( lpdis->hDC, rcItem.right, rcItem.bottom, nullptr );
			LineTo( lpdis->hDC, rcItem.left, rcItem.bottom );

			MoveToEx( lpdis->hDC, rcItem.left, rcItem.bottom, nullptr );
			LineTo( lpdis->hDC, rcItem.left, rcItem.top );

			bParsed = TRUE;
			return TRUE;
		}
		break;
	case WM_MEASUREITEM:
		{
			dcxlParam(LPMEASUREITEMSTRUCT, lpmis);

			if (lpmis == nullptr)
				break;

			lpmis->itemHeight = 16; 

			bParsed = TRUE;
			return TRUE;
		}
		break;
	}
	return 0L;
}

LRESULT DcxColorCombo::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {

	case WM_LBUTTONUP:
		{
			if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
				execAliasEx(TEXT("lbup,%u"), getUserID( ) );
		}
		break;
	case WM_DESTROY:
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
