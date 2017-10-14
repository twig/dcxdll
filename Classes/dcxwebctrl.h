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

class DcxWebControl : public DcxControl, public IOleClientSite, public IOleInPlaceSite, public DWebBrowserEvents2 {

public:
	DcxWebControl() = delete;
	DcxWebControl(const DcxWebControl &) = delete;
	DcxWebControl &operator =(const DcxWebControl &) = delete;	// No assignments!

	DcxWebControl(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxWebControl( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest( const TString & input ) override;
	//void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;
	std::tuple<NoTheme, WindowStyle, WindowExStyle> parseControlStyles(const TString & tsStyles) override;

	// IDispatch Interface
	HRESULT STDMETHODCALLTYPE GetTypeInfoCount( UINT __RPC_FAR *pctinfo ) { *pctinfo = NULL; return S_OK; }
	HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT,LCID,ITypeInfo __RPC_FAR *__RPC_FAR * ) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetIDsOfNames( REFIID,LPOLESTR __RPC_FAR*, UINT,LCID,DISPID __RPC_FAR* ) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE Invoke( DISPID, REFIID, LCID, WORD, DISPPARAMS __RPC_FAR *, VARIANT __RPC_FAR *, EXCEPINFO __RPC_FAR *, UINT __RPC_FAR * );

	// IOleClientSite Interface
	HRESULT STDMETHODCALLTYPE SaveObject( ) { return S_OK; }
	HRESULT STDMETHODCALLTYPE GetMoniker( DWORD, DWORD, IMoniker __RPC_FAR *__RPC_FAR * ) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetContainer( IOleContainer __RPC_FAR *__RPC_FAR* ) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE ShowObject( ) { return S_OK; }
	HRESULT STDMETHODCALLTYPE OnShowWindow( BOOL ) { return S_OK; }
	HRESULT STDMETHODCALLTYPE RequestNewObjectLayout( ) { return S_OK; }

	// IOleInPlaceSite Interface
	HRESULT STDMETHODCALLTYPE CanInPlaceActivate( ) { return S_OK; }
	HRESULT STDMETHODCALLTYPE OnInPlaceActivate( ) { return S_OK; };
	HRESULT STDMETHODCALLTYPE OnUIActivate( ) { return S_OK; }
	HRESULT STDMETHODCALLTYPE Scroll( SIZE ) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE OnUIDeactivate( BOOL ) { return S_OK; }
	HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate( ) { return S_OK; }
	HRESULT STDMETHODCALLTYPE DiscardUndoState( ) { return S_OK; }
	HRESULT STDMETHODCALLTYPE DeactivateAndUndo( ) { return S_OK; }
	HRESULT STDMETHODCALLTYPE OnPosRectChange( LPCRECT ) { return S_OK; }
	HRESULT STDMETHODCALLTYPE GetWindowContext( IOleInPlaceFrame __RPC_FAR *__RPC_FAR*, IOleInPlaceUIWindow __RPC_FAR *__RPC_FAR*, LPRECT, LPRECT, LPOLEINPLACEFRAMEINFO );

	// IOleWindow Interface
	HRESULT STDMETHODCALLTYPE GetWindow( HWND __RPC_FAR * phwnd ) { *phwnd = m_Hwnd; return S_OK; }
	HRESULT STDMETHODCALLTYPE ContextSensitiveHelp( BOOL ) { return E_NOTIMPL; }

	// IUnknown Interface
	ULONG STDMETHODCALLTYPE AddRef( ) { return 2; }
	ULONG STDMETHODCALLTYPE Release( ) { return 8; }
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID, __RPC_FAR void* __RPC_FAR * );

	inline const TString getType() const override { return TEXT("webctrl"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::WEBCTRL; }

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

	IOleInPlaceObject	* m_pOleInPlaceObject;
	IWebBrowser2 * m_pWebBrowser2;
	IOleObject * m_pOleObject;
	IConnectionPointContainer	* m_pCPC;
	IConnectionPoint * m_pCP;

	DWORD m_dwCookie;
	bool m_bHideEvents;
};

#endif // _DCXWEBCTRL_H_
