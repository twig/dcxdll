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

	virtual bool    HasVideo() const = 0;

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

	VMR7(const VMR7&) = default;
	VMR7(VMR7&&) noexcept = default;
	VMR7& operator =(const VMR7&) = default;
	VMR7& operator =(VMR7&&) noexcept = default;

	bool    HasVideo() const noexcept override { return (m_pWindowless != nullptr); }

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

	VMR9(const VMR9&) = default;
	VMR9(VMR9&&) noexcept = default;
	VMR9& operator =(const VMR9&) = default;
	VMR9& operator =(VMR9&&) noexcept = default;

	bool    HasVideo() const noexcept override { return (m_pWindowless != nullptr); }

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

	EVR(const EVR&) = default;
	EVR(EVR&&) noexcept = default;
	EVR& operator =(const EVR&) = default;
	EVR& operator =(EVR&&) noexcept = default;

	bool    HasVideo() const noexcept override { return (m_pVideoDisplay != nullptr); }

	HRESULT AddToGraph(IGraphBuilder* pGraph, HWND hwnd) override;
	HRESULT FinalizeGraph(IGraphBuilder* pGraph) override;

	HRESULT UpdateVideoWindow(HWND hwnd, const LPRECT prc) override;
	HRESULT Repaint(HWND hwnd, HDC hdc) override;
	HRESULT DisplayModeChanged() override;
};
