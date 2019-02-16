/*!
 * \file dcxdirectshow.h
 * \brief blah
 *
 * blah
 *
 * \author Ook
 * \version 1.0
 *
 * \b Revisions
 *
 * © ScriptsDB.org - 2006
 */

#ifndef _DCXDIRECTSHOW_H_
#define _DCXDIRECTSHOW_H_

#include "defines.h"

#ifdef DCX_USE_DXSDK
#include "Classes/dcxcontrol.h"
//#include <dshow.h>
//#include <initguid.h>
//#include <qnetwork.h>
//#include <d3d9.h>
//#include <vmr9.h>
//#include <DXGIDebug.h> // win8 only

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxDirectshow
	: public DcxControl
{
public:
	DcxDirectshow() = delete;
	DcxDirectshow(const DcxDirectshow &) = delete;
	DcxDirectshow &operator =(const DcxDirectshow &) = delete;	// No assignments!
	DcxDirectshow(DcxDirectshow &&) = delete;
	DcxDirectshow &operator =(DcxDirectshow &&) = delete;

	DcxDirectshow( const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxDirectshow( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) noexcept final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("directshow"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::DIRECTSHOW; }

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	const TString getStyles(void) const final;

	static WNDPROC m_hDefaultClassProc;	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

protected:
	HRESULT InitWindowlessVMR(const HWND hwndApp, IGraphBuilder* pGraph, IVMRWindowlessControl9** ppWc);
	HRESULT SetVideoPos(void);
	void ReleaseAll(void);

	enum Properties: UINT { PROP_AUTHOR = 0, PROP_TITLE, PROP_RATING, PROP_DESCRIPTION };

	HRESULT getProperty(const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &prop, const int type) const;
	HRESULT setAlpha(float alpha);
	HRESULT setVideo(const TString &flags, const float brightness, const float contrast, const float hue, const float saturation);
	HRESULT getVideo(VMR9ProcAmpControl *amc) const;
	HRESULT getVideoRange(VMR9ProcAmpControlFlags prop, VMR9ProcAmpControlRange *acr) const;
	UINT64 getPosition(void) const; //!< gets video position within stream
	HRESULT setPosition(const UINT64 pos); //!< sets the video position to x .
	UINT64 getDuration(void) const; //!< gets video duration.
	DWORD CheckSeekCapabilities(DWORD dwCaps) const;
	HRESULT setVolume(const long vol);
	long getVolume(void) const;

#define WM_GRAPHNOTIFY  WM_APP + 1

private:
	IGraphBuilder			* m_pGraph{ nullptr };
	IMediaControl			*m_pControl{ nullptr };
	IMediaEventEx			*m_pEvent{ nullptr };
	IMediaSeeking			*m_pSeek{ nullptr };
	IVMRWindowlessControl9	*m_pWc{ nullptr };

	TString					m_tsFilename;

	bool					m_bKeepRatio{ false };
	bool					m_bLoop{ false };
	bool					m_bReserved[2]{ false };
};

#pragma comment(lib, "Strmiids.lib")
#endif // DCX_USE_DXSDK

#endif // _DCXDIRECTSHOW_H_
