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
#include <dshow.h>
#include <initguid.h>
#include <qnetwork.h>
#include <d3d9.h>
#include <vmr9.h>
//#include <DXGIDebug.h> // win8 only

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxDirectshow : public DcxControl {

public:
	DcxDirectshow() = delete;
	DcxDirectshow(const DcxDirectshow &) = delete;
	DcxDirectshow &operator =(const DcxDirectshow &) = delete;	// No assignments!

	DcxDirectshow( const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxDirectshow( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	inline const TString getType() const override { return TEXT("directshow"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::DIRECTSHOW; }

	void toXml(TiXmlElement *const xml) const override;
	TiXmlElement * toXml(void) const override;
	const TString getStyles(void) const override;

protected:
	HRESULT InitWindowlessVMR(const HWND hwndApp, IGraphBuilder* pGraph, IVMRWindowlessControl9** ppWc);
	HRESULT SetVideoPos(void);
	void ReleaseAll(void);

	enum Properties: UINT { PROP_AUTHOR = 0, PROP_TITLE, PROP_RATING, PROP_DESCRIPTION };

	HRESULT getProperty(TCHAR *prop, const int type) const;
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
	IGraphBuilder			*m_pGraph;
	IMediaControl			*m_pControl;
	IMediaEventEx			*m_pEvent;
	IMediaSeeking			*m_pSeek;
	IVMRWindowlessControl9	*m_pWc;

	TString					m_tsFilename;

	bool					m_bKeepRatio;
	bool					m_bLoop;
	bool					m_bReserved[2];
};

#pragma comment(lib, "Strmiids.lib")
#endif // DCX_USE_DXSDK

#endif // _DCXDIRECTSHOW_H_
