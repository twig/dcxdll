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

#include "../defines.h"
#include "dcxdialog.h"
#include "dcxdialogcollection.h"
#include "dcxcontrol.h"

#include "dcxtoolbar.h"
#include "dcxrichedit.h"

#include "layout/layoutcellfixed.h"
#include "layout/layoutcellfill.h"
#include "layout/layoutcellpane.h"

extern DcxDialogCollection Dialogs;

extern mIRCDLL mIRCLink;

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
{
	this->m_tsName = tsName;
	this->m_tsAliasName = tsAliasName;
	this->m_hBackBrush = NULL;
	this->m_bitmapBg = NULL;
	this->m_colTransparentBg = RGB(255,0,255);

	this->m_hCursor = NULL;
	this->m_bCursorFromFile = FALSE;

	this->addStyle(WS_CLIPCHILDREN);

	this->m_hOldWindowProc = (WNDPROC) SetWindowLong(this->m_Hwnd, GWL_WNDPROC, (LONG) DcxDialog::WindowProc);

	this->m_pLayoutManager = new LayoutManager(this->m_Hwnd);

	this->m_MouseID = 0;
	this->m_FocusID = 0;

	this->m_ToolTipHWND = NULL;

	this->m_iRefCount = 0;

	this->m_bDoDrag = false;

	this->m_dEventMask = -1;

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
	//if (this->m_Region)
	//	DeleteObject(this->m_Region); // system deletes this for us.

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

			TString filename = input.gettok(4, -1, " ");
			filename.trim();

			if (filename != "none")
				this->m_bitmapBg = dcxLoadBitmap(this->m_bitmapBg, filename);
		}

		InvalidateRect(this->m_Hwnd, NULL, TRUE);
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
			TString com = input.gettok(1, "\t").gettok(3, " ");
			TString path = input.gettok(1, "\t").gettok(4, -1, " ");
			TString p2 = input.gettok(2, "\t");

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
		TString flags = input.gettok(3, " ");
		int index = input.gettok(4, " ").to_int();
		TString filename = input.gettok(5, -1, " ");
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
		TString flag = input.gettok(3," ");

		if ((flag.len() < 2) || (flag[0] != '+')) {
			DCXError("/xdialog -R","Invalid Flag");
			return;
		}

		RECT rc;
		GetWindowRect(this->m_Hwnd,&rc);

		switch (flag[1])
		{
			// image file - [COLOR] [FILE]
			case 'f': 
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

				if (this->m_bitmapBg != NULL) {
					this->m_Region = BitmapRegion(this->m_bitmapBg,this->m_colTransparentBg,TRUE);

					if (this->m_Region != NULL)
						SetWindowRgn(this->m_Hwnd,this->m_Region,TRUE);
				}

				break;
			}
			
			case 'r': // rounded rect - radius args (optional)
			{
				int radius;

				if (numtok > 3)
					radius = input.gettok(4, " ").to_int();
				else
					radius = 20;

				this->m_Region = CreateRoundRectRgn(0,0,rc.right - rc.left,rc.bottom - rc.top, radius, radius);

				if (this->m_Region)
					SetWindowRgn(this->m_Hwnd,this->m_Region,TRUE);

				break;
			}

			case 'c': // circle - radius arg (optional)
			{
				if (numtok > 3) {
					int radius = input.gettok(4, " ").to_int();
					if (radius < 1) radius = 100; // handle cases where arg isnt a number or is a negative.
					int cx = ((rc.right - rc.left)/2);
					int cy = ((rc.bottom - rc.top)/2);
					this->m_Region = CreateEllipticRgn(cx-radius,cy-radius,cx+radius,cy+radius);
				}
				else
					this->m_Region = CreateEllipticRgn(0,0,rc.right - rc.left,rc.bottom - rc.top);

				if (this->m_Region)
					SetWindowRgn(this->m_Hwnd,this->m_Region,TRUE);

				break;
			}
			
			case 'p': // polygon
			{
				// u need at least 3 points for a shape
				if (numtok < 6) {
					DCXError("/xdialog -R","Invalid arguments for +p flag");
					return;
				}

				TString strPoints = input.gettok(4, -1, " ");
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

				this->m_Region = CreatePolygonRgn(pnts,tPoints,WINDING);

				if (this->m_Region)
					SetWindowRgn(this->m_Hwnd,this->m_Region,TRUE);

				delete [] pnts;
				break;
			}

			case 'n': // none, no args
			{
				SetWindowRgn(this->m_Hwnd,NULL,TRUE);
				break;
			}
			case 'd': // drag - <1|0>
			{
				if ((BOOL)input.gettok(4," ").to_int())
					this->m_bDoDrag = true;
				else
					this->m_bDoDrag = false;
			}
			break;

			default:
			{
				DCXError("xdialog -R", "Invalid Flag");
				break;
			}
		}
		this->redrawWindow();
	}
	// xdialog -E [NAME] [SWITCH] [+flags] [-flags]
	else if (flags.switch_cap_flags[4] && numtok > 3) {
		//this->m_dEventMask = (DWORD)input.gettok(3," ").to_num();
		DWORD mask = this->m_dEventMask;
		TString p_flags = input.gettok(3," ");
		TString n_flags = input.gettok(4," ");

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

DWORD DcxDialog::getAnimateStyles(TString &flags) const {
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
		if (Dialogs.getDialogByHandle(this->m_Hwnd) != NULL) {
			lstrcpy(szReturnValue, "$true");
		}
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

						bParsed = TRUE;
						if (lstrcmp("nomove", ret) != 0) {
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

						bParsed = TRUE;
						if (lstrcmp("stop", ret) != 0)
							lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
					}
					break;
				}

				case SC_MAXIMIZE:
				{
					if (p_this->m_dEventMask & DCX_EVENT_SIZE) {
						char ret[256];

						p_this->callAliasEx(ret, "%s,%d", "max", 0);

						bParsed = TRUE;
						if (lstrcmp("stop", ret) != 0)
							lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
					}

					break;
				}

				case SC_RESTORE:
				{
					if (p_this->m_dEventMask & DCX_EVENT_SIZE) {
						p_this->callAliasEx(NULL, "%s,%d", "restore", 0);

						bParsed = TRUE;
						lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
					}

					break;
				}

				case SC_SIZE:
				{
					if (p_this->m_dEventMask & DCX_EVENT_SIZE) {
						char ret[256];

						p_this->callAliasEx(ret, "%s,%d", "beginsize", 0);

						bParsed = TRUE;
						if (lstrcmp("nosize", ret) != 0) {
							p_this->m_bInSizing = true;
							lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
						}
					}

					break;
				}
			} // WM_SYSCOMMAND switch

			break;
		}

		case WM_EXITSIZEMOVE:
		{
			if ((p_this->m_bInSizing) && (p_this->m_dEventMask & DCX_EVENT_SIZE))
				p_this->callAliasEx(NULL, "%s,%d", "endsize", 0);
			else if ((p_this->m_bInMoving)  && (p_this->m_dEventMask & DCX_EVENT_MOVE))
				p_this->callAliasEx(NULL, "%s,%d", "endmove", 0);

			p_this->m_bInMoving = false;
			p_this->m_bInSizing = false;
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
			HDC hdc = (HDC) wParam;
			RECT rwnd;

			GetClientRect(p_this->getHwnd(), &rwnd);

			// background color
			if (p_this->getBackClrBrush())
				FillRect(hdc, &rwnd, p_this->getBackClrBrush());
			else
				FillRect(hdc, &rwnd, GetSysColorBrush(COLOR_3DFACE));

			// draw bitmap
			if (p_this->m_bitmapBg) {
				HDC hdcbmp = CreateCompatibleDC(hdc);
				BITMAP bmp;

				GetObject(p_this->m_bitmapBg, sizeof(BITMAP), &bmp);
				SelectObject(hdcbmp, p_this->m_bitmapBg);

				int x = 0;
				int y = 0;
				int w = rwnd.right - rwnd.left;
				int h = rwnd.bottom - rwnd.top;

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
		{
			DcxControl *p_Control = p_this->getControlByHWND((HWND) lParam);

			if (p_Control != NULL) {
				COLORREF clrText = p_Control->getTextColor();
				//COLORREF clrBackText = p_Control->getBackColor();
				//HBRUSH hBackBrush = p_Control->getBackClrBrush();

				if (clrText != -1)
					SetTextColor((HDC) wParam, clrText);

				//if (clrBackText != -1)
				//  SetBkColor((HDC) wParam, clrBackText);

				SetBkMode((HDC) wParam, TRANSPARENT);

				//if ( hBackBrush != NULL )
				//return (LRESULT) hBackBrush;

				bParsed = TRUE;
				lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
			}

			break;
		}

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
			break;
		}

		case WM_LBUTTONDOWN:
		{
			if (p_this->m_dEventMask & DCX_EVENT_CLICK) {
				p_this->callAliasEx(NULL, "%s,%d", "sclick", 0);
				p_this->callAliasEx(NULL, "%s,%d", "lbdown", 0);
			}
			if (p_this->m_bDoDrag)
				p_this->m_bDrag = true;
			break;
		}

		case WM_LBUTTONUP:
		{
			if (p_this->m_dEventMask & DCX_EVENT_CLICK)
				p_this->callAliasEx(NULL, "%s,%d", "lbup", 0);
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
			if (p_this->m_dEventMask & DCX_EVENT_CLICK) {
				p_this->callAliasEx(NULL, "%s,%d", "rclick", 0);
				p_this->callAliasEx(NULL, "%s,%d", "rbdown", 0);
			}
			break;
		}

		case WM_RBUTTONUP:
		{
			if (p_this->m_dEventMask & DCX_EVENT_CLICK)
				p_this->callAliasEx(NULL, "%s,%d", "rbup", 0);
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