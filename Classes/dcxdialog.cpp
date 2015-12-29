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
, m_hBackBrush(nullptr)
, m_bitmapBg(nullptr)
, m_colTransparentBg(RGB(255,0,255))
, m_hCursor(nullptr)
, m_bCursorFromFile(FALSE)
, m_MouseID(0)
, m_FocusID(0)
, m_ToolTipHWND(nullptr)
, m_iRefCount(0)
, m_bDoDrag(false)
, m_dEventMask(DCX_EVENT_ALL)
, m_bTracking(FALSE)
, m_bDoGhostDrag(255)
, m_bGhosted(false)
, m_zLayerCurrent(0)
, m_popup(nullptr)
, m_hOldWindowProc(nullptr)
, m_hFakeHwnd(nullptr)
, m_iAlphaLevel(255)
, m_bHaveKeyColour(false)
, m_cKeyColour(CLR_NONE)
, m_bVistaStyle(false)
, m_pVistaBits(nullptr)
, m_hVistaBitmap(nullptr)
, m_hVistaHDC(nullptr)
, m_bVerboseErrors(true)
, m_bErrorTriggered(false)
, m_bDialogActive(true)
{

	this->addStyle(WS_CLIPCHILDREN);

	//this->addExStyle(WS_EX_TRANSPARENT); // WS_EX_TRANSPARENT|WS_EX_LAYERED gives a window u can click through to the win behind.

	this->m_hOldWindowProc = SubclassWindow(this->m_Hwnd, DcxDialog::WindowProc);

	this->m_pLayoutManager = new LayoutManager(this->m_Hwnd);

	//this->m_Shadow.hWin = nullptr;
	//this->m_Shadow.Status = 0;

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
	delete this->m_pLayoutManager;

	delete this->m_popup;

	PreloadData();
	//this->RemoveShadow();

	this->RemoveVistaStyle();

	if (this->m_bCursorFromFile && this->m_hCursor != nullptr)
		DestroyCursor(this->m_hCursor);

	RemoveProp(this->m_Hwnd, TEXT("dcx_this"));
}

/*!
 * \brief blah
 *
 * blah
 */
const TString &DcxDialog::getName() const noexcept {
	return this->m_tsName;
}

/*!
 * \brief blah
 *
 * blah
 */

