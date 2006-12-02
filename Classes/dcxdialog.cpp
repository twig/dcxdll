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
//
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
// This code has been modified for use with DCX, & is used for the DcxDialog Shadows.
// .... CWndShadow

#include "../defines.h"
#include "dcxdialog.h"
#include "dcxdialogcollection.h"
#include "dcxcontrol.h"

#include "dcxtoolbar.h"
#include "dcxrichedit.h"

#include "layout/layoutcellfixed.h"
#include "layout/layoutcellfill.h"
#include "layout/layoutcellpane.h"

#include "tinyxml/tinyxml.h"

#include "math.h"

extern DcxDialogCollection Dialogs;

extern mIRCDLL mIRCLink;
/*!
 * walkScript is a recursive function for dcXML triggered in /xdialog -X
 * Feel free to put this elsewhere if more suited as long as it wont break it :)
 * (Mpdreamz)
 */

void walkScript(TiXmlElement* element, char *dname, int depth=0) {  
	TiXmlElement* child = 0;  
	TString cmd;
	for( child = element->FirstChildElement(); child; child = child->NextSiblingElement() ) {  
		TiXmlElement* parent = child->Parent()->ToElement();
		const char type = 
		cmd.sprintf("/echo -a Element %s %s %i parent: %s",
			child->Value(),child->Attribute("type"),depth,parent->Value());
		mIRCcom(cmd.to_chr());
		walkScript(child->ToElement(), dname, depth+1);  
	}  
} 


/*!
 * \brief Constructor
 *
 * \param mHwnd Dialog Window Handle
 * \param tsName Dialog Name
 * \param tsAliasName Dialog Callback alias Name
 */


DcxDialog::DcxDialog(const HWND mHwnd, TString &tsName, TString &tsAliasName)
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
, m_dEventMask(-1)
, m_bTracking(FALSE)
, m_bDoGhostDrag(255)
, m_bGhosted(false)
{
	this->addStyle(WS_CLIPCHILDREN);

	this->m_hOldWindowProc = (WNDPROC) SetWindowLong(this->m_Hwnd, GWL_WNDPROC, (LONG) DcxDialog::WindowProc);

	this->m_pLayoutManager = new LayoutManager(this->m_Hwnd);

	this->m_Shadow.hWin = NULL;
	this->m_Shadow.Status = 0;

	SetProp(this->m_Hwnd, "dcx_this", (HANDLE) this);

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

	if (this->m_bitmapBg)
		DeleteObject(m_bitmapBg);
	this->RemoveShadow();

	RemoveProp(this->m_Hwnd, "dcx_this");
}

/*!
 * \brief blah
 *
 * blah
 */
TString DcxDialog::getName() const {
	return this->m_tsName;
}

/*!
 * \brief blah
 *
 * blah
 */

TString DcxDialog::getAliasName() const {
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
  char error[500];
  wsprintf( error, "%d Controls", this->m_vpControls.size( ) );
  mIRCError( error );

  VectorOfControlPtrs::iterator it;
  DcxControl * p_Control;

  
  while ( ( p_Control = this->m_vpControls.pop_back( ) ) != NULL ) {

    wsprintf( error, "ID %d %d", p_Control->getUserID( ), this->m_vpControls.size( ) );
    mIRCError( error );

    if ( p_Control != NULL ) {

      if ( p_Control->getType( ) == "window" || p_Control->getType( ) == "dialog" )
        delete p_Control;
      else
        DestroyWindow( p_Control->getHwnd( ) );
    }
  }
  this->m_vpControls.clear( );
  

  wsprintf( error, "%d Controls", this->m_vpControls.size( ) );
  mIRCError( error );
  */
}

/*!
 * \brief blah
 *
 * blah
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

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::parseCommandRequest(TString &input) {
	XSwitchFlags flags;

	ZeroMemory((void*) &flags, sizeof(XSwitchFlags));
	this->parseSwitchFlags(input.gettok(2, " "), &flags);

	int numtok = input.numtok(" ");

	// xdialog -a [NAME] [SWITCH] [+FLAGS] [DURATION]
	if (flags.switch_flags[0] && numtok > 3) {
		AnimateWindow(this->m_Hwnd,
			input.gettok(4, " ").to_int(), 
			getAnimateStyles(input.gettok(3, " ")));

		if (IsWindowVisible(this->m_Hwnd))
			this->redrawWindow();
	}
	// xdialog -b [NAME] [SWITCH] [+FLAGS]
	else if (flags.switch_flags[1] && numtok > 2) {
		this->removeStyle(WS_BORDER | WS_DLGFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX |
			WS_SYSMENU | WS_SIZEBOX | WS_CAPTION);

		this->removeExStyle(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_CONTEXTHELP |
			WS_EX_TOOLWINDOW | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE);

		LONG Styles = 0, ExStyles = 0;

		this->parseBorderStyles(input.gettok(3, " "), &Styles, &ExStyles);
		this->addStyle(Styles);
		this->addExStyle(ExStyles);

		SetWindowPos(this->m_Hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		InvalidateRect(this->m_Hwnd, NULL, TRUE);
		SendMessage(this->m_Hwnd, WM_NCPAINT, (WPARAM) 1, (LPARAM) 0);
	}
	// xdialog -c [NAME] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	else if (flags.switch_flags[2] && numtok > 7) {
		UINT ID = mIRC_ID_OFFSET + input.gettok(3, " ").to_int();

		if ((IsWindow(GetDlgItem(this->m_Hwnd, ID)) == FALSE) && 
			(ID > mIRC_ID_OFFSET - 1) && (this->getControlByID(ID) == NULL))
		{
			DcxControl *p_Control = DcxControl::controlFactory(this, ID, input,4);

			if (p_Control != NULL)
				this->addControl(p_Control);
		}
		else {
			TString error;
			error.sprintf("Control with ID \"%d\" already exists", ID - mIRC_ID_OFFSET);
			DCXError("/xdialog -c",error.to_chr());
		}
	}
	// xdialog -d [NAME] [SWITCH] [ID]
	else if (flags.switch_flags[3] && numtok > 2) {
		/*
		if ( input.gettok( 3, " " ) == "*" ) { 

		this->deleteAllControls( );
		}
		else {
		*/

		UINT ID = mIRC_ID_OFFSET + input.gettok(3, " ").to_int();
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

			if ((p_Control->getType() == "dialog") || (p_Control->getType() == "window"))
				delete p_Control;
			else if (p_Control->getRefCount() == 0) {
				this->deleteControl(p_Control); // remove control from internal list!
				DestroyWindow(cHwnd);
			}
			else {
				TString error;
				error.sprintf("Can't delete control with ID \"%d\" when it is inside it's own event (dialog %s)", p_Control->getUserID(), this->m_tsName.to_chr());
				DCXError("/xdialog -d",error.to_chr());
			}
		}
		// unknown control
		else {
			TString error;
			error.sprintf("Unknown control with ID \"%d\" (dialog %s)", ID - mIRC_ID_OFFSET, this->m_tsName.to_chr());
			DCXError("/xdialog -d",error.to_chr());
		}
	}
	// xdialog -f [NAME] [SWITCH] [+FLAGS] [COUNT] [TIMEOUT]
	else if (flags.switch_flags[5] && numtok > 4) {
		UINT iFlags = this->parseFlashFlags(input.gettok(3, " "));
		INT iCount = input.gettok(4, " ").to_int();
		DWORD dwTimeout = (DWORD)input.gettok(5, " ").to_num();
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
	else if (flags.switch_flags[6] && numtok > 3) {
		this->m_uStyleBg = this->parseBkgFlags(input.gettok(3, " "));

		if (this->m_uStyleBg & DBS_BKGCOLOR) {
			COLORREF clrColor = (COLORREF)input.gettok(4, " ").to_num();

			if (this->m_hBackBrush != NULL) {
				DeleteObject(this->m_hBackBrush);
				this->m_hBackBrush = NULL;
			}

			if (clrColor != -1)
				this->m_hBackBrush = CreateSolidBrush(clrColor);
		}
		else if (this->m_uStyleBg & DBS_BKGBITMAP) {
			if (this->m_bitmapBg) {
				DeleteObject(this->m_bitmapBg);
				this->m_bitmapBg = NULL;
			}

			TString filename(input.gettok(4, -1, " "));
			filename.trim();

			if (filename != "none")
				this->m_bitmapBg = dcxLoadBitmap(this->m_bitmapBg, filename);
		}

		//InvalidateRect(this->m_Hwnd, NULL, TRUE);
		this->redrawWindow();
	}
	// xdialog -j [NAME] [SWITCH] (ID)
	else if (flags.switch_flags[9]) {
		if (numtok > 2) {
			UINT id = mIRC_ID_OFFSET + input.gettok(3, " ").to_int();
			DcxControl * p_Control;

			if ((id > mIRC_ID_OFFSET - 1) &&
				IsWindow(GetDlgItem(this->m_Hwnd, id)) &&
				((p_Control = this->getControlByID(id)) != NULL))
			{
				p_Control->redrawWindow();
			}
			else {
				TString error;
				error.sprintf("Could not find control %d", id - mIRC_ID_OFFSET);
				DCXError("/xdialog -j",error.to_chr());
			}

			return;
		}
		this->redrawWindow();
	}
	/*
	//xdialog -l [NAME] [SWITCH] [OPTIONS]

	[OPTIONS] :

	root [TAB]+flpiw [ID] [WEIGHT] [W] [H]
	add PATH[TAB]+flpiw [ID] [WEIGHT] [W] [H]
	space PATH[TAB]+ [L] [T] [R] [B]
	*/
	else if (flags.switch_flags[11] && numtok > 2) {
		if (input.gettok(3, " ") == "update") {
			if (this->m_pLayoutManager != NULL) {
				RECT rc;

				GetClientRect(this->m_Hwnd, &rc);
				this->m_pLayoutManager->updateLayout(rc);
			}
			this->redrawWindow();
		}
		else if (input.gettok(3, " ") == "clear") {
			if (this->m_pLayoutManager != NULL)
				delete this->m_pLayoutManager;
			this->m_pLayoutManager = new LayoutManager(this->m_Hwnd);
			//this->redrawWindow(); // dont redraw here, leave that for an `update` cmd
		}
		else if (numtok > 7) {
			TString com(input.gettok(1, "\t").gettok(3, " "));
			TString path(input.gettok(1, "\t").gettok(4, -1, " "));
			TString p2(input.gettok(2, "\t"));

			com.trim();
			path.trim();
			p2.trim();

			UINT flags = this->parseLayoutFlags(p2.gettok(1, " "));
			UINT ID = p2.gettok(2, " ").to_int();
			UINT WGT = p2.gettok(3, " ").to_int();
			UINT W = p2.gettok(4, " ").to_int();
			UINT H = p2.gettok(5, " ").to_int();

			if (com == "root" || com == "cell") {
				HWND cHwnd = GetDlgItem(this->m_Hwnd, mIRC_ID_OFFSET + ID);
				LayoutCell * p_Cell = NULL;

				// LayoutCellPane
				if (flags & LAYOUTPANE) {
					if (flags & LAYOUTHORZ)
						p_Cell = new LayoutCellPane(LayoutCellPane::HORZ);
					else
						p_Cell = new LayoutCellPane(LayoutCellPane::VERT);
				} // if ( flags & LAYOUTPANE )
				// LayoutFill Cell
				else if (flags & LAYOUTFILL) {
					if (flags & LAYOUTID) {
						if (cHwnd != NULL && IsWindow(cHwnd))
							p_Cell = new LayoutCellFill(cHwnd);
						else {
							TString error;
							error.sprintf("Cell Fill -> Invalid ID : %d", ID);
							DCXError("/xdialog -l",error.to_chr());
							return;
						}
					}
					else {
						p_Cell = new LayoutCellFill();
					}
				} // else if ( flags & LAYOUTFILL )
				// LayoutCellFixed
				else if (flags & LAYOUTFIXED) {
					LayoutCellFixed::FixedType type;

					if (flags & LAYOUTVERT && flags & LAYOUTHORZ)
						type = LayoutCellFixed::BOTH;
					else if (flags & LAYOUTVERT)
						type = LayoutCellFixed::HEIGHT;
					else
						type = LayoutCellFixed::WIDTH;

					// Defined Rectangle
					if (flags & LAYOUTDIM) {
						RECT rc;
						SetRect(&rc, 0, 0, W, H);

						if (flags & LAYOUTID) {
							if (cHwnd != NULL && IsWindow(cHwnd))
								p_Cell = new LayoutCellFixed(cHwnd, rc, type);
							else {
								TString error;
								error.sprintf("Cell Fixed -> Invalid ID : %d", ID);
								DCXError("/xdialog -l",error.to_chr());
								return;
							}
						}
						else
							p_Cell = new LayoutCellFixed(rc, type);
					}
					// No defined Rectangle
					else {
						if (flags & LAYOUTID) {
							if (cHwnd != NULL && IsWindow(cHwnd))
								p_Cell = new LayoutCellFixed(cHwnd, type);
							else {
								TString error;
								error.sprintf("Cell Fixed -> Invalid ID : %d", ID);
								DCXError("/xdialog -l",error.to_chr());
								return;
							}
						}
					} //else
				} // else if ( flags & LAYOUTFIXED )
				else {
					DCXError("/xdialog -l","Unknown Cell Type");
					return;
				}

				if (com == "root") {
					if (p_Cell != NULL)
						this->m_pLayoutManager->setRoot(p_Cell);
				} // if ( com == "root" )
				else if (com == "cell") {
					if (p_Cell != NULL) {
						LayoutCell * p_GetCell;

						if (path == "root")
							p_GetCell = this->m_pLayoutManager->getRoot();
						else
							p_GetCell = this->m_pLayoutManager->getCell(path);

						if (p_GetCell == NULL) {
							TString error;
							error.sprintf("Invalid item path: %s", path.to_chr());
							DCXError("/xdialog -l",error.to_chr());
							return;
						}

						if (p_GetCell->getType() == LayoutCell::PANE) {
							LayoutCellPane *p_PaneCell = (LayoutCellPane*) p_GetCell;
							p_PaneCell->addChild(p_Cell, WGT);
						}
					}
				} // else if ( com == "cell" )
			} // if ( com ==  "root" || com == "cell" )
			else if (com ==  "space") {
				LayoutCell * p_GetCell;

				if (path == "root")
					p_GetCell = this->m_pLayoutManager->getRoot();
				else
					p_GetCell = this->m_pLayoutManager->getCell(path);

				if (p_GetCell != NULL) {
					RECT rc;

					SetRect(&rc, ID, WGT, W, H);
					p_GetCell->setBorder(rc);
				}
			} // else if ( com == "space" )
		} // if ( numtok > 7 )
	}
	// xdialog -q [NAME] [SWITCH] [+FLAGS] [CURSOR|FILENAME]
	else if (flags.switch_flags[16] && numtok > 3) {
		if (this->m_bCursorFromFile) {
			DeleteObject(this->m_hCursor);
			this->m_hCursor = NULL;
			this->m_bCursorFromFile = FALSE;
		}
		else
			this->m_hCursor = NULL;

		UINT iFlags = this->parseCursorFlags(input.gettok(3, " "));

		if (iFlags & DCCS_FROMRESSOURCE)
			this->m_hCursor = LoadCursor(NULL, this->parseCursorType(input.gettok(4, " ")));
		else if (iFlags & DCCS_FROMFILE) {
			this->m_hCursor = LoadCursorFromFile(input.gettok(4, -1, " ").to_chr());
			this->m_bCursorFromFile = TRUE;
		}
	}
	// xdialog -x [NAME] [SWITCH]
	else if (flags.switch_flags[23]) {
		if (this->getRefCount() == 0)
			DestroyWindow(this->m_Hwnd);
		else {
			TString cmd;

			cmd.sprintf("/.timer -m 1 0 xdialog -x %s", this->getName().to_chr());
			mIRCcom(cmd.to_chr());
		}
	}
	// xdialog -h [NAME] [SWITCH]
	else if (flags.switch_flags[7]) {
		ShowWindow(this->m_Hwnd, SW_HIDE);
	}
	// xdialog -m [NAME] [SWITCH]
	else if (flags.switch_flags[12]) {
		ShowWindow(this->m_Hwnd, SW_MAXIMIZE);
	}
	// xdialog -n [NAME] [SWITCH]
	else if (flags.switch_flags[13]) {
		ShowWindow(this->m_Hwnd, SW_MINIMIZE);
	}
	// xdialog -r [NAME] [SWITCH]
	else if (flags.switch_flags[17]) {
		ShowWindow(this->m_Hwnd, SW_RESTORE);
	}
	// xdialog -s [NAME] [SWITCH]
	else if (flags.switch_flags[18]) {
		ShowWindow(this->m_Hwnd, SW_SHOW);
	}
	// xdialog -t [NAME] [SWITCH] [COLOR]
	else if (flags.switch_flags[19] && numtok > 2) {
		if (input.gettok(3, " ") == "alpha") {
			// Set WS_EX_LAYERED on this window
			SetWindowLong(this->m_Hwnd, GWL_EXSTYLE, GetWindowLong(this->m_Hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);

			// Make this window x% alpha
			SetLayeredWindowAttributes(this->m_Hwnd, 0, (255 * input.gettok(4, " ").to_int()) / 100, LWA_ALPHA);
		}
		else if (input.gettok(3, " ") == "transparentcolor") {
			// Set WS_EX_LAYERED on this window
			SetWindowLong(this->m_Hwnd, GWL_EXSTYLE, GetWindowLong(this->m_Hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);

			// Make colour transparent
			SetLayeredWindowAttributes(this->m_Hwnd, input.gettok(4, " ").to_int(), 0, LWA_COLORKEY);
		}
		else if (input.gettok(3, " ") == "bgcolor") {
			this->m_colTransparentBg = input.gettok(3, " ").to_int();
		}
		else {
			DCXError("/xdialog -t","Unknown Switch");
			return;
		}
		this->redrawWindow();
	}
	// xdialog -T [NAME] [SWITCH] [FLAGS] [STYLES]
	else if (flags.switch_cap_flags[19] && numtok > 2) {
		if (this->m_ToolTipHWND != NULL) {
			DCXError("/xdialog -T","Tooltip already exists. Cannot recreate");
			return;
		}

		UINT styles = parseTooltipFlags(input.gettok(3, " "));

		// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/tooltip/styles.asp
		this->m_ToolTipHWND = CreateWindowEx(WS_EX_TOPMOST,
			TOOLTIPS_CLASS, NULL,
			styles,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			this->m_Hwnd,
			NULL, GetModuleHandle(NULL), NULL);
	}
	// xdialog -w [NAME] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags.switch_flags[22] && numtok > 4) {
		TString flags(input.gettok(3, " "));
		int index = input.gettok(4, " ").to_int();
		TString filename(input.gettok(5, -1, " "));
		filename.trim();
		ChangeHwndIcon(this->m_Hwnd,&flags,index,&filename);
	}
	// xdialog -z [NAME] [SWITCH] [COLOR]
	else if (flags.switch_flags[25] && numtok > 2) {
		mIRCError("-z");

		/*
		/xdialog -z [dname] [+flags] [groupname] (controlid)

		group manager
		- list of groups (vector)
		- add group (by string)
		- remove group (by string)

		+c create new group
		+c [group]

		+a add new control to group
		+a [group] [id]

		+s show control in group
		+s [group] [id]

		+d remove control from group
		+d [group] [id]

		+k clear controls in group
		+k [group]

		+r remove group
		+r [group]


		group requires (all by id)
		- list of controls in group (vector)
		- currently visible control (id)
		- name of group (strign)

		- show control in group (by id)
		- add controls to groups (by id)
		- remove controls from gorups (by id)
		*/
	}
	// xdialog -R [NAME] [SWITCH] [FLAG] [ARGS]
	else if (flags.switch_cap_flags[17] && numtok > 2) {
		TString flag(input.gettok(3," "));

		if ((flag.len() < 2) || (flag[0] != '+')) {
			DCXError("/xdialog -R","Invalid Flag");
			return;
		}

		RECT rc;
		GetWindowRect(this->m_Hwnd,&rc);

		HRGN m_Region = NULL;
		int RegionMode = 0;
		bool noRegion = false;

		if (flag.find('o',0))
			RegionMode = RGN_OR;
		else if (flag.find('a',0))
			RegionMode = RGN_AND;
		else if (flag.find('i',0))
			RegionMode = RGN_DIFF;
		else if (flag.find('x',0))
			RegionMode = RGN_XOR;

		// image file - [COLOR] [FILE]
		if (flag.find('f',0))
		{
			if (numtok < 5) {
				DCXError("/xdialog -R","Invalid arguments for +f flag");
				return;
			}

			//SetWindowRgn(this->m_Hwnd,NULL,TRUE);
			this->m_colTransparentBg = (COLORREF)input.gettok(4," ").to_num();
			//this->m_uStyleBg = DBS_BKGBITMAP|DBS_BKGSTRETCH|DBS_BKGCENTER;
			this->m_uStyleBg = DBS_BKGBITMAP;
			this->m_bitmapBg = dcxLoadBitmap(this->m_bitmapBg,input.gettok(5,-1," "));

			if (this->m_bitmapBg != NULL)
				m_Region = BitmapRegion(this->m_bitmapBg,this->m_colTransparentBg,TRUE);
		}
		else if (flag.find('r',0)) // rounded rect - radius args (optional)
		{
			int radius;

			if (numtok > 3)
				radius = input.gettok(4, " ").to_int();
			else
				radius = 20;

			m_Region = CreateRoundRectRgn(0,0,rc.right - rc.left,rc.bottom - rc.top, radius, radius);
		}
		else if (flag.find('c',0)) // circle - radius arg (optional)
		{
			if (numtok > 3) {
				int radius = input.gettok(4, " ").to_int();
				if (radius < 1) radius = 100; // handle cases where arg isnt a number or is a negative.
				int cx = ((rc.right - rc.left)/2);
				int cy = ((rc.bottom - rc.top)/2);
				m_Region = CreateEllipticRgn(cx-radius,cy-radius,cx+radius,cy+radius);
			}
			else
				m_Region = CreateEllipticRgn(0,0,rc.right - rc.left,rc.bottom - rc.top);
		}
		else if (flag.find('p',0)) // polygon
		{
			// u need at least 3 points for a shape
			if (numtok < 6) {
				DCXError("/xdialog -R","Invalid arguments for +p flag");
				return;
			}

			TString strPoints(input.gettok(4, -1, " "));
			TString strPoint;
			int tPoints = strPoints.numtok(" ");

			if (tPoints < 1) {
				DCXError("/xdialog -R","Invalid Points");
				return;
			}

			int cnt = 1;
			POINT *pnts = new POINT[tPoints];

			while (cnt <= tPoints) {
				strPoint = strPoints.gettok(cnt," ");
				pnts[cnt-1].x = (LONG)strPoint.gettok(1, ",").to_num();
				pnts[cnt-1].y = (LONG)strPoint.gettok(2, ",").to_num();
				cnt++;
			}

			m_Region = CreatePolygonRgn(pnts,tPoints,WINDING);

			delete [] pnts;
		}
		else if (flag.find('d',0)) // drag - <1|0>
		{
			noRegion = true;
			if ((BOOL)input.gettok(4," ").to_int())
				this->m_bDoDrag = true;
			else
				this->m_bDoDrag = false;
		}
		else if (flag.find('g',0)) // ghost drag - <0-255>
		{
			int alpha = input.gettok(4," ").to_int();
			if ((alpha >= 0) && (alpha <= 255)) {
				noRegion = true;
				this->m_bDoGhostDrag = alpha;
			}
			else {
				DCXError("xdialog -R +g","Alpha Out Of Range");
				return;
			}
		}
		else if (flag.find('s',0)) // shadow - <colour> <sharpness> <darkness> <size> <xoffset> <yoffset>
		{
			if (numtok == 9) {
				noRegion = true;
				COLORREF s_clr = (COLORREF)input.gettok(4).to_num();
				int s_sharp = input.gettok(5).to_int();
				int s_dark = input.gettok(6).to_int();
				int s_size = input.gettok(7).to_int();
				int s_x = input.gettok(8).to_int();
				int s_y = input.gettok(9).to_int();
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
		else if (flag.find('n',0)) { // none, no args
			noRegion = true;
			SetWindowRgn(this->m_Hwnd,NULL,TRUE);
		}
		else
			DCXError("xdialog -R", "Invalid Flag");

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
				DCXError("/xdialog -R","Unable to create region.");
		}
		this->redrawWindow();
	}
	// xdialog -E [NAME] [SWITCH] [+flags] [-flags]
	else if (flags.switch_cap_flags[4] && numtok > 3) {
		//this->m_dEventMask = (DWORD)input.gettok(3," ").to_num();
		DWORD mask = this->m_dEventMask;
		TString p_flags(input.gettok(3," "));
		TString n_flags(input.gettok(4," "));

		if ((p_flags[0] != '+') || (n_flags[0] != '-')) {
			DCXError("xdialog -E", "Invalid Flag");
			return;
		}
		if (p_flags.find('c',0))
			mask |= DCX_EVENT_CLICK;
		if (p_flags.find('d',0))
			mask |= DCX_EVENT_DRAG;
		if (p_flags.find('e',0))
			mask |= DCX_EVENT_EDIT;
		if (p_flags.find('f',0))
			mask |= DCX_EVENT_FOCUS;
		if (p_flags.find('h',0))
			mask |= DCX_EVENT_HELP;
		if (p_flags.find('m',0))
			mask |= DCX_EVENT_MOUSE;
		if (p_flags.find('s',0))
			mask |= DCX_EVENT_SIZE;
		if (p_flags.find('t',0))
			mask |= DCX_EVENT_THEME;
		if (p_flags.find('C',0))
			mask |= DCX_EVENT_CLOSE;
		if (p_flags.find('M',0))
			mask |= DCX_EVENT_MOVE;

		if (n_flags.find('c',0))
			mask &= ~DCX_EVENT_CLICK;
		if (n_flags.find('d',0))
			mask &= ~DCX_EVENT_DRAG;
		if (n_flags.find('e',0))
			mask &= ~DCX_EVENT_EDIT;
		if (n_flags.find('f',0))
			mask &= ~DCX_EVENT_FOCUS;
		if (n_flags.find('h',0))
			mask &= ~DCX_EVENT_HELP;
		if (n_flags.find('m',0))
			mask &= ~DCX_EVENT_MOUSE;
		if (n_flags.find('s',0))
			mask &= ~DCX_EVENT_SIZE;
		if (n_flags.find('t',0))
			mask &= ~DCX_EVENT_THEME;
		if (n_flags.find('C',0))
			mask &= ~DCX_EVENT_CLOSE;
		if (n_flags.find('M',0))
			mask &= ~DCX_EVENT_MOVE;

		this->m_dEventMask = mask;
	}
	// xdialog -X [NAME] [SWITCH] [FILENAME]
	else if (flags.switch_cap_flags[23] && numtok > 2) {
		//const char *dcxmlfile = input.gettok(3," ").to_chr(); <- faulty code, pointer to tempary var's contents
		TiXmlDocument doc(input.gettok(3," ").to_chr());
		bool valid_XML = doc.LoadFile();
		TString cmd;
		if (valid_XML) { 
			TiXmlElement *valid_DCXML = 0;
			valid_DCXML = doc.FirstChildElement("dialog");
			if (valid_DCXML) {
				mIRCDebug("yay DCXML! %s %s",input.gettok(2,"\"").to_chr(),valid_DCXML->Value());

				mIRCDebug("xdialog -l %s root $chr(9) +%s %s", this->getName().to_chr(),valid_DCXML->Attribute("direction"),valid_DCXML->Attribute("space"));

				walkScript(valid_DCXML,this->getName().to_chr());
			}
			else
			  mIRCDebug("File isn't valid dcXML %s",input.gettok(2,"\"").to_chr());
		}
		else
			mIRCDebug("File isn't valid XML %s",input.gettok(2,"\"").to_chr());
		doc.Clear();
	}
	// invalid command
	else {
		TString errmsg;

		if (numtok > 2) {
			errmsg.sprintf("D_ERROR xdialog: Invalid command /xdialog %s %s %s",
				input.gettok(2, " ").to_chr(),
				input.gettok(1, " ").to_chr(),
				input.gettok(3, -1, " ").to_chr());
		}
		else {
			errmsg.sprintf("D_ERROR xdialog: Invalid command /xdialog %s %s",
				input.gettok(2, " ").to_chr(),
				input.gettok(1, " ").to_chr());
		}

		mIRCError(errmsg.to_chr());
	}
}

/*!
 * \brief blah
 *
 * blah
 */


void DcxDialog::parseBorderStyles(TString &flags, LONG *Styles, LONG *ExStyles) {
	INT i = 1, len = flags.len();

	// no +sign, missing params
	if (flags[0] != '+') 
		return;

	while (i < len) {
		if (flags[i] == 'b')
			*Styles |= WS_BORDER;
		else if (flags[i] == 'c')
			*ExStyles |= WS_EX_CLIENTEDGE;
		else if (flags[i] == 'd')
			*Styles |= WS_DLGFRAME ;
		else if (flags[i] == 'f')
			*ExStyles |= WS_EX_DLGMODALFRAME;
		else if (flags[i] == 'h')
			*ExStyles |= WS_EX_CONTEXTHELP;
		else if (flags[i] == 'm')
			*Styles |= WS_MAXIMIZEBOX;
		else if (flags[i] == 'n')
			*Styles |= WS_MINIMIZEBOX;
		else if (flags[i] == 'o')
			*ExStyles |= WS_EX_TOOLWINDOW;
		else if (flags[i] == 's')
			*ExStyles |= WS_EX_STATICEDGE;
		else if (flags[i] == 't')
			*Styles |= WS_CAPTION;
		else if (flags[i] == 'w')
			*ExStyles |= WS_EX_WINDOWEDGE;
		else if (flags[i] == 'y')
			*Styles |= WS_SYSMENU;
		else if (flags[i] == 'z')
			*Styles |= WS_SIZEBOX;

		++i;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

DWORD DcxDialog::getAnimateStyles(TString &flags) {
	DWORD Styles = 0;
	int i = 1, len = flags.len();

	while (i < len) {
		if (flags[i] == 's')
			Styles |= AW_SLIDE;
		else if (flags[i] == 'h')
			Styles |= AW_HIDE;
		else if (flags[i] == 'a')
			Styles |= AW_ACTIVATE;
		else if (flags[i] == 'b')
			Styles |= AW_BLEND;
		else if (flags[i] == 'v')
			Styles |= AW_VER_POSITIVE;
		else if (flags[i] == 'u')
			Styles |= AW_VER_NEGATIVE;
		else if (flags[i] == 'c')
			Styles |= AW_CENTER;
		else if (flags[i] == 'o')
			Styles |= AW_HOR_POSITIVE;
		else if (flags[i] == 'n')
			Styles |= AW_HOR_NEGATIVE;

		i++;
	}

	return Styles;
}

/*!
 * \brief blah
 *
 * blah
 */
UINT DcxDialog::parseLayoutFlags(TString &flags) {
	INT i = 1, len = flags.len();
	UINT iFlags = 0;

	// no +sign, missing params
	if (flags[0] != '+') 
		return iFlags;

	while (i < len) {
		if (flags[i] == 'f')
			iFlags |= LAYOUTFIXED;
		else if (flags[i] == 'h')
			iFlags |= LAYOUTHORZ;
		else if (flags[i] == 'i')
			iFlags |= LAYOUTID;
		else if (flags[i] == 'l')
			iFlags |= LAYOUTFILL ;
		else if (flags[i] == 'p')
			iFlags |= LAYOUTPANE;
		else if (flags[i] == 'v')
			iFlags |= LAYOUTVERT;
		else if (flags[i] == 'w')
			iFlags |= LAYOUTDIM;

		++i;
	}

	return iFlags;
}

UINT DcxDialog::parseTooltipFlags(TString &flags) {
	int i = 1;
	int len = flags.len();
	UINT iFlags = 0;

	// no +sign, missing params
	if (flags[0] != '+') 
		return iFlags;

	while (i < len) {
		if (flags[i] == 'a')
			iFlags |= TTS_ALWAYSTIP;
		else if (flags[i] == 'b')
			iFlags |= TTS_BALLOON;
		else if (flags[i] == 'c') // no idea what this does or if it works at all
			iFlags |= TTS_CLOSE;
		else if (flags[i] == 'f')
			iFlags |= TTS_NOFADE;
		else if (flags[i] == 'p')
			iFlags |= TTS_NOPREFIX;
		else if (flags[i] == 's')
			iFlags |= TTS_NOANIMATE;
		//else if (flags[i] == 't')
		//	iFlags |= TTS_USEVISUALSTYLE;

		++i;
	}

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxDialog::parseBkgFlags(TString &flags) {
	INT i = 1, len = flags.len(), iFlags = 0;

	// no +sign, missing params
	if (flags[0] != '+') 
		return iFlags;

	while (i < len) {
		if (flags[i] == 'b')
			iFlags |= DBS_BKGCOLOR;
		else if (flags[i] == 'i')
			iFlags |= DBS_BKGBITMAP;

		else if (flags[i] == 'n')
			iFlags |= DBS_BKGNORMAL;
		else if (flags[i] == 't')
			iFlags |= DBS_BKGTILE;
		else if (flags[i] == 's')
			iFlags |= DBS_BKGSTRETCH;

		else if (flags[i] == 'c')
			iFlags |= DBS_BKGCENTER;
		else if (flags[i] == 'v')
			iFlags |= DBS_BKGVCENTER;
		else if (flags[i] == 'r')
			iFlags |= DBS_BKGRIGHT;
		else if (flags[i] == 'o')
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

UINT DcxDialog::parseFlashFlags(TString &flags) {
	INT i = 1, len = flags.len(), iFlags = 0;

	// no +sign, missing params
	if (flags[0] != '+') 
		return iFlags;

	while (i < len) {
		if (flags[i] == 'a')
			iFlags |= FLASHW_ALL;
		else if (flags[i] == 'c')
			iFlags |= FLASHW_CAPTION;
		else if (flags[i] == 'f')
			iFlags |= FLASHW_TIMERNOFG;
		else if (flags[i] == 'r')
			iFlags |= FLASHW_TRAY;
		else if (flags[i] == 's')
			iFlags |= FLASHW_STOP;
		else if (flags[i] == 't')
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

UINT DcxDialog::parseCursorFlags(TString &flags) {
	INT i = 1, len = flags.len();
	UINT iFlags = 0;

	// no +sign, missing params
	if (flags[0] != '+') 
		return iFlags;

	while (i < len) {
		if (flags[i] == 'f')
			iFlags |= DCCS_FROMFILE;
		else if (flags[i] == 'r')
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

LPSTR DcxDialog::parseCursorType(TString &cursor) {
	if (cursor == "appstarting")
		return IDC_APPSTARTING;
	else if (cursor == "arrow")
		return IDC_ARROW;
	else if (cursor == "cross")
		return IDC_CROSS;
	else if (cursor == "hand")
		return IDC_HAND;
	else if (cursor == "help")
		return IDC_HELP;
	else if (cursor == "ibeam")
		return IDC_IBEAM;
	else if (cursor == "no")
		return IDC_NO;
	else if (cursor == "sizeall")
		return IDC_SIZEALL;
	else if (cursor == "sizenesw")
		return IDC_SIZENESW;
	else if (cursor == "sizens")
		return IDC_SIZENS;
	else if (cursor == "sizenwse")
		return IDC_SIZENWSE;
	else if (cursor == "sizewe")
		return IDC_SIZEWE;
	else if (cursor == "uparrow")
		return IDC_UPARROW;
	else if (cursor == "wait")
		return IDC_WAIT;

	return NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::parseInfoRequest(TString &input, char *szReturnValue) {
	int numtok = input.numtok(" ");

	// [NAME] [PROP] [ID]
	if (input.gettok(2, " ") == "isid" && numtok > 2) {
		int nID = input.gettok(3, " ").to_int();

		if (IsWindow(GetDlgItem(this->m_Hwnd, nID + mIRC_ID_OFFSET)) || 
			(this->getControlByID(nID + mIRC_ID_OFFSET) != NULL))
		{
			lstrcpy(szReturnValue, "$true");
		}
		else
			lstrcpy(szReturnValue, "$false");

		return;
	}
	// [NAME] [PROP]
	else if (input.gettok(2, " ") == "nextid") {
		int nID = 6001;

		while (IsWindow(GetDlgItem(this->m_Hwnd, nID)) || 
			(this->getControlByID(nID) != NULL))
		{
			nID++;
		}

		wsprintf(szReturnValue, "%d", nID - mIRC_ID_OFFSET);
		return;
	}
	// [NAME] [PROP] [N]
	if (input.gettok(2, " ") == "id" && numtok > 2) {
		int N = input.gettok(3, " ").to_int() -1;

		if (N == -1)
			wsprintf(szReturnValue, "%d", this->m_vpControls.size());
		else if ((N > -1) && (N < (int) this->m_vpControls.size()))
			wsprintf(szReturnValue, "%d", this->m_vpControls[N]->getUserID());

		return;
	}
	// [NAME] [PROP]
	else if (input.gettok(2, " ") == "ismenu") {
		if (GetMenu(this->m_Hwnd) != NULL)
			lstrcpy(szReturnValue, "$true");
		else
			lstrcpy(szReturnValue, "$false");

		return;
	}
	else if (input.gettok(2, " ") == "ismarked") {
		if (Dialogs.getDialogByHandle(this->m_Hwnd) != NULL)
			lstrcpy(szReturnValue, "$true");
		else
			lstrcpy(szReturnValue, "$false");

		return;
	}
	else if (input.gettok(2, " ") == "parent") {
		wsprintf(szReturnValue, "%s", this->getParentName().to_chr());
		return;
	}
	// [NAME] [PROP]
	else if (input.gettok(2, " ") == "mouseid") {
		wsprintf(szReturnValue, "%d", this->m_MouseID);
		return;
	}
	// [NAME] [PROP]
	else if (input.gettok(2, " ") == "focusid") {
		wsprintf(szReturnValue, "%d", this->m_FocusID);
		return;
	}
	// [NAME] [PROP]
	else if (input.gettok(2, " ") == "mouse") {
		POINT pt;

		GetCursorPos(&pt);
		ScreenToClient(this->m_Hwnd, &pt);
		wsprintf(szReturnValue, "%d %d", pt.x, pt.y);

		return;
	}
	// [NAME] [PROP]
	else if (input.gettok(2, " ") == "key") {
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

		wsprintf(szReturnValue, "%d", iKeyState);
		return;
	}
	// [NAME] [PROP]
	else if (input.gettok(2, " ") == "alias") {
		wsprintf(szReturnValue, "%s", this->getAliasName().to_chr());
		return;
	}
	// [NAME] [PROP]
	else if (input.gettok(2, " ") == "visible") {
		if (IsWindowVisible(this->m_Hwnd))
			lstrcpy(szReturnValue, "$true");
		else
			lstrcpy(szReturnValue, "$false");
		return;
	}
	// invalid info request
	else {
		mIRCDebug("D_ERROR $xdialog: Invalid property '%s' or parameters", input.gettok(2, -1, " ").to_chr());
	}

	szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxDialog::callAliasEx(char *szReturn, const char *szFormat, ...) {
	va_list args;
	va_start(args, szFormat);
	char parms[2048];

	vsprintf(parms, szFormat, args);
	wsprintf(mIRCLink.m_pData, "$%s(%s,%s)",
		this->getAliasName().to_chr(), 
		this->getName().to_chr(),
		parms);

	this->incRef();
	SendMessage(mIRCLink.m_mIRCHWND, WM_USER +201, 0, mIRCLink.m_map_cnt);

	if (szReturn)
		lstrcpy(szReturn, mIRCLink.m_pData);

	this->decRef();
	va_end(args);

	if (!lstrcmp(mIRCLink.m_pData, "$false"))
		return FALSE;

	return TRUE;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::updateLayout(RECT &rc) {
	if (this->m_pLayoutManager != NULL)
		this->m_pLayoutManager->updateLayout(rc);
}

/*!
 * \brief blah
 *
 * blah
 */

HBRUSH DcxDialog::getBackClrBrush() {
	return this->m_hBackBrush;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::setMouseControl(UINT mUID) {
	if (mUID != this->m_MouseID) {
		if (this->m_dEventMask & DCX_EVENT_MOUSE) {
			this->callAliasEx(NULL, "%s,%d", "mouseleave", this->m_MouseID);
			this->callAliasEx(NULL, "%s,%d", "mouseenter", mUID);
		}
		this->m_MouseID = mUID;
	}
	else if (this->m_dEventMask & DCX_EVENT_MOUSE)
		this->callAliasEx(NULL, "%s,%d", "mouse", mUID);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::setFocusControl(UINT mUID) {
	if (mUID != this->m_FocusID) {
		if (this->m_dEventMask & DCX_EVENT_FOCUS) {
			this->callAliasEx(NULL, "%s,%d", "focusout", this->m_FocusID);
			this->callAliasEx(NULL, "%s,%d", "focus", mUID);
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
	DcxDialog *p_this = (DcxDialog*) GetProp(mHwnd, "dcx_this");

	// sanity check for prop existing.
	if (p_this == NULL)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	bool fBlocked = (InSendMessageEx(NULL) & (ISMEX_REPLIED|ISMEX_SEND)) == ISMEX_SEND;

	// If Message is blocking just call old win proc
	if (fBlocked)
		return CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);

	BOOL bParsed = FALSE;
	LRESULT lRes = 0L;

	p_this->incRef();
	switch (uMsg) {
		case WM_MOVE:
			{
				if ((p_this->m_Shadow.Status & DCX_SS_VISABLE) && p_this->isShadowed())
				{
					RECT WndRect;
					GetWindowRect(mHwnd, &WndRect);
					SetWindowPos(p_this->m_Shadow.hWin, 0,
						WndRect.left + p_this->m_Shadow.nxOffset - p_this->m_Shadow.nSize,
						WndRect.top + p_this->m_Shadow.nyOffset - p_this->m_Shadow.nSize,
						0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
				}
			}
			break;
		case WM_THEMECHANGED:
		{
			if (p_this->m_dEventMask & DCX_EVENT_THEME)
				p_this->callAliasEx(NULL, "%s,%d", "themechanged", 0);
			break;
		}
		case WM_NOTIFY:
		{
			LPNMHDR hdr = (LPNMHDR) lParam;

			if (!hdr)
				break;

			if (IsWindow(hdr->hwndFrom)) {
				DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,"dcx_cthis");
				if (c_this != NULL) lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
			break;
		}

		case WM_HSCROLL:
		case WM_VSCROLL:
		{
			if (IsWindow((HWND) lParam)) {
				DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,"dcx_cthis");
				if (c_this != NULL) {
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;
		}

		case WM_COMPAREITEM:
			{
				LPCOMPAREITEMSTRUCT idata = (LPCOMPAREITEMSTRUCT)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
					if (c_this != NULL) {
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
			}
			break;

		case WM_DELETEITEM:
		{
			DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
			if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
				DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
				if (c_this != NULL) {
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;
		}

    case WM_MEASUREITEM:
      {
				HWND cHwnd = GetDlgItem(mHwnd, wParam);
				if (IsWindow(cHwnd)) {
					DcxControl *c_this = (DcxControl *) GetProp(cHwnd,"dcx_cthis");
					if (c_this != NULL) {
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
      }
      break;

		case WM_DRAWITEM:
		{
			DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
			if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
				DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
				if (c_this != NULL) {
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;
		}

		case WM_COMMAND:
		{
			if ((HIWORD(wParam) == 0) && (LOWORD(wParam) == 2) && (lParam == NULL)) {
				if (p_this->m_dEventMask & DCX_EVENT_CLOSE) {
					char ret[256];

					p_this->callAliasEx(ret, "%s,%d", "close", 0);

					if (lstrcmp("noclose", ret) == 0)
						bParsed = TRUE;
				}
			}
			else if (IsWindow((HWND) lParam)) {
				DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,"dcx_cthis");
				if (c_this != NULL) {
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
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
						char ret[256];
						p_this->callAliasEx(ret, "%s,%d", "beginmove", 0);

						if (lstrcmp("nomove", ret) != 0) {
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
						char ret[256];

						p_this->callAliasEx(ret, "%s,%d", "close", 0);

						if (lstrcmp("noclose", ret) == 0)
							bParsed = TRUE;
					}
					break;
				}

				case SC_MINIMIZE:
				{
					if (p_this->m_dEventMask & DCX_EVENT_SIZE) {
						char ret[256];

						p_this->callAliasEx(ret, "%s,%d", "min", 0);

						if (lstrcmp("stop", ret) != 0) {
							bParsed = TRUE;
							lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
						}
					}
					break;
				}

				case SC_MAXIMIZE:
				{
					if (p_this->m_dEventMask & DCX_EVENT_SIZE) {
						char ret[256];

						p_this->callAliasEx(ret, "%s,%d", "max", 0);

						if (lstrcmp("stop", ret) != 0) {
							bParsed = TRUE;
							lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
						}
					}

					break;
				}

				case SC_RESTORE:
				{
					if (p_this->m_dEventMask & DCX_EVENT_SIZE) {
						p_this->callAliasEx(NULL, "%s,%d", "restore", 0);

						//bParsed = TRUE;
						//lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
					}

					break;
				}

				case SC_SIZE:
				{
					if (p_this->m_dEventMask & DCX_EVENT_SIZE) {
						char ret[256];

						p_this->callAliasEx(ret, "%s,%d", "beginsize", 0);

						if (lstrcmp("nosize", ret) != 0) {
							bParsed = TRUE;
							p_this->m_bInSizing = true;
							lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
						}
					}

					break;
				}
			} // WM_SYSCOMMAND switch

			break;
		}

		// ghost drag stuff
		case WM_ENTERSIZEMOVE:
			{
				if (p_this->m_bDoGhostDrag < 255 && SetLayeredWindowAttributesUx != NULL) {
					long style = GetWindowLong(p_this->m_Hwnd, GWL_EXSTYLE);
					// Set WS_EX_LAYERED on this window
					if (!(style & WS_EX_LAYERED))
						SetWindowLong(p_this->m_Hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
					// Make this window 75 alpha
					SetLayeredWindowAttributesUx(p_this->m_Hwnd, 0, p_this->m_bDoGhostDrag, LWA_ALPHA);
					p_this->m_bGhosted = true;
				}
			}
			break;

		case WM_EXITSIZEMOVE:
		{
			if ((p_this->m_Shadow.Status & DCX_SS_VISABLE) && p_this->isShadowed())
				p_this->UpdateShadow();

			if ((p_this->m_bInSizing) && (p_this->m_dEventMask & DCX_EVENT_SIZE))
				p_this->callAliasEx(NULL, "%s,%d", "endsize", 0);
			else if ((p_this->m_bInMoving)  && (p_this->m_dEventMask & DCX_EVENT_MOVE))
				p_this->callAliasEx(NULL, "%s,%d", "endmove", 0);

			p_this->m_bInMoving = false;
			p_this->m_bInSizing = false;
			// turn off ghosting.
			if (p_this->m_bGhosted && SetLayeredWindowAttributesUx != NULL) {
				// Make this window solid
				SetLayeredWindowAttributesUx(p_this->m_Hwnd, 0, 255, LWA_ALPHA);
				p_this->m_bGhosted = false;
			}
			break;
		}

		case WM_MOVING:
		{
			if (p_this->m_dEventMask & DCX_EVENT_MOVE)
				p_this->callAliasEx(NULL, "%s,%d", "moving", 0);
			break;
		}

		case WM_SIZE:
		{
			if ((p_this->m_Shadow.Status & DCX_SS_ENABLED) && p_this->isShadowed())
			{
				if(SIZE_MAXIMIZED == wParam || SIZE_MINIMIZED == wParam)
				{
					ShowWindow(p_this->m_Shadow.hWin, SW_HIDE);
					p_this->m_Shadow.Status &= ~DCX_SS_VISABLE;
				}
				else if(p_this->m_Shadow.Status & DCX_SS_PARENTVISIBLE)	// Parent maybe resized even if invisible
				{
					// Awful! It seems that if the window size was not decreased
					// the window region would never be updated until WM_PAINT was sent.
					// So do not Update() until next WM_PAINT is received in this case
					if(LOWORD(lParam) > LOWORD(p_this->m_Shadow.WndSize) || HIWORD(lParam) > HIWORD(p_this->m_Shadow.WndSize))
						p_this->m_Shadow.bUpdate = true;
					else
						p_this->UpdateShadow();
					if(!(p_this->m_Shadow.Status & DCX_SS_VISABLE))
					{
						ShowWindow(p_this->m_Shadow.hWin, SW_SHOWNA);
						p_this->m_Shadow.Status |= DCX_SS_VISABLE;
					}
				}
				p_this->m_Shadow.WndSize = lParam;
			}

			if (p_this->m_dEventMask & DCX_EVENT_SIZE)
				p_this->callAliasEx(NULL, "%s,%d,%d,%d", "sizing", 0, LOWORD(lParam), HIWORD(lParam));

			HWND bars = NULL;

			while ((bars = FindWindowEx(mHwnd, bars, DCX_REBARCTRLCLASS, NULL)) != NULL) {
				SendMessage(bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0);
			}

			while ((bars = FindWindowEx(mHwnd, bars, DCX_STATUSBARCLASS, NULL)) != NULL) {
				SendMessage(bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0);
			}

			while ((bars = FindWindowEx(mHwnd, bars, DCX_TOOLBARCLASS, NULL)) != NULL) {
				DcxToolBar *t = (DcxToolBar*) p_this->getControlByHWND(bars);

				t->autoPosition(LOWORD(lParam), HIWORD(lParam));
				//SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) lParam );
			}

			RECT rc;

			SetRect(&rc, 0, 0, LOWORD(lParam), HIWORD(lParam));
			p_this->updateLayout(rc);
			p_this->redrawWindow();
			break;
		}

		case WM_WINDOWPOSCHANGING:
		{
			if (p_this->m_dEventMask & DCX_EVENT_MOVE) {
				char ret[256];

				p_this->callAliasEx(ret, "%s,%d", "changing", 0);

				WINDOWPOS *wp = (WINDOWPOS *) lParam;
				if (wp != NULL) {
					if (lstrcmp("nosize", ret) == 0)
						wp->flags |= SWP_NOSIZE;
					else if (lstrcmp("nomove", ret) == 0)
						wp->flags |= SWP_NOMOVE;
					else if (lstrcmp("nochange", ret) == 0)
						wp->flags |= SWP_NOSIZE | SWP_NOMOVE;
				}
			}
			break;
		}

		case WM_ERASEBKGND:
		{
			if (mHwnd != p_this->getHwnd())
				break;
			//HDC hdc = (HDC) wParam;
			RECT rwnd;

			GetClientRect(p_this->getHwnd(), &rwnd);

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
		//{
		//	DcxControl *p_Control = p_this->getControlByHWND((HWND) lParam);

		//	if (p_Control != NULL) {
		//		COLORREF clrText = p_Control->getTextColor();
		//		//COLORREF clrBackText = p_Control->getBackColor();
		//		//HBRUSH hBackBrush = p_Control->getBackClrBrush();

		//		if (clrText != -1)
		//			SetTextColor((HDC) wParam, clrText);

		//		//if (clrBackText != -1)
		//		//  SetBkColor((HDC) wParam, clrBackText);

		//		SetBkMode((HDC) wParam, TRANSPARENT);

		//		//if ( hBackBrush != NULL )
		//		//return (LRESULT) hBackBrush;

		//		bParsed = TRUE;
		//		lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
		//	}

		//	break;
		//}

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
				if (clrText != -1)
					SetTextColor((HDC) wParam, clrText);

				if (clrBackText != -1)
					SetBkColor((HDC) wParam, clrBackText);

				//if (p_Control->isExStyle(WS_EX_TRANSPARENT)) {
				//	bParsed = TRUE;
				//	return (LRESULT)GetStockObject(NULL_BRUSH);
				//}

				if (hBackBrush != NULL) {
					bParsed = TRUE;
					lRes = (LRESULT) hBackBrush;
				}
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
				tme.dwHoverTime = 1;
				p_this->m_bTracking = (BOOL) _TrackMouseEvent( &tme );
				if (p_this->m_dEventMask & DCX_EVENT_MOUSE)
					p_this->callAliasEx(NULL, "%s,%d", "denter", 0); // this tells you when the mouse enters or
			}
			break;
		}

		case WM_MOUSELEAVE:
		{
			if ( p_this->m_bTracking ) {
				p_this->m_bTracking = FALSE;
				if (p_this->m_dEventMask & DCX_EVENT_MOUSE)
					p_this->callAliasEx(NULL, "%s,%d", "dleave", 0); // leaves a dialogs client area.
			}
		}
		break;

		case WM_LBUTTONDOWN:
		{
			if (p_this->m_dEventMask & DCX_EVENT_CLICK)
				p_this->callAliasEx(NULL, "%s,%d", "lbdown", 0);
			if (p_this->m_bDoDrag)
				p_this->m_bDrag = true;
			break;
		}

		case WM_LBUTTONUP:
		{
			if (p_this->m_dEventMask & DCX_EVENT_CLICK) {
				p_this->callAliasEx(NULL, "%s,%d", "lbup", 0);
				p_this->callAliasEx(NULL, "%s,%d", "sclick", 0);
			}
			break;
		}

		case WM_LBUTTONDBLCLK:
		{
			if (p_this->m_dEventMask & DCX_EVENT_CLICK) {
				p_this->callAliasEx(NULL, "%s,%d", "dclick", 0);
				p_this->callAliasEx(NULL, "%s,%d", "lbdblclk", 0);
			}
			break;
		}

		case WM_RBUTTONDOWN:
		{
			if (p_this->m_dEventMask & DCX_EVENT_CLICK)
				p_this->callAliasEx(NULL, "%s,%d", "rbdown", 0);
			break;
		}

		case WM_RBUTTONUP:
		{
			if (p_this->m_dEventMask & DCX_EVENT_CLICK) {
				p_this->callAliasEx(NULL, "%s,%d", "rbup", 0);
				p_this->callAliasEx(NULL, "%s,%d", "rclick", 0);
			}
			break;
		}

		case WM_RBUTTONDBLCLK:
		{
			if (p_this->m_dEventMask & DCX_EVENT_CLICK)
				p_this->callAliasEx(NULL, "%s,%d", "rbdblclk", 0);
			break;
		}

		case WM_MBUTTONDOWN:
		{
			if (p_this->m_dEventMask & DCX_EVENT_CLICK)
				p_this->callAliasEx(NULL, "%s,%d", "mbdown", 0);
			break;
		}

		case WM_MBUTTONUP:
		{
			if (p_this->m_dEventMask & DCX_EVENT_CLICK)
				p_this->callAliasEx(NULL, "%s,%d", "mbup", 0);
			break;
		}

		case WM_MBUTTONDBLCLK:
		{
			if (p_this->m_dEventMask & DCX_EVENT_CLICK)
				p_this->callAliasEx(NULL, "%s,%d", "mbdblclk", 0);
			break;
		}

		case WM_MOUSEWHEEL:
		{
			if (p_this->m_dEventMask & DCX_EVENT_CLICK) {
				DWORD fwKeys = GET_KEYSTATE_WPARAM(wParam);
				DWORD zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
				TString flags("+");
				
				if (fwKeys & MK_CONTROL) // control button
					flags = flags + "c";
				if (fwKeys & MK_LBUTTON) // left mouse button
					flags = flags + "l";
				if (fwKeys & MK_MBUTTON) // middle mouse button button
					flags = flags + "m";
				if (fwKeys & MK_RBUTTON) // right mouse button
					flags = flags + "r";
				if (fwKeys & MK_SHIFT) // shift button
					flags = flags + "s";

				p_this->callAliasEx(NULL, "%s,%d,%s,%s",
					"mwheel",
					p_this->m_MouseID,
					((int) zDelta > 0 ? "up" : "down"),
					flags.to_chr());

				bParsed = TRUE;
				lRes = FALSE; // stop parsing of WM_MOUSEWHEEL
			}
			break;
		}

		//case WM_INITMENUPOPUP:
		//{
		//	mIRCError("WM_INITMENUPOPUP");
		//	break;
		//}

		case WM_SETCURSOR:
		{
			if ((LOWORD(lParam) == HTCLIENT) &&
				((HWND) wParam == p_this->getHwnd()) && 
				(p_this->getCursor() != NULL)) 
			{
				SetCursor(p_this->getCursor());
				bParsed = TRUE;
				lRes = TRUE;
			}

			break;
		}

		case WM_DROPFILES:
		{
			HDROP files = (HDROP) wParam;
			char filename[500];
			int count = DragQueryFile(files, 0xFFFFFFFF,  filename, 500);

			if (count) {
				if (p_this->m_dEventMask & DCX_EVENT_DRAG) {
					char ret[20];

					p_this->callAliasEx(ret, "%s,%d,%d", "dragbegin", 0, count);

					// cancel drag drop event
					if (lstrcmpi(ret, "cancel") == 0) {
						DragFinish(files);
						bParsed = TRUE;
						break;
					}
					// for each file, send callback message
					for (int i = 0; i < count; i++) {
						if (DragQueryFile(files, i, filename, 500))
							p_this->callAliasEx(NULL, "%s,%d,%s", "dragfile", 0, filename);
					}

					p_this->callAliasEx(NULL, "%s,%d", "dragfinish", 0);
				}
			}

			DragFinish(files);
			break;
		}

		case WM_ACTIVATE:
		{
			if (p_this->m_dEventMask & DCX_EVENT_FOCUS) {
				switch (wParam) {
					case WA_ACTIVE:
					case WA_CLICKACTIVE:
					{
						p_this->callAliasEx(NULL, "%s,%d", "activate", 0);
						break;
					}

					case WA_INACTIVE:
					{
						p_this->callAliasEx(NULL, "%s,%d", "deactivate", 0);
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
			}
			break;
		case WM_SHOWWINDOW:
			{
				if ((p_this->m_Shadow.Status & DCX_SS_ENABLED) && p_this->isShadowed())
				{
					if(!wParam)	// the window is being hidden
					{
						ShowWindow(p_this->m_Shadow.hWin, SW_HIDE);
						p_this->m_Shadow.Status &= ~(DCX_SS_VISABLE | DCX_SS_PARENTVISIBLE);
					}
					else if(!(p_this->m_Shadow.Status & DCX_SS_PARENTVISIBLE))
					{
						p_this->m_Shadow.bUpdate = true;
						ShowWindow(p_this->m_Shadow.hWin, SW_SHOWNA);
						p_this->m_Shadow.Status |= DCX_SS_VISABLE | DCX_SS_PARENTVISIBLE;
					}
				}
			}
			break;

		case WM_NCDESTROY: 
		{
			if (p_this != NULL) {

				LRESULT res = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);

				Dialogs.deleteDialog(p_this);
				delete p_this;
				return res;
			}

			break;
		}

		default:
			break;
	}
	p_this->decRef();
	if (bParsed)
		return lRes;

	if (p_this != NULL)
		return CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);

	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}


void DcxDialog::setParentName(TString &strParent) {
	this->m_tsParentName.sprintf("%s", strParent.to_chr());
}

TString DcxDialog::getParentName() {
	return this->m_tsParentName;
}

void DcxDialog::DrawDialogBackground(HDC hdc, DcxDialog *p_this, LPRECT rwnd)
{
	// background color
	if (p_this->getBackClrBrush() != NULL)
		FillRect(hdc, rwnd, p_this->getBackClrBrush());
	else
		FillRect(hdc, rwnd, GetSysColorBrush(COLOR_3DFACE));

	if (p_this->m_bitmapBg == NULL)
		return;

	HDC hdcbmp = CreateCompatibleDC(hdc);
	BITMAP bmp;

	GetObject(p_this->m_bitmapBg, sizeof(BITMAP), &bmp);
	SelectObject(hdcbmp, p_this->m_bitmapBg);

	int x = 0;
	int y = 0;
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
	if (UpdateLayeredWindowUx == NULL)
		return false;

	if (!this->isShadowed()) {
		// Create the shadow window
		this->m_Shadow.hWin = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, DCX_SHADOWCLASS, NULL,
			WS_CAPTION | WS_POPUPWINDOW, CW_USEDEFAULT, 0, 0, 0, this->m_Hwnd, NULL, GetModuleHandle(NULL), NULL);

		if (!IsWindow(this->m_Shadow.hWin))
			return false;

		// Determine the initial show state of shadow according to parent window's state
		LONG lParentStyle = GetWindowLong(this->m_Hwnd, GWL_STYLE);
		if(!(WS_VISIBLE & lParentStyle))	// Parent invisible
			this->m_Shadow.Status = DCX_SS_ENABLED;
		else if((WS_MAXIMIZE | WS_MINIMIZE) & lParentStyle)	// Parent visible but does not need shadow
			this->m_Shadow.Status = DCX_SS_ENABLED | DCX_SS_PARENTVISIBLE;
		else	// Show the shadow
		{
			this->m_Shadow.Status = DCX_SS_ENABLED | DCX_SS_VISABLE | DCX_SS_PARENTVISIBLE;
			ShowWindow(this->m_Shadow.hWin, SW_SHOWNA);
			this->UpdateShadow();
		}
		return true;
	}
	return false;
}

void DcxDialog::RemoveShadow(void)
{
	if (IsWindow(this->m_Shadow.hWin))
		DestroyWindow(this->m_Shadow.hWin);
}

void DcxDialog::UpdateShadow(void)
{
	if (UpdateLayeredWindowUx == NULL)
		return;

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
	HBITMAP hOriBmp = (HBITMAP)SelectObject(hMemDC, hbitmap);

	POINT ptDst = {WndRect.left + this->m_Shadow.nxOffset - this->m_Shadow.nSize, WndRect.top + this->m_Shadow.nyOffset - this->m_Shadow.nSize};
	POINT ptSrc = {0, 0};
	SIZE WndSize = {nShadWndWid, nShadWndHei};
	BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

	MoveWindow(this->m_Shadow.hWin, ptDst.x, ptDst.y, nShadWndWid, nShadWndHei, FALSE);

	/*BOOL bRet=*/ UpdateLayeredWindowUx(this->m_Shadow.hWin, NULL, &ptDst, &WndSize, hMemDC,
		&ptSrc, 0, &blendPixelFunction, ULW_ALPHA);

	//_ASSERT(bRet); // something was wrong....

	// Delete used resources
	SelectObject(hMemDC, hOriBmp);
	DeleteObject(hbitmap);
	DeleteDC(hMemDC);
}

bool DcxDialog::isShadowed(void)
{
	return (IsWindow(this->m_Shadow.hWin) ? true : false);
}

void DcxDialog::MakeShadow(UINT32 *pShadBits, HWND hParent, RECT *rcParent)
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
		//	TRACE("%d %d\n", ptAnchorsOri[i][0], ptAnchorsOri[i][1]);
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
	for(int i = 0; i <= 2 * nKernelSize; i++)
	{
		for(int j = 0; j <= 2 * nKernelSize; j++)
		{
			double dLength = sqrt((i - nKernelSize) * (i - nKernelSize) + (j - nKernelSize) * (double)(j - nKernelSize));
			if(dLength < nCenterSize)
				*pKernelIter = this->m_Shadow.nDarkness << 24 | PreMultiply(this->m_Shadow.Color, this->m_Shadow.nDarkness);
			else if(dLength <= nKernelSize)
			{
				UINT32 nFactor = ((UINT32)((1 - (dLength - nCenterSize) / (this->m_Shadow.nSharpness + 1)) * this->m_Shadow.nDarkness));
				*pKernelIter = nFactor << 24 | PreMultiply(this->m_Shadow.Color, nFactor);
			}
			else
				*pKernelIter = 0;
			//TRACE("%d ", *pKernelIter >> 24);
			pKernelIter ++;
		}
		//TRACE("\n");
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
	UINT32 clCenter = this->m_Shadow.nDarkness << 24 | PreMultiply(this->m_Shadow.Color, this->m_Shadow.nDarkness);
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
