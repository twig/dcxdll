/*!
 * \file dcxdialog.cpp
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
// CWndShadow ...
// Copyright (c) 2006 Perry Zhu, All Rights Reserved.
//
// mailto:perry@live.com
//
//
// This source file may be redistributed unmodified by any means PROVIDING 
// it is NOT sold for profit without the authors expressed written 
// consent, and providing that this notice and the author's name and all 
// copyright notices remain intact. This software is by no means to be 
// included as part of any third party components library, or as part any
// development solution that offers MFC extensions that are sold for profit. 
// 
// If the source code is used in any commercial applications then a statement 
// along the lines of:
// 
// "Portions Copyright (c) 2006 Perry Zhu" must be included in the "Startup 
// Banner", "About Box" or "Printed Documentation". This software is provided 
// "as is" without express or implied warranty. Use it at your own risk! The 
// author accepts no liability for any damage/loss of business that this 
// product may cause.
//
// This code has been modified for use with DCX.
// ... CWndShadow
/*
	Vista Style Dialogs
	Taken from http://www.codeproject.com/useritems/VisitaLookingDialog.asp
	By Jerry Wang
*/

#include "defines.h"
#include "Dcx.h"
#include "Classes/dcxdialog.h"
#include "Classes/dcxdialogcollection.h"
#include "Classes/dcxcontrol.h"
#include "Classes/dcxtoolbar.h"
#include "Classes/dcxrichedit.h"
#include "Classes/mirc/dcxlist.h"
#include "Classes/layout/layoutcellfixed.h"
#include "Classes/layout/layoutcellfill.h"
#include "Classes/layout/layoutcellpane.h"

#include <math.h>

#include "Classes/xpopup\xpopupmenumanager.h"



//extern mIRCDLL mIRCLink;
bool DcxDialog::m_bIsMenuBar = false;
bool DcxDialog::m_bIsSysMenu = false;



/*!
 * \brief Constructor
 *
 * \param mHwnd Dialog Window Handle
 * \param tsName Dialog Name
 * \param tsAliasName Dialog Callback alias Name
 */


DcxDialog::DcxDialog(const HWND mHwnd, const TString &tsName, const TString &tsAliasName)
: DcxWindow(mHwnd, 0)
, m_uStyleBg(DBS_BKGNORMAL)
, m_tsName(tsName)
, m_tsAliasName(tsAliasName)
, m_hBackBrush(NULL)
, m_bitmapBg(NULL)
, m_colTransparentBg(RGB(255,0,255))
, m_hCursor(NULL)
, m_bCursorFromFile(FALSE)
, m_MouseID(0)
, m_FocusID(0)
, m_ToolTipHWND(NULL)
, m_iRefCount(0)
, m_bDoDrag(false)
, m_dEventMask(DCX_EVENT_ALL)
, m_bTracking(FALSE)
, m_bDoGhostDrag(255)
, m_bGhosted(false)
, m_zLayerCurrent(0)
, m_popup(NULL)
, m_hOldWindowProc(NULL)
, m_hFakeHwnd(NULL)
, m_iAlphaLevel(255)
, m_bHaveKeyColour(false)
, m_cKeyColour(CLR_NONE)
, m_bVistaStyle(false)
, m_pVistaBits(NULL)
, m_hVistaBitmap(NULL)
, m_hVistaHDC(NULL)
, m_bVerboseErrors(true)
//#ifdef DCX_USE_GDIPLUS
//, m_pImage(NULL)
//#endif
{
	this->addStyle(WS_CLIPCHILDREN);

	//this->addExStyle(WS_EX_TRANSPARENT); // WS_EX_TRANSPARENT|WS_EX_LAYERED gives a window u can click through to the win behind.

	this->m_hOldWindowProc = SubclassWindow(this->m_Hwnd, DcxDialog::WindowProc);

	this->m_pLayoutManager = new LayoutManager(this->m_Hwnd);

	this->m_Shadow.hWin = NULL;
	this->m_Shadow.Status = 0;
	SetRectEmpty(&this->m_GlassOffsets);

	SetProp(this->m_Hwnd, TEXT("dcx_this"), (HANDLE) this);

	DragAcceptFiles(this->m_Hwnd, TRUE);
}

/*!
 * \brief Destructor
 *
 * blah
 */

DcxDialog::~DcxDialog() {
	if (this->m_pLayoutManager != NULL)
		delete this->m_pLayoutManager;

	if (this->m_popup != NULL)
		delete this->m_popup;

	PreloadData();
	this->RemoveShadow();

	this->RemoveVistaStyle();

	if (this->m_bCursorFromFile && this->m_hCursor != NULL)
		DestroyCursor(this->m_hCursor);

	RemoveProp(this->m_Hwnd, TEXT("dcx_this"));
}

/*!
 * \brief blah
 *
 * blah
 */
const TString &DcxDialog::getName() const {
	return this->m_tsName;
}

/*!
 * \brief blah
 *
 * blah
 */

const TString &DcxDialog::getAliasName() const {
  return this->m_tsAliasName;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::addControl(DcxControl *p_Control) {
	this->m_vpControls.push_back(p_Control);
	p_Control->redrawWindow();
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::deleteControl(DcxControl *p_Control) {
	VectorOfControlPtrs::iterator itStart = this->m_vpControls.begin();
	VectorOfControlPtrs::iterator itEnd = this->m_vpControls.end();

	while (itStart != itEnd) {
		if (*itStart == p_Control && *itStart != NULL) {
			this->m_vpControls.erase(itStart);
			return;
		}

		itStart++;
	}
}

void DcxDialog::deleteAllControls() {
	/*
  TCHAR error[500];
  wsprintf( error, TEXT("%d Controls"), this->m_vpControls.size( ) );
  mIRCError( error );

  VectorOfControlPtrs::iterator it;
  DcxControl * p_Control;

  
  while ( ( p_Control = this->m_vpControls.pop_back( ) ) != NULL ) {

    wsprintf( error, TEXT("ID %d %d"), p_Control->getUserID( ), this->m_vpControls.size( ) );
    mIRCError( error );

    if ( p_Control != NULL ) {

      if ( p_Control->getType( ) == TEXT("window") || p_Control->getType( ) == TEXT("dialog") )
        delete p_Control;
      else
        DestroyWindow( p_Control->getHwnd( ) );
    }
  }
  this->m_vpControls.clear( );
  

  wsprintf( error, TEXT("%d Controls"), this->m_vpControls.size( ) );
  mIRCError( error );
  */
}

/*!
 * \brief blah
 *
 * The ID must include the mIRC_ID_OFFSET of 6000.
 */
DcxControl *DcxDialog::getControlByID(const UINT ID) {
	VectorOfControlPtrs::iterator itStart = this->m_vpControls.begin();
	VectorOfControlPtrs::iterator itEnd = this->m_vpControls.end();

	while (itStart != itEnd) {
		if ((*itStart)->getID() == ID)
			return *itStart;

		itStart++;
	}

	return NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

DcxControl *DcxDialog::getControlByHWND(const HWND mHwnd) {
	VectorOfControlPtrs::iterator itStart = this->m_vpControls.begin();
	VectorOfControlPtrs::iterator itEnd = this->m_vpControls.end();

	while (itStart != itEnd) {
		if ((*itStart)->getHwnd() == mHwnd)
			return *itStart;

		itStart++;
	}

	return NULL;
}

// clears existing image and icon data and sets pointers to null
void DcxDialog::PreloadData() {
	if (this->m_bitmapBg != NULL) {
		DeleteBitmap(this->m_bitmapBg);
		this->m_bitmapBg = NULL;
	}
//#ifdef DCX_USE_GDIPLUS
//	if (this->m_pImage != NULL) {
//		delete this->m_pImage;
//		this->m_pImage = NULL;
//	}
//#endif
}

/*!
 * \brief blah
 *
 * blah
 */
void DcxDialog::parseCommandRequestEX(const TCHAR *szFormat, ...) {
	TString msg((UINT)2048);
	va_list args;
	va_start(args, szFormat);
#if UNICODE
	vswprintf(msg.to_chr(), 2028, szFormat, args);
#else
	vsprintf(msg.to_chr(), szFormat, args);
#endif
	this->parseCommandRequest(msg);
	va_end(args);
}
void DcxDialog::parseComControlRequestEX(const int id, const TCHAR *szFormat, ...) {
	DcxControl * p_Control = this->getControlByID((UINT) id + mIRC_ID_OFFSET);
	if (p_Control != NULL) {
		TString msg;
		va_list args;
		va_start(args, szFormat);
		msg.tvprintf(szFormat, &args);
		va_end(args);
		p_Control->parseCommandRequest(msg);
	}
}



void DcxDialog::parseCommandRequest( TString &input) {
	XSwitchFlags flags(input.gettok(2));
	int numtok = input.numtok( );

	// xdialog -a [NAME] [SWITCH] [+FLAGS] [DURATION]
	if (flags[TEXT('a')] && numtok > 3) {
		AnimateWindow(this->m_Hwnd,
			input.gettok( 4 ).to_int(), 
			getAnimateStyles(input.gettok( 3 )));

		if (IsWindowVisible(this->m_Hwnd))
			this->redrawWindow();
	}
	// xdialog -b [NAME] [SWITCH] [+FLAGS]
	else if (flags[TEXT('b')] && numtok > 2) {
		this->removeStyle(WS_BORDER | WS_DLGFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX |
			WS_SYSMENU | WS_SIZEBOX | WS_CAPTION);

		if (Dcx::XPPlusModule.isUseable())
			this->removeExStyle(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_CONTEXTHELP |
				WS_EX_TOOLWINDOW | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE | WS_EX_COMPOSITED | WS_EX_LAYERED);
		else
			this->removeExStyle(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_CONTEXTHELP |
				WS_EX_TOOLWINDOW | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE | WS_EX_LAYERED);

		this->RemoveVistaStyle();

		LONG Styles = 0, ExStyles = 0;

		this->parseBorderStyles(input.gettok( 3 ), &Styles, &ExStyles);
		this->addStyle(Styles);
		this->addExStyle(ExStyles);

		if (input.gettok( 3 ).find(TEXT('v'),0)) {
			// Vista Style Dialog
			this->CreateVistaStyle();
		}
		SetWindowPos(this->m_Hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		InvalidateRect(this->m_Hwnd, NULL, TRUE);
		SendMessage(this->m_Hwnd, WM_NCPAINT, (WPARAM) 1, (LPARAM) 0);
	}
	// xdialog -c [NAME] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	else if (flags[TEXT('c')] && numtok > 7) {
		UINT ID = mIRC_ID_OFFSET + input.gettok( 3 ).to_int();

		if ((IsWindow(GetDlgItem(this->m_Hwnd, ID)) == FALSE) && 
			(ID > mIRC_ID_OFFSET - 1) && (this->getControlByID(ID) == NULL))
		{
			try {
				DcxControl *p_Control = DcxControl::controlFactory(this, ID, input,4);

				if (p_Control != NULL)
					this->addControl(p_Control);
			}
			catch ( TCHAR *err ) {
				this->showErrorEx(NULL, TEXT("-c"), TEXT("Unable To Create Control %d (%s)"), ID - mIRC_ID_OFFSET, err);
			}
		}
		else
			this->showErrorEx(NULL,TEXT("-c"), TEXT("Control with ID \"%d\" already exists"), ID - mIRC_ID_OFFSET);
	}
	// xdialog -d [NAME] [SWITCH] [ID]
	else if (flags[TEXT('d')] && numtok > 2) {
		/*
		if ( input.gettok( 3 ) == TEXT("*") ) { 

		this->deleteAllControls( );
		}
		else {
		*/

		UINT ID = mIRC_ID_OFFSET + input.gettok( 3 ).to_int();
		DcxControl * p_Control;

		if (IsWindow(GetDlgItem(this->m_Hwnd, ID)) && 
			(ID > mIRC_ID_OFFSET - 1) && (p_Control = this->getControlByID(ID)) != NULL)
		{
			HWND cHwnd = p_Control->getHwnd();
			UINT cid = p_Control->getUserID();

			// fix up focus id
			if (cid == this->m_FocusID) {
				HWND h = GetNextDlgTabItem(this->m_Hwnd, cHwnd, FALSE);

				// control exists and is not the last one
				if (h && (h != cHwnd))
					this->setFocusControl(GetDlgCtrlID(h) - mIRC_ID_OFFSET);
				else
					this->setFocusControl(0);
			}

			if ((p_Control->getType() == TEXT("dialog")) || (p_Control->getType() == TEXT("window")))
				delete p_Control;
			else if (p_Control->getRefCount() == 0) {
				this->deleteControl(p_Control); // remove control from internal list!
				DestroyWindow(cHwnd);
			}
			else
				this->showErrorEx(NULL, TEXT("-d"), TEXT("Can't delete control with ID \"%d\" when it is inside it's own event (dialog %s)"), p_Control->getUserID(), this->m_tsName.to_chr());
		}
		// unknown control
		else
			this->showErrorEx(NULL, TEXT("-d"), TEXT("Unknown control with ID \"%d\" (dialog %s)"), ID - mIRC_ID_OFFSET, this->m_tsName.to_chr());
	}
	// xdialog -f [NAME] [SWITCH] [+FLAGS] [COUNT] [TIMEOUT]
	else if (flags[TEXT('f')] && numtok > 4) {
		UINT iFlags = this->parseFlashFlags(input.gettok( 3 ));
		INT iCount = input.gettok( 4 ).to_int();
		DWORD dwTimeout = (DWORD)input.gettok( 5 ).to_num();
		FLASHWINFO fli;

		ZeroMemory(&fli, sizeof(FLASHWINFO));
		fli.cbSize = sizeof(FLASHWINFO);
		fli.dwFlags = iFlags;
		fli.hwnd = this->m_Hwnd;
		fli.uCount = iCount;
		fli.dwTimeout = dwTimeout;

		FlashWindowEx(& fli);
	}
	// xdialog -g [NAME] [SWITCH] [+FLAGS] [COLOR|FILENAME]
	else if (flags[TEXT('g')] && numtok > 3) {
		this->m_uStyleBg = this->parseBkgFlags(input.gettok( 3 ));

		if (this->m_uStyleBg & DBS_BKGCOLOR) {
			COLORREF clrColor = (COLORREF)input.gettok( 4 ).to_num();

			if (this->m_hBackBrush != NULL) {
				DeleteObject(this->m_hBackBrush);
				this->m_hBackBrush = NULL;
			}

			if (clrColor != -1)
				this->m_hBackBrush = CreateSolidBrush(clrColor);
		}
		else if (this->m_uStyleBg & DBS_BKGBITMAP) {
			PreloadData();

			TString filename(input.gettok(4, -1).trim());

			if (filename != TEXT("none")) {
				this->m_bitmapBg = dcxLoadBitmap(this->m_bitmapBg, filename);
//#ifdef DCX_USE_GDIPLUS
//				if (mIRCLink.m_bUseGDIPlus)
//					this->LoadGDIPlusImage(filename);
//#endif
			}
		}

		//InvalidateRect(this->m_Hwnd, NULL, TRUE);
		this->redrawWindow();
	}
	// xdialog -j [NAME] [SWITCH] (ID)
	else if (flags[TEXT('j')]) {
		if (numtok > 2) {
			UINT id = mIRC_ID_OFFSET + input.gettok( 3 ).to_int();
			DcxControl * p_Control;

			if ((id > mIRC_ID_OFFSET - 1) && IsWindow(GetDlgItem(this->m_Hwnd, id)) && ((p_Control = this->getControlByID(id)) != NULL))
			{
				p_Control->redrawWindow();
			}
			else
				this->showErrorEx(NULL, TEXT("-j"), TEXT("Could not find control %d"), id - mIRC_ID_OFFSET);

			return;
		}
		else
			this->redrawWindow();
	}
	// This commands presence here doesnt make sense, why is a dialog command trying to alter the main mirc window?
	// TODO: doesnt work. xdialog -k [NAME] [SWITCH] [STATE]
	//else if (flags[TEXT('k')] && numtok > 2) {
	//	bool state = (input.gettok(3).to_int() > 0);
	//	LONG styles = GetWindowLong(Dcx::mIRC.getHWND(), GWL_EXSTYLE);

	//	if (state)
	//		SetWindowLong(Dcx::mIRC.getHWND(), GWL_EXSTYLE, styles | WS_EX_APPWINDOW);
	//	else
	//		SetWindowLong(Dcx::mIRC.getHWND(), GWL_EXSTYLE, styles & ~WS_EX_APPWINDOW);
	//}
	/*
	//xdialog -l [NAME] [SWITCH] [OPTIONS]

	[OPTIONS] :

	root [TAB]+flpiw [ID] [WEIGHT] [W] [H]
	add PATH[TAB]+flpiw [ID] [WEIGHT] [W] [H]
	space PATH[TAB]+ [L] [T] [R] [B]
	*/
	else if (flags[TEXT('l')] && numtok > 2) {
		if (input.gettok( 3 ) == TEXT("update")) {
			if (this->m_pLayoutManager != NULL) {
				RECT rc;

				GetClientRect(this->m_Hwnd, &rc);
				if (this->updateLayout(rc))
					this->redrawWindow();
			}
		}
		else if (input.gettok( 3 ) == TEXT("clear")) {
			if (this->m_pLayoutManager != NULL)
				delete this->m_pLayoutManager;
			this->m_pLayoutManager = new LayoutManager(this->m_Hwnd);
			//this->redrawWindow(); // dont redraw here, leave that for an `update` cmd
		}
		else if (numtok > 7) {
			TString com(input.gettok(1, TSTAB).gettok(3).trim());
			TString path(input.gettok(1, TSTAB).gettok(4, -1).trim());
			TString p2(input.gettok(2, TSTAB).trim());

			UINT lflags = this->parseLayoutFlags(p2.gettok( 1 ));
			UINT ID = p2.gettok( 2 ).to_int();
			UINT WGT = p2.gettok( 3 ).to_int();
			UINT W = p2.gettok( 4 ).to_int();
			UINT H = p2.gettok( 5 ).to_int();

			if (com == TEXT("root") || com == TEXT("cell")) {
				HWND cHwnd = GetDlgItem(this->m_Hwnd, mIRC_ID_OFFSET + ID);
				LayoutCell * p_Cell = NULL;

				// LayoutCellPane
				if (lflags & LAYOUTPANE) {
					if (lflags & LAYOUTHORZ)
						p_Cell = new LayoutCellPane(LayoutCellPane::HORZ);
					else
						p_Cell = new LayoutCellPane(LayoutCellPane::VERT);
				} // if ( lflags & LAYOUTPANE )
				// LayoutFill Cell
				else if (lflags & LAYOUTFILL) {
					if (lflags & LAYOUTID) {
						if (cHwnd != NULL && IsWindow(cHwnd))
							p_Cell = new LayoutCellFill(cHwnd);
						else {
							this->showErrorEx(NULL, TEXT("-l"), TEXT("Cell Fill -> Invalid ID : %d"), ID);
							return;
						}
					}
					else {
						p_Cell = new LayoutCellFill();
					}
				} // else if ( lflags & LAYOUTFILL )
				// LayoutCellFixed
				else if (lflags & LAYOUTFIXED) {
					LayoutCellFixed::FixedType type;

					if (lflags & LAYOUTVERT && lflags & LAYOUTHORZ)
						type = LayoutCellFixed::BOTH;
					else if (lflags & LAYOUTVERT)
						type = LayoutCellFixed::HEIGHT;
					else
						type = LayoutCellFixed::WIDTH;

					// Defined Rectangle
					if (lflags & LAYOUTDIM) {
						RECT rc;
						SetRect(&rc, 0, 0, W, H);

						if (lflags & LAYOUTID) {
							if (cHwnd != NULL && IsWindow(cHwnd))
								p_Cell = new LayoutCellFixed(cHwnd, rc, type);
							else {
								this->showErrorEx(NULL, TEXT("-l"), TEXT("Cell Fixed -> Invalid ID : %d"), ID);
								return;
							}
						}
						else
							p_Cell = new LayoutCellFixed(rc, type);
					}
					// No defined Rectangle
					else {
						if (lflags & LAYOUTID) {
							if (cHwnd != NULL && IsWindow(cHwnd))
								p_Cell = new LayoutCellFixed(cHwnd, type);
							else {
								this->showErrorEx(NULL, TEXT("-l"), TEXT("Cell Fixed -> Invalid ID : %d"), ID);
								return;
							}
						}
					} //else
				} // else if ( lflags & LAYOUTFIXED )
				else {
					this->showError(NULL, TEXT("-l"), TEXT("Unknown Cell Type"));
					return;
				}

				if (com == TEXT("root")) {
					if (p_Cell != NULL)
						this->m_pLayoutManager->setRoot(p_Cell);
				} // if ( com == TEXT("root") )
				else if (com == TEXT("cell")) {
					if (p_Cell != NULL) {
						LayoutCell * p_GetCell;

						if (path == TEXT("root"))
							p_GetCell = this->m_pLayoutManager->getRoot();
						else
							p_GetCell = this->m_pLayoutManager->getCell(path);

						if (p_GetCell == NULL) {
							this->showErrorEx(NULL, TEXT("-l"), TEXT("Invalid item path: %s"), path.to_chr());
							return;
						}

						if (p_GetCell->getType() == LayoutCell::PANE) {
							LayoutCellPane *p_PaneCell = (LayoutCellPane*) p_GetCell;
							p_PaneCell->addChild(p_Cell, WGT);
						}
					}
				} // else if ( com == TEXT("cell") )
			} // if ( com ==  TEXT("root") || com == TEXT("cell") )
			else if (com ==  TEXT("space")) {
				LayoutCell * p_GetCell;

				if (path == TEXT("root"))
					p_GetCell = this->m_pLayoutManager->getRoot();
				else
					p_GetCell = this->m_pLayoutManager->getCell(path);

				if (p_GetCell != NULL) {
					RECT rc;

					SetRect(&rc, ID, WGT, W, H);
					p_GetCell->setBorder(rc);
				}
			} // else if ( com == TEXT("space") )
		} // if ( numtok > 7 )
	}
	// xdialog -q [NAME] [SWITCH] [+FLAGS] [CURSOR|FILENAME]
	else if (flags[TEXT('q')] && numtok > 3) {
		//if (this->m_bCursorFromFile) {
		//	DeleteObject(this->m_hCursor);
		//	this->m_hCursor = NULL;
		//	this->m_bCursorFromFile = FALSE;
		//}
		//else
		//	this->m_hCursor = NULL;

		//UINT iFlags = this->parseCursorFlags(input.gettok( 3 ));

		//if (iFlags & DCCS_FROMRESSOURCE)
		//	this->m_hCursor = LoadCursor(NULL, this->parseCursorType(input.gettok( 4 )));
		//else if (iFlags & DCCS_FROMFILE) {
		//	this->m_hCursor = LoadCursorFromFile(input.gettok(4, -1).to_chr());
		//	this->m_bCursorFromFile = TRUE;
		//}
		UINT iFlags = this->parseCursorFlags( input.gettok( 3 ) );
		HCURSOR hCursor = NULL;
		if ( this->m_bCursorFromFile )
			hCursor = this->m_hCursor;
		this->m_hCursor = NULL;
		this->m_bCursorFromFile = FALSE;
		if ( iFlags & DCCS_FROMRESSOURCE )
			this->m_hCursor = LoadCursor( NULL, this->parseCursorType( input.gettok( 4 ) ) );
		else if ( iFlags & DCCS_FROMFILE ) {
			TString filename(input.gettok( 4, -1 ));
			if (IsFile(filename)) {
				this->m_hCursor = (HCURSOR)LoadImage(NULL, filename.to_chr(), IMAGE_CURSOR, 0,0, LR_DEFAULTSIZE|LR_LOADFROMFILE );
				this->m_bCursorFromFile = TRUE;
			}
			else
				this->showErrorEx(NULL,TEXT("-q"), TEXT("Unable to Access File: %s"), filename.to_chr());
		}
		if (this->m_hCursor == NULL)
			this->showError(NULL, TEXT("-q"), TEXT("Unable to Load Cursor"));
		if (hCursor != NULL) {
			if (GetCursor() == hCursor) {
				if (this->m_hCursor != NULL)
					SetCursor(this->m_hCursor);
				else
					SetCursor(LoadCursor(NULL,IDC_ARROW));
			}
			DestroyCursor( hCursor );
		}
	}
	// xdialog -x [NAME]
	else if (flags[TEXT('x')]) {
		if (this->getRefCount() == 0) {
			//DestroyWindow(this->m_Hwnd);
			//SendMessage(this->m_Hwnd,WM_CLOSE,NULL,NULL); // this allows the dialogs WndProc to EndDialog() if needed.
			TCHAR ret[32];
			Dcx::mIRC.evalex(ret, 32, TEXT("$dialog(%s).modal"), this->m_tsName.to_chr());
			if (lstrcmp(ret, TEXT("$true")) == 0) // Modal Dialog
				SendMessage(this->m_Hwnd,WM_CLOSE,NULL,NULL); // this allows the dialogs WndProc to EndDialog() if needed.
			else // Modeless Dialog
				DestroyWindow(this->m_Hwnd);
		}
		else
			Dcx::mIRC.execex(TEXT("/.timer -m 1 0 xdialog -x %s"), this->getName().to_chr());
	}
	// xdialog -h [NAME]
	else if (flags[TEXT('h')]) {
		ShowWindow(this->m_Hwnd, SW_HIDE);
	}
	// xdialog -m [NAME]
	else if (flags[TEXT('m')]) {
		ShowWindow(this->m_Hwnd, SW_MAXIMIZE);
	}
	// xdialog -n [NAME]
	else if (flags[TEXT('n')]) {
		ShowWindow(this->m_Hwnd, SW_MINIMIZE);
	}
	// xdialog -r [NAME]
	else if (flags[TEXT('r')]) {
		ShowWindow(this->m_Hwnd, SW_RESTORE);
	}
	// xdialog -s [NAME]
	else if (flags[TEXT('s')]) {
		ShowWindow(this->m_Hwnd, SW_SHOW);
	}
	// xdialog -t [NAME] [SWITCH] [TYPE] [TYPE ARGS]
	else if (flags[TEXT('t')] && numtok > 2) {
		// Alpha transparency
		if (input.gettok( 3 ) == TEXT("alpha")) {
			if (input.gettok( 4 ) == TEXT("none")) {
				this->m_iAlphaLevel = 255;
				if (!this->m_bVistaStyle) {
					if (GetWindowExStyle(this->m_Hwnd) & WS_EX_LAYERED) {
						RemStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
						AddStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
						if (this->m_bHaveKeyColour) // reapply keycolour if any.
							SetLayeredWindowAttributes(this->m_Hwnd, this->m_cKeyColour, 0, LWA_COLORKEY);
					}
				}
			}
			else {
				BYTE alpha = (BYTE)input.gettok( 4 ).to_int();

				if (alpha > 255)
					alpha = 255;
				//else if (alpha < 0) // can only be >= 0
				//	alpha = 0;

				this->m_iAlphaLevel = alpha;
				if (!this->m_bVistaStyle) {
					// Set WS_EX_LAYERED on this window
					AddStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);

					// Make this window x% alpha
					SetLayeredWindowAttributes(this->m_Hwnd, 0, this->m_iAlphaLevel, LWA_ALPHA);
				}
			}
		}
		// Transparent color
		else if (input.gettok( 3 ) == TEXT("transparentcolor")) {
			if (input.gettok( 4 ) == TEXT("none")) {
				this->m_cKeyColour = CLR_NONE;
				this->m_bHaveKeyColour = false;
				if (!this->m_bVistaStyle) {
					if (GetWindowExStyle(this->m_Hwnd) & WS_EX_LAYERED) {
						RemStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
						AddStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
						if (this->m_iAlphaLevel != 255) // reapply alpha if any.
							SetLayeredWindowAttributes(this->m_Hwnd, 0, this->m_iAlphaLevel, LWA_ALPHA);
					}
				}
			}
			else {
				this->m_cKeyColour = (COLORREF)input.gettok( 4 ).to_int();
				this->m_bHaveKeyColour = true;
				if (!this->m_bVistaStyle) {
					// Set WS_EX_LAYERED on this window
					AddStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);

					// Make colour transparent
					SetLayeredWindowAttributes(this->m_Hwnd, this->m_cKeyColour, 0, LWA_COLORKEY);
				}
			}
		}
		// Background color
		else if (input.gettok( 3 ) == TEXT("bgcolor")) {
			this->m_colTransparentBg = input.gettok( 4 ).to_int();
		}
		// TODO: not going to document this, have no way to redrawing the window.
		// http://www.codeproject.com/KB/vb/ClickThroughWindows.aspx
		// NB: may not be compatible with vista style.
		// Click-through
		else if (input.gettok(3) == TEXT("clickthrough")) {
			if (input.gettok(4) == TEXT("none")) {
				if (this->isExStyle(WS_EX_LAYERED|WS_EX_TRANSPARENT))
					RemStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
				// re-apply any alpha or keycolour.
				if (((this->m_iAlphaLevel != 255) || (this->m_bHaveKeyColour)) && (!this->m_bVistaStyle)) {
					AddStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
					if (this->m_iAlphaLevel != 255) // reapply alpha if any.
						SetLayeredWindowAttributes(this->m_Hwnd, 0, this->m_iAlphaLevel, LWA_ALPHA);
					if (this->m_bHaveKeyColour) // reapply keycolour if any.
						SetLayeredWindowAttributes(this->m_Hwnd, this->m_cKeyColour, 0, LWA_COLORKEY);
				}
			}
			else
				AddStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
		}
		else {
			this->showError(NULL, TEXT("-t"), TEXT("Unknown Switch"));
			return;
		}
		this->redrawWindow();
	}
	// xdialog -T [NAME] [SWITCH] [FLAGS] [STYLES]
	else if (flags[TEXT('T')] && numtok > 2) {
		if (IsWindow(this->m_ToolTipHWND)) {
			this->showError(NULL, TEXT("-T"), TEXT("Tooltip already exists. Cannot recreate"));
			return;
		}

		UINT styles = parseTooltipFlags(input.gettok( 3 ));

		// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/tooltip/styles.asp
		this->m_ToolTipHWND = CreateWindowEx(WS_EX_TOPMOST,
			TOOLTIPS_CLASS, NULL,
			styles,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			this->m_Hwnd,
			NULL, GetModuleHandle(NULL), NULL);
		if (IsWindow(this->m_ToolTipHWND)) { // MUST set a limit before $crlf will give multiline tips.
			SendMessage(this->m_ToolTipHWND,TTM_SETMAXTIPWIDTH,0,400); // 400 seems a good limit for now, we could also add an option to set this.
			//if (input.gettok( 3 ).find(TEXT('T'),0)) {
			//	AddStyles(this->m_ToolTipHWND,GWL_EXSTYLE,WS_EX_LAYERED);
			//	SetLayeredWindowAttributesUx(this->m_ToolTipHWND, 0, 192, LWA_ALPHA);
			//}
		}
	}
	// xdialog -w [NAME] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')] && numtok > 4) {
		TString tsFlags(input.gettok( 3 ));
		int index = input.gettok( 4 ).to_int();
		TString filename(input.gettok(5, -1).trim());

		ChangeHwndIcon(this->m_Hwnd,tsFlags,index,filename);
	}
	// xdialog -z [NAME] [SWITCH] [+FLAGS] [N]
	else if (flags[TEXT('z')] && numtok > 3) {
		TString flag(input.gettok( 3 ));
		int n = input.gettok( 4 ).to_int();
		DcxControl* ctrl = NULL;

		// invalid input for [N]
		if (n <= 0) {
			this->showError(NULL,TEXT("-z"), TEXT("Invalid Parameters"));
			return;
		}

		// adding control ID to list
		if (flag[1] == TEXT('a'))
		{
			// check if the ID is already in the list
			VectorOfInts::iterator itStart = this->m_vZLayers.begin();
			VectorOfInts::iterator itEnd = this->m_vZLayers.end();

			// add mIRC offset since getControlByID() needs it
			n += mIRC_ID_OFFSET;

			while (itStart != itEnd) {
				if (*itStart == n) {
					this->showErrorEx(NULL,TEXT("-z"),TEXT("control %d already in list"), n);
					return;
				}

				itStart++;
			}

			// if the specified control exists on the dialog, hide it
			ctrl = getControlByID(n);

			if (ctrl) {
				ShowWindow(ctrl->getHwnd(), SW_HIDE);
				RECT rc;
				GetClientRect(this->getHwnd(), &rc);
				if (this->updateLayout(rc))
					this->redrawWindow();
			}

			// append the item to the end of the list
			this->m_vZLayers.push_back(n);
		}
		// position to match CID [CID]
		else if (flag[1] == TEXT('p')) {
			// add mIRC offset since getControlByID() needs it
			n += mIRC_ID_OFFSET;

			// get the control which will be used to retrieve the target position
			ctrl = getControlByID(n);

			// target control not found
			if (!ctrl) {
				this->showError(NULL,TEXT("-z"),TEXT("No such control"));
				return;
			}

			// variables used to move control
			VectorOfInts::iterator itStart = this->m_vZLayers.begin();
			VectorOfInts::iterator itEnd = this->m_vZLayers.end();
			RECT r;

			// figure out coordinates of control
			GetWindowRect(ctrl->getHwnd(), &r);
			MapWindowRect(NULL, GetParent(ctrl->getHwnd()), &r);

			// for each control in the internal list
			while (itStart != itEnd) {
				// ignore target control
				if (*itStart != n) {
					// get control to be moved
					ctrl = getControlByID(*itStart);

					// if it exists, move it
					if (ctrl)
						MoveWindow(ctrl->getHwnd(), r.left, r.top, r.right - r.left, r.bottom - r.top, FALSE);
				}

				itStart++;
			}
		}
		// show index [N]
		else if (flag[1] == TEXT('s')) {
			// minus since indexes are zero-based
			n--;

			// if the index is out of bounds
			if (n >= (int) this->m_vZLayers.size()) {
				this->showError(NULL,TEXT("-z"),TEXT("Index array out of bounds"));
				return;
			}

			this->execAliasEx(TEXT("%s,%d,%d"), TEXT("zlayershow"), n +1, this->m_vZLayers[n] - mIRC_ID_OFFSET);

			// hide the previous control
			ctrl = getControlByID(this->m_vZLayers[this->m_zLayerCurrent]);

			if (ctrl)
				ShowWindow(ctrl->getHwnd(), SW_HIDE);

			// set the new index to the currently selected index
			this->m_zLayerCurrent = n;
			ctrl = getControlByID(this->m_vZLayers[n]);

			// if the selected control exists, show control
			if (ctrl) { 
				ShowWindow(ctrl->getHwnd(), SW_SHOW);
				RECT rc;
				GetClientRect(this->getHwnd(), &rc);
				if (this->updateLayout(rc))
					this->redrawWindow();
			}
			else
				this->showError(NULL,TEXT("-z"),TEXT("Invalid Control ID"));
		}

		return;
	}
	// xdialog -P [NAME] [SWITCH] ([+FLAGS] (FLAG OPTIONS))
	else if (flags[TEXT('P')]) {
		HMENU menu = NULL;

		// create the menu
		if (this->m_popup == NULL) {
			menu = GetMenu(this->m_Hwnd);

			if (menu != NULL)
				this->m_popup = new XPopupMenu(TEXT("dialog"), menu);
			else
				this->showError(NULL,TEXT("-P"),TEXT("Menu Does Not Exist"));
		}
		if (this->m_popup != NULL) {
			TString menuargs;
			menuargs.tsprintf(TEXT("dialog %s"), input.gettok( 3, -1).to_chr());
			Dcx::XPopups.parseCommand(menuargs, this->m_popup);
		}
	}
	// xdialog -R [NAME] [SWITCH] [FLAG] [ARGS]
	else if (flags[TEXT('R')] && numtok > 2) {
		TString flag(input.gettok( 3 ));

		if ((flag.len() < 2) || (flag[0] != TEXT('+'))) {
			this->showError(NULL, TEXT("-R"), TEXT("Invalid Flag"));
			return;
		}

		RECT rc;
		dcxGetWindowRect(this->m_Hwnd,&rc);

		HRGN m_Region = NULL;
		int RegionMode = 0;
		bool noRegion = false;

		if (flag.find(TEXT('o'),0))
			RegionMode = RGN_OR;
		else if (flag.find(TEXT('a'),0))
			RegionMode = RGN_AND;
		else if (flag.find(TEXT('i'),0))
			RegionMode = RGN_DIFF;
		else if (flag.find(TEXT('x'),0))
			RegionMode = RGN_XOR;

		// image file - [COLOR] [FILE]
		if (flag.find(TEXT('f'),0))
		{
			if (numtok < 5) {
				this->showError(NULL, TEXT("-R +f"), TEXT("Invalid arguments"));
				return;
			}

			PreloadData();
			//SetWindowRgn(this->m_Hwnd,NULL,TRUE);
			this->m_colTransparentBg = (COLORREF)input.gettok( 4 ).to_num();
			//this->m_uStyleBg = DBS_BKGBITMAP|DBS_BKGSTRETCH|DBS_BKGCENTER;
			this->m_uStyleBg = DBS_BKGBITMAP;
			TString filename(input.gettok(5,-1));
			this->m_bitmapBg = dcxLoadBitmap(this->m_bitmapBg,filename);

			if (this->m_bitmapBg != NULL)
				m_Region = BitmapRegion(this->m_bitmapBg,this->m_colTransparentBg,TRUE);
			else
				this->showError(NULL, TEXT("-R +f"), TEXT("Unable To Load Image file."));
		}
		else if (flag.find(TEXT('r'),0)) // rounded rect - radius args (optional)
		{
			int radius;

			if (numtok > 3)
				radius = input.gettok( 4 ).to_int();
			else
				radius = 20;

			m_Region = CreateRoundRectRgn(0,0,rc.right - rc.left,rc.bottom - rc.top, radius, radius);
		}
		else if (flag.find(TEXT('c'),0)) // circle - radius arg (optional)
		{
			if (numtok > 3) {
				int radius = input.gettok( 4 ).to_int();
				if (radius < 1) radius = 100; // handle cases where arg isnt a number or is a negative.
				int cx = ((rc.right - rc.left)/2);
				int cy = ((rc.bottom - rc.top)/2);
				m_Region = CreateEllipticRgn(cx-radius,cy-radius,cx+radius,cy+radius);
			}
			else
				m_Region = CreateEllipticRgn(0,0,rc.right - rc.left,rc.bottom - rc.top);
		}
		else if (flag.find(TEXT('p'),0)) // polygon
		{
			// u need at least 3 points for a shape
			if (numtok < 6) {
				this->showError(NULL, TEXT("-R +p"), TEXT("Invalid arguments"));
				return;
			}

			TString strPoints(input.gettok(4, -1));
			TString strPoint;
			int tPoints = strPoints.numtok( );

			if (tPoints < 1) {
				this->showError(NULL, TEXT("-R +p"), TEXT("Invalid Points"));
				return;
			}

			int cnt = 1;
			POINT *pnts = new POINT[tPoints];

			while (cnt <= tPoints) {
				strPoint = strPoints.gettok( cnt );
				pnts[cnt-1].x = (LONG)strPoint.gettok(1, TSCOMMA).to_num();
				pnts[cnt-1].y = (LONG)strPoint.gettok(2, TSCOMMA).to_num();
				cnt++;
			}

			m_Region = CreatePolygonRgn(pnts,tPoints,WINDING);

			delete [] pnts;
		}
		else if (flag.find(TEXT('d'),0)) // drag - <1|0>
		{
			noRegion = true;
			if ((BOOL)input.gettok( 4 ).to_int())
				this->m_bDoDrag = true;
			else
				this->m_bDoDrag = false;
		}
		else if (flag.find(TEXT('g'),0)) // ghost drag - <0-255>
		{
			BYTE alpha = (BYTE)input.gettok( 4 ).to_int();
			if (/*(alpha >= 0) &&*/ (alpha <= 255)) { // unsigned int will ALWAYS be >= 0
				noRegion = true;
				this->m_bDoGhostDrag = alpha;
			}
			else {
				this->showError(NULL,TEXT("-R +g"), TEXT("Alpha Out Of Range"));
				return;
			}
		}
		else if (flag.find(TEXT('s'),0)) // shadow - <colour> <sharpness> <darkness> <size> <xoffset> <yoffset>
		{
			if (numtok == 9) {
				noRegion = true;
				COLORREF s_clr = (COLORREF)input.gettok( 4 ).to_num();
				int s_sharp = input.gettok( 5 ).to_int();
				int s_dark = input.gettok( 6 ).to_int();
				int s_size = input.gettok( 7 ).to_int();
				int s_x = input.gettok( 8 ).to_int();
				int s_y = input.gettok( 9 ).to_int();
				this->AddShadow();
				this->SetShadowColor(s_clr);
				this->SetShadowSharpness(s_sharp);
				this->SetShadowDarkness(s_dark);
				this->SetShadowSize(s_size);
				this->SetShadowPosition(s_x,s_y);
			}
			else {
				noRegion = true;
				this->RemoveShadow();
			}
		}
		else if (flag.find(TEXT('n'),0)) { // none, no args
			noRegion = true;
			SetWindowRgn(this->m_Hwnd,NULL,TRUE);
		}
		else
			this->showError(NULL, TEXT("-R"), TEXT("Invalid Flag"));

		if (!noRegion) {
			if (m_Region != NULL) {
				if (RegionMode != 0) {
					HRGN wrgn = CreateRectRgn(0,0,0,0);
					if (wrgn != NULL) {
						if (GetWindowRgn(this->m_Hwnd,wrgn) != ERROR)
							CombineRgn(m_Region,m_Region,wrgn,RegionMode);
						DeleteObject(wrgn);
					}
				}
				SetWindowRgn(this->m_Hwnd,m_Region,TRUE);
			}
			else
				this->showError(NULL, TEXT("-R"), TEXT("Unable to create region."));
		}
		this->redrawWindow();
	}
	// xdialog -E [NAME] [SWITCH] [+flags] [-flags]
	else if (flags[TEXT('E')] && numtok > 3) {
		//this->m_dEventMask = (DWORD)input.gettok( 3 ).to_num();
		DWORD mask = this->m_dEventMask;
		TString p_flags(input.gettok( 3 ));
		TString n_flags(input.gettok( 4 ));

		if ((p_flags[0] != TEXT('+')) || (n_flags[0] != TEXT('-'))) {
			this->showError(NULL,TEXT("-E"), TEXT("Invalid Flag"));
			return;
		}
		if (p_flags.find(TEXT('c'),0))
			mask |= DCX_EVENT_CLICK;
		if (p_flags.find(TEXT('d'),0))
			mask |= DCX_EVENT_DRAG;
		if (p_flags.find(TEXT('e'),0))
			mask |= DCX_EVENT_EDIT;
		if (p_flags.find(TEXT('f'),0))
			mask |= DCX_EVENT_FOCUS;
		if (p_flags.find(TEXT('h'),0))
			mask |= DCX_EVENT_HELP;
		if (p_flags.find(TEXT('m'),0))
			mask |= DCX_EVENT_MOUSE;
		if (p_flags.find(TEXT('s'),0))
			mask |= DCX_EVENT_SIZE;
		if (p_flags.find(TEXT('t'),0))
			mask |= DCX_EVENT_THEME;
		if (p_flags.find(TEXT('C'),0))
			mask |= DCX_EVENT_CLOSE;
		if (p_flags.find(TEXT('M'),0))
			mask |= DCX_EVENT_MOVE;

		if (n_flags.find(TEXT('c'),0))
			mask &= ~DCX_EVENT_CLICK;
		if (n_flags.find(TEXT('d'),0))
			mask &= ~DCX_EVENT_DRAG;
		if (n_flags.find(TEXT('e'),0))
			mask &= ~DCX_EVENT_EDIT;
		if (n_flags.find(TEXT('f'),0))
			mask &= ~DCX_EVENT_FOCUS;
		if (n_flags.find(TEXT('h'),0))
			mask &= ~DCX_EVENT_HELP;
		if (n_flags.find(TEXT('m'),0))
			mask &= ~DCX_EVENT_MOUSE;
		if (n_flags.find(TEXT('s'),0))
			mask &= ~DCX_EVENT_SIZE;
		if (n_flags.find(TEXT('t'),0))
			mask &= ~DCX_EVENT_THEME;
		if (n_flags.find(TEXT('C'),0))
			mask &= ~DCX_EVENT_CLOSE;
		if (n_flags.find(TEXT('M'),0))
			mask &= ~DCX_EVENT_MOVE;

		this->m_dEventMask = mask;
	}
	// xdialog -V [NAME] [SWITCH] [left] [right] [top] [bottom]
	else if (flags[TEXT('V')] && numtok > 5) {
		this->m_GlassOffsets.left = input.gettok( 3 ).to_int();
		this->m_GlassOffsets.right = input.gettok( 4 ).to_int();
		this->m_GlassOffsets.top = input.gettok( 5 ).to_int();
		this->m_GlassOffsets.bottom = input.gettok( 6 ).to_int();

		if (Dcx::VistaModule.isUseable()) {
			MARGINS margins = {this->m_GlassOffsets.left,this->m_GlassOffsets.right,this->m_GlassOffsets.top,this->m_GlassOffsets.bottom};
			Dcx::VistaModule.dcxDwmExtendFrameIntoClientArea(this->m_Hwnd, &margins);
		}
		this->redrawWindow();
	}
	// xdialog -U [NAME] [SWITCH]
	else if (flags[TEXT('U')]) {
		SetFocus(NULL);
	}
	// xdialog -S [NAME] [SWITCH] [X Y W H]
	else if (flags[TEXT('S')] && (numtok > 5)) {
		int x = input.gettok( 3 ).to_int( );
		int y = input.gettok( 4 ).to_int( );
		int w = input.gettok( 5 ).to_int( );
		int h = input.gettok( 6 ).to_int( );

		RECT rcWindow, rcClient;
		UINT iFlags = SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOOWNERZORDER;

		GetClientRect(this->m_Hwnd, &rcClient);

		dcxGetWindowRect(this->m_Hwnd, &rcWindow);

		// Convert windows screen position to its position within it's parent.
		if (this->isStyle(WS_CHILD))
			MapWindowRect(NULL, GetParent(this->m_Hwnd), &rcWindow);

		// if x & y are -1, not moving. NB: This still allows -2 etc.. positioning. (window positioned offscreen)
		if ((x == -1) && (y == -1))
			iFlags |= SWP_NOMOVE;

		// if w or h are < 0, no sizing. NB: no negative sizes allowed.
		if ((w < 0) && (h < 0))
			iFlags |= SWP_NOSIZE;

		// this handles the case where either x or y is -1 but the other isn't.
		if (x == -1)
			x = rcWindow.left;
		if (y == -1)
			y = rcWindow.top;

		// This handles the case where either w or h are < 0 but the other isn't.
		if (w < 0)
			w = (rcWindow.right - rcWindow.left);
		if (h < 0)
			h = (rcWindow.bottom - rcWindow.top);

		// Calculate the actual sizes without the window border
		// http://windows-programming.suite101.com/article.cfm/client_area_size_with_movewindow
		POINT ptDiff;
		ptDiff.x = (rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left);
		ptDiff.y = (rcWindow.bottom - rcWindow.top) - (rcClient.bottom - rcClient.top);

		SetWindowPos( this->m_Hwnd, NULL, x, y, w + ptDiff.x, h + ptDiff.y, iFlags );
	}
	// invalid command
	else
		this->showError(NULL, input.gettok(2).to_chr(), TEXT("Invalid Command"));
}