const TString &DcxDialog::getAliasName() const noexcept {
  return this->m_tsAliasName;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::addControl(DcxControl *p_Control)
{
	if (p_Control == nullptr)
		throw Dcx::dcxException("Invalid Argument");

	this->m_vpControls.push_back(p_Control);
	p_Control->redrawWindow();
}

DcxControl *DcxDialog::addControl(const TString &input, const UINT offset, const UINT64 mask, HWND hParent)
{
	const TString tsID(input.getfirsttok(offset));
	UINT ID = 0U;

	if (isalpha(tsID[0]))
	{
		// assume a name
		ID = NameToID(tsID);	// check if name alrdy has an ID
		if (ID == 0)
			ID = getUniqueID();
	}
	else
		ID = mIRC_ID_OFFSET + tsID.to_<UINT>();

	if (ID < mIRC_ID_OFFSET + 1)
		throw Dcx::dcxException(TEXT("Invalid ID \"%\""), ID - mIRC_ID_OFFSET);

	if (!this->isIDValid(ID, true))
		throw Dcx::dcxException(TEXT("Control with ID \"%\" already exists"), ID - mIRC_ID_OFFSET);

	auto p_ctrl = DcxControl::controlFactory(this, ID, input, offset + 1, mask, hParent);

	this->addControl(p_ctrl);

	this->AddNamedId(tsID, ID);	// NB: adds numbers as names too: "200" == 6200 allowing it to track all used id's

	return p_ctrl;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::deleteControl(const DcxControl *const p_Control) {
	if (p_Control == nullptr)
		return;

	auto itStart = this->m_vpControls.begin();
	auto itEnd = this->m_vpControls.end();

	while (itStart != itEnd) {
		if (*itStart == p_Control && *itStart != nullptr) {
			this->m_vpControls.erase(itStart);
			return;
		}

		++itStart;
	}
}

//void DcxDialog::deleteAllControls() {
//}

/*!
 * \brief blah
 *
 * The ID must include the mIRC_ID_OFFSET of 6000.
 */
DcxControl *DcxDialog::getControlByID(const UINT ID) const {
	for (const auto &x: this->m_vpControls) {
		if (x->getID() == ID)
			return x;
	}
	return nullptr;

	//auto itEnd = m_vpControls.end();
	//auto itGot = std::find_if(m_vpControls.begin(), itEnd, [ID](VectorOfControlPtrs::const_reference arg) { return (arg->getID() == ID); });
	//if (itGot != itEnd)
	//	return (*itGot);
	//return nullptr;
}

//
// * \brief getControlByHWND(const HWND mHwnd)
// *
// * Returns DcxControl * if found or nullptr if not.
//
DcxControl *DcxDialog::getControlByHWND(const HWND mHwnd) const {
	for (const auto &x: this->m_vpControls) {
		if (x->getHwnd() == mHwnd)
			return x;
	}
	return nullptr;

	//auto itEnd = m_vpControls.end();
	//auto itGot = std::find_if(m_vpControls.begin(), itEnd, [mHwnd](VectorOfControlPtrs::const_reference arg) { return (arg->getHwnd() == mHwnd); });
	//if (itGot != itEnd)
	//	return (*itGot);
	//return nullptr;
}

// clears existing image and icon data and sets pointers to null
void DcxDialog::PreloadData() {
	if (this->m_bitmapBg != nullptr) {
		DeleteBitmap(this->m_bitmapBg);
		this->m_bitmapBg = nullptr;
	}
//#ifdef DCX_USE_GDIPLUS
//	delete this->m_pImage;
//	this->m_pImage = nullptr;
//#endif
}

/*!
 * \brief blah
 *
 * blah
 */
void DcxDialog::parseCommandRequestEX(const TCHAR *const szFormat, ...) {
	TString msg;

	va_list args;
	va_start(args, szFormat);
	msg.tvprintf(szFormat, args);
	va_end(args);

	this->parseCommandRequest(msg);
}

void DcxDialog::parseComControlRequestEX(const UINT id, const TCHAR *const szFormat, ...) {
	auto p_Control = this->getControlByID(id + mIRC_ID_OFFSET);
	if (p_Control == nullptr)
		throw Dcx::dcxException("parseComControlRequestEX() - Unable to find control");

	TString msg;

	va_list args;
	va_start(args, szFormat);
	msg.tvprintf(szFormat, args);
	va_end(args);

	p_Control->parseCommandRequest(msg);
}

void DcxDialog::parseCommandRequest( const TString &input) {
	const XSwitchFlags flags(input.getfirsttok(2));		// tok 2
	const auto numtok = input.numtok();

	// xdialog -a [NAME] [SWITCH] [+FLAGS] [DURATION]
	if (flags[TEXT('a')] && numtok > 3) {
		AnimateWindow(this->m_Hwnd,
			input.gettok( 4 ).to_int(),
			getAnimateStyles(input.getnexttok( )));	// tok 3

		if (IsWindowVisible(this->m_Hwnd))
			this->redrawWindow();
	}
	// xdialog -b [NAME] [SWITCH] [+FLAGS]
	else if (flags[TEXT('b')] && numtok > 2) {
		this->removeStyle(WS_BORDER | WS_DLGFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX |
			WS_SYSMENU | WS_SIZEBOX | WS_CAPTION);

		this->removeExStyle(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_CONTEXTHELP |
			WS_EX_TOOLWINDOW | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE | WS_EX_COMPOSITED | WS_EX_LAYERED);

		this->RemoveVistaStyle();

		LONG Styles = 0, ExStyles = 0;
		const auto tsStyles(input.getnexttok());	// tok 3

		this->parseBorderStyles(tsStyles, &Styles, &ExStyles);
		this->addStyle(Styles);
		this->addExStyle(ExStyles);

		if (tsStyles.find(TEXT('v'),0)) {
			// Vista Style Dialog
			this->CreateVistaStyle();
		}
		SetWindowPos(this->m_Hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		InvalidateRect(this->m_Hwnd, nullptr, TRUE);
		SendMessage(this->m_Hwnd, WM_NCPAINT, (WPARAM) 1, (LPARAM) 0);
	}
	// xdialog -c [NAME] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	else if (flags[TEXT('c')] && numtok > 7) {
		//const UINT ID = mIRC_ID_OFFSET + input.getnexttok( ).to_int();	// tok 3
		//
		//const TString tsID(input.getnexttok());
		//UINT ID = 0;
		//
		//if (isalpha(tsID[0]))
		//{
		//	// assume a name
		//	ID = NameToID(tsID);	// check if name alrdy has an ID
		//	if ( ID == 0)
		//		ID = getUniqueID();
		//}
		//else
		//	ID = mIRC_ID_OFFSET + tsID.to_<UINT>();
		//
		//if (ID < mIRC_ID_OFFSET + 1)
		//	throw Dcx::dcxException(TEXT("Invalid ID \"%\""), ID - mIRC_ID_OFFSET);
		//
		//if (!this->isIDValid(ID, true))
		//	throw Dcx::dcxException(TEXT("Control with ID \"%\" already exists"), ID - mIRC_ID_OFFSET);
		//
		//try {
		//	//throw std::exception("test");
		//
		//	this->addControl(DcxControl::controlFactory(this, ID, input, 4));
		//}
		//catch (std::exception &e)
		//{
		//	this->showErrorEx(nullptr, TEXT("-c"), TEXT("Unable To Create Control %d (%S)"), ID - mIRC_ID_OFFSET, e.what());
		//	throw;
		//}
		//
		//this->AddNamedId(tsID, ID);	// NB: adds numbers as names too: "200" == 200 allowing it to track all used id's

		try {
			this->addControl(input, 3, CTLF_ALLOW_ALL, nullptr);
		}
		catch (std::exception &e)
		{
			this->showErrorEx(nullptr, TEXT("-c"), TEXT("Unable To Create Control (%S)"), e.what());
			throw;
		}
	}
	// xdialog -d [NAME] [SWITCH] [ID]
	else if (flags[TEXT('d')] && numtok > 2) {
		/*
		if ( input.gettok( 3 ) == TEXT("*") ) { 

		this->deleteAllControls( );
		}
		else {
		*/

		const UINT ID = (mIRC_ID_OFFSET + input.getnexttok( ).to_int());	// tok 3

		//if (!this->isIDValid(ID))
		//	throw Dcx::dcxException(TEXT("Unknown control with ID \"%\" (dialog %)"), ID - mIRC_ID_OFFSET, this->m_tsName);
		//
		//auto p_Control = this->getControlByID(ID);

		auto p_Control = this->getControlByID(ID);
		if (p_Control == nullptr)
			throw Dcx::dcxException(TEXT("Unknown control with ID \"%\" (dialog %)"), ID - mIRC_ID_OFFSET, this->m_tsName);

		auto cHwnd = p_Control->getHwnd();
		const auto cid = p_Control->getUserID();

		// fix up focus id
		if (cid == this->m_FocusID) {
			auto h = GetNextDlgTabItem(this->m_Hwnd, cHwnd, FALSE);

			// control exists and is not the last one
			if (h && (h != cHwnd))
				this->setFocusControl(GetDlgCtrlID(h) - mIRC_ID_OFFSET);
			else
				this->setFocusControl(0);
		}

		const auto dct = p_Control->getControlType();

		if (dct == DcxControlTypes::DIALOG || dct == DcxControlTypes::WINDOW)
			delete p_Control;
		else {
			if (p_Control->getRefCount() != 0)
				throw Dcx::dcxException(TEXT("Can't delete control with ID \"%\" when it is inside it's own event (dialog %)"), p_Control->getUserID(), this->m_tsName);
					
			this->deleteControl(p_Control); // remove control from internal list!
			DestroyWindow(cHwnd);
		}

		//if ((p_Control->getType() == TEXT("dialog")) || (p_Control->getType() == TEXT("window")))
		//	delete p_Control;
		//else {
		//	if (p_Control->getRefCount() != 0)
		//		throw Dcx::dcxException(TEXT("Can't delete control with ID \"%\" when it is inside it's own event (dialog %)"), p_Control->getUserID(), this->m_tsName);
		//		
		//	this->deleteControl(p_Control); // remove control from internal list!
		//	DestroyWindow(cHwnd);
		//}
	}
	// xdialog -f [NAME] [SWITCH] [+FLAGS] [COUNT] [TIMEOUT]
	else if (flags[TEXT('f')] && numtok > 4) {
		const auto iFlags = this->parseFlashFlags(input.getnexttok());	// tok 3
		const auto iCount = input.getnexttok().to_int();				// tok 4
		const auto dwTimeout = (DWORD)input.getnexttok().to_num();	// tok 5
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
		this->m_uStyleBg = this->parseBkgFlags(input.getnexttok( ));			// tok 3

		if (dcx_testflag(this->m_uStyleBg, DBS_BKGCOLOR)) {
			const auto clrColor = (COLORREF)input.getnexttok( ).to_num();	// tok 4

			if (this->m_hBackBrush != nullptr) {
				DeleteBrush(this->m_hBackBrush);
				this->m_hBackBrush = nullptr;
			}

			if (clrColor != CLR_INVALID)
				this->m_hBackBrush = CreateSolidBrush(clrColor);
		}
		else if (dcx_testflag(this->m_uStyleBg, DBS_BKGBITMAP)) {
			PreloadData();

			auto filename(input.getlasttoks().trim());	// tok 4, -1

			if (filename != TEXT("none")) {
				this->m_bitmapBg = dcxLoadBitmap(this->m_bitmapBg, filename);
//#ifdef DCX_USE_GDIPLUS
//				if (mIRCLink.m_bUseGDIPlus)
//					this->LoadGDIPlusImage(filename);
//#endif
			}
		}

		//InvalidateRect(this->m_Hwnd, nullptr, TRUE);
		this->redrawWindow();
	}
	// xdialog -j [NAME] [SWITCH] (ID)
	else if (flags[TEXT('j')]) {
		if (numtok > 2) {
			const UINT id = (mIRC_ID_OFFSET + input.getnexttok( ).to_int());	// tok 3

			if (!this->isIDValid(id))
				throw Dcx::dcxException(TEXT("Could not find control %"), id - mIRC_ID_OFFSET);
			
			this->getControlByID(id)->redrawWindow();
		}
		else
			this->redrawWindow();
	}
	/*
	//xdialog -l [NAME] [SWITCH] [OPTIONS]

	[OPTIONS] :

	root [TAB]+flpiw [ID] [WEIGHT] [W] [H]
	add PATH[TAB]+flpiw [ID] [WEIGHT] [W] [H]
	space PATH[TAB]+ [L] [T] [R] [B]
	*/
	else if (flags[TEXT('l')] && numtok > 2) {
		const auto tsCmd(input.getnexttok());	// tok 3
		if (tsCmd == TEXT("update")) {
			if (this->m_pLayoutManager != nullptr) {
				RECT rc;

				if (!GetClientRect(this->m_Hwnd, &rc))
					throw Dcx::dcxException("Unable to get client rect!");

				if (this->updateLayout(rc))
					this->redrawWindow();
			}
		}
		else if (tsCmd == TEXT("clear")) {
			delete this->m_pLayoutManager;
			this->m_pLayoutManager = new LayoutManager(this->m_Hwnd);
			//this->redrawWindow(); // dont redraw here, leave that for an `update` cmd
		}
		else if (numtok > 7)
			this->m_pLayoutManager->AddCell(input);
		else
			throw Dcx::dcxException("Invalid Arguments");
	}
	// xdialog -q [NAME] [SWITCH] [+FLAGS] [CURSOR|FILENAME]
	else if (flags[TEXT('q')] && numtok > 3) {
		//const UINT iFlags = this->parseCursorFlags( input.getnexttok( ) );	// tok 3
		//HCURSOR hCursor = nullptr;
		//if ( this->m_bCursorFromFile )
		//	hCursor = this->m_hCursor;
		//this->m_hCursor = nullptr;
		//this->m_bCursorFromFile = false;
		//if (dcx_testflag(iFlags, DCCS_FROMRESSOURCE))
		//	//this->m_hCursor = LoadCursor( nullptr, this->parseCursorType( input.getnexttok( ) ) );	// tok 4
		//	this->m_hCursor = (HCURSOR)LoadImage(nullptr, this->parseCursorType(input.getnexttok()), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
		//else if (dcx_testflag(iFlags, DCCS_FROMFILE)) {
		//	TString filename(input.getlasttoks());	// tok 4, -1
		//	if (!IsFile(filename))
		//		throw Dcx::dcxException(TEXT("Unable to Access File: %"), filename);
		//	
		//	this->m_hCursor = (HCURSOR)LoadImage(nullptr, filename.to_chr(), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
		//	this->m_bCursorFromFile = true;
		//}
		//if (this->m_hCursor == nullptr)
		//	throw Dcx::dcxException("Unable to Load Cursor");
		//
		//if (GetCursor() == hCursor)
		//	SetCursor(this->m_hCursor);
	//
		//if (hCursor != nullptr)
		//	DestroyCursor( hCursor );

		// get cursors flags (either +f or +r atm)
		const auto iFlags = this->parseCursorFlags(input.getfirsttok(3));
		// filename is the resource name if +r
		// otherwise its the filename if +f
		auto filename(input.getlasttoks());
		// get resource cursor type, will be a nullptr if filename is a file
		// NB: if a filename is the same as a resource name then this will return the resource, but the file will be loaded.
		const auto CursorType = this->parseCursorType(filename);

		this->m_hCursor = Dcx::dcxLoadCursor(iFlags, CursorType, this->m_bCursorFromFile, this->m_hCursor, filename);
	}
	// xdialog -x [NAME]
	else if (flags[TEXT('x')]) {
		if (this->getRefCount() == 0) {
			//DestroyWindow(this->m_Hwnd);
			//SendMessage(this->m_Hwnd,WM_CLOSE,nullptr,nullptr); // this allows the dialogs WndProc to EndDialog() if needed.
			TCHAR ret[32];
			mIRCLinker::evalex(ret, 32, TEXT("$dialog(%s).modal"), this->m_tsName.to_chr());
			if (lstrcmp(ret, TEXT("$true")) == 0) // Modal Dialog
				SendMessage(this->m_Hwnd,WM_CLOSE,NULL,NULL); // this allows the dialogs WndProc to EndDialog() if needed.
			else // Modeless Dialog
				DestroyWindow(this->m_Hwnd);
		}
		else
			mIRCLinker::execex(TEXT("/.timer -m 1 0 xdialog -x %s"), this->getName().to_chr());
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
		const auto tsCmd(input.getnexttok());	// tok 3
		if (tsCmd == TEXT("alpha")) {
			const auto tsAlpha(input.getnexttok());	// tok 4
			if (tsAlpha == TEXT("none")) {
				this->m_iAlphaLevel = 255;
				if (!this->m_bVistaStyle) {
					if (this->isExStyle(WS_EX_LAYERED)) {
						this->removeExStyle(WS_EX_LAYERED);
						this->addExStyle(WS_EX_LAYERED);
						//RemStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
						//AddStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
						if (this->m_bHaveKeyColour) // reapply keycolour if any.
							SetLayeredWindowAttributes(this->m_Hwnd, this->m_cKeyColour, 0, LWA_COLORKEY);
					}
				}
			}
			else {
				this->m_iAlphaLevel = (BYTE)(tsAlpha.to_int() & 0xFF);

				if (!this->m_bVistaStyle) {
					// Set WS_EX_LAYERED on this window
					//AddStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
					this->addExStyle(WS_EX_LAYERED);

					// Make this window x% alpha
					SetLayeredWindowAttributes(this->m_Hwnd, 0, this->m_iAlphaLevel, LWA_ALPHA);
				}
			}
		}
		// Transparent color
		else if (tsCmd == TEXT("transparentcolor")) {
			const auto tsClr(input.getnexttok( ));	// tok 4
			if (tsClr == TEXT("none")) {
				this->m_cKeyColour = CLR_NONE;
				this->m_bHaveKeyColour = false;
				if (!this->m_bVistaStyle) {
					if (this->isExStyle(WS_EX_LAYERED)) {
						//RemStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
						//AddStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
						this->removeExStyle(WS_EX_LAYERED);
						this->addExStyle(WS_EX_LAYERED);
						if (this->m_iAlphaLevel != 255) // reapply alpha if any.
							SetLayeredWindowAttributes(this->m_Hwnd, 0, this->m_iAlphaLevel, LWA_ALPHA);
					}
				}
			}
			else {
				this->m_cKeyColour = tsClr.to_<COLORREF>();
				this->m_bHaveKeyColour = true;
				if (!this->m_bVistaStyle) {
					// Set WS_EX_LAYERED on this window
					//AddStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
					this->addExStyle(WS_EX_LAYERED);

					// Make colour transparent
					SetLayeredWindowAttributes(this->m_Hwnd, this->m_cKeyColour, 0, LWA_COLORKEY);
				}
			}
		}
		// Background color
		else if (tsCmd == TEXT("bgcolor")) {
			this->m_colTransparentBg = input.getnexttok().to_<COLORREF>();	// tok 4
		}
		// TODO: not going to document this, have no way to redrawing the window.
		// http://www.codeproject.com/KB/vb/ClickThroughWindows.aspx
		// NB: may not be compatible with vista style.
		// Click-through
		else if (tsCmd == TEXT("clickthrough")) {
			if (input.getnexttok( ) == TEXT("none")) {	// tok 4
				if (this->isExStyle(WS_EX_LAYERED|WS_EX_TRANSPARENT))
					//RemStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
					this->removeExStyle( WS_EX_LAYERED | WS_EX_TRANSPARENT);
				// re-apply any alpha or keycolour.
				if (((this->m_iAlphaLevel != 255) || (this->m_bHaveKeyColour)) && (!this->m_bVistaStyle)) {
					//AddStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
					this->addExStyle(WS_EX_LAYERED);
					if (this->m_iAlphaLevel != 255) // reapply alpha if any.
						SetLayeredWindowAttributes(this->m_Hwnd, 0, this->m_iAlphaLevel, LWA_ALPHA);
					if (this->m_bHaveKeyColour) // reapply keycolour if any.
						SetLayeredWindowAttributes(this->m_Hwnd, this->m_cKeyColour, 0, LWA_COLORKEY);
				}
			}
			else
				//AddStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
				this->addExStyle(WS_EX_LAYERED | WS_EX_TRANSPARENT);
		}
		else
			throw Dcx::dcxException("Unknown Switch");

		this->redrawWindow();
	}
	// xdialog -T [NAME] [SWITCH] [FLAGS] [STYLES]
	else if (flags[TEXT('T')] && numtok > 2) {
		if (IsWindow(this->m_ToolTipHWND))
			throw Dcx::dcxException("Tooltip already exists. Cannot recreate");

		const auto styles = parseTooltipFlags(input.getnexttok( ));	// tok 3

		// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/tooltip/styles.asp
		this->m_ToolTipHWND = CreateWindowEx(WS_EX_TOPMOST,
			TOOLTIPS_CLASS, nullptr,
			styles,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			this->m_Hwnd,
			nullptr, GetModuleHandle(nullptr), nullptr);
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
		const auto tsFlags(input.getnexttok());		// tok 3
		const auto index = input.getnexttok().to_int();	// tok 4
		auto filename(input.getlasttoks().trim());	// tok 5, -1

		ChangeHwndIcon(this->m_Hwnd,tsFlags,index,filename);
	}
	// xdialog -z [NAME] [SWITCH] [+FLAGS] [N]
	else if (flags[TEXT('z')] && numtok > 3) {
		const XSwitchFlags xflag(input.getnexttok( ));	// tok 3
		auto n = input.getnexttok().to_int();		// tok 4

		// invalid input for [N]
		if ((n <= 0) || (!xflag[TEXT('+')]))
			throw Dcx::dcxException("Invalid Parameters");

		// adding control ID to list
		if (xflag[TEXT('a')])
		{
			// add mIRC offset since getControlByID() needs it
			n += mIRC_ID_OFFSET;

			// check if the ID is already in the list
			for (const auto &x: this->m_vZLayers) {
				if (x == n)
					throw Dcx::dcxException(TEXT("control % already in list"), n);
			}

			// if the specified control exists on the dialog, hide it
			auto ctrl = getControlByID(n);

			if (ctrl != nullptr) {
				ShowWindow(ctrl->getHwnd(), SW_HIDE);
				
				RECT rc;
				if (!GetClientRect(this->getHwnd(), &rc))
					throw Dcx::dcxException("Unable to get client rect!");
				
				if (this->updateLayout(rc))
					this->redrawWindow();
			}

			// append the item to the end of the list
			this->m_vZLayers.push_back(n);
		}
		// position to match CID [CID]
		else if (xflag[TEXT('p')]) {
			// add mIRC offset since getControlByID() needs it
			n += mIRC_ID_OFFSET;

			// get the control which will be used to retrieve the target position
			auto ctrl = getControlByID(n);

			// target control not found
			if (ctrl == nullptr)
				throw Dcx::dcxException("No such control");

			// variables used to move control
			RECT r;

			// figure out coordinates of control
			if (!GetWindowRect(ctrl->getHwnd(), &r))
				throw Dcx::dcxException("Unable to get window rect!");

			MapWindowRect(nullptr, GetParent(ctrl->getHwnd()), &r);

			// for each control in the internal list
			for (const auto &x : this->m_vZLayers) {
				// ignore target control
				if (x != n) {
					// get control to be moved
					ctrl = getControlByID(x);

					// if it exists, move it
					if (ctrl != nullptr)
						MoveWindow(ctrl->getHwnd(), r.left, r.top, r.right - r.left, r.bottom - r.top, FALSE);
				}
			}
		}
		// show index [N]
		else if (xflag[TEXT('s')]) {
			// minus since indexes are zero-based
			n--;

			// if the index is out of bounds
			if (n >= (int) this->m_vZLayers.size())
				throw Dcx::dcxException("Index array out of bounds");

			this->execAliasEx(TEXT("%s,%d,%d"), TEXT("zlayershow"), n +1, this->m_vZLayers[n] - mIRC_ID_OFFSET);

			// hide the previous control
			auto ctrl = getControlByID(this->m_vZLayers[this->m_zLayerCurrent]);

			if (ctrl != nullptr)
				ShowWindow(ctrl->getHwnd(), SW_HIDE);

			// set the new index to the currently selected index
			this->m_zLayerCurrent = n;
			ctrl = getControlByID(this->m_vZLayers[n]);

			// if the selected control exists, show control
			if (ctrl == nullptr)
				throw Dcx::dcxException("Invalid Control ID");
	
			ShowWindow(ctrl->getHwnd(), SW_SHOW);
			RECT rc;
			if (!GetClientRect(this->getHwnd(), &rc))
				throw Dcx::dcxException("Unable to get client rect!");
				
			if (this->updateLayout(rc))
				this->redrawWindow();
		}

		return;
	}
	// xdialog -P [NAME] [SWITCH] ([+FLAGS] (FLAG OPTIONS))
	else if (flags[TEXT('P')]) {

		// create the menu
		if (this->m_popup == nullptr) {
			auto menu = GetMenu(this->m_Hwnd);

			if (menu == nullptr)
				throw Dcx::dcxException("Menu Does Not Exist");
			
			this->m_popup = new XPopupMenu(TEXT("dialog"), menu);
		}
		if (this->m_popup != nullptr) {
			TString menuargs(TEXT("dialog "));
			menuargs += input.getlasttoks();
			Dcx::XPopups.parseCommand(menuargs, this->m_popup);
		}
	}
	// xdialog -R [NAME] [SWITCH] [FLAG] [ARGS]
	else if (flags[TEXT('R')] && numtok > 2) {
		const XSwitchFlags xflags(input.getnexttok( ));	// tok 3

		if (!xflags[TEXT('+')])
			throw Dcx::dcxException("Invalid Flag");

		RECT rc;
		if (!dcxGetWindowRect(this->m_Hwnd, &rc))
			throw Dcx::dcxException("Unable to get window rect!");

		HRGN m_Region = nullptr;
		auto RegionMode = 0;
		auto noRegion = false;

		if (xflags[TEXT('o')])
			RegionMode = RGN_OR;
		else if (xflags[TEXT('a')])
			RegionMode = RGN_AND;
		else if (xflags[TEXT('i')])
			RegionMode = RGN_DIFF;
		else if (xflags[TEXT('x')])
			RegionMode = RGN_XOR;

		// image file - [COLOR] [FILE]
		if (xflags[TEXT('f')])
		{
			if (numtok < 5)
				throw Dcx::dcxException("Invalid arguments");

			PreloadData();
			//SetWindowRgn(this->m_Hwnd,nullptr,TRUE);
			this->m_colTransparentBg = (COLORREF)input.getnexttok( ).to_num();	// tok 4
			//this->m_uStyleBg = DBS_BKGBITMAP|DBS_BKGSTRETCH|DBS_BKGCENTER;
			this->m_uStyleBg = DBS_BKGBITMAP;
			auto filename(input.getlasttoks());								// tok 5, -1
			this->m_bitmapBg = dcxLoadBitmap(this->m_bitmapBg,filename);

			if (this->m_bitmapBg == nullptr)
				throw Dcx::dcxException("Unable To Load Image file.");
			
			m_Region = BitmapRegion(this->m_bitmapBg, this->m_colTransparentBg, TRUE);
		}
		else if (xflags[TEXT('r')]) // rounded rect - radius args (optional)
		{
			int radius;

			if (numtok > 3)
				radius = input.getnexttok( ).to_int();	// tok 4
			else
				radius = 20;

			m_Region = CreateRoundRectRgn(0,0,rc.right - rc.left,rc.bottom - rc.top, radius, radius);
		}
		else if (xflags[TEXT('c')]) // circle - radius arg (optional)
		{
			if (numtok > 3) {
				auto radius = input.getnexttok().to_int();	// tok 4
				if (radius < 1)
					radius = 100; // handle cases where arg isnt a number or is a negative.
				const auto cx = ((rc.right - rc.left) / 2);
				const auto cy = ((rc.bottom - rc.top) / 2);
				m_Region = CreateEllipticRgn(cx-radius,cy-radius,cx+radius,cy+radius);
			}
			else
				m_Region = CreateEllipticRgn(0,0,rc.right - rc.left,rc.bottom - rc.top);
		}
		else if (xflags[TEXT('p')]) // polygon
		{
			// You need at least 3 points for a shape
			if (numtok < 6)
				throw Dcx::dcxException("Invalid arguments");

			const auto strPoints(input.getlasttoks());	// tok 4, -1
			const auto tPoints = strPoints.numtok();

			if (tPoints < 1)
				throw Dcx::dcxException("Invalid Points");

			auto pnts = std::make_unique<POINT[]>(tPoints);

			UINT cnt = 0;
			for (const auto &strPoint : strPoints)
			{
				pnts[cnt].x = strPoint.getfirsttok(1, TSCOMMA).to_<LONG>();
				pnts[cnt].y = strPoint.getnexttok(TSCOMMA).to_<LONG>();	// tok 2
				++cnt;
			}
			m_Region = CreatePolygonRgn(pnts.get(),tPoints,WINDING);
		}
		else if (xflags[TEXT('d')]) // drag - <1|0>
		{
			noRegion = true;
			if ((BOOL)input.getnexttok( ).to_int())	// tok 4
				this->m_bDoDrag = true;
			else
				this->m_bDoDrag = false;
		}
		else if (xflags[TEXT('g')]) // ghost drag - <0-255>
		{
			this->m_bDoGhostDrag = (BYTE)(input.getnexttok().to_int() & 0xFF);	// tok 4

			noRegion = true;
		}
		else if (xflags[TEXT('s')]) // shadow - <colour> <sharpness> <darkness> <size> <xoffset> <yoffset>
		{
			// no custom shadows in V3
			noRegion = true;

			//if (numtok == 9) {
			//	const auto s_clr = (COLORREF)input.getnexttok().to_num();	// tok 4
			//	const auto s_sharp = input.getnexttok().to_int();	// tok 5
			//	const auto s_dark = input.getnexttok().to_int();	// tok 6
			//	const auto s_size = input.getnexttok().to_int();	// tok 7
			//	const auto s_x = input.getnexttok().to_int();		// tok 8
			//	const auto s_y = input.getnexttok().to_int();		// tok 9
			//
			//	this->AddShadow();
			//	this->SetShadowColor(s_clr);
			//	this->SetShadowSharpness(s_sharp);
			//	this->SetShadowDarkness(s_dark);
			//	this->SetShadowSize(s_size);
			//	this->SetShadowPosition(s_x,s_y);
			//}
			//else {
			//	this->RemoveShadow();
			//}
		}
		else if (xflags[TEXT('n')]) { // none, no args
			noRegion = true;
			SetWindowRgn(this->m_Hwnd,nullptr,TRUE);
		}
		else
			throw Dcx::dcxException("Invalid Flag");

		if (!noRegion) {
			if (m_Region != nullptr)
				throw Dcx::dcxException("Unable to create region.");

			if (RegionMode != 0) {
				auto wrgn = CreateRectRgn(0, 0, 0, 0);
				if (wrgn != nullptr) {
					if (GetWindowRgn(this->m_Hwnd,wrgn) != ERROR)
						CombineRgn(m_Region,m_Region,wrgn,RegionMode);
					DeleteRgn(wrgn);
				}
			}
			SetWindowRgn(this->m_Hwnd,m_Region,TRUE);
		}
		this->redrawWindow();
	}
	// xdialog -E [NAME] [SWITCH] [+flags] [-flags]
	else if (flags[TEXT('E')] && numtok > 3) {
		auto mask = this->m_dEventMask;
		const XSwitchFlags xpFlags(input.getnexttok( ));	// tok 3
		const XSwitchFlags xnFlags(input.getnexttok( ));	// tok 4

		if (!xpFlags[TEXT('+')] || !xnFlags[TEXT('-')])
			throw Dcx::dcxException("Invalid Flag");

		if (xpFlags[TEXT('c')])
			mask |= DCX_EVENT_CLICK;
		if (xpFlags[TEXT('d')])
			mask |= DCX_EVENT_DRAG;
		if (xpFlags[TEXT('e')])
			mask |= DCX_EVENT_EDIT;
		if (xpFlags[TEXT('f')])
			mask |= DCX_EVENT_FOCUS;
		if (xpFlags[TEXT('h')])
			mask |= DCX_EVENT_HELP;
		if (xpFlags[TEXT('m')])
			mask |= DCX_EVENT_MOUSE;
		if (xpFlags[TEXT('s')])
			mask |= DCX_EVENT_SIZE;
		if (xpFlags[TEXT('t')])
			mask |= DCX_EVENT_THEME;
		if (xpFlags[TEXT('C')])
			mask |= DCX_EVENT_CLOSE;
		if (xpFlags[TEXT('M')])
			mask |= DCX_EVENT_MOVE;

		if (xnFlags[TEXT('c')])
			mask &= ~DCX_EVENT_CLICK;
		if (xnFlags[TEXT('d')])
			mask &= ~DCX_EVENT_DRAG;
		if (xnFlags[TEXT('e')])
			mask &= ~DCX_EVENT_EDIT;
		if (xnFlags[TEXT('f')])
			mask &= ~DCX_EVENT_FOCUS;
		if (xnFlags[TEXT('h')])
			mask &= ~DCX_EVENT_HELP;
		if (xnFlags[TEXT('m')])
			mask &= ~DCX_EVENT_MOUSE;
		if (xnFlags[TEXT('s')])
			mask &= ~DCX_EVENT_SIZE;
		if (xnFlags[TEXT('t')])
			mask &= ~DCX_EVENT_THEME;
		if (xnFlags[TEXT('C')])
			mask &= ~DCX_EVENT_CLOSE;
		if (xnFlags[TEXT('M')])
			mask &= ~DCX_EVENT_MOVE;

		this->m_dEventMask = mask;
	}
	// xdialog -V [NAME] [SWITCH] [left] [right] [top] [bottom]
	else if (flags[TEXT('V')] && numtok > 5) {
		this->m_GlassOffsets.left = input.getnexttok( ).to_int();	// tok 3
		this->m_GlassOffsets.right = input.getnexttok( ).to_int();	// tok 4
		this->m_GlassOffsets.top = input.getnexttok( ).to_int();	// tok 5
		this->m_GlassOffsets.bottom = input.getnexttok( ).to_int();	// tok 6

		if (Dcx::VistaModule.isUseable()) {
			const MARGINS margins = {this->m_GlassOffsets.left,this->m_GlassOffsets.right,this->m_GlassOffsets.top,this->m_GlassOffsets.bottom};
			Dcx::VistaModule.dcxDwmExtendFrameIntoClientArea(this->m_Hwnd, &margins);
		}
		this->redrawWindow();
	}
	// xdialog -U [NAME] [SWITCH]
	else if (flags[TEXT('U')]) {
		SetFocus(nullptr);
	}
	// xdialog -S [NAME] [SWITCH] [X Y W H]
	else if (flags[TEXT('S')] && (numtok > 5)) {
		auto x = input.getnexttok().to_int();	// tok 3
		auto y = input.getnexttok().to_int();	// tok 4
		auto w = input.getnexttok().to_int();	// tok 5
		auto h = input.getnexttok().to_int();	// tok 6

		RECT rcWindow, rcClient;
		UINT iFlags = SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOOWNERZORDER;

		if (!GetClientRect(this->m_Hwnd, &rcClient))
			throw Dcx::dcxException("Unable to get client rect!");

		//if (!dcxGetWindowRect(this->m_Hwnd, &rcWindow))
		if (!GetWindowRect(this->m_Hwnd, &rcWindow))
			throw Dcx::dcxException("Unable to get window rect!");

		// Convert windows screen position to its position within it's parent.
		if (this->isStyle(WS_CHILD))
			MapWindowRect(nullptr, GetParent(this->m_Hwnd), &rcWindow);

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
			w = rcClient.right;
		if (h < 0)
			h = rcClient.bottom;

		// Calculate the actual sizes without the window border
		// http://windows-programming.suite101.com/article.cfm/client_area_size_with_movewindow
		POINT ptDiff;
		//ptDiff.x = (rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left);
		//ptDiff.y = (rcWindow.bottom - rcWindow.top) - (rcClient.bottom - rcClient.top);
		ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
		ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;

		SetWindowPos( this->m_Hwnd, nullptr, x, y, w + ptDiff.x, h + ptDiff.y, iFlags );
	}
	// invalid command
	else
		throw Dcx::dcxException("Invalid Command");
}

/*!
 * \brief blah
 *
 * blah
 */


void DcxDialog::parseBorderStyles(const TString &flags, LONG *const Styles, LONG *const ExStyles) {
	const XSwitchFlags xflags(flags);

	// no +sign, missing params
	if (!xflags[TEXT('+')]) 
		return;

	if (xflags[TEXT('b')])
		*Styles |= WS_BORDER;
	if (xflags[TEXT('c')])
		*ExStyles |= WS_EX_CLIENTEDGE;
	if (xflags[TEXT('d')])
		*Styles |= WS_DLGFRAME;
	if (xflags[TEXT('f')])
		*ExStyles |= WS_EX_DLGMODALFRAME;
	if (xflags[TEXT('h')])
		*ExStyles |= WS_EX_CONTEXTHELP;
	if (xflags[TEXT('m')])
		*Styles |= WS_MAXIMIZEBOX;
	if (xflags[TEXT('n')])
		*Styles |= WS_MINIMIZEBOX;
	if (xflags[TEXT('o')])
		*ExStyles |= WS_EX_TOOLWINDOW;
	if (xflags[TEXT('s')])
		*ExStyles |= WS_EX_STATICEDGE;
	if (xflags[TEXT('t')])
		*Styles |= WS_CAPTION;
	if (xflags[TEXT('w')])
		*ExStyles |= WS_EX_WINDOWEDGE;
	if (xflags[TEXT('y')])
		*Styles |= WS_SYSMENU;
	if (xflags[TEXT('z')])
		*Styles |= WS_SIZEBOX;
	if (xflags[TEXT('x')])
		*ExStyles |= WS_EX_COMPOSITED;
	//	WS_EX_COMPOSITED style causes problems for listview control & maybe others, but when it works it looks really cool :)
	//	this improves transparency etc.. on xp+ only, looking into how this affects us.
	if (xflags[TEXT('v')])
		*ExStyles |= WS_EX_LAYERED;
}

/*!
 * \brief blah
 *
 * blah
 */

const DWORD DcxDialog::getAnimateStyles(const TString &flags) {
	DWORD Styles = 0;
	const XSwitchFlags xflags(flags);

	if (!xflags[TEXT('+')])
		return 0;

	if (xflags[TEXT('s')])
		Styles |= AW_SLIDE;
	if (xflags[TEXT('h')])
		Styles |= AW_HIDE;
	if (xflags[TEXT('a')])
		Styles |= AW_ACTIVATE;
	if (xflags[TEXT('b')])
		Styles |= AW_BLEND;
	if (xflags[TEXT('v')])
		Styles |= AW_VER_POSITIVE;
	if (xflags[TEXT('u')])
		Styles |= AW_VER_NEGATIVE;
	if (xflags[TEXT('c')])
		Styles |= AW_CENTER;
	if (xflags[TEXT('o')])
		Styles |= AW_HOR_POSITIVE;
	if (xflags[TEXT('n')])
		Styles |= AW_HOR_NEGATIVE;

	return Styles;
}

/*!
 * \brief blah
 *
 * blah
 */

const UINT DcxDialog::parseTooltipFlags(const TString &flags) {
	UINT iFlags = 0;
	const XSwitchFlags xflags(flags);

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('a')])
		iFlags |= TTS_ALWAYSTIP;
	if (xflags[TEXT('b')])
		iFlags |= TTS_BALLOON;
	if (xflags[TEXT('c')]) // no idea what this does or if it works at all
		iFlags |= TTS_CLOSE;
	if (xflags[TEXT('f')])
		iFlags |= TTS_NOFADE;
	if (xflags[TEXT('p')])
		iFlags |= TTS_NOPREFIX;
	if (xflags[TEXT('s')])
		iFlags |= TTS_NOANIMATE;
	if (xflags[TEXT('t')])
		iFlags |= TTS_USEVISUALSTYLE;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

const UINT DcxDialog::parseBkgFlags(const TString &flags) {
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('b')])
		iFlags |= DBS_BKGCOLOR;
	if (xflags[TEXT('i')])
		iFlags |= DBS_BKGBITMAP;

	if (xflags[TEXT('n')])
		iFlags |= DBS_BKGNORMAL;
	if (xflags[TEXT('t')])
		iFlags |= DBS_BKGTILE;
	if (xflags[TEXT('s')])
		iFlags |= DBS_BKGSTRETCH;

	if (xflags[TEXT('c')])
		iFlags |= DBS_BKGCENTER;
	if (xflags[TEXT('v')])
		iFlags |= DBS_BKGVCENTER;
	if (xflags[TEXT('r')])
		iFlags |= DBS_BKGRIGHT;
	if (xflags[TEXT('o')])
		iFlags |= DBS_BKGBOTTOM;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

