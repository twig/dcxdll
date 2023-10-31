/*!
 * \file dcxwebctrl.h
 * \brief blah
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.0
 *
 * \b Revisions
 *
 * © ScriptsDB.org - 2006
 */

#ifndef _DCXWEBCTRL_H_
#define _DCXWEBCTRL_H_

#include "defines.h"
#include "Dcx.h"
#include "Classes/dcxcontrol.h"
#include <exdisp.h>
#include <exdispid.h>
#include <objbase.h>
#include <mshtml.h>

// WebView2 replace iexplorer embeded control with msft edge one.
//https://go.microsoft.com/fwlink/p/?LinkId=2124703
// https://www.codeproject.com/Tips/5287858/WebView2-Edge-Browser-in-MFC-Cplusplus-Application
//Microsoft.Windows.ImplementationLibrary
//https://github.com/Microsoft/wil

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxWebControl final
	: public DcxControl, public IOleClientSite, public IOleInPlaceSite, public DWebBrowserEvents2
{
public:
	DcxWebControl() = delete;
	DcxWebControl(const DcxWebControl&) = delete;
	GSL_SUPPRESS(c.128) DcxWebControl& operator =(const DcxWebControl&) = delete;	// No assignments!
	DcxWebControl(DcxWebControl&&) = delete;
	GSL_SUPPRESS(c.128) DcxWebControl& operator =(DcxWebControl&&) = delete;

	DcxWebControl(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxWebControl() noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept final;

	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;

	bool InitializeInterface() noexcept;

	// IDispatch Interface (DWebBrowserEvents2)
	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT __RPC_FAR* pctinfo) noexcept override { *pctinfo = 0; return S_OK; }
	HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo __RPC_FAR* __RPC_FAR*) noexcept override { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, LPOLESTR __RPC_FAR*, UINT, LCID, DISPID __RPC_FAR*) noexcept override { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS __RPC_FAR*, VARIANT __RPC_FAR*, EXCEPINFO __RPC_FAR*, UINT __RPC_FAR*) override;

	// IOleClientSite Interface
	HRESULT STDMETHODCALLTYPE SaveObject() noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE GetMoniker(DWORD, DWORD, IMoniker __RPC_FAR* __RPC_FAR*) noexcept override { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer __RPC_FAR* __RPC_FAR*) noexcept override { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE ShowObject() noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL) noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE RequestNewObjectLayout() noexcept override { return S_OK; }

	// IOleInPlaceSite Interface
	HRESULT STDMETHODCALLTYPE CanInPlaceActivate() noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE OnInPlaceActivate() noexcept override { return S_OK; };
	HRESULT STDMETHODCALLTYPE OnUIActivate() noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE Scroll(SIZE) noexcept override { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL) noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate() noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE DiscardUndoState() noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE DeactivateAndUndo() noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT) noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE GetWindowContext(IOleInPlaceFrame __RPC_FAR* __RPC_FAR* ppFrame, IOleInPlaceUIWindow __RPC_FAR* __RPC_FAR* ppDoc, LPRECT pPR, LPRECT pCR, LPOLEINPLACEFRAMEINFO pFI) noexcept override;

	// IOleWindow Interface
	HRESULT STDMETHODCALLTYPE GetWindow(HWND __RPC_FAR* phwnd) noexcept override { *phwnd = m_Hwnd; return S_OK; }
	HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL) noexcept override { return E_NOTIMPL; }

	// IUnknown Interface
	ULONG STDMETHODCALLTYPE AddRef() noexcept override { return 2; }
	ULONG STDMETHODCALLTYPE Release() noexcept override { return 8; }
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR* __RPC_FAR* ppvObject) noexcept override;

	inline const TString getType() const final { return TEXT("webctrl"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::WEBCTRL; }

	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml() const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	//const TString getStyles(void) const final;

	TString getURL() const;
	bool IsStatusbarEnabled() const;
	bool IsFullScreenEnabled() const;
	bool IsAddressbarEnabled() const;
	TString getStatusText() const;

	void setFullScreenState(bool bEnable);
	void setAddressbarState(bool bEnable);
	void setStatusbarState(bool bEnable);
	void setStatusText(const TString& tsText);
	void setURL(const TString& tsURL, const TString &sFlags, const TString &tsMask);

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

private:
	void SafeRelease() noexcept;
	TString CallScript(const TString& tsCmd) const;

	template <typename T>
	void SafeReleaseCom(T* face) const noexcept
	{
		T tmp = *face;
		*face = nullptr;
		try {
			if (tmp)
				tmp->Release();
		}
		catch (...)
		{
		}
	}

	//protected:

	IOleInPlaceObject* m_pOleInPlaceObject{ nullptr };
	IWebBrowser2* m_pWebBrowser2{ nullptr };
	IOleObject* m_pOleObject{ nullptr };
	IConnectionPointContainer* m_pCPC{ nullptr };
	IConnectionPoint* m_pCP{ nullptr };

	DWORD m_dwCookie{};
	bool m_bHideEvents{ true };
};

#endif // _DCXWEBCTRL_H_