/*!
 * \brief blah
 *
 * blah
 */


void DcxDialog::parseBorderStyles(const TString &flags, LONG *Styles, LONG *ExStyles) {
	INT i = 1, len = flags.len();

	// no +sign, missing params
	if (flags[0] != TEXT('+')) 
		return;

	while (i < len) {
		if (flags[i] == TEXT('b'))
			*Styles |= WS_BORDER;
		else if (flags[i] == TEXT('c'))
			*ExStyles |= WS_EX_CLIENTEDGE;
		else if (flags[i] == TEXT('d'))
			*Styles |= WS_DLGFRAME ;
		else if (flags[i] == TEXT('f'))
			*ExStyles |= WS_EX_DLGMODALFRAME;
		else if (flags[i] == TEXT('h'))
			*ExStyles |= WS_EX_CONTEXTHELP;
		else if (flags[i] == TEXT('m'))
			*Styles |= WS_MAXIMIZEBOX;
		else if (flags[i] == TEXT('n'))
			*Styles |= WS_MINIMIZEBOX;
		else if (flags[i] == TEXT('o'))
			*ExStyles |= WS_EX_TOOLWINDOW;
		else if (flags[i] == TEXT('s'))
			*ExStyles |= WS_EX_STATICEDGE;
		else if (flags[i] == TEXT('t'))
			*Styles |= WS_CAPTION;
		else if (flags[i] == TEXT('w'))
			*ExStyles |= WS_EX_WINDOWEDGE;
		else if (flags[i] == TEXT('y'))
			*Styles |= WS_SYSMENU;
		else if (flags[i] == TEXT('z'))
			*Styles |= WS_SIZEBOX;
		else if (flags[i] == TEXT('x') && Dcx::XPPlusModule.isUseable())
			*ExStyles |= WS_EX_COMPOSITED;
		//	WS_EX_COMPOSITED style causes problems for listview control & maybe others, but when it works it looks really cool :)
		//	this improves transparency etc.. on xp+ only, looking into how this affects us.
		else if (flags[i] == TEXT('v'))
			*ExStyles |= WS_EX_LAYERED;
		++i;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

DWORD DcxDialog::getAnimateStyles( const TString &flags) {
	DWORD Styles = 0;
	int i = 1, len = flags.len();

	if (flags[0] != TEXT('+'))
		return 0;

	while (i < len) {
		switch (flags[i])
		{
		case TEXT('s'):
			Styles |= AW_SLIDE;
			break;
		case TEXT('h'):
			Styles |= AW_HIDE;
			break;
		case TEXT('a'):
			Styles |= AW_ACTIVATE;
			break;
		case TEXT('b'):
			Styles |= AW_BLEND;
			break;
		case TEXT('v'):
			Styles |= AW_VER_POSITIVE;
			break;
		case TEXT('u'):
			Styles |= AW_VER_NEGATIVE;
			break;
		case TEXT('c'):
			Styles |= AW_CENTER;
			break;
		case TEXT('o'):
			Styles |= AW_HOR_POSITIVE;
			break;
		case TEXT('n'):
			Styles |= AW_HOR_NEGATIVE;
		default:
			break;
		}

		i++;
	}

	return Styles;
}

/*!
 * \brief blah
 *
 * blah
 */
UINT DcxDialog::parseLayoutFlags(const TString &flags) {
	INT i = 1, len = flags.len();
	UINT iFlags = 0;

	// no +sign, missing params
	if (flags[0] != TEXT('+')) 
		return iFlags;

	while (i < len) {
		if (flags[i] == TEXT('f'))
			iFlags |= LAYOUTFIXED;
		else if (flags[i] == TEXT('h'))
			iFlags |= LAYOUTHORZ;
		else if (flags[i] == TEXT('i'))
			iFlags |= LAYOUTID;
		else if (flags[i] == TEXT('l'))
			iFlags |= LAYOUTFILL ;
		else if (flags[i] == TEXT('p'))
			iFlags |= LAYOUTPANE;
		else if (flags[i] == TEXT('v'))
			iFlags |= LAYOUTVERT;
		else if (flags[i] == TEXT('w'))
			iFlags |= LAYOUTDIM;

		++i;
	}

	return iFlags;
}

UINT DcxDialog::parseTooltipFlags(const TString &flags) {
	int i = 1;
	int len = flags.len();
	UINT iFlags = 0;

	// no +sign, missing params
	if (flags[0] != TEXT('+')) 
		return iFlags;

	while (i < len) {
		if (flags[i] == TEXT('a'))
			iFlags |= TTS_ALWAYSTIP;
		else if (flags[i] == TEXT('b'))
			iFlags |= TTS_BALLOON;
		else if (flags[i] == TEXT('c')) // no idea what this does or if it works at all
			iFlags |= TTS_CLOSE;
		else if (flags[i] == TEXT('f'))
			iFlags |= TTS_NOFADE;
		else if (flags[i] == TEXT('p'))
			iFlags |= TTS_NOPREFIX;
		else if (flags[i] == TEXT('s'))
			iFlags |= TTS_NOANIMATE;
#if DCX_USE_WINSDK && WINVER >= 0x600
		else if (flags[i] == TEXT('t'))
			iFlags |= TTS_USEVISUALSTYLE;
#endif

		++i;
	}

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxDialog::parseBkgFlags(const TString &flags) {
	INT i = 1, len = flags.len(), iFlags = 0;

	// no +sign, missing params
	if (flags[0] != TEXT('+')) 
		return iFlags;

	while (i < len) {
		if (flags[i] == TEXT('b'))
			iFlags |= DBS_BKGCOLOR;
		else if (flags[i] == TEXT('i'))
			iFlags |= DBS_BKGBITMAP;

		else if (flags[i] == TEXT('n'))
			iFlags |= DBS_BKGNORMAL;
		else if (flags[i] == TEXT('t'))
			iFlags |= DBS_BKGTILE;
		else if (flags[i] == TEXT('s'))
			iFlags |= DBS_BKGSTRETCH;

		else if (flags[i] == TEXT('c'))
			iFlags |= DBS_BKGCENTER;
		else if (flags[i] == TEXT('v'))
			iFlags |= DBS_BKGVCENTER;
		else if (flags[i] == TEXT('r'))
			iFlags |= DBS_BKGRIGHT;
		else if (flags[i] == TEXT('o'))
			iFlags |= DBS_BKGBOTTOM;

		i++;
	}

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxDialog::parseFlashFlags(const TString &flags) {
	INT i = 1, len = flags.len(), iFlags = 0;

	// no +sign, missing params
	if (flags[0] != TEXT('+')) 
		return iFlags;

	while (i < len) {
		if (flags[i] == TEXT('a'))
			iFlags |= FLASHW_ALL;
		else if (flags[i] == TEXT('c'))
			iFlags |= FLASHW_CAPTION;
		else if (flags[i] == TEXT('f'))
			iFlags |= FLASHW_TIMERNOFG;
		else if (flags[i] == TEXT('r'))
			iFlags |= FLASHW_TRAY;
		else if (flags[i] == TEXT('s'))
			iFlags |= FLASHW_STOP;
		else if (flags[i] == TEXT('t'))
			iFlags |= FLASHW_TIMER;

		++i;
	}

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxDialog::parseCursorFlags(const TString &flags) {
	INT i = 1, len = flags.len();
	UINT iFlags = 0;

	// no +sign, missing params
	if (flags[0] != TEXT('+')) 
		return iFlags;

	while (i < len) {
		if (flags[i] == TEXT('f'))
			iFlags |= DCCS_FROMFILE;
		else if (flags[i] == TEXT('r'))
			iFlags |= DCCS_FROMRESSOURCE;

		++i;
	}

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

PTCHAR DcxDialog::parseCursorType(const TString &cursor) {
	if (cursor == TEXT("appstarting"))
		return IDC_APPSTARTING;
	else if (cursor == TEXT("arrow"))
		return IDC_ARROW;
	else if (cursor == TEXT("cross"))
		return IDC_CROSS;
	else if (cursor == TEXT("hand"))
		return IDC_HAND;
	else if (cursor == TEXT("help"))
		return IDC_HELP;
	else if (cursor == TEXT("ibeam"))
		return IDC_IBEAM;
	else if (cursor == TEXT("no"))
		return IDC_NO;
	else if (cursor == TEXT("sizeall"))
		return IDC_SIZEALL;
	else if (cursor == TEXT("sizenesw"))
		return IDC_SIZENESW;
	else if (cursor == TEXT("sizens"))
		return IDC_SIZENS;
	else if (cursor == TEXT("sizenwse"))
		return IDC_SIZENWSE;
	else if (cursor == TEXT("sizewe"))
		return IDC_SIZEWE;
	else if (cursor == TEXT("uparrow"))
		return IDC_UPARROW;
	else if (cursor == TEXT("wait"))
		return IDC_WAIT;

	return NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::parseInfoRequest( TString &input, TCHAR *szReturnValue) {
	int numtok = input.numtok( );
	TString prop(input.gettok( 2 ));

	// [NAME] [PROP] [ID]
	if (prop == TEXT("isid") && numtok > 2) {
		int nID = input.gettok( 3 ).to_int();

		if (IsWindow(GetDlgItem(this->m_Hwnd, nID + mIRC_ID_OFFSET)) || 
			(this->getControlByID(nID + mIRC_ID_OFFSET) != NULL))
		{
			lstrcpy(szReturnValue, TEXT("$true"));
		}
		else
			lstrcpy(szReturnValue, TEXT("$false"));

		return;
	}
	// [NAME] [PROP]
	else if (prop == TEXT("nextid")) {
		int nID = mIRC_ID_OFFSET + 1;

		while (IsWindow(GetDlgItem(this->m_Hwnd, nID)) || 
			(this->getControlByID(nID) != NULL))
		{
			nID++;
		}

		wsprintf(szReturnValue, TEXT("%d"), nID - mIRC_ID_OFFSET);
		return;
	}
	// [NAME] [PROP] [N|NAMEDID]
	if (prop == TEXT("id") && numtok > 2) {
		int N = input.gettok( 3 ).to_int() -1;

		if (N == -1)
		{
			if (input.gettok( 3 ) == TEXT('0'))
				wsprintf(szReturnValue, TEXT("%d"), this->m_vpControls.size());
			else
			{
				
				for(IntegerHash::iterator it = this->namedIds.begin(); it != this->namedIds.end(); ++it)
				{
					if (it->first == input.gettok( 3 )) 
					{
						wsprintf(szReturnValue, TEXT("%i"), it->second);
						return;
					}
				}
				szReturnValue[0] = TEXT('\0');
				//wsprintf(szReturnValue, TEXT("%s"), TEXT(""));
				return;
			}
		}
		else if ((N > -1) && (N < (int) this->m_vpControls.size()))
			wsprintf(szReturnValue, TEXT("%d"), this->m_vpControls[N]->getUserID());

		return;
	}
	// [NAME] [PROP]
	else if (prop == TEXT("ismenu")) {
		if (GetMenu(this->m_Hwnd) != NULL)
			lstrcpy(szReturnValue, TEXT("$true"));
		else
			lstrcpy(szReturnValue, TEXT("$false"));

		return;
	}
	// [NAME] [PROP]
	else if (prop == TEXT("ismarked")) {
		if (Dcx::Dialogs.getDialogByHandle(this->m_Hwnd) != NULL)
			lstrcpy(szReturnValue, TEXT("$true"));
		else
			lstrcpy(szReturnValue, TEXT("$false"));

		return;
	}
	// [NAME] [PROP]
	else if (prop == TEXT("parent")) {
		wsprintf(szReturnValue, TEXT("%s"), this->getParentName().to_chr());
		return;
	}
	// [NAME] [PROP]
	else if (prop == TEXT("mouseid")) {
		wsprintf(szReturnValue, TEXT("%d"), this->m_MouseID);
		return;
	}
	// [NAME] [PROP]
	else if (prop == TEXT("focusid")) {
		wsprintf(szReturnValue, TEXT("%d"), this->m_FocusID);
		return;
	}
	// [NAME] [PROP]
	else if (prop == TEXT("mouse")) {
		POINT pt;

		GetCursorPos(&pt);
		MapWindowPoints(NULL, this->m_Hwnd, &pt, 1);
		wsprintf(szReturnValue, TEXT("%d %d"), pt.x, pt.y);

		return;
	}
	// [NAME] [PROP]
	else if (prop == TEXT("key")) {
		UINT iKeyState = 0;

		if (GetAsyncKeyState(VK_LBUTTON) < 0)
			iKeyState |= 1;
		if (GetAsyncKeyState(VK_RBUTTON) < 0)
			iKeyState |= 2;
		if (GetAsyncKeyState(VK_MBUTTON) < 0)
			iKeyState |= 4;
		if (GetAsyncKeyState(VK_LSHIFT) < 0)
			iKeyState |= 8;
		if (GetAsyncKeyState(VK_LCONTROL) < 0)
			iKeyState |= 16;
		if (GetAsyncKeyState(VK_LMENU) < 0)
			iKeyState |= 32;
		if (GetAsyncKeyState(VK_RSHIFT) < 0)
			iKeyState |= 64;
		if (GetAsyncKeyState(VK_RCONTROL) < 0)
			iKeyState |= 128;
		if (GetAsyncKeyState(VK_RMENU) < 0)
			iKeyState |= 256;
		if (GetAsyncKeyState(VK_LEFT) < 0)
			iKeyState |= 512;
		if (GetAsyncKeyState(VK_UP) < 0)
			iKeyState |= 1024;
		if (GetAsyncKeyState(VK_RIGHT) < 0)
			iKeyState |= 2048;
		if (GetAsyncKeyState(VK_DOWN) < 0)
			iKeyState |= 4096;
		if (GetAsyncKeyState(VK_CAPITAL) < 0)
			iKeyState |= 8192;

		wsprintf(szReturnValue, TEXT("%d"), iKeyState);
		return;
	}
	// [NAME] [PROP]
	else if (prop == TEXT("alias")) {
		wsprintf(szReturnValue, TEXT("%s"), this->getAliasName().to_chr());
		return;
	}
	// [NAME] [PROP] [N]
	else if ((prop == TEXT("zlayer")) && (numtok > 2)) {
		int n = input.gettok( 3 ).to_int();
		int size = (int) this->m_vZLayers.size();

		// return total number of id's
		if (n == 0)
			wsprintf(szReturnValue, TEXT("%d"), size);
		// return the Nth id
		else if ((n > 0) && (n <= size)) {
			n--;
			wsprintf(szReturnValue, TEXT("%d"), this->m_vZLayers[n] - mIRC_ID_OFFSET);
		}

		return;
	}
	// [NAME] [PROP]
	else if (prop == TEXT("zlayercurrent")) {
		wsprintf(szReturnValue, TEXT("%d"), this->m_zLayerCurrent +1);
		return;
	}
	// [NAME] [PROP]
	else if (prop == TEXT("visible")) {
		if (IsWindowVisible(this->m_Hwnd))
			lstrcpy(szReturnValue, TEXT("$true"));
		else
			lstrcpy(szReturnValue, TEXT("$false"));
		return;
	}
	// [NAME] [PROP]
	else if (prop == TEXT("glasscolor")) {
		RGBQUAD clr = {0};
		BOOL bOpaque = FALSE;
		Dcx::VistaModule.dcxDwmGetColorizationColor((DWORD *)&clr, &bOpaque);
		wsprintf(szReturnValue, TEXT("%d"), RGB(clr.rgbRed,clr.rgbGreen,clr.rgbBlue));
		return;
	}
	// invalid info request
	else
		this->showError(prop.to_chr(), NULL, TEXT("Invalid property or parameters"));

	szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

bool DcxDialog::evalAliasEx(TCHAR *szReturn, const int maxlen, const TCHAR *szFormat, ...) {
	TString line;
	va_list args;

	va_start(args, szFormat);
	line.tvprintf(szFormat, &args);
	va_end(args);

	return evalAlias(szReturn, maxlen, line.to_chr());
}

bool DcxDialog::evalAlias(TCHAR *szReturn, const int maxlen, const TCHAR *szArgs) {
	//// create a temp %var for the args
	//// This solves the ,() in args bugs, but causes problems with the , that we want.
	//int rCnt = this->getRefCount();
	//Dcx::mIRC.execex(TEXT("/set -n %%d%d %s"), rCnt, params);
	this->incRef();
	bool res = Dcx::mIRC.evalex(szReturn, maxlen, TEXT("$%s(%s,%s)"), this->getAliasName().to_chr(), this->getName().to_chr(), szArgs);
	this->decRef();
	return res;
}

bool DcxDialog::execAliasEx(const TCHAR *szFormat, ...) {
	TString line;
	va_list args;

	va_start(args, szFormat);
	line.tvprintf(szFormat, &args);
	va_end(args);

	return execAlias(line.to_chr());
}

bool DcxDialog::execAlias(const TCHAR *szArgs) {
	this->incRef();
	bool res = Dcx::mIRC.evalex(NULL, 0, TEXT("$%s(%s,%s)"), this->getAliasName().to_chr(), this->getName().to_chr(), szArgs);
	this->decRef();
	return res;
}

/*!
 * \brief blah
 *
 * blah
 */

bool DcxDialog::updateLayout(RECT &rc) {
	if (this->m_pLayoutManager == NULL)
		return false;
	//if (this->m_pLayoutManager->getRoot() == NULL) // updateLayout() does root check for us.
	//	return false;
	return (this->m_pLayoutManager->updateLayout(rc) ? true : false);
}

/*!
 * \brief blah
 *
 * blah
 */

HBRUSH DcxDialog::getBackClrBrush() const {
	return this->m_hBackBrush;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::setMouseControl(const UINT mUID) {
	if (mUID != this->m_MouseID) {
		if (this->m_dEventMask & DCX_EVENT_MOUSE) {
			this->execAliasEx(TEXT("%s,%d"), TEXT("mouseleave"), this->m_MouseID);
			this->execAliasEx(TEXT("%s,%d"), TEXT("mouseenter"), mUID);
		}
		this->m_MouseID = mUID;
	}
	else if (this->m_dEventMask & DCX_EVENT_MOUSE)
		this->execAliasEx(TEXT("%s,%d"), TEXT("mouse"), mUID);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::setFocusControl(const UINT mUID) {
	if (mUID != this->m_FocusID) {
		if (this->m_dEventMask & DCX_EVENT_FOCUS) {
			this->execAliasEx(TEXT("%s,%d"), TEXT("focusout"), this->m_FocusID);
			this->execAliasEx(TEXT("%s,%d"), TEXT("focus"), mUID);
		}
		this->m_FocusID = mUID;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT WINAPI DcxDialog::WindowProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	DcxDialog *p_this = (DcxDialog*) GetProp(mHwnd, TEXT("dcx_this"));

	// sanity check for prop existing.
	if (p_this == NULL)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	// If Message is blocking just call old win proc
	if ((InSendMessageEx(NULL) & (ISMEX_REPLIED|ISMEX_SEND)) == ISMEX_SEND)
		return CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);

	BOOL bParsed = FALSE;
	LRESULT lRes = 0L;

	// TODO: Is this instruction needed? Referencecount is managed by callAliasEx too ...
	//p_this->incRef();

	switch (uMsg) {
		case WM_HELP:
			{
				if (p_this->getEventMask() & DCX_EVENT_HELP)
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("help"), 0);
				bParsed = TRUE;
				lRes = TRUE;
			}
			break;

		case WM_MOVE:
			{
				if ((p_this->m_Shadow.Status & DCX_SS_VISABLE) && p_this->isShadowed())
				{
					RECT WndRect;
					dcxGetWindowRect(mHwnd, &WndRect);
					SetWindowPos(p_this->m_Shadow.hWin, 0,
						WndRect.left + p_this->m_Shadow.nxOffset - p_this->m_Shadow.nSize,
						WndRect.top + p_this->m_Shadow.nyOffset - p_this->m_Shadow.nSize,
						0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
				}
				p_this->SetVistaStylePos();
			}
			break;
		case WM_THEMECHANGED:
			{
				if (p_this->m_dEventMask & DCX_EVENT_THEME)
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("themechanged"), 0);
				break;
			}
		case WM_NOTIFY:
			{
				LPNMHDR hdr = (LPNMHDR) lParam;

				if (!hdr)
					break;

				if (IsWindow(hdr->hwndFrom)) {
					DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,TEXT("dcx_cthis"));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
				break;
			}
		case WM_COMMAND:
			{
				if ((HIWORD(wParam) == 0) && (LOWORD(wParam) == 2) && (lParam == NULL)) {
					if (p_this->getRefCount() > 0) {
						// This stops a crash when someone uses /dialog -x within the callback alias without a /timer
						// NB: After this is done you must use /xdialog -x to close the dialog, /dialog -x will no longer work.
						bParsed = TRUE;
						Dcx::mIRC.execex(TEXT("/.timer -m 1 0 xdialog -x %s"), p_this->getName().to_chr());
					}
					else if (p_this->m_dEventMask & DCX_EVENT_CLOSE) {
						TCHAR ret[256];

						p_this->evalAliasEx(ret, 255, TEXT("%s,%d"), TEXT("close"), 0);

						if (lstrcmp(TEXT("noclose"), ret) == 0)
							bParsed = TRUE;
					}
					break;
				}
			}
			// fall through
		case WM_HSCROLL:
		case WM_VSCROLL:
			{
				if (IsWindow((HWND) lParam)) {
					DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,TEXT("dcx_cthis"));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
				break;
			}

		case WM_COMPAREITEM:
			{
				LPCOMPAREITEMSTRUCT idata = (LPCOMPAREITEMSTRUCT)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DELETEITEM:
			{
				DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
				break;
			}

		case WM_MEASUREITEM:
			{
				HWND cHwnd = GetDlgItem(mHwnd, wParam);
				if (IsWindow(cHwnd)) {
					DcxControl *c_this = (DcxControl *) GetProp(cHwnd,TEXT("dcx_cthis"));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}

				LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;

				if (p_this->m_popup != NULL && lpmis->CtlType == ODT_MENU) {
					XPopupMenuItem *p_Item = (XPopupMenuItem*) lpmis->itemData;

					if (p_Item != NULL) {
						SIZE size = p_Item->getItemSize(mHwnd);

						lpmis->itemWidth = size.cx;
						lpmis->itemHeight = size.cy;
						lRes = TRUE; 
						bParsed = TRUE;
					}
				}
			}
			break;

		case WM_DRAWITEM:
			{
				DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
				else if (p_this->m_popup != NULL && idata->CtlType == ODT_MENU) {
					XPopupMenuItem *p_Item = (XPopupMenuItem*) idata->itemData;

					if (p_Item != NULL) {
						p_Item->DrawItem(idata);
						lRes = TRUE; 
						bParsed = TRUE;
					}
				}
				break;
			}

		case WM_SYSCOMMAND:
			{
				switch (wParam & 0xFFF0) {
				case SC_MOVE:
					{
						if (p_this->m_dEventMask & DCX_EVENT_MOVE) {
							TCHAR ret[256];
							p_this->evalAliasEx(ret, 255, TEXT("%s,%d"), TEXT("beginmove"), 0);

							if (lstrcmp(TEXT("nomove"), ret) != 0) {
								bParsed = TRUE;
								p_this->m_bInMoving = true;
								lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
							}
						}
						break;
					}

				case SC_CLOSE:
					{
						if (p_this->m_dEventMask & DCX_EVENT_CLOSE) {
							TCHAR ret[256];

							p_this->evalAliasEx(ret, 255, TEXT("%s,%d"), TEXT("scclose"), 0);

							if (lstrcmp(TEXT("noclose"), ret) == 0)
								bParsed = TRUE;
						}
						break;
					}

				case SC_MINIMIZE:
					{
						if (p_this->m_dEventMask & DCX_EVENT_SIZE) {
							TCHAR ret[256];

							p_this->evalAliasEx(ret, 255, TEXT("%s,%d"), TEXT("min"), 0);

							if (lstrcmp(TEXT("stop"), ret) != 0) {
								bParsed = TRUE;
								lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
							}
						}
						break;
					}

				case SC_MAXIMIZE:
					{
						if (p_this->m_dEventMask & DCX_EVENT_SIZE) {
							TCHAR ret[256];

							p_this->evalAliasEx(ret, 255, TEXT("%s,%d"), TEXT("max"), 0);

							if (lstrcmp(TEXT("stop"), ret) != 0) {
								bParsed = TRUE;
								lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
							}
						}

						break;
					}

				case SC_RESTORE:
					{
						if (p_this->m_dEventMask & DCX_EVENT_SIZE) {
							p_this->execAliasEx(TEXT("%s,%d"), TEXT("restore"), 0);

							//bParsed = TRUE;
							//lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
						}

						break;
					}

				case SC_SIZE:
					{
						TCHAR ret[256];

						if (p_this->m_dEventMask & DCX_EVENT_SIZE) // mask only controls sending of event, if event isnt sent then DefWindowProc should still be called.
							p_this->evalAliasEx(ret, 255, TEXT("%s,%d"), TEXT("beginsize"), 0);

						if (lstrcmp(TEXT("nosize"), ret) != 0) {
							bParsed = TRUE;
							p_this->m_bInSizing = true;
							lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
						}
						break;
					}
				} // WM_SYSCOMMAND switch

				break;
			}

			// ghost drag stuff
		case WM_ENTERSIZEMOVE:
			{
				if (p_this->m_bDoGhostDrag < 255) {
					if (!p_this->m_bVistaStyle) {
						DWORD style = GetWindowExStyle(mHwnd);
						// Set WS_EX_LAYERED on this window
						if (!(style & WS_EX_LAYERED))
							SetWindowLong(mHwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
						// Make this window alpha
						SetLayeredWindowAttributes(mHwnd, 0, p_this->m_bDoGhostDrag, LWA_ALPHA);
					}
					p_this->m_bGhosted = true;
				}
				p_this->UpdateVistaStyle();
			}
			break;

		case WM_EXITSIZEMOVE:
			{
				if ((p_this->m_Shadow.Status & DCX_SS_VISABLE) && p_this->isShadowed())
					p_this->UpdateShadow();

				if ((p_this->m_bInSizing) && (p_this->m_dEventMask & DCX_EVENT_SIZE))
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("endsize"), 0);
				else if ((p_this->m_bInMoving)  && (p_this->m_dEventMask & DCX_EVENT_MOVE))
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("endmove"), 0);

//#if !defined(NDEBUG) || defined(DCX_DEV_BUILD)
				bool bDoRedraw = p_this->m_bInSizing;
//#endif

				p_this->m_bInMoving = false;
				p_this->m_bInSizing = false;
				// turn off ghosting.
				if (p_this->m_bGhosted) {
					if (!p_this->m_bVistaStyle) {
						// Make this window solid
						SetLayeredWindowAttributes(mHwnd, 0, p_this->m_iAlphaLevel, LWA_ALPHA);
					}
					p_this->m_bGhosted = false;
				}
				p_this->UpdateVistaStyle();
//#if !defined(NDEBUG) || defined(DCX_DEV_BUILD)
				if (bDoRedraw && !p_this->IsVistaStyle() && !p_this->isExStyle(WS_EX_COMPOSITED))
					p_this->redrawWindow();
//#endif
				break;
			}

		case WM_MOVING:
			{
				if (p_this->m_dEventMask & DCX_EVENT_MOVE)
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("moving"), 0);
				break;
			}

		case WM_SIZE:
			{
				// After window region is set it needs updated whenever the dialog is resized.
				// No way to scale a region tho :/
				//HRGN hRGN = CreateRectRgn(0,0,0,0);
				//if (GetWindowRgn(p_this->m_Hwnd, hRGN) != ERROR)
				//{
				//}
				//DeleteObject(hRGN);
				
				if ((p_this->m_Shadow.Status & DCX_SS_ENABLED) && p_this->isShadowed())
				{
					if(SIZE_MAXIMIZED == wParam || SIZE_MINIMIZED == wParam)
					{
						ShowWindow(p_this->m_Shadow.hWin, SW_HIDE);
						p_this->m_Shadow.Status &= ~DCX_SS_VISABLE;
					}
					else
					{
						if(p_this->isStyle(WS_VISIBLE))	// Parent may be resized even if invisible
						{
							p_this->m_Shadow.Status |= DCX_SS_PARENTVISIBLE;
							if(!(p_this->m_Shadow.Status & DCX_SS_VISABLE))
							{
								p_this->m_Shadow.Status |= DCX_SS_VISABLE;
								// Update before show, because if not, restore from maximized will
								// see a glance misplaced shadow
								p_this->UpdateShadow();
								ShowWindow(p_this->m_Shadow.hWin, SW_SHOWNA);
								// If restore from minimized, the window region will not be updated until WM_PAINT:(
								p_this->m_Shadow.bUpdate = true;
							}
							// Awful! It seems that if the window size was not decreased
							// the window region would never be updated until WM_PAINT was sent.
							// So do not Update() until next WM_PAINT is received in this case
							else if(LOWORD(lParam) > LOWORD(p_this->m_Shadow.WndSize) || HIWORD(lParam) > HIWORD(p_this->m_Shadow.WndSize))
								p_this->m_Shadow.bUpdate = true;
							else
								p_this->UpdateShadow();
						}
					}
					p_this->m_Shadow.WndSize = lParam;
				}

				if (p_this->m_dEventMask & DCX_EVENT_SIZE)
					p_this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("sizing"), 0, LOWORD(lParam), HIWORD(lParam));

				HWND bars = NULL;

				//SendMessage(mHwnd, WM_SETREDRAW, FALSE, 0);
				while ((bars = FindWindowEx(mHwnd, bars, DCX_REBARCTRLCLASS, NULL)) != NULL) {
					SendMessage(bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0);
				}

				while ((bars = FindWindowEx(mHwnd, bars, DCX_STATUSBARCLASS, NULL)) != NULL) {
					//SendMessage(bars, WM_SETREDRAW, FALSE, 0);
					SendMessage(bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0);
					//SendMessage(bars, WM_SETREDRAW, TRUE, 0);
				}
				while ((bars = FindWindowEx(mHwnd, bars, DCX_PANELCLASS, NULL)) != NULL) {
					SendMessage(bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0);
				}

				while ((bars = FindWindowEx(mHwnd, bars, DCX_TOOLBARCLASS, NULL)) != NULL) {
					DcxToolBar *t = (DcxToolBar*) p_this->getControlByHWND(bars);

					t->autoPosition(LOWORD(lParam), HIWORD(lParam));
					//SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) lParam );
				}

				RECT rc;

				SetRect(&rc, 0, 0, LOWORD(lParam), HIWORD(lParam));
				p_this->SetVistaStyleSize();
				p_this->updateLayout(rc);
				//SendMessage(mHwnd, WM_SETREDRAW, TRUE, 0);
				//This is needed (or some other solution) to update the bkg image & transp controls on it
//#if defined(NDEBUG) && !defined(DCX_DEV_BUILD)
//				p_this->redrawWindow(); // Causes alot of flicker.
//#else
				// Only included in debug & dev builds atm.
				if (p_this->IsVistaStyle() || p_this->isExStyle(WS_EX_COMPOSITED))
					p_this->redrawWindow();
				else {
					p_this->redrawBufferedWindow(); // Avoids flicker.
					//p_this->redrawWindow();
					// NB: This only fixed richedit controls that are direct children of the dialog NOT grandchildren.
					while ((bars = FindWindowEx(mHwnd, bars, DCX_RICHEDITCLASS, NULL)) != NULL) { // workaround added for RichText controls which seem to not redraw correctly via WM_PRINT
						RedrawWindow( bars, NULL, NULL, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_UPDATENOW|RDW_INVALIDATE|RDW_ERASE|RDW_FRAME );
					}
				}
//#endif
				break;
			}
			//case WM_NCCALCSIZE:
			//	{
			//	}
			//	break;

		case WM_WINDOWPOSCHANGING:
			{
				if (p_this->m_dEventMask & DCX_EVENT_MOVE) {
					WINDOWPOS *wp = (WINDOWPOS *) lParam;

					// break if nomove & nosize specified, since thats all we care about.
					if ((wp == NULL) || ((wp->flags & SWP_NOMOVE) && (wp->flags & SWP_NOSIZE)))
						break;

					TCHAR ret[256], *p = NULL;

					switch ((wp->flags & (SWP_NOSIZE|SWP_NOMOVE))) {
					case SWP_NOSIZE:
						p = TEXT("moving");
						break;
					case SWP_NOMOVE:
						p = TEXT("sizing");
						break;
					default:
						p = TEXT("both");
						break;
					}

					//p_this->callAliasEx(ret, TEXT("changing,0,%d,%d,%d,%d,%d"), (wp->flags & 3),wp->x, wp->y, wp->cx, wp->cy);
					p_this->evalAliasEx(ret, 255, TEXT("changing,0,%s,%d,%d,%d,%d"), p,wp->x, wp->y, wp->cx, wp->cy);

					if (lstrcmp(TEXT("nosize"), ret) == 0)
						wp->flags |= SWP_NOSIZE;
					else if (lstrcmp(TEXT("nomove"), ret) == 0)
						wp->flags |= SWP_NOMOVE;
					else if (lstrcmp(TEXT("nochange"), ret) == 0)
						wp->flags |= SWP_NOSIZE | SWP_NOMOVE;
				}
				break;
			}

		case WM_ERASEBKGND:
			{
				if (mHwnd != p_this->getHwnd())
					break;
				RECT rwnd;

				GetClientRect(mHwnd, &rwnd);

				DcxDialog::DrawDialogBackground((HDC) wParam,p_this,&rwnd);

				bParsed = TRUE;
				lRes = TRUE;
				break;
			}

		case WM_CTLCOLORDLG:
			{
				bParsed = TRUE;
				lRes = (INT_PTR) p_this->getBackClrBrush();
				break;
			}

		case WM_CTLCOLORBTN:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLOREDIT:
			{
				DcxControl *p_Control = p_this->getControlByHWND((HWND) lParam);

				if (p_Control != NULL) {
					COLORREF clrText = p_Control->getTextColor();
					COLORREF clrBackText = p_Control->getBackColor();
					HBRUSH hBackBrush = p_Control->getBackClrBrush();

					// let static controls shine through
					//				SetBkMode((HDC) wParam, TRANSPARENT);
					//				return (LRESULT) GetStockBrush(HOLLOW_BRUSH);

					//http://www.ddj.com/dept/windows/184416395
					//http://www.codeguru.com/Cpp/misc/samples/article.php/c1469#download
					bParsed = TRUE;
					lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);

					if (clrText != CLR_INVALID)
						SetTextColor((HDC) wParam, clrText);

					if (clrBackText != CLR_INVALID)
						SetBkColor((HDC) wParam, clrBackText);

					if (p_Control->isExStyle(WS_EX_TRANSPARENT)) {
						// when transparent set as no bkg brush & default transparent drawing.
						SetBkMode((HDC) wParam, TRANSPARENT);
						//SetBkColor((HDC) wParam, CLR_NONE);
						hBackBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
					}

					if (hBackBrush != NULL)
						lRes = (LRESULT) hBackBrush;

				}
				break;
			}

		case WM_MOUSEMOVE:
			{
				p_this->setMouseControl(0);
				if (p_this->m_bDrag) {
					POINT pt;
					GetCursorPos(&pt);
					PostMessage(p_this->m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM( pt.x, pt.y));
					p_this->m_bDrag = false;
				}
				if ( p_this->m_bTracking == FALSE ) {
					TRACKMOUSEEVENT tme;
					tme.cbSize = sizeof(TRACKMOUSEEVENT);
					tme.hwndTrack = p_this->m_Hwnd;
					tme.dwFlags = TME_LEAVE;
					tme.dwHoverTime = HOVER_DEFAULT; //1;
					p_this->m_bTracking = (BOOL) _TrackMouseEvent( &tme );
					if (p_this->m_dEventMask & DCX_EVENT_MOUSE)
						p_this->execAliasEx(TEXT("%s,%d"), TEXT("denter"), 0); // this tells you when the mouse enters or
					p_this->UpdateVistaStyle();
				}
				break;
			}

		case WM_MOUSELEAVE:
			{
				if ( p_this->m_bTracking ) {
					p_this->m_bTracking = FALSE;
					if (p_this->m_dEventMask & DCX_EVENT_MOUSE)
						p_this->execAliasEx(TEXT("%s,%d"), TEXT("dleave"), 0); // leaves a dialogs client area.
				}
				p_this->UpdateVistaStyle();
			}
			break;

		case WM_LBUTTONDOWN:
			{
				if (p_this->m_dEventMask & DCX_EVENT_CLICK)
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("lbdown"), 0);
				if (p_this->m_bDoDrag)
					p_this->m_bDrag = true;
				break;
			}

		case WM_LBUTTONUP:
			{
				if (p_this->m_dEventMask & DCX_EVENT_CLICK) {
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("lbup"), 0);
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("sclick"), 0);
				}
				break;
			}

		case WM_LBUTTONDBLCLK:
			{
				if (p_this->m_dEventMask & DCX_EVENT_CLICK) {
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("dclick"), 0);
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("lbdblclk"), 0);
				}
				break;
			}

		case WM_RBUTTONDOWN:
			{
				if (p_this->m_dEventMask & DCX_EVENT_CLICK)
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("rbdown"), 0);
				break;
			}

		case WM_RBUTTONUP:
			{
				if (p_this->m_dEventMask & DCX_EVENT_CLICK) {
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("rbup"), 0);
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("rclick"), 0);
				}
				break;
			}

		case WM_RBUTTONDBLCLK:
			{
				if (p_this->m_dEventMask & DCX_EVENT_CLICK)
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("rbdblclk"), 0);
				break;
			}

		case WM_MBUTTONDOWN:
			{
				if (p_this->m_dEventMask & DCX_EVENT_CLICK)
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("mbdown"), 0);
				break;
			}

		case WM_MBUTTONUP:
			{
				if (p_this->m_dEventMask & DCX_EVENT_CLICK)
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("mbup"), 0);
				break;
			}

		case WM_MBUTTONDBLCLK:
			{
				if (p_this->m_dEventMask & DCX_EVENT_CLICK)
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("mbdblclk"), 0);
				break;
			}

		case WM_MOUSEWHEEL:
			{
				if (p_this->m_dEventMask & DCX_EVENT_CLICK) {
					DWORD fwKeys = GET_KEYSTATE_WPARAM(wParam);
					DWORD zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
					TString flags(TEXT('+'));

					if (fwKeys & MK_CONTROL) // control button
						flags += TEXT('c');
					if (fwKeys & MK_LBUTTON) // left mouse button
						flags += TEXT('l');
					if (fwKeys & MK_MBUTTON) // middle mouse button button
						flags += TEXT('m');
					if (fwKeys & MK_RBUTTON) // right mouse button
						flags += TEXT('r');
					if (fwKeys & MK_SHIFT) // shift button
						flags += TEXT('s');

					p_this->execAliasEx(TEXT("%s,%d,%s,%s"),
						TEXT("mwheel"),
						p_this->m_MouseID,
						((int) zDelta > 0 ? TEXT("up") : TEXT("down")),
						flags.to_chr());

					bParsed = TRUE;
					lRes = FALSE; // stop parsing of WM_MOUSEWHEEL
				}
				break;
			}

		case WM_INITMENU:
		case WM_INITMENUPOPUP:
			{
				if (HIWORD(lParam) == FALSE && p_this->m_popup != NULL) {
					// let mIRC populate the menus dynamically
					lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
					bParsed = TRUE;

					if ((lRes == 0) && Dcx::XPopups.isMenuBarMenu(GetMenu(mHwnd), (HMENU) wParam)) {

						m_bIsMenuBar = true;

						p_this->m_popup->convertMenu((HMENU) wParam, TRUE);
					}
					m_bIsSysMenu = false;
				}
				else
					m_bIsSysMenu = true;
				break;
			}

		case WM_UNINITMENUPOPUP:
			{

				if (p_this->m_popup != NULL && m_bIsMenuBar && !m_bIsSysMenu)
					p_this->m_popup->deleteAllItemData((HMENU) wParam);

				break;
			}

		case WM_SETCURSOR:
			{
				if ((LOWORD(lParam) == HTCLIENT) && ((HWND) wParam == p_this->getHwnd()) && (p_this->getCursor() != NULL))
				{
					if (GetCursor() != p_this->getCursor())
						SetCursor(p_this->getCursor());
					bParsed = TRUE;
					lRes = TRUE;
				}
				break;
			}

		case WM_DROPFILES:
			{
				HDROP files = (HDROP) wParam;
				TCHAR filename[500];
				int count = DragQueryFile(files, 0xFFFFFFFF,  filename, 500);

				if (count) {
					if (p_this->m_dEventMask & DCX_EVENT_DRAG) {
						TCHAR ret[20];

						p_this->evalAliasEx(ret, 19, TEXT("%s,%d,%d"), TEXT("dragbegin"), 0, count);

						// cancel drag drop event
						if (lstrcmpi(ret, TEXT("cancel")) == 0) {
							DragFinish(files);
							bParsed = TRUE;
							break;
						}
						// for each file, send callback message
						for (int i = 0; i < count; i++) {
							if (DragQueryFile(files, i, filename, 500))
								p_this->execAliasEx(TEXT("%s,%d,%s"), TEXT("dragfile"), 0, filename);
						}

						p_this->execAliasEx(TEXT("%s,%d"), TEXT("dragfinish"), 0);
					}
				}

				DragFinish(files);
				break;
			}

		case WM_ACTIVATE:
			{
				if (wParam == WA_ACTIVE && p_this->m_bVistaStyle) {
					bParsed = TRUE;
					lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
					InvalidateRect(mHwnd, NULL, TRUE);
				}

				if (p_this->m_dEventMask & DCX_EVENT_FOCUS) {
					switch (wParam) {
					case WA_ACTIVE:
					case WA_CLICKACTIVE:
						{
							p_this->execAliasEx(TEXT("%s,%d"), TEXT("activate"), 0);
							break;
						}

					case WA_INACTIVE:
						{
							p_this->execAliasEx(TEXT("%s,%d"), TEXT("deactivate"), 0);
							break;
						}
					} // switch
				}
				break;
			}
		case LB_GETITEMRECT:
			{
				/*
				*	NB: This message is sent by mIRC V6.20 when you click on a docked dialog,
				*			causing an infinite loop, returning LB_ERR stops the loop.
				*	This affects ALL dialogs not just dcx ones.
				* This is not a real fix but a workaround.
				*/
				lRes = LB_ERR;
				bParsed = TRUE;
			}
			break;

		case WM_PAINT:
			{
				if (p_this->m_Shadow.bUpdate && p_this->isShadowed())
				{
					p_this->UpdateShadow();
					p_this->m_Shadow.bUpdate = false;
				}
				if (p_this->IsVistaStyle() && !IsIconic(mHwnd)) {
					ValidateRect(mHwnd, NULL);
					p_this->UpdateVistaStyle();
					lRes = 0L;
					bParsed = TRUE;
				}
			}
			break;
			//case WM_NCPAINT:
			//	{
			//		HDC hdc;
			//		hdc = GetDCEx(hwnd, (HRGN)wParam, DCX_WINDOW|DCX_INTERSECTRGN);
			//		// Paint into this DC
			//		ReleaseDC(hwnd, hdc);
			//		bParsed = TRUE;
			//	}
			//	break;

			//case WM_NCLBUTTONDOWN:
			//case WM_NCLBUTTONUP:
			//case WM_NCMOUSEHOVER:
			//case WM_NCMOUSELEAVE:
			////case WM_NCHITTEST:
			//case WM_NCPAINT:
			//	{
			//		if (p_this->IsVistaStyle()) {
			//			lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
			//			//InvalidateRect(mHwnd, NULL, FALSE);
			//			p_this->UpdateVistaStyle();
			//			bParsed = TRUE;
			//		}
			//	}
			//	break;

		case WM_SHOWWINDOW:
			{
				if(p_this->m_Shadow.Status & DCX_SS_ENABLED && !(p_this->m_Shadow.Status & DCX_SS_DISABLEDBYAERO))
				{
					lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
					if(!wParam)	// the window is being hidden
					{
						ShowWindow(p_this->m_Shadow.hWin, SW_HIDE);
						p_this->m_Shadow.Status &= ~(DCX_SS_VISABLE | DCX_SS_PARENTVISIBLE);
					}
					else
					{
						p_this->m_Shadow.bUpdate = true;
						p_this->ShowShadow();
					}
					bParsed = TRUE;
				}
			}
			break;

		case WM_DWMCOMPOSITIONCHANGED:
			{
				if (p_this->isShadowed()) {
					if (Dcx::VistaModule.refreshComposite())
						p_this->m_Shadow.Status |= DCX_SS_DISABLEDBYAERO;
					else
						p_this->m_Shadow.Status &= ~DCX_SS_DISABLEDBYAERO;

					p_this->ShowShadow();
				}
			}
			break;

		case WM_NCDESTROY:
			{
				lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);

				Dcx::Dialogs.deleteDialog(p_this);
				delete p_this;
				return lRes;
			}

		default:
			{
				// Try to process DragList events
				if (p_this->m_vDragLists.size() != 0)
					lRes = ProcessDragListMessage(p_this, uMsg, wParam, lParam, bParsed);
			}
			break;
	}
	//p_this->decRef();
	// TODO: Is this instruction needed? Referencecount is managed by callAliasEx too ...

	if (bParsed)
		return lRes;

	return CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
}


void DcxDialog::setParentName(const TString &strParent) {
	this->m_tsParentName = strParent;
}

const TString &DcxDialog::getParentName() const {
	return this->m_tsParentName;
}

void DcxDialog::DrawDialogBackground(HDC hdc, DcxDialog *p_this, LPCRECT rwnd)
{
	// background color
	if (p_this->getBackClrBrush() != NULL)
		FillRect(hdc, rwnd, p_this->getBackClrBrush());
	else
		FillRect(hdc, rwnd, GetSysColorBrush(COLOR_3DFACE));

	if (p_this->m_bitmapBg == NULL)
		return;

	HDC hdcbmp = CreateCompatibleDC(hdc);

	if (hdcbmp == NULL)
		return;

	BITMAP bmp;

	GetObject(p_this->m_bitmapBg, sizeof(BITMAP), &bmp);
	HBITMAP hOldBitmap = SelectBitmap(hdcbmp, p_this->m_bitmapBg);

	int x = rwnd->left;
	int y = rwnd->top;
	int w = rwnd->right - rwnd->left;
	int h = rwnd->bottom - rwnd->top;

	// stretch
	if (p_this->m_uStyleBg & DBS_BKGSTRETCH) {
		//BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcbmp, 0, 0, SRCCOPY);
		TransparentBlt(hdc, x, y, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, p_this->m_colTransparentBg);
	}
	// tile
	else if (p_this->m_uStyleBg & DBS_BKGTILE) {
		for (y = 0; y < h; y += bmp.bmHeight) {
			for (x = 0; x < w; x += bmp.bmWidth) {
				//BitBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, hdcbmp, 0, 0, SRCCOPY);
				TransparentBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, p_this->m_colTransparentBg);
			}
		}
	}
	// normal
	//else if (p_this->m_uStyleBg & DBS_BKGNORMAL) {
	else {
		// align horizontally
		if (p_this->m_uStyleBg & DBS_BKGCENTER)
			x = (w - bmp.bmWidth) / 2;
		else if (p_this->m_uStyleBg & DBS_BKGRIGHT)
			x = w - bmp.bmWidth;

		// align vertically
		if (p_this->m_uStyleBg & DBS_BKGVCENTER)
			y = (h - bmp.bmHeight) / 2;
		else if (p_this->m_uStyleBg & DBS_BKGBOTTOM)
			y = h - bmp.bmHeight;

		TransparentBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, p_this->m_colTransparentBg);
	}
	SelectBitmap(hdcbmp, hOldBitmap);
	DeleteDC(hdcbmp);
}

// CWndShadow ...
//
// Copyright (c) 2006 Perry Zhu, All Rights Reserved.
//
// mailto:perry@live.com
//
// This code has been modified for use with DCX, & is used for the DcxDialog Shadows.

bool DcxDialog::AddShadow(void)
{
	if (!this->isShadowed()) {
		// Create the shadow window
		this->m_Shadow.hWin = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, DCX_SHADOWCLASS, NULL,
			WS_CAPTION | WS_POPUPWINDOW, CW_USEDEFAULT, 0, 0, 0, this->m_Hwnd, NULL, GetModuleHandle(NULL), NULL);

		if (!IsWindow(this->m_Shadow.hWin))
			return false;

		this->m_Shadow.Status = DCX_SS_ENABLED;

		if (Dcx::VistaModule.isAero())
			this->m_Shadow.Status |= DCX_SS_DISABLEDBYAERO;

		this->ShowShadow();

		return true;
	}
	return false;
}
void DcxDialog::ShowShadow(void)
{
	// Clear all except the enabled status
	this->m_Shadow.Status &= DCX_SS_ENABLED | DCX_SS_DISABLEDBYAERO;

	if((this->m_Shadow.Status & DCX_SS_ENABLED) && !(this->m_Shadow.Status & DCX_SS_DISABLEDBYAERO))	// Enabled
	{
		// Determine the show state of shadow according to parent window's state
		DWORD lParentStyle = GetWindowStyle(this->m_Hwnd);

		if(WS_VISIBLE & lParentStyle)	// Parent visible
		{
			this->m_Shadow.Status |= DCX_SS_PARENTVISIBLE;

			// Parent is normal, show the shadow
			if(!((WS_MAXIMIZE | WS_MINIMIZE) & lParentStyle))	// Parent visible but does not need shadow
				this->m_Shadow.Status |= DCX_SS_VISABLE;
		}
	}

	if(this->m_Shadow.Status & DCX_SS_VISABLE)
	{
		ShowWindow(this->m_Shadow.hWin, SW_SHOWNA);
		this->UpdateShadow();
	}
	else
		ShowWindow(this->m_Shadow.hWin, SW_HIDE);
}

