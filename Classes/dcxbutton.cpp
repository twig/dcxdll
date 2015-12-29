/*!
 * \file dcxbutton.cpp
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
#include "Classes/dcxbutton.h"
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

DcxButton::DcxButton(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
	, m_bBitmapText(FALSE)
	, m_bHasIcons(FALSE)
	, m_bHover(FALSE)
	, m_bTouched(FALSE)
	, m_bSelected(FALSE)
	, m_bTracking(FALSE)
	, m_iIconSize(16)
	, m_ImageList(nullptr)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	this->m_Hwnd = CreateWindowEx(
		ExStyles,
		DCX_BUTTONCLASS,
		nullptr,
		WS_CHILD | BS_PUSHBUTTON | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(this->m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(this->m_Hwnd, L" ", L" ");

	this->m_bNoTheme = (bNoTheme ? true : false);

	ZeroMemory(&this->m_aBitmaps, 4 * sizeof(HBITMAP));
	ZeroMemory(&this->m_aColors, 4 * sizeof(COLORREF));
	ZeroMemory(&this->m_aTransp, 4 * sizeof(COLORREF));

	this->m_aColors[0] = GetSysColor(COLOR_BTNTEXT); // normal
	//this->m_aColors[1] = GetSysColor(COLOR_BTNTEXT); // hover
	//this->m_aColors[2] = GetSysColor(COLOR_BTNTEXT); // pushed
	this->m_aColors[1] = this->m_aColors[0]; // hover
	this->m_aColors[2] = this->m_aColors[0]; // pushed
	this->m_aColors[3] = GetSysColor(COLOR_GRAYTEXT); // disabled

	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);

	if (styles.istok(TEXT("tooltips"))) {
		if (!IsWindow(p_Dialog->getToolTip()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");

		this->m_ToolTipHWND = p_Dialog->getToolTip();
		AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
	}

	// fix to allow pressing enter to work.
	if (dcx_testflag(Styles, BS_DEFPUSHBUTTON))
		SetFocus(this->m_Hwnd);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxButton::~DcxButton() {

	ImageList_Destroy(this->getImageList());

	for (auto i = 0U; i < Dcx::countof(this->m_aBitmaps); i++) {
		if (this->m_aBitmaps[i] != nullptr)
			DeleteBitmap(this->m_aBitmaps[i]);
	}

	this->unregistreDefaultWindowProc();
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxButton::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	*Styles |= BS_NOTIFY;

	for (const auto &tsStyle: styles)
	{
		if ( tsStyle == TEXT("bitmap") )
			*Styles |= BS_BITMAP;
		else if ( tsStyle == TEXT("default") )
			*Styles |= BS_DEFPUSHBUTTON;
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

void DcxButton::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const
{
	// [NAME] [ID] [PROP]
	if (input.gettok(3) == TEXT("text")) {
		// if copy fails drop through
		dcx_strcpyn(szReturnValue, this->m_tsCaption.to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxButton::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -c [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	if (flags[TEXT('c')] && numtok > 4) {
		const auto iColorStyles = this->parseColorFlags(input.getnexttok());	// tok 4
		const auto clrColor = input.getnexttok().to_<COLORREF>();		// tok 5

		if (dcx_testflag(iColorStyles, BTNCS_NORMAL))
			this->m_aColors[0] = clrColor;
		if (dcx_testflag(iColorStyles, BTNCS_HOVER))
			this->m_aColors[1] = clrColor;
		if (dcx_testflag(iColorStyles, BTNCS_SELECTED))
			this->m_aColors[2] = clrColor;
		if (dcx_testflag(iColorStyles, BTNCS_DISABLED))
			this->m_aColors[3] = clrColor;

		this->redrawWindow();
	}
	// xdid -k [NAME] [ID] [SWITCH] [+FLAGS] [COLOR] [FILENAME]
	else if (flags[TEXT('k')] && (numtok > 5) && (this->isStyle(BS_BITMAP) || this->isStyle(BS_OWNERDRAW))) {
		const auto iColorStyles = this->parseColorFlags(input.getnexttok());	// tok 4
		const auto clrColor = input.getnexttok().to_<COLORREF>();		// tok 5

		auto filename(input.getlasttoks().trim());	// tok 6, -1

		if (dcx_testflag(iColorStyles, BTNCS_NORMAL)) {
			this->m_aBitmaps[0] = dcxLoadBitmap(this->m_aBitmaps[0], filename);
			this->m_aTransp[0] = clrColor;
		}
		if (dcx_testflag(iColorStyles, BTNCS_HOVER)) {
			this->m_aBitmaps[1] = dcxLoadBitmap(this->m_aBitmaps[1], filename);
			this->m_aTransp[1] = clrColor;
		}
		if (dcx_testflag(iColorStyles, BTNCS_SELECTED)) {
			this->m_aBitmaps[2] = dcxLoadBitmap(this->m_aBitmaps[2], filename);
			this->m_aTransp[2] = clrColor;
		}
		if (dcx_testflag(iColorStyles, BTNCS_DISABLED)) {
			this->m_aBitmaps[3] = dcxLoadBitmap(this->m_aBitmaps[3], filename);
			this->m_aTransp[3] = clrColor;
		}

		this->redrawWindow( );
	}
	// xdid -l [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('l')] && numtok > 3) {
		const auto size = input.getnexttok().to_<UINT>();	// tok 4

		if (size == 32 || size == 24)
			this->m_iIconSize = size;
		else
			this->m_iIconSize = 16;

		if (this->getImageList() != nullptr) {
			ImageList_Destroy(this->getImageList());
			this->setImageList(nullptr);
			this->m_bHasIcons = FALSE;
		}
	}
	// xdid -t [NAME] [ID] [SWITCH] ItemText
	else if ( flags[TEXT('t')] && numtok > 2 ) {
		this->m_tsCaption = (numtok > 3 ? input.getlasttoks().trim() : TEXT(""));	// tok 4, -1
		this->redrawWindow();
	}
	// xdid -w [NAME] [ID] [SWITCH] [FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')] && numtok > 5) {
		const auto tflags(input.getnexttok());		// tok 4
		const auto index = input.getnexttok().to_int();	// tok 5
		const auto flag = parseColorFlags(tflags);
		auto filename(input.getlasttoks());			// tok 6, -1

		// load the icon

		//auto icon = dcxLoadIcon(index, filename, (this->m_iIconSize > 16), tflags);
		//
		//if (icon == nullptr)
		//	throw Dcx::dcxException("Unable to load icon");
		//
		//// prepare the image list
		//himl = this->getImageList();
		//if (himl == nullptr) {
		//	himl = this->createImageList();
		//
		//	if (himl != nullptr) {
		//		this->setImageList(himl);
		//
		//		ImageList_AddIcon(himl, icon);
		//		ImageList_AddIcon(himl, icon);
		//		ImageList_AddIcon(himl, icon);
		//		ImageList_AddIcon(himl, icon);
		//
		//		this->m_bHasIcons = TRUE;
		//	}
		//}
		//else {
		//	if (dcx_testflag(flag, BTNCS_DISABLED))
		//		ImageList_ReplaceIcon(himl, 3, icon);
		//	if (dcx_testflag(flag, BTNCS_SELECTED))
		//		ImageList_ReplaceIcon(himl, 2, icon);
		//	if (dcx_testflag(flag, BTNCS_HOVER))
		//		ImageList_ReplaceIcon(himl, 1, icon);
		//	if (dcx_testflag(flag, BTNCS_NORMAL))
		//		ImageList_ReplaceIcon(himl, 0, icon);
		//}
		//DestroyIcon(icon);

		Dcx::dcxIcon icon(index, filename, (this->m_iIconSize > 16), tflags);

		// prepare the image list
		auto himl = this->getImageList();
		if (himl == nullptr) {
			himl = this->createImageList();

			if (himl != nullptr) {
				this->setImageList(himl);

				ImageList_AddIcon(himl, icon);
				ImageList_AddIcon(himl, icon);
				ImageList_AddIcon(himl, icon);
				ImageList_AddIcon(himl, icon);

				this->m_bHasIcons = TRUE;
			}
		}
		else {
			if (dcx_testflag(flag, BTNCS_DISABLED))
				ImageList_ReplaceIcon(himl, 3, icon);
			if (dcx_testflag(flag, BTNCS_SELECTED))
				ImageList_ReplaceIcon(himl, 2, icon);
			if (dcx_testflag(flag, BTNCS_HOVER))
				ImageList_ReplaceIcon(himl, 1, icon);
			if (dcx_testflag(flag, BTNCS_NORMAL))
				ImageList_ReplaceIcon(himl, 0, icon);
		}

		this->redrawWindow();
	}
	// xdid -m [NAME] [ID] [SWITCH] [ENABLED]
	else if (flags[TEXT('m')] && numtok > 3) {
		const auto b = input.getnexttok().to_int();	// tok 4

		this->m_bBitmapText = (b > 0);	// any value > 0 taken as being true
		this->redrawWindow();
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

const UINT DcxButton::parseColorFlags(const TString & flags) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')]) 
		return iFlags;

	if (xflags[TEXT('d')])
		iFlags |= BTNCS_DISABLED;
	if (xflags[TEXT('h')])
		iFlags |= BTNCS_HOVER;
	if (xflags[TEXT('n')])
		iFlags |= BTNCS_NORMAL;
	if (xflags[TEXT('s')])
		iFlags |= BTNCS_SELECTED;
	if (xflags[TEXT('g')])
		iFlags |= BTNIS_GREY;
	if (xflags[TEXT('a')])
		iFlags |= BTNIS_ASSOC;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

const HIMAGELIST &DcxButton::getImageList(  ) const {

	return this->m_ImageList;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxButton::setImageList( const HIMAGELIST himl ) {

	this->m_ImageList = himl;
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxButton::createImageList() {
	return ImageList_Create(this->m_iIconSize, this->m_iIconSize, ILC_COLOR32 | ILC_MASK, 1, 0);
}

const TString DcxButton::getStyles(void) const
{
	auto tsStyles(__super::getStyles());
	const auto Styles = GetWindowStyle(this->m_Hwnd);

	if (dcx_testflag(Styles, BS_BITMAP))
		tsStyles.addtok(TEXT("bitmap"));
	if (dcx_testflag(Styles, BS_DEFPUSHBUTTON)) 
		tsStyles.addtok(TEXT("default"));

	return tsStyles;
}
	
void DcxButton::toXml(TiXmlElement *const xml) const
{
	__super::toXml(xml);
	xml->SetAttribute("caption", this->m_tsCaption.c_str());
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxButton::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg )
	{
		case WM_COMMAND:
		{
			switch ( HIWORD( wParam ) )
			{
			case BN_CLICKED:
				{
					if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
						this->execAliasEx(TEXT("%s,%d"), TEXT("sclick"), this->getUserID( ) );
				}
				break;
			case BN_DBLCLK:
				{
					if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
						this->execAliasEx(TEXT("%s,%d"), TEXT("dclick"), this->getUserID( ) );
				}
				break;
			}
		}
		break;
		//case WM_NOTIFY:
		//	{
		//		LPNMHDR pnmh = (LPNMHDR)lParam;
		//		switch (pnmh->code) {
		//			case BCN_HOTITEMCHANGE:
		//				{
		//					LPNMBCHOTITEM item = (LPNMBCHOTITEM)lParam;
		//					if (item->dwFlags & HICF_ENTERING)
		//						mIRCError(TEXT("Entering Button"));
		//					else
		//						mIRCError(TEXT("Leaving Button"));
		//				}
		//				break;
		//			default:
		//				break;
		//		}
		//	}
		//	break;
	}
	return 0L;
}

LRESULT DcxButton::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

	case WM_MOUSEMOVE:
		{
			this->m_pParentDialog->setMouseControl( this->getUserID( ) );

			if ( this->m_bTracking == FALSE ) {
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.hwndTrack = this->m_Hwnd;
				tme.dwFlags = TME_LEAVE | TME_HOVER;
				tme.dwHoverTime = HOVER_DEFAULT; //1;
				this->m_bTracking = (BOOL) _TrackMouseEvent( &tme );		
			}

			if (!this->m_bTouched) {
				this->m_bTouched = true;
				InvalidateRect( this->m_Hwnd, nullptr, FALSE );
			}
		}
		break;

	case WM_MOUSEHOVER:
		{
			if ( !this->m_bHover && this->m_bTracking )
				this->m_bHover = true;
		}
		break;

	case WM_MOUSELEAVE:
		{
			if ( this->m_bTracking ) {
				this->m_bHover = false;
				this->m_bTracking = FALSE;
				this->m_bTouched = false;
				this->m_bSelected = false;
				InvalidateRect( this->m_Hwnd, nullptr, FALSE );
			}
		}
		break;

	case WM_LBUTTONDOWN:
		{
			if (!this->m_bSelected) {
				this->m_bSelected = true;
				InvalidateRect( this->m_Hwnd, nullptr, FALSE );
			}
			if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
				this->execAliasEx(TEXT("%s,%d"), TEXT("lbdown"), this->getUserID());
		}
		break;

	case WM_LBUTTONUP:
		{
			this->m_bSelected = false;
			if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
				this->execAliasEx(TEXT("%s,%d"), TEXT("lbup"), this->getUserID());
		}
		break;

	case WM_ENABLE:
		{
			InvalidateRect( this->m_Hwnd, nullptr, TRUE );
		}
		break;

	case WM_UPDATEUISTATE:
		{
			InvalidateRect( this->m_Hwnd, nullptr, TRUE );
		}
		break;

	case WM_ERASEBKGND:
		{
			bParsed = TRUE;
			return TRUE;
		}
		break;
	case WM_PRINTCLIENT:
		{
			this->DrawClientArea((HDC)wParam, uMsg, lParam);
			bParsed = TRUE;
		}
		break;
	case WM_PAINT:
		{
			bParsed = TRUE;
			PAINTSTRUCT ps;

			auto hdc = BeginPaint(this->m_Hwnd, &ps);

			this->DrawClientArea(hdc, uMsg, lParam);

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
		return this->CommonMessage( uMsg, wParam, lParam, bParsed);
		break;
  }

  return 0L;
}

void DcxButton::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	BOOL isBitmap = this->isStyle(BS_BITMAP);
	int nState; // get buttons state.
	RECT rcClient;

	// get controls client area
	if (!GetClientRect(this->m_Hwnd, &rcClient))
		return;

	// get controls width & height.
	const auto w = (rcClient.right - rcClient.left), h = (rcClient.bottom - rcClient.top);

	if ( IsWindowEnabled( this->m_Hwnd ) == FALSE )
		nState = 3;
	else if ( this->m_bSelected )
		nState = 2;
	else if ( this->m_bTouched )
		nState = 1;
	else
		nState = 0;

	// Setup alpha blend if any.
	auto ai = this->SetupAlphaBlend(&hdc);
	Auto(this->FinishAlphaBlend(ai));

	HTHEME hTheme = nullptr;
	int iStateId = 0;
	if (!this->m_bNoTheme && Dcx::UXModule.dcxIsThemeActive()) {
		hTheme = Dcx::UXModule.dcxOpenThemeData(this->m_Hwnd, L"BUTTON");

		// this allows the theme buttons to have a transparent background like the normal ones
		switch (nState)
		{
		case 1:
			iStateId = PBS_HOT;
			break;
		case 2:
			iStateId = PBS_PRESSED;
			break;
		case 3:
			iStateId = PBS_DISABLED;
			break;
		default:
			iStateId = PBS_NORMAL;
			break;
		}
	}

	// fill background.
	if (this->isExStyle(WS_EX_TRANSPARENT))
	{ // fill with parent image
		if (!this->m_bAlphaBlend)
			this->DrawParentsBackground(hdc,&rcClient);
	}
	else // normal bkg
		DcxControl::DrawCtrlBackground(hdc,this,&rcClient, hTheme, BP_PUSHBUTTON, iStateId);

	// Bitmapped button
	if (isBitmap) {
		// create a new HDC for background rendering
		Dcx::dcxHDC hdcbmp(hdc);

		BITMAP bmp;

		// get bitmaps info.
		if (GetObject(this->m_aBitmaps[nState], sizeof(BITMAP), &bmp) != 0)
		{
			// associate bitmap with HDC
			const auto oldbm = SelectBitmap(hdcbmp, this->m_aBitmaps[nState]);
			Auto(SelectBitmap(hdcbmp, oldbm)); // got to put the old bm back.

			TransparentBlt(hdc, rcClient.left, rcClient.top, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, this->m_aTransp[nState]);
		}

		//auto hdcbmp = CreateCompatibleDC(hdc);
		//if (hdcbmp != nullptr) {
		//	BITMAP bmp;
		//
		//	// get bitmaps info.
		//	if (GetObject(this->m_aBitmaps[nState], sizeof(BITMAP), &bmp) != 0)
		//	{
		//		// associate bitmap with HDC
		//		const auto oldbm = SelectBitmap(hdcbmp, this->m_aBitmaps[nState]);
		//		TransparentBlt(hdc, rcClient.left, rcClient.top, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, this->m_aTransp[nState]);
		//		SelectBitmap(hdcbmp, oldbm); // got to put the old bm back.
		//	}
		//	DeleteDC( hdcbmp );
		//}
	}

	// Regular button
	if ((!isBitmap) || (this->m_bBitmapText)) {          
		// draw default window bg
		if (!isBitmap) {
			if (hTheme != nullptr) {
				// This method causes the button to valis when the dialog is resized using the redrawBuffered() function.
				//HRGN hRgn = nullptr;
				//if (GetThemeBackgroundRegionUx(hTheme, hdc, BP_PUSHBUTTON, iStateId, &rcClient, &hRgn) == S_OK)
				//	SelectClipRgn(hdc, hRgn);
				//
				//CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM)hdc, lParam);
				//if (hRgn != nullptr)
				//	DeleteRgn(hRgn);

				// This method causes the theme bkg to re-appear during resize, but button is otherwise drawn correctly.
				if (uMsg != WM_PRINTCLIENT) {
					HRGN hRgn = nullptr;
					if (Dcx::UXModule.dcxGetThemeBackgroundRegion(hTheme, hdc, BP_PUSHBUTTON, iStateId, &rcClient, &hRgn) == S_OK)
					{
						auto hZeroRgn = CreateRectRgn(0, 0, 0, 0);
						if ((hRgn != nullptr) && (!EqualRgn(hRgn, hZeroRgn)))
							SelectClipRgn(hdc, hRgn);
						DeleteRgn(hZeroRgn);
						DeleteRgn(hRgn);
					}
				}
				CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM)hdc, lParam);

				// This method causes the button to valis when the dialog is resized using the redrawBuffered() function.
				//HDC *buf = CreateHDCBuffer(hdc, &rcClient);
				//if (buf != nullptr) {
				//	COLORREF bkgClr = RGB(0,0,0);
				//	GetThemeColorUx(hTheme, BP_PUSHBUTTON, iStateId, TMT_FILLCOLOR, &bkgClr);
				//	//bkgClr = GetPixel(*buf, 0,0);
				//	CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM)*buf, lParam);
				//	TransparentBlt( hdc, rcClient.left, rcClient.top, w, h, *buf, 0, 0, w, h, bkgClr );
				//	DeleteHDCBuffer(buf);
				//}
				//else
				//	CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM)hdc, lParam);
			}
			else {
				auto rc = rcClient;
				UINT iState = DFCS_BUTTONPUSH|DFCS_ADJUSTRECT;
				switch (nState)
				{
				case 1:
					iState |= DFCS_HOT;
					break;
				case 2:
					iState |= DFCS_PUSHED;
					break;
				case 3:
					iState |= DFCS_INACTIVE;
					break;
				default:
					break;
				}
				DrawFrameControl(hdc, &rc, DFC_BUTTON, iState);
				if (this->m_bGradientFill) {
					auto clrStart = this->m_clrStartGradient;
					auto clrEnd = this->m_clrEndGradient;

					if (clrStart == CLR_INVALID)
						clrStart = GetSysColor(COLOR_3DFACE);
					if (clrEnd == CLR_INVALID)
						clrEnd = GetSysColor(COLOR_GRADIENTACTIVECAPTION);

					XPopupMenuItem::DrawGradient( hdc, &rc, clrStart, clrEnd, this->m_bGradientVertical);
				}
			}
		}

		auto hFontOld = SelectFont(hdc, this->m_hFont);
		Auto(SelectFont(hdc, hFontOld));

		const auto oldbkg = SetBkMode(hdc, TRANSPARENT);
		Auto(SetBkMode(hdc, oldbkg));

		auto himl = this->getImageList();

		RECT rcTxt;

		rcTxt.top = BUTTON_YPAD;
		rcTxt.right = (rcClient.right - BUTTON_XPAD);
		rcTxt.bottom = (rcClient.bottom - BUTTON_YPAD);
		rcTxt.left = BUTTON_XPAD;

		if ( !this->m_tsCaption.empty() ) {
			//this->calcTextRect(hdc, this->m_tsCaption, &rcTxt, DT_WORD_ELLIPSIS | DT_SINGLELINE);
			auto t(this->m_tsCaption);
			if (this->m_bCtrlCodeText)
				t.strip();
			if (!this->m_bSelected && this->m_bShadowText)
				dcxDrawShadowText(hdc, t.to_wchr(), t.len(), &rcTxt, DT_WORD_ELLIPSIS | DT_SINGLELINE | DT_CALCRECT, this->m_aColors[nState], 0,5,5);
			else
				DrawText(hdc, t.to_chr(), t.len(), &rcTxt, DT_WORD_ELLIPSIS | DT_SINGLELINE | DT_CALCRECT);
		}

		const auto iCenter = w / 2;
		const auto iVCenter = h / 2;
		const auto iTextW = (rcTxt.right - rcTxt.left);
		const auto iTextH = (rcTxt.bottom - rcTxt.top);

		// If there is an icon
		if (himl != nullptr && this->m_bHasIcons) {
			auto iIconLeft = (iCenter - ((this->m_iIconSize + ICON_XPAD + iTextW) / 2));
			auto iIconTop = (iVCenter - (this->m_iIconSize / 2));

			if (iIconLeft < BUTTON_XPAD)
				iIconLeft = BUTTON_XPAD;

			if (iIconTop < BUTTON_YPAD)
				iIconTop = BUTTON_YPAD;

			rcTxt.left = iIconLeft + this->m_iIconSize + ICON_XPAD;

			if (nState == 3) // disabled
				ImageList_Draw(himl, nState, hdc, iIconLeft, iIconTop, ILD_TRANSPARENT | ILD_BLEND50);
			else
				ImageList_Draw(himl, nState, hdc, iIconLeft, iIconTop, ILD_TRANSPARENT);
		}
		else {
			rcTxt.left = (iCenter - (iTextW / 2));
			if ( rcTxt.left < BUTTON_XPAD )
				rcTxt.left = BUTTON_XPAD;
		}

		if ( iTextW > 0 ) {
			rcTxt.top = iVCenter - iTextH / 2;

			if ( rcTxt.top < BUTTON_YPAD )
				rcTxt.top = BUTTON_YPAD;

			rcTxt.right = (rcClient.right - BUTTON_XPAD);
			rcTxt.bottom = (rcClient.bottom - BUTTON_YPAD);

			const auto oldClr = SetTextColor(hdc, this->m_aColors[nState]);
			Auto(SetTextColor(hdc, oldClr));

			//this->ctrlDrawText(hdc,this->m_tsCaption, &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE);
			if (!this->m_bCtrlCodeText) {
				if (!this->m_bSelected && this->m_bShadowText)
					dcxDrawShadowText(hdc,this->m_tsCaption.to_wchr(), this->m_tsCaption.len(),&rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE, this->m_aColors[nState], 0, 5, 5);
				else
					DrawText( hdc, this->m_tsCaption.to_chr(), this->m_tsCaption.len( ), &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE );
			}
			else
				mIRC_DrawText(hdc, this->m_tsCaption, &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE, (!this->m_bSelected && this->m_bShadowText));
		}
	}

	if (hTheme != nullptr)
		Dcx::UXModule.dcxCloseThemeData(hTheme);
}
