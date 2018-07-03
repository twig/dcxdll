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

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxWebControl
	: public DcxControl, public IOleClientSite, public IOleInPlaceSite, public DWebBrowserEvents2
{
public:
	DcxWebControl() = delete;
	DcxWebControl(const DcxWebControl &) = delete;
	DcxWebControl &operator =(const DcxWebControl &) = delete;	// No assignments!
	DcxWebControl(DcxWebControl &&) = delete;
	DcxWebControl &operator =(DcxWebControl &&) = delete;

	DcxWebControl(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxWebControl( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) noexcept final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest( const TString & input ) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	// IDispatch Interface
	HRESULT STDMETHODCALLTYPE GetTypeInfoCount( UINT __RPC_FAR *pctinfo ) noexcept override { *pctinfo = NULL; return S_OK; }
	HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT,LCID,ITypeInfo __RPC_FAR *__RPC_FAR * ) noexcept override { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetIDsOfNames( REFIID,LPOLESTR __RPC_FAR*, UINT,LCID,DISPID __RPC_FAR* ) noexcept override { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE Invoke( DISPID, REFIID, LCID, WORD, DISPPARAMS __RPC_FAR *, VARIANT __RPC_FAR *, EXCEPINFO __RPC_FAR *, UINT __RPC_FAR * ) override;

	// IOleClientSite Interface
	HRESULT STDMETHODCALLTYPE SaveObject( ) noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE GetMoniker( DWORD, DWORD, IMoniker __RPC_FAR *__RPC_FAR * ) noexcept override { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetContainer( IOleContainer __RPC_FAR *__RPC_FAR* ) noexcept override { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE ShowObject( ) noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE OnShowWindow( BOOL ) noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE RequestNewObjectLayout( ) noexcept override { return S_OK; }

	// IOleInPlaceSite Interface
	HRESULT STDMETHODCALLTYPE CanInPlaceActivate( ) noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE OnInPlaceActivate( ) noexcept override { return S_OK; };
	HRESULT STDMETHODCALLTYPE OnUIActivate( ) noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE Scroll( SIZE ) noexcept override { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE OnUIDeactivate( BOOL ) noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate( ) noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE DiscardUndoState( ) noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE DeactivateAndUndo( ) noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE OnPosRectChange( LPCRECT ) noexcept override { return S_OK; }
	HRESULT STDMETHODCALLTYPE GetWindowContext( IOleInPlaceFrame __RPC_FAR *__RPC_FAR*, IOleInPlaceUIWindow __RPC_FAR *__RPC_FAR*, LPRECT, LPRECT, LPOLEINPLACEFRAMEINFO ) noexcept override;

	// IOleWindow Interface
	HRESULT STDMETHODCALLTYPE GetWindow( HWND __RPC_FAR * phwnd ) noexcept override { *phwnd = m_Hwnd; return S_OK; }
	HRESULT STDMETHODCALLTYPE ContextSensitiveHelp( BOOL ) noexcept override { return E_NOTIMPL; }

	// IUnknown Interface
	ULONG STDMETHODCALLTYPE AddRef( ) noexcept override { return 2; }
	ULONG STDMETHODCALLTYPE Release( ) noexcept override { return 8; }
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID, __RPC_FAR void* __RPC_FAR * ) noexcept override;

	inline const TString getType() const final { return TEXT("webctrl"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::WEBCTRL; }

	static WNDPROC m_hDefaultClassProc;	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

private:
	void SafeRelease() noexcept;
	TString CallScript(const TString &tsCmd) const;

	template <typename T>
	void SafeReleaseCom(T *face) noexcept
	{
		T tmp = *face;
		*face = nullptr;
		if (tmp != nullptr)
			tmp->Release();
	}

//protected:

	IOleInPlaceObject	* m_pOleInPlaceObject{ nullptr };
	IWebBrowser2 * m_pWebBrowser2{ nullptr };
	IOleObject * m_pOleObject{ nullptr };
	IConnectionPointContainer	* m_pCPC{ nullptr };
	IConnectionPoint * m_pCP{ nullptr };

	DWORD m_dwCookie{};
	bool m_bHideEvents{ true };
	//static IClassFactory * m_pClassFactory;
};

#endif // _DCXWEBCTRL_H_