const UINT DcxDialog::parseFlashFlags(const TString &flags) {
	UINT iFlags = 0;
	const XSwitchFlags xflags(flags);

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('a')])
		iFlags |= FLASHW_ALL;
	if (xflags[TEXT('c')])
		iFlags |= FLASHW_CAPTION;
	if (xflags[TEXT('f')])
		iFlags |= FLASHW_TIMERNOFG;
	if (xflags[TEXT('r')])
		iFlags |= FLASHW_TRAY;
	if (xflags[TEXT('s')])
		iFlags |= FLASHW_STOP;
	if (xflags[TEXT('t')])
		iFlags |= FLASHW_TIMER;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::parseInfoRequest( const TString &input, TCHAR *szReturnValue) const
{
	const auto numtok = input.numtok();
	const auto prop(input.getfirsttok(2));

	// [NAME] [PROP] [ID]
	if (prop == TEXT("isid") && numtok > 2) {
		const auto nID = input.getnexttok().to_int() + mIRC_ID_OFFSET;	// tok 3
		dcx_ConRet(IsWindow(GetDlgItem(this->m_Hwnd, nID)) || (this->getControlByID(nID) != nullptr), szReturnValue);
	}
	// [NAME] [PROP]
	else if (prop == TEXT("nextid")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), getUniqueID() - mIRC_ID_OFFSET);

		//auto nID = mIRC_ID_OFFSET + 1;
		//
		//while (IsWindow(GetDlgItem(this->m_Hwnd, nID)) || (this->getControlByID(nID) != nullptr))
		//{
		//	nID++;
		//}
		//
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), nID - mIRC_ID_OFFSET);
	}
	// [NAME] [PROP] [N|NAMEDID]
	else if (prop == TEXT("id") && numtok > 2) {
		const auto tsID(input.getnexttok());	// tok 3
		const auto N = tsID.to_int() - 1;

		if (N == -1)
		{
			if (tsID == TEXT('0'))	// check its an actual zero, not some text name (which also gives a zero result)
				wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), this->m_vpControls.size());
			else
			{
				//for (const auto &x: this->namedIds) {
				//	if (x.first == tsID) {
				//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%i"), x.second);
				//		return;
				//	}
				//}

				const auto it = namedIds.find(tsID);
				if (it != namedIds.end())
					wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), it->second - mIRC_ID_OFFSET);
			}
		}
		else if ((N > -1) && (N < (int) this->m_vpControls.size()))
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), this->m_vpControls[N]->getUserID());
	}
	// [NAME] [PROP]
	else if (prop == TEXT("ismenu")) {
		dcx_ConRet(GetMenu(this->m_Hwnd) != nullptr, szReturnValue);
	}
	// [NAME] [PROP]
	else if (prop == TEXT("ismarked")) {
		// no need to test anything, if it got here we already know its marked.
		dcx_strcpyn(szReturnValue, TEXT("$true"), MIRC_BUFFER_SIZE_CCH);

		//dcx_ConRet(Dcx::Dialogs.getDialogByHandle(this->m_Hwnd) != nullptr, szReturnValue);

	}
	// [NAME] [PROP]
	else if (prop == TEXT("parent")) {
		dcx_strcpyn(szReturnValue, this->getParentName().to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [PROP]
	else if (prop == TEXT("mouseid")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), this->m_MouseID);
	}
	// [NAME] [PROP]
	else if (prop == TEXT("focusid")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), this->m_FocusID);
	}
	// [NAME] [PROP]
	else if (prop == TEXT("mouse")) {
		POINT pt = { 0 };

		if (GetCursorPos(&pt))
			MapWindowPoints(nullptr, this->m_Hwnd, &pt, 1);

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), pt.x, pt.y);
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

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), iKeyState);
	}
	// [NAME] [PROP]
	else if (prop == TEXT("alias")) {
		dcx_strcpyn(szReturnValue, this->getAliasName().to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [PROP] [N]
	else if ((prop == TEXT("zlayer")) && (numtok > 2)) {
		const auto n = input.getnexttok().to_<VectorOfInts::size_type>();	// tok 3
		const auto size = this->m_vZLayers.size();

		if (n > size)
			throw Dcx::dcxException("Out Of Range");

		// return total number of id's
		if (n == 0)
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), size);
		// return the Nth id
		else
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), this->m_vZLayers[n - 1] - mIRC_ID_OFFSET);
	}
	// [NAME] [PROP]
	else if (prop == TEXT("zlayercurrent")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), this->m_zLayerCurrent +1);
	}
	// [NAME] [PROP]
	else if (prop == TEXT("visible")) {
		dcx_ConRet((IsWindowVisible(this->m_Hwnd) != FALSE), szReturnValue);

	}
	// [NAME] [PROP]
	else if (prop == TEXT("glasscolor")) {
		RGBQUAD clr = {0};
		auto bOpaque = FALSE;
		if (SUCCEEDED(Dcx::VistaModule.dcxDwmGetColorizationColor((DWORD *)&clr, &bOpaque)))
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), RGB(clr.rgbRed, clr.rgbGreen, clr.rgbBlue));
		else
			dcx_strcpyn(szReturnValue, TEXT("-FAIL Unable to get Glass colour."), MIRC_BUFFER_SIZE_CCH);
	}
	// invalid info request
	else
		throw Dcx::dcxException("Invalid property or parameters");
}

/*!
 * \brief blah
 *
 * blah
 */

bool DcxDialog::evalAliasEx(TCHAR *const szReturn, const int maxlen, const TCHAR *const szFormat, ...) {
	TString line;
	va_list args;

	va_start(args, szFormat);
	line.tvprintf(szFormat, args);
	va_end(args);

	return evalAlias(szReturn, maxlen, line.to_chr());
}

bool DcxDialog::evalAlias(TCHAR *const szReturn, const int maxlen, const TCHAR *const szArgs)
{
	this->incRef();
	const auto res = mIRCLinker::evalex(szReturn, maxlen, TEXT("$%s(%s,%s)"), this->getAliasName().to_chr(), this->getName().to_chr(), MakeTextmIRCSafe(szArgs).to_chr());
	this->decRef();
	return res;
}

bool DcxDialog::execAliasEx(const TCHAR *const szFormat, ...) {
	TString line;
	va_list args;

	va_start(args, szFormat);
	line.tvprintf(szFormat, args);
	va_end(args);

	return execAlias(line.to_chr());
}

bool DcxDialog::execAlias(const TCHAR *const szArgs)
{
	return evalAlias(nullptr, 0, szArgs);
}

/*!
 * \brief blah
 *
 * blah
 */

const bool DcxDialog::updateLayout(RECT &rc) {
	if (this->m_pLayoutManager == nullptr)
		return false;

	return this->m_pLayoutManager->updateLayout(rc);
}

/*!
 * \brief blah
 *
 * blah
 */

const HBRUSH &DcxDialog::getBackClrBrush() const {
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
	auto p_this = static_cast<DcxDialog *>(GetProp(mHwnd, TEXT("dcx_this")));

	// sanity check for prop existing.
	if ((p_this == nullptr) || (p_this->m_hOldWindowProc == nullptr))
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);
	
	// If Message is blocking just call old win proc
	if ((InSendMessageEx(nullptr) & (ISMEX_REPLIED|ISMEX_SEND)) == ISMEX_SEND)
		return CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);

	BOOL bParsed = FALSE;
	LRESULT lRes = 0L;

	// TODO: Is this instruction needed? Referencecount is managed by callAliasEx too ...
	//p_this->incRef();

	switch (uMsg) {
		case WM_HELP:
			{
				if (dcx_testflag(p_this->getEventMask(), DCX_EVENT_HELP))
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("help"), 0);
				bParsed = TRUE;
				lRes = TRUE;
			}
			break;
			// no shadow in V3

		//case WM_MOVE:
		//	{
		//		if ((p_this->m_Shadow.Status & DCX_SS_VISABLE) && p_this->isShadowed())
		//		{
		//			RECT WndRect;
		//			if (dcxGetWindowRect(mHwnd, &WndRect))
		//				SetWindowPos(p_this->m_Shadow.hWin, 0,
		//					WndRect.left + p_this->m_Shadow.nxOffset - p_this->m_Shadow.nSize,
		//					WndRect.top + p_this->m_Shadow.nyOffset - p_this->m_Shadow.nSize,
		//					0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
		//		}
		//		p_this->SetVistaStylePos();
		//	}
		//	break;

		case WM_THEMECHANGED:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_THEME))
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("themechanged"), 0);
				break;
			}
		case WM_NOTIFY:
			{
				dcxlParam(LPNMHDR, hdr);

				if (hdr == nullptr)
					break;

				if (IsWindow(hdr->hwndFrom)) {
					auto c_this = static_cast<DcxControl *>(GetProp(hdr->hwndFrom, TEXT("dcx_cthis")));
					if (c_this != nullptr)
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
						mIRCLinker::execex(TEXT("/.timer -m 1 0 xdialog -x %s"), p_this->getName().to_chr());
					}
					else if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLOSE)) {
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
					auto c_this = static_cast<DcxControl *>(GetProp((HWND)lParam, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
				break;
			}

		case WM_COMPAREITEM:
			{
				dcxlParam(const LPCOMPAREITEMSTRUCT, idata);

				if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
					auto c_this = static_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DELETEITEM:
			{
				dcxlParam(const LPDELETEITEMSTRUCT, idata);
				if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
					auto c_this = static_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
				break;
			}

		case WM_MEASUREITEM:
			{
				auto cHwnd = GetDlgItem(mHwnd, wParam);
				if (IsWindow(cHwnd)) {
					auto c_this = static_cast<DcxControl *>(GetProp(cHwnd, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}

				dcxlParam(LPMEASUREITEMSTRUCT, lpmis);

				if (p_this->m_popup != nullptr && lpmis->CtlType == ODT_MENU) {
					auto p_Item = reinterpret_cast<XPopupMenuItem*>(lpmis->itemData);

					if (p_Item != nullptr) {
						const auto size = p_Item->getItemSize(mHwnd);

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
				dcxlParam(const LPDRAWITEMSTRUCT, idata);
				if (idata == nullptr)
					break;

				if (IsWindow(idata->hwndItem)) {
					auto c_this = static_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
				else if (p_this->m_popup != nullptr && idata->CtlType == ODT_MENU) {
					auto p_Item = reinterpret_cast<XPopupMenuItem*>(idata->itemData);

					if (p_Item != nullptr) {
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
						if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_MOVE)) {
							TCHAR ret[256];
							p_this->evalAliasEx(ret, 255, TEXT("%s,%d"), TEXT("beginmove"), 0);

							if (lstrcmp(TEXT("nomove"), ret) != 0) {
								bParsed = TRUE;	// Ook: needs looked at as do the other switches here...
								p_this->m_bInMoving = true;
								lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
							}
						}
						break;
					}

				case SC_CLOSE:
					{
						if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLOSE)) {
							TCHAR ret[256];

							p_this->evalAliasEx(ret, 255, TEXT("%s,%d"), TEXT("scclose"), 0);

							if (lstrcmp(TEXT("noclose"), ret) == 0)
								bParsed = TRUE;
						}
						break;
					}

				case SC_MINIMIZE:
					{
						if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_SIZE)) {
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
						if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_SIZE)) {
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
						if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_SIZE)) {
							p_this->execAliasEx(TEXT("%s,%d"), TEXT("restore"), 0);

							//bParsed = TRUE;
							//lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
						}

						break;
					}

				case SC_SIZE:
					{
						TCHAR ret[256] = {0};

						if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_SIZE)) // mask only controls sending of event, if event isnt sent then DefWindowProc should still be called.
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
						const auto style = GetWindowExStyle(mHwnd);
						// Set WS_EX_LAYERED on this window
						if (!dcx_testflag(style,WS_EX_LAYERED))
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
				// no shadow in v3

				//if ((p_this->m_Shadow.Status & DCX_SS_VISABLE) && p_this->isShadowed())
				//	p_this->UpdateShadow();

				if ((p_this->m_bInSizing) && (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_SIZE)))
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("endsize"), 0);
				else if ((p_this->m_bInMoving)  && (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_MOVE)))
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("endmove"), 0);

//#if !defined(NDEBUG) || defined(DCX_DEV_BUILD)
				const auto bDoRedraw = p_this->m_bInSizing;
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
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_MOVE))
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

				// no shadow in v3

				//if ((p_this->m_Shadow.Status & DCX_SS_ENABLED) && p_this->isShadowed())
				//{
				//	if(SIZE_MAXIMIZED == wParam || SIZE_MINIMIZED == wParam)
				//	{
				//		ShowWindow(p_this->m_Shadow.hWin, SW_HIDE);
				//		p_this->m_Shadow.Status &= ~DCX_SS_VISABLE;
				//	}
				//	else
				//	{
				//		if(p_this->isStyle(WS_VISIBLE))	// Parent may be resized even if invisible
				//		{
				//			p_this->m_Shadow.Status |= DCX_SS_PARENTVISIBLE;
				//			if(!(p_this->m_Shadow.Status & DCX_SS_VISABLE))
				//			{
				//				p_this->m_Shadow.Status |= DCX_SS_VISABLE;
				//				// Update before show, because if not, restore from maximized will
				//				// see a glance misplaced shadow
				//				p_this->UpdateShadow();
				//				ShowWindow(p_this->m_Shadow.hWin, SW_SHOWNA);
				//				// If restore from minimized, the window region will not be updated until WM_PAINT:(
				//				p_this->m_Shadow.bUpdate = true;
				//			}
				//			// Awful! It seems that if the window size was not decreased
				//			// the window region would never be updated until WM_PAINT was sent.
				//			// So do not Update() until next WM_PAINT is received in this case
				//			else if(LOWORD(lParam) > LOWORD(p_this->m_Shadow.WndSize) || HIWORD(lParam) > HIWORD(p_this->m_Shadow.WndSize))
				//				p_this->m_Shadow.bUpdate = true;
				//			else
				//				p_this->UpdateShadow();
				//		}
				//	}
				//	p_this->m_Shadow.WndSize = lParam;
				//}

				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_SIZE))
					p_this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("sizing"), 0, LOWORD(lParam), HIWORD(lParam));

				HWND bars = nullptr;

				//SendMessage(mHwnd, WM_SETREDRAW, FALSE, 0);
				while ((bars = FindWindowEx(mHwnd, bars, DCX_REBARCTRLCLASS, nullptr)) != nullptr) {
					SendMessage(bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0);
				}

				while ((bars = FindWindowEx(mHwnd, bars, DCX_STATUSBARCLASS, nullptr)) != nullptr) {
					//SendMessage(bars, WM_SETREDRAW, FALSE, 0);
					SendMessage(bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0);
					//SendMessage(bars, WM_SETREDRAW, TRUE, 0);
				}
				while ((bars = FindWindowEx(mHwnd, bars, DCX_PANELCLASS, nullptr)) != nullptr) {
					SendMessage(bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0);
				}

				while ((bars = FindWindowEx(mHwnd, bars, DCX_TOOLBARCLASS, nullptr)) != nullptr) {
					auto t = static_cast<DcxToolBar*>(p_this->getControlByHWND(bars));
					if (t != nullptr)
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
					while ((bars = FindWindowEx(mHwnd, bars, DCX_RICHEDITCLASS, nullptr)) != nullptr) { // workaround added for RichText controls which seem to not redraw correctly via WM_PRINT
						RedrawWindow( bars, nullptr, nullptr, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_UPDATENOW|RDW_INVALIDATE|RDW_ERASE|RDW_FRAME );
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
				dcxlParam(LPWINDOWPOS, wp);

				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_MOVE)) {

					// break if nomove & nosize specified, since thats all we care about.
					if ((wp == nullptr) || ((dcx_testflag(wp->flags, SWP_NOMOVE)) && (dcx_testflag(wp->flags, SWP_NOSIZE))))
						break;

					TCHAR ret[256], *p = nullptr;

					switch ((wp->flags & (SWP_NOSIZE | SWP_NOMOVE))) {
					case SWP_NOSIZE:
						p = TEXT("moving");
						break;
					case SWP_NOMOVE:
						p = TEXT("sizing");
						break;
					case (SWP_NOSIZE | SWP_NOMOVE) :
						p = TEXT("neither");
						break;
					default:
						p = TEXT("both");
						break;
					}

					//p_this->callAliasEx(ret, TEXT("changing,0,%d,%d,%d,%d,%d"), (dcx_testflag(wp->flags, 3)),wp->x, wp->y, wp->cx, wp->cy);
					p_this->evalAliasEx(ret, 255, TEXT("changing,0,%s,%d,%d,%d,%d"), p, wp->x, wp->y, wp->cx, wp->cy);

					if (lstrcmp(TEXT("nosize"), ret) == 0)
						wp->flags |= SWP_NOSIZE;
					else if (lstrcmp(TEXT("nomove"), ret) == 0)
						wp->flags |= SWP_NOMOVE;
					else if (lstrcmp(TEXT("nochange"), ret) == 0)
						wp->flags |= SWP_NOSIZE | SWP_NOMOVE;
				}
				//RECT rc;
				//
				//SetRect(&rc, 0, 0, (wp->cx - wp->x), (wp->cy - wp->y));
				//p_this->updateLayout(rc);
				break;
			}

		case WM_ERASEBKGND:
			{
				if (mHwnd != p_this->getHwnd())
					break;
				RECT rwnd;

				if (GetClientRect(mHwnd, &rwnd))
				{

					DcxDialog::DrawDialogBackground((HDC)wParam, p_this, &rwnd);

					bParsed = TRUE;
					lRes = TRUE;
				}
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
				auto p_Control = p_this->getControlByHWND((HWND)lParam);

				if (p_Control != nullptr) {
					const auto clrText = p_Control->getTextColor();
					const auto clrBackText = p_Control->getBackColor();
					auto hBackBrush = p_Control->getBackClrBrush();

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

					if (hBackBrush != nullptr)
						lRes = (LRESULT) hBackBrush;

				}
				break;
			}

		case WM_MOUSEMOVE:
			{
				p_this->setMouseControl(0);
				if (p_this->m_bDrag) {
					POINT pt;
					if (GetCursorPos(&pt))
						PostMessage(p_this->m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM( pt.x, pt.y));
					p_this->m_bDrag = false;
				}
				if ( p_this->m_bTracking == FALSE ) {
					TRACKMOUSEEVENT tme;
					tme.cbSize = sizeof(TRACKMOUSEEVENT);
					tme.hwndTrack = p_this->m_Hwnd;
					tme.dwFlags = TME_LEAVE;
					tme.dwHoverTime = HOVER_DEFAULT; //1;
					p_this->m_bTracking = _TrackMouseEvent( &tme );
					if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_MOUSE))
						p_this->execAliasEx(TEXT("%s,%u"), TEXT("denter"), 0); // this tells you when the mouse enters or
					p_this->UpdateVistaStyle();
				}
				break;
			}

		case WM_MOUSELEAVE:
			{
				if ( p_this->m_bTracking ) {
					p_this->m_bTracking = FALSE;
					if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_MOUSE))
						p_this->execAliasEx(TEXT("%s,%u"), TEXT("dleave"), 0); // leaves a dialogs client area.
				}
				p_this->UpdateVistaStyle();
			}
			break;

		case WM_LBUTTONDOWN:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK))
					p_this->execAliasEx(TEXT("%s,%u"), TEXT("lbdown"), 0);
				if (p_this->m_bDoDrag)
					p_this->m_bDrag = true;
				break;
			}

		case WM_LBUTTONUP:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK)) {
					p_this->execAliasEx(TEXT("%s,%u"), TEXT("lbup"), 0);
					p_this->execAliasEx(TEXT("%s,%u"), TEXT("sclick"), 0);
				}
				break;
			}

		case WM_LBUTTONDBLCLK:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK)) {
					p_this->execAliasEx(TEXT("%s,%u"), TEXT("dclick"), 0);
					p_this->execAliasEx(TEXT("%s,%u"), TEXT("lbdblclk"), 0);
				}
				break;
			}

		case WM_RBUTTONDOWN:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK))
					p_this->execAliasEx(TEXT("%s,%u"), TEXT("rbdown"), 0);
				break;
			}

		case WM_RBUTTONUP:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK)) {
					p_this->execAliasEx(TEXT("%s,%u"), TEXT("rbup"), 0);
					p_this->execAliasEx(TEXT("%s,%u"), TEXT("rclick"), 0);
				}
				break;
			}

		case WM_RBUTTONDBLCLK:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK))
					p_this->execAliasEx(TEXT("%s,%u"), TEXT("rbdblclk"), 0);
				break;
			}

		case WM_MBUTTONDOWN:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK))
					p_this->execAliasEx(TEXT("%s,%u"), TEXT("mbdown"), 0);
				break;
			}

		case WM_MBUTTONUP:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK))
					p_this->execAliasEx(TEXT("%s,%u"), TEXT("mbup"), 0);
				break;
			}

		case WM_MBUTTONDBLCLK:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK))
					p_this->execAliasEx(TEXT("%s,%u"), TEXT("mbdblclk"), 0);
				break;
			}

		case WM_MOUSEWHEEL:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK)) {
					const auto fwKeys = GET_KEYSTATE_WPARAM(wParam);
					const auto zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
					TString flags(TEXT('+'));

					if (dcx_testflag(fwKeys, MK_CONTROL)) // control button
						flags += TEXT('c');
					if (dcx_testflag(fwKeys, MK_LBUTTON)) // left mouse button
						flags += TEXT('l');
					if (dcx_testflag(fwKeys, MK_MBUTTON)) // middle mouse button button
						flags += TEXT('m');
					if (dcx_testflag(fwKeys, MK_RBUTTON)) // right mouse button
						flags += TEXT('r');
					if (dcx_testflag(fwKeys, MK_SHIFT)) // shift button
						flags += TEXT('s');

					p_this->execAliasEx(TEXT("%s,%u,%s,%s"),
						TEXT("mwheel"),
						p_this->m_MouseID,
						(zDelta > 0 ? TEXT("up") : TEXT("down")),
						flags.to_chr());

					bParsed = TRUE;
					lRes = FALSE; // stop parsing of WM_MOUSEWHEEL
				}
				break;
			}

		case WM_INITMENU:
		case WM_INITMENUPOPUP:
			{
				if (HIWORD(lParam) == FALSE && p_this->m_popup != nullptr) {
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
				if (p_this->m_popup != nullptr && m_bIsMenuBar && !m_bIsSysMenu)
					p_this->m_popup->deleteAllItemData((HMENU) wParam);

				break;
			}

		case WM_SETCURSOR:
			{
				if ((LOWORD(lParam) == HTCLIENT) && ((HWND) wParam == p_this->getHwnd()) && (p_this->getCursor() != nullptr))
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
				dcxwParam(HDROP, files);

				TCHAR filename[500];
				const auto count = DragQueryFile(files, 0xFFFFFFFF, filename, 500);

				if (count > 0) {
					if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_DRAG)) {
						TCHAR ret[20];

						p_this->evalAliasEx(ret, 19, TEXT("%s,%u,%u"), TEXT("dragbegin"), 0, count);

						// cancel drag drop event
						if (lstrcmpi(ret, TEXT("cancel")) == 0) {
							DragFinish(files);
							bParsed = TRUE;
							break;
						}
						// for each file, send callback message
						for (auto i = decltype(count){0}; i < count; i++) {
							if (DragQueryFile(files, i, filename, 500))
								p_this->execAliasEx(TEXT("%s,%u,%s"), TEXT("dragfile"), 0, filename);
						}

						p_this->execAliasEx(TEXT("%s,%u"), TEXT("dragfinish"), 0);
					}
				}

				DragFinish(files);
				break;
			}

		case WM_ACTIVATE:
			{
				//if (wParam == WA_ACTIVE && p_this->m_bVistaStyle) {
				//	bParsed = TRUE;
				//	lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
				//	InvalidateRect(mHwnd, nullptr, TRUE);
				//}
//
				//if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_FOCUS)) {
				//	switch (wParam) {
				//	case WA_ACTIVE:
				//	case WA_CLICKACTIVE:
				//		{
				//			p_this->execAliasEx(TEXT("%s,%u"), TEXT("activate"), 0);
				//			break;
				//		}
//
				//	case WA_INACTIVE:
				//		{
				//			p_this->execAliasEx(TEXT("%s,%u"), TEXT("deactivate"), 0);
				//			break;
				//		}
				//	} // switch
				//}
				//break;

				switch (wParam) {
				case WA_ACTIVE:
				{
					if (p_this->m_bVistaStyle) {
						bParsed = TRUE;
						lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
						InvalidateRect(mHwnd, nullptr, TRUE);
					}
				}
				case WA_CLICKACTIVE:
				{
					if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_FOCUS))
						p_this->execAliasEx(TEXT("%s,%u"), TEXT("activate"), 0);
					p_this->m_bDialogActive = true;
					break;
				}

				case WA_INACTIVE:
				{
					if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_FOCUS))
						p_this->execAliasEx(TEXT("%s,%u"), TEXT("deactivate"), 0);
					p_this->m_bDialogActive = false;
					break;
				}
				} // switch
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
				// no shadow in v3

				//if (p_this->m_Shadow.bUpdate && p_this->isShadowed())
				//{
				//	p_this->UpdateShadow();
				//	p_this->m_Shadow.bUpdate = false;
				//}

				if (p_this->IsVistaStyle() && !IsIconic(mHwnd)) {
					ValidateRect(mHwnd, nullptr);
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
			//			//InvalidateRect(mHwnd, nullptr, FALSE);
			//			p_this->UpdateVistaStyle();
			//			bParsed = TRUE;
			//		}
			//	}
			//	break;

		//case WM_SHOWWINDOW:
		//	{
		//		if(p_this->m_Shadow.Status & DCX_SS_ENABLED && !(p_this->m_Shadow.Status & DCX_SS_DISABLEDBYAERO))
		//		{
		//			lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
		//			if(!wParam)	// the window is being hidden
		//			{
		//				ShowWindow(p_this->m_Shadow.hWin, SW_HIDE);
		//				p_this->m_Shadow.Status &= ~(DCX_SS_VISABLE | DCX_SS_PARENTVISIBLE);
		//			}
		//			else
		//			{
		//				p_this->m_Shadow.bUpdate = true;
		//				p_this->ShowShadow();
		//			}
		//			bParsed = TRUE;
		//		}
		//	}
		//	break;

		//case WM_DWMCOMPOSITIONCHANGED:
		//	{
		//		if (p_this->isShadowed()) {
		//			if (Dcx::VistaModule.refreshComposite())
		//				p_this->m_Shadow.Status |= DCX_SS_DISABLEDBYAERO;
		//			else
		//				p_this->m_Shadow.Status &= ~DCX_SS_DISABLEDBYAERO;
//
		//			p_this->ShowShadow();
		//		}
		//	}
		//	break;

		case WM_KEYDOWN:
		{
			if (dcx_testflag(p_this->getEventMask(), DCX_EVENT_EDIT)) {
				if (wParam == VK_RETURN)
					p_this->execAliasEx(TEXT("%s,%d"), TEXT("return"), 0);

				//if ((p_this->m_bIgnoreRepeat) && (lParam & 0x40000000)) // ignore repeats
					// break;

				p_this->execAliasEx(TEXT("%s,%d,%d"), TEXT("keydown"), 0, wParam);
			}
			break;
		}

		case WM_KEYUP:
		{
			if (dcx_testflag(p_this->getEventMask(), DCX_EVENT_EDIT))
				p_this->execAliasEx(TEXT("%s,%d,%d"), TEXT("keyup"), 0, wParam);
			break;
		}

		case WM_NCDESTROY:
			{
				if (IsWindow(mHwnd))
				{
					if ((WNDPROC)GetWindowLongPtr(mHwnd, GWLP_WNDPROC) == DcxDialog::WindowProc)
						SubclassWindow(mHwnd, p_this->m_hOldWindowProc);
				}

				lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);

				Dcx::Dialogs.deleteDialog(p_this);
				delete p_this;
				return lRes;
			}

		default:
			{
				// Try to process DragList events
				if (!p_this->m_vDragLists.empty())
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

const TString &DcxDialog::getParentName() const noexcept {
	return this->m_tsParentName;
}

void DcxDialog::DrawDialogBackground(HDC hdc, DcxDialog *const p_this, LPCRECT rwnd)
{
	// background color
	if (p_this->getBackClrBrush() != nullptr)
		FillRect(hdc, rwnd, p_this->getBackClrBrush());
	else
		FillRect(hdc, rwnd, GetSysColorBrush(COLOR_3DFACE));

	if (p_this->m_bitmapBg == nullptr)
		return;

	BITMAP bmp;

	if (GetObject(p_this->m_bitmapBg, sizeof(BITMAP), &bmp) == 0)
		return;

	auto hdcbmp = CreateCompatibleDC(hdc);

	if (hdcbmp == nullptr)
		return;

	Auto(DeleteDC(hdcbmp));

	//Dcx::dcxHDC hdcbmp(hdc);

	auto hOldBitmap = SelectBitmap(hdcbmp, p_this->m_bitmapBg);

	Auto(SelectBitmap(hdcbmp, hOldBitmap));

	auto x = rwnd->left;
	auto y = rwnd->top;
	const auto w = rwnd->right - rwnd->left;
	const auto h = rwnd->bottom - rwnd->top;

	// stretch
	if (dcx_testflag(p_this->m_uStyleBg, DBS_BKGSTRETCH)) {
		//BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcbmp, 0, 0, SRCCOPY);
		TransparentBlt(hdc, x, y, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, p_this->m_colTransparentBg);
	}
	// tile
	else if (dcx_testflag(p_this->m_uStyleBg, DBS_BKGTILE)) {
		for (y = 0; y < h; y += bmp.bmHeight) {
			for (x = 0; x < w; x += bmp.bmWidth) {
				//BitBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, hdcbmp, 0, 0, SRCCOPY);
				TransparentBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, p_this->m_colTransparentBg);
			}
		}
	}
	// normal
	//else if (dcx_testflag(p_this->m_uStyleBg, DBS_BKGNORMAL)) {
	else {
		// align horizontally
		if (dcx_testflag(p_this->m_uStyleBg, DBS_BKGCENTER))
			x = (w - bmp.bmWidth) / 2;
		else if (dcx_testflag(p_this->m_uStyleBg, DBS_BKGRIGHT))
			x = w - bmp.bmWidth;

		// align vertically
		if (dcx_testflag(p_this->m_uStyleBg, DBS_BKGVCENTER))
			y = (h - bmp.bmHeight) / 2;
		else if (dcx_testflag(p_this->m_uStyleBg, DBS_BKGBOTTOM))
			y = h - bmp.bmHeight;

		TransparentBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, p_this->m_colTransparentBg);
	}
}

//// CWndShadow ...
////
//// Copyright (c) 2006 Perry Zhu, All Rights Reserved.
////
//// mailto:perry@live.com
////
//// This code has been modified for use with DCX, & is used for the DcxDialog Shadows.
//
//const bool DcxDialog::AddShadow(void)
//{
//	if (!this->isShadowed()) {
//		// Create the shadow window
//		this->m_Shadow.hWin = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, DCX_SHADOWCLASS, nullptr,
//			WS_CAPTION | WS_POPUPWINDOW, CW_USEDEFAULT, 0, 0, 0, this->m_Hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
//
//		if (!IsWindow(this->m_Shadow.hWin))
//			return false;
//
//		this->m_Shadow.Status = DCX_SS_ENABLED;
//
//		if (Dcx::VistaModule.isAero())
//			this->m_Shadow.Status |= DCX_SS_DISABLEDBYAERO;
//
//		this->ShowShadow();
//
//		return true;
//	}
//	return false;
//}
//
//void DcxDialog::ShowShadow(void)
//{
//	// Clear all except the enabled status
//	this->m_Shadow.Status &= DCX_SS_ENABLED | DCX_SS_DISABLEDBYAERO;
//
//	if((this->m_Shadow.Status & DCX_SS_ENABLED) && !(this->m_Shadow.Status & DCX_SS_DISABLEDBYAERO))	// Enabled
//	{
//		// Determine the show state of shadow according to parent window's state
//		const DWORD lParentStyle = GetWindowStyle(this->m_Hwnd);
//
//		if(WS_VISIBLE & lParentStyle)	// Parent visible
//		{
//			this->m_Shadow.Status |= DCX_SS_PARENTVISIBLE;
//
//			// Parent is normal, show the shadow
//			if(!((WS_MAXIMIZE | WS_MINIMIZE) & lParentStyle))	// Parent visible but does not need shadow
//				this->m_Shadow.Status |= DCX_SS_VISABLE;
//		}
//	}
//
//	if(this->m_Shadow.Status & DCX_SS_VISABLE)
//	{
//		ShowWindow(this->m_Shadow.hWin, SW_SHOWNA);
//		this->UpdateShadow();
//	}
//	else
//		ShowWindow(this->m_Shadow.hWin, SW_HIDE);
//}
//
//void DcxDialog::RemoveShadow(void)
//{
//	if (IsWindow(this->m_Shadow.hWin))
//		DestroyWindow(this->m_Shadow.hWin);
//}
//
//void DcxDialog::UpdateShadow(void)
//{
//	RECT WndRect;
//	GetWindowRect(this->m_Hwnd, &WndRect);
//	const int nShadWndWid = WndRect.right - WndRect.left + this->m_Shadow.nSize * 2;
//	const int nShadWndHei = WndRect.bottom - WndRect.top + this->m_Shadow.nSize * 2;
//
//	// Create the alpha blending bitmap
//	BITMAPINFO bmi;        // bitmap header
//
//	ZeroMemory(&bmi, sizeof(BITMAPINFO));
//	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//	bmi.bmiHeader.biWidth = nShadWndWid;
//	bmi.bmiHeader.biHeight = nShadWndHei;
//	bmi.bmiHeader.biPlanes = 1;
//	bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
//	bmi.bmiHeader.biCompression = BI_RGB;
//	bmi.bmiHeader.biSizeImage = nShadWndWid * nShadWndHei * 4;
//
//	BYTE *pvBits;          // pointer to DIB section
//	HBITMAP hbitmap = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, (void **)&pvBits, nullptr, 0);
//
//	if (hbitmap == nullptr)
//		return;
//
//	ZeroMemory(pvBits, bmi.bmiHeader.biSizeImage);
//	MakeShadow((UINT32 *)pvBits, this->m_Hwnd, &WndRect);
//
//	HDC hMemDC = CreateCompatibleDC(nullptr);
//	if (hMemDC != nullptr)
//	{
//		HBITMAP hOriBmp = SelectBitmap(hMemDC, hbitmap);
//
//		POINT ptDst = {WndRect.left + this->m_Shadow.nxOffset - this->m_Shadow.nSize, WndRect.top + this->m_Shadow.nyOffset - this->m_Shadow.nSize};
//		POINT ptSrc = {0, 0};
//		SIZE WndSize = {nShadWndWid, nShadWndHei};
//		BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
//
//		MoveWindow(this->m_Shadow.hWin, ptDst.x, ptDst.y, nShadWndWid, nShadWndHei, FALSE);
//
//		UpdateLayeredWindow(this->m_Shadow.hWin, nullptr, &ptDst, &WndSize, hMemDC, &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
//
//		// Delete used resources
//		DeleteBitmap(SelectBitmap(hMemDC, hOriBmp));
//		DeleteDC(hMemDC);
//	}
//}
//
//const bool DcxDialog::isShadowed(void) const
//{
//	return (IsWindow(this->m_Shadow.hWin) ? true : false);
//}
//
//void DcxDialog::MakeShadow(UINT32 *const pShadBits, const HWND hParent, const RECT *const rcParent)
//{
//	// The shadow algorithm:
//	// Get the region of parent window,
//	// Apply morphologic erosion to shrink it into the size (ShadowWndSize - Sharpness)
//	// Apply modified (with blur effect) morphologic dilation to make the blurred border
//	// The algorithm is optimized by assuming parent window is just "one piece" and without "wholes" on it
//
//	// Get the region of parent window,
//	// Create a full rectangle region in case of the window region is not defined
//	HRGN hParentRgn = CreateRectRgn(0, 0, rcParent->right - rcParent->left, rcParent->bottom - rcParent->top);
//	GetWindowRgn(hParent, hParentRgn);
//
//	// Determine the Start and end point of each horizontal scan line
//	const SIZE szParent = {rcParent->right - rcParent->left, rcParent->bottom - rcParent->top};
//	const SIZE szShadow = {szParent.cx + 2 * this->m_Shadow.nSize, szParent.cy + 2 * this->m_Shadow.nSize};
//	// Extra 2 lines (set to be empty) in ptAnchors are used in dilation
//	const int nAnchors = max(szParent.cy, szShadow.cy);	// # of anchor points pares
//	int (*ptAnchors)[2] = new int[nAnchors + 2][2];
//	int (*ptAnchorsOri)[2] = new int[szParent.cy][2];	// anchor points, will not modify during erosion
//	ptAnchors[0][0] = szParent.cx;
//	ptAnchors[0][1] = 0;
//	ptAnchors[nAnchors + 1][0] = szParent.cx;
//	ptAnchors[nAnchors + 1][1] = 0;
//	if(this->m_Shadow.nSize > 0)
//	{
//		// Put the parent window anchors at the center
//		for(int i = 0; i < this->m_Shadow.nSize; i++)
//		{
//			ptAnchors[i + 1][0] = szParent.cx;
//			ptAnchors[i + 1][1] = 0;
//			ptAnchors[szShadow.cy - i][0] = szParent.cx;
//			ptAnchors[szShadow.cy - i][1] = 0;
//		}
//		ptAnchors += this->m_Shadow.nSize;
//	}
//	for(int i = 0; i < szParent.cy; i++)
//	{
//		// find start point
//		int j;
//		for(j = 0; j < szParent.cx; j++)
//		{
//			if(PtInRegion(hParentRgn, j, i))
//			{
//				ptAnchors[i + 1][0] = j + this->m_Shadow.nSize;
//				ptAnchorsOri[i][0] = j;
//				break;
//			}
//		}
//
//		if(j >= szParent.cx)	// Start point not found
//		{
//			ptAnchors[i + 1][0] = szParent.cx;
//			ptAnchorsOri[i][1] = 0;
//			ptAnchors[i + 1][0] = szParent.cx;
//			ptAnchorsOri[i][1] = 0;
//		}
//		else
//		{
//			// find end point
//			for(j = szParent.cx - 1; j >= ptAnchors[i + 1][0]; j--)
//			{
//				if(PtInRegion(hParentRgn, j, i))
//				{
//					ptAnchors[i + 1][1] = j + 1 + this->m_Shadow.nSize;
//					ptAnchorsOri[i][1] = j + 1;
//					break;
//				}
//			}
//		}
//		//if(0 != ptAnchorsOri[i][1])
//		//	TRACE(TEXT("%d %d\n"), ptAnchorsOri[i][0], ptAnchorsOri[i][1]);
//	}
//
//	if(this->m_Shadow.nSize > 0)
//		ptAnchors -= this->m_Shadow.nSize;	// Restore pos of ptAnchors for erosion
//	int (*ptAnchorsTmp)[2] = new int[nAnchors + 2][2];	// Store the result of erosion
//	// First and last line should be empty
//	ptAnchorsTmp[0][0] = szParent.cx;
//	ptAnchorsTmp[0][1] = 0;
//	ptAnchorsTmp[nAnchors + 1][0] = szParent.cx;
//	ptAnchorsTmp[nAnchors + 1][1] = 0;
//	int nEroTimes = 0;
//	// morphologic erosion
//	for(int i = 0; i < this->m_Shadow.nSharpness - this->m_Shadow.nSize; i++)
//	{
//		nEroTimes++;
//		//ptAnchorsTmp[1][0] = szParent.cx;
//		//ptAnchorsTmp[1][1] = 0;
//		//ptAnchorsTmp[szParent.cy + 1][0] = szParent.cx;
//		//ptAnchorsTmp[szParent.cy + 1][1] = 0;
//		for(int j = 1; j < nAnchors + 1; j++)
//		{
//			ptAnchorsTmp[j][0] = max(ptAnchors[j - 1][0], max(ptAnchors[j][0], ptAnchors[j + 1][0])) + 1;
//			ptAnchorsTmp[j][1] = min(ptAnchors[j - 1][1], min(ptAnchors[j][1], ptAnchors[j + 1][1])) - 1;
//		}
//		// Exchange ptAnchors and ptAnchorsTmp;
//		int (*ptAnchorsXange)[2] = ptAnchorsTmp;
//		ptAnchorsTmp = ptAnchors;
//		ptAnchors = ptAnchorsXange;
//		//std::swap(ptAnchors, ptAnchorsTmp);
//	}
//
//	// morphologic dilation
//	ptAnchors += (this->m_Shadow.nSize < 0 ? -this->m_Shadow.nSize : 0) + 1;	// now coordinates in ptAnchors are same as in shadow window
//	// Generate the kernel
//	const int nKernelSize = this->m_Shadow.nSize > this->m_Shadow.nSharpness ? this->m_Shadow.nSize : this->m_Shadow.nSharpness;
//	const int nCenterSize = this->m_Shadow.nSize > this->m_Shadow.nSharpness ? (this->m_Shadow.nSize - this->m_Shadow.nSharpness) : 0;
//	UINT32 *pKernel = new UINT32[(2 * nKernelSize + 1) * (2 * nKernelSize + 1)];
//	UINT32 *pKernelIter = pKernel;
//	const DWORD preColDark = PreMultiply(this->m_Shadow.Color, this->m_Shadow.nDarkness);
//	for(int i = 0; i <= 2 * nKernelSize; i++)
//	{
//		for(int j = 0; j <= 2 * nKernelSize; j++)
//		{
//			const double dLength = sqrt((i - nKernelSize) * (i - nKernelSize) + (j - nKernelSize) * (double)(j - nKernelSize));
//			if(dLength < nCenterSize)
//				*pKernelIter = this->m_Shadow.nDarkness << 24 | preColDark;
//			else if(dLength <= nKernelSize)
//			{
//				const UINT32 nFactor = ((UINT32)((1 - (dLength - nCenterSize) / (this->m_Shadow.nSharpness + 1)) * this->m_Shadow.nDarkness));
//				*pKernelIter = nFactor << 24 | PreMultiply(this->m_Shadow.Color, (BYTE)(nFactor & 0xFF));
//				// TODO: Examin this nFactor usage in PreMultiply() as its converting a UINT32 to an unsigned char
//				// changed nFactor to (BYTE)(nFactor & 0xFF) to mask out > byte bits.
//			}
//			else
//				*pKernelIter = 0;
//			//TRACE(TEXT("%d "), *pKernelIter >> 24);
//			pKernelIter ++;
//		}
//		//TRACE(TEXT("\n"));
//	}
//	// Generate blurred border
//	for(int i = nKernelSize; i < szShadow.cy - nKernelSize; i++)
//	{
//		if(ptAnchors[i][0] < ptAnchors[i][1])
//		{
//			int j;
//
//			// Start of line
//			for(j = ptAnchors[i][0];
//				j < min(max(ptAnchors[i - 1][0], ptAnchors[i + 1][0]) + 1, ptAnchors[i][1]);
//				j++)
//			{
//				for(int k = 0; k <= 2 * nKernelSize; k++)
//				{
//					UINT32 *pPixel = pShadBits +
//						(szShadow.cy - i - 1 + nKernelSize - k) * szShadow.cx + j - nKernelSize;
//					UINT32 *pKernelPixel = pKernel + k * (2 * nKernelSize + 1);
//					for(int l = 0; l <= 2 * nKernelSize; l++)
//					{
//						if(*pPixel < *pKernelPixel)
//							*pPixel = *pKernelPixel;
//						pPixel++;
//						pKernelPixel++;
//					}
//				}
//			}	// for() start of line
//
//			// End of line
//			for(j = max(j, min(ptAnchors[i - 1][1], ptAnchors[i + 1][1]) - 1);
//				j < ptAnchors[i][1];
//				j++)
//			{
//				for(int k = 0; k <= 2 * nKernelSize; k++)
//				{
//					UINT32 *pPixel = pShadBits +
//						(szShadow.cy - i - 1 + nKernelSize - k) * szShadow.cx + j - nKernelSize;
//					UINT32 *pKernelPixel = pKernel + k * (2 * nKernelSize + 1);
//					for(int l = 0; l <= 2 * nKernelSize; l++)
//					{
//						if(*pPixel < *pKernelPixel)
//							*pPixel = *pKernelPixel;
//						pPixel++;
//						pKernelPixel++;
//					}
//				}
//			}	// for() end of line
//
//		}
//	}	// for() Generate blurred border
//
//	// Erase unwanted parts and complement missing
//	const UINT32 clCenter = this->m_Shadow.nDarkness << 24 | preColDark;
//	for(int i = min(nKernelSize, max(this->m_Shadow.nSize - this->m_Shadow.nyOffset, 0));
//		i < max(szShadow.cy - nKernelSize, min(szParent.cy + this->m_Shadow.nSize - this->m_Shadow.nyOffset, szParent.cy + 2 * this->m_Shadow.nSize));
//		i++)
//	{
//		UINT32 *pLine = pShadBits + (szShadow.cy - i - 1) * szShadow.cx;
//		if(i - this->m_Shadow.nSize + this->m_Shadow.nyOffset < 0 || i - this->m_Shadow.nSize + this->m_Shadow.nyOffset >= szParent.cy)	// Line is not covered by parent window
//		{
//			for(int j = ptAnchors[i][0]; j < ptAnchors[i][1]; j++)
//			{
//				*(pLine + j) = clCenter;
//			}
//		}
//		else
//		{
//			for(int j = ptAnchors[i][0];
//				j < min(ptAnchorsOri[i - this->m_Shadow.nSize + this->m_Shadow.nyOffset][0] + this->m_Shadow.nSize - this->m_Shadow.nxOffset, ptAnchors[i][1]);
//				j++)
//				*(pLine + j) = clCenter;
//			for(int j = max(ptAnchorsOri[i - this->m_Shadow.nSize + this->m_Shadow.nyOffset][0] + this->m_Shadow.nSize - this->m_Shadow.nxOffset, 0);
//				j < min(ptAnchorsOri[i - this->m_Shadow.nSize + this->m_Shadow.nyOffset][1] + this->m_Shadow.nSize - this->m_Shadow.nxOffset, szShadow.cx);
//				j++)
//				*(pLine + j) = 0;
//			for(int j = max(ptAnchorsOri[i - this->m_Shadow.nSize + this->m_Shadow.nyOffset][1] + this->m_Shadow.nSize - this->m_Shadow.nxOffset, ptAnchors[i][0]);
//				j < ptAnchors[i][1];
//				j++)
//				*(pLine + j) = clCenter;
//		}
//	}
//
//	// Delete used resources
//	delete[] (ptAnchors - (this->m_Shadow.nSize < 0 ? -this->m_Shadow.nSize : 0) - 1);
//	delete[] ptAnchorsTmp;
//	delete[] ptAnchorsOri;
//	delete[] pKernel;
//	DeleteObject(hParentRgn);
//}
//
//const bool DcxDialog::SetShadowSize(const int NewSize)
//{
//	if(NewSize > 20 || NewSize < -20)
//		return false;
//
//	this->m_Shadow.nSize = (signed char)NewSize;
//	if(DCX_SS_VISABLE & m_Shadow.Status)
//		UpdateShadow();
//	return true;
//}
//
//const bool DcxDialog::SetShadowSharpness(const UINT NewSharpness)
//{
//	if(NewSharpness > 20)
//		return false;
//
//	this->m_Shadow.nSharpness = (unsigned char)NewSharpness;
//	if(DCX_SS_VISABLE & this->m_Shadow.Status)
//		UpdateShadow();
//	return true;
//}
//
//const bool DcxDialog::SetShadowDarkness(const UINT NewDarkness)
//{
//	if(NewDarkness > 255)
//		return false;
//
//	this->m_Shadow.nDarkness = (unsigned char)NewDarkness;
//	if(DCX_SS_VISABLE & this->m_Shadow.Status)
//		UpdateShadow();
//	return true;
//}
//
//const bool DcxDialog::SetShadowPosition(const int NewXOffset, const int NewYOffset)
//{
//	if(NewXOffset > 20 || NewXOffset < -20 ||
//		NewYOffset > 20 || NewYOffset < -20)
//		return false;
//	
//	this->m_Shadow.nxOffset = (signed char)NewXOffset;
//	this->m_Shadow.nyOffset = (signed char)NewYOffset;
//	if(DCX_SS_VISABLE & this->m_Shadow.Status)
//		UpdateShadow();
//	return true;
//}
//
//const bool DcxDialog::SetShadowColor(const COLORREF NewColor)
//{
//	this->m_Shadow.Color = NewColor;
//	if(DCX_SS_VISABLE & this->m_Shadow.Status)
//		UpdateShadow();
//	return true;
//}
//// .... CWndShadow

/*
**	Show error messages related to /xdialog & $xdialog() calls
*/
void DcxDialog::showError(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const err) const
{
	if (this->m_bErrorTriggered)
		return;

	this->m_bErrorTriggered = true;

	if (this->IsVerbose())
	{
		TString res;
		if (prop != nullptr)
			res.tsprintf(TEXT("D_IERROR xdialog(%s).%s: %s"), this->getName().to_chr(), prop, err);
		else
			res.tsprintf(TEXT("D_CERROR xdialog %s %s: %s"), cmd, this->getName().to_chr(), err);
		mIRCLinker::echo(res.to_chr());
	}

	const_cast<DcxDialog *>(this)->execAliasEx(TEXT("error,0,dialog,%s,%s,%s"), (prop != nullptr ? prop : TEXT("none")), (cmd != nullptr ? cmd : TEXT("none")), err);

	this->m_bErrorTriggered = false;
}

void DcxDialog::showErrorEx(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const fmt, ...) const
{
	TString err;
	va_list args;

	va_start( args, fmt );
	err.tvprintf(fmt, args);
	va_end( args );

	this->showError(prop, cmd, err.to_chr());
}

/*
**	Show error messages related to /xdid & $xdid() calls
*/
void DcxDialog::showControlError(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const err) const
{
	if (this->m_bErrorTriggered)
		return;

	this->m_bErrorTriggered = true;

	if (this->IsVerbose())
	{
		TString res;
		if (prop != nullptr)
			res.tsprintf(TEXT("D_IERROR xdid(%s).%s: %s"), this->getName().to_chr(), prop, err);
		else
			res.tsprintf(TEXT("D_CERROR xdid %s %s: %s"), cmd, this->getName().to_chr(), err);
		mIRCLinker::echo(res.to_chr());
	}

	const_cast<DcxDialog *>(this)->execAliasEx(TEXT("error,0,dialog,%s,%s,%s"), (prop != nullptr ? prop : TEXT("none")), (cmd != nullptr ? cmd : TEXT("none")), err);

	this->m_bErrorTriggered = false;
}

void DcxDialog::showControlErrorEx(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const fmt, ...) const
{
	TString err;
	va_list args;

	va_start(args, fmt);
	err.tvprintf(fmt, args);
	va_end(args);

	this->showControlError(prop, cmd, err.to_chr());
}

void DcxDialog::CreateVistaStyle(void)
{
	if (Dcx::VistaModule.refreshComposite()) {
		// Vista+ only code. dont do anything at this point if vista+

		//DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;
		//
		////enable non-client area rendering on window
		//HRESULT hr = Dcx::VistaModule.dcxDwmSetWindowAttribute(this->m_Hwnd, DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));
		//if (SUCCEEDED(hr)) {
			//MARGINS margins = {this->m_GlassOffsets.left,this->m_GlassOffsets.right,this->m_GlassOffsets.top,this->m_GlassOffsets.bottom};
			//Dcx::VistaModule.dcxDwmExtendFrameIntoClientArea(this->m_Hwnd, &margins);
		//	//DWM_BLURBEHIND bb = {0};
		//
		//	//// Specify blur-behind and blur region.
		//	//bb.dwFlags = DWM_BB_ENABLE;
		//	//bb.fEnable = true;
		//	//bb.hRgnBlur = nullptr;
		//
		//	//// Enable blur-behind.
		//	//Dcx::VistaModule.dcxDwmEnableBlurBehindWindow(this->m_Hwnd, &bb);
		//
		//	//this->m_bVistaStyle = true;
		//}
		return;
	}
#ifdef DCX_USE_GDIPLUS
	// don't use this style with aero, needs specific code to allow aero to do these effects for us.
	if (Dcx::GDIModule.isUseable() && !Dcx::VistaModule.isAero()) {
		// this code is for windows 2000 & windows XP
		RECT rc;
		if (!GetWindowRect(this->m_Hwnd, &rc))
			return;

		const DWORD ExStyles = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_LEFT;
		const DWORD Styles = WS_VISIBLE/*|WS_OVERLAPPED*/|WS_CLIPCHILDREN;
		SIZE szWin;
		szWin.cx = (rc.right - rc.left);
		szWin.cy = (rc.bottom - rc.top);
		this->m_hFakeHwnd = CreateWindowEx(ExStyles,DCX_VISTACLASS,nullptr,Styles,rc.left,rc.top,szWin.cx,szWin.cy,this->m_Hwnd,nullptr,GetModuleHandle(nullptr), nullptr);
		if (IsWindow(this->m_hFakeHwnd)) {
			if (this->CreateVistaStyleBitmap(szWin))
			{
				SetLayeredWindowAttributes(this->m_Hwnd,0,5,LWA_ALPHA);
				auto hRgn = CreateRectRgn(0,0,0,0);
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

const bool DcxDialog::CreateVistaStyleBitmap(const SIZE &szWin)
{
#ifdef DCX_USE_GDIPLUS
	if (this->m_hVistaBitmap != nullptr)
		DeleteBitmap(this->m_hVistaBitmap);

	BITMAPINFOHEADER bmih = { 0 };
	const auto nBytesPerLine = ((szWin.cx * 32 + 31) & (~31)) >> 3;
	// Populate BITMAPINFO header
	bmih.biSize=sizeof(BITMAPINFOHEADER);
	bmih.biWidth=szWin.cx;
	bmih.biHeight=-szWin.cy; // make this a top down bitmap
	bmih.biPlanes=1;
	bmih.biBitCount=32;
	bmih.biCompression=BI_RGB;
	bmih.biClrUsed=0;
	bmih.biSizeImage=nBytesPerLine*szWin.cy;

	this->m_hVistaBitmap = ::CreateDIBSection(nullptr, (PBITMAPINFO)&bmih, DIB_RGB_COLORS, &this->m_pVistaBits, nullptr, 0);
	if (this->m_hVistaBitmap != nullptr) {
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
	if (this->m_hVistaBitmap != nullptr)
		DeleteBitmap(this->m_hVistaBitmap);
	this->m_hFakeHwnd = nullptr;
}

#ifdef DCX_USE_GDIPLUS
void DcxDialog::DrawCaret(Graphics & graph)
{
	auto pWnd = GetFocus();
	if( pWnd == nullptr || !IsWindow(pWnd) )
		return;

	TCHAR tszBuf[MAX_PATH] = {TEXT('\0')};
	::GetClassName( pWnd, tszBuf, MAX_PATH);
	TString strClassName(tszBuf);

	if( strClassName != TEXT("EDIT"))
		return;

	POINT pt;
	if (GetCaretPos(&pt))
	{
		MapWindowPoints(pWnd, this->m_hFakeHwnd, &pt, 1);
		Pen pen(Color(0, 0, 0), 1.0f);	// black
		graph.DrawLine(&pen, pt.x, pt.y, pt.x, pt.y + 13);
	}
}

void DcxDialog::DrawDialog( Graphics & graphics, HDC hDC)
{
	RECT rc;
	if (!GetWindowRect(this->m_Hwnd, &rc))
		return;

	auto hdcMemory = ::CreateCompatibleDC(hDC);

	if (hdcMemory == nullptr)
		return;

	Auto(DeleteDC(hdcMemory));

	auto w = (rc.right - rc.left), h = (rc.bottom - rc.top);
	auto hBitmap = ::CreateCompatibleBitmap(hDC, w, h);

	if (hBitmap == nullptr)
		return;

	Auto(DeleteBitmap(hBitmap));

	auto hbmpOld = SelectBitmap(hdcMemory, hBitmap);

	Auto(SelectBitmap(hdcMemory, hbmpOld));

	SendMessage(this->m_Hwnd, WM_PRINT, (WPARAM)hdcMemory,PRF_CLIENT|PRF_NONCLIENT|PRF_ERASEBKGND);

	Bitmap bitmap( hBitmap, nullptr);
	graphics.DrawImage( &bitmap, 0, 0);
}

void DcxDialog::DrawCtrl( Graphics & graphics, HDC hDC, HWND hWnd)
{
	if( !::IsWindow(hWnd) || !::IsWindowVisible(hWnd) )
		return;

	RECT rc;
	if (!GetWindowRect(hWnd, &rc))
		return;

	this->MapVistaRect(nullptr, &rc);

	const auto w = (rc.right - rc.left), h = (rc.bottom - rc.top);

	// Don't bother drawing if its not visible in clip rect.
	if (!graphics.IsVisible(rc.left, rc.top, w, h))
		return;

	auto hdcMemory = ::CreateCompatibleDC(hDC);

	if (hdcMemory == nullptr)
		return;

	Auto(DeleteDC(hdcMemory));

	auto hBitmap = ::CreateCompatibleBitmap(hDC, w, h);

	if (hBitmap == nullptr)
		return;

	Auto(DeleteBitmap(hBitmap));

	auto hbmpOld = SelectBitmap(hdcMemory, hBitmap);

	Auto(SelectBitmap(hdcMemory, hbmpOld));

	SendMessage(hWnd, WM_PRINT, (WPARAM)hdcMemory, (LPARAM)PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE | PRF_ERASEBKGND);

	Bitmap bitmap( hBitmap, nullptr);
	graphics.DrawImage( &bitmap, rc.left, rc.top);
}
#endif

void DcxDialog::UpdateVistaStyle(const LPRECT rcUpdate)
{
#ifdef DCX_USE_GDIPLUS
	if (!IsWindow(this->m_hFakeHwnd) || !IsWindowVisible(this->m_Hwnd) || IsIconic(this->m_Hwnd) || this->m_hVistaBitmap == nullptr)
		return;

	{ // maintain a matching region.
		auto hRgn = CreateRectRgn(0, 0, 0, 0);
		if (GetWindowRgn(this->m_Hwnd, hRgn))
		{
			auto hFakeRgn = CreateRectRgn(0, 0, 0, 0);
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
	RECT rc, rcParentWin, rcParentClient, rcClip, glassOffsets;
	if (!GetWindowRect(this->m_hFakeHwnd, &rc))
		return;

	if (!SetRectEmpty(&rcClip))
		return;

	if (!GetWindowRect(this->m_Hwnd, &rcParentWin))
		return;

	if (!GetClientRect(this->m_Hwnd, &rcParentClient))
		return;

	MapWindowRect(this->m_Hwnd, nullptr, &rcParentClient);

	POINT ptSrc = { 0, 0};
	POINT ptWinPos = { rc.left, rc.top};
	SIZE szWin = { (rc.right - rc.left), (rc.bottom - rc.top) };

	auto alpha = this->m_iAlphaLevel;
	if (this->m_bGhosted)
		alpha = this->m_bDoGhostDrag;

	const auto half_alpha = (BYTE)(alpha / 2);

	BLENDFUNCTION stBlend = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };

	auto hDC = ::GetDC(this->m_hFakeHwnd);
	if (hDC == nullptr)
		return;

	Auto(ReleaseDC(this->m_hFakeHwnd, hDC));

	auto hdcMemory = ::CreateCompatibleDC(hDC);

	if (hdcMemory == nullptr)
		return;

	Auto(DeleteDC(hdcMemory));

	auto pvBits = this->m_pVistaBits;
	auto hbmpMem = this->m_hVistaBitmap;

	auto hbmpOld = SelectBitmap(hdcMemory, hbmpMem);

	Auto(SelectBitmap(hdcMemory, hbmpOld));

	this->m_hVistaHDC = hdcMemory;

	Graphics graph(hdcMemory);
	graph.SetPageScale(1.0);
	graph.SetPageUnit(UnitPixel);
	graph.SetSmoothingMode(SmoothingModeNone);

	const SIZE offsets = { (rcParentClient.left - rcParentWin.left), (rcParentClient.top - rcParentWin.top) };
	//offsets.cx = (rcParentClient.left - rcParentWin.left);
	//offsets.cy = (rcParentClient.top - rcParentWin.top);
	this->m_sVistaOffsets = offsets;

	// Glass area = window edge +/- offset
	glassOffsets.left = offsets.cx + this->m_GlassOffsets.left;
	glassOffsets.top = offsets.cy + this->m_GlassOffsets.top;
	glassOffsets.right = szWin.cx - ((rcParentWin.right - rcParentClient.right) + this->m_GlassOffsets.right);
	glassOffsets.bottom = szWin.cy - ((rcParentWin.bottom - rcParentClient.bottom) + this->m_GlassOffsets.bottom);

	// Check for update rect (area of child control in screen coordinates)
	// If found set clipping area as child control's area.
	if (rcUpdate != nullptr) {
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
		const auto Yend = min(rcClip.bottom, szWin.cy), Ybase = max(rcClip.top, 0);
		const auto Xend = min(rcClip.right, szWin.cx), Xbase = max(rcClip.left, 0);
		POINT pt;

		for( auto y = Ybase; y < Yend; y++)
		{
			// (szWin.cx * 4 * y) == row
			// (Xbase * 4) == offset within row.
			PBYTE pPixel = ((PBYTE)pvBits) + (szWin.cx * 4 * y) + (Xbase * 4);

			pt.y = y; //szWin.cy - y;

			for (auto x = Xbase; x < Xend; x++)
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
	auto hwndChild = ::GetWindow(this->m_Hwnd, GW_CHILD);
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
	//
	//		for( int x = 0; x < szWin.cx; x++)
	//		{
	//			if( alpha < pPixel[3] )
	//			{
	//				pPixel[3] = alpha;
	//
	//				pPixel[0] = pPixel[0] * pPixel[3] / 255;
	//				pPixel[1] = pPixel[1] * pPixel[3] / 255;
	//				pPixel[2] = pPixel[2] * pPixel[3] / 255;
	//			}
	//
	//			pPixel += 4;
	//		}
	//	}
	//}

	// NB: Unable to combine ULW_COLORKEY & ULW_ALPHA for some reason...
	UpdateLayeredWindow( this->m_hFakeHwnd, hDC, &ptWinPos, &szWin, hdcMemory, &ptSrc, 0, &stBlend, ULW_ALPHA);

	this->m_hVistaHDC = nullptr;

	graph.ReleaseHDC(hdcMemory);
#endif
}

void DcxDialog::SetVistaStylePos(void)
{
	if (!IsWindow(this->m_hFakeHwnd))
		return;

	RECT rc;
	if (GetWindowRect(this->m_Hwnd, &rc))
		SetWindowPos(this->m_hFakeHwnd, nullptr, rc.left, rc.top, 0,0, SWP_NOSIZE|SWP_NOZORDER);
}

void DcxDialog::SetVistaStyleSize(void)
{
	if (!IsWindow(this->m_hFakeHwnd))
		return;

	RECT rc;
	if (GetWindowRect(this->m_Hwnd, &rc))
	{
		const SIZE szWin = { (rc.right - rc.left), (rc.bottom - rc.top) };
		this->CreateVistaStyleBitmap(szWin);
		SetWindowPos(this->m_hFakeHwnd, nullptr, 0, 0, szWin.cx, szWin.cy, SWP_NOMOVE | SWP_NOZORDER);
	}
}

void DcxDialog::MapVistaRect(HWND hwnd, LPRECT rc) const
{
	MapWindowRect(hwnd, this->m_Hwnd, rc);
	OffsetRect(rc, this->m_sVistaOffsets.cx, this->m_sVistaOffsets.cy);
}

// Adds the control to the list
void DcxDialog::RegisterDragList(DcxList *const list)
{
   this->m_vDragLists.push_back(list);
}

// Removes the control from the list
void DcxDialog::UnregisterDragList(const DcxList *const list)
{
	//auto itStart = this->m_vDragLists.begin();
	//auto itEnd = this->m_vDragLists.end();
	//
	//while (itStart != itEnd) {
	//	if (*itStart == list) {
	//		m_vDragLists.erase(itStart);
	//		return;
	//	}
	//
	//	++itStart;
	//}

	auto itEnd = m_vDragLists.end();
	auto itGot = std::find(m_vDragLists.begin(), itEnd, list);
	if (itGot != itEnd)
		m_vDragLists.erase(itGot);
}

// Checks if the message should be parsed
LRESULT DcxDialog::ProcessDragListMessage(DcxDialog *const p_this, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed)
{
	for (auto &x: p_this->m_vDragLists)
	{
		if (x->getDragListId() == uMsg)
			return x->ParentMessage(uMsg, wParam, lParam, bParsed);
	}

	return 0L;
	//std::find(p_this->m_vDragLists, uMsg);

	// would this not be better as a map?
	// <UINT, DcxList *> where UINT is msg id, & DcxList is the list object for that msg id
	// this would then allow:
	//	auto itGot = std::find(m_vDragLists, uMsg);
	//	if (itGot != m_vDragLists.end())
	//		return itGot->second->ParentMessage(uMsg, wParam, lParam, bParsed);
	//	return 0L;
}

void DcxDialog::toXml(TiXmlElement *const xml) const
{
	this->toXml(xml, TEXT("unnamed"));
}

void DcxDialog::toXml(TiXmlElement *const xml, const TString &name) const
{
	TString dest;
	TGetWindowText(this->m_Hwnd, dest);
	xml->SetAttribute("name", name.c_str());
	xml->SetAttribute("caption", dest.c_str());
	this->m_pLayoutManager->getRoot()->toXml(xml);
}

TiXmlElement * DcxDialog::toXml() const
{
	auto result = new TiXmlElement("dialog");
	this->toXml(result);
	return result;
}

TiXmlElement * DcxDialog::toXml(const TString & name) const
{
	auto result = new TiXmlElement("dialog");
	this->toXml(result, name);
	return result;
}

//#ifdef DCX_USE_GDIPLUS
//bool DcxDialog::LoadGDIPlusImage(TString &filename) {
//	if (!IsFile(filename)) {
//		this->showError(nullptr,TEXT("LoadGDIPlusImage"), TEXT("Unable to open file"));
//		return false;
//	}
//	this->m_pImage = new Image(filename.to_wchr(),TRUE);
//
//	// couldnt allocate image object.
//	if (this->m_pImage == nullptr) {
//		this->showError(nullptr,TEXT("LoadGDIPlusImage"), TEXT("Couldn't allocate image object."));
//		return false;
//	}
//	// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
//	Status status = this->m_pImage->GetLastStatus();
//	if (status != Ok) {
//		this->showError(nullptr,TEXT("LoadGDIPlusImage"), GetLastStatusStr(status));
//		PreloadData();
//		return false;
//	}
//	return true;
//}
//#endif

const bool DcxDialog::isIDValid(_In_ const UINT ID, _In_ const bool bUnused) const
{
	if (bUnused)	// a valid ID thats NOT in use
		return ((ID > (mIRC_ID_OFFSET - 1)) && !IsWindow(GetDlgItem(this->m_Hwnd, ID)) && (this->getControlByID(ID) == nullptr));
	//a control that already exists.
	return ((ID > (mIRC_ID_OFFSET - 1)) && IsWindow(GetDlgItem(this->m_Hwnd, ID)) && (this->getControlByID(ID) != nullptr));
}
