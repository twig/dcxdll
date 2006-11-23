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

#include "../defines.h"
#include "dcxcontrol.h"
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

  DcxWebControl( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxWebControl( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  // IDispatch Interface
  HRESULT STDMETHODCALLTYPE GetTypeInfoCount( UINT __RPC_FAR *pctinfo ) { *pctinfo = NULL; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT,LCID,ITypeInfo __RPC_FAR *__RPC_FAR * ) { return E_NOTIMPL; }
  HRESULT STDMETHODCALLTYPE GetIDsOfNames( REFIID,LPOLESTR __RPC_FAR*,
                                           UINT,LCID,DISPID __RPC_FAR* ) { return E_NOTIMPL; }        
  HRESULT STDMETHODCALLTYPE Invoke( DISPID, REFIID, LCID, WORD, DISPPARAMS __RPC_FAR *, 
                                    VARIANT __RPC_FAR *, EXCEPINFO __RPC_FAR *, UINT __RPC_FAR * );	

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
  HRESULT STDMETHODCALLTYPE GetWindowContext( IOleInPlaceFrame __RPC_FAR *__RPC_FAR*, 
                                              IOleInPlaceUIWindow __RPC_FAR *__RPC_FAR*,
                                              LPRECT, LPRECT, LPOLEINPLACEFRAMEINFO );        

  // IOleWindow Interface
  HRESULT STDMETHODCALLTYPE GetWindow( HWND __RPC_FAR * phwnd ) { *phwnd = this->m_Hwnd; return S_OK; }
  HRESULT STDMETHODCALLTYPE ContextSensitiveHelp( BOOL ) { return E_NOTIMPL; }

  // IUnknown Interface
  ULONG STDMETHODCALLTYPE AddRef( ) { return 2; }
  ULONG STDMETHODCALLTYPE Release( ) { return 8; }
  HRESULT STDMETHODCALLTYPE QueryInterface( REFIID, __RPC_FAR void* __RPC_FAR * );

  inline TString getType( ) { return TString( "webctrl" ); };

protected:

	IOleInPlaceObject	* m_pOleInPlaceObject;
	IWebBrowser2 * m_pWebBrowser2;
	IHTMLWindow2 * m_pIHTMLWindow2;
	IHTMLDocument2 * m_pIHTMLDocument2;
	IOleObject * m_pOleObject;
	IConnectionPointContainer	* m_pCPC;
	IConnectionPoint * m_pCP;
	
	DWORD m_dwCookie;

};

#endif // _DCXWEBCTRL_H_