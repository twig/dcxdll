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

#pragma once

#include <d3d9.h>
#include <Vmr9.h>
#include <Evr.h>

//-----------------------------------------------------------------------------
// BaseVideoRenderer
//
// Abstract class to manage the video renderer filter.
// Specific implementations handle the VMR-7, VMR-9, or EVR filter.
//-----------------------------------------------------------------------------

class BaseVideoRenderer
{
public:
	BaseVideoRenderer() noexcept = default;
	BaseVideoRenderer(const BaseVideoRenderer&) = default;
	BaseVideoRenderer(BaseVideoRenderer&&) noexcept = default;
	virtual ~BaseVideoRenderer() { }
	BaseVideoRenderer& operator =(const BaseVideoRenderer&) = default;
	BaseVideoRenderer& operator =(BaseVideoRenderer&&) noexcept = default;

	virtual BOOL    HasVideo() const = 0;

	virtual HRESULT AddToGraph(IGraphBuilder* pGraph, HWND hwnd) = 0;
	virtual HRESULT FinalizeGraph(IGraphBuilder* pGraph) = 0;

	virtual HRESULT UpdateVideoWindow(HWND hwnd, const LPRECT prc) = 0;
	virtual HRESULT Repaint(HWND hwnd, HDC hdc) = 0;
	virtual HRESULT DisplayModeChanged() = 0;
};


//-----------------------------------------------------------------------------
// VMR7
//
// Manages the VMR-7 video renderer filter.
//-----------------------------------------------------------------------------

class VMR7
	: public BaseVideoRenderer
{
	IVMRWindowlessControl* m_pWindowless{};

public:

	VMR7() noexcept;
	~VMR7();

	BOOL    HasVideo() const noexcept override { return (m_pWindowless != nullptr); }

	HRESULT AddToGraph(IGraphBuilder* pGraph, HWND hwnd) override;
	HRESULT FinalizeGraph(IGraphBuilder* pGraph) override;

	HRESULT UpdateVideoWindow(HWND hwnd, const LPRECT prc) override;
	HRESULT Repaint(HWND hwnd, HDC hdc) override;
	HRESULT DisplayModeChanged() override;
};


//-----------------------------------------------------------------------------
// VMR9
//
// Manages the VMR-9 video renderer filter.
//-----------------------------------------------------------------------------

class VMR9
	: public BaseVideoRenderer
{
	IVMRWindowlessControl9* m_pWindowless{};

public:

	VMR9() noexcept;
	~VMR9();

	BOOL    HasVideo() const noexcept override { return (m_pWindowless != nullptr); }

	HRESULT AddToGraph(IGraphBuilder* pGraph, HWND hwnd) override;
	HRESULT FinalizeGraph(IGraphBuilder* pGraph) override;

	HRESULT UpdateVideoWindow(HWND hwnd, const LPRECT prc) override;
	HRESULT Repaint(HWND hwnd, HDC hdc) override;
	HRESULT DisplayModeChanged() override;
};


//-----------------------------------------------------------------------------
// EVR
//
// Manages the EVR video renderer filter.
//-----------------------------------------------------------------------------

class EVR
	: public BaseVideoRenderer
{
	IBaseFilter* m_pEVR{};
	IMFVideoDisplayControl* m_pVideoDisplay{};

public:

	EVR() noexcept;
	~EVR();

	BOOL    HasVideo() const noexcept override { return (m_pVideoDisplay != nullptr); }

	HRESULT AddToGraph(IGraphBuilder* pGraph, HWND hwnd) override;
	HRESULT FinalizeGraph(IGraphBuilder* pGraph) override;

	HRESULT UpdateVideoWindow(HWND hwnd, const LPRECT prc) override;
	HRESULT Repaint(HWND hwnd, HDC hdc) override;
	HRESULT DisplayModeChanged() override;
};
