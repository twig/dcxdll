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
DcxDirectshow::DcxDirectshow(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog, DcxControlTypes::DIRECTSHOW)
{
	//assert(_DXSDK_BUILD_MAJOR == 1962);  //this checks that the DirectX SDK (June 2010) build is installed. (directx sdk is now included in windows sdk 8.0+)
	//assert(DIRECT3D_VERSION >= 9);	// make sure directx version 9+ is available.
	static_assert(DIRECT3D_VERSION >= 9, "Invalid DirectX version, v9+ required...");	// make sure directx version 9+ is available.

	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WS_EX_CLIENTEDGE,
		DCX_DIRECTSHOWCLASS,
		ws.m_Styles | WS_CHILD | WS_CLIPSIBLINGS,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		DcxUXModule::dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDirectshow::~DcxDirectshow()
{
	this->ReleaseAll();
}

const TString DcxDirectshow::getStyles(void) const
{
	auto styles(__super::getStyles());

	if (this->m_bKeepRatio)
		styles.addtok(TEXT("fixratio"));

	return styles;
}

dcxWindowStyles DcxDirectshow::parseControlStyles(const TString& tsStyles)
{
	dcxWindowStyles ws;

	ws.m_Styles |= SS_NOTIFY;

	if (tsStyles.istok(TEXT("fixratio")))
		m_bKeepRatio = true;

	return parseGeneralControlStyles(tsStyles, ws);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */
GSL_SUPPRESS(type.4)
void DcxDirectshow::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	const auto prop(input.getfirsttok(3));

	if (const auto propHash = std::hash<TString>{}(prop); !this->m_pGraph)
	{
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
			if (!m_pWc)
				throw Dcx::dcxException("No IVMRWindowlessControl9 set.");

			long lWidth{}, lHeight{}, lARWidth{}, lARHeight{};

			if (const auto hr = m_pWc->GetNativeVideoSize(&lWidth, &lHeight, &lARWidth, &lARHeight); FAILED(hr))
			{
				DX_ERR(prop.to_chr(), nullptr, hr);
				throw Dcx::dcxException("Unable to get Native Video Size");
			}

			// width height arwidth arheight
			_ts_snprintf(szReturnValue, TEXT("%d %d %d %d"), lWidth, lHeight, lARWidth, lARHeight);
		}
		break;
		// [NAME] [ID] [PROP]
		case L"author"_hash:
			getProperty(szReturnValue, Properties::PROP_AUTHOR);
			break;
			// [NAME] [ID] [PROP]
		case L"title"_hash:
			getProperty(szReturnValue, Properties::PROP_TITLE);
			break;
			// [NAME] [ID] [PROP]
		case L"description"_hash:
			getProperty(szReturnValue, Properties::PROP_DESCRIPTION);
			break;
			// [NAME] [ID] [PROP]
		case L"copyright"_hash:
			getProperty(szReturnValue, Properties::PROP_COPYRIGHT);
			break;
			// [NAME] [ID] [PROP]
		case L"moreinfo"_hash:
			getProperty(szReturnValue, Properties::PROP_MOREINFO);
			break;
			// [NAME] [ID] [PROP]
		case L"video"_hash:
		{
			VMR9ProcAmpControl amc{};

			if (const auto hr = this->getVideo(&amc); FAILED(hr))
			{
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
			_ts_snprintf(szReturnValue, TEXT("%s %f %f %f %f"), vflags.to_chr(), amc.Brightness, amc.Contrast, amc.Hue, amc.Saturation);
		}
		break;
		// [NAME] [ID] [PROP]
		case L"brange"_hash:
		{
			VMR9ProcAmpControlRange acr{};

			if (const auto hr = this->getVideoRange(ProcAmpControl9_Brightness, &acr); FAILED(hr))
			{
				DX_ERR(prop.to_chr(), nullptr, hr);
				throw Dcx::dcxException("Unable to get Video Information");
			}
			const auto nr = NormalizeValue(acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);

			_ts_snprintf(szReturnValue, TEXT("%f %f %f %f %u %u"), acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize, nr.Value, nr.MaxValue);
			// NB: wnsprintf() doesn't support %f
		}
		break;
		// [NAME] [ID] [PROP]
		case L"crange"_hash:
		{
			VMR9ProcAmpControlRange acr{};

			if (const auto hr = this->getVideoRange(ProcAmpControl9_Contrast, &acr); FAILED(hr))
			{
				DX_ERR(prop.to_chr(), nullptr, hr);
				throw Dcx::dcxException("Unable to get Video Information");
			}

			const auto nr = NormalizeValue(acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);

			_ts_snprintf(szReturnValue, TEXT("%f %f %f %f %u %u"), acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize, nr.Value, nr.MaxValue);
			// NB: wnsprintf() doesn't support %f
		}
		break;
		// [NAME] [ID] [PROP]
		case L"hrange"_hash:
		{
			VMR9ProcAmpControlRange acr{};

			if (const auto hr = this->getVideoRange(ProcAmpControl9_Hue, &acr); FAILED(hr))
			{
				DX_ERR(prop.to_chr(), nullptr, hr);
				throw Dcx::dcxException("Unable to get Video Information");
			}

			const auto nr = NormalizeValue(acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);

			_ts_snprintf(szReturnValue, TEXT("%f %f %f %f %u %u"), acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize, nr.Value, nr.MaxValue);
			// NB: wnsprintf() doesn't support %f
		}
		break;
		// [NAME] [ID] [PROP]
		case L"srange"_hash:
		{
			VMR9ProcAmpControlRange acr{};

			if (const auto hr = getVideoRange(ProcAmpControl9_Saturation, &acr); FAILED(hr))
			{
				DX_ERR(prop.to_chr(), nullptr, hr);
				throw Dcx::dcxException("Unable to get Video Information");
			}

			const auto nr = NormalizeValue(acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize);

			_ts_snprintf(szReturnValue, TEXT("%f %f %f %f %u %u"), acr.DefaultValue, acr.MinValue, acr.MaxValue, acr.StepSize, nr.Value, nr.MaxValue);
			// NB: wnsprintf() doesn't support %f
		}
		break;
		// [NAME] [ID] [PROP]
		case L"currentpos"_hash:
			_ts_snprintf(szReturnValue, TEXT("D_OK %I64u"), getPosition());
			break;
			// [NAME] [ID] [PROP]
		case L"duration"_hash:
		{
			if (this->CheckSeekCapabilities(AM_SEEKING_CanGetDuration) & AM_SEEKING_CanGetDuration)
				_ts_snprintf(szReturnValue, TEXT("D_OK %I64u"), getDuration());
			else
				szReturnValue = TEXT("D_ERROR Method Not Supported");
		}
		break;
		// [NAME] [ID] [PROP]
		case L"volume"_hash:
			_ts_snprintf(szReturnValue, TEXT("D_OK %lf"), getVolume());
			break;
			// [NAME] [ID] [PROP]
		case L"balance"_hash:
			_ts_snprintf(szReturnValue, TEXT("D_OK %ld"), getBalance());
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

			if (!m_pControl)
				throw Dcx::dcxException("No IMediaControl set.");

			OAFilterState pfs = State_Stopped;

			if (const auto hr = this->m_pControl->GetState(1000, &pfs); SUCCEEDED(hr))
			{
				const TCHAR* szState{ nullptr };

				switch (pfs)
				{
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
				_ts_snprintf(szReturnValue, TEXT("D_OK %s"), szState);
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
}

/*!
 * \brief blah
 *
 * blah
 */

GSL_SUPPRESS(type.4)
void DcxDirectshow::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -a [NAME] [ID] [SWITCH] [+FLAGS] [FILE]
	if (flags[TEXT('a')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tsFlags(input.getnexttok().trim());
		const auto tsFilename(input.getlasttoks().trim());

		this->loadFile(tsFlags, tsFilename);
	}
	// xdid -c [NAME] [ID] [SWITCH] [COMMAND]
	else if (flags[TEXT('c')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		if (!this->m_pControl)
			throw Dcx::dcxException("No File Loaded");

		switch (std::hash<TString>{}(input.getnexttok()))
		{
		case TEXT("play"_hash): // play
			m_pControl->Run();
			break;
		case TEXT("pause"_hash): // pause
			m_pControl->Pause();
			break;
		case TEXT("stop"_hash): // stop
		{
			m_pControl->Stop(); // stop play
			setPosition(0);
			m_pControl->StopWhenReady(); // causes new image to be rendered.
		}
		break;
		case TEXT("close"_hash): //close
		{
			m_pControl->Stop();
			ReleaseAll();
		}
		break;
		case TEXT("seek"_hash): // seek
		{
			m_pControl->Pause(); // pause play
			setPosition(input.getnexttokas<UINT64>());	// tok 5
			m_pControl->StopWhenReady(); // causes new image to be rendered.
		}
		break;
		// error
		default:
			throw DcxExceptions::dcxInvalidCommand();
		}
	}
	// xdid -v [NAME] [ID] [SWITCH] [+FLAGS] [BRIGHTNESS] [CONTRAST] [HUE] [SATURATION] [ALPHA]
	else if (flags[TEXT('v')])
	{
		if (numtok < 8)
			throw DcxExceptions::dcxInvalidArguments();

		if (!m_pControl)
			throw Dcx::dcxException("No File Loaded");

		const auto tsFlags(input.getnexttok());
		const auto fBrightness(input.getnexttokas<float>());
		const auto fContrast(input.getnexttokas<float>());
		const auto fHue(input.getnexttokas<float>());
		const auto fSaturation(input.getnexttokas<float>());
		const auto fAlpha(input.getnexttokas<float>());

		if (const auto hr = setVideo(tsFlags, fBrightness, fContrast, fHue, fSaturation, fAlpha); FAILED(hr))
		{
			DX_ERR(nullptr, TEXT("-v"), hr);
			throw Dcx::dcxException("Unable to set video");
		}
	}
	// xdid -V [NAME] [ID] [SWITCH] [+FLAGS] [ARGS]
	else if (flags[TEXT('V')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto flag(input.getnexttok());	// tok 4

		if (flag[0] != TEXT('+'))
			throw DcxExceptions::dcxInvalidFlag();

		if (!this->m_pControl)
			throw Dcx::dcxException("No File Loaded");

		switch (flag[1])
		{
		case TEXT('v'): // Volume
		{
			if (const auto hr = this->setVolume(input.getnexttokas<float>()); FAILED(hr))
			{
				DX_ERR(nullptr, TEXT("-V +v"), hr);
				throw Dcx::dcxException("Unable to Set Volume");
			}
		}
		break;
		case TEXT('b'): // Balance
		{
			if (const auto hr = this->setBalance(input.getnexttokas<long>()); FAILED(hr))
			{
				DX_ERR(nullptr, TEXT("-V +b"), hr);
				throw Dcx::dcxException("Unable to Set Balance");
			}
		}
		break;
		default:
			throw DcxExceptions::dcxInvalidFlag();
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
LRESULT DcxDirectshow::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept
{
	return 0L;
}

LRESULT DcxDirectshow::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{

	LRESULT lRes = 0L;
	switch (uMsg)
	{
	case WM_ERASEBKGND:
	{
		if (RECT rect{}; GetClientRect(m_Hwnd, &rect))
			DcxControl::DrawCtrlBackground(reinterpret_cast<HDC>(wParam), this, &rect);
		bParsed = TRUE;
		return TRUE;
	}
	break;

	case WM_PRINTCLIENT:
	{
		bParsed = TRUE;
		dcxwParam(HDC, hdc);

		if (this->m_pWc)
		{
			// Request the VMR to paint the video.
			this->m_pWc->RepaintVideo(m_Hwnd, hdc);
		}
		else { // There is no video, so paint the whole client area.
			if (RECT rcClient{}; GetClientRect(m_Hwnd, &rcClient))
				DcxControl::DrawCtrlBackground(reinterpret_cast<HDC>(wParam), this, &rcClient);
		}
	}
	break;
	case WM_PAINT:
	{
		bParsed = TRUE;
		PAINTSTRUCT ps{};

		auto hdc = BeginPaint(m_Hwnd, &ps);
		Auto(EndPaint(m_Hwnd, &ps));

		if (hdc)
		{
			if (this->m_pWc)
			{
				// Request the VMR to paint the video.
				this->m_pWc->RepaintVideo(m_Hwnd, hdc);
			}
			else // There is no video, so paint the whole client area.
				DcxControl::DrawCtrlBackground(hdc, this, &ps.rcPaint);
		}
	}
	break;

	case WM_SIZE:
	{
		if (this->m_pWc)
			this->SetVideoPos();
	}
	break;

	case WM_DISPLAYCHANGE:
	{
		if (this->m_pWc)
			this->m_pWc->DisplayModeChanged();
	}
	break;

	case WM_GRAPHNOTIFY:
	{
		bParsed = TRUE;
		if (!this->m_pEvent)
			break;
		// Get all the events
		long evCode{};
		LONG_PTR param1{}, param2{};
		while (SUCCEEDED(this->m_pEvent->GetEvent(&evCode, &param1, &param2, 0)))
		{
			this->m_pEvent->FreeEventParams(evCode, param1, param2);
			switch (evCode)
			{
			case EC_COMPLETE:
			{
				if (this->m_pSeek)
				{
					LONGLONG rtNow{}; // seek to start.
					this->m_pSeek->SetPositions(&rtNow, AM_SEEKING_AbsolutePositioning, nullptr, AM_SEEKING_NoPositioning);
				}
				if (!this->m_bLoop)
				{
					if (this->m_pControl)
						this->m_pControl->StopWhenReady();
					this->execAliasEx(TEXT("dshow,%u,completed"), getUserID());
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
			default:
				break;
			}
		}
	}
	break;

	case WM_DESTROY:
	{
		this->CallDefaultClassProc(uMsg, wParam, lParam);

		delete this;
		bParsed = TRUE;
	}
	break;

	default:
		lRes = this->CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return lRes;
}

wil::com_ptr<IVMRWindowlessControl9> DcxDirectshow::InitWindowlessVMR(const HWND hwndApp, const wil::com_ptr<IGraphBuilder>& pGraph)
{
	if (!pGraph || !hwndApp)
		throw Dcx::dcxException("InitWindowlessVMR() - Invalid Filter Graph Manager");

	HRESULT hr = S_OK;

	try {
		auto pVmr = wil::CoCreateInstance<IBaseFilter>(CLSID_VideoMixingRenderer9, CLSCTX_INPROC);
		if (!pVmr)
			throw Dcx::dcxException("InitWindowlessVMR() - Unable to Create Video Mixing Renderer9");

		// Add the VMR to the filter graph.
		hr = pGraph->AddFilter(pVmr.get(), L"Video Mixing Renderer");	// dont use TEXT() here.
		if (FAILED(hr))
			throw Dcx::dcxException("InitWindowlessVMR() - Unable to Add Filter: Video Mixing Renderer");

		// Set the rendering mode.
		{
			auto pConfig = pVmr.try_query<IVMRFilterConfig9>();
			if (!pConfig)
			{
				hr = E_FAIL;
				throw Dcx::dcxException("InitWindowlessVMR() - Unable to Get Filter Config9");
			}

			hr = pConfig->SetRenderingMode(VMR9Mode_Windowless);
			if (dcx_testflag(dcxGetWindowExStyle(hwndApp), WS_EX_TRANSPARENT))
				hr = pConfig->SetRenderingPrefs(RenderPrefs9_DoNotRenderBorder);

			if (FAILED(hr))
				throw Dcx::dcxException("InitWindowlessVMR() - Unable to Set Rendering Options");
		}
		// Set the window.
		wil::com_ptr<IVMRWindowlessControl9> pRes;
		{
			auto pWc = pVmr.try_query<IVMRWindowlessControl9>();
			if (!pWc)
			{
				hr = E_FAIL;
				throw Dcx::dcxException("InitWindowlessVMR() - Unable to Get Windowless Control9");
			}

			hr = pWc->SetVideoClippingWindow(hwndApp);
			if (FAILED(hr))
			{
				// An error occurred, so release the interface.
				throw Dcx::dcxException("InitWindowlessVMR() - Unable to Set Clipping Window");
			}

			pRes = pWc; // Return this as an AddRef'd pointer.
		}
		return pRes;
	}
	catch (const std::exception)
	{
		DX_ERR(nullptr, TEXT("InitWindowlessVMR"), hr);
		throw;
	}
}

GSL_SUPPRESS(type.4)
HRESULT DcxDirectshow::SetVideoPos(void)
{
	if ((!this->m_pWc) || (!m_Hwnd))
		return E_POINTER;

	long lWidth{}, lHeight{};
	auto hr = this->m_pWc->GetNativeVideoSize(&lWidth, &lHeight, nullptr, nullptr);
	if (SUCCEEDED(hr))
	{
		RECT rcSrc{ 0,0,lWidth,lHeight }, rcDest{};
		// Set the source rectangle.

		// Get the window client area.
		if (!GetClientRect(m_Hwnd, &rcDest))
			return E_FAIL;

		// Set the destination rectangle.
		if (!SetRect(&rcDest, 0, 0, rcDest.right, rcDest.bottom))
			return E_FAIL;

		// Set the video position.
		hr = this->m_pWc->SetVideoPosition(&rcSrc, &rcDest);
	}
	return hr;
}

void DcxDirectshow::ReleaseAll() noexcept
{
	try {
		if (this->m_pEvent)
			this->m_pEvent->SetNotifyWindow(0, 0, 0);
		this->m_pControl.reset();
		this->m_pEvent.reset();
		this->m_pSeek.reset();
		this->m_pWc.reset();
		this->m_pGraph.reset();
	}
	catch (...) {}
	this->m_tsFilename.clear();	// = TEXT("");
}

GSL_SUPPRESS(type.4)
HRESULT DcxDirectshow::getProperty(const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& prop, const Properties type) const noexcept
{
	if (!this->m_pGraph)
		return E_POINTER;

	HRESULT hr{ E_FAIL };

	// Ook: for some reason try_query fails here.
	wil::com_ptr<IAMMediaContent> iam;
	this->m_pGraph->QueryInterface(IID_IAMMediaContent, reinterpret_cast<void**>(&iam));
	//if (auto iam = this->m_pGraph.try_query<IAMMediaContent>(); iam)
	if (iam)
	{
		BSTR com_prop = nullptr;
		switch (type)
		{
		case Properties::PROP_AUTHOR:
			hr = iam->get_AuthorName(&com_prop);
			break;
		case Properties::PROP_TITLE:
			hr = iam->get_Title(&com_prop);
			break;
		case Properties::PROP_RATING:
			hr = iam->get_Rating(&com_prop);
			break;
		case Properties::PROP_DESCRIPTION:
			hr = iam->get_Description(&com_prop);
			break;
		case Properties::PROP_COPYRIGHT:
			hr = iam->get_Copyright(&com_prop);
			break;
		case Properties::PROP_MOREINFO:
			hr = iam->get_MoreInfoText(&com_prop);
			break;
		default:
			break;
		}
		if (SUCCEEDED(hr) && (com_prop))
		{
			_ts_snprintf(prop, TEXT("%ws"), com_prop);
			SysFreeString(com_prop);
		}
		else
			prop = TEXT("Not Supported");
	}
	else
		prop = TEXT("failed");

	return hr;
}

GSL_SUPPRESS(type.4)
HRESULT DcxDirectshow::setAlpha(float alpha)
{
	if (!m_Hwnd || !this->m_pGraph)
		return E_POINTER;

	if ((alpha < 0) || (alpha > 1.0))
		alpha = 1.0;

	RECT rcClient{}, rcWin{};

	if ((!GetClientRect(m_Hwnd, &rcClient)) || (!GetWindowRect(m_Hwnd, &rcWin)))
		return E_FAIL;

	wil::com_ptr<IBaseFilter> pVmr;
	if (const auto hr = this->m_pGraph->FindFilterByName(L"Video Mixing Renderer", &pVmr); !pVmr)
		return hr;

	if (auto pMixer = pVmr.try_query<IVMRMixerControl9>(); pMixer)
	{
		// this works BUT only gives u alpha over other streams in the mixer, not the dialog/controls bg.
		//hr = pMixer->SetAlpha(0,alpha);
		// Get the current mixing preferences.
		DWORD dwPrefs{};
		if (SUCCEEDED(pMixer->GetMixingPrefs(&dwPrefs)))
		{
			// Remove the current render target flag.
			dwPrefs &= ~MixerPref9_RenderTargetMask;

			// Add the render target flag that we want.
			dwPrefs |= MixerPref9_RenderTargetYUV;
			dwPrefs |= MixerPref9_NonSquareMixing;

			// Set the new flags.
			pMixer->SetMixingPrefs(dwPrefs);
		}
	}

	if (auto pBm = pVmr.try_query<IVMRMixerBitmap9>(); pBm)
	{
		if (auto hdc = GetDC(m_Hwnd); hdc)
		{ // make duplicate hdc;
			Auto(ReleaseDC(m_Hwnd, hdc));

			if (const auto hdcBuf = CreateHDCBuffer(hdc, &rcWin); hdcBuf)
			{
				Auto(DeleteHDCBuffer(hdcBuf));

				long cx = 0, cy = 0;

				this->DrawParentsBackground(*hdcBuf, &rcClient);

				if (auto hr = this->m_pWc->GetNativeVideoSize(&cx, &cy, nullptr, nullptr); SUCCEEDED(hr))
				{
					VMR9AlphaBitmap bmpInfo{};
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

					if (hr = pBm->SetAlphaBitmap(&bmpInfo); SUCCEEDED(hr))
					{
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
						return pBm->UpdateAlphaBitmapParameters(&bmpInfo);
					}
				}
			}
		}
	}
	return E_FAIL;
}

float DcxDirectshow::getAlpha() const
{
	if (!m_Hwnd || !this->m_pGraph)
		return 1.0f;

	wil::com_ptr<IBaseFilter> pVmr;
	this->m_pGraph->FindFilterByName(L"Video Mixing Renderer", &pVmr);

	if (!pVmr)
		return 1.0f;

	if (auto pBm = pVmr.try_query<IVMRMixerBitmap9>(); pBm)
	{
		VMR9AlphaBitmap bmpInfo{};

		if (const auto hr = pBm->GetAlphaBitmapParameters(&bmpInfo); SUCCEEDED(hr))
			return bmpInfo.fAlpha;
	}
	return 1.0f;
}

GSL_SUPPRESS(type.4)
HRESULT DcxDirectshow::setVideo(const TString& flags, const float brightness, const float contrast, const float hue, const float saturation, const float alpha)
{
	if (!this->m_pGraph)
		return E_POINTER;

	wil::com_ptr<IBaseFilter> pVmr;
	if (const auto hr = this->m_pGraph->FindFilterByName(L"Video Mixing Renderer", &pVmr); !pVmr)
		return hr;

	auto Mixer = pVmr.try_query<IVMRMixerControl9>();
	if (!Mixer)
		return E_FAIL;

	const XSwitchFlags xflags(flags);
	DWORD dwflags{};

	if (xflags[TEXT('a')])	// set video alpha value.
		setAlpha(alpha);
	if (xflags[TEXT('b')])
		dwflags |= ProcAmpControl9_Brightness;
	if (xflags[TEXT('c')])
		dwflags |= ProcAmpControl9_Contrast;
	if (xflags[TEXT('h')])
		dwflags |= ProcAmpControl9_Hue;
	if (xflags[TEXT('s')])
		dwflags |= ProcAmpControl9_Saturation;

	//if (xflags[TEXT('N')])
	//{
	//	// Inputs are normalized, must be changed to native ranges.
	//	// meaning inputs are 0 - max
	//	auto fNativeBrightness = brightness;
	//}

	if (dwflags != 0)
	{
		//VMR9ProcAmpControl amc{};
		//amc.dwSize = sizeof(VMR9ProcAmpControl);
		//amc.dwFlags = dwflags;
		//amc.Brightness = brightness;
		//amc.Contrast = contrast;
		//amc.Hue = hue;
		//amc.Saturation = saturation;

		VMR9ProcAmpControl amc{ sizeof(VMR9ProcAmpControl), dwflags, brightness, contrast, hue, saturation };

		return Mixer->SetProcAmpControl(0, std::addressof(amc));
	}
	return S_OK;
}

GSL_SUPPRESS(type.4)
HRESULT DcxDirectshow::getVideo(VMR9ProcAmpControl* amc) const
{
	if (!amc || !this->m_pGraph)
		return E_POINTER;

	wil::com_ptr<IBaseFilter> pVmr;
	if (const auto hr = this->m_pGraph->FindFilterByName(L"Video Mixing Renderer", &pVmr); !pVmr)
		return hr;

	auto Mixer = pVmr.try_query<IVMRMixerControl9>();
	if (!Mixer)
		return E_FAIL;

	ZeroMemory(amc, sizeof(VMR9ProcAmpControl));
	amc->dwSize = sizeof(VMR9ProcAmpControl);
	amc->dwFlags = ProcAmpControl9_Brightness | ProcAmpControl9_Contrast | ProcAmpControl9_Hue | ProcAmpControl9_Saturation;

	return Mixer->GetProcAmpControl(0, amc);
}

GSL_SUPPRESS(type.4)
HRESULT DcxDirectshow::getVideoRange(VMR9ProcAmpControlFlags prop, VMR9ProcAmpControlRange* acr) const
{
	if (!this->m_pGraph)
		return E_POINTER;

	wil::com_ptr<IBaseFilter> pVmr;
	if (const auto hr = this->m_pGraph->FindFilterByName(L"Video Mixing Renderer", &pVmr); !pVmr)
		return hr;

	auto Mixer = pVmr.try_query<IVMRMixerControl9>();
	if (!Mixer)
		return E_FAIL;

	ZeroMemory(acr, sizeof(VMR9ProcAmpControlRange));
	acr->dwSize = sizeof(VMR9ProcAmpControlRange);
	acr->dwProperty = prop;
	return Mixer->GetProcAmpControlRange(0, acr);
}

GSL_SUPPRESS(type.4)
std::optional<VMR9ProcAmpControlRange> DcxDirectshow::getVideoRange(VMR9ProcAmpControlFlags prop) const
{
	if (!this->m_pGraph)
		return {};

	wil::com_ptr<IBaseFilter> pVmr;
	this->m_pGraph->FindFilterByName(L"Video Mixing Renderer", &pVmr);

	if (pVmr)
	{
		if (auto pMixer = pVmr.try_query<IVMRMixerControl9>(); pMixer)
		{
			if (VMR9ProcAmpControlRange acr{ sizeof(VMR9ProcAmpControlRange), prop }; SUCCEEDED(pMixer->GetProcAmpControlRange(0, std::addressof(acr))))
				return acr;
		}
	}
	return {};
}

GSL_SUPPRESS(type.4)
UINT64 DcxDirectshow::getPosition() const
{
	if (!this->m_pSeek)
		return 0ULL;
	/*
		* ms-help://MS.MSSDK.1033/MS.WinSDK.1033/directshow/htm/am_seeking_seeking_capabilitiesenumeration.htm
		* Most DirectShow filters do not report the AM_SEEKING_CanGetCurrentPos capability flag.
		*	However, the Filter Graph Manager's implementation of IMediaSeeking::GetCurrentPosition is based on the reference clock,
		*	so you can call this method even if the capabilities flags do not include AM_SEEKING_CanGetCurrentPos.
	*/

	if (UINT64 pos{}; SUCCEEDED(this->m_pSeek->GetCurrentPosition(reinterpret_cast<INT64*>(&pos))))
	{
		// Format result into milliseconds
		return pos / 10000;
	}

	return 0ULL;
}

GSL_SUPPRESS(type.4)
HRESULT DcxDirectshow::setPosition(const UINT64 pos)
{
	if (!this->m_pSeek)
		return E_POINTER;

	UINT64 mpos = pos * 10000; // convert to 100-nano secs units.
	HRESULT hr = E_FAIL;

	if (dcx_testflag(this->CheckSeekCapabilities(AM_SEEKING_CanSeekAbsolute), AM_SEEKING_CanSeekAbsolute))
		hr = this->m_pSeek->SetPositions(reinterpret_cast<LONGLONG*>(&mpos), AM_SEEKING_AbsolutePositioning, nullptr, AM_SEEKING_NoPositioning);

	return hr;
}

GSL_SUPPRESS(type.4)
UINT64 DcxDirectshow::getDuration() const
{
	if (!this->m_pSeek)
		return 0;

	if (dcx_testflag(this->CheckSeekCapabilities(AM_SEEKING_CanGetDuration), AM_SEEKING_CanGetDuration))
	{ // can get current pos
		if (UINT64 pos{}; SUCCEEDED(this->m_pSeek->GetDuration((INT64*)&pos)))
			// Format result into milliseconds
			return pos / 10000;
	}

	return 0;
}

DWORD DcxDirectshow::CheckSeekCapabilities(DWORD dwCaps) const
{
	if (!this->m_pSeek)
		return 0;

	this->m_pSeek->CheckCapabilities(&dwCaps);
	return dwCaps;
}

GSL_SUPPRESS(type.4)
HRESULT DcxDirectshow::setVolume(const float vol)
{
	if ((vol < 0.0) || (vol > 100.0))
		return E_FAIL;

	if (!this->m_pGraph)
		return E_POINTER;

	if (auto myCom = this->m_pGraph.try_query<IBasicAudio>(); myCom)
		return myCom->put_Volume(PercentageToRange(vol));

	return E_FAIL;
}

GSL_SUPPRESS(type.4)
float DcxDirectshow::getVolume() const
{
	float vol{};

	if (!this->m_pGraph)
		return vol;

	if (auto myCom = this->m_pGraph.try_query<IBasicAudio>(); myCom)
	{
#pragma warning(push,3)
#pragma warning(disable:4244)
		if (long t{}; SUCCEEDED(myCom->get_Volume(&t)))
			vol = RangeToPercentage(t);
#pragma warning(pop)
	}
	return vol;
}

GSL_SUPPRESS(type.4)
HRESULT DcxDirectshow::setBalance(const long vol)
{
	if ((vol < -10000) || (vol > 10000))
		return E_FAIL;

	if (!this->m_pGraph)
		return E_POINTER;

	if (auto myCom = this->m_pGraph.try_query<IBasicAudio>(); myCom)
		return myCom->put_Balance(vol);

	return E_FAIL;
}

GSL_SUPPRESS(type.4)
long DcxDirectshow::getBalance(void) const
{
	long vol{};

	if (!this->m_pGraph)
		return vol;

	if (auto myCom = this->m_pGraph.try_query<IBasicAudio>(); myCom)
	{
#pragma warning(push,3)
#pragma warning(disable:4244)
		if (long t{}; SUCCEEDED(myCom->get_Balance(&t)))
			vol = t;
#pragma warning(pop)
	}
	return vol;
}

void DcxDirectshow::loadFile(const TString& tsFlags, const TString& tsFilename)
{
	const XSwitchFlags xflags(tsFlags);
	auto filename(tsFilename);

	this->ReleaseAll();

	if (!Dcx::isDX9Installed())
		Dcx::initDirectX();

	if (!Dcx::isDX9Installed())
		throw Dcx::dcxException("Needs DirectX 9+");

	if (!xflags[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	if (!IsFile(filename))
		throw Dcx::dcxException(TEXT("Unable to Access File: %"), filename);

	HRESULT hr = S_OK;

	try {
		// Create the Filter Graph Manager and query for interfaces.
		//hr = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, reinterpret_cast<void**>(&this->m_pGraph));
		//
		//if (FAILED(hr) || !this->m_pGraph)
		//	throw Dcx::dcxException("Unable to Create FilterGraph");
		//
		//hr = this->m_pGraph->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&this->m_pControl));
		//if (FAILED(hr) || !this->m_pControl)
		//	throw Dcx::dcxException("Unable to Get IMediaControl");
		//
		//hr = this->m_pGraph->QueryInterface(IID_IMediaEventEx, reinterpret_cast<void**>(&this->m_pEvent));
		//if (FAILED(hr) || !this->m_pEvent)
		//	throw Dcx::dcxException("Unable to Get IMediaEventEx");
		//
		//hr = this->m_pGraph->QueryInterface(IID_IMediaSeeking, reinterpret_cast<void**>(&this->m_pSeek));
		//if (FAILED(hr) || !this->m_pSeek)
		//	throw Dcx::dcxException("Unable to Get IMediaSeeking");
		//
		//hr = this->m_pEvent->SetNotifyWindow(reinterpret_cast<OAHWND>(m_Hwnd), WM_GRAPHNOTIFY, 0);
		//if (FAILED(hr))
		//	throw Dcx::dcxException("Unable to Set Window Notify");

		//hr = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, reinterpret_cast<void**>(&this->m_pGraph));
		this->m_pGraph = wil::CoCreateInstance<IGraphBuilder>(CLSID_FilterGraph, CLSCTX_INPROC_SERVER);
		if (!this->m_pGraph)
			throw Dcx::dcxException("Unable to Create FilterGraph");

		this->m_pControl = this->m_pGraph.try_query<IMediaControl>();
		if (!this->m_pControl)
			throw Dcx::dcxException("Unable to Get IMediaControl");

		this->m_pEvent = this->m_pGraph.try_query<IMediaEventEx>();
		if (!this->m_pEvent)
			throw Dcx::dcxException("Unable to Get IMediaEventEx");

		this->m_pSeek = this->m_pGraph.try_query<IMediaSeeking>();
		if (!this->m_pSeek)
			throw Dcx::dcxException("Unable to Get IMediaSeeking");

		hr = this->m_pEvent->SetNotifyWindow(reinterpret_cast<OAHWND>(m_Hwnd), WM_GRAPHNOTIFY, 0);
		if (FAILED(hr))
			throw Dcx::dcxException("Unable to Set Window Notify");

		if (!xflags[TEXT('a')])
		{
			this->m_pWc = DcxDirectshow::InitWindowlessVMR(m_Hwnd, this->m_pGraph);

			if (!m_pWc)
				throw Dcx::dcxException("Unable to Create VMR9");
		}

		if (this->m_pWc)
		{
			if (this->m_bKeepRatio)
				hr = this->m_pWc->SetAspectRatioMode(VMR9ARMode_LetterBox); // caused video to maintain aspect ratio
			else
				hr = this->m_pWc->SetAspectRatioMode(VMR9ARMode_None);

			if (FAILED(hr))
				throw Dcx::dcxException("Unable to Set Aspect");
		}

		hr = this->m_pGraph->RenderFile(filename.to_chr(), nullptr);
		if (FAILED(hr))
		{
			TString tsTmp(gsl::narrow_cast<TString::size_type>(128U));
			this->evalAliasEx(tsTmp.to_chr(), gsl::narrow_cast<int>(tsTmp.capacity_cch()), TEXT("loaderror,%u,%s"), getUserID(), filename.to_chr());
			if (tsTmp == TEXT("$false"))
			{
				this->ReleaseAll();
				return;
			}
			throw Dcx::dcxException("Unable to render file (No codec for file format?)");
		}

		if (this->m_pWc)
		{
			hr = this->SetVideoPos();
			if (FAILED(hr))
				throw Dcx::dcxException("Unable to set Video Position");

			if (this->IsAlphaBlend())
				this->setAlpha(0.5);

			//if (this->IsAlphaBlend())
			//{
			//	const float fAlpha = m_iAlphaLevel / 255.0;
			//	this->setAlpha(fAlpha);
			//}
		}
		else {
			// if VMR == nullptr then disable video.

			auto p_Video = this->m_pGraph.try_query<IVideoWindow>();
			if (!p_Video)
				throw Dcx::dcxException("Unable to get video window");

			p_Video->put_Visible(OAFALSE);
			p_Video->put_AutoShow(OAFALSE);
			p_Video->put_Owner(reinterpret_cast<OAHWND>(m_Hwnd));
			p_Video->put_MessageDrain(reinterpret_cast<OAHWND>(m_Hwnd));
			long styles{};

			hr = p_Video->get_WindowStyle(&styles);
			if (FAILED(hr))
				throw Dcx::dcxException("Unable to get window styles");

			styles &= ~(WS_OVERLAPPEDWINDOW | WS_POPUPWINDOW | WS_DLGFRAME);
			styles |= WS_CHILD;
			p_Video->put_WindowStyle(styles);
			p_Video->put_Left(0);
			p_Video->put_Top(0);
		}

		m_bLoop = xflags[TEXT('l')];

		if (xflags[TEXT('p')])
			m_pControl->Run();

		m_tsFilename = filename;

		InvalidateRect(m_Hwnd, nullptr, TRUE);
	}
	catch (const std::exception)
	{
		DX_ERR(nullptr, TEXT("loadFile()"), hr);
		this->ReleaseAll();
		throw;
	}
}

/// <summary>
/// Normalize a range into a zero range
/// </summary>
/// <param name="fValue">- The current value.</param>
/// <param name="fMin">- The min value allowed.</param>
/// <param name="fMax">- The max value allowed.</param>
/// <param name="fStep">- The amount the value can be changed by (changes must be multiples of this amount).</param>
/// <returns>All values are shifted into a zero range where the min is zero and the step is one.</returns>
NormalizedRange DcxDirectshow::NormalizeValue(float fValue, float fMin, float fMax, float fStep) noexcept
{
	fValue = std::clamp(fValue, fMin, fMax);
	const auto fTotalSteps = (fMax - fMin) / fStep;
	const auto fValueStep = abs(fValue + fMin) / fStep;
	//const auto fValuePrecentage = (fValueStep / fTotalSteps) * 100;

	return { gsl::narrow_cast<UINT>(fValueStep), gsl::narrow_cast<UINT>(fTotalSteps) };
}

void DcxDirectshow::toXml(TiXmlElement* const xml) const
{
	if (!xml)
		return;

	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());
	xml->SetAttribute("src", m_tsFilename.c_str());

	if (this->m_pGraph)
	{
		//fBrightness, fContrast, fHue, fSaturation, fAlpha
		VMR9ProcAmpControl amc{};
		this->getVideo(&amc);

		xml->SetDoubleAttribute("brightness", amc.Brightness);
		xml->SetDoubleAttribute("contrast", amc.Contrast);
		xml->SetDoubleAttribute("hue", amc.Hue);
		xml->SetDoubleAttribute("saturation", amc.Saturation);
		xml->SetDoubleAttribute("videoalpha", this->getAlpha());
		xml->SetAttribute("balance", this->getBalance());
		xml->SetDoubleAttribute("volume", this->getVolume());
		{
			TString tsPos;
			tsPos.addtok(this->getPosition());
			xml->SetAttribute("pos", tsPos.c_str());
		}
	}
}

TiXmlElement* DcxDirectshow::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxDirectshow::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);
	if (const TString tsFilename(queryAttribute(xThis, "src")); !tsFilename.empty())
	{
		const TString tsFlags(queryAttribute(xThis, "flags", "+"));

		this->loadFile(tsFlags, tsFilename);

		if (this->m_pGraph)
		{
			{
				//fBrightness, fContrast, fHue, fSaturation, fAlpha
				TString tsFlags2(L"+");

				float fBrightness{};
				if (xThis->QueryFloatAttribute("brightness", &fBrightness) == TiXmlReturns::TIXML_SUCCESS)
					tsFlags2 += L"b";

				float fContrast{};
				if (xThis->QueryFloatAttribute("contrast", &fContrast) == TiXmlReturns::TIXML_SUCCESS)
					tsFlags2 += L"c";

				float fHue{};
				if (xThis->QueryFloatAttribute("hue", &fHue) == TiXmlReturns::TIXML_SUCCESS)
					tsFlags2 += L"h";

				float fSaturation{};
				if (xThis->QueryFloatAttribute("saturation", &fSaturation) == TiXmlReturns::TIXML_SUCCESS)
					tsFlags2 += L"s";

				float fAlpha{};
				if (xThis->QueryFloatAttribute("videoalpha", &fAlpha) == TiXmlReturns::TIXML_SUCCESS)
					tsFlags2 += L"a";

				if (tsFlags2.len() > 1)
					this->setVideo(tsFlags2, fBrightness, fContrast, fHue, fSaturation, fAlpha);
			}

			//volume
			if (float fVolume{}; xThis->QueryFloatAttribute("volume", &fVolume) == TiXmlReturns::TIXML_SUCCESS)
				this->setVolume(fVolume);

			//pos
			if (const TString tmp(queryAttribute(xThis, "pos")); !tmp.empty())
				this->setPosition(tmp.to_<UINT64>());
		}
	}
}

LRESULT DcxDirectshow::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (!m_Hwnd)
		return 0;

	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
#endif // DCX_USE_DXSDK
