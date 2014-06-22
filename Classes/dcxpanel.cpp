/*!
 * \file dcxpanel.cpp
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
#include "Classes/dcxpanel.h"
#include "Classes/dcxdialog.h"
#include "Classes/layout/layoutcellfixed.h"
#include "Classes/layout/layoutcellfill.h"
#include "Classes/layout/layoutcellpane.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxPanel::DcxPanel( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, const TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles | WS_EX_CONTROLPARENT, 
		DCX_PANELCLASS, 
		NULL,
		WS_CHILD | Styles, 
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL), 
		NULL);

	if (!IsWindow(this->m_Hwnd))
		throw TEXT("Unable To Create Window");

	if ( bNoTheme )
		Dcx::UXModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->m_pLayoutManager = new LayoutManager( this->m_Hwnd );

	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxPanel::~DcxPanel( ) {

	if ( this->m_pLayoutManager != NULL )
		delete this->m_pLayoutManager;

	this->unregistreDefaultWindowProc( );
}

void DcxPanel::toXml(TiXmlElement * xml) const
{
	__super::toXml(xml);
	this->m_pLayoutManager->getRoot()->toXml(xml);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxPanel::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme)
{
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

void DcxPanel::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const
{
	if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
		return;

	szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxPanel::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const UINT numtok = input.numtok( );

	// xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if ( flags[TEXT('c')] && numtok > 8 ) {

		const UINT ID = mIRC_ID_OFFSET + input.getnexttok( ).to_int( );	// tok 4

		if (this->m_pParentDialog->isIDValid(ID, true))
		{
			try {
				DcxControl * p_Control = DcxControl::controlFactory(this->m_pParentDialog,ID,input,5,CTLF_ALLOW_ALL,this->m_Hwnd);

				if ( p_Control != NULL ) {
					this->m_pParentDialog->addControl( p_Control );
					this->redrawWindow( );
				}
			}
			catch ( TCHAR *err ) {
				this->showErrorEx(NULL, TEXT("-c"), TEXT("Unable To Create Control %d (%s)"), ID - mIRC_ID_OFFSET, err);
			}
		}
		else
			this->showErrorEx(NULL, TEXT("-c"), TEXT("Control with ID \"%d\" already exists"), ID - mIRC_ID_OFFSET );
	}
	// xdid -d [NAME] [ID] [SWITCH] [ID]
	else if ( flags[TEXT('d')] && numtok > 3 ) {

		const UINT ID = mIRC_ID_OFFSET + input.getnexttok( ).to_int( );	// tok 4

		if (this->m_pParentDialog->isIDValid(ID))
		{
			DcxControl * p_Control = this->m_pParentDialog->getControlByID(ID);
			if (p_Control != NULL) {
				HWND cHwnd = p_Control->getHwnd();
				if (p_Control->getType() == TEXT("dialog") || p_Control->getType() == TEXT("window"))
					delete p_Control;
				else if (p_Control->getRefCount() == 0) {
					this->m_pParentDialog->deleteControl(p_Control); // remove from internal list!
					DestroyWindow(cHwnd);
				}
				else
					this->showErrorEx(NULL, TEXT("-d"), TEXT("Can't delete control with ID \"%d\" when it is inside it's own event (dialog %s)"), p_Control->getUserID(), this->m_pParentDialog->getName().to_chr());
			}
			else
				this->showErrorEx(NULL, TEXT("-d"), TEXT("Unable to get control with ID \"%d\" (dialog %s)"), p_Control->getUserID(), this->m_pParentDialog->getName().to_chr());
		}
		else
			this->showErrorEx(NULL, TEXT("-d"), TEXT("Unknown control with ID \"%d\" (dialog %s)"), ID - mIRC_ID_OFFSET, this->m_pParentDialog->getName( ).to_chr( ) );
	}
	/*
	//xdid -l [NAME] [ID] [SWITCH] [OPTIONS]

	[OPTIONS] :

	root [TAB]+flpiw [ID] [WEIGHT] [W] [H]
	add PATH[TAB]+flpiw [ID] [WEIGHT] [W] [H]
	space PATH[TAB]+ [L] [T] [R] [B]
	*/
	else if ( flags[TEXT('l')] && numtok > 3 ) {

		const TString tsCmd(input.getnexttok( ));	// tok 4

		if ( tsCmd == TEXT("update") ) {

			if ( this->m_pLayoutManager != NULL ) {

				RECT rc;
				GetClientRect( this->m_Hwnd, &rc );
				this->m_pLayoutManager->updateLayout( rc );
				this->redrawWindow();
			}
		}
		else if (tsCmd == TEXT("clear")) {
			delete this->m_pLayoutManager;
			this->m_pLayoutManager = new LayoutManager(this->m_Hwnd);
			//this->redrawWindow(); // dont redraw here, leave that for an `update` cmd
		}
		else if ( numtok > 8 ) {
			try {
				this->m_pLayoutManager->AddCell(input, 4);
			}
			catch (const TCHAR *errStr)
			{
				this->showError(NULL, TEXT("-l"), errStr);
			}
			catch (std::bad_alloc)
			{
				this->showError(NULL, TEXT("-l"), TEXT("Unable to allocate memory!"));
			}
			//const TString com(input.gettok(1, TSTAB).gettok(4).trim());
			//const TString path(input.getfirsttok(1, TSTAB).gettok(5, -1).trim());
			//const TString p2(input.getnexttok( TSTAB).trim());	// tok 2

			//const UINT iflags = this->parseLayoutFlags( p2.getfirsttok( 1 ) );
			//const UINT ID = p2.getnexttok( ).to_int( );		// tok 2
			//const UINT WGT = p2.getnexttok( ).to_int( );	// tok 3
			//const UINT W = p2.getnexttok( ).to_int( );		// tok 4
			//const UINT H = p2.getnexttok( ).to_int( );		// tok 5

			//if ( com ==  TEXT("root") || com == TEXT("cell") ) {

			//	HWND cHwnd = GetDlgItem( this->m_Hwnd, mIRC_ID_OFFSET + ID );

			//	LayoutCell * p_Cell = NULL;

			//	// LayoutCellPane
			//	if ( iflags & LAYOUTPANE ) {

			//		if ( iflags & LAYOUTHORZ )
			//			p_Cell = new LayoutCellPane( LayoutCellPane::HORZ );
			//		else
			//			p_Cell = new LayoutCellPane( LayoutCellPane::VERT );
			//	} // if ( flags & LAYOUTPANE )
			//	// LayoutFill Cell
			//	else if ( iflags & LAYOUTFILL ) {
			//		if ( iflags & LAYOUTID ) {
			//			if ( cHwnd != NULL && IsWindow( cHwnd ) )
			//				p_Cell = new LayoutCellFill( cHwnd );
			//			else {
			//				this->showErrorEx(NULL, TEXT("-l"), TEXT("Cell Fill -> Invalid ID : %d"), ID );
			//				return;
			//			}
			//		}
			//		else {
			//			p_Cell = new LayoutCellFill( );
			//		}
			//	} // else if ( flags & LAYOUTFILL )
			//	// LayoutCellFixed
			//	else if ( iflags & LAYOUTFIXED ) {

			//		LayoutCellFixed::FixedType type;

			//		if ( iflags & LAYOUTVERT && iflags & LAYOUTHORZ )
			//			type = LayoutCellFixed::BOTH;
			//		else if ( iflags & LAYOUTVERT )
			//			type = LayoutCellFixed::HEIGHT;
			//		else
			//			type = LayoutCellFixed::WIDTH;

			//		// Defined Rectangle
			//		if ( iflags & LAYOUTDIM ) {

			//			RECT rc;
			//			SetRect( &rc, 0, 0, W, H );

			//			if ( iflags & LAYOUTID ) {

			//				if ( cHwnd != NULL && IsWindow( cHwnd ) )
			//					p_Cell = new LayoutCellFixed( cHwnd, rc, type );
			//				else {
			//					this->showErrorEx(NULL, TEXT("-l"), TEXT("Cell Fixed -> Invalid ID : %d"), ID );
			//					return;
			//				}
			//			}
			//			else
			//				p_Cell = new LayoutCellFixed( rc, type );

			//		}
			//		// No defined Rectangle
			//		else {

			//			if ( iflags & LAYOUTID ) {

			//				if ( cHwnd != NULL && IsWindow( cHwnd ) )
			//					p_Cell = new LayoutCellFixed( cHwnd, type );
			//				else {
			//					this->showErrorEx(NULL, TEXT("-l"), TEXT("Cell Fixed -> Invalid ID : %d"), ID );
			//					return;
			//				}
			//			}
			//		} //else
			//	} // else if ( flags & LAYOUTFIXED )
			//	else {
			//		this->showError(NULL, TEXT("-l"), TEXT("Unknown Cell Type"));
			//		//DCXError(TEXT("/xdid -l"), TEXT("Unknown Cell Type") );
			//		return;
			//	}

			//	if ( com == TEXT("root") ) {

			//		if ( p_Cell != NULL )
			//			this->m_pLayoutManager->setRoot( p_Cell );

			//	} // if ( com == TEXT("root") )
			//	else if ( com == TEXT("cell") ) {

			//		if ( p_Cell != NULL ) {

			//			LayoutCell * p_GetCell;

			//			if ( path == TEXT("root") )
			//				p_GetCell = this->m_pLayoutManager->getRoot( );
			//			else
			//				p_GetCell = this->m_pLayoutManager->getCell( path );

			//			if ( p_GetCell == NULL ) {
			//				this->showErrorEx(NULL, TEXT("-l"), TEXT("Invalid item path: %s"), path.to_chr( ) );
			//				delete p_Cell;
			//				return;
			//			}

			//			if ( p_GetCell->getType( ) == LayoutCell::PANE ) {

			//				//LayoutCellPane * p_PaneCell = (LayoutCellPane *) p_GetCell;
			//				LayoutCellPane * p_PaneCell = reinterpret_cast<LayoutCellPane *>(p_GetCell);
			//				p_PaneCell->addChild(p_Cell, WGT);
			//			}
			//		}
			//	} // else if ( com == TEXT("cell") )
			//} // if ( com ==  TEXT("root") || com == TEXT("cell") )
			//else if ( com ==  TEXT("space") ) {

			//	LayoutCell * p_GetCell;

			//	if ( path == TEXT("root") )
			//		p_GetCell = this->m_pLayoutManager->getRoot( );
			//	else
			//		p_GetCell = this->m_pLayoutManager->getCell( path );

			//	if ( p_GetCell == NULL ) {
			//		this->showErrorEx(NULL, TEXT("-l"), TEXT("Invalid item path: %s"), path.to_chr( ) );
			//		return;
			//	}
			//	else {

			//		RECT rc;
			//		SetRect( &rc, ID, WGT, W, H );
			//		p_GetCell->setBorder( rc );
			//	}
			//} // else if ( com == TEXT("space") )
		} // if ( numtok > 7 )
	}
	// xdid -t [NAME] [ID] [SWITCH] [TEXT]
	else if (flags[TEXT('t')] && numtok > 3) {
		SetWindowText(this->m_Hwnd, input.gettok(4, -1).to_chr());
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

//const UINT DcxPanel::parseLayoutFlags( const TString & flags ) {
//
//	const XSwitchFlags xflags(flags);
//	UINT iFlags = 0;
//
//	// no +sign, missing params
//	if ( !xflags[TEXT('+')] ) 
//		return iFlags;
//
//	if ( xflags[TEXT('f')] )
//		iFlags |= LAYOUTFIXED;
//	if ( xflags[TEXT('h')] )
//		iFlags |= LAYOUTHORZ;
//	if ( xflags[TEXT('i')] )
//		iFlags |= LAYOUTID;
//	if ( xflags[TEXT('l')] )
//		iFlags |= LAYOUTFILL ;
//	if ( xflags[TEXT('p')] )
//		iFlags |= LAYOUTPANE;
//	if ( xflags[TEXT('v')] )
//		iFlags |= LAYOUTVERT;
//	if ( xflags[TEXT('w')] )
//		iFlags |= LAYOUTDIM;
//
//	return iFlags;
//}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxPanel::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxPanel::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
	switch( uMsg ) {

		case WM_NOTIFY : 
			{
				LPNMHDR hdr = (LPNMHDR) lParam;

				if (!hdr)
					break;

				if (IsWindow(hdr->hwndFrom)) {
					//DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,TEXT("dcx_cthis"));
					DcxControl *c_this = static_cast<DcxControl *>(GetProp(hdr->hwndFrom, TEXT("dcx_cthis")));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_HSCROLL:
		case WM_VSCROLL:
		case WM_COMMAND:
			{
				if (IsWindow((HWND) lParam)) {
					//DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,TEXT("dcx_cthis"));
					DcxControl *c_this = static_cast<DcxControl *>(GetProp((HWND)lParam, TEXT("dcx_cthis")));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_COMPAREITEM:
			{
				LPCOMPAREITEMSTRUCT idata = (LPCOMPAREITEMSTRUCT)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					//DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
					DcxControl *c_this = static_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DELETEITEM:
			{
				DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					//DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
					DcxControl *c_this = static_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_MEASUREITEM:
			{
				HWND cHwnd = GetDlgItem(this->m_Hwnd, wParam);
				if (IsWindow(cHwnd)) {
					//DcxControl *c_this = (DcxControl *) GetProp(cHwnd,TEXT("dcx_cthis"));
					DcxControl *c_this = static_cast<DcxControl *>(GetProp(cHwnd, TEXT("dcx_cthis")));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DRAWITEM:
			{
				DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					//DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
					DcxControl *c_this = static_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_SIZE:
			{
					//bParsed = TRUE;
					//lRes = CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, wParam, lParam);

				HWND bars = NULL;

				while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_REBARCTRLCLASS, NULL ) ) != NULL ) {
					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_STATUSBARCLASS, NULL ) ) != NULL ) {
					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				//while ((bars = FindWindowEx(this->m_Hwnd, bars, DCX_PANELCLASS, NULL)) != NULL) {
				//	SendMessage(bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0);
				//}

				while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_TOOLBARCLASS, NULL ) ) != NULL ) {
					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_SIZE)
					this->execAliasEx(TEXT("%s,%d"), TEXT("sizing"), this->getUserID( ) );

				//if (this->m_pLayoutManager != NULL) {
				//	RECT rc;
				//	SetRect( &rc, 0, 0, LOWORD( lParam ), HIWORD( lParam ) );
				//	if (this->m_pLayoutManager->updateLayout( rc ))
				//		this->redrawWindow( );
				//}
			}
			break;
		case WM_WINDOWPOSCHANGING:
			{
				if (lParam != NULL) {
					WINDOWPOS * wp = (WINDOWPOS *) lParam;
					if (this->m_pLayoutManager != NULL) {
						RECT rc;
						SetRect( &rc, 0, 0, wp->cx, wp->cy );
						//if (this->m_pLayoutManager->updateLayout( rc ))
						//	this->redrawWindow( );
						this->m_pLayoutManager->updateLayout( rc );
					}
				}
			}
			break;

		//case WM_EXITSIZEMOVE:
		//	{
		//		this->redrawWindow();
		//	}
		//	break;

		case WM_ERASEBKGND:
			{
				// fill background.
				if (this->isExStyle(WS_EX_TRANSPARENT))
					this->DrawParentsBackground((HDC)wParam);
				else // normal bkg
					DcxControl::DrawCtrlBackground((HDC)wParam, this);
				bParsed = TRUE;
				return TRUE;
			}
			break;

		case WM_PRINTCLIENT:
			{
				HDC hdc = (HDC)wParam;

				bParsed = TRUE;

				// Setup alpha blend if any.
				LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

				{ // simply fill with bkg
					if (this->isExStyle(WS_EX_TRANSPARENT)) {
						if (!this->m_bAlphaBlend)
							this->DrawParentsBackground(hdc);
					}
					else
						DcxControl::DrawCtrlBackground(hdc,this);
				}

				this->FinishAlphaBlend(ai);
			}
			break;

		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc;

				hdc = BeginPaint( this->m_Hwnd, &ps );

				bParsed = TRUE;

				// Setup alpha blend if any.
				LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

				{ // simply fill with bkg
					if (this->isExStyle(WS_EX_TRANSPARENT)) {
						if (!this->m_bAlphaBlend)
							this->DrawParentsBackground(hdc);
					}
					else
						DcxControl::DrawCtrlBackground(hdc,this);
				}

				this->FinishAlphaBlend(ai);

				EndPaint( this->m_Hwnd, &ps );
			}
			break;

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
