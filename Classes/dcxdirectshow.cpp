/*!
 * \file dcxdirectshow.cpp
 * \brief blah
 *
 * blah
 *
 * \author Ook
 * \version 1.0
 *
 * \b Revisions
 *	1.0
 *
 * © ScriptsDB.org - 2006
 */

#include "dcxdirectshow.h"

#ifdef USE_DXSDK
#include "dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxDirectshow::DcxDirectshow( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles )
: DcxControl( ID, p_Dialog )
, m_pGraph(NULL)
, m_pControl(NULL)
, m_pEvent(NULL)
, m_pWc(NULL)
, m_pSeek(NULL)
, m_bKeepRatio(false)
, m_bLoop(false)
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(
    ExStyles | WS_EX_CLIENTEDGE,
    "STATIC",
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

	if ( bNoTheme )
		dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDirectshow::~DcxDirectshow( ) {

	this->ReleaseAll();
  this->unregistreDefaultWindowProc( );
}

void DcxDirectshow::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  *Styles |= SS_NOTIFY;

  unsigned int i = 1, numtok = styles.numtok( " " );

  while ( i <= numtok ) {

		if ( styles.gettok( i , " " ) == "alpha" )
			this->m_bAlphaBlend = true;
		else if (( styles.gettok( i , " " ) == "shadow" ))
			this->m_bShadowText = true;
		else if (( styles.gettok( i , " " ) == "fixratio" ))
			this->m_bKeepRatio = true;

    i++;
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

void DcxDirectshow::parseInfoRequest( TString & input, char * szReturnValue ) {
  //int numtok = input.numtok( " " );

  // [NAME] [ID] [PROP]
	if ( input.gettok( 3, " " ) == "size" && this->m_pWc != NULL) {
		long lWidth, lHeight, lARWidth, lARHeight;
		HRESULT hr = this->m_pWc->GetNativeVideoSize(&lWidth, &lHeight, &lARWidth, &lARHeight);
		if (SUCCEEDED(hr)) {
			// width height arwidth arheight
			wsprintf(szReturnValue,"%d %d %d %d", lWidth, lHeight, lARWidth, lARHeight);
			return;
		}
		else
			dcxInfoError("directshow","size",this->m_pParentDialog->getName().to_chr(),this->getUserID(),"Unable to get Native Video Size");
  }
  // [NAME] [ID] [PROP]
	else if ( input.gettok( 3, " " ) == "author" && this->m_pGraph != NULL) {
		this->getProperty(szReturnValue, PROP_AUTHOR);
		return;
  }
  // [NAME] [ID] [PROP]
	else if ( input.gettok( 3, " " ) == "title" && this->m_pGraph != NULL) {
		this->getProperty(szReturnValue, PROP_TITLE);
		return;
  }
	else if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
		return;

	szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDirectshow::parseCommandRequest(TString &input) {
	XSwitchFlags flags;

	ZeroMemory((void*) &flags, sizeof(XSwitchFlags));
	this->parseSwitchFlags(input.gettok(3, " "), &flags);
  int numtok = input.numtok( " " );

  // xdid -a [NAME] [ID] [SWITCH] [+FLAGS] [FILE]
  if ( flags.switch_flags[0] && numtok > 3 ) {
		TString flag(input.gettok(4));
		TString filename(input.gettok(5,-1));
		flag.trim();
		filename.trim();
		this->ReleaseAll();
		// Create the Filter Graph Manager and query for interfaces.
		HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&this->m_pGraph);

		if (SUCCEEDED(hr))
			hr = this->m_pGraph->QueryInterface(IID_IMediaControl, (void **)&this->m_pControl);
		if (SUCCEEDED(hr))
			hr = this->m_pGraph->QueryInterface(IID_IMediaEventEx, (void **)&this->m_pEvent);
		if (SUCCEEDED(hr))
			hr = this->m_pGraph->QueryInterface(IID_IMediaSeeking, (void **)&this->m_pSeek);
		if (SUCCEEDED(hr))
			hr = this->m_pEvent->SetNotifyWindow((OAHWND)this->m_Hwnd,WM_GRAPHNOTIFY,0);
		if (SUCCEEDED(hr))
			hr = DcxDirectshow::InitWindowlessVMR(this->m_Hwnd,this->m_pGraph,&this->m_pWc);
		if (SUCCEEDED(hr)) {
			if (this->m_bKeepRatio)
				hr = this->m_pWc->SetAspectRatioMode(VMR9ARMode_LetterBox); // caused video to maintain aspect ratio
			else
				hr = this->m_pWc->SetAspectRatioMode(VMR9ARMode_None);
		}
		if (SUCCEEDED(hr)) {
			hr = this->m_pGraph->RenderFile(filename.to_wchr(),NULL);
			if (SUCCEEDED(hr)) {
				hr = this->SetVideoPos();
				if (this->m_bAlphaBlend)
					hr = this->setAlpha(0.5);
				if (SUCCEEDED(hr)) {
					if (flag.find('l',0))
						this->m_bLoop = true;
					else
						this->m_bLoop = false;
					if (flag.find('p',0))
						this->m_pControl->Run();
				}
				else
					DCXError("/xdid -a","Unable to set Video Position");
			}
			else
				DCXError("/xdid -a","Unable to render file");
		}
		if (!SUCCEEDED(hr)) { // if anything failed, release all & show error.
			this->ReleaseAll();
			DCXError("/xdid -a","Unable to Setup Filter Graph");
		}
	}
  // xdid -c [NAME] [ID] [SWITCH] [COMMAND]
  else if ( flags.switch_flags[2] && numtok > 3 ) {
		if (this->m_pControl != NULL) {
			static const TString cmdlist("play pause stop close");
			int nType = cmdlist.findtok(input.gettok(4).to_chr(),1);
			switch (nType)
			{
			case 1: // play
				this->m_pControl->Run();
				break;
			case 2: // pause
				this->m_pControl->Pause();
				break;
			case 3: // stop
				{
					this->m_pControl->Stop(); // stop play
					LONGLONG rtNow = 0; // seek to start.
					this->m_pSeek->SetPositions(&rtNow, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
					this->m_pControl->StopWhenReady(); // causes new image to be rendered.
				}
				break;
			case 4: //close
				{
					this->m_pControl->Stop();
					this->ReleaseAll();
				}
				break;
			case 0: // error
			default:
				DCXError("/xdid -c","Invalid Command");
				break;
			}
		}
		else
			DCXError("/xdid -c", "No File Loaded");
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxDirectshow::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxDirectshow::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
  switch( uMsg ) {

    case WM_HELP:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_HELP)
	        this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
      }
      break;

    case WM_ERASEBKGND: 
      {
				bParsed = TRUE;
				return TRUE;
      }
      break;

		case WM_PAINT:
			{
				bParsed = TRUE;
				PAINTSTRUCT ps;
				HDC         hdc;
				hdc = BeginPaint(this->m_Hwnd, &ps);
				if (this->m_pWc != NULL)
				{
					// Request the VMR to paint the video.
					HRESULT hr = this->m_pWc->RepaintVideo(this->m_Hwnd, hdc);
				}
				else if (!this->isExStyle(WS_EX_TRANSPARENT))  // There is no video, so paint the whole client area.
				{
					RECT rcClient;
					GetClientRect(this->m_Hwnd, &rcClient);
					FillRect(hdc, &rcClient, (HBRUSH)(COLOR_BTNFACE + 1));
				}
				EndPaint(this->m_Hwnd, &ps); 
			}
			break;

		case WM_SIZE:
			{
				if (this->m_pWc != NULL)
					this->SetVideoPos();
			}
			break;

		case WM_DISPLAYCHANGE:
			{
				if (this->m_pWc != NULL)
					this->m_pWc->DisplayModeChanged();
			}
			break;

		case WM_GRAPHNOTIFY:
			{
				bParsed = TRUE;
				if (this->m_pEvent == NULL)
					break;
				// Get all the events
				long evCode;
				LONG_PTR param1, param2;
				while (SUCCEEDED(this->m_pEvent->GetEvent(&evCode, &param1, &param2, 0)))
				{
					this->m_pEvent->FreeEventParams(evCode, param1, param2);
					switch (evCode)
					{
					case EC_COMPLETE:
						{
							LONGLONG rtNow = 0; // seek to start.
							this->m_pSeek->SetPositions(&rtNow, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
							if (!this->m_bLoop) {
								this->m_pControl->StopWhenReady();
								this->callAliasEx(NULL,"%s,%d,%s","dshow",this->getUserID(),"completed");
							}
						}
						break;
					//case EC_PAUSED: // oddly this is sent when we play the file too.
					//	this->callAliasEx(NULL,"%s,%d,%s","dshow",this->getUserID(),"paused");
					//	break;
					//case EC_USERABORT:
					//case EC_ERRORABORT:
					//	this->callAliasEx(NULL,"%s,%d,%s","dshow",this->getUserID(),"aborted");
					//	break;
					}
				} 
			}
			break;

		case WM_SETCURSOR:
			{
				if ( LOWORD( lParam ) == HTCLIENT && (HWND) wParam == this->m_Hwnd ) {
					if ( this->m_hCursor != NULL )
						SetCursor( this->m_hCursor );
					else
						SetCursor( LoadCursor( NULL, IDC_ARROW ) );
					bParsed = TRUE;
					return TRUE;
				}
			}
			break;

    case WM_MOUSEMOVE:
      {
        this->m_pParentDialog->setMouseControl( this->getUserID( ) );
      }
      break;

    case WM_LBUTTONDOWN:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->callAliasEx( NULL, "%s,%d", "lbdown", this->getUserID( ) );
      }
      break;

    case WM_LBUTTONUP:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
	        this->callAliasEx( NULL, "%s,%d", "lbup", this->getUserID( ) );
					this->callAliasEx( NULL, "%s,%d", "sclick", this->getUserID( ) );
				}
      }
      break;

    case WM_LBUTTONDBLCLK:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
					this->callAliasEx( NULL, "%s,%d", "dclick", this->getUserID( ) );
					this->callAliasEx( NULL, "%s,%d", "lbdblclk", this->getUserID( ) );
				}
      }
      break;

    case WM_SETFOCUS:
      {
        this->m_pParentDialog->setFocusControl( this->getUserID( ) );
      }
      break;

    case WM_DESTROY:
      {
        delete this;
        bParsed = TRUE;
      }
      break;

    default:
      break;
  }

  return lRes;
}

HRESULT DcxDirectshow::InitWindowlessVMR(
    HWND hwndApp,                  // Window to hold the video. 
    IGraphBuilder* pGraph,         // Pointer to the Filter Graph Manager. 
    IVMRWindowlessControl9** ppWc   // Receives a pointer to the VMR.
    ) 
{ 
    if (!pGraph || !ppWc)
        return E_POINTER;

    IBaseFilter* pVmr = NULL; 
    IVMRWindowlessControl9* pWc = NULL; 
    // Create the VMR.
    HRESULT hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, 
        CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVmr); 
    if (FAILED(hr))
    {
        return hr;
    }
    
    // Add the VMR to the filter graph.
    hr = pGraph->AddFilter(pVmr, L"Video Mixing Renderer"); 
    if (FAILED(hr)) 
    {
        pVmr->Release();
        return hr;
    }
    // Set the rendering mode.
    IVMRFilterConfig9* pConfig; 
    hr = pVmr->QueryInterface(IID_IVMRFilterConfig9, (void**)&pConfig);
    if (SUCCEEDED(hr))
    {
        hr = pConfig->SetRenderingMode(VMR9Mode_Windowless);
				if (GetWindowLong(hwndApp,GWL_EXSTYLE) & WS_EX_TRANSPARENT)
					hr = pConfig->SetRenderingPrefs(RenderPrefs9_DoNotRenderBorder);
        pConfig->Release();
    }
    if (SUCCEEDED(hr))
    {
        // Set the window. 
        hr = pVmr->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pWc);
        if( SUCCEEDED(hr)) 
        { 
            hr = pWc->SetVideoClippingWindow(hwndApp);
						//if (SUCCEEDED(hr)) {
						//	IVMRMixerControl9 *pMixer;
						//	hr = pVmr->QueryInterface(IID_IVMRMixerControl9, (void**)&pMixer);
						//	if (SUCCEEDED(hr)) {
						//		pMixer->SetAlpha(0,0.5);
						//		pMixer->Release();
						//	}
						//}
            if (SUCCEEDED(hr))
            {
                *ppWc = pWc; // Return this as an AddRef'd pointer. 
            }
            else
            {
                // An error occurred, so release the interface.
                pWc->Release();
            }
        } 
    } 
    pVmr->Release(); 
    return hr; 
} 

HRESULT DcxDirectshow::SetVideoPos(void)
{
	if (this->m_pWc == NULL)
		return E_POINTER;

	long lWidth, lHeight;
	HRESULT hr = this->m_pWc->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL);
	if (SUCCEEDED(hr))
	{
		RECT rcSrc, rcDest;
		// Set the source rectangle.
		SetRect(&rcSrc, 0, 0, lWidth, lHeight);

		// Get the window client area.
		GetClientRect(this->m_Hwnd, &rcDest);
		// Set the destination rectangle.
		SetRect(&rcDest, 0, 0, rcDest.right, rcDest.bottom);

		// Set the video position.
		hr = this->m_pWc->SetVideoPosition(&rcSrc, &rcDest);
	}
	return hr;
}

void DcxDirectshow::ReleaseAll(void)
{
	if (this->m_pControl != NULL)
		this->m_pControl->Release();
	if (this->m_pEvent != NULL) {
		this->m_pEvent->SetNotifyWindow(NULL,0,0);
		this->m_pEvent->Release();
	}
	if (this->m_pSeek != NULL)
		this->m_pSeek->Release();
	if (this->m_pWc != NULL)
		this->m_pWc->Release();
	if (this->m_pGraph != NULL)
		this->m_pGraph->Release();
	this->m_pControl = NULL;
	this->m_pEvent = NULL;
	this->m_pGraph = NULL;
	this->m_pWc = NULL;
	this->m_pSeek = NULL;
}
// getProperty() is non-functional atm. Where do i get this interface from? or a similar one.
HRESULT DcxDirectshow::getProperty(char *prop, int type)
{
	IAMMediaContent *iam;
	HRESULT hr = this->m_pGraph->QueryInterface(IID_IAMMediaContent,(void **)&iam);
	if (SUCCEEDED(hr)) {
		BSTR com_prop;
		switch (type)
		{
		case PROP_AUTHOR:
			hr = iam->get_AuthorName(&com_prop);
			break;
		case PROP_TITLE:
			hr = iam->get_Title(&com_prop);
			break;
		case PROP_RATING:
			hr = iam->get_Rating(&com_prop);
			break;
		case PROP_DESCRIPTION:
			hr = iam->get_Description(&com_prop);
			break;
		}
		if (SUCCEEDED(hr)) {
			wsprintf(prop,"%lS", com_prop);
			SysFreeString(com_prop);
		}
		else
			lstrcpy(prop,"Not Supported");
		iam->Release();
	}
	else
		lstrcpy(prop,"failed");
	return hr;
}

HRESULT DcxDirectshow::setAlpha(float alpha)
{
	if (alpha < 0)
		alpha = 255;

	IBaseFilter* pVmr = NULL; 

	HRESULT hr = this->m_pGraph->FindFilterByName(L"Video Mixing Renderer",&pVmr);

	if (FAILED(hr))
			return hr;

	// this works BUT only gives u alpha over other streams in the mixer, not the dialog/controls bg.
	IVMRMixerControl9 *pMixer = NULL;
	hr = pVmr->QueryInterface(IID_IVMRMixerControl9, (void**)&pMixer);
	if (SUCCEEDED(hr)) {
	//	hr = pMixer->SetAlpha(0,alpha);
		// Get the current mixing preferences.
		DWORD dwPrefs;
		pMixer->GetMixingPrefs(&dwPrefs);  

		dwPrefs |= MixerPref9_NonSquareMixing;

		// Set the new flags.
		pMixer->SetMixingPrefs(dwPrefs);
		pMixer->Release();
	}
	IVMRMixerBitmap9 *pBm = NULL;
	hr = pVmr->QueryInterface(IID_IVMRMixerBitmap9, (void**)&pBm);
	if (SUCCEEDED(hr)) {
		HDC hdc = GetDC(this->m_Hwnd);
		if (hdc != NULL) { // make duplicate hdc;
			HDC hdcbkg = CreateCompatibleDC( hdc );
			if (hdcbkg != NULL) {
				RECT rcClient, rcWin;
				GetClientRect(this->m_Hwnd, &rcClient);
				GetWindowRect(this->m_Hwnd, &rcWin);
				int w = (rcWin.right - rcWin.left), h = (rcWin.bottom - rcWin.top);
				HBITMAP memBM = CreateCompatibleBitmap ( hdc, w, h );
				if (memBM != NULL) {
					// associate bitmap with HDC
					HBITMAP oldBM = (HBITMAP)SelectObject ( hdcbkg, memBM );

					this->DrawParentsBackground(hdcbkg);

					long cx, cy;
					hr = this->m_pWc->GetNativeVideoSize(&cx, &cy, NULL, NULL);
					if (SUCCEEDED(hr)) {
						VMR9AlphaBitmap bmpInfo;
						ZeroMemory(&bmpInfo, sizeof(bmpInfo) );
						bmpInfo.dwFlags = VMR9AlphaBitmap_hDC;
						//bmpInfo.dwFilterMode = MixerPref9_AnisotropicFiltering;
						bmpInfo.hdc = hdcbkg;
						// Set the transparency value (1.0 is opaque, 0.0 is transparent).
						bmpInfo.fAlpha = 1.0;
						// Show the entire bitmap in the top-left corner of the video image.
						SetRect(&bmpInfo.rSrc, 0, 0, w, h);
						//CopyRect(&bmpInfo.rSrc, &rcClient);
						bmpInfo.rDest.left = 0.f;
						bmpInfo.rDest.top = 0.f;
						bmpInfo.rDest.right = 1.0; //(float)w / cx;
						bmpInfo.rDest.bottom = 1.0; //(float)h / cy;
						hr = pBm->SetAlphaBitmap(&bmpInfo);
						ZeroMemory(&bmpInfo, sizeof(bmpInfo) );
						bmpInfo.dwFlags = VMR9AlphaBitmap_SrcRect;
						bmpInfo.hdc = NULL;
						// Set the transparency value (1.0 is opaque, 0.0 is transparent).
						bmpInfo.fAlpha = alpha;
						//POINT pt;
						//pt.x = rcWin.left;
						//pt.y = rcWin.top;
						//ScreenToClient(GetParent(this->m_Hwnd),&pt);
						//CopyRect(&bmpInfo.rSrc, &rcWin);
						//SetRect(&bmpInfo.rSrc, pt.x, pt.y, pt.x + w, pt.y + h);
						CopyRect(&bmpInfo.rSrc, &rcClient);
						bmpInfo.rDest.left = 0.f;
						bmpInfo.rDest.top = 0.f;
						bmpInfo.rDest.right = 1.0; //(float)(rcClient.right - rcClient.left) / cx;
						bmpInfo.rDest.bottom = 1.0; //(float)(rcClient.bottom - rcClient.top) / cy;
						hr = pBm->UpdateAlphaBitmapParameters(&bmpInfo);
					}
					DeleteObject(SelectObject(hdcbkg,oldBM));
				}
				else
					hr = E_FAIL;
				DeleteDC( hdcbkg );
			}
			else
				hr = E_FAIL;
			ReleaseDC(this->m_Hwnd, hdc);
		}
		else
			hr = E_FAIL;
		pBm->Release();
	}
	pVmr->Release(); 
	return hr; 
}

#endif // USE_DXSDK