void DcxDialog::RemoveShadow(void)
{
	if (IsWindow(this->m_Shadow.hWin))
		DestroyWindow(this->m_Shadow.hWin);
}

void DcxDialog::UpdateShadow(void)
{
	RECT WndRect;
	GetWindowRect(this->m_Hwnd, &WndRect);
	int nShadWndWid = WndRect.right - WndRect.left + this->m_Shadow.nSize * 2;
	int nShadWndHei = WndRect.bottom - WndRect.top + this->m_Shadow.nSize * 2;

	// Create the alpha blending bitmap
	BITMAPINFO bmi;        // bitmap header

	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = nShadWndWid;
	bmi.bmiHeader.biHeight = nShadWndHei;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = nShadWndWid * nShadWndHei * 4;

	BYTE *pvBits;          // pointer to DIB section
	HBITMAP hbitmap = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void **)&pvBits, NULL, 0);

	ZeroMemory(pvBits, bmi.bmiHeader.biSizeImage);
	MakeShadow((UINT32 *)pvBits, this->m_Hwnd, &WndRect);

	HDC hMemDC = CreateCompatibleDC(NULL);
	HBITMAP hOriBmp = SelectBitmap(hMemDC, hbitmap);

	POINT ptDst = {WndRect.left + this->m_Shadow.nxOffset - this->m_Shadow.nSize, WndRect.top + this->m_Shadow.nyOffset - this->m_Shadow.nSize};
	POINT ptSrc = {0, 0};
	SIZE WndSize = {nShadWndWid, nShadWndHei};
	BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

	MoveWindow(this->m_Shadow.hWin, ptDst.x, ptDst.y, nShadWndWid, nShadWndHei, FALSE);

	UpdateLayeredWindow(this->m_Shadow.hWin, NULL, &ptDst, &WndSize, hMemDC, &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);

	// Delete used resources
	DeleteBitmap(SelectBitmap(hMemDC, hOriBmp));
	DeleteDC(hMemDC);
}

bool DcxDialog::isShadowed(void) const
{
	return (IsWindow(this->m_Shadow.hWin) ? true : false);
}

void DcxDialog::MakeShadow(UINT32 *pShadBits, const HWND hParent, const RECT *rcParent)
{
	// The shadow algorithm:
	// Get the region of parent window,
	// Apply morphologic erosion to shrink it into the size (ShadowWndSize - Sharpness)
	// Apply modified (with blur effect) morphologic dilation to make the blurred border
	// The algorithm is optimized by assuming parent window is just "one piece" and without "wholes" on it

	// Get the region of parent window,
	// Create a full rectangle region in case of the window region is not defined
	HRGN hParentRgn = CreateRectRgn(0, 0, rcParent->right - rcParent->left, rcParent->bottom - rcParent->top);
	GetWindowRgn(hParent, hParentRgn);

	// Determine the Start and end point of each horizontal scan line
	SIZE szParent = {rcParent->right - rcParent->left, rcParent->bottom - rcParent->top};
	SIZE szShadow = {szParent.cx + 2 * this->m_Shadow.nSize, szParent.cy + 2 * this->m_Shadow.nSize};
	// Extra 2 lines (set to be empty) in ptAnchors are used in dilation
	int nAnchors = max(szParent.cy, szShadow.cy);	// # of anchor points pares
	int (*ptAnchors)[2] = new int[nAnchors + 2][2];
	int (*ptAnchorsOri)[2] = new int[szParent.cy][2];	// anchor points, will not modify during erosion
	ptAnchors[0][0] = szParent.cx;
	ptAnchors[0][1] = 0;
	ptAnchors[nAnchors + 1][0] = szParent.cx;
	ptAnchors[nAnchors + 1][1] = 0;
	if(this->m_Shadow.nSize > 0)
	{
		// Put the parent window anchors at the center
		for(int i = 0; i < this->m_Shadow.nSize; i++)
		{
			ptAnchors[i + 1][0] = szParent.cx;
			ptAnchors[i + 1][1] = 0;
			ptAnchors[szShadow.cy - i][0] = szParent.cx;
			ptAnchors[szShadow.cy - i][1] = 0;
		}
		ptAnchors += this->m_Shadow.nSize;
	}
	for(int i = 0; i < szParent.cy; i++)
	{
		// find start point
		int j;
		for(j = 0; j < szParent.cx; j++)
		{
			if(PtInRegion(hParentRgn, j, i))
			{
				ptAnchors[i + 1][0] = j + this->m_Shadow.nSize;
				ptAnchorsOri[i][0] = j;
				break;
			}
		}

		if(j >= szParent.cx)	// Start point not found
		{
			ptAnchors[i + 1][0] = szParent.cx;
			ptAnchorsOri[i][1] = 0;
			ptAnchors[i + 1][0] = szParent.cx;
			ptAnchorsOri[i][1] = 0;
		}
		else
		{
			// find end point
			for(j = szParent.cx - 1; j >= ptAnchors[i + 1][0]; j--)
			{
				if(PtInRegion(hParentRgn, j, i))
				{
					ptAnchors[i + 1][1] = j + 1 + this->m_Shadow.nSize;
					ptAnchorsOri[i][1] = j + 1;
					break;
				}
			}
		}
		//if(0 != ptAnchorsOri[i][1])
		//	TRACE(TEXT("%d %d\n"), ptAnchorsOri[i][0], ptAnchorsOri[i][1]);
	}

	if(this->m_Shadow.nSize > 0)
		ptAnchors -= this->m_Shadow.nSize;	// Restore pos of ptAnchors for erosion
	int (*ptAnchorsTmp)[2] = new int[nAnchors + 2][2];	// Store the result of erosion
	// First and last line should be empty
	ptAnchorsTmp[0][0] = szParent.cx;
	ptAnchorsTmp[0][1] = 0;
	ptAnchorsTmp[nAnchors + 1][0] = szParent.cx;
	ptAnchorsTmp[nAnchors + 1][1] = 0;
	int nEroTimes = 0;
	// morphologic erosion
	for(int i = 0; i < this->m_Shadow.nSharpness - this->m_Shadow.nSize; i++)
	{
		nEroTimes++;
		//ptAnchorsTmp[1][0] = szParent.cx;
		//ptAnchorsTmp[1][1] = 0;
		//ptAnchorsTmp[szParent.cy + 1][0] = szParent.cx;
		//ptAnchorsTmp[szParent.cy + 1][1] = 0;
		for(int j = 1; j < nAnchors + 1; j++)
		{
			ptAnchorsTmp[j][0] = max(ptAnchors[j - 1][0], max(ptAnchors[j][0], ptAnchors[j + 1][0])) + 1;
			ptAnchorsTmp[j][1] = min(ptAnchors[j - 1][1], min(ptAnchors[j][1], ptAnchors[j + 1][1])) - 1;
		}
		// Exchange ptAnchors and ptAnchorsTmp;
		int (*ptAnchorsXange)[2] = ptAnchorsTmp;
		ptAnchorsTmp = ptAnchors;
		ptAnchors = ptAnchorsXange;
	}

	// morphologic dilation
	ptAnchors += (this->m_Shadow.nSize < 0 ? -this->m_Shadow.nSize : 0) + 1;	// now coordinates in ptAnchors are same as in shadow window
	// Generate the kernel
	int nKernelSize = this->m_Shadow.nSize > this->m_Shadow.nSharpness ? this->m_Shadow.nSize : this->m_Shadow.nSharpness;
	int nCenterSize = this->m_Shadow.nSize > this->m_Shadow.nSharpness ? (this->m_Shadow.nSize - this->m_Shadow.nSharpness) : 0;
	UINT32 *pKernel = new UINT32[(2 * nKernelSize + 1) * (2 * nKernelSize + 1)];
	UINT32 *pKernelIter = pKernel;
	const DWORD preColDark = PreMultiply(this->m_Shadow.Color, this->m_Shadow.nDarkness);
	for(int i = 0; i <= 2 * nKernelSize; i++)
	{
		for(int j = 0; j <= 2 * nKernelSize; j++)
		{
			double dLength = sqrt((i - nKernelSize) * (i - nKernelSize) + (j - nKernelSize) * (double)(j - nKernelSize));
			if(dLength < nCenterSize)
				*pKernelIter = this->m_Shadow.nDarkness << 24 | preColDark;
			else if(dLength <= nKernelSize)
			{
				UINT32 nFactor = ((UINT32)((1 - (dLength - nCenterSize) / (this->m_Shadow.nSharpness + 1)) * this->m_Shadow.nDarkness));
				*pKernelIter = nFactor << 24 | PreMultiply(this->m_Shadow.Color, (BYTE)(nFactor & 0xFF));
				// TODO: Examin this nFactor usage in PreMultiply() as its converting a UINT32 to an unsigned char
				// changed nFactor to (BYTE)(nFactor & 0xFF) to mask out > byte bits.
			}
			else
				*pKernelIter = 0;
			//TRACE(TEXT("%d "), *pKernelIter >> 24);
			pKernelIter ++;
		}
		//TRACE(TEXT("\n"));
	}
	// Generate blurred border
	for(int i = nKernelSize; i < szShadow.cy - nKernelSize; i++)
	{
		int j;
		if(ptAnchors[i][0] < ptAnchors[i][1])
		{

			// Start of line
			for(j = ptAnchors[i][0];
				j < min(max(ptAnchors[i - 1][0], ptAnchors[i + 1][0]) + 1, ptAnchors[i][1]);
				j++)
			{
				for(int k = 0; k <= 2 * nKernelSize; k++)
				{
					UINT32 *pPixel = pShadBits +
						(szShadow.cy - i - 1 + nKernelSize - k) * szShadow.cx + j - nKernelSize;
					UINT32 *pKernelPixel = pKernel + k * (2 * nKernelSize + 1);
					for(int l = 0; l <= 2 * nKernelSize; l++)
					{
						if(*pPixel < *pKernelPixel)
							*pPixel = *pKernelPixel;
						pPixel++;
						pKernelPixel++;
					}
				}
			}	// for() start of line

			// End of line
			for(j = max(j, min(ptAnchors[i - 1][1], ptAnchors[i + 1][1]) - 1);
				j < ptAnchors[i][1];
				j++)
			{
				for(int k = 0; k <= 2 * nKernelSize; k++)
				{
					UINT32 *pPixel = pShadBits +
						(szShadow.cy - i - 1 + nKernelSize - k) * szShadow.cx + j - nKernelSize;
					UINT32 *pKernelPixel = pKernel + k * (2 * nKernelSize + 1);
					for(int l = 0; l <= 2 * nKernelSize; l++)
					{
						if(*pPixel < *pKernelPixel)
							*pPixel = *pKernelPixel;
						pPixel++;
						pKernelPixel++;
					}
				}
			}	// for() end of line

		}
	}	// for() Generate blurred border

	// Erase unwanted parts and complement missing
	UINT32 clCenter = this->m_Shadow.nDarkness << 24 | preColDark;
	for(int i = min(nKernelSize, max(this->m_Shadow.nSize - this->m_Shadow.nyOffset, 0));
		i < max(szShadow.cy - nKernelSize, min(szParent.cy + this->m_Shadow.nSize - this->m_Shadow.nyOffset, szParent.cy + 2 * this->m_Shadow.nSize));
		i++)
	{
		UINT32 *pLine = pShadBits + (szShadow.cy - i - 1) * szShadow.cx;
		if(i - this->m_Shadow.nSize + this->m_Shadow.nyOffset < 0 || i - this->m_Shadow.nSize + this->m_Shadow.nyOffset >= szParent.cy)	// Line is not covered by parent window
		{
			for(int j = ptAnchors[i][0]; j < ptAnchors[i][1]; j++)
			{
				*(pLine + j) = clCenter;
			}
		}
		else
		{
			for(int j = ptAnchors[i][0];
				j < min(ptAnchorsOri[i - this->m_Shadow.nSize + this->m_Shadow.nyOffset][0] + this->m_Shadow.nSize - this->m_Shadow.nxOffset, ptAnchors[i][1]);
				j++)
				*(pLine + j) = clCenter;
			for(int j = max(ptAnchorsOri[i - this->m_Shadow.nSize + this->m_Shadow.nyOffset][0] + this->m_Shadow.nSize - this->m_Shadow.nxOffset, 0);
				j < min(ptAnchorsOri[i - this->m_Shadow.nSize + this->m_Shadow.nyOffset][1] + this->m_Shadow.nSize - this->m_Shadow.nxOffset, szShadow.cx);
				j++)
				*(pLine + j) = 0;
			for(int j = max(ptAnchorsOri[i - this->m_Shadow.nSize + this->m_Shadow.nyOffset][1] + this->m_Shadow.nSize - this->m_Shadow.nxOffset, ptAnchors[i][0]);
				j < ptAnchors[i][1];
				j++)
				*(pLine + j) = clCenter;
		}
	}

	// Delete used resources
	delete[] (ptAnchors - (this->m_Shadow.nSize < 0 ? -this->m_Shadow.nSize : 0) - 1);
	delete[] ptAnchorsTmp;
	delete[] ptAnchorsOri;
	delete[] pKernel;
	DeleteObject(hParentRgn);
}

bool DcxDialog::SetShadowSize(int NewSize)
{
	if(NewSize > 20 || NewSize < -20)
		return false;

	this->m_Shadow.nSize = (signed char)NewSize;
	if(DCX_SS_VISABLE & m_Shadow.Status)
		UpdateShadow();
	return true;
}

bool DcxDialog::SetShadowSharpness(unsigned int NewSharpness)
{
	if(NewSharpness > 20)
		return false;

	this->m_Shadow.nSharpness = (unsigned char)NewSharpness;
	if(DCX_SS_VISABLE & this->m_Shadow.Status)
		UpdateShadow();
	return true;
}

bool DcxDialog::SetShadowDarkness(unsigned int NewDarkness)
{
	if(NewDarkness > 255)
		return false;

	this->m_Shadow.nDarkness = (unsigned char)NewDarkness;
	if(DCX_SS_VISABLE & this->m_Shadow.Status)
		UpdateShadow();
	return true;
}

bool DcxDialog::SetShadowPosition(int NewXOffset, int NewYOffset)
{
	if(NewXOffset > 20 || NewXOffset < -20 ||
		NewYOffset > 20 || NewYOffset < -20)
		return false;
	
	this->m_Shadow.nxOffset = (signed char)NewXOffset;
	this->m_Shadow.nyOffset = (signed char)NewYOffset;
	if(DCX_SS_VISABLE & this->m_Shadow.Status)
		UpdateShadow();
	return true;
}

bool DcxDialog::SetShadowColor(COLORREF NewColor)
{
	this->m_Shadow.Color = NewColor;
	if(DCX_SS_VISABLE & this->m_Shadow.Status)
		UpdateShadow();
	return true;
}
// .... CWndShadow
void DcxDialog::showError(const TCHAR *prop, const TCHAR *cmd, const TCHAR *err)
{
	if (this->IsVerbose())
	{
		TString res;
		if (prop != NULL)
			res.tsprintf(TEXT("D_IERROR xdialog(%s).%s: %s"), this->getName().to_chr(), prop, err);
		else
			res.tsprintf(TEXT("D_CERROR xdialog %s %s: %s"), cmd, this->getName().to_chr(), err);
		Dcx::mIRC.echo(res.to_chr());
	}

	this->execAliasEx(TEXT("error,0,dialog,%s,%s,%s"), (prop != NULL ? prop : TEXT("none")), (cmd != NULL ? cmd : TEXT("none")), err);
}

void DcxDialog::showErrorEx(const TCHAR *prop, const TCHAR *cmd, const TCHAR *fmt, ...)
{
	TString err;
	va_list args;

	va_start( args, fmt );
	err.tvprintf(fmt, &args);
	va_end( args );

	this->showError(prop, cmd, err.to_chr());
}

void DcxDialog::CreateVistaStyle(void)
{
#ifdef DCX_USE_WINSDK
	if (Dcx::VistaModule.refreshComposite()) {
		// Vista+ only code. dont do anything at this point if vista+

		//DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;

		////enable non-client area rendering on window
		//HRESULT hr = Dcx::VistaModule.dcxDwmSetWindowAttribute(this->m_Hwnd, DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));
		//if (SUCCEEDED(hr)) {
			//MARGINS margins = {this->m_GlassOffsets.left,this->m_GlassOffsets.right,this->m_GlassOffsets.top,this->m_GlassOffsets.bottom};
			//Dcx::VistaModule.dcxDwmExtendFrameIntoClientArea(this->m_Hwnd, &margins);
		//	//DWM_BLURBEHIND bb = {0};

		//	//// Specify blur-behind and blur region.
		//	//bb.dwFlags = DWM_BB_ENABLE;
		//	//bb.fEnable = true;
		//	//bb.hRgnBlur = NULL;

		//	//// Enable blur-behind.
		//	//Dcx::VistaModule.dcxDwmEnableBlurBehindWindow(this->m_Hwnd, &bb);

		//	//this->m_bVistaStyle = true;
		//}
		return;
	}
#endif
#ifdef DCX_USE_GDIPLUS
	// don't use this style with aero, needs specific code to allow aero to do these effects for us.
	if (Dcx::GDIModule.isUseable() && !Dcx::VistaModule.isAero()) {
		// this code is for windows 2000 & windows XP
		RECT rc;
		GetWindowRect(this->m_Hwnd, &rc);
		DWORD ExStyles = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_LEFT;
		DWORD Styles = WS_VISIBLE/*|WS_OVERLAPPED*/|WS_CLIPCHILDREN;
		SIZE szWin;
		szWin.cx = (rc.right - rc.left);
		szWin.cy = (rc.bottom - rc.top);
		this->m_hFakeHwnd = CreateWindowEx(ExStyles,DCX_VISTACLASS,NULL,Styles,rc.left,rc.top,szWin.cx,szWin.cy,this->m_Hwnd,NULL,GetModuleHandle(NULL), NULL);
		if (IsWindow(this->m_hFakeHwnd)) {
			if (this->CreateVistaStyleBitmap(szWin))
			{
				SetLayeredWindowAttributes(this->m_Hwnd,0,5,LWA_ALPHA);
				HRGN hRgn = CreateRectRgn(0,0,0,0);
				if (GetWindowRgn(this->m_Hwnd, hRgn))
					SetWindowRgn(this->m_hFakeHwnd, hRgn, TRUE);
				else
					DeleteRgn(hRgn);
				this->m_bVistaStyle = true;
			}
			else
				DestroyWindow(this->m_hFakeHwnd);
		}
	}
#endif
}

bool DcxDialog::CreateVistaStyleBitmap(const SIZE &szWin)
{
#ifdef DCX_USE_GDIPLUS
	if (this->m_hVistaBitmap != NULL)
		DeleteObject(this->m_hVistaBitmap);

	BITMAPINFOHEADER bmih = { 0 };
	int nBytesPerLine = ((szWin.cx * 32 + 31) & (~31)) >> 3;
	// Populate BITMAPINFO header
	bmih.biSize=sizeof(BITMAPINFOHEADER);
	bmih.biWidth=szWin.cx;
	bmih.biHeight=-szWin.cy; // make this a top down bitmap
	bmih.biPlanes=1;
	bmih.biBitCount=32;
	bmih.biCompression=BI_RGB;
	bmih.biClrUsed=0;
	bmih.biSizeImage=nBytesPerLine*szWin.cy;

	this->m_hVistaBitmap = ::CreateDIBSection(NULL, (PBITMAPINFO)&bmih, DIB_RGB_COLORS, &this->m_pVistaBits, NULL, 0);
	if (this->m_hVistaBitmap != NULL) {
		memset( this->m_pVistaBits, 0, szWin.cx * 4 * szWin.cy);
		return true;
	}
#endif
	return false;
}

void DcxDialog::RemoveVistaStyle(void)
{
	this->m_bVistaStyle = false;
	if (IsWindow(this->m_hFakeHwnd)) {
		if (this->isExStyle(WS_EX_LAYERED))
			SetLayeredWindowAttributes(this->m_Hwnd,0,this->m_iAlphaLevel,LWA_ALPHA);
		DestroyWindow(this->m_hFakeHwnd);
	}
	if (this->m_hVistaBitmap != NULL)
		DeleteBitmap(this->m_hVistaBitmap);
	this->m_hFakeHwnd = NULL;
}

#ifdef DCX_USE_GDIPLUS
void DcxDialog::DrawCaret(Graphics & graph)
{
	HWND pWnd = GetFocus();
	if( pWnd == NULL || !IsWindow(pWnd) )
		return;

	TCHAR tszBuf[MAX_PATH] = {TEXT('\0')};
	::GetClassName( pWnd, tszBuf, MAX_PATH);
	TString strClassName(tszBuf);

	if( strClassName != TEXT("EDIT"))
		return;

	POINT pt;
	GetCaretPos(&pt);
	MapWindowPoints(pWnd, this->m_hFakeHwnd, &pt, 1);

	Pen pen( Color::Black, 1.0f);
	graph.DrawLine( &pen, pt.x, pt.y, pt.x, pt.y + 13);
}

void DcxDialog::DrawDialog( Graphics & graphics, HDC hDC)
{
	RECT rc;
	GetWindowRect(this->m_Hwnd, &rc);

	HDC hdcMemory = ::CreateCompatibleDC(hDC);
	if (hdcMemory != NULL) {
		int w = (rc.right - rc.left), h = (rc.bottom - rc.top);
		HBITMAP hBitmap = ::CreateCompatibleBitmap( hDC, w, h);
		if (hBitmap != NULL) {
			HGDIOBJ hbmpOld = ::SelectObject( hdcMemory, hBitmap);

			::SendMessage(this->m_Hwnd, WM_PRINT, (WPARAM)hdcMemory,PRF_CLIENT|PRF_NONCLIENT|PRF_ERASEBKGND);

			Bitmap bitmap( hBitmap, NULL);
			graphics.DrawImage( &bitmap, 0, 0);

			::DeleteObject(::SelectObject( hdcMemory, hbmpOld));
		}
		::DeleteDC(hdcMemory);
	}
}

void DcxDialog::DrawCtrl( Graphics & graphics, HDC hDC, HWND hWnd)
{
	if( !::IsWindow(hWnd) || !::IsWindowVisible(hWnd) )
		return;

	RECT rc;
	GetWindowRect(hWnd, &rc);
	this->MapVistaRect(NULL, &rc);

	int w = (rc.right - rc.left), h = (rc.bottom - rc.top);

	// Don't bother drawing if its not visible in clip rect.
	if (!graphics.IsVisible(rc.left, rc.top, w, h))
		return;

	HDC hdcMemory = ::CreateCompatibleDC(hDC);
	if (hdcMemory != NULL) {
		HBITMAP hBitmap = ::CreateCompatibleBitmap( hDC, w, h);
		if (hBitmap != NULL) {
			HBITMAP hbmpOld = SelectBitmap( hdcMemory, hBitmap);

			::SendMessage( hWnd, WM_PRINT, (WPARAM)hdcMemory, (LPARAM)PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE | PRF_ERASEBKGND);

			Bitmap bitmap( hBitmap, NULL);
			graphics.DrawImage( &bitmap, rc.left, rc.top);

			DeleteBitmap(SelectBitmap( hdcMemory, hbmpOld));
		}
		::DeleteDC(hdcMemory);
	}
}
#endif

void DcxDialog::UpdateVistaStyle(const LPRECT rcUpdate)
{
#ifdef DCX_USE_GDIPLUS
	if (!IsWindow(this->m_hFakeHwnd) || !IsWindowVisible(this->m_Hwnd) || IsIconic(this->m_Hwnd) || this->m_hVistaBitmap == NULL)
		return;

	{ // maintain a matching region.
		HRGN hRgn = CreateRectRgn(0,0,0,0);
		if (GetWindowRgn(this->m_Hwnd, hRgn))
		{
			HRGN hFakeRgn = CreateRectRgn(0,0,0,0);
			if (GetWindowRgn(this->m_hFakeHwnd, hFakeRgn))
			{
				if (!EqualRgn(hRgn, hFakeRgn))
					SetWindowRgn(this->m_hFakeHwnd, hRgn, FALSE);
				else
					DeleteRgn(hRgn);
			}
			else
				SetWindowRgn(this->m_hFakeHwnd, hRgn, FALSE);
			DeleteRgn(hFakeRgn);
		}
		else
			DeleteRgn(hRgn);
	}
	RECT rc;
	::GetWindowRect( this->m_hFakeHwnd, &rc);
	POINT ptSrc = { 0, 0};
	POINT ptWinPos = { rc.left, rc.top};
	SIZE szWin = { (rc.right - rc.left), (rc.bottom - rc.top) };

	BYTE alpha = this->m_iAlphaLevel;
	if (this->m_bGhosted)
		alpha = this->m_bDoGhostDrag;

	BYTE half_alpha = (BYTE)(alpha / 2);

	BLENDFUNCTION stBlend = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };

	HDC hDC = ::GetDC(this->m_hFakeHwnd);
	HDC hdcMemory = ::CreateCompatibleDC(hDC);

	PVOID pvBits = this->m_pVistaBits;
	HBITMAP hbmpMem = this->m_hVistaBitmap;
	if(hbmpMem)
	{
		HBITMAP hbmpOld = SelectBitmap( hdcMemory, hbmpMem);

		this->m_hVistaHDC = hdcMemory;

		Graphics graph(hdcMemory);
		graph.SetPageScale(1.0);
		graph.SetPageUnit(UnitPixel);
		graph.SetSmoothingMode(SmoothingModeNone);

		RECT rcParentWin, rcParentClient, rcClip, glassOffsets;
		SIZE offsets;
		POINT pt;

		SetRectEmpty(&rcClip);
		GetWindowRect(this->m_Hwnd, &rcParentWin);
		GetClientRect(this->m_Hwnd, &rcParentClient);
		MapWindowRect(this->m_Hwnd, NULL, &rcParentClient);
		offsets.cx = (rcParentClient.left - rcParentWin.left);
		offsets.cy = (rcParentClient.top - rcParentWin.top);
		this->m_sVistaOffsets = offsets;
		// Glass area = window edge +/- offset
		glassOffsets.left = offsets.cx + this->m_GlassOffsets.left;
		glassOffsets.top = offsets.cy + this->m_GlassOffsets.top;
		glassOffsets.right = szWin.cx - ((rcParentWin.right - rcParentClient.right) + this->m_GlassOffsets.right);
		glassOffsets.bottom = szWin.cy - ((rcParentWin.bottom - rcParentClient.bottom) + this->m_GlassOffsets.bottom);

		// Check for update rect (area of child control in screen coordinates)
		// If found set clipping area as child control's area.
		if (rcUpdate != NULL) {
			CopyRect(&rcClip, rcUpdate);
			OffsetRect(&rcClip, -rcParentWin.left, -rcParentWin.top);
			Rect clipRect(rcClip.left, rcClip.top , (rcClip.right - rcClip.left), (rcClip.bottom - rcClip.top));
			graph.SetClip(clipRect);
			DrawDialog( graph, hDC); // draw dialog after setting update controls clip rect.
		}
		else {
			// otherwise set clipping area to client area.
			rcClip.right = szWin.cx;
			rcClip.bottom = szWin.cy;
			DrawDialog( graph, hDC); // draw dialog before setting client area clip rect.
			Rect clipRect(offsets.cx, offsets.cy, (rcParentClient.right - rcParentClient.left), (rcParentClient.bottom - rcParentClient.top));
			graph.SetClip(clipRect);
		}

		// Alpha Glass area when not ghost dragging & alpha isnt being set for whole dialog.
		// This method allows the glass area to be translucent but the controls to still be solid.
		// Update: commented out ghost & alpha check to allow glass area to still be seen when dialog as a whole is translucent
		//	make this behaviour optional?
		//if (!this->m_bGhosted && alpha == 255) {
			GdiFlush(); // sync drawing
			int Yend = min(rcClip.bottom, szWin.cy), Ybase = max(rcClip.top, 0);
			int Xend = min(rcClip.right, szWin.cx), Xbase = max(rcClip.left, 0);
			for( int y = Ybase; y < Yend; y++)
			{
				// (szWin.cx * 4 * y) == row
				// (Xbase * 4) == offset within row.
				PBYTE pPixel = ((PBYTE)pvBits) + (szWin.cx * 4 * y) + (Xbase * 4);

				pt.y = y; //szWin.cy - y;

				for( int x = Xbase; x < Xend; x++)
				{
					pt.x = x;
					//if (this->m_bHaveKeyColour) {
					//	if ((pPixel[0] == GetRValue(this->m_cKeyColour)) && (pPixel[1] == GetGValue(this->m_cKeyColour)) && (pPixel[2] == GetBValue(this->m_cKeyColour))) {
					//		pPixel[3] = 0;
					//	}
					//}
					if (!PtInRect(&glassOffsets,pt)) {
						pPixel[3] = half_alpha; // set glass area as 50% (0x7f) transparent

						pPixel[0] = (BYTE)(pPixel[0] * pPixel[3] / 255);
						pPixel[1] = (BYTE)(pPixel[1] * pPixel[3] / 255);
						pPixel[2] = (BYTE)(pPixel[2] * pPixel[3] / 255);
					}

					pPixel += 4;
				}
			}
		//}

		// Draw all the controls
		HWND hwndChild = ::GetWindow( this->m_Hwnd, GW_CHILD);
		while(hwndChild)
		{
			DrawCtrl( graph, hDC, hwndChild);
			hwndChild = ::GetWindow( hwndChild, GW_HWNDNEXT);
		}

		// draw the caret
		DrawCaret(graph);

		// The below loops are replaced by setting the constant alpha in the blend function. DONT REMOVE LOOPS.
		//// Alpha when ghost dragging or alpha is set for whole dialog.
		//if( alpha >= 0 && alpha < 255 )
		//{
		//	for( int y = 0; y < szWin.cy; y++)
		//	{
		//		PBYTE pPixel = ((PBYTE)pvBits) + szWin.cx * 4 * y;

		//		for( int x = 0; x < szWin.cx; x++)
		//		{
		//			if( alpha < pPixel[3] )
		//			{
		//				pPixel[3] = alpha;

		//				pPixel[0] = pPixel[0] * pPixel[3] / 255;
		//				pPixel[1] = pPixel[1] * pPixel[3] / 255;
		//				pPixel[2] = pPixel[2] * pPixel[3] / 255;
		//			}

		//			pPixel += 4;
		//		}
		//	}
		//}

		// NB: Unable to combine ULW_COLORKEY & ULW_ALPHA for some reason...
		UpdateLayeredWindow( this->m_hFakeHwnd, hDC, &ptWinPos, &szWin, hdcMemory, &ptSrc, 0, &stBlend, ULW_ALPHA);

		this->m_hVistaHDC = NULL;

		graph.ReleaseHDC(hdcMemory);
		SelectBitmap( hdcMemory, hbmpOld);
	}

	::DeleteDC(hdcMemory);
	::DeleteDC(hDC);
#endif
}

void DcxDialog::SetVistaStylePos(void)
{
	if (!IsWindow(this->m_hFakeHwnd))
		return;

	RECT rc;
	GetWindowRect(this->m_Hwnd, &rc);
	SetWindowPos(this->m_hFakeHwnd, NULL, rc.left, rc.top, 0,0, SWP_NOSIZE|SWP_NOZORDER);
}

void DcxDialog::SetVistaStyleSize(void)
{
	if (!IsWindow(this->m_hFakeHwnd))
		return;

	RECT rc;
	GetWindowRect(this->m_Hwnd, &rc);
	SIZE szWin;
	szWin.cx = (rc.right - rc.left);
	szWin.cy = (rc.bottom - rc.top);
	this->CreateVistaStyleBitmap(szWin);
	SetWindowPos(this->m_hFakeHwnd, NULL, 0,0, szWin.cx, szWin.cy, SWP_NOMOVE|SWP_NOZORDER);
}

void DcxDialog::MapVistaRect(HWND hwnd, LPRECT rc) const
{
	MapWindowRect(hwnd, this->m_Hwnd, rc);
	OffsetRect(rc, this->m_sVistaOffsets.cx, this->m_sVistaOffsets.cy);
}

// Adds the control to the list
void DcxDialog::RegisterDragList(DcxList* list)
{
   this->m_vDragLists.push_back(list);
}

// Removes the control from the list
void DcxDialog::UnregisterDragList(DcxList* list)
{
   VectorOfDragListPtrs::iterator itStart = this->m_vDragLists.begin();
   VectorOfDragListPtrs::iterator itEnd = this->m_vDragLists.end();

   while (itStart != itEnd) {
      if (*itStart == list) {
         m_vDragLists.erase(itStart);
         return;
      }

      itStart++;
   }
}

// Checks if the message should be parsed
LRESULT DcxDialog::ProcessDragListMessage(DcxDialog* p_this, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed)
{
   VectorOfDragListPtrs::iterator itStart = p_this->m_vDragLists.begin();
   VectorOfDragListPtrs::iterator itEnd = p_this->m_vDragLists.end();
   DcxList* list;

   while (itStart != itEnd)
   {
      list = *itStart;

      if (list->getDragListId() == uMsg)
         return list->ParentMessage(uMsg, wParam, lParam, bParsed);

      itStart++;
   }

   return 0L;
}

void DcxDialog::toXml(TiXmlElement * xml) {
	this->toXml(xml, TEXT("unnamed"));
}

void DcxDialog::toXml(TiXmlElement * xml, const TString &name) {
	// NEEDS FIXED!
	//TString dest;
	//TGetWindowText(this->m_Hwnd, dest);
	//xml->SetAttribute("name", name.to_chr());
	//xml->SetAttribute("caption", dest.to_chr());
	//this->m_pLayoutManager->getRoot()->toXml(xml);
}

TiXmlElement * DcxDialog::toXml() {
	TiXmlElement * result = new TiXmlElement("dialog");
	this->toXml(result);
	return result;
}

TiXmlElement * DcxDialog::toXml(const TString & name) {
	TiXmlElement * result = new TiXmlElement("dialog");
	this->toXml(result, name);
	return result;
}



//#ifdef DCX_USE_GDIPLUS
//bool DcxDialog::LoadGDIPlusImage(TString &filename) {
//	if (!IsFile(filename)) {
//		this->showError(NULL,TEXT("LoadGDIPlusImage"), TEXT("Unable to open file"));
//		return false;
//	}
//	this->m_pImage = new Image(filename.to_wchr(),TRUE);
//
//	// couldnt allocate image object.
//	if (this->m_pImage == NULL) {
//		this->showError(NULL,TEXT("LoadGDIPlusImage"), TEXT("Couldn't allocate image object."));
//		return false;
//	}
//	// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
//	Status status = this->m_pImage->GetLastStatus();
//	if (status != Ok) {
//		this->showError(NULL,TEXT("LoadGDIPlusImage"), GetLastStatusStr(status));
//		PreloadData();
//		return false;
//	}
//	return true;
//}
//#endif
