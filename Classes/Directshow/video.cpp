//////////////////////////////////////////////////////////////////////////
// 
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#include "defines.h"
#include "video.h"
#include "DshowUtil.h"
#include <Mfidl.h>

HRESULT InitializeEVR(IBaseFilter* pEVR, HWND hwnd, IMFVideoDisplayControl** ppWc);
HRESULT InitWindowlessVMR9(IBaseFilter* pVMR, HWND hwnd, IVMRWindowlessControl9** ppWc);
HRESULT InitWindowlessVMR(IBaseFilter* pVMR, HWND hwnd, IVMRWindowlessControl** ppWc);

HRESULT RemoveUnconnectedRenderer(IGraphBuilder* pGraph, IBaseFilter* pRenderer, BOOL* pbRemoved);

/// VMR-7 Wrapper

VMR7::VMR7() noexcept
	: m_pWindowless(nullptr)
{

}

VMR7::~VMR7()
{
	SAFE_RELEASE(m_pWindowless);
}

HRESULT VMR7::AddToGraph(IGraphBuilder* pGraph, HWND hwnd)
{
	if ((!pGraph) || (!hwnd))
		return E_POINTER;

	HRESULT hr = S_OK;

	IBaseFilter* pVMR = nullptr;

	hr = AddFilterByCLSID(pGraph, CLSID_VideoMixingRenderer, &pVMR, L"VMR-7");

	// Set windowless mode on the VMR. This must be done before the VMR is connected.
	if (SUCCEEDED(hr))
	{
		hr = InitWindowlessVMR(pVMR, hwnd, &m_pWindowless);
	}

	return hr;
}


HRESULT VMR7::FinalizeGraph(IGraphBuilder* pGraph)
{
	if (!pGraph)
		return E_POINTER;

	if (m_pWindowless == nullptr)
		return S_OK;

	HRESULT hr = S_OK;
	BOOL bRemoved = FALSE;

	IBaseFilter* pFilter = nullptr;

	hr = m_pWindowless->QueryInterface(__uuidof(IBaseFilter), (void**)&pFilter);

	if (SUCCEEDED(hr))
	{
		hr = RemoveUnconnectedRenderer(pGraph, pFilter, &bRemoved);

		// If we removed the VMR, then we also need to release our 
		// pointer to the VMR's windowless control interface.
		if (bRemoved)
		{
			SAFE_RELEASE(m_pWindowless);
		}
	}

	SAFE_RELEASE(pFilter);
	return hr;
}

HRESULT VMR7::UpdateVideoWindow(HWND hwnd, const LPRECT prc)
{
	HRESULT hr = S_OK;

	if (m_pWindowless == nullptr)
		return hr; // no-op

	if (!hwnd)
		return E_POINTER;

	if (prc)
	{
		hr = m_pWindowless->SetVideoPosition(nullptr, prc);
	}
	else
	{
		RECT rc;
		GetClientRect(hwnd, &rc);
		hr = m_pWindowless->SetVideoPosition(nullptr, &rc);
	}
	return hr;
}

HRESULT VMR7::Repaint(HWND hwnd, HDC hdc)
{
	if ((!hwnd) || (!hdc))
		return E_POINTER;

	HRESULT hr = S_OK;

	if (m_pWindowless)
	{
		hr = m_pWindowless->RepaintVideo(hwnd, hdc);
	}
	return hr;
}

HRESULT VMR7::DisplayModeChanged()
{
	HRESULT hr = S_OK;

	if (m_pWindowless)
	{
		hr = m_pWindowless->DisplayModeChanged();
	}
	return hr;
}


//-----------------------------------------------------------------------------
// InitWindowlessVMR
// Initialize the VMR-7 for windowless mode. 
//-----------------------------------------------------------------------------

HRESULT InitWindowlessVMR(
	IBaseFilter* pVMR,				// Pointer to the VMR
	HWND hwnd,						// Clipping window
	IVMRWindowlessControl** ppWC	// Receives a pointer to the VMR.
)
{
	if ((!hwnd) || (!pVMR) || (!ppWC))
		return E_POINTER;

	IVMRFilterConfig* pConfig = nullptr;
	IVMRWindowlessControl* pWC = nullptr;

	HRESULT hr = S_OK;

	// Set the rendering mode.  
	hr = pVMR->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig);
	if (SUCCEEDED(hr))
	{
		hr = pConfig->SetRenderingMode(VMRMode_Windowless);
	}

	// Query for the windowless control interface.
	if (SUCCEEDED(hr))
	{
		hr = pVMR->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWC);
	}

	// Set the clipping window.
	if (SUCCEEDED(hr))
	{
		hr = pWC->SetVideoClippingWindow(hwnd);
	}

	// Preserve aspect ratio by letter-boxing
	if (SUCCEEDED(hr))
	{
		hr = pWC->SetAspectRatioMode(VMR_ARMODE_LETTER_BOX);
	}

	// Return the IVMRWindowlessControl pointer to the caller.
	if (SUCCEEDED(hr))
	{
		*ppWC = pWC;
		(*ppWC)->AddRef();
	}

	SAFE_RELEASE(pConfig);
	SAFE_RELEASE(pWC);

	return hr;
}


/// VMR-9 Wrapper

VMR9::VMR9() noexcept
	: m_pWindowless(nullptr)
{

}


VMR9::~VMR9()
{
	SAFE_RELEASE(m_pWindowless);
}

HRESULT VMR9::AddToGraph(IGraphBuilder* pGraph, HWND hwnd)
{
	if ((!pGraph) || (!hwnd))
		return E_POINTER;

	HRESULT hr = S_OK;

	IBaseFilter* pVMR = nullptr;

	hr = AddFilterByCLSID(pGraph, CLSID_VideoMixingRenderer9, &pVMR, L"VMR-9");

	// Set windowless mode on the VMR. This must be done before the VMR is connected.
	if (SUCCEEDED(hr))
	{
		hr = InitWindowlessVMR9(pVMR, hwnd, &m_pWindowless);
	}


	return hr;
}


HRESULT VMR9::FinalizeGraph(IGraphBuilder* pGraph)
{
	if (!pGraph)
		return E_POINTER;

	if (m_pWindowless == nullptr)
		return S_OK;

	HRESULT hr = S_OK;
	BOOL bRemoved = FALSE;

	IBaseFilter* pFilter = nullptr;

	hr = m_pWindowless->QueryInterface(__uuidof(IBaseFilter), (void**)&pFilter);

	if (SUCCEEDED(hr))
	{
		hr = RemoveUnconnectedRenderer(pGraph, pFilter, &bRemoved);

		// If we removed the VMR, then we also need to release our 
		// pointer to the VMR's windowless control interface.
		if (bRemoved)
		{
			SAFE_RELEASE(m_pWindowless);
		}
	}

	SAFE_RELEASE(pFilter);
	return hr;
}




HRESULT VMR9::UpdateVideoWindow(HWND hwnd, const LPRECT prc)
{
	HRESULT hr = S_OK;

	if (m_pWindowless == nullptr)
		return hr; // no-op

	if (!hwnd)
		return E_POINTER;

	if (prc)
	{
		hr = m_pWindowless->SetVideoPosition(nullptr, prc);
	}
	else
	{

		RECT rc;
		GetClientRect(hwnd, &rc);
		hr = m_pWindowless->SetVideoPosition(nullptr, &rc);
	}
	return hr;
}

HRESULT VMR9::Repaint(HWND hwnd, HDC hdc)
{
	HRESULT hr = S_OK;

	if (m_pWindowless)
		hr = m_pWindowless->RepaintVideo(hwnd, hdc);

	return hr;
}

HRESULT VMR9::DisplayModeChanged()
{
	HRESULT hr = S_OK;

	if (m_pWindowless)
	{
		hr = m_pWindowless->DisplayModeChanged();
	}
	return hr;


}


//-----------------------------------------------------------------------------
// InitWindowlessVMR9
// Initialize the VMR-9 for windowless mode. 
//-----------------------------------------------------------------------------

HRESULT InitWindowlessVMR9(
	IBaseFilter* pVMR,				// Pointer to the VMR
	HWND hwnd,						// Clipping window
	IVMRWindowlessControl9** ppWC	// Receives a pointer to the VMR.
)
{
	if ((!hwnd) || (!pVMR) || (!ppWC))
		return E_POINTER;

	IVMRFilterConfig9* pConfig = nullptr;
	IVMRWindowlessControl9* pWC = nullptr;

	HRESULT hr = S_OK;

	// Set the rendering mode.  
	hr = pVMR->QueryInterface(IID_IVMRFilterConfig9, (void**)&pConfig);
	if (SUCCEEDED(hr))
	{
		hr = pConfig->SetRenderingMode(VMR9Mode_Windowless);
	}

	// Query for the windowless control interface.
	if (SUCCEEDED(hr))
	{
		hr = pVMR->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pWC);
	}

	// Set the clipping window.
	if (SUCCEEDED(hr))
	{
		hr = pWC->SetVideoClippingWindow(hwnd);
	}

	// Preserve aspect ratio by letter-boxing
	if (SUCCEEDED(hr))
	{
		hr = pWC->SetAspectRatioMode(VMR9ARMode_LetterBox);
	}

	// Return the IVMRWindowlessControl pointer to the caller.
	if (SUCCEEDED(hr))
	{
		*ppWC = pWC;
		(*ppWC)->AddRef();
	}

	SAFE_RELEASE(pConfig);
	SAFE_RELEASE(pWC);

	return hr;
}





/// EVR Wrapper

EVR::EVR() noexcept
	: m_pEVR(nullptr), m_pVideoDisplay(nullptr)
{

}


EVR::~EVR()
{
	SAFE_RELEASE(m_pEVR);
	SAFE_RELEASE(m_pVideoDisplay);
}

HRESULT EVR::AddToGraph(IGraphBuilder* pGraph, HWND hwnd)
{
	if ((!hwnd) || (!pGraph))
		return E_POINTER;

	HRESULT hr = S_OK;

	IBaseFilter* pEVR = nullptr;

	hr = AddFilterByCLSID(pGraph, CLSID_EnhancedVideoRenderer, &pEVR, L"EVR");

	if (SUCCEEDED(hr))
	{
		hr = InitializeEVR(pEVR, hwnd, &m_pVideoDisplay);
	}

	if (SUCCEEDED(hr))
	{
		// Note: Because IMFVideoDisplayControl is a service interface,
		// you cannot QI the pointer to get back the IBaseFilter pointer.
		// Therefore, we need to cache the IBaseFilter pointer.

		m_pEVR = pEVR;
		m_pEVR->AddRef();
	}

	SAFE_RELEASE(pEVR);

	return hr;
}


HRESULT EVR::FinalizeGraph(IGraphBuilder* pGraph)
{
	if (!pGraph)
		return E_POINTER;

	if (m_pEVR == nullptr)
		return S_OK;

	HRESULT hr = S_OK;
	BOOL bRemoved = FALSE;

	hr = RemoveUnconnectedRenderer(pGraph, m_pEVR, &bRemoved);

	if (bRemoved)
	{
		SAFE_RELEASE(m_pEVR);
		SAFE_RELEASE(m_pVideoDisplay);
	}

	return hr;
}


HRESULT EVR::UpdateVideoWindow(HWND hwnd, const LPRECT prc)
{
	HRESULT hr = S_OK;

	if (m_pVideoDisplay == nullptr)
		return hr; // no-op

	if (!hwnd)
		return E_POINTER;

	if (prc)
	{
		hr = m_pVideoDisplay->SetVideoPosition(nullptr, prc);
	}
	else
	{

		RECT rc;
		GetClientRect(hwnd, &rc);
		hr = m_pVideoDisplay->SetVideoPosition(nullptr, &rc);
	}
	return hr;
}

HRESULT EVR::Repaint(HWND hwnd, HDC hdc)
{
	HRESULT hr = S_OK;

	if (m_pVideoDisplay)
		hr = m_pVideoDisplay->RepaintVideo();

	return hr;
}

HRESULT EVR::DisplayModeChanged()
{
	return S_OK; // No-op
}


//-----------------------------------------------------------------------------
// InitializeEVR
// Initialize the EVR filter. 
//-----------------------------------------------------------------------------

HRESULT InitializeEVR(
	IBaseFilter* pEVR,				// Pointer to the EVR
	HWND hwnd,						// Clipping window
	IMFVideoDisplayControl** ppDisplayControl
)
{
	if ((!pEVR) || (!ppDisplayControl) || (!hwnd))
		return E_POINTER;

	HRESULT hr = S_OK;

	IMFGetService* pService = nullptr;
	IMFVideoDisplayControl* pDisplay = nullptr;

	hr = pEVR->QueryInterface(__uuidof(IMFGetService), (void**)&pService);
	if (SUCCEEDED(hr))
	{
		hr = pService->GetService(
			MR_VIDEO_RENDER_SERVICE,
			__uuidof(IMFVideoDisplayControl),
			(void**)&pDisplay
		);
	}

	// Set the clipping window.
	if (SUCCEEDED(hr))
	{
		hr = pDisplay->SetVideoWindow(hwnd);
	}

	// Preserve aspect ratio by letter-boxing
	if (SUCCEEDED(hr))
	{
		hr = pDisplay->SetAspectRatioMode(MFVideoARMode_PreservePicture);
	}

	// Return the IMFVideoDisplayControl pointer to the caller.
	if (SUCCEEDED(hr))
	{
		*ppDisplayControl = pDisplay;
		(*ppDisplayControl)->AddRef();
	}

	SAFE_RELEASE(pService);
	SAFE_RELEASE(pDisplay);

	return hr;
}

//-----------------------------------------------------------------------------
// DShowPlayer::RemoveUnconnectedRenderer
// Description: Remove a renderer filter from the graph if the filter is
//              not connected. 
//-----------------------------------------------------------------------------

HRESULT RemoveUnconnectedRenderer(IGraphBuilder* pGraph, IBaseFilter* pRenderer, BOOL* pbRemoved)
{
	if ((!pGraph) || (!pRenderer) || (!pbRemoved))
		return E_POINTER;

	IPin* pPin = nullptr;

	BOOL bRemoved = FALSE;

	// Look for a connected input pin on the renderer.

	HRESULT hr = FindConnectedPin(pRenderer, PINDIR_INPUT, &pPin);
	SAFE_RELEASE(pPin);

	// If this function succeeds, the renderer is connected, so we don't remove it.
	// If it fails, it means the renderer is not connected to anything, so
	// we remove it.

	if (FAILED(hr))
	{
		hr = pGraph->RemoveFilter(pRenderer);
		bRemoved = TRUE;
	}

	if (SUCCEEDED(hr))
	{
		*pbRemoved = bRemoved;
	}

	return hr;
}
