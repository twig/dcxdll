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
#include "defines.h"
#include "Classes/dcxdirectshow.h"
#include "Dcx.h"

#ifdef DCX_USE_DXSDK
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

DcxDirectshow::DcxDirectshow(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
	, m_pGraph(nullptr)
	, m_pControl(nullptr)
	, m_pEvent(nullptr)
	, m_pWc(nullptr)
	, m_pSeek(nullptr)
	, m_bKeepRatio(false)
	, m_bLoop(false)
{
	//assert(_DXSDK_BUILD_MAJOR == 1962);  //this checks that the DirectX SDK (June 2010) build is installed. (directx sdk is now included in windows sdk 8.0+)
	//assert(DIRECT3D_VERSION >= 9);	// make sure directx version 9+ is available.
	static_assert(DIRECT3D_VERSION >= 9,"Invalid DirectX version, v9+ required...");	// make sure directx version 9+ is available.

	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	m_Hwnd = CreateWindowEx(
		(DWORD)(ExStyles | WS_EX_CLIENTEDGE),
		TEXT("STATIC"),
		nullptr,
		(DWORD)(WS_CHILD | WS_CLIPSIBLINGS | Styles),
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDirectshow::~DcxDirectshow() {

	this->ReleaseAll();
	this->unregistreDefaultWindowProc();
}

const TString DcxDirectshow::getStyles(void) const
{
	auto styles(__super::getStyles());

	if (this->m_bKeepRatio)
		styles.addtok(TEXT("fixratio"));

	return styles;
}

void DcxDirectshow::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	*Styles |= SS_NOTIFY;

	//for (TString tsStyle(styles.getfirsttok(1)); !tsStyle.empty(); tsStyle = styles.getnexttok())
	//{
	//	if (( tsStyle == TEXT("fixratio") ))
	//		this->m_bKeepRatio = true;
	//}

	if (styles.istok(TEXT("fixratio")))
		m_bKeepRatio = true;

	parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxDirectshow::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
#if DCX_USE_HASHING
	const auto prop(input.getfirsttok(3));
	const auto propHash = std::hash<TString>{}(prop);

	if (this->m_pGraph == nullptr) {
		switch (propHash)
		{
			// [NAME] [ID] [PROP]
		case L"isloaded"_hash:
			szReturnValue = TEXT("$false");
			break;
			// [NAME] [ID] [PROP]
		case L"state"_hash:
			szReturnValue = TEXT("D_OK nofile");
			break;
		default:
			if (!parseGlobalInfoRequest(input, szReturnValue))
				throw Dcx::dcxException("No File Loaded");
			break;
		}
	}
	else {
		// [NAME] [ID] [PROP]
		switch (propHash)
		{
		case L"isloaded"_hash:
			szReturnValue = TEXT("$true");
			break;
			// [NAME] [ID] [PROP]
		case L"fname"_hash:
			szReturnValue = m_tsFilename.to_chr();
			break;
			// [NAME] [ID] [PROP]
		case L"size"_hash:
		{
			long lWidth, lHeight, lARWidth, lARHeight;
			auto hr = m_pWc->GetNativeVideoSize(&lWidth, &lHeight, &lARWidth, &lARHeight);
			if (FAILED(hr)) {
				DX_ERR(prop.to_chr(), nullptr, hr);
				throw Dcx::dcxException("Unable to get Native Video Size");
			}

			// width height arwidth arheight
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), lWidth, lHeight, lARWidth, lARHeight);
		}
		break;
		// [NAME] [ID] [PROP]
		case L"author"_hash:
			getProperty(szReturnValue, PROP_AUTHOR);
			break;
			// [NAME] [ID] [PROP]
		case L"title"_hash:
			getProperty(szReturnValue, PROP_TITLE);
			break;
			// [NAME] [ID] [PROP]
		case L"video"_hash:
		{
			VMR9ProcAmpControl amc;
			auto hr = this->getVideo(&amc);
			if (FAILED(hr)) {
				DX_ERR(prop.to_chr(), nullptr, hr);
				throw Dcx::dcxException("Unable to get Video Information");
			}

			TString vflags(TEXT('+'));
			if (amc.dwFlags & ProcAmpControl9_Brightness)
				vflags += TEXT('b');
			if (amc.dwFlags & ProcAmpControl9_Contrast)
				vflags += TEXT('c');
			if (amc.dwFlags & ProcAmpControl9_Hue)
				vflags += TEXT('h');
			if (amc.dwFlags & ProcAmpControl9_Saturation)
				vflags += TEXT('s');

			// NB: wnsprintf() doesn't support %f
			swprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%s %f %f %f %f"), vflags.to_chr(), amc.Brightness, amc.Contrast, amc.Hue, amc.Saturation);
		}
		break;
		// [NAME] [ID] [PROP]
		case L"brange"_hash:
		{
			VMR9ProcAmpControlRange acr;
			auto hr = this->getVideoRange(ProcAmpControl9_Brightness, &acr);
			if (FAILED(hr)) {
				DX_ERR(prop.to_chr(), nullptr, hr);
				throw Dcx::dcxException("Unable to get Video Information");
			}

			swprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%f %f %f %f"), acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);
			// NB: wnsprintf() doesn't support %f
		}
		break;
		// [NAME] [ID] [PROP]
		case L"crange"_hash:
		{
			VMR9ProcAmpControlRange acr;
			auto hr = this->getVideoRange(ProcAmpControl9_Contrast, &acr);
			if (FAILED(hr)) {
				DX_ERR(prop.to_chr(), nullptr, hr);
				throw Dcx::dcxException("Unable to get Video Information");
			}

			swprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%f %f %f %f"), acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);
			// NB: wnsprintf() doesn't support %f
		}
		break;
		// [NAME] [ID] [PROP]
		case L"hrange"_hash:
		{
			VMR9ProcAmpControlRange acr;
			auto hr = this->getVideoRange(ProcAmpControl9_Hue, &acr);
			if (FAILED(hr)) {
				DX_ERR(prop.to_chr(), nullptr, hr);
				throw Dcx::dcxException("Unable to get Video Information");
			}

			swprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%f %f %f %f"), acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);
			// NB: wnsprintf() doesn't support %f
		}
		break;
		// [NAME] [ID] [PROP]
		case L"srange"_hash:
		{
			VMR9ProcAmpControlRange acr;
			auto hr = getVideoRange(ProcAmpControl9_Saturation, &acr);
			if (FAILED(hr)) {
				DX_ERR(prop.to_chr(), nullptr, hr);
				throw Dcx::dcxException("Unable to get Video Information");
			}

			swprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%f %f %f %f"), acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);
			// NB: wnsprintf() doesn't support %f
		}
		break;
		// [NAME] [ID] [PROP]
		case L"currentpos"_hash:
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("D_OK %I64u"), getPosition());
			break;
			// [NAME] [ID] [PROP]
		case L"duration"_hash:
		{
			if (this->CheckSeekCapabilities(AM_SEEKING_CanGetDuration) & AM_SEEKING_CanGetDuration)
				wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("D_OK %I64u"), this->getDuration());
			else
				szReturnValue = TEXT("D_ERROR Method Not Supported");
		}
		break;
		// [NAME] [ID] [PROP]
		case L"volume"_hash:
			swprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("D_OK %ld"), getVolume());
			break;
			// [NAME] [ID] [PROP]
		case L"state"_hash:
		{
			/*
			sprintf(szReturnValue, TEXT("D_OK %s"), TEXT("nofile"));  // done
			sprintf(szReturnValue, TEXT("D_OK %s"), TEXT("stopped")); // done
			sprintf(szReturnValue, TEXT("D_OK %s"), TEXT("paused"));  // done
			sprintf(szReturnValue, TEXT("D_OK %s"), TEXT("playing")); // done
			sprintf(szReturnValue, TEXT("D_OK %s"), TEXT("rewind"));
			sprintf(szReturnValue, TEXT("D_OK %s"), TEXT("fastforward"));
			and anything else you can think of i guess
			*/

			OAFilterState pfs = 0;
			PTCHAR szState = nullptr;
			auto hr = this->m_pControl->GetState(1000, &pfs);

			if (SUCCEEDED(hr)) {
				switch (pfs) {
				case State_Stopped:
					szState = TEXT("stopped");
					break;
				case State_Paused:
					szState = TEXT("paused");
					break;
				case State_Running:
					szState = TEXT("playing");
					break;
				default:
					szState = TEXT("unknown");
					break;
				}
				wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("D_OK %s"), szState);
			}
			else {
				DX_ERR(prop.to_chr(), nullptr, hr);
				throw Dcx::dcxException("Unable to get State Information");
			}
		}
		break;
		default:
			parseGlobalInfoRequest(input, szReturnValue);
		}
	}
#else
	const auto prop(input.getfirsttok(3));

	if (this->m_pGraph == nullptr) {
		// [NAME] [ID] [PROP]
		if ( prop == TEXT("isloaded")) {
			dcx_strcpyn(szReturnValue, TEXT("$false"), MIRC_BUFFER_SIZE_CCH);
		}
		// [NAME] [ID] [PROP]
		else if (prop == TEXT("state")) {
			dcx_strcpyn(szReturnValue, TEXT("D_OK nofile"), MIRC_BUFFER_SIZE_CCH);
		}
		else if (!parseGlobalInfoRequest(input, szReturnValue))
			throw Dcx::dcxException("No File Loaded");
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("isloaded")) {
		dcx_strcpyn(szReturnValue, TEXT("$true"), MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("fname")) {
		dcx_strcpyn(szReturnValue, this->m_tsFilename.to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("size")) {
		long lWidth, lHeight, lARWidth, lARHeight;
		auto hr = m_pWc->GetNativeVideoSize(&lWidth, &lHeight, &lARWidth, &lARHeight);
		if (FAILED(hr)) {
			DX_ERR(prop.to_chr(), nullptr, hr);
			throw Dcx::dcxException("Unable to get Native Video Size");
		}

		// width height arwidth arheight
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), lWidth, lHeight, lARWidth, lARHeight);
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("author")) {
		getProperty(szReturnValue, PROP_AUTHOR);
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("title")) {
		getProperty(szReturnValue, PROP_TITLE);
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("video")) {
		VMR9ProcAmpControl amc;
		auto hr = this->getVideo(&amc);
		if (FAILED(hr)) {
			DX_ERR(prop.to_chr(), nullptr, hr);
			throw Dcx::dcxException("Unable to get Video Information");
		}

		TString vflags(TEXT('+'));
		if (amc.dwFlags & ProcAmpControl9_Brightness)
			vflags += TEXT('b');
		if (amc.dwFlags & ProcAmpControl9_Contrast)
			vflags += TEXT('c');
		if (amc.dwFlags & ProcAmpControl9_Hue)
			vflags += TEXT('h');
		if (amc.dwFlags & ProcAmpControl9_Saturation)
			vflags += TEXT('s');

		// NB: wnsprintf() doesn't support %f
		swprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%s %f %f %f %f"), vflags.to_chr(), amc.Brightness, amc.Contrast, amc.Hue, amc.Saturation);
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("brange")) {
		VMR9ProcAmpControlRange acr;
		auto hr = this->getVideoRange(ProcAmpControl9_Brightness, &acr);
		if (FAILED(hr)) {
			DX_ERR(prop.to_chr(), nullptr, hr);
			throw Dcx::dcxException("Unable to get Video Information");
		}

		swprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH,TEXT("%f %f %f %f"), acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);
		// NB: wnsprintf() doesn't support %f
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("crange")) {
		VMR9ProcAmpControlRange acr;
		auto hr = this->getVideoRange(ProcAmpControl9_Contrast, &acr);
		if (FAILED(hr)) {
			DX_ERR(prop.to_chr(), nullptr, hr);
			throw Dcx::dcxException("Unable to get Video Information");
		}

		swprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%f %f %f %f"), acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);
		// NB: wnsprintf() doesn't support %f
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("hrange")) {
		VMR9ProcAmpControlRange acr;
		auto hr = this->getVideoRange(ProcAmpControl9_Hue, &acr);
		if (FAILED(hr)) {
			DX_ERR(prop.to_chr(), nullptr, hr);
			throw Dcx::dcxException("Unable to get Video Information");
		}

		swprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%f %f %f %f"), acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);
		// NB: wnsprintf() doesn't support %f
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("srange")) {
		VMR9ProcAmpControlRange acr;
		auto hr = this->getVideoRange(ProcAmpControl9_Saturation, &acr);
		if (FAILED(hr)) {
			DX_ERR(prop.to_chr(), nullptr, hr);
			throw Dcx::dcxException("Unable to get Video Information");
		}

		swprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%f %f %f %f"), acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);
		// NB: wnsprintf() doesn't support %f
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("currentpos")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("D_OK %I64u"), this->getPosition());
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("duration")) {
		if (this->CheckSeekCapabilities(AM_SEEKING_CanGetDuration) & AM_SEEKING_CanGetDuration)
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("D_OK %I64u"), this->getDuration());
		else
			dcx_strcpyn(szReturnValue, TEXT("D_ERROR Method Not Supported"), MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("volume")) {
		swprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("D_OK %ld"), this->getVolume());
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("state")) {
		/*
		sprintf(szReturnValue, TEXT("D_OK %s"), TEXT("nofile"));  // done
		sprintf(szReturnValue, TEXT("D_OK %s"), TEXT("stopped")); // done
		sprintf(szReturnValue, TEXT("D_OK %s"), TEXT("paused"));  // done
		sprintf(szReturnValue, TEXT("D_OK %s"), TEXT("playing")); // done
		sprintf(szReturnValue, TEXT("D_OK %s"), TEXT("rewind"));
		sprintf(szReturnValue, TEXT("D_OK %s"), TEXT("fastforward"));
		and anything else you can think of i guess
		*/

		OAFilterState pfs = 0;
		PTCHAR szState = nullptr;
		auto hr = this->m_pControl->GetState(1000, &pfs);

		if (SUCCEEDED(hr)) {
			switch (pfs) {
			case State_Stopped:
				szState = TEXT("stopped");
				break;
			case State_Paused:
				szState = TEXT("paused");
				break;
			case State_Running:
				szState = TEXT("playing");
				break;
			default:
				szState = TEXT("unknown");
				break;
			}
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("D_OK %s"), szState);
		}
		else {
			DX_ERR(prop.to_chr(), nullptr, hr);
			throw Dcx::dcxException("Unable to get State Information");
		}
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
#endif
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDirectshow::parseCommandRequest( const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok( );

	// xdid -a [NAME] [ID] [SWITCH] [+FLAGS] [FILE]
	if ( flags[TEXT('a')] && numtok > 4 ) {
		const XSwitchFlags xflags(input.getnexttok( ).trim());	// tok 4
		auto filename(input.getlasttoks().trim());			// tok 5, -1

		this->ReleaseAll();

		if (!Dcx::isDX9Installed())
			Dcx::initDirectX();

		if (!Dcx::isDX9Installed())
			throw Dcx::dcxException("Needs DirectX 9+");

		if (!xflags[TEXT('+')])
			throw Dcx::dcxException("Invalid Flags");

		if (!IsFile(filename))
			throw Dcx::dcxException(TEXT("Unable to Access File: %"), filename);

		HRESULT hr = S_OK;

		try {
			// Create the Filter Graph Manager and query for interfaces.
			hr = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&this->m_pGraph);

			if (FAILED(hr))
				throw Dcx::dcxException("Unable to Create FilterGraph");

			hr = this->m_pGraph->QueryInterface(IID_IMediaControl, (void **)&this->m_pControl);
			if (FAILED(hr))
				throw Dcx::dcxException("Unable to Get IMediaControl");

			hr = this->m_pGraph->QueryInterface(IID_IMediaEventEx, (void **)&this->m_pEvent);
			if (FAILED(hr))
				throw Dcx::dcxException("Unable to Get IMediaEventEx");
			
			hr = this->m_pGraph->QueryInterface(IID_IMediaSeeking, (void **)&this->m_pSeek);
			if (FAILED(hr))
				throw Dcx::dcxException("Unable to Get IMediaSeeking");
			
			hr = this->m_pEvent->SetNotifyWindow((OAHWND)m_Hwnd, WM_GRAPHNOTIFY, 0);
			if (FAILED(hr))
				throw Dcx::dcxException("Unable to Set Window Notify");
			
			if (!xflags[TEXT('a')])
				hr = DcxDirectshow::InitWindowlessVMR(m_Hwnd, this->m_pGraph, &this->m_pWc);

			if (FAILED(hr))
				throw Dcx::dcxException("Unable to Create VMR9");

			if (this->m_pWc != nullptr) {
				if (this->m_bKeepRatio)
					hr = this->m_pWc->SetAspectRatioMode(VMR9ARMode_LetterBox); // caused video to maintain aspect ratio
				else
					hr = this->m_pWc->SetAspectRatioMode(VMR9ARMode_None);

				if (FAILED(hr))
					throw Dcx::dcxException("Unable to Set Aspect");
			}

			hr = this->m_pGraph->RenderFile(filename.to_chr(), nullptr);
			if (FAILED(hr))
				throw Dcx::dcxException("Unable to render file (No codec for file format?)");

			if (this->m_pWc != nullptr) {
				hr = this->SetVideoPos();
				if (FAILED(hr))
					throw Dcx::dcxException("Unable to set Video Position");

				if (this->m_bAlphaBlend)
					this->setAlpha(0.5);
			}
			else { // if VMR == nullptr then disable video.
				IVideoWindow *p_Video;
				hr = this->m_pGraph->QueryInterface(IID_IVideoWindow, (void **)&p_Video);
				if (FAILED(hr))
					throw Dcx::dcxException("Unable to get video window");

				Auto(p_Video->Release());

				p_Video->put_Visible(OAFALSE);
				p_Video->put_AutoShow(OAFALSE);
				p_Video->put_Owner((OAHWND)m_Hwnd);
				p_Video->put_MessageDrain((OAHWND)m_Hwnd);
				DWORD styles = 0;

				hr = p_Video->get_WindowStyle((long *)&styles);
				if (FAILED(hr))
					throw Dcx::dcxException("Unable to get window styles");

				styles &= ~(WS_OVERLAPPEDWINDOW | WS_POPUPWINDOW | WS_DLGFRAME);
				styles |= WS_CHILD;
				p_Video->put_WindowStyle((long)styles);
				p_Video->put_Left(0);
				p_Video->put_Top(0);
			}

			//if (xflags[TEXT('l')])
			//	this->m_bLoop = true;
			//else
			//	this->m_bLoop = false;

			m_bLoop = xflags[TEXT('l')];

			if (xflags[TEXT('p')])
				m_pControl->Run();

			m_tsFilename = filename;

			InvalidateRect(m_Hwnd, nullptr, TRUE);
		}
		catch (std::exception)
		{
			DX_ERR(nullptr, TEXT("-a"), hr);
			this->ReleaseAll();
			throw;
		}
	}
	// xdid -c [NAME] [ID] [SWITCH] [COMMAND]
	else if ( flags[TEXT('c')] && numtok > 3 ) {
		if (this->m_pControl == nullptr)
			throw Dcx::dcxException("No File Loaded");

		static const TString cmdlist(TEXT("play pause stop close seek"));
		const auto nType = cmdlist.findtok(input.getnexttok(), 1);	// tok 4
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
				this->setPosition(input.getnexttok().to_<UINT64>());	// tok 5
				this->m_pControl->StopWhenReady(); // causes new image to be rendered.
			}
			break;
		case 0: // error
		default:
			throw Dcx::dcxException("Invalid Command");
		}
	}
	// xdid -v [NAME] [ID] [SWITCH] [+FLAGS] [BRIGHTNESS] [CONTRAST] [HUE] [SATURATION]
	else if ( flags[TEXT('v')] && numtok > 7 ) {
		if (m_pControl == nullptr)
			throw Dcx::dcxException("No File Loaded");

		auto tsFlags(input.getnexttok());
		auto fBrightness(input.getnexttok().to_<float>());
		auto fContrast(input.getnexttok().to_<float>());
		auto fHue(input.getnexttok().to_<float>());
		auto fSaturation(input.getnexttok().to_<float>());

		auto hr = setVideo(tsFlags, fBrightness, fContrast, fHue, fSaturation);
		if (FAILED(hr)) {
			DX_ERR(nullptr,TEXT("-v"), hr);
			throw Dcx::dcxException("Unable to set video");
		}
	}
	// xdid -V [NAME] [ID] [SWITCH] [+FLAGS] [ARGS]
	else if ( flags[TEXT('V')] && numtok > 4 ) {
		const auto flag(input.getnexttok());	// tok 4

		if (flag[0] != TEXT('+'))
			throw Dcx::dcxException("Invalid Flags Identifier");

		if (this->m_pControl == nullptr)
			throw Dcx::dcxException("No File Loaded");

		switch (flag[1]) {
			case TEXT('v'): // Volume
				{
					auto hr = this->setVolume(input.getnexttok().to_<long>());	// tok 5
					if (FAILED(hr)) {
						DX_ERR(nullptr, TEXT("-V +v"), hr);
						throw Dcx::dcxException("Unable to Set Volume");
					}
				}
				break;
			case TEXT('b'): // Balance
				break;
			default:
				throw Dcx::dcxException("Unknown Flag");
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
			if (GetClientRect( m_Hwnd, &rect ))
				DcxControl::DrawCtrlBackground((HDC) wParam,this,&rect);
			bParsed = TRUE;
			return TRUE;
		}
		break;

	case WM_PRINTCLIENT:
		{
			bParsed = TRUE;
			dcxwParam(HDC, hdc);

			if (this->m_pWc != nullptr)
			{
				// Request the VMR to paint the video.
				this->m_pWc->RepaintVideo(m_Hwnd, hdc);
			}
			else { // There is no video, so paint the whole client area.
				RECT rcClient;
				if (GetClientRect(m_Hwnd, &rcClient))
					DcxControl::DrawCtrlBackground((HDC) wParam,this,&rcClient);
			}
		}
		break;
	case WM_PAINT:
		{
			bParsed = TRUE;
			PAINTSTRUCT ps;

			auto hdc = BeginPaint(m_Hwnd, &ps);
			if (hdc != nullptr)
			{
				if (this->m_pWc != nullptr)
				{
					// Request the VMR to paint the video.
					this->m_pWc->RepaintVideo(m_Hwnd, hdc);
				}
				else // There is no video, so paint the whole client area.
					DcxControl::DrawCtrlBackground(hdc, this, &ps.rcPaint);
			}
			EndPaint(m_Hwnd, &ps); 
		}
		break;

	case WM_SIZE:
		{
			if (this->m_pWc != nullptr)
				this->SetVideoPos();
		}
		break;

	case WM_DISPLAYCHANGE:
		{
			if (this->m_pWc != nullptr)
				this->m_pWc->DisplayModeChanged();
		}
		break;

	case WM_GRAPHNOTIFY:
		{
			bParsed = TRUE;
			if (this->m_pEvent == nullptr)
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
						this->m_pSeek->SetPositions(&rtNow, AM_SEEKING_AbsolutePositioning, nullptr, AM_SEEKING_NoPositioning);
						if (!this->m_bLoop) {
							this->m_pControl->StopWhenReady();
							this->execAliasEx(TEXT("%s,%d,%s"),TEXT("dshow"),this->getUserID(),TEXT("completed"));
						}
					}
					break;
					//case EC_PAUSED: // oddly this is sent when we play the file too.
					//	this->execAliasEx(TEXT("%s,%d,%s"),TEXT("dshow"),this->getUserID(),TEXT("paused"));
					//	break;
					//case EC_USERABORT:
					//case EC_ERRORABORT:
					//	this->execAliasEx(TEXT("%s,%d,%s"),TEXT("dshow"),this->getUserID(),TEXT("aborted"));
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
	const HWND hwndApp,				// Window to hold the video. 
	IGraphBuilder* pGraph,			// Pointer to the Filter Graph Manager. 
	IVMRWindowlessControl9** ppWc	// Receives a pointer to the VMR.
	)
{
	if (!pGraph || !ppWc)
		return E_POINTER;

	HRESULT hr = S_OK;

	try {
		IBaseFilter* pVmr = nullptr;

		// Create the VMR.
		hr = CoCreateInstance(CLSID_VideoMixingRenderer9, nullptr, CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVmr);

		if (FAILED(hr))
			throw Dcx::dcxException("InitWindowlessVMR() - Unable to Create Video Mixing Renderer9");

		Auto(pVmr->Release());

		// Add the VMR to the filter graph.
		hr = pGraph->AddFilter(pVmr, L"Video Mixing Renderer");	// dont use TEXT() here.
		if (FAILED(hr))
			throw Dcx::dcxException("InitWindowlessVMR() - Unable to Add Filter: Video Mixing Renderer");

		// Set the rendering mode.
		{
			IVMRFilterConfig9* pConfig;
			hr = pVmr->QueryInterface(IID_IVMRFilterConfig9, (void**)&pConfig);
			if (FAILED(hr))
				throw Dcx::dcxException("InitWindowlessVMR() - Unable to Get Filter Config9");
			Auto(pConfig->Release());

			hr = pConfig->SetRenderingMode(VMR9Mode_Windowless);
			if (dcx_testflag(GetWindowExStyle(hwndApp), WS_EX_TRANSPARENT))
				hr = pConfig->SetRenderingPrefs(RenderPrefs9_DoNotRenderBorder);

			if (FAILED(hr))
				throw Dcx::dcxException("InitWindowlessVMR() - Unable to Set Rendering Options");
		}
		// Set the window.
		{
			IVMRWindowlessControl9* pWc = nullptr;
			hr = pVmr->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pWc);
			if (FAILED(hr))
				throw Dcx::dcxException("InitWindowlessVMR() - Unable to Get Windowless Control9");

			hr = pWc->SetVideoClippingWindow(hwndApp);
			//if (SUCCEEDED(hr)) {
			//	IVMRMixerControl9 *pMixer;
			//	hr = pVmr->QueryInterface(IID_IVMRMixerControl9, (void**)&pMixer);
			//	if (SUCCEEDED(hr)) {
			//		pMixer->SetAlpha(0,0.5);
			//		pMixer->Release();
			//	}
			//}
			if (FAILED(hr))
			{
				// An error occurred, so release the interface.
				pWc->Release();
				throw Dcx::dcxException("InitWindowlessVMR() - Unable to Set Clipping Window");
			}

			*ppWc = pWc; // Return this as an AddRef'd pointer.
		}
		return hr;
	}
	catch (std::exception)
	{
		DX_ERR(nullptr, TEXT("InitWindowlessVMR"), hr);
		throw;
	}
}

HRESULT DcxDirectshow::SetVideoPos(void)
{
	if (this->m_pWc == nullptr)
		return E_POINTER;

	long lWidth, lHeight;
	auto hr = this->m_pWc->GetNativeVideoSize(&lWidth, &lHeight, nullptr, nullptr);
	if (SUCCEEDED(hr))
	{
		RECT rcSrc, rcDest;
		// Set the source rectangle.
		SetRect(&rcSrc, 0, 0, lWidth, lHeight);

		// Get the window client area.
		if (!GetClientRect(m_Hwnd, &rcDest))
			return E_FAIL;

		// Set the destination rectangle.
		SetRect(&rcDest, 0, 0, rcDest.right, rcDest.bottom);

		// Set the video position.
		hr = this->m_pWc->SetVideoPosition(&rcSrc, &rcDest);
	}
	return hr;
}

void DcxDirectshow::ReleaseAll(void)
{
	if (this->m_pControl != nullptr)
		this->m_pControl->Release();
	if (this->m_pEvent != nullptr) {
		this->m_pEvent->SetNotifyWindow(NULL,0,0);
		this->m_pEvent->Release();
	}
	if (this->m_pSeek != nullptr)
		this->m_pSeek->Release();
	if (this->m_pWc != nullptr)
		this->m_pWc->Release();
	if (this->m_pGraph != nullptr)
		this->m_pGraph->Release();
	this->m_pControl = nullptr;
	this->m_pEvent = nullptr;
	this->m_pGraph = nullptr;
	this->m_pWc = nullptr;
	this->m_pSeek = nullptr;
	this->m_tsFilename.clear();	// = TEXT("");
}
// getProperty() is non-functional atm. Where do i get this interface from? or a similar one.
HRESULT DcxDirectshow::getProperty(TCHAR *prop, const int type) const
{
	IAMMediaContent *iam;
	auto hr = this->m_pGraph->QueryInterface(IID_IAMMediaContent, (void **)&iam);
	if (SUCCEEDED(hr)) {
		Auto(iam->Release());

		BSTR com_prop = nullptr;
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
			//_snwprintf(prop, MIRC_BUFFER_SIZE_CCH, TEXT("%lS"), com_prop);
			_snwprintf(prop, MIRC_BUFFER_SIZE_CCH, TEXT("%ws"), com_prop);
			SysFreeString(com_prop);
		}
		else
			dcx_strcpyn(prop, TEXT("Not Supported"), MIRC_BUFFER_SIZE_CCH);
	}
	else
		dcx_strcpyn(prop, TEXT("failed"), MIRC_BUFFER_SIZE_CCH);

	return hr;
}

HRESULT DcxDirectshow::setAlpha(float alpha)
{
	if ((alpha < 0) || (alpha > 1.0))
		alpha = 1.0;

	RECT rcClient = { 0 }, rcWin = { 0 };

	if ((!GetClientRect(m_Hwnd, &rcClient)) || (!GetWindowRect(m_Hwnd, &rcWin)))
		return E_FAIL;

	IBaseFilter* pVmr = nullptr;

	auto hr = this->m_pGraph->FindFilterByName(L"Video Mixing Renderer", &pVmr);

	if (FAILED(hr))
		return hr;
	Auto(pVmr->Release());

	IVMRMixerControl9 *pMixer = nullptr;
	hr = pVmr->QueryInterface(IID_IVMRMixerControl9, (void**)&pMixer);
	if (SUCCEEDED(hr)) {
		Auto(pMixer->Release());

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
	}
	IVMRMixerBitmap9 *pBm = nullptr;
	hr = pVmr->QueryInterface(IID_IVMRMixerBitmap9, (void**)&pBm);
	if (SUCCEEDED(hr)) {
		Auto(pBm->Release());

		auto hdc = GetDC(m_Hwnd);
		if (hdc != nullptr) { // make duplicate hdc;
			Auto(ReleaseDC(m_Hwnd, hdc));

			auto hdcBuf = CreateHDCBuffer(hdc, &rcWin);

			if (hdcBuf != nullptr) {
				Auto(DeleteHDCBuffer(hdcBuf));

				long cx, cy;

				this->DrawParentsBackground(*hdcBuf, &rcClient);

				hr = this->m_pWc->GetNativeVideoSize(&cx, &cy, nullptr, nullptr);
				if (SUCCEEDED(hr)) {
					VMR9AlphaBitmap bmpInfo;
					ZeroMemory(&bmpInfo, sizeof(bmpInfo));
					const auto w = (rcWin.right - rcWin.left), h = (rcWin.bottom - rcWin.top);

					bmpInfo.dwFlags = VMR9AlphaBitmap_hDC;
					//bmpInfo.dwFilterMode = MixerPref9_AnisotropicFiltering;
					bmpInfo.hdc = *hdcBuf;
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
					if (SUCCEEDED(hr)) {
						ZeroMemory(&bmpInfo, sizeof(bmpInfo));
						bmpInfo.dwFlags = VMR9AlphaBitmap_SrcRect;
						bmpInfo.hdc = nullptr;
						// Set the transparency value (1.0 is opaque, 0.0 is transparent).
						bmpInfo.fAlpha = alpha;
						//POINT pt;
						//pt.x = rcWin.left;
						//pt.y = rcWin.top;
						//ScreenToClient(GetParent(m_Hwnd),&pt);
						//CopyRect(&bmpInfo.rSrc, &rcWin);
						//SetRect(&bmpInfo.rSrc, pt.x, pt.y, pt.x + w, pt.y + h);
						CopyRect(&bmpInfo.rSrc, &rcClient);
						bmpInfo.rDest.left = 0.f;
						bmpInfo.rDest.top = 0.f;
						bmpInfo.rDest.right = 1.0; //(float)(rcClient.right - rcClient.left) / cx;
						bmpInfo.rDest.bottom = 1.0; //(float)(rcClient.bottom - rcClient.top) / cy;
						hr = pBm->UpdateAlphaBitmapParameters(&bmpInfo);
					}
				}
			}
			/*
						HDC hdcbkg = CreateCompatibleDC( hdc );
						if (hdcbkg != nullptr) {
						RECT rcClient, rcWin;
						GetClientRect(m_Hwnd, &rcClient);
						GetWindowRect(m_Hwnd, &rcWin);
						const int w = (rcWin.right - rcWin.left), h = (rcWin.bottom - rcWin.top);
						HBITMAP memBM = CreateCompatibleBitmap ( hdc, w, h );
						if (memBM != nullptr) {
						// associate bitmap with HDC
						const HBITMAP oldBM = SelectBitmap ( hdcbkg, memBM );

						this->DrawParentsBackground(hdcbkg, &rcClient);

						long cx, cy;
						hr = this->m_pWc->GetNativeVideoSize(&cx, &cy, nullptr, nullptr);
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
						bmpInfo.hdc = nullptr;
						// Set the transparency value (1.0 is opaque, 0.0 is transparent).
						bmpInfo.fAlpha = alpha;
						//POINT pt;
						//pt.x = rcWin.left;
						//pt.y = rcWin.top;
						//ScreenToClient(GetParent(m_Hwnd),&pt);
						//CopyRect(&bmpInfo.rSrc, &rcWin);
						//SetRect(&bmpInfo.rSrc, pt.x, pt.y, pt.x + w, pt.y + h);
						CopyRect(&bmpInfo.rSrc, &rcClient);
						bmpInfo.rDest.left = 0.f;
						bmpInfo.rDest.top = 0.f;
						bmpInfo.rDest.right = 1.0; //(float)(rcClient.right - rcClient.left) / cx;
						bmpInfo.rDest.bottom = 1.0; //(float)(rcClient.bottom - rcClient.top) / cy;
						hr = pBm->UpdateAlphaBitmapParameters(&bmpInfo);
						}
						DeleteBitmap(SelectBitmap(hdcbkg,oldBM));
						}
						else
						hr = E_FAIL;
						DeleteDC( hdcbkg );
						}
						else
						hr = E_FAIL;
						ReleaseDC(m_Hwnd, hdc);
						*/
		}
		else
			hr = E_FAIL;
	}
	return hr;
}
HRESULT DcxDirectshow::setVideo(const TString &flags, const float brightness, const float contrast, const float hue, const float saturation)
{
	IBaseFilter* pVmr = nullptr; 

	auto hr = this->m_pGraph->FindFilterByName(L"Video Mixing Renderer", &pVmr);

	if (FAILED(hr))
		return hr;
	Auto(pVmr->Release());

	IVMRMixerControl9 *pMixer = nullptr;
	hr = pVmr->QueryInterface(IID_IVMRMixerControl9, (void**)&pMixer);
	if (SUCCEEDED(hr)) {
		Auto(pMixer->Release());

		const XSwitchFlags xflags(flags);
		DWORD dwflags = 0;
		if (xflags[TEXT('b')])
			dwflags |= ProcAmpControl9_Brightness;
		if (xflags[TEXT('c')])
			dwflags |= ProcAmpControl9_Contrast;
		if (xflags[TEXT('h')])
			dwflags |= ProcAmpControl9_Hue;
		if (xflags[TEXT('s')])
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
	}
	return hr; 
}

HRESULT DcxDirectshow::getVideo(VMR9ProcAmpControl *amc) const
{
	IBaseFilter* pVmr = nullptr; 

	auto hr = this->m_pGraph->FindFilterByName(L"Video Mixing Renderer", &pVmr);

	if (FAILED(hr))
		return hr;
	Auto(pVmr->Release());

	IVMRMixerControl9 *pMixer = nullptr;
	hr = pVmr->QueryInterface(IID_IVMRMixerControl9, (void**)&pMixer);
	if (SUCCEEDED(hr)) {
		Auto(pMixer->Release());

		ZeroMemory(amc,sizeof(VMR9ProcAmpControl));
		amc->dwSize = sizeof(VMR9ProcAmpControl);
		hr = pMixer->GetProcAmpControl(0,amc);
	}
	return hr;
}

HRESULT DcxDirectshow::getVideoRange(VMR9ProcAmpControlFlags prop, VMR9ProcAmpControlRange *acr) const
{
	IBaseFilter* pVmr = nullptr; 

	auto hr = this->m_pGraph->FindFilterByName(L"Video Mixing Renderer", &pVmr);

	if (FAILED(hr))
		return hr;
	Auto(pVmr->Release());

	IVMRMixerControl9 *pMixer = nullptr;
	hr = pVmr->QueryInterface(IID_IVMRMixerControl9, (void**)&pMixer);
	if (SUCCEEDED(hr)) {
		Auto(pMixer->Release());

		ZeroMemory(acr,sizeof(VMR9ProcAmpControlRange));
		acr->dwSize = sizeof(VMR9ProcAmpControlRange);
		acr->dwProperty = prop;
		hr = pMixer->GetProcAmpControlRange(0,acr);
	}
	return hr;
}

UINT64 DcxDirectshow::getPosition() const
{
	if (this->m_pSeek == nullptr)
		return 0;
	/*
		* ms-help://MS.MSSDK.1033/MS.WinSDK.1033/directshow/htm/am_seeking_seeking_capabilitiesenumeration.htm
		* Most DirectShow filters do not report the AM_SEEKING_CanGetCurrentPos capability flag.
		*	However, the Filter Graph Manager's implementation of IMediaSeeking::GetCurrentPosition is based on the reference clock,
		*	so you can call this method even if the capabilities flags do not include AM_SEEKING_CanGetCurrentPos.
	*/
	UINT64 pos;
	if (SUCCEEDED(this->m_pSeek->GetCurrentPosition((INT64 *)&pos)))
	{
		// Format result into milliseconds
		return pos / 10000;
	}

	return 0;
}
HRESULT DcxDirectshow::setPosition(const UINT64 pos)
{
	if (this->m_pSeek == nullptr)
		return E_POINTER;

	UINT64 mpos = pos * 10000; // convert to 100-nano secs units.
	DWORD dwCaps = AM_SEEKING_CanSeekAbsolute;
	HRESULT hr = E_FAIL;

	if (dcx_testflag(this->CheckSeekCapabilities(dwCaps), AM_SEEKING_CanSeekAbsolute))
		hr = this->m_pSeek->SetPositions((LONGLONG *)&mpos,AM_SEEKING_AbsolutePositioning,nullptr,AM_SEEKING_NoPositioning);

	return hr;
}

UINT64 DcxDirectshow::getDuration() const
{
	if (this->m_pSeek == nullptr)
		return 0;

	DWORD dwCaps = AM_SEEKING_CanGetDuration;
	if (dcx_testflag(this->CheckSeekCapabilities(dwCaps), AM_SEEKING_CanGetDuration))
	{ // can get current pos
		UINT64 pos = 0ULL;
		if (SUCCEEDED(this->m_pSeek->GetDuration((INT64 *)&pos)))
			// Format result into milliseconds
			return pos / 10000;
	}

	return 0;
}
DWORD DcxDirectshow::CheckSeekCapabilities(DWORD dwCaps) const
{
	if (this->m_pSeek == nullptr)
		return 0;

	this->m_pSeek->CheckCapabilities(&dwCaps);
	return dwCaps;
}

HRESULT DcxDirectshow::setVolume(const long vol)
{
	if ((vol < 0) || (vol > 100))
		return E_FAIL;

	IBasicAudio *pAudio = nullptr;
	auto hr = this->m_pGraph->QueryInterface(IID_IBasicAudio, (void**)&pAudio);
	if (SUCCEEDED(hr)) {
		Auto(pAudio->Release());

#pragma warning(push,3)
#pragma warning(disable:4244)
		//const long t = (long)-((10000.0 / 100.0) * (100.0 - vol));
		const long t = (long)-(100 * (100 - vol));
#pragma warning(pop)
		hr = pAudio->put_Volume(t);
	}
	return hr;
}

long DcxDirectshow::getVolume() const
{
	IBasicAudio *pAudio = nullptr;
	long vol = 0;

	auto hr = this->m_pGraph->QueryInterface(IID_IBasicAudio, (void**)&pAudio);
	if (SUCCEEDED(hr)) {
		Auto(pAudio->Release());

		long t = 0;
		hr = pAudio->get_Volume(&t);
#pragma warning(push,3)
#pragma warning(disable:4244)
		if (SUCCEEDED(hr))
			//vol = (float)(100.0 - ((abs(t) / 10000.0) * 100.0));
			//vol = (abs(t) / 100);
		    vol = (100 - ((abs(t) / 10000) * 100));
#pragma warning(pop)
	}
	return vol;
}

#endif // DCX_USE_DXSDK
