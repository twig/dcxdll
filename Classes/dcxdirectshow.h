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

#include "../defines.h"
#include "dcxcontrol.h"
#include <dshow.h>

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxDirectshow : public DcxControl {

public:

  DcxDirectshow( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles );
  virtual ~DcxDirectshow( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  inline TString getType( ) { return TString( "directshow" ); };

protected:
	static HRESULT InitWindowlessVMR(HWND hwndApp, IGraphBuilder* pGraph, IVMRWindowlessControl** ppWc);
	HRESULT SetVideoPos(void);
	void ReleaseAll(void);
#define WM_GRAPHNOTIFY  WM_APP + 1

	IGraphBuilder					*m_pGraph;
	IMediaControl					*m_pControl;
	IMediaEventEx					*m_pEvent;
	IVMRWindowlessControl *m_pWc;
};

#endif // _DCXDIRECTSHOW_H_