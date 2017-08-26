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
, m_bCursorFromFile(false)
, m_MouseID(0)
, m_FocusID(0)
, m_ToolTipHWND(nullptr)
, m_iRefCount(0)
, m_bDoDrag(false)
, m_dEventMask(DCX_EVENT_ALL)
, m_bTracking(FALSE)
, m_uGhostDragAlpha(255)
, m_bGhosted(false)
, m_zLayerCurrent(0)
, m_popup(nullptr)
//, m_hOldWindowProc(nullptr)
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
, m_hCursorList( )
, m_pLayoutManager(new LayoutManager(m_Hwnd))
, m_GlassOffsets()
{
	addStyle(WS_CLIPCHILDREN);

	//addExStyle(WS_EX_TRANSPARENT); // WS_EX_TRANSPARENT|WS_EX_LAYERED gives a window u can click through to the win behind.

	//m_hOldWindowProc = SubclassWindow(m_Hwnd, DcxDialog::WindowProc);
	m_hDefaultWindowProc = SubclassWindow(m_Hwnd, DcxDialog::WindowProc);

	SetProp(m_Hwnd, TEXT("dcx_this"), (HANDLE) this);

	DragAcceptFiles(m_Hwnd, TRUE);
}

/*!
 * \brief Destructor
 *
 * blah
 */

DcxDialog::~DcxDialog() {
	delete m_pLayoutManager;

	delete m_popup;

	PreloadData();

	RemoveVistaStyle();

	if (m_bCursorFromFile && m_hCursor != nullptr)
		DestroyCursor(m_hCursor);

#if _MSC_VER < 1911
	for (auto &x : m_hCursorList)
	{
		if ((x.first != nullptr) && (x.second))
			DestroyCursor(x.first);
		x.first = nullptr;
		x.second = false;
	}
#else
	for (auto &[hCursor, bLoaded] : m_hCursorList)
	{
		//auto[hCursor, bLoaded] = x;
		if ((hCursor != nullptr) && (bLoaded))
			DestroyCursor(hCursor);
		hCursor = nullptr;
		bLoaded = false;
	}
#endif
	RemoveProp(m_Hwnd, TEXT("dcx_this"));
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

	m_vpControls.push_back(p_Control);
	p_Control->redrawWindow();
}

DcxControl *DcxDialog::addControl(const TString &input, const UINT offset, const UINT64 mask, HWND hParent)
{
	const auto tsID(input.getfirsttok(gsl::narrow_cast<int>(offset)));
	auto ID = 0U;

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

	addControl(p_ctrl);

	AddNamedId(tsID, ID);	// NB: adds numbers as names too: "200" == 6200 allowing it to track all used id's

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

	//auto itStart = m_vpControls.begin();
	//auto itEnd = m_vpControls.end();
	//
	//while (itStart != itEnd) {
	//	if (*itStart == p_Control) {
	//		// *itStart cant be a nullptr as p_Control has alrdy been checked against nullptr
	//		deleteNamedID(p_Control->getID());
	//		m_vpControls.erase(itStart);
	//		return;
	//	}
	//
	//	++itStart;
	//}

	//auto itEnd = m_vpControls.end();
	//auto itGot = std::find_if(m_vpControls.begin(), itEnd, [p_Control](const auto &arg) { return (arg == p_Control); });
	//if (itGot != itEnd)
	//{
	//	deleteNamedID(p_Control->getID());
	//	m_vpControls.erase(itGot);
	//}

	if (Dcx::eraseIfFound(m_vpControls, p_Control))
		deleteNamedID(p_Control->getID());
}

//void DcxDialog::deleteAllControls() {
//}

/*!
 * \brief blah
 *
 * The ID must include the mIRC_ID_OFFSET of 6000.
 */
DcxControl *DcxDialog::getControlByID(const UINT ID) const {
	for (const auto &x: m_vpControls) {
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
	for (const auto &x: m_vpControls) {
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
	if (m_bitmapBg != nullptr) {
		DeleteBitmap(m_bitmapBg);
		m_bitmapBg = nullptr;
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

	parseCommandRequest(msg);
}

void DcxDialog::parseComControlRequestEX(const UINT id, const TCHAR *const szFormat, ...) {
	auto p_Control = getControlByID(id + mIRC_ID_OFFSET);
	if (p_Control == nullptr)
		throw Dcx::dcxException("parseComControlRequestEX() - Unable to find control %", id);

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
		AnimateWindow(m_Hwnd,
			input.gettok( 4 ).to_<DWORD>(),
			getAnimateStyles(input.getnexttok( )));	// tok 3

		if (IsWindowVisible(m_Hwnd))
			redrawWindow();
	}
	// xdialog -b [NAME] [SWITCH] [+FLAGS]
	else if (flags[TEXT('b')] && numtok > 2) {
		removeStyle(WS_BORDER | WS_DLGFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX |
			WS_SYSMENU | WS_SIZEBOX | WS_CAPTION);

		removeExStyle(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_CONTEXTHELP |
			WS_EX_TOOLWINDOW | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE | WS_EX_COMPOSITED | WS_EX_LAYERED);

		RemoveVistaStyle();

		LONG Styles = 0, ExStyles = 0;
		const auto tsStyles(input.getnexttok());	// tok 3

		parseBorderStyles(tsStyles, &Styles, &ExStyles);
		addStyle(Styles);
		addExStyle(ExStyles);

		if (tsStyles.find(TEXT('v'),0)) {
			// Vista Style Dialog
			CreateVistaStyle();
		}
		SetWindowPos(m_Hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		InvalidateRect(m_Hwnd, nullptr, TRUE);
		SendMessage(m_Hwnd, WM_NCPAINT, (WPARAM) 1, (LPARAM) 0);
	}
	// xdialog -c [NAME] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	else if (flags[TEXT('c')] && numtok > 7) {
		try {
			addControl(input, 3, CTLF_ALLOW_ALL, nullptr);
		}
		catch (const std::exception &e)
		{
			showErrorEx(nullptr, TEXT("-c"), TEXT("Unable To Create Control (%S)"), e.what());
			//showError(nullptr, TEXT("-c"), TEXT("Unable To Create Control (%)"), e.what());
			throw;
		}
	}
	// xdialog -d [NAME] [SWITCH] [ID]
	else if (flags[TEXT('d')] && numtok > 2) {

		const auto tsID(input.getnexttok());	// tok 3

		const auto ID = NameToID(tsID);

		auto p_Control = getControlByID(ID);
		if (p_Control == nullptr)
			throw Dcx::dcxException(TEXT("Unknown control with ID \"%\" (dialog %)"), ID - mIRC_ID_OFFSET, this->m_tsName);

		auto cHwnd = p_Control->getHwnd();

		// fix up focus id
		if (const auto cid = p_Control->getUserID(); cid == m_FocusID) {
			// control exists and is not the last one
			if (auto h = GetNextDlgTabItem(m_Hwnd, cHwnd, FALSE); (h && (h != cHwnd)))
				setFocusControl(GetDlgCtrlID(h) - mIRC_ID_OFFSET);
			else
				setFocusControl(0);
		}

		if (const auto dct = p_Control->getControlType(); (dct == DcxControlTypes::DIALOG || dct == DcxControlTypes::WINDOW))
			delete p_Control;
		else {
			if (p_Control->getRefCount() != 0)
				throw Dcx::dcxException(TEXT("Can't delete control with ID \"%\" when it is inside it's own event (dialog %)"), p_Control->getUserID(), this->m_tsName);
					
			deleteControl(p_Control); // remove control from internal list!
			DestroyWindow(cHwnd);
		}
	}
	// xdialog -f [NAME] [SWITCH] [+FLAGS] [COUNT] [TIMEOUT]
	else if (flags[TEXT('f')] && numtok > 4) {
		const auto iFlags = parseFlashFlags(input.getnexttok());	// tok 3
		const auto iCount = input.getnexttok().to_<UINT>();				// tok 4
		const auto dwTimeout = input.getnexttok().to_<DWORD>();	// tok 5
		FLASHWINFO fli{ sizeof(FLASHWINFO), m_Hwnd, iFlags, iCount, dwTimeout };

		//FLASHWINFO fli;
		//ZeroMemory(&fli, sizeof(FLASHWINFO));
		//fli.cbSize = sizeof(FLASHWINFO);
		//fli.dwFlags = iFlags;
		//fli.hwnd = m_Hwnd;
		//fli.uCount = iCount;
		//fli.dwTimeout = dwTimeout;

		FlashWindowEx(& fli);
	}
	// xdialog -g [NAME] [SWITCH] [+FLAGS] [COLOR|FILENAME]
	else if (flags[TEXT('g')] && numtok > 3) {
		m_uStyleBg = parseBkgFlags(input.getnexttok( ));			// tok 3

		if (dcx_testflag(m_uStyleBg, DBS_BKGCOLOR)) {
			const auto clrColor = input.getnexttok( ).to_<COLORREF>();	// tok 4

			if (m_hBackBrush != nullptr) {
				DeleteBrush(m_hBackBrush);
				m_hBackBrush = nullptr;
			}

			if (clrColor != CLR_INVALID)
				m_hBackBrush = CreateSolidBrush(clrColor);
		}
		else if (dcx_testflag(m_uStyleBg, DBS_BKGBITMAP)) {
			PreloadData();

			if (auto filename(input.getlasttoks().trim());	/* tok 4, -1 */ filename != TEXT("none"))
				m_bitmapBg = dcxLoadBitmap(m_bitmapBg, filename);
		}

		//InvalidateRect(m_Hwnd, nullptr, TRUE);
		redrawWindow();
	}
	// xdialog -j [NAME] [SWITCH] (ID)
	else if (flags[TEXT('j')]) {
		if (numtok > 2) {
			const auto tsID(input.getnexttok( ));	// tok 3
			const auto id = NameToID(tsID);

			if (!isIDValid(id))
				throw Dcx::dcxException(TEXT("Could not find control %"), id - mIRC_ID_OFFSET);

			if (auto p_Control = getControlByID(id); p_Control != nullptr)
				p_Control->redrawWindow();
		}
		else
			redrawWindow();
	}
	/*
	//xdialog -l [NAME] [SWITCH] [OPTIONS]

	[OPTIONS] :

	root [TAB]+flpiw [ID] [WEIGHT] [W] [H]
	add PATH[TAB]+flpiw [ID] [WEIGHT] [W] [H]
	space PATH[TAB]+ [L] [T] [R] [B]
	*/
	else if (flags[TEXT('l')] && numtok > 2) {
#if DCX_USE_HASHING
		if (m_pLayoutManager == nullptr)
			throw Dcx::dcxException("No LayoutManager available");

		switch (std::hash<TString>{}(input.getnexttok())) // tok 3
		{
		case TEXT("update"_hash):
		{
			RECT rc;

			if (!GetClientRect(m_Hwnd, &rc))
				throw Dcx::dcxException("Unable to get client rect!");

			if (updateLayout(rc))
				redrawWindow();
		}
		break;
		case TEXT("clear"_hash):
		{
			delete m_pLayoutManager;
			m_pLayoutManager = new LayoutManager(m_Hwnd);
			//this->redrawWindow(); // dont redraw here, leave that for an `update` cmd
		}
		break;
		default:
		{
			if (numtok > 7)
				m_pLayoutManager->AddCell(input);
			else
				throw Dcx::dcxException("Invalid Arguments");
		}
		}
#else
		const auto tsCmd(input.getnexttok());	// tok 3
		if (tsCmd == TEXT("update")) {
			if (m_pLayoutManager != nullptr) {
				RECT rc;

				if (!GetClientRect(m_Hwnd, &rc))
					throw Dcx::dcxException("Unable to get client rect!");

				if (updateLayout(rc))
					redrawWindow();
			}
		}
		else if (tsCmd == TEXT("clear")) {
			delete m_pLayoutManager;
			m_pLayoutManager = new LayoutManager(m_Hwnd);
			//this->redrawWindow(); // dont redraw here, leave that for an `update` cmd
		}
		else if (numtok > 7)
			m_pLayoutManager->AddCell(input);
		else
			throw Dcx::dcxException("Invalid Arguments");
#endif
	}
	// xdialog -q [NAME] [SWITCH] [+FLAGS] [CURSOR|FILENAME]
	else if (flags[TEXT('q')] && numtok > 3) {
		// Ook: TODO allow different cursors for client areas, menus, non client areas
		// get cursors flags (either +f or +r atm)
		const auto tsFlags(input.getnexttok());	// tok 3
		const auto iFlags = parseCursorFlags(tsFlags);
		// filename is the resource name if +r
		// otherwise its the filename if +f
		auto filename(input.getlasttoks());
		// get resource cursor type, will be a nullptr if filename is a file
		// NB: if a filename is the same as a resource name then this will return the resource, but the file will be loaded.
		const auto CursorType = parseCursorType(filename);

		if (const auto CursorArea = parseCursorArea(tsFlags); CursorArea > 0)
			m_hCursorList[CursorArea].first = Dcx::dcxLoadCursor(iFlags, CursorType, m_hCursorList[CursorArea].second, m_hCursorList[CursorArea].first, filename);
		else
			m_hCursor = Dcx::dcxLoadCursor(iFlags, CursorType, m_bCursorFromFile, m_hCursor, filename);
	}
	// xdialog -x [NAME]
	else if (flags[TEXT('x')]) {
		if (getRefCount() == 0) {
			//DestroyWindow(m_Hwnd);
			//SendMessage(m_Hwnd,WM_CLOSE,nullptr,nullptr); // this allows the dialogs WndProc to EndDialog() if needed.

			//TCHAR sRet[32];
			//mIRCLinker::evalex(sRet, Dcx::countof(sRet), TEXT("$dialog(%s).modal"), this->m_tsName.to_chr());
			//if (ts_strcmp(sRet, TEXT("$true")) == 0) // Modal Dialog
			//	SendMessage(m_Hwnd,WM_CLOSE,NULL,NULL); // this allows the dialogs WndProc to EndDialog() if needed.
			//else // Modeless Dialog
			//	DestroyWindow(m_Hwnd);

			//stString<32> sRet;
			//mIRCLinker::evalex(sRet, sRet.size(), TEXT("$dialog(%s).modal"), m_tsName.to_chr());
			//if (sRet == TEXT("$true")) // Modal Dialog
			//	SendMessage(m_Hwnd, WM_CLOSE, NULL, NULL); // this allows the dialogs WndProc to EndDialog() if needed.
			//else // Modeless Dialog
			//	DestroyWindow(m_Hwnd);

			//if (mIRCLinker::evalex(nullptr, 0, TEXT("$dialog(%s).modal"), m_tsName.to_chr())) // Modal Dialog
			//	SendMessage(m_Hwnd, WM_CLOSE, NULL, NULL); // this allows the dialogs WndProc to EndDialog() if needed.
			//else // Modeless Dialog
			//	DestroyWindow(m_Hwnd);
			if (mIRCLinker::eval(nullptr, TEXT("$dialog(%).modal"), m_tsName)) // Modal Dialog
				SendMessage(m_Hwnd, WM_CLOSE, NULL, NULL); // this allows the dialogs WndProc to EndDialog() if needed.
			else // Modeless Dialog
				DestroyWindow(m_Hwnd);
		}
		else
			//mIRCLinker::execex(TEXT("/.timer -m 1 0 xdialog -x %s"), this->getName().to_chr());
			mIRCLinker::exec(TEXT("/.timer -m 1 0 xdialog -x %"), this->getName());
	}
	// xdialog -h [NAME]
	else if (flags[TEXT('h')]) {
		ShowWindow(m_Hwnd, SW_HIDE);
	}
	// xdialog -m [NAME]
	else if (flags[TEXT('m')]) {
		ShowWindow(m_Hwnd, SW_MAXIMIZE);
	}
	// xdialog -n [NAME]
	else if (flags[TEXT('n')]) {
		ShowWindow(m_Hwnd, SW_MINIMIZE);
	}
	// xdialog -r [NAME]
	else if (flags[TEXT('r')]) {
		ShowWindow(m_Hwnd, SW_RESTORE);
	}
	// xdialog -s [NAME]
	else if (flags[TEXT('s')]) {
		ShowWindow(m_Hwnd, SW_SHOW);
	}
	// xdialog -t [NAME] [SWITCH] [TYPE] [TYPE ARGS]
	else if (flags[TEXT('t')] && numtok > 2) {
#if DCX_USE_HASHING
		const auto tsCmd(input.getnexttok());	// tok 3
		const auto tsArgs(input.getnexttok());	// tok 4

		// Alpha transparency
		switch (std::hash<TString>{}(tsCmd))
		{
		case TEXT("alpha"_hash):
		{
			if (tsArgs == TEXT("none")) {
				this->m_iAlphaLevel = 255;
				if (!this->m_bVistaStyle) {
					if (this->isExStyle(WS_EX_LAYERED)) {
						this->removeExStyle(WS_EX_LAYERED);
						this->addExStyle(WS_EX_LAYERED);
						if (this->m_bHaveKeyColour) // reapply keycolour if any.
							SetLayeredWindowAttributes(m_Hwnd, this->m_cKeyColour, 0, LWA_COLORKEY);
					}
				}
			}
			else {
				this->m_iAlphaLevel = (BYTE)(tsArgs.to_int() & 0xFF);

				if (!this->m_bVistaStyle) {
					// Set WS_EX_LAYERED on this window
					this->addExStyle(WS_EX_LAYERED);

					// Make this window x% alpha
					SetLayeredWindowAttributes(m_Hwnd, 0, this->m_iAlphaLevel, LWA_ALPHA);
				}
			}
		}
		break;
		// Transparent color
		case TEXT("transparentcolor"_hash):
		{
			if (tsArgs == TEXT("none")) {
				this->m_cKeyColour = CLR_NONE;
				this->m_bHaveKeyColour = false;
				if (!this->m_bVistaStyle) {
					if (this->isExStyle(WS_EX_LAYERED)) {
						this->removeExStyle(WS_EX_LAYERED);
						this->addExStyle(WS_EX_LAYERED);
						if (this->m_iAlphaLevel != 255) // reapply alpha if any.
							SetLayeredWindowAttributes(m_Hwnd, 0, this->m_iAlphaLevel, LWA_ALPHA);
					}
				}
			}
			else {
				this->m_cKeyColour = tsArgs.to_<COLORREF>();
				this->m_bHaveKeyColour = true;
				if (!this->m_bVistaStyle) {
					// Set WS_EX_LAYERED on this window
					this->addExStyle(WS_EX_LAYERED);

					// Make colour transparent
					SetLayeredWindowAttributes(m_Hwnd, this->m_cKeyColour, 0, LWA_COLORKEY);
				}
			}
		}
		break;
		// Background color
		case TEXT("bgcolor"_hash):
		{
			this->m_colTransparentBg = tsArgs.to_<COLORREF>();
		}
		break;
		// TODO: not going to document this, have no way to redrawing the window.
		// http://www.codeproject.com/KB/vb/ClickThroughWindows.aspx
		// NB: may not be compatible with vista style.
		// Click-through
		case TEXT("clickthrough"_hash):
		{
			if (tsArgs == TEXT("none")) {
				if (this->isExStyle(WS_EX_LAYERED | WS_EX_TRANSPARENT))
					//RemStyles(m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
					this->removeExStyle(WS_EX_LAYERED | WS_EX_TRANSPARENT);
				// re-apply any alpha or keycolour.
				if (((this->m_iAlphaLevel != 255) || (this->m_bHaveKeyColour)) && (!this->m_bVistaStyle)) {
					//AddStyles(m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
					this->addExStyle(WS_EX_LAYERED);
					if (this->m_iAlphaLevel != 255) // reapply alpha if any.
						SetLayeredWindowAttributes(m_Hwnd, 0, this->m_iAlphaLevel, LWA_ALPHA);
					if (this->m_bHaveKeyColour) // reapply keycolour if any.
						SetLayeredWindowAttributes(m_Hwnd, this->m_cKeyColour, 0, LWA_COLORKEY);
				}
			}
			else
				//AddStyles(m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
				this->addExStyle(WS_EX_LAYERED | WS_EX_TRANSPARENT);
		}
		break;
		default:
			throw Dcx::dcxException("Unknown Switch");
		}

		redrawWindow();
#else
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
						//RemStyles(m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
						//AddStyles(m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
						if (this->m_bHaveKeyColour) // reapply keycolour if any.
							SetLayeredWindowAttributes(m_Hwnd, this->m_cKeyColour, 0, LWA_COLORKEY);
					}
				}
			}
			else {
				this->m_iAlphaLevel = (BYTE)(tsAlpha.to_int() & 0xFF);

				if (!this->m_bVistaStyle) {
					// Set WS_EX_LAYERED on this window
					//AddStyles(m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
					this->addExStyle(WS_EX_LAYERED);

					// Make this window x% alpha
					SetLayeredWindowAttributes(m_Hwnd, 0, this->m_iAlphaLevel, LWA_ALPHA);
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
						//RemStyles(m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
						//AddStyles(m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
						this->removeExStyle(WS_EX_LAYERED);
						this->addExStyle(WS_EX_LAYERED);
						if (this->m_iAlphaLevel != 255) // reapply alpha if any.
							SetLayeredWindowAttributes(m_Hwnd, 0, this->m_iAlphaLevel, LWA_ALPHA);
					}
				}
			}
			else {
				this->m_cKeyColour = tsClr.to_<COLORREF>();
				this->m_bHaveKeyColour = true;
				if (!this->m_bVistaStyle) {
					// Set WS_EX_LAYERED on this window
					//AddStyles(m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
					this->addExStyle(WS_EX_LAYERED);

					// Make colour transparent
					SetLayeredWindowAttributes(m_Hwnd, this->m_cKeyColour, 0, LWA_COLORKEY);
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
					//RemStyles(m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
					this->removeExStyle( WS_EX_LAYERED | WS_EX_TRANSPARENT);
				// re-apply any alpha or keycolour.
				if (((this->m_iAlphaLevel != 255) || (this->m_bHaveKeyColour)) && (!this->m_bVistaStyle)) {
					//AddStyles(m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
					this->addExStyle(WS_EX_LAYERED);
					if (this->m_iAlphaLevel != 255) // reapply alpha if any.
						SetLayeredWindowAttributes(m_Hwnd, 0, this->m_iAlphaLevel, LWA_ALPHA);
					if (this->m_bHaveKeyColour) // reapply keycolour if any.
						SetLayeredWindowAttributes(m_Hwnd, this->m_cKeyColour, 0, LWA_COLORKEY);
				}
			}
			else
				//AddStyles(m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
				this->addExStyle(WS_EX_LAYERED | WS_EX_TRANSPARENT);
		}
		else
			throw Dcx::dcxException("Unknown Switch");

		this->redrawWindow();
#endif
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
			m_Hwnd,
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

		ChangeHwndIcon(m_Hwnd,tsFlags,index,filename);
	}
	// xdialog -z [NAME] [SWITCH] [+FLAGS] [N]
	else if (flags[TEXT('z')] && numtok > 3) {
		// Ook: TODO change to support named id's
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
			//for (const auto &x: m_vZLayers) {
			//	if (x == n)
			//		throw Dcx::dcxException(TEXT("control % already in list"), n);
			//}

			if (Dcx::find(m_vZLayers, n))
				throw Dcx::dcxException(TEXT("control % already in list"), n);

			// if the specified control exists on the dialog, hide it
			auto ctrl = getControlByID(static_cast<UINT>(n));

			if (ctrl != nullptr) {
				ShowWindow(ctrl->getHwnd(), SW_HIDE);
				
				RECT rc;
				if (!GetClientRect(getHwnd(), &rc))
					throw Dcx::dcxException("Unable to get client rect!");
				
				if (updateLayout(rc))
					redrawWindow();
			}

			// append the item to the end of the list
			m_vZLayers.push_back(n);
		}
		// position to match CID [CID]
		else if (xflag[TEXT('p')]) {
			// add mIRC offset since getControlByID() needs it
			n += mIRC_ID_OFFSET;

			// get the control which will be used to retrieve the target position
			auto ctrl = getControlByID(static_cast<UINT>(n));

			// target control not found
			if (ctrl == nullptr)
				throw Dcx::dcxException("No such control");

			// variables used to move control
			RECT r;

			// figure out coordinates of control

			//if (!GetWindowRect(ctrl->getHwnd(), &r))
			//	throw Dcx::dcxException("Unable to get window rect!");
			//
			//MapWindowRect(nullptr, GetParent(ctrl->getHwnd()), &r);

			if (!GetWindowRectParent(ctrl->getHwnd(), &r))
				throw Dcx::dcxException("Unable to get window rect!");

			// for each control in the internal list
			for (const auto &x : m_vZLayers) {
				// ignore target control
				if (x != n) {
					// get control to be moved
					ctrl = getControlByID(static_cast<UINT>(x));

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
			if (n >= static_cast<int>(m_vZLayers.size()))
				throw Dcx::dcxException("Index array out of bounds");

			execAliasEx(TEXT("zlayershow,%d,%d"), n +1, m_vZLayers[static_cast<UINT>(n)] - mIRC_ID_OFFSET);

			// hide the previous control
			auto ctrl = getControlByID(static_cast<UINT>(m_vZLayers[m_zLayerCurrent]));

			if (ctrl != nullptr)
				ShowWindow(ctrl->getHwnd(), SW_HIDE);

			// set the new index to the currently selected index
			m_zLayerCurrent = static_cast<UINT>(n);
			ctrl = getControlByID(static_cast<UINT>(m_vZLayers[static_cast<UINT>(n)]));

			// if the selected control exists, show control
			if (ctrl == nullptr)
				throw Dcx::dcxException("Invalid Control ID");
	
			ShowWindow(ctrl->getHwnd(), SW_SHOW);
			RECT rc;
			if (!GetClientRect(getHwnd(), &rc))
				throw Dcx::dcxException("Unable to get client rect!");
				
			if (updateLayout(rc))
				redrawWindow();
		}

		return;
	}
	// xdialog -P [NAME] [SWITCH] ([+FLAGS] (FLAG OPTIONS))
	else if (flags[TEXT('P')]) {

		// create the menu
		if (m_popup == nullptr) {
			auto menu = GetMenu(m_Hwnd);

			if (menu == nullptr)
				throw Dcx::dcxException("Menu Does Not Exist");
			
			m_popup = new XPopupMenu(TEXT("dialog"_ts), menu);
		}
		if (m_popup != nullptr) {
			//TString menuargs(TEXT("dialog "));
			//menuargs += input.getlasttoks();

			auto menuargs(TEXT("dialog "_ts));
			menuargs += input.getlasttoks();

			//TString menuargs{ TEXT("dialog "_ts), input.getlasttoks() };
			Dcx::XPopups.parseCommand(menuargs, m_popup);
		}
	}
	// xdialog -R [NAME] [SWITCH] [FLAG] [ARGS]
	else if (flags[TEXT('R')] && numtok > 2) {
		const XSwitchFlags xflags(input.getnexttok( ));	// tok 3

		if (!xflags[TEXT('+')])
			throw Dcx::dcxException("Invalid Flag");

		RECT rc;
		if (!dcxGetWindowRect(m_Hwnd, &rc))
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
			//SetWindowRgn(m_Hwnd,nullptr,TRUE);
			m_colTransparentBg = input.getnexttok( ).to_<COLORREF>();	// tok 4
			//this->m_uStyleBg = DBS_BKGBITMAP|DBS_BKGSTRETCH|DBS_BKGCENTER;
			m_uStyleBg = DBS_BKGBITMAP;
			auto filename(input.getlasttoks());								// tok 5, -1
			m_bitmapBg = dcxLoadBitmap(m_bitmapBg,filename);

			if (m_bitmapBg == nullptr)
				throw Dcx::dcxException("Unable To Load Image file.");
			
			m_Region = BitmapRegion(m_bitmapBg, m_colTransparentBg, TRUE);
		}
		else if (xflags[TEXT('r')]) // rounded rect - radius args (optional)
		{
			auto radius = 20;

			if (numtok > 3)
				radius = input.getnexttok( ).to_int();	// tok 4

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
				pnts[cnt].x = strPoint.getfirsttok(1, TSCOMMACHAR).to_<LONG>();
				pnts[cnt].y = strPoint.getnexttok(TSCOMMACHAR).to_<LONG>();	// tok 2
				++cnt;
			}
			m_Region = CreatePolygonRgn(pnts.get(), static_cast<int>(tPoints),WINDING);
		}
		else if (xflags[TEXT('d')]) // drag - <1|0>
		{
			noRegion = true;
			m_bDoDrag = (input.getnexttok().to_<int>() > 0);	// tok 4
		}
		else if (xflags[TEXT('g')]) // ghost drag - <0-255>
		{
			m_uGhostDragAlpha = (BYTE)(input.getnexttok().to_<UINT>() & 0xFF);	// tok 4

			noRegion = true;
		}
		else if (xflags[TEXT('s')]) // shadow - <colour> <sharpness> <darkness> <size> <xoffset> <yoffset>
		{
			// no custom shadows in V3
			noRegion = true;
		}
		else if (xflags[TEXT('n')]) { // none, no args
			noRegion = true;
			SetWindowRgn(m_Hwnd,nullptr,TRUE);
		}
		else
			throw Dcx::dcxException("Invalid Flag");

		if (!noRegion) {
			if (m_Region == nullptr)
				throw Dcx::dcxException("Unable to create region.");

			if (RegionMode != 0) {
				if (auto wrgn = CreateRectRgn(0, 0, 0, 0); wrgn != nullptr) {
					if (GetWindowRgn(m_Hwnd,wrgn) != ERROR)
						CombineRgn(m_Region,m_Region,wrgn,RegionMode);
					DeleteRgn(wrgn);
				}
			}
			SetWindowRgn(m_Hwnd,m_Region,TRUE);
		}
		redrawWindow();
	}
	// xdialog -E [NAME] [SWITCH] [+flags] [-flags]
	else if (flags[TEXT('E')] && numtok > 3) {
		auto mask = m_dEventMask;
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

		m_dEventMask = mask;
	}
	// xdialog -V [NAME] [SWITCH] [left] [right] [top] [bottom]
	else if (flags[TEXT('V')] && numtok > 5) {
		m_GlassOffsets.left = input.getnexttok( ).to_<LONG>();	// tok 3
		m_GlassOffsets.right = input.getnexttok( ).to_<LONG>();	// tok 4
		m_GlassOffsets.top = input.getnexttok( ).to_<LONG>();	// tok 5
		m_GlassOffsets.bottom = input.getnexttok( ).to_<LONG>();	// tok 6

		if (Dcx::VistaModule.isUseable()) {
			const MARGINS margins{ m_GlassOffsets.left, m_GlassOffsets.right, m_GlassOffsets.top, m_GlassOffsets.bottom };
			Dcx::VistaModule.dcxDwmExtendFrameIntoClientArea(m_Hwnd, &margins);
		}
		redrawWindow();
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

		RECT rcWindow = { 0 }, rcClient = { 0 };
		UINT iFlags = SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOOWNERZORDER;

		if (!GetClientRect(m_Hwnd, &rcClient))
			throw Dcx::dcxException("Unable to get client rect!");

		//if (!dcxGetWindowRect(m_Hwnd, &rcWindow))
		if (!GetWindowRect(m_Hwnd, &rcWindow))
			throw Dcx::dcxException("Unable to get window rect!");

		// Convert windows screen position to its position within it's parent.
		if (isStyle(WS_CHILD))
			MapWindowRect(nullptr, GetParent(m_Hwnd), &rcWindow);

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
		//POINT ptDiff;
		////ptDiff.x = (rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left);
		////ptDiff.y = (rcWindow.bottom - rcWindow.top) - (rcClient.bottom - rcClient.top);
		//ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
		//ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
		//
		//SetWindowPos( m_Hwnd, nullptr, x, y, w + ptDiff.x, h + ptDiff.y, iFlags );

		auto Diffx = (rcWindow.right - rcWindow.left) - rcClient.right;
		auto Diffy = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;

		SetWindowPos(m_Hwnd, nullptr, x, y, w + Diffx, h + Diffy, iFlags);
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


void DcxDialog::parseBorderStyles(const TString &flags, LONG *const Styles, LONG *const ExStyles) noexcept
{
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

const DWORD DcxDialog::getAnimateStyles(const TString &flags) noexcept
{
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

const UINT DcxDialog::parseTooltipFlags(const TString &flags) noexcept
{
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

const UINT DcxDialog::parseBkgFlags(const TString &flags) noexcept
{
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

const UINT DcxDialog::parseFlashFlags(const TString &flags) noexcept
{
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

//void DcxDialog::parseInfoRequest( const TString &input, TCHAR *szReturnValue) const
//{
//	const auto numtok = input.numtok();
//	const auto prop(input.getfirsttok(2));
//
//	// [NAME] [PROP] [ID]
//	if (prop == TEXT("isid") && numtok > 2) {
//		const auto nID = input.getnexttok().to_int() + mIRC_ID_OFFSET;	// tok 3
//		dcx_ConRet(IsWindow(GetDlgItem(m_Hwnd, nID)) || (getControlByID(nID) != nullptr), szReturnValue);
//	}
//	// [NAME] [PROP]
//	else if (prop == TEXT("nextid")) {
//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), getUniqueID() - mIRC_ID_OFFSET);
//
//		//auto nID = mIRC_ID_OFFSET + 1;
//		//
//		//while (IsWindow(GetDlgItem(m_Hwnd, nID)) || (this->getControlByID(nID) != nullptr))
//		//{
//		//	nID++;
//		//}
//		//
//		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), nID - mIRC_ID_OFFSET);
//	}
//	// [NAME] [PROP] [N|NAMEDID]
//	else if (prop == TEXT("id") && numtok > 2) {
//		const auto tsID(input.getnexttok());	// tok 3
//		const auto N = tsID.to_int() - 1;
//
//		if (N == -1)
//		{
//			if (tsID == TEXT('0'))	// check its an actual zero, not some text name (which also gives a zero result)
//				wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), this->m_vpControls.size());
//			else
//			{
//				//for (const auto &x: this->namedIds) {
//				//	if (x.first == tsID) {
//				//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%i"), x.second);
//				//		return;
//				//	}
//				//}
//
//				const auto it = m_NamedIds.find(tsID);
//				if (it != m_NamedIds.end())
//					wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), it->second - mIRC_ID_OFFSET);
//			}
//		}
//		else if ((N > -1) && (N < static_cast<int>(m_vpControls.size())))
//			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_vpControls[N]->getUserID());
//	}
//	// [NAME] [PROP]
//	else if (prop == TEXT("ismenu")) {
//		dcx_ConRet(GetMenu(m_Hwnd) != nullptr, szReturnValue);
//	}
//	// [NAME] [PROP]
//	else if (prop == TEXT("ismarked")) {
//		// no need to test anything, if it got here we already know its marked.
//		dcx_strcpyn(szReturnValue, TEXT("$true"), MIRC_BUFFER_SIZE_CCH);
//
//		//dcx_ConRet(Dcx::Dialogs.getDialogByHandle(m_Hwnd) != nullptr, szReturnValue);
//
//	}
//	// [NAME] [PROP]
//	else if (prop == TEXT("parent")) {
//		dcx_strcpyn(szReturnValue, getParentName().to_chr(), MIRC_BUFFER_SIZE_CCH);
//	}
//	// [NAME] [PROP]
//	else if (prop == TEXT("mouseid")) {
//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_MouseID);
//	}
//	// [NAME] [PROP]
//	else if (prop == TEXT("focusid")) {
//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_FocusID);
//	}
//	// [NAME] [PROP]
//	else if (prop == TEXT("mouse")) {
//		POINT pt = { 0 };
//
//		if (GetCursorPos(&pt))
//			MapWindowPoints(nullptr, m_Hwnd, &pt, 1);
//
//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), pt.x, pt.y);
//	}
//	// [NAME] [PROP]
//	else if (prop == TEXT("key")) {
//		UINT iKeyState = 0;
//
//		if (GetAsyncKeyState(VK_LBUTTON) < 0)
//			iKeyState |= 1;
//		if (GetAsyncKeyState(VK_RBUTTON) < 0)
//			iKeyState |= 2;
//		if (GetAsyncKeyState(VK_MBUTTON) < 0)
//			iKeyState |= 4;
//		if (GetAsyncKeyState(VK_LSHIFT) < 0)
//			iKeyState |= 8;
//		if (GetAsyncKeyState(VK_LCONTROL) < 0)
//			iKeyState |= 16;
//		if (GetAsyncKeyState(VK_LMENU) < 0)
//			iKeyState |= 32;
//		if (GetAsyncKeyState(VK_RSHIFT) < 0)
//			iKeyState |= 64;
//		if (GetAsyncKeyState(VK_RCONTROL) < 0)
//			iKeyState |= 128;
//		if (GetAsyncKeyState(VK_RMENU) < 0)
//			iKeyState |= 256;
//		if (GetAsyncKeyState(VK_LEFT) < 0)
//			iKeyState |= 512;
//		if (GetAsyncKeyState(VK_UP) < 0)
//			iKeyState |= 1024;
//		if (GetAsyncKeyState(VK_RIGHT) < 0)
//			iKeyState |= 2048;
//		if (GetAsyncKeyState(VK_DOWN) < 0)
//			iKeyState |= 4096;
//		if (GetAsyncKeyState(VK_CAPITAL) < 0)
//			iKeyState |= 8192;
//
//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), iKeyState);
//	}
//	// [NAME] [PROP]
//	else if (prop == TEXT("alias")) {
//		dcx_strcpyn(szReturnValue, getAliasName().to_chr(), MIRC_BUFFER_SIZE_CCH);
//	}
//	// [NAME] [PROP] [N]
//	else if ((prop == TEXT("zlayer")) && (numtok > 2)) {
//		const auto n = input.getnexttok().to_<VectorOfInts::size_type>();	// tok 3
//		const auto size = m_vZLayers.size();
//
//		if (n > size)
//			throw Dcx::dcxException("Out Of Range");
//
//		// return total number of id's
//		if (n == 0)
//			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), size);
//		// return the Nth id
//		else
//			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_vZLayers[n - 1] - mIRC_ID_OFFSET);
//	}
//	// [NAME] [PROP]
//	else if (prop == TEXT("zlayercurrent")) {
//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_zLayerCurrent +1);
//	}
//	// [NAME] [PROP]
//	else if (prop == TEXT("visible")) {
//		dcx_ConRet((IsWindowVisible(m_Hwnd) != FALSE), szReturnValue);
//
//	}
//	// [NAME] [PROP]
//	else if (prop == TEXT("glasscolor")) {
//		RGBQUAD clr = {0};
//		auto bOpaque = FALSE;
//		if (SUCCEEDED(Dcx::VistaModule.dcxDwmGetColorizationColor((DWORD *)&clr, &bOpaque)))
//			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), RGB(clr.rgbRed, clr.rgbGreen, clr.rgbBlue));
//		else
//			dcx_strcpyn(szReturnValue, TEXT("-FAIL Unable to get Glass colour."), MIRC_BUFFER_SIZE_CCH);
//	}
//	// invalid info request
//	else
//		throw Dcx::dcxException("Invalid property or parameters");
//}

#if DCX_USE_HASHING
void DcxDialog::parseInfoRequest(const TString &input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	const auto numtok = input.numtok();
	switch (std::hash<TString>{}(input.getfirsttok(2)))
	{
		// [NAME] [PROP] [ID]
	case L"isid"_hash:
		if (numtok > 2) {
			const auto nID = input.getnexttok().to_<UINT>() + mIRC_ID_OFFSET;	// tok 3
			szReturnValue = dcx_truefalse(IsWindow(GetDlgItem(m_Hwnd, static_cast<int>(nID))) || (getControlByID(nID) != nullptr));
		}
		break;
		// [NAME] [PROP]
	case L"nextid"_hash:
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), getUniqueID() - mIRC_ID_OFFSET);
		_ts_snprintf(szReturnValue, TEXT("%u"), getUniqueID() - mIRC_ID_OFFSET);
		break;
		// [NAME] [PROP] [N|NAMEDID]
	case L"id"_hash:
		if (numtok > 2) {
			const auto tsID(input.getnexttok());	// tok 3
			const auto N = tsID.to_int() - 1;

			//if (N == -1)
			//{
			//	if (tsID == TEXT('0'))	// check its an actual zero, not some text name (which also gives a zero result)
			//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), this->m_vpControls.size());
			//	else
			//	{
			//		const auto it = m_NamedIds.find(tsID);
			//		if (it != m_NamedIds.end())
			//			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), it->second - mIRC_ID_OFFSET);
			//	}
			//}
			//else if ((N > -1) && (N < static_cast<int>(m_vpControls.size())))
			//	wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_vpControls[static_cast<UINT>(N)]->getUserID());

			if (N == -1)
			{
				if (tsID == TEXT('0'))	// check its an actual zero, not some text name (which also gives a zero result)
					_ts_snprintf(szReturnValue, TEXT("%u"), m_vpControls.size());
				else
				{
					const auto it = m_NamedIds.find(tsID);
					if (it != m_NamedIds.end())
						_ts_snprintf(szReturnValue, TEXT("%u"), it->second - mIRC_ID_OFFSET);
				}
			}
			else if ((N > -1) && (N < gsl::narrow_cast<int>(m_vpControls.size())))
				_ts_snprintf(szReturnValue, TEXT("%u"), m_vpControls[gsl::narrow_cast<UINT>(N)]->getUserID());
		}
		break;
		// [NAME] [PROP]
	case L"ismenu"_hash:
		szReturnValue = dcx_truefalse(GetMenu(m_Hwnd) != nullptr);
		break;
		// [NAME] [PROP]
	case L"ismarked"_hash:
		// no need to test anything, if it got here we already know its marked.
		szReturnValue = TEXT("$true");
		break;
		// [NAME] [PROP]
	case L"parent"_hash:
		szReturnValue = getParentName().to_chr();
		break;
		// [NAME] [PROP]
	case L"mouseid"_hash:
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_MouseID);
		_ts_snprintf(szReturnValue, TEXT("%u"), m_MouseID);
		break;
		// [NAME] [PROP]
	case L"focusid"_hash:
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_FocusID);
		_ts_snprintf(szReturnValue, TEXT("%u"), m_FocusID);
		break;
		// [NAME] [PROP]
	case L"mouse"_hash:
	{
#if DCX_USE_WRAPPERS
		Dcx::dcxCursorPos pt(m_Hwnd);

		_ts_snprintf(szReturnValue, TEXT("%d %d"), pt.x, pt.y);
#else
		POINT pt = { 0 };
		
		if (GetCursorPos(&pt))
			MapWindowPoints(nullptr, m_Hwnd, &pt, 1);
		
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), pt.x, pt.y);
		_ts_snprintf(szReturnValue, TEXT("%d %d"), pt.x, pt.y);
#endif
	}
	break;
	// [NAME] [PROP]
	case L"key"_hash:
	{
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

		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), iKeyState);
		_ts_snprintf(szReturnValue, TEXT("%u"), iKeyState);
	}
	break;
	// [NAME] [PROP]
	case L"alias"_hash:
		szReturnValue = getAliasName().to_chr();
		break;
		// [NAME] [PROP] [N]
	case L"zlayer"_hash:
		if (numtok > 2) {
			const auto n = input.getnexttok().to_<VectorOfInts::size_type>();	// tok 3
			const auto size = m_vZLayers.size();

			if (n > size)
				throw Dcx::dcxException("Out Of Range");

			//// return total number of id's
			//if (n == 0)
			//	wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), size);
			//// return the Nth id
			//else
			//	wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_vZLayers[n - 1] - mIRC_ID_OFFSET);

			// return total number of id's
			if (n == 0)
				_ts_snprintf(szReturnValue, TEXT("%u"), size);
			// return the Nth id
			else
				_ts_snprintf(szReturnValue, TEXT("%u"), m_vZLayers[n - 1] - mIRC_ID_OFFSET);
		}
		break;
		// [NAME] [PROP]
	case L"zlayercurrent"_hash:
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_zLayerCurrent + 1);
		_ts_snprintf(szReturnValue, TEXT("%u"), m_zLayerCurrent + 1);
		break;
		// [NAME] [PROP]
	case L"visible"_hash:
		szReturnValue = dcx_truefalse((IsWindowVisible(m_Hwnd) != FALSE));
		break;
		// [NAME] [PROP]
	case L"glasscolor"_hash:
	{
		RGBQUAD clr = { 0 };
		auto bOpaque = FALSE;
		if (SUCCEEDED(Dcx::VistaModule.dcxDwmGetColorizationColor((DWORD *)&clr, &bOpaque)))
			//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), RGB(clr.rgbRed, clr.rgbGreen, clr.rgbBlue));
			_ts_snprintf(szReturnValue, TEXT("%u"), RGB(clr.rgbRed, clr.rgbGreen, clr.rgbBlue));
		else
			szReturnValue = TEXT("-FAIL Unable to get Glass colour.");
	}
	break;
	// invalid info request
	default:
		throw Dcx::dcxException("Invalid property or parameters");
		break;
	}
}
#else
void DcxDialog::parseInfoRequest2(const TString &input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	const auto numtok = input.numtok();
	const auto prop(input.getfirsttok(2));

	// [NAME] [PROP] [ID]
	if (prop == TEXT("isid") && numtok > 2) {
		const auto nID = input.getnexttok().to_<UINT>() + mIRC_ID_OFFSET;	// tok 3
		szReturnValue = dcx_truefalse(IsWindow(GetDlgItem(m_Hwnd, nID)) || (getControlByID(nID) != nullptr));
	}
	// [NAME] [PROP]
	else if (prop == TEXT("nextid")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), getUniqueID() - mIRC_ID_OFFSET);
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
				const auto it = m_NamedIds.find(tsID);
				if (it != m_NamedIds.end())
					wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), it->second - mIRC_ID_OFFSET);
			}
		}
		else if ((N > -1) && (N < static_cast<int>(m_vpControls.size())))
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_vpControls[N]->getUserID());
	}
	// [NAME] [PROP]
	else if (prop == TEXT("ismenu")) {
		szReturnValue = dcx_truefalse(GetMenu(m_Hwnd) != nullptr);
	}
	// [NAME] [PROP]
	else if (prop == TEXT("ismarked")) {
		// no need to test anything, if it got here we already know its marked.
		szReturnValue = TEXT("$true");
	}
	// [NAME] [PROP]
	else if (prop == TEXT("parent")) {
		szReturnValue = getParentName().to_chr();
	}
	// [NAME] [PROP]
	else if (prop == TEXT("mouseid")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_MouseID);
	}
	// [NAME] [PROP]
	else if (prop == TEXT("focusid")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_FocusID);
	}
	// [NAME] [PROP]
	else if (prop == TEXT("mouse")) {
		POINT pt = { 0 };

		if (GetCursorPos(&pt))
			MapWindowPoints(nullptr, m_Hwnd, &pt, 1);

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
		szReturnValue = getAliasName().to_chr();
	}
	// [NAME] [PROP] [N]
	else if ((prop == TEXT("zlayer")) && (numtok > 2)) {
		const auto n = input.getnexttok().to_<VectorOfInts::size_type>();	// tok 3
		const auto size = m_vZLayers.size();

		if (n > size)
			throw Dcx::dcxException("Out Of Range");

		// return total number of id's
		if (n == 0)
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), size);
		// return the Nth id
		else
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_vZLayers[n - 1] - mIRC_ID_OFFSET);
	}
	// [NAME] [PROP]
	else if (prop == TEXT("zlayercurrent")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_zLayerCurrent + 1);
	}
	// [NAME] [PROP]
	else if (prop == TEXT("visible")) {
		szReturnValue = dcx_truefalse((IsWindowVisible(m_Hwnd) != FALSE));
	}
	// [NAME] [PROP]
	else if (prop == TEXT("glasscolor")) {
		RGBQUAD clr = { 0 };
		auto bOpaque = FALSE;
		if (SUCCEEDED(Dcx::VistaModule.dcxDwmGetColorizationColor((DWORD *)&clr, &bOpaque)))
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), RGB(clr.rgbRed, clr.rgbGreen, clr.rgbBlue));
		else
			szReturnValue = TEXT("-FAIL Unable to get Glass colour.");
	}
	// invalid info request
	else
		throw Dcx::dcxException("Invalid property or parameters");
}
#endif

/*!
 * \brief blah
 *
 * blah
 */

//bool DcxDialog::evalAliasEx(TCHAR *const szReturn, const int maxlen, const TCHAR *const szFormat, ...) {
//	TString line;
//	va_list args = nullptr;
//
//	va_start(args, szFormat);
//	line.tvprintf(szFormat, args);
//	va_end(args);
//
//	return evalAlias(szReturn, maxlen, line.to_chr());
//}
//
//bool DcxDialog::evalAlias(TCHAR *const szReturn, const int maxlen, const TCHAR *const szArgs)
//{
//	incRef();
//	
//	//const auto res = mIRCLinker::evalex(szReturn, maxlen, TEXT("$%s(%s,%s)"), getAliasName().to_chr(), getName().to_chr(), MakeTextmIRCSafe(szArgs).to_chr());
//	const auto res = mIRCLinker::eval(szReturn, TEXT("$%(%,%)"), getAliasName(), getName(), MakeTextmIRCSafe(szArgs));
//
//	decRef();
//
//	return res;
//}

bool DcxDialog::evalAliasEx(const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturn, const int maxlen, const TCHAR *const szFormat, ...) {
	TString line;
	va_list args = nullptr;

	va_start(args, szFormat);
	line.tvprintf(szFormat, args);
	va_end(args);

	return evalAlias(szReturn, maxlen, line.to_chr());
}

bool DcxDialog::evalAlias(const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturn, const int maxlen, const TCHAR *const szArgs)
{
	incRef();
	Auto(decRef());

	//return mIRCLinker::evalex(szReturn, maxlen, TEXT("$%s(%s,%s)"), getAliasName().to_chr(), getName().to_chr(), MakeTextmIRCSafe(szArgs).to_chr());
	return mIRCLinker::eval(szReturn, TEXT("$%(%,%)"), getAliasName(), getName(), MakeTextmIRCSafe(szArgs));
}

bool DcxDialog::execAliasEx(const TCHAR *const szFormat, ...) {
	TString line;
	va_list args = nullptr;

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
	if (m_pLayoutManager == nullptr)
		return false;

	return m_pLayoutManager->updateLayout(rc);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::setMouseControl(const UINT mUID) {
	if (mUID != m_MouseID) {
		if (dcx_testflag(m_dEventMask, DCX_EVENT_MOUSE))
		{
			execAliasEx(TEXT("mouseleave,%u"), m_MouseID);
			execAliasEx(TEXT("mouseenter,%u"), mUID);
		}
		m_MouseID = mUID;
	}
	else if (dcx_testflag(m_dEventMask, DCX_EVENT_MOUSE))
		execAliasEx(TEXT("mouse,%u"), mUID);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::setFocusControl(const UINT mUID) {
	if (mUID != m_FocusID) {
		if (dcx_testflag(m_dEventMask, DCX_EVENT_FOCUS))
		{
			execAliasEx(TEXT("focusout,%u"), m_FocusID);
			execAliasEx(TEXT("focus,%u"), mUID);
		}
		m_FocusID = mUID;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT WINAPI DcxDialog::WindowProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	auto p_this = static_cast<DcxDialog *>(GetProp(mHwnd, TEXT("dcx_this")));

	//// sanity check for prop existing.
	//if ((p_this == nullptr) || (p_this->m_hOldWindowProc == nullptr))
	//	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
	//
	//// If Message is blocking just call old win proc
	//if ((InSendMessageEx(nullptr) & (ISMEX_REPLIED|ISMEX_SEND)) == ISMEX_SEND)
	//	return CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);

	// sanity check for prop existing.
	if ((p_this == nullptr) || (p_this->m_hDefaultWindowProc == nullptr))
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	// If Message is blocking just call old win proc
	if ((InSendMessageEx(nullptr) & (ISMEX_REPLIED | ISMEX_SEND)) == ISMEX_SEND)
		return p_this->CallDefaultProc(mHwnd, uMsg, wParam, lParam);

	BOOL bParsed = FALSE;
	LRESULT lRes = 0L;

	// TODO: Is this instruction needed? Referencecount is managed by callAliasEx too ...
	//p_this->incRef();

	switch (uMsg) {
		case WM_HELP:
			{
				if (dcx_testflag(p_this->getEventMask(), DCX_EVENT_HELP))
					p_this->execAlias(TEXT("help,0"));
				bParsed = TRUE;
				lRes = TRUE;
			}
			break;

		case WM_THEMECHANGED:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_THEME))
					p_this->execAlias(TEXT("themechanged,0"));
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
						//mIRCLinker::execex(TEXT("/.timer -m 1 0 xdialog -x %s"), p_this->getName().to_chr());
						mIRCLinker::exec(TEXT("/.timer -m 1 0 xdialog -x %"), p_this->getName());
					}
					else if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLOSE)) {
						if (p_this->evalAliasT(TEXT("close,0")).second == TEXT("noclose"))
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
				auto cHwnd = GetDlgItem(mHwnd, static_cast<int>(wParam));
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

						lpmis->itemWidth = static_cast<UINT>(size.cx);
						lpmis->itemHeight = static_cast<UINT>(size.cy);
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
							//TCHAR ret[256];
							//p_this->evalAlias(ret, Dcx::countof(ret), TEXT("beginmove,0"));
							//
							//if (ts_strcmp(TEXT("nomove"), ret) != 0) {
							//	bParsed = TRUE;	// Ook: needs looked at as do the other switches here...
							//	p_this->m_bInMoving = true;
							//	lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
							//}

							//stString<256> sRet;
							//p_this->evalAlias(sRet, Dcx::countof(sRet), TEXT("beginmove,0"));
							//
							//if (sRet != TEXT("nomove")) {
							//	bParsed = TRUE;	// Ook: needs looked at as do the other switches here...
							//	p_this->m_bInMoving = true;
							//	lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
							//}

							if (p_this->evalAliasT(TEXT("beginmove,0")).second != TEXT("nomove"))
							{
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
							//TCHAR ret[256];
							//
							//p_this->evalAlias(ret, Dcx::countof(ret), TEXT("scclose,0"));
							//
							//if (ts_strcmp(TEXT("noclose"), ret) == 0)
							//	bParsed = TRUE;

							//stString<256> sRet;
							//
							//p_this->evalAlias(sRet, Dcx::countof(sRet), TEXT("scclose,0"));
							//
							//if (sRet == TEXT("noclose"))
							//	bParsed = TRUE;

							if (p_this->evalAliasT(TEXT("scclose,0")).second == TEXT("noclose"))
								bParsed = TRUE;
						}
						break;
					}

				case SC_MINIMIZE:
					{
						if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_SIZE)) {
							//TCHAR ret[256];
							//
							//p_this->evalAlias(ret, Dcx::countof(ret), TEXT("min,0"));
							//
							//if (ts_strcmp(TEXT("stop"), ret) != 0) {
							//	bParsed = TRUE;
							//	lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
							//}

							//stString<256> sRet;
							//
							//p_this->evalAlias(sRet, Dcx::countof(sRet), TEXT("min,0"));
							//
							//if (sRet != TEXT("stop")) {
							//	bParsed = TRUE;
							//	lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
							//}

							if (p_this->evalAliasT(TEXT("min,0")).second != TEXT("stop"))
							{
								bParsed = TRUE;
								lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
							}
						}
						break;
					}

				case SC_MAXIMIZE:
					{
						if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_SIZE)) {
							//TCHAR ret[256];
							//
							//p_this->evalAlias(ret, Dcx::countof(ret), TEXT("max,0"));
							//
							//if (ts_strcmp(TEXT("stop"), ret) != 0) {
							//	bParsed = TRUE;
							//	lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
							//}

							//stString<256> sRet;
							//
							//p_this->evalAlias(sRet, sRet.size(), TEXT("max,0"));
							//
							//if (sRet != TEXT("stop")) {
							//	bParsed = TRUE;
							//	lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
							//}

							if (p_this->evalAliasT(TEXT("max,0")).second != TEXT("stop"))
							{
								bParsed = TRUE;
								lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
							}
						}

						break;
					}

				case SC_RESTORE:
					{
						if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_SIZE)) {
							p_this->execAlias(TEXT("restore,0"));

							//bParsed = TRUE;
							//lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
						}

						break;
					}

				case SC_SIZE:
					{
						//TCHAR ret[256] = {0};
						//
						//if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_SIZE)) // mask only controls sending of event, if event isnt sent then DefWindowProc should still be called.
						//	p_this->evalAlias(ret, Dcx::countof(ret), TEXT("beginsize,0"));
						//
						//if (ts_strcmp(TEXT("nosize"), ret) != 0) {
						//	bParsed = TRUE;
						//	p_this->m_bInSizing = true;
						//	lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
						//}

						//gsl::wstring_span<256> sRet;
						//
						//if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_SIZE)) // mask only controls sending of event, if event isnt sent then DefWindowProc should still be called.
						//	p_this->evalAlias(sRet.data(), sRet.length(), TEXT("beginsize,0"));
						//
						//if (sRet != TEXT("nosize")) {
						//	bParsed = TRUE;
						//	p_this->m_bInSizing = true;
						//	lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
						//}

						//stString<256> sRet;
						//
						//if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_SIZE)) // mask only controls sending of event, if event isnt sent then DefWindowProc should still be called.
						//	p_this->evalAlias(sRet, static_cast<int>(sRet.size()), TEXT("beginsize,0"));
						//
						//if (sRet != TEXT("nosize")) {
						//	bParsed = TRUE;
						//	p_this->m_bInSizing = true;
						//	lRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);
						//}

						bool bDoSize = false;
						if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_SIZE)) // mask only controls sending of event, if event isnt sent then DefWindowProc should still be called.
							bDoSize = (p_this->evalAliasT(TEXT("beginsize,0")).second != TEXT("nosize"));

						if (bDoSize)
						{
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
				if (p_this->m_uGhostDragAlpha < 255) {
					if (!p_this->m_bVistaStyle) {
						const auto style = GetWindowExStyle(mHwnd);
						// Set WS_EX_LAYERED on this window
						if (!dcx_testflag(style,WS_EX_LAYERED))
							SetWindowLong(mHwnd, GWL_EXSTYLE, static_cast<LONG>(style) | WS_EX_LAYERED);
						// Make this window alpha
						SetLayeredWindowAttributes(mHwnd, 0, p_this->m_uGhostDragAlpha, LWA_ALPHA);
					}
					p_this->m_bGhosted = true;
				}
				p_this->UpdateVistaStyle();
			}
			break;

		case WM_EXITSIZEMOVE:
			{
				if ((p_this->m_bInSizing) && (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_SIZE)))
					p_this->execAlias(TEXT("endsize,0"));
				else if ((p_this->m_bInMoving)  && (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_MOVE)))
					p_this->execAlias(TEXT("endmove,0"));

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
					p_this->execAlias(TEXT("moving,0"));
				break;
			}

		case WM_SIZE:
			{
				// After window region is set it needs updated whenever the dialog is resized.
				// No way to scale a region tho :/
				//HRGN hRGN = CreateRectRgn(0,0,0,0);
				//if (GetWindowRgn(p_m_Hwnd, hRGN) != ERROR)
				//{
				//}
				//DeleteObject(hRGN);

				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_SIZE))
					p_this->execAliasEx(TEXT("sizing,0,%d,%d"), LOWORD(lParam), HIWORD(lParam));

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

		case WM_WINDOWPOSCHANGING:
			{
				dcxlParam(LPWINDOWPOS, wp);

				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_MOVE)) {

					// break if nomove & nosize specified, since thats all we care about.
					if ((wp == nullptr) || ((dcx_testflag(wp->flags, SWP_NOMOVE)) && (dcx_testflag(wp->flags, SWP_NOSIZE))))
						break;

					//TCHAR ret[256] = { 0 }, *p = nullptr;
					//
					//switch ((wp->flags & (SWP_NOSIZE | SWP_NOMOVE))) {
					//case SWP_NOSIZE:
					//	p = TEXT("moving");
					//	break;
					//case SWP_NOMOVE:
					//	p = TEXT("sizing");
					//	break;
					//case (SWP_NOSIZE | SWP_NOMOVE) :
					//	p = TEXT("neither");
					//	break;
					//default:
					//	p = TEXT("both");
					//	break;
					//}
					//
					//p_this->evalAliasEx(ret, Dcx::countof(ret), TEXT("changing,0,%s,%d,%d,%d,%d"), p, wp->x, wp->y, wp->cx, wp->cy);
					//
					//if (ts_strcmp(TEXT("nosize"), ret) == 0)
					//	wp->flags |= SWP_NOSIZE;
					//else if (ts_strcmp(TEXT("nomove"), ret) == 0)
					//	wp->flags |= SWP_NOMOVE;
					//else if (ts_strcmp(TEXT("nochange"), ret) == 0)
					//	wp->flags |= SWP_NOSIZE | SWP_NOMOVE;

					TCHAR *p = nullptr;

					switch ((wp->flags & (SWP_NOSIZE | SWP_NOMOVE))) {
					case SWP_NOSIZE:
						p = TEXT("moving");
						break;
					case SWP_NOMOVE:
						p = TEXT("sizing");
						break;
					case (SWP_NOSIZE | SWP_NOMOVE):
						p = TEXT("neither");
						break;
					default:
						p = TEXT("both");
						break;
					}

					//stString<256> sRet;
					//
					//p_this->evalAliasEx(sRet, static_cast<int>(sRet.size()), TEXT("changing,0,%s,%d,%d,%d,%d"), p, wp->x, wp->y, wp->cx, wp->cy);
					//
					//if (sRet == TEXT("nosize"))
					//	wp->flags |= SWP_NOSIZE;
					//else if (sRet == TEXT("nomove"))
					//	wp->flags |= SWP_NOMOVE;
					//else if (sRet == TEXT("nochange"))
					//	wp->flags |= SWP_NOSIZE | SWP_NOMOVE;

#if DCX_USE_HASHING
					switch (std::hash<TString>()(p_this->evalAliasT(TEXT("changing,0,%,%,%,%,%"), p, wp->x, wp->y, wp->cx, wp->cy).second))
					{
					case TEXT("nosize"_hash):
						wp->flags |= SWP_NOSIZE;
						break;
					case TEXT("nomove"_hash):
						wp->flags |= SWP_NOMOVE;
						break;
					case TEXT("nochange"_hash):
						wp->flags |= SWP_NOSIZE | SWP_NOMOVE;
						break;
					}
#else
					auto sRet = p_this->evalAliasT(TEXT("changing,0,%,%,%,%,%"), p, wp->x, wp->y, wp->cx, wp->cy).second;

					if (sRet == TEXT("nosize"))
						wp->flags |= SWP_NOSIZE;
					else if (sRet == TEXT("nomove"))
						wp->flags |= SWP_NOMOVE;
					else if (sRet == TEXT("nochange"))
						wp->flags |= SWP_NOSIZE | SWP_NOMOVE;
#endif

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
					//lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
					lRes = p_this->CallDefaultProc(mHwnd, uMsg, wParam, lParam);

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
						p_this->execAlias(TEXT("denter,0")); // this tells you when the mouse enters or
					p_this->UpdateVistaStyle();
				}
				break;
			}

		case WM_MOUSELEAVE:
			{
				if ( p_this->m_bTracking ) {
					p_this->m_bTracking = FALSE;
					if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_MOUSE))
						p_this->execAlias(TEXT("dleave,0")); // leaves a dialogs client area.
				}
				p_this->UpdateVistaStyle();
			}
			break;

		case WM_LBUTTONDOWN:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK))
					p_this->execAlias(TEXT("lbdown,0"));
				if (p_this->m_bDoDrag)
					p_this->m_bDrag = true;
				break;
			}

		case WM_LBUTTONUP:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK)) {
					p_this->execAlias(TEXT("lbup,0"));
					p_this->execAlias(TEXT("sclick,0"));
				}
				break;
			}

		case WM_LBUTTONDBLCLK:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK)) {
					p_this->execAlias(TEXT("dclick,0"));
					p_this->execAlias(TEXT("lbdblclk,0"));
				}
				break;
			}

		case WM_RBUTTONDOWN:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK))
					p_this->execAlias(TEXT("rbdown,0"));
				break;
			}

		case WM_RBUTTONUP:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK)) {
					p_this->execAlias(TEXT("rbup,0"));
					p_this->execAlias(TEXT("rclick,0"));
				}
				break;
			}

		case WM_RBUTTONDBLCLK:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK))
					p_this->execAlias(TEXT("rbdblclk,0"));
				break;
			}

		case WM_MBUTTONDOWN:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK))
					p_this->execAlias(TEXT("mbdown,0"));
				break;
			}

		case WM_MBUTTONUP:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK))
					p_this->execAlias(TEXT("mbup,0"));
				break;
			}

		case WM_MBUTTONDBLCLK:
			{
				if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_CLICK))
					p_this->execAlias(TEXT("mbdblclk,0"));
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

					p_this->execAliasEx(TEXT("mwheel,%u,%s,%s"),
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
					//lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
					lRes = p_this->CallDefaultProc(mHwnd, uMsg, wParam, lParam);
					bParsed = TRUE;

					auto hMenu = reinterpret_cast<HMENU>(wParam);

					if ((lRes == 0) && Dcx::XPopups.isMenuBarMenu(GetMenu(mHwnd), hMenu)) {

						m_bIsMenuBar = true;

						p_this->m_popup->convertMenu(hMenu, TRUE);
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
					p_this->m_popup->deleteAllItemData(reinterpret_cast<HMENU>(wParam));

				break;
			}

		case WM_SETCURSOR:
			{
				//if ((LOWORD(lParam) == HTCLIENT) && ((HWND) wParam == p_this->getHwnd()) && (p_this->getCursor() != nullptr))
				// removing the above checks allows the cursor to be customized for the whole dialog including menus...
				//if ((LOWORD(lParam) != HTERROR) && (p_this->getCursor() != nullptr))
				//{
				//	if (GetCursor() != p_this->getCursor())
				//		SetCursor(p_this->getCursor());
				//
				//	bParsed = TRUE;
				//	lRes = TRUE;
				//}

				auto wHitCode = LOWORD(lParam);

				if ((HWND)wParam == p_this->getHwnd())
				{
					auto hCursor = p_this->getCursor(wHitCode);
					if (hCursor == nullptr)
						hCursor = p_this->getCursor();

					if (hCursor != nullptr)
					{
						if (GetCursor() != hCursor)
							SetCursor(hCursor);

						bParsed = TRUE;
						lRes = TRUE;
					}
				}
				break;
			}

		case WM_DROPFILES:
			{
				dcxwParam(HDROP, files);

				//TCHAR filename[500];
				//const auto count = DragQueryFile(files, 0xFFFFFFFF, filename, Dcx::countof(filename));
				//
				//if (count > 0) {
				//	if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_DRAG)) {
				//		TCHAR ret[20];
				//
				//		p_this->evalAliasEx(ret, Dcx::countof(ret), TEXT("dragbegin,0,%u"), count);
				//
				//		// cancel drag drop event
				//		if (lstrcmpi(ret, TEXT("cancel")) == 0) {
				//			DragFinish(files);
				//			bParsed = TRUE;
				//			break;
				//		}
				//		// for each file, send callback message
				//		for (auto i = decltype(count){0}; i < count; i++) {
				//			if (DragQueryFile(files, i, filename, Dcx::countof(filename)))
				//				p_this->execAliasEx(TEXT("dragfile,0,%s"), filename);
				//		}
				//
				//		p_this->execAlias(TEXT("dragfinish,0"));
				//	}
				//}
				//
				//DragFinish(files);

				stString<500> stFilename;
				const auto count = DragQueryFile(files, 0xFFFFFFFF, stFilename, Dcx::countof(stFilename));

				if (count > 0) {
					if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_DRAG)) {
						//stString<20> sRet;
						//
						//p_this->evalAliasEx(sRet, static_cast<int>(sRet.size()), TEXT("dragbegin,0,%u"), count);
						//
						//// cancel drag drop event
						//if (sRet == TEXT("cancel")) {
						//	DragFinish(files);
						//	bParsed = TRUE;
						//	break;
						//}

						auto sRet = p_this->evalAliasT(TEXT("dragbegin,0,%"), count).second;

						// cancel drag drop event
						if (sRet == TEXT("cancel")) {
							DragFinish(files);
							bParsed = TRUE;
							break;
						}

						// for each file, send callback message
						for (auto i = decltype(count){0}; i < count; i++) {
							if (DragQueryFile(files, i, stFilename, stFilename.size()))
								p_this->execAliasEx(TEXT("dragfile,0,%s"), stFilename.data());
						}

						p_this->execAlias(TEXT("dragfinish,0"));
					}
				}

				DragFinish(files);
				break;
			}

		case WM_ACTIVATE:
			{
				switch (wParam) {
				case WA_ACTIVE:
				{
					if (p_this->m_bVistaStyle) {
						bParsed = TRUE;
						//lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
						lRes = p_this->CallDefaultProc(mHwnd, uMsg, wParam, lParam);
						InvalidateRect(mHwnd, nullptr, TRUE);
					}
				}
				case WA_CLICKACTIVE:
				{
					if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_FOCUS))
						p_this->execAlias(TEXT("activate,0"));
					p_this->m_bDialogActive = true;
					break;
				}

				case WA_INACTIVE:
				{
					if (dcx_testflag(p_this->m_dEventMask, DCX_EVENT_FOCUS))
						p_this->execAlias(TEXT("deactivate,0"));
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
				if (p_this->IsVistaStyle() && !IsIconic(mHwnd)) {
					ValidateRect(mHwnd, nullptr);
					p_this->UpdateVistaStyle();
					lRes = 0L;
					bParsed = TRUE;
				}
			}
			break;

		case WM_KEYDOWN:
		{
			if (dcx_testflag(p_this->getEventMask(), DCX_EVENT_EDIT)) {
				if (wParam == VK_RETURN)
					p_this->execAlias(TEXT("return,0"));

				//if ((p_this->m_bIgnoreRepeat) && (lParam & 0x40000000)) // ignore repeats
					// break;

				p_this->execAliasEx(TEXT("keydown,0,%d"), wParam);
			}
			break;
		}

		case WM_KEYUP:
		{
			if (dcx_testflag(p_this->getEventMask(), DCX_EVENT_EDIT))
				p_this->execAliasEx(TEXT("keyup,0,%d"), wParam);
			break;
		}

		case WM_NCDESTROY:
			{
				//if (IsWindow(mHwnd))
				//{
				//	if ((WNDPROC)GetWindowLongPtr(mHwnd, GWLP_WNDPROC) == DcxDialog::WindowProc)
				//		SubclassWindow(mHwnd, p_this->m_hOldWindowProc);
				//}
			//
				//lRes = CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);

				if (IsWindow(mHwnd))
				{
					if ((WNDPROC)GetWindowLongPtr(mHwnd, GWLP_WNDPROC) == DcxDialog::WindowProc)
						SubclassWindow(mHwnd, p_this->m_hDefaultWindowProc);
				}

				lRes = p_this->CallDefaultProc(mHwnd, uMsg, wParam, lParam);
			
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

	//return CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
	return p_this->CallDefaultProc(mHwnd, uMsg, wParam, lParam);
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

#if DCX_USE_WRAPPERS
	Dcx::dcxHDCBitmapResource hdcbmp(hdc, p_this->m_bitmapBg);
#else
	auto hdcbmp = CreateCompatibleDC(hdc);

	if (hdcbmp == nullptr)
		return;

	Auto(DeleteDC(hdcbmp));

	auto hOldBitmap = SelectBitmap(hdcbmp, p_this->m_bitmapBg);

	Auto(SelectBitmap(hdcbmp, hOldBitmap));
#endif

	auto x = rwnd->left;
	auto y = rwnd->top;
	const auto w = rwnd->right - rwnd->left;
	const auto h = rwnd->bottom - rwnd->top;

	// stretch
	if (dcx_testflag(p_this->m_uStyleBg, DBS_BKGSTRETCH)) {
		//BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcbmp, 0, 0, SRCCOPY);
		//TransparentBlt(hdc, x, y, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, p_this->m_colTransparentBg);
		
		SetStretchBltMode(hdc, STRETCH_HALFTONE);
		SetBrushOrgEx(hdc, 0, 0, nullptr);
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

/*
**	Show error messages, internal function called by showError() & showControlError()
*/
void DcxDialog::i_showError(const TCHAR *const cType, const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const err) const
{
	if (m_bErrorTriggered)
		return;

	m_bErrorTriggered = true;

	if (IsVerbose())
	{
		//TString res;
		//if (prop != nullptr)
		//	res.tsprintf(TEXT("D_IERROR %s(%s).%s: %s"), cType, getName().to_chr(), prop, err);
		//else
		//	res.tsprintf(TEXT("D_CERROR %s %s %s: %s"), cType, cmd, getName().to_chr(), err);
		//mIRCLinker::echo(res.to_chr());

		TString res;
		if (prop != nullptr)
			_ts_sprintf(res, TEXT("D_IERROR %(%).%: %"), cType, getName(), prop, err);
		else
			_ts_sprintf(res, TEXT("D_CERROR % % %: %"), cType, cmd, getName(), err);
		mIRCLinker::echo(res.to_chr());
	}

	const_cast<DcxDialog *>(this)->execAliasEx(TEXT("error,0,dialog,%s,%s,%s"), (prop != nullptr ? prop : TEXT("none")), (cmd != nullptr ? cmd : TEXT("none")), err);

	m_bErrorTriggered = false;
}

/*
**	Show error messages related to /xdialog & $xdialog() calls
*/
void DcxDialog::showError(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const err) const
{
	i_showError(TEXT("xdialog"), prop, cmd, err);
}

void DcxDialog::showErrorEx(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const fmt, ...) const
{
	TString err;
	va_list args;

	va_start( args, fmt );
	err.tvprintf(fmt, args);
	va_end( args );

	showError(prop, cmd, err.to_chr());
}

/*
**	Show error messages related to /xdid & $xdid() calls
*/
void DcxDialog::showControlError(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const err) const
{
	i_showError(TEXT("xdid"), prop, cmd, err);
}

void DcxDialog::showControlErrorEx(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const fmt, ...) const
{
	TString err;
	va_list args;

	va_start(args, fmt);
	err.tvprintf(fmt, args);
	va_end(args);

	showControlError(prop, cmd, err.to_chr());
}

void DcxDialog::CreateVistaStyle(void)
{
	if (Dcx::VistaModule.refreshComposite()) {
		// Vista+ only code. dont do anything at this point if vista+

		//DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;
		//
		////enable non-client area rendering on window
		//HRESULT hr = Dcx::VistaModule.dcxDwmSetWindowAttribute(m_Hwnd, DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));
		//if (SUCCEEDED(hr)) {
			//MARGINS margins = {this->m_GlassOffsets.left,this->m_GlassOffsets.right,this->m_GlassOffsets.top,this->m_GlassOffsets.bottom};
			//Dcx::VistaModule.dcxDwmExtendFrameIntoClientArea(m_Hwnd, &margins);
		//	//DWM_BLURBEHIND bb = {0};
		//
		//	//// Specify blur-behind and blur region.
		//	//bb.dwFlags = DWM_BB_ENABLE;
		//	//bb.fEnable = true;
		//	//bb.hRgnBlur = nullptr;
		//
		//	//// Enable blur-behind.
		//	//Dcx::VistaModule.dcxDwmEnableBlurBehindWindow(m_Hwnd, &bb);
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
		if (!GetWindowRect(m_Hwnd, &rc))
			return;

		const DWORD ExStyles = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_LEFT;
		const DWORD Styles = WS_VISIBLE/*|WS_OVERLAPPED*/|WS_CLIPCHILDREN;

		SIZE szWin{ (rc.right - rc.left),(rc.bottom - rc.top) };

		this->m_hFakeHwnd = CreateWindowEx(ExStyles,DCX_VISTACLASS,nullptr,Styles,rc.left,rc.top,szWin.cx,szWin.cy,m_Hwnd,nullptr,GetModuleHandle(nullptr), nullptr);
		if (IsWindow(this->m_hFakeHwnd)) {
			if (this->CreateVistaStyleBitmap(szWin))
			{
				SetLayeredWindowAttributes(m_Hwnd,0,5,LWA_ALPHA);

				if (auto hRgn = CreateRectRgn(0, 0, 0, 0); GetWindowRgn(m_Hwnd, hRgn))
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
	bmih.biSizeImage = static_cast<DWORD>(nBytesPerLine*szWin.cy);

	this->m_hVistaBitmap = ::CreateDIBSection(nullptr, (PBITMAPINFO)&bmih, DIB_RGB_COLORS, &this->m_pVistaBits, nullptr, 0);
	if (this->m_hVistaBitmap != nullptr) {
		memset( this->m_pVistaBits, 0, static_cast<size_t>(szWin.cx * 4 * szWin.cy));
		return true;
	}
#endif
	return false;
}

void DcxDialog::RemoveVistaStyle(void)
{
	m_bVistaStyle = false;
	if (IsWindow(m_hFakeHwnd)) {
		if (isExStyle(WS_EX_LAYERED))
			SetLayeredWindowAttributes(m_Hwnd,0,m_iAlphaLevel,LWA_ALPHA);
		DestroyWindow(m_hFakeHwnd);
	}
	if (m_hVistaBitmap != nullptr)
		DeleteBitmap(m_hVistaBitmap);

	m_hFakeHwnd = nullptr;
}

#ifdef DCX_USE_GDIPLUS
void DcxDialog::DrawCaret(Gdiplus::Graphics & graph)
{
	auto pWnd = GetFocus();
	if( pWnd == nullptr || !IsWindow(pWnd) )
		return;

	stString<MAX_PATH> strClassName;
	::GetClassName( pWnd, strClassName, static_cast<int>(strClassName.size()));

	if( strClassName != TEXT("EDIT"))
		return;

	POINT pt;
	if (GetCaretPos(&pt))
	{
		MapWindowPoints(pWnd, m_hFakeHwnd, &pt, 1);
		Gdiplus::Pen pen(Gdiplus::Color(0, 0, 0), 1.0f);	// black
		graph.DrawLine(&pen, pt.x, pt.y, pt.x, pt.y + 13);
	}
}

void DcxDialog::DrawDialog(Gdiplus::Graphics & graphics, HDC hDC)
{
#if DCX_USE_WRAPPERS
	RECT rc;
	if (!GetWindowRect(m_Hwnd, &rc))
		return;

	auto w = (rc.right - rc.left), h = (rc.bottom - rc.top);

	//Dcx::dcxBitmapResource hBitmap(hDC, w, h);
	//
	//Dcx::dcxHDCBitmapResource hdcMemory(hDC, hBitmap);
	//
	//SendMessage(m_Hwnd, WM_PRINT, (WPARAM)(HDC)hdcMemory, PRF_CLIENT | PRF_NONCLIENT | PRF_ERASEBKGND);
	//
	//Gdiplus::Bitmap bitmap(hBitmap, nullptr);
	//graphics.DrawImage(&bitmap, 0, 0);

	Dcx::dcxHDCBitmap2Resource hdcMemory(hDC, w, h);

	SendMessage(m_Hwnd, WM_PRINT, (WPARAM)(HDC)hdcMemory, PRF_CLIENT | PRF_NONCLIENT | PRF_ERASEBKGND);

	Gdiplus::Bitmap bitmap(hdcMemory.getBitMap(), nullptr);
	graphics.DrawImage(&bitmap, 0, 0);

#else
	RECT rc;
	if (!GetWindowRect(m_Hwnd, &rc))
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

	SendMessage(m_Hwnd, WM_PRINT, (WPARAM)(HDC)hdcMemory,PRF_CLIENT|PRF_NONCLIENT|PRF_ERASEBKGND);

	Gdiplus::Bitmap bitmap( hBitmap, nullptr);
	graphics.DrawImage( &bitmap, 0, 0);
#endif
}

void DcxDialog::DrawCtrl(Gdiplus::Graphics & graphics, HDC hDC, HWND hWnd)
{
#if DCX_USE_WRAPPERS
	if (!::IsWindow(hWnd) || !::IsWindowVisible(hWnd))
		return;

	RECT rc;
	if (!GetWindowRect(hWnd, &rc))
		return;

	MapVistaRect(nullptr, &rc);

	const auto w = (rc.right - rc.left), h = (rc.bottom - rc.top);

	// Don't bother drawing if its not visible in clip rect.
	if (!graphics.IsVisible(rc.left, rc.top, w, h))
		return;

	//Dcx::dcxBitmapResource hBitmap(hDC, w, h);
	//
	//Dcx::dcxHDCBitmapResource hdcMemory(hDC, hBitmap);
	//
	//SendMessage(hWnd, WM_PRINT, (WPARAM)(HDC)hdcMemory, (LPARAM)PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE | PRF_ERASEBKGND);
	//
	//Gdiplus::Bitmap bitmap(hBitmap, nullptr);
	//graphics.DrawImage(&bitmap, rc.left, rc.top);

	Dcx::dcxHDCBitmap2Resource hdcMemory(hDC, w, h);

	SendMessage(hWnd, WM_PRINT, (WPARAM)(HDC)hdcMemory, (LPARAM)PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE | PRF_ERASEBKGND);

	Gdiplus::Bitmap bitmap(hdcMemory.getBitMap(), nullptr);
	graphics.DrawImage(&bitmap, rc.left, rc.top);
#else
	if( !::IsWindow(hWnd) || !::IsWindowVisible(hWnd) )
		return;
	
	RECT rc;
	if (!GetWindowRect(hWnd, &rc))
		return;
	
	MapVistaRect(nullptr, &rc);
	
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
	
	Gdiplus::Bitmap bitmap( hBitmap, nullptr);
	graphics.DrawImage( &bitmap, rc.left, rc.top);
#endif
}
#endif

void DcxDialog::UpdateVistaStyle(const LPRECT rcUpdate)
{
#ifdef DCX_USE_GDIPLUS
	if (!IsWindow(m_hFakeHwnd) || !IsWindowVisible(m_Hwnd) || IsIconic(m_Hwnd) || m_hVistaBitmap == nullptr)
		return;

	{ // maintain a matching region.
		if (auto hRgn = CreateRectRgn(0, 0, 0, 0); GetWindowRgn(m_Hwnd, hRgn))
		{
			auto hFakeRgn = CreateRectRgn(0, 0, 0, 0);
			Auto(DeleteRgn(hFakeRgn));

			if (GetWindowRgn(m_hFakeHwnd, hFakeRgn))
			{
				if (!EqualRgn(hRgn, hFakeRgn))
					SetWindowRgn(m_hFakeHwnd, hRgn, FALSE);
				else
					DeleteRgn(hRgn);
			}
			else
				SetWindowRgn(m_hFakeHwnd, hRgn, FALSE);
		}
		else
			DeleteRgn(hRgn);
	}
	RECT rc = { 0 }, rcParentWin = { 0 }, rcParentClient = { 0 }, rcClip = { 0 };
	if (!GetWindowRect(m_hFakeHwnd, &rc))
		return;

	if (!GetWindowRect(m_Hwnd, &rcParentWin))
		return;

	if (!GetClientRect(m_Hwnd, &rcParentClient))
		return;

	MapWindowRect(m_Hwnd, nullptr, &rcParentClient);

	POINT ptSrc{ 0, 0 };
	POINT ptWinPos{ rc.left, rc.top };

	auto alpha = ((m_bGhosted) ? m_uGhostDragAlpha : m_iAlphaLevel);

	const auto half_alpha = (BYTE)(alpha / 2);

	BLENDFUNCTION stBlend = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };

	auto hDC = ::GetDC(m_hFakeHwnd);
	if (hDC == nullptr)
		return;

	Auto(ReleaseDC(m_hFakeHwnd, hDC));

	auto hdcMemory = ::CreateCompatibleDC(hDC);

	if (hdcMemory == nullptr)
		return;

	Auto(DeleteDC(hdcMemory));

	auto pvBits = m_pVistaBits;
	auto hbmpMem = m_hVistaBitmap;

	auto hbmpOld = SelectBitmap(hdcMemory, hbmpMem);

	Auto(SelectBitmap(hdcMemory, hbmpOld));

	m_hVistaHDC = hdcMemory;
	Auto(m_hVistaHDC = nullptr);

	Gdiplus::Graphics graph(hdcMemory);

	Auto(graph.ReleaseHDC(hdcMemory));

	graph.SetPageScale(1.0);
	graph.SetPageUnit(Gdiplus::UnitPixel);
	graph.SetSmoothingMode(Gdiplus::SmoothingModeNone);

	const SIZE szOffsets{ (rcParentClient.left - rcParentWin.left), (rcParentClient.top - rcParentWin.top) };

	m_sVistaOffsets = szOffsets;

	SIZE szWin{ (rc.right - rc.left), (rc.bottom - rc.top) };

	// Glass area = window edge +/- offset
	RECT glassOffsets{ szOffsets.cx + m_GlassOffsets.left, szOffsets.cy + m_GlassOffsets.top, szWin.cx - ((rcParentWin.right - rcParentClient.right) + m_GlassOffsets.right), szWin.cy - ((rcParentWin.bottom - rcParentClient.bottom) + m_GlassOffsets.bottom) };

	// Check for update rect (area of child control in screen coordinates)
	// If found set clipping area as child control's area.
	if (rcUpdate != nullptr) {
		CopyRect(&rcClip, rcUpdate);
		OffsetRect(&rcClip, -rcParentWin.left, -rcParentWin.top);
		Gdiplus::Rect clipRect(rcClip.left, rcClip.top , (rcClip.right - rcClip.left), (rcClip.bottom - rcClip.top));
		graph.SetClip(clipRect);
		DrawDialog( graph, hDC); // draw dialog after setting update controls clip rect.
	}
	else {
		// otherwise set clipping area to client area.
		rcClip.right = szWin.cx;
		rcClip.bottom = szWin.cy;
		DrawDialog( graph, hDC); // draw dialog before setting client area clip rect.
		Gdiplus::Rect clipRect(szOffsets.cx, szOffsets.cy, (rcParentClient.right - rcParentClient.left), (rcParentClient.bottom - rcParentClient.top));
		graph.SetClip(clipRect);
	}

	// Alpha Glass area when not ghost dragging & alpha isnt being set for whole dialog.
	// This method allows the glass area to be translucent but the controls to still be solid.
	// Update: commented out ghost & alpha check to allow glass area to still be seen when dialog as a whole is translucent
	//	make this behaviour optional?
	//if (!this->m_bGhosted && alpha == 255) {
		GdiFlush(); // sync drawing
		const auto Yend = std::min(rcClip.bottom, szWin.cy), Ybase = std::max(rcClip.top, 0L);
		const auto Xend = std::min(rcClip.right, szWin.cx), Xbase = std::max(rcClip.left, 0L);
		POINT pt = { 0 };

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
	auto hwndChild = ::GetWindow(m_Hwnd, GW_CHILD);
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
	UpdateLayeredWindow( m_hFakeHwnd, hDC, &ptWinPos, &szWin, hdcMemory, &ptSrc, 0, &stBlend, ULW_ALPHA);
#endif
}

void DcxDialog::SetVistaStylePos(void)
{
	if (!IsWindow(m_hFakeHwnd))
		return;

	RECT rc;
	if (GetWindowRect(m_Hwnd, &rc))
		SetWindowPos(m_hFakeHwnd, nullptr, rc.left, rc.top, 0,0, SWP_NOSIZE|SWP_NOZORDER);
}

void DcxDialog::SetVistaStyleSize(void)
{
	if (!IsWindow(m_hFakeHwnd))
		return;

	RECT rc;
	if (GetWindowRect(m_Hwnd, &rc))
	{
		const SIZE szWin{ (rc.right - rc.left), (rc.bottom - rc.top) };
		CreateVistaStyleBitmap(szWin);
		SetWindowPos(m_hFakeHwnd, nullptr, 0, 0, szWin.cx, szWin.cy, SWP_NOMOVE | SWP_NOZORDER);
	}
}

void DcxDialog::MapVistaRect(HWND hwnd, LPRECT rc) const
{
	MapWindowRect(hwnd, m_Hwnd, rc);
	OffsetRect(rc, m_sVistaOffsets.cx, m_sVistaOffsets.cy);
}

// Adds the control to the list
void DcxDialog::RegisterDragList(DcxList *const list)
{
   m_vDragLists.push_back(list);
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

	//auto itEnd = m_vDragLists.end();
	//auto itGot = std::find(m_vDragLists.begin(), itEnd, list);
	//if (itGot != itEnd)
	//	m_vDragLists.erase(itGot);

	Dcx::eraseIfFound(m_vDragLists, list);
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
	if (xml == nullptr)
		return;

	toXml(xml, TEXT("unnamed"));
}

void DcxDialog::toXml(TiXmlElement *const xml, const TString &name) const
{
	if (xml == nullptr)
		return;

	TString dest;
	TGetWindowText(m_Hwnd, dest);
	xml->SetAttribute("name", name.c_str());
	xml->SetAttribute("caption", dest.c_str());

	if (m_pLayoutManager != nullptr)
		m_pLayoutManager->getRoot()->toXml(xml);
}

TiXmlElement * DcxDialog::toXml() const
{
	auto result = new TiXmlElement("dialog");
	toXml(result);
	return result;
}

TiXmlElement * DcxDialog::toXml(const TString & name) const
{
	auto result = new TiXmlElement("dialog");
	toXml(result, name);
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
		return ((ID > (mIRC_ID_OFFSET - 1)) && !IsWindow(GetDlgItem(m_Hwnd, static_cast<int>(ID))) && (getControlByID(ID) == nullptr));
	//a control that already exists.
	return ((ID > (mIRC_ID_OFFSET - 1)) && IsWindow(GetDlgItem(m_Hwnd, static_cast<int>(ID))) && (getControlByID(ID) != nullptr));
}
