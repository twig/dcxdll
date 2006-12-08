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
	if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
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

		if (SUCCEEDED(hr)) {
			hr = this->m_pGraph->QueryInterface(IID_IMediaControl, (void **)&this->m_pControl);
			hr = this->m_pGraph->QueryInterface(IID_IMediaEventEx, (void **)&this->m_pEvent);
			//if (SUCCEEDED(hr)) {
			//	this->m_pEvent->SetNotifyWindow((OAHWND)this->m_Hwnd,WM_GRAPHNOTIFY,0);
			//}
			hr = DcxDirectshow::InitWindowlessVMR(this->m_Hwnd,this->m_pGraph,&this->m_pWc);
		}
		if (this->m_pControl != NULL && this->m_pEvent != NULL && this->m_pGraph != NULL && this->m_pWc != NULL) {
			hr = this->m_pGraph->RenderFile(filename.to_wchr(),NULL);
			if (SUCCEEDED(hr)) {
				// Find the native video size.
				long lWidth, lHeight; 
				hr = this->m_pWc->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL); 
				if (SUCCEEDED(hr))
				{
					hr = this->SetVideoPos();
					if (!SUCCEEDED(hr))
						DCXError("/xdid -a","Unable to set Video Position");
				}
				else
					DCXError("/xdid -a","Unable to get Native Video Size");
			}
			else
				DCXError("/xdid -a","Unable to render file");
		}
		else {
			this->ReleaseAll();
			DCXError("/xdid -a","Unable to Setup Filter Graph");
		}
	}
  // xdid -c [NAME] [ID] [SWITCH] [COMMAND]
  else if ( flags.switch_flags[2] && numtok > 3 ) {
		if (this->m_pControl != NULL && this->m_pEvent != NULL && this->m_pGraph != NULL && this->m_pWc != NULL) {
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
				this->m_pControl->Stop();
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
				else  // There is no video, so paint the whole client area.
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
				this->SetVideoPos();
			}
			break;

		case WM_DISPLAYCHANGE:
			{
				if (this->m_pWc != NULL)
					this->m_pWc->DisplayModeChanged();
			}
			break;

		//case WM_GRAPHNOTIFY:
		//	{
		//		bParsed = TRUE;
		//		if (this->m_pEvent == NULL)
		//			break;
		//		// Get all the events
		//		long evCode;
		//		LONG_PTR param1, param2;
		//		while (SUCCEEDED(this->m_pEvent->GetEvent(&evCode, &param1, &param2, 0)))
		//		{
		//			this->m_pEvent->FreeEventParams(evCode, param1, param2);
		//			switch (evCode)
		//			{
		//			case EC_COMPLETE:
		//				this->callAliasEx(NULL,"%s,%d,%s","dshow",this->getUserID(),"completed");
		//				break;
		//			case EC_PAUSED:
		//				this->callAliasEx(NULL,"%s,%d,%s","dshow",this->getUserID(),"paused");
		//				break;
		//			case EC_USERABORT:
		//			case EC_ERRORABORT:
		//				this->callAliasEx(NULL,"%s,%d,%s","dshow",this->getUserID(),"aborted");
		//				break;
		//			}
		//		} 
		//	}
		//	break;

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
    IVMRWindowlessControl** ppWc   // Receives a pointer to the VMR.
    ) 
{ 
    if (!pGraph || !ppWc)
    {
        return E_POINTER;
    }
    IBaseFilter* pVmr = NULL; 
    IVMRWindowlessControl* pWc = NULL; 
    // Create the VMR. 
    HRESULT hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL, 
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
    IVMRFilterConfig* pConfig; 
    hr = pVmr->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig); 
    if (SUCCEEDED(hr)) 
    { 
        hr = pConfig->SetRenderingMode(VMRMode_Windowless); 
        pConfig->Release(); 
    }
    if (SUCCEEDED(hr))
    {
        // Set the window. 
        hr = pVmr->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWc);
        if( SUCCEEDED(hr)) 
        { 
            hr = pWc->SetVideoClippingWindow(hwndApp);
						hr = pWc->SetAspectRatioMode(VMR_ARMODE_LETTER_BOX); // caused video to maintain aspect ration
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
	if (this->m_pWc != NULL)
		this->m_pWc->Release();
	if (this->m_pGraph != NULL)
		this->m_pGraph->Release();
	this->m_pControl = NULL;
	this->m_pEvent = NULL;
	this->m_pGraph = NULL;
	this->m_pWc = NULL;
}