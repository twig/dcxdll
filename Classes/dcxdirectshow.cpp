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

#ifdef DCX_USE_DXSDK
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
    WS_CHILD | WS_CLIPSIBLINGS | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

	if (!IsWindow(this->m_Hwnd))
		throw "Unable To Create Window";

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

	unsigned int i = 1, numtok = styles.numtok( );

	while ( i <= numtok ) {

		if (( styles.gettok( i ) == "fixratio" ))
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
  //int numtok = input.numtok( );

	TString prop(input.gettok( 3 ));

	if (this->m_pGraph == NULL) {
		// [NAME] [ID] [PROP]
		if ( prop == "isloaded") {
			lstrcpy(szReturnValue,"$false");
			return;
		}
		// [NAME] [ID] [PROP]
		else if ( prop == "state") {
			lstrcpy(szReturnValue,"D_OK nofile");
			return;
		}
		else if (this->parseGlobalInfoRequest( input, szReturnValue ))
			return;
		else
			this->showError(prop.to_chr(),NULL,"No File Loaded");
	}
	// [NAME] [ID] [PROP]
	else if ( prop == "isloaded") {
		lstrcpy(szReturnValue,"$true");
		return;
	}
	// [NAME] [ID] [PROP]
	else if ( prop == "fname") {
		lstrcpyn(szReturnValue,this->m_tsFilename.to_chr(), 900);
		return;
	}
  // [NAME] [ID] [PROP]
	else if ( prop == "size") {
		long lWidth, lHeight, lARWidth, lARHeight;
		HRESULT hr = this->m_pWc->GetNativeVideoSize(&lWidth, &lHeight, &lARWidth, &lARHeight);
		if (SUCCEEDED(hr)) {
			// width height arwidth arheight
			wsprintf(szReturnValue,"%d %d %d %d", lWidth, lHeight, lARWidth, lARHeight);
			return;
		}
		else {
			this->showError(prop.to_chr(),NULL,"Unable to get Native Video Size");
			DX_ERR(prop.to_chr(),NULL,hr);
		}
  }
  // [NAME] [ID] [PROP]
	else if ( prop == "author") {
		this->getProperty(szReturnValue, PROP_AUTHOR);
		return;
  }
  // [NAME] [ID] [PROP]
	else if ( prop == "title") {
		this->getProperty(szReturnValue, PROP_TITLE);
		return;
  }
  // [NAME] [ID] [PROP]
	else if ( prop == "video") {
		VMR9ProcAmpControl amc;
		HRESULT hr = this->getVideo(&amc);
		if (SUCCEEDED(hr)) {
			TString vflags('+');
			if (amc.dwFlags & ProcAmpControl9_Brightness)
				vflags += 'b';
			if (amc.dwFlags & ProcAmpControl9_Contrast)
				vflags += 'c';
			if (amc.dwFlags & ProcAmpControl9_Hue)
				vflags += 'h';
			if (amc.dwFlags & ProcAmpControl9_Saturation)
				vflags += 's';
			// NB: wsprintf() doesn't support %f
			sprintf(szReturnValue,"%s %f %f %f %f", vflags.to_chr(), amc.Brightness, amc.Contrast, amc.Hue, amc.Saturation);
			return;
		}
		else {
			this->showError(prop.to_chr(),NULL,"Unable to get Video Information");
			DX_ERR(prop.to_chr(),NULL, hr);
		}
  }
  // [NAME] [ID] [PROP]
	else if ( prop == "brange") {
		VMR9ProcAmpControlRange acr;
		HRESULT hr = this->getVideoRange(ProcAmpControl9_Brightness, &acr);
		if (SUCCEEDED(hr)) {
			// NB: wsprintf() doesn't support %f
			sprintf(szReturnValue,"%f %f %f %f", acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);
			return;
		}
		else {
			this->showError(prop.to_chr(),NULL,"Unable to get Video Information");
			DX_ERR(prop.to_chr(),NULL, hr);
		}
  }
  // [NAME] [ID] [PROP]
	else if ( prop == "crange") {
		VMR9ProcAmpControlRange acr;
		HRESULT hr = this->getVideoRange(ProcAmpControl9_Contrast, &acr);
		if (SUCCEEDED(hr)) {
			// NB: wsprintf() doesn't support %f
			sprintf(szReturnValue,"%f %f %f %f", acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);
			return;
		}
		else {
			this->showError(prop.to_chr(),NULL,"Unable to get Video Information");
			DX_ERR(prop.to_chr(),NULL, hr);
		}
  }
  // [NAME] [ID] [PROP]
	else if ( prop == "hrange") {
		VMR9ProcAmpControlRange acr;
		HRESULT hr = this->getVideoRange(ProcAmpControl9_Hue, &acr);
		if (SUCCEEDED(hr)) {
			// NB: wsprintf() doesn't support %f
			sprintf(szReturnValue,"%f %f %f %f", acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);
			return;
		}
		else {
			this->showError(prop.to_chr(),NULL,"Unable to get Video Information");
			DX_ERR(prop.to_chr(),NULL, hr);
		}
  }
  // [NAME] [ID] [PROP]
	else if ( prop == "srange") {
		VMR9ProcAmpControlRange acr;
		HRESULT hr = this->getVideoRange(ProcAmpControl9_Saturation, &acr);
		if (SUCCEEDED(hr)) {
			// NB: wsprintf() doesn't support %f
			sprintf(szReturnValue,"%f %f %f %f", acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);
			return;
		}
		else {
			this->showError(prop.to_chr(),NULL,"Unable to get Video Information");
			DX_ERR(prop.to_chr(),NULL, hr);
		}
  }
  // [NAME] [ID] [PROP]
	else if ( prop == "currentpos") {
		wsprintf(szReturnValue,"D_OK %I64d", this->getPosition());
		return;
  }
  // [NAME] [ID] [PROP]
	else if ( prop == "duration") {
		if (this->CheckSeekCapabilities(AM_SEEKING_CanGetDuration) & AM_SEEKING_CanGetDuration)
			wsprintf(szReturnValue,"D_OK %I64d", this->getDuration());
		else
			lstrcpy(szReturnValue,"D_ERROR Method Not Supported");
		return;
  }
  // [NAME] [ID] [PROP]
	else if ( prop == "volume") {
		sprintf(szReturnValue,"D_OK %.2f", this->getVolume());
		return;
  }
	// [NAME] [ID] [PROP]
	else if (prop == "state") {
		/*
		sprintf(szReturnValue, "D_OK %s", "nofile");  // done
		sprintf(szReturnValue, "D_OK %s", "stopped"); // done
		sprintf(szReturnValue, "D_OK %s", "paused");  // done
		sprintf(szReturnValue, "D_OK %s", "playing"); // done
		sprintf(szReturnValue, "D_OK %s", "rewind");
		sprintf(szReturnValue, "D_OK %s", "fastforward");
		and anything else you can think of i guess
		*/

		OAFilterState pfs = 0;
		PTCHAR szState = NULL;
		HRESULT hr = this->m_pControl->GetState(1000, &pfs);

		if (SUCCEEDED(hr)) {
			switch (pfs) {
				case State_Stopped:
					szState = "stopped";
					break;
				case State_Paused:
					szState = "paused";
					break;
				case State_Running:
					szState = "playing";
					break;
				default:
					szState = "unknown";
					break;
			}
			sprintf(szReturnValue, "D_OK %s", szState);
		}
		else {
			this->showError(prop.to_chr(),NULL,"Unable to get State Information");
			DX_ERR(prop.to_chr(),NULL, hr);
			lstrcpy(szReturnValue,"D_ERROR Unable To Get State");
		}
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
	XSwitchFlags flags(input.gettok(3));
	int numtok = input.numtok( );

	// xdid -a [NAME] [ID] [SWITCH] [+FLAGS] [FILE]
	if ( flags['a'] && numtok > 4 ) {
		TString flag(input.gettok(4));
		TString filename(input.gettok(5,-1));
		flag.trim();
		filename.trim();
		this->ReleaseAll();

		if (!mIRCLink.m_bDX9Installed)
			DXSetup(NULL, 0);

		if (!mIRCLink.m_bDX9Installed) {
			this->showError(NULL, "-a", "Needs DirectX 9+");
			return;
		}
		if (!IsFile(filename)) {
			this->showErrorEx(NULL,"-a", "Unable to Access File: %s", filename.to_chr());
			return;
		}
		// Create the Filter Graph Manager and query for interfaces.
		HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&this->m_pGraph);
		bool inErr = false;

		if (SUCCEEDED(hr))
			hr = this->m_pGraph->QueryInterface(IID_IMediaControl, (void **)&this->m_pControl);
		else {
			this->showError(NULL,"-a", "Unable to Create FilterGraph");
			DX_ERR(NULL,"-a", hr);
			inErr = true;
		}
		if (SUCCEEDED(hr))
			hr = this->m_pGraph->QueryInterface(IID_IMediaEventEx, (void **)&this->m_pEvent);
		else if (!inErr) {
			this->showError(NULL,"-a", "Unable to Get IMediaControl");
			DX_ERR(NULL,"-a", hr);
			inErr = true;
		}
		if (SUCCEEDED(hr))
			hr = this->m_pGraph->QueryInterface(IID_IMediaSeeking, (void **)&this->m_pSeek);
		else if (!inErr) {
			this->showError(NULL,"-a", "Unable to Get IMediaEventEx");
			DX_ERR(NULL,"-a", hr);
			inErr = true;
		}
		if (SUCCEEDED(hr))
			hr = this->m_pEvent->SetNotifyWindow((OAHWND)this->m_Hwnd,WM_GRAPHNOTIFY,0);
		else if (!inErr) {
			this->showError(NULL,"-a", "Unable to Get IMediaSeeking");
			DX_ERR(NULL,"-a", hr);
			inErr = true;
		}
		if (SUCCEEDED(hr)) {
			if (!flag.find('a',0))
				hr = DcxDirectshow::InitWindowlessVMR(this->m_Hwnd,this->m_pGraph,&this->m_pWc);
		}
		else if (!inErr) {
			this->showError(NULL,"-a", "Unable to Set Window Notify");
			DX_ERR(NULL,"-a", hr);
			inErr = true;
		}
		if (SUCCEEDED(hr)) {
			if (this->m_pWc != NULL) {
				if (this->m_bKeepRatio)
					hr = this->m_pWc->SetAspectRatioMode(VMR9ARMode_LetterBox); // caused video to maintain aspect ratio
				else
					hr = this->m_pWc->SetAspectRatioMode(VMR9ARMode_None);
			}
		}
		else if (!inErr) {
			this->showError(NULL,"-a", "Unable to Create VMR9");
			DX_ERR(NULL,"-a", hr);
			inErr = true;
		}
		if (SUCCEEDED(hr)) {
			hr = this->m_pGraph->RenderFile(filename.to_wchr(),NULL);
			if (SUCCEEDED(hr)) {
				if (this->m_pWc != NULL) {
					hr = this->SetVideoPos();
					if (this->m_bAlphaBlend)
						this->setAlpha(0.5);
				}
				else { // if VMR == NULL then disable video.
					IVideoWindow *p_Video;
					HRESULT hr2 = this->m_pGraph->QueryInterface(IID_IVideoWindow, (void **)&p_Video);
					if (SUCCEEDED(hr2)) {
						p_Video->put_Visible(OAFALSE);
						p_Video->put_AutoShow(OAFALSE);
						p_Video->put_Owner((OAHWND)this->m_Hwnd);
						p_Video->put_MessageDrain((OAHWND)this->m_Hwnd);
						DWORD styles = 0;
						p_Video->get_WindowStyle((long *)&styles);
						styles &= ~(WS_OVERLAPPEDWINDOW|WS_POPUPWINDOW|WS_DLGFRAME);
						styles |= WS_CHILD;
						p_Video->put_WindowStyle(styles);
						p_Video->put_Left(0);
						p_Video->put_Top(0);
						p_Video->Release();
					}
				}
				if (SUCCEEDED(hr)) {
					if (flag.find('l',0))
						this->m_bLoop = true;
					else
						this->m_bLoop = false;
					if (flag.find('p',0))
						this->m_pControl->Run();
				}
				else {
					this->showError(NULL,"-a", "Unable to set Video Position");
					DX_ERR(NULL,"-a", hr);
				}
			}
			else {
				this->showError(NULL,"-a", "Unable to render file (No codec for file format?)");
				DX_ERR(NULL,"-a", hr);
			}
		}
		else if (!inErr) {
			this->showError(NULL,"-a", "Unable to Set Aspect");
			DX_ERR(NULL,"-a", hr);
			//DCXError("/xdid -a","Unable to Set Aspect");
			inErr = true;
		}
		if (!SUCCEEDED(hr)) { // if anything failed, release all & show error.
			this->ReleaseAll();
			this->showError(NULL,"-a", "Unable to Setup Filter Graph");
			DX_ERR(NULL,"-a", hr);
			//DCXError("/xdid -a","Unable to Setup Filter Graph");
		}
		else
			this->m_tsFilename = filename;

		InvalidateRect(this->m_Hwnd, NULL, TRUE);
	}
	// xdid -c [NAME] [ID] [SWITCH] [COMMAND]
	else if ( flags['c'] && numtok > 3 ) {
		if (this->m_pControl != NULL) {
			static const TString cmdlist("play pause stop close seek");
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
					this->setPosition(0);
					this->m_pControl->StopWhenReady(); // causes new image to be rendered.
				}
				break;
			case 4: //close
				{
					this->m_pControl->Stop();
					this->ReleaseAll();
				}
				break;
			case 5: // seek
				{
					this->m_pControl->Pause(); // pause play
					this->setPosition(input.gettok(5).to_num());
					this->m_pControl->StopWhenReady(); // causes new image to be rendered.
				}
				break;
			case 0: // error
			default:
				this->showError(NULL,"-c", "Invalid Command");
				//DCXError("/xdid -c","Invalid Command");
				break;
			}
		}
		else
			this->showError(NULL,"-c", "No File Loaded");
			//DCXError("/xdid -c", "No File Loaded");
	}
	// xdid -v [NAME] [ID] [SWITCH] [+FLAGS] [BRIGHTNESS] [CONTRAST] [HUE] [SATURATION]
	else if ( flags['v'] && numtok > 7 ) {
		if (this->m_pControl != NULL) {
			HRESULT hr = this->setVideo(input.gettok(4),(float)input.gettok(5).to_float(), (float)input.gettok(6).to_num(), (float)input.gettok(7).to_num(), (float)input.gettok(8).to_num());
			if (FAILED(hr)) {
				this->showError(NULL,"-v", "Unable to set video");
				DX_ERR(NULL,"-v", hr);
				//DCXError("/xdid -v", "Unable to set video");
			}
		}
		else {
			this->showError(NULL,"-v", "No File Loaded");
			//DCXError("/xdid -v", "No File Loaded");
		}
	}
	// xdid -V [NAME] [ID] [SWITCH] [+FLAGS] [ARGS]
	else if ( flags['V'] && numtok > 4 ) {
		TString flag(input.gettok( 4 ));

		if (flag[0] != '+') {
			this->showError(NULL, "-V", "Invalid Flags Identifier");
			return;
		}
		if (this->m_pControl == NULL) {
			this->showError(NULL,"-v", "No File Loaded");
			return;
		}
		switch (flag[1]) {
			case 'v': // Volume
				{
					HRESULT hr = this->setVolume((float)input.gettok(5).to_float());
					if (FAILED(hr)) {
						this->showError(NULL,"-V +v", "Unable to Set Volume");
						DX_ERR(NULL,"-V +v", hr);
					}
				}
				break;
			case 'b': // Balance
				break;
			default:
				this->showError(NULL, "-V", "Unknown Flag");
				break;
		}
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

    case WM_ERASEBKGND: 
      {
				RECT rect;
				GetClientRect( this->m_Hwnd, &rect );
				DcxControl::DrawCtrlBackground((HDC) wParam,this,&rect);
				bParsed = TRUE;
				return TRUE;
      }
      break;

		case WM_PRINTCLIENT:
			{
				bParsed = TRUE;
				HDC hdc = (HDC)wParam;
				if (this->m_pWc != NULL)
				{
					// Request the VMR to paint the video.
					HRESULT hr = this->m_pWc->RepaintVideo(this->m_Hwnd, hdc);
				}
				else { // There is no video, so paint the whole client area.
					RECT rcClient;
					GetClientRect(this->m_Hwnd, &rcClient);
					DcxControl::DrawCtrlBackground((HDC) wParam,this,&rcClient);
				}
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
				else // There is no video, so paint the whole client area.
					DcxControl::DrawCtrlBackground((HDC) wParam,this,&ps.rcPaint);
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

HRESULT DcxDirectshow::InitWindowlessVMR(
		const HWND hwndApp,                  // Window to hold the video. 
		IGraphBuilder* pGraph,         // Pointer to the Filter Graph Manager. 
		IVMRWindowlessControl9** ppWc   // Receives a pointer to the VMR.
		) 
{
	if (!pGraph || !ppWc)
		return E_POINTER;

	IBaseFilter* pVmr = NULL; 
	IVMRWindowlessControl9* pWc = NULL; 
	// Create the VMR.
	HRESULT hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVmr);

	if (FAILED(hr)) {
		this->showError(NULL,"InitWindowlessVMR", "Unable to Create Video Mixing Renderer9");
		DX_ERR(NULL,"InitWindowlessVMR", hr);
		return hr;
	}

	// Add the VMR to the filter graph.
	hr = pGraph->AddFilter(pVmr, L"Video Mixing Renderer");
	if (FAILED(hr))
	{
		pVmr->Release();
		this->showError(NULL,"InitWindowlessVMR", "Unable to Add Filter: Video Mixing Renderer");
		DX_ERR(NULL,"InitWindowlessVMR", hr);
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
	else {
		this->showError(NULL,"InitWindowlessVMR", "Unable to Get Filter Config9");
		DX_ERR(NULL,"InitWindowlessVMR", hr);
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
				this->showError(NULL,"InitWindowlessVMR", "Unable to Set Clipping Window");
				DX_ERR(NULL,"InitWindowlessVMR", hr);
			}
		}
		else {
			this->showError(NULL,"InitWindowlessVMR", "Unable to Get Windowless Control9");
			DX_ERR(NULL,"InitWindowlessVMR", hr);
		}
	}
	else {
		this->showError(NULL,"InitWindowlessVMR", "Unable to Set Rendering Options");
		DX_ERR(NULL,"InitWindowlessVMR", hr);
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
	this->m_tsFilename = "";
}
// getProperty() is non-functional atm. Where do i get this interface from? or a similar one.
HRESULT DcxDirectshow::getProperty(char *prop, const int type) const
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
			_snprintf(prop, 900, "%lS", com_prop);
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

	IVMRMixerControl9 *pMixer = NULL;
	hr = pVmr->QueryInterface(IID_IVMRMixerControl9, (void**)&pMixer);
	if (SUCCEEDED(hr)) {
		// this works BUT only gives u alpha over other streams in the mixer, not the dialog/controls bg.
		//hr = pMixer->SetAlpha(0,alpha);
		// Get the current mixing preferences.
		DWORD dwPrefs;
		pMixer->GetMixingPrefs(&dwPrefs);  

		// Remove the current render target flag.
		dwPrefs &= ~MixerPref_RenderTargetMask; 

		// Add the render target flag that we want.
		dwPrefs |= MixerPref_RenderTargetYUV;
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

					this->DrawParentsBackground(hdcbkg, &rcClient);

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
HRESULT DcxDirectshow::setVideo(const TString flags, const float brightness, const float contrast, const float hue, const float saturation)
{
	IBaseFilter* pVmr = NULL; 

	HRESULT hr = this->m_pGraph->FindFilterByName(L"Video Mixing Renderer",&pVmr);

	if (FAILED(hr))
		return hr;

	IVMRMixerControl9 *pMixer = NULL;
	hr = pVmr->QueryInterface(IID_IVMRMixerControl9, (void**)&pMixer);
	if (SUCCEEDED(hr)) {
		DWORD dwflags = 0;
		if (flags.find('b',0))
			dwflags |= ProcAmpControl9_Brightness;
		if (flags.find('c',0))
			dwflags |= ProcAmpControl9_Contrast;
		if (flags.find('h',0))
			dwflags |= ProcAmpControl9_Hue;
		if (flags.find('s',0))
			dwflags |= ProcAmpControl9_Saturation;

		if (dwflags != 0) {
			VMR9ProcAmpControl amc;
			amc.dwFlags = dwflags;
			amc.dwSize = sizeof(VMR9ProcAmpControl);
			amc.Brightness = brightness;
			amc.Contrast = contrast;
			amc.Hue = hue;
			amc.Saturation = saturation;
			hr = pMixer->SetProcAmpControl(0,&amc);
		}
		pMixer->Release();
	}
	pVmr->Release(); 
	return hr; 
}

HRESULT DcxDirectshow::getVideo(VMR9ProcAmpControl *amc) const
{
	IBaseFilter* pVmr = NULL; 

	HRESULT hr = this->m_pGraph->FindFilterByName(L"Video Mixing Renderer",&pVmr);

	if (FAILED(hr))
		return hr;

	IVMRMixerControl9 *pMixer = NULL;
	hr = pVmr->QueryInterface(IID_IVMRMixerControl9, (void**)&pMixer);
	if (SUCCEEDED(hr)) {
		ZeroMemory(amc,sizeof(VMR9ProcAmpControl));
		amc->dwSize = sizeof(VMR9ProcAmpControl);
		hr = pMixer->GetProcAmpControl(0,amc);
		pMixer->Release();
	}
	pVmr->Release(); 
	return hr;
}

HRESULT DcxDirectshow::getVideoRange(VMR9ProcAmpControlFlags prop, VMR9ProcAmpControlRange *acr) const
{
	IBaseFilter* pVmr = NULL; 

	HRESULT hr = this->m_pGraph->FindFilterByName(L"Video Mixing Renderer",&pVmr);

	if (FAILED(hr))
		return hr;

	IVMRMixerControl9 *pMixer = NULL;
	hr = pVmr->QueryInterface(IID_IVMRMixerControl9, (void**)&pMixer);
	if (SUCCEEDED(hr)) {
		ZeroMemory(acr,sizeof(VMR9ProcAmpControlRange));
		acr->dwSize = sizeof(VMR9ProcAmpControlRange);
		acr->dwProperty = prop;
		hr = pMixer->GetProcAmpControlRange(0,acr);
		pMixer->Release();
	}
	pVmr->Release(); 
	return hr;
}

UINT64 DcxDirectshow::getPosition() const
{
	if (this->m_pSeek == NULL)
		return 0;
	/*
		* ms-help://MS.MSSDK.1033/MS.WinSDK.1033/directshow/htm/am_seeking_seeking_capabilitiesenumeration.htm
		* Most DirectShow filters do not report the AM_SEEKING_CanGetCurrentPos capability flag.
		*	However, the Filter Graph Manager's implementation of IMediaSeeking::GetCurrentPosition is based on the reference clock,
		*	so you can call this method even if the capabilities flags do not include AM_SEEKING_CanGetCurrentPos.
	*/
	LONGLONG pos = 0;
	this->m_pSeek->GetCurrentPosition(&pos);

	// Format result into milliseconds
	return ((UINT64)pos / 10000);
}
HRESULT DcxDirectshow::setPosition(const UINT64 pos)
{
	if (this->m_pSeek == NULL)
		return E_POINTER;

	UINT64 mpos = pos * 10000; // convert to 100-nano secs units.
	DWORD dwCaps = AM_SEEKING_CanSeekAbsolute;
	HRESULT hr = E_FAIL;
	if (this->CheckSeekCapabilities(dwCaps) & AM_SEEKING_CanSeekAbsolute) {
		hr = this->m_pSeek->SetPositions((LONGLONG *)&mpos,AM_SEEKING_AbsolutePositioning,NULL,AM_SEEKING_NoPositioning);
	}
	return hr;
}

UINT64 DcxDirectshow::getDuration() const
{
	if (this->m_pSeek == NULL)
		return 0;

	LONGLONG pos = 0;
	DWORD dwCaps = AM_SEEKING_CanGetDuration;
	if (this->CheckSeekCapabilities(dwCaps) & AM_SEEKING_CanGetDuration) { // can get current pos
		this->m_pSeek->GetDuration(&pos);
	}

	// Format result into milliseconds
	return ((UINT64)pos / 10000);
}
DWORD DcxDirectshow::CheckSeekCapabilities(DWORD dwCaps) const
{
	if (this->m_pSeek == NULL)
		return 0;

	this->m_pSeek->CheckCapabilities(&dwCaps);
	return dwCaps;
}

HRESULT DcxDirectshow::setVolume(const float vol)
{
	IBasicAudio *pAudio = NULL;
	HRESULT hr = this->m_pGraph->QueryInterface(IID_IBasicAudio, (void**)&pAudio);
	if (SUCCEEDED(hr)) {
#pragma warning(push,3)
#pragma warning(disable:4244)
		long t = -((10000.0 / 100.0) * (100 - vol));
#pragma warning(pop)
		hr = pAudio->put_Volume(t);
		pAudio->Release();
	}
	return hr;
}

float DcxDirectshow::getVolume() const
{
	IBasicAudio *pAudio = NULL;
	HRESULT hr = this->m_pGraph->QueryInterface(IID_IBasicAudio, (void**)&pAudio);
	float vol = 0.0;
	if (SUCCEEDED(hr)) {
		long t = 0;
		hr = pAudio->get_Volume(&t);
#pragma warning(push,3)
#pragma warning(disable:4244)
		if (SUCCEEDED(hr))
			vol = 100 - ((abs(t) / 10000.0) * 100.0);
#pragma warning(pop)
		pAudio->Release();
	}
	return vol;
}

#endif // DCX_USE_DXSDK
