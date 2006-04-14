/*!
 * \file divider.cpp
 * \brief Custom Win32 API Divider Control
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

#include "divider.h"
#include "../../defines.h"

/*!
 * \brief blah
 *
 * blah
 */

void InitDivider( HINSTANCE hInstance ) {

  WNDCLASSEX	wc;

	wc.cbSize			    = sizeof( WNDCLASSEX );
	wc.style		  	  = 0;
	wc.lpfnWndProc	  = DividerWndProc;
	wc.cbClsExtra		  = 0;
	wc.cbWndExtra		  = 0;
	wc.hInstance		  = hInstance;
	wc.hIcon			    = NULL;
	wc.hCursor			  = NULL;
	wc.hbrBackground	= (HBRUSH)(COLOR_3DFACE+1);
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= DIVIDERCLASSNAME;
	wc.hIconSm			  = NULL;

	RegisterClassEx( &wc );
}

/*!
 * \brief blah
 *
 * blah
 */

void UnInitDivider( ) {

  UnregisterClass( DIVIDERCLASSNAME, GetModuleHandle( NULL ) );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT CALLBACK DividerWndProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {

  switch( uMsg ) {

    case WM_CREATE:
      {
        
        LPDVCONTROLDATA lpdvdata = new DVCONTROLDATA;
        SetProp( mHwnd, "dvc_data", (HANDLE) lpdvdata );
        ZeroMemory( lpdvdata, sizeof( DVCONTROLDATA ) );

        lpdvdata->m_bDragging = FALSE;
        lpdvdata->m_iLineWidth = 2;
        lpdvdata->m_iBarPos = 100;
        lpdvdata->m_iOldPos = 0;

        lpdvdata->m_Panes[0].cbSize = sizeof( DVPANEINFO );
        lpdvdata->m_Panes[1].cbSize = sizeof( DVPANEINFO );

        /*
       
        HWND hwndChildz1;
        HWND hwndChildz2;
        
        hwndChildz1 = CreateWindowEx(WS_EX_CLIENTEDGE,
          "EDIT", "", 
          WS_VISIBLE|WS_CHILD|ES_MULTILINE|ES_AUTOVSCROLL,
          0,0,0,0,mHwnd, 0, GetModuleHandle( NULL ), 0);		

        hwndChildz2 = CreateWindowEx(WS_EX_CLIENTEDGE,
          "EDIT", "", 
          WS_VISIBLE|WS_CHILD|ES_MULTILINE|ES_AUTOVSCROLL,
          0,0,0,0,mHwnd, 0, GetModuleHandle( NULL ), 0);

        DVPANEINFO dvpi;
        ZeroMemory( &dvpi, sizeof( DVPANEINFO ) );
        dvpi.cbSize = sizeof( DVPANEINFO );
        dvpi.fMask = DVPIM_CHILD | DVPIM_MIN | DVPIM_IDEAL;
        dvpi.cxIdeal = 100;
        dvpi.cxMin = 50;
        dvpi.hChild = hwndChildz1;

        SendMessage( mHwnd, DV_SETPANE, (WPARAM) DVF_PANELEFT, (LPARAM) &dvpi );

        dvpi.hChild = hwndChildz2;

        SendMessage( mHwnd, DV_SETPANE, (WPARAM) DVF_PANERIGHT, (LPARAM) &dvpi );
        
        */

        return 0L;
      }
      break;

    case WM_SIZE:
      {
        Divider_SizeWindowContents( mHwnd, LOWORD(lParam), HIWORD(lParam) );
        return 0L;
      }
      break;

    case WM_LBUTTONDOWN:
      {
        Divider_OnLButtonDown(mHwnd, uMsg, wParam, lParam);
        return 0L;
      }
      break;

    case WM_LBUTTONUP:
      {
        Divider_OnLButtonUp(mHwnd, uMsg, wParam, lParam);
        return 0L;
      }
      break;

    case WM_MOUSEMOVE:
      {
        Divider_OnMouseMove(mHwnd, uMsg, wParam, lParam);
        return 0;
      }
      break;

    case WM_SETCURSOR:
      {
        if ( LOWORD( lParam ) == HTCLIENT && (HWND) wParam == mHwnd ) {

          HCURSOR hCursor;

          if ( GetWindowLong( mHwnd, GWL_STYLE ) & DVS_VERT ) {

            hCursor = LoadCursor( NULL, IDC_SIZEWE );
          }
          else {

            hCursor = LoadCursor( NULL, IDC_SIZENS );
          }
          SetCursor( hCursor );
          return TRUE;
        }
      }
      break;

    // wParam == PANE ID, lParam == pointer to a pane item
    case DV_SETPANE:
      {

        LPDVCONTROLDATA lpdvdata = (LPDVCONTROLDATA) GetProp( mHwnd, "dvc_data" );

        if ( lpdvdata == NULL )
          return FALSE;

        LPDVPANEINFO lpdvpi = (LPDVPANEINFO) lParam;
        
        if ( lpdvpi == NULL )
          return FALSE;

        // Left/Top Pane
        if ( wParam == DVF_PANELEFT ) {

          // Invalid structure size
          if ( lpdvpi->cbSize != lpdvdata->m_Panes[0].cbSize )
            return FALSE;

          //MessageBox( mHwnd, "Received DV_SETPANE", "DV_SETPANE", MB_OK );

          if ( lpdvpi->fMask & DVPIM_CHILD ) {
            lpdvdata->m_Panes[0].hChild = lpdvpi->hChild;
          }
          if ( lpdvpi->fMask & DVPIM_IDEAL ) {
            lpdvdata->m_Panes[0].cxIdeal = lpdvpi->cxIdeal;
          }
          if ( lpdvpi->fMask & DVPIM_MIN ) {
            lpdvdata->m_Panes[0].cxMin = lpdvpi->cxMin;
          }
          if ( lpdvpi->fMask & DVPIM_STYLE ) {
            lpdvdata->m_Panes[0].fStyle = lpdvpi->fStyle;
          }
        }
        // Right/Bottom Pane
        else if ( wParam == DVF_PANERIGHT ) {

          // Invalid structure size
          if ( lpdvpi->cbSize != lpdvdata->m_Panes[1].cbSize )
            return FALSE;

          if ( lpdvpi->fMask & DVPIM_CHILD ) {
            lpdvdata->m_Panes[1].hChild = lpdvpi->hChild;
          }
          if ( lpdvpi->fMask & DVPIM_IDEAL ) {
            lpdvdata->m_Panes[1].cxIdeal = lpdvpi->cxIdeal;
          }
          if ( lpdvpi->fMask & DVPIM_MIN ) {
            lpdvdata->m_Panes[1].cxMin = lpdvpi->cxMin;
          }
          if ( lpdvpi->fMask & DVPIM_STYLE ) {
            lpdvdata->m_Panes[1].fStyle = lpdvpi->fStyle;
          }
        }

        RECT rc;
        GetClientRect( mHwnd, &rc );

        Divider_SizeWindowContents( mHwnd, rc.right - rc.left, rc.bottom - rc.top );

        return TRUE;
      }
      break;

    case DV_GETPANE:
      {

        // Incomplete for now

      }
      break;

    // wParam == NULL, lParam == POSITION VALUE
    case DV_SETDIVPOS:
      {

        LPDVCONTROLDATA lpdvdata = (LPDVCONTROLDATA) GetProp( mHwnd, "dvc_data" );

        RECT rc;
        GetClientRect( mHwnd, &rc );
        UINT iPos = (UINT) lParam;

        UINT width;
        if ( GetWindowLong( mHwnd, GWL_STYLE ) & DVS_VERT )
          width = rc.right - rc.left;
        else
          width = rc.bottom - rc.top;

        /*
        char data[500];
        wsprintf( data, "%d %d %d", rc.right, rc.bottom, width );
        mIRCError( data );
        */

        if ( iPos > lpdvdata->m_Panes[0].cxMin && iPos < ( width - lpdvdata->m_Panes[1].cxMin ) ) {
         
          //mIRCError( "Setting Width" );
          lpdvdata->m_iBarPos = iPos;
          Divider_SizeWindowContents( mHwnd, rc.right - rc.left, rc.bottom - rc.top );
        }
      }
      break;

    case WM_DESTROY:
      {

        LPDVCONTROLDATA lpdvdata = (LPDVCONTROLDATA) GetProp( mHwnd, "dvc_data" );

        // Clear Panes and delete child windows
        // Send Notifications to Parent Window About deletion of child windows

        NMDIVIDER nmdv;
        ZeroMemory( &nmdv, sizeof( NMDIVIDER ) );
        nmdv.hdr.hwndFrom = mHwnd;
        nmdv.hdr.idFrom = GetWindowLong( mHwnd, GWL_ID );
        nmdv.hdr.code = DVN_DELETEDPANE;
        nmdv.fMask = DVNM_LPARAM | DVNM_PANEID;

        nmdv.iPaneId = DVF_PANELEFT;
        nmdv.lParam = lpdvdata->m_Panes[0].lParam;
        SendMessage( GetParent( mHwnd ), WM_NOTIFY, (WPARAM) nmdv.hdr.idFrom, (LPARAM) &nmdv ); 

        nmdv.iPaneId = DVF_PANERIGHT;
        nmdv.lParam = lpdvdata->m_Panes[1].lParam;
        SendMessage( GetParent( mHwnd ), WM_NOTIFY, (WPARAM) nmdv.hdr.idFrom, (LPARAM) &nmdv ); 

        if ( IsWindow( lpdvdata->m_Panes[0].hChild ) )
          DestroyWindow( lpdvdata->m_Panes[0].hChild );

        if ( IsWindow( lpdvdata->m_Panes[1].hChild ) )          
          DestroyWindow( lpdvdata->m_Panes[1].hChild );

        if ( lpdvdata != NULL )
          delete lpdvdata;

        RemoveProp( mHwnd, "dvc_data" );

        return 0L;
      }
      break;

    default:
      break;
  }

  return DefWindowProc( mHwnd, uMsg, wParam, lParam );
}

/*!
 * \brief blah
 *
 * blah
 */

void Divider_SizeWindowContents( HWND mHwnd, int nWidth, int nHeight ) {

  LPDVCONTROLDATA lpdvdata = (LPDVCONTROLDATA) GetProp( mHwnd, "dvc_data" );

  if ( GetWindowLong( mHwnd, GWL_STYLE ) & DVS_VERT ) {

    MoveWindow( lpdvdata->m_Panes[0].hChild, 0, 0, lpdvdata->m_iBarPos, nHeight, TRUE );
    MoveWindow( lpdvdata->m_Panes[1].hChild, lpdvdata->m_iBarPos + lpdvdata->m_iLineWidth, 0, 
      nWidth - lpdvdata->m_iBarPos - lpdvdata->m_iLineWidth, nHeight, TRUE );
  }
  else {

    MoveWindow( lpdvdata->m_Panes[0].hChild, 0, 0, nWidth, lpdvdata->m_iBarPos, TRUE );
    MoveWindow( lpdvdata->m_Panes[1].hChild, 0, lpdvdata->m_iBarPos + lpdvdata->m_iLineWidth, 
      nWidth, nHeight - lpdvdata->m_iBarPos - lpdvdata->m_iLineWidth, TRUE );
  }
}


/*!
 * \brief blah
 *
 * blah
 */

void DrawXorBar( HDC hdc, int x1, int y1, int width, int height )
{
	static WORD _dotPatternBmp[8] = 
	{ 
		0x00aa, 0x0055, 0x00aa, 0x0055, 
		0x00aa, 0x0055, 0x00aa, 0x0055
	};

	HBITMAP hbm;
	HBRUSH  hbr, hbrushOld;

	hbm = CreateBitmap( 8, 8, 1, 1, _dotPatternBmp );
	hbr = CreatePatternBrush(hbm);
	
	SetBrushOrgEx( hdc, x1, y1, 0 );
	hbrushOld = (HBRUSH) SelectObject( hdc, hbr );
	
	PatBlt( hdc, x1, y1, width, height, PATINVERT );
	
	SelectObject ( hdc, hbrushOld );
	
	DeleteObject( hbr );
	DeleteObject( hbm );
}

/*!
 * \brief blah
 *
 * blah
 */

void Divider_CalcBarPos( HWND mHwnd, POINT * pt, RECT * rect ) {

  LPDVCONTROLDATA lpdvdata = (LPDVCONTROLDATA) GetProp( mHwnd, "dvc_data" );

  GetWindowRect( mHwnd, rect );

  ClientToScreen( mHwnd, pt );
  pt->x -= rect->left;
  pt->y -= rect->top;

  OffsetRect( rect, -rect->left, -rect->top );

  if ( GetWindowLong( mHwnd, GWL_STYLE ) & DVS_VERT ) {

    if ( pt->x < (int) lpdvdata->m_Panes[0].cxMin )
      pt->x = (int) lpdvdata->m_Panes[0].cxMin;
    else if ( pt->x > rect->right - (int) lpdvdata->m_Panes[1].cxMin )
      pt->x = rect->right - (int) lpdvdata->m_Panes[1].cxMin;

    if ( pt->x < 3 ) 
      pt->x = 2;
    else if ( pt->x > rect->right - 4 )
      pt->x = rect->right - 4;

  }
  else {

    if ( pt->y < (int) lpdvdata->m_Panes[0].cxMin )
      pt->y = (int) lpdvdata->m_Panes[0].cxMin;
    else if ( pt->y > rect->bottom - (int) lpdvdata->m_Panes[1].cxMin )
      pt->y = rect->bottom - (int) lpdvdata->m_Panes[1].cxMin;

    if ( pt->y < 3 ) 
      pt->y = 2;
    else if ( pt->y > rect->bottom - 4 )
      pt->y = rect->bottom - 4;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT Divider_OnLButtonDown( HWND mHwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
	POINT pt;
	HDC hdc;
	RECT rect;

  LPDVCONTROLDATA lpdvdata = (LPDVCONTROLDATA) GetProp( mHwnd, "dvc_data" );

	pt.x = (short) LOWORD( lParam );  // horizontal position of cursor 
	pt.y = (short) HIWORD( lParam );

  Divider_CalcBarPos( mHwnd, &pt, &rect );

  lpdvdata->m_bDragging = TRUE;

	SetCapture( mHwnd );

	hdc = GetWindowDC( mHwnd );
  if ( GetWindowLong( mHwnd, GWL_STYLE ) & DVS_VERT ) {

    DrawXorBar( hdc, pt.x - 2, 1, 4, rect.bottom - 2 );
    lpdvdata->m_iOldPos = pt.x;
  }
  else {

	  DrawXorBar( hdc, 1, pt.y - 2, rect.right - 2, 4 );
    lpdvdata->m_iOldPos = pt.y;
  }
	ReleaseDC( mHwnd, hdc );
	
	return 0L;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT Divider_OnLButtonUp( HWND mHwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
	HDC hdc;
	RECT rect;
	POINT pt;

  LPDVCONTROLDATA lpdvdata = (LPDVCONTROLDATA) GetProp( mHwnd, "dvc_data" );

	pt.x = (short) LOWORD( lParam );  // horizontal position of cursor 
	pt.y = (short) HIWORD( lParam );

  if ( lpdvdata->m_bDragging == FALSE )
    return 0L;
 
  Divider_CalcBarPos( mHwnd, &pt, &rect );

	hdc = GetWindowDC( mHwnd );

  if ( GetWindowLong( mHwnd, GWL_STYLE ) & DVS_VERT ) {

    DrawXorBar( hdc, lpdvdata->m_iOldPos - 2, 1, 4, rect.bottom - 2 );
    lpdvdata->m_iOldPos = pt.x;
  }
  else {

	  DrawXorBar( hdc, 1, lpdvdata->m_iOldPos - 2, rect.right - 2, 4 );
    lpdvdata->m_iOldPos = pt.y;
  }

	ReleaseDC( mHwnd, hdc );

  lpdvdata->m_bDragging = FALSE;

	//convert the divider position back to screen coords.
	GetWindowRect( mHwnd, &rect );
	pt.x += rect.left;
	pt.y += rect.top;

	//now convert into CLIENT coordinates
	ScreenToClient( mHwnd, &pt );
	GetClientRect( mHwnd, &rect );

  if ( GetWindowLong( mHwnd, GWL_STYLE ) & DVS_VERT )
    lpdvdata->m_iBarPos = pt.x;
  else
    lpdvdata->m_iBarPos = pt.y;
	
  //position the child controls
	Divider_SizeWindowContents( mHwnd, rect.right, rect.bottom );

	ReleaseCapture( );

	return 0L;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT Divider_OnMouseMove( HWND mHwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
	HDC hdc;
	RECT rect;
	POINT pt;

  LPDVCONTROLDATA lpdvdata = (LPDVCONTROLDATA) GetProp( mHwnd, "dvc_data" );

  if ( lpdvdata->m_bDragging == FALSE )
    return 0L;
 
	pt.x = (short) LOWORD( lParam );  // horizontal position of cursor 
	pt.y = (short) HIWORD( lParam );

  Divider_CalcBarPos( mHwnd, &pt, &rect );

  /*
  if( pt.y != lpdvdata->m_iOldPos && wParam & MK_LBUTTON )
  {
  hdc = GetWindowDC( mHwnd );
  */

  if ( GetWindowLong( mHwnd, GWL_STYLE ) & DVS_VERT ) {

    if( pt.x != lpdvdata->m_iOldPos && wParam & MK_LBUTTON )
    {
      hdc = GetWindowDC( mHwnd );

      DrawXorBar( hdc, lpdvdata->m_iOldPos - 2, 1, 4, rect.bottom - 2 );
      DrawXorBar( hdc, pt.x - 2, 1, 4, rect.bottom - 2 );
      lpdvdata->m_iOldPos = pt.x;

      ReleaseDC( mHwnd, hdc );
    }
  }
  else {

    if( pt.y != lpdvdata->m_iOldPos && wParam & MK_LBUTTON ) {

      hdc = GetWindowDC( mHwnd );

      DrawXorBar( hdc, 1, lpdvdata->m_iOldPos - 2, rect.right - 2, 4 );
      DrawXorBar( hdc, 1, pt.y - 2, rect.right - 2, 4 );
      lpdvdata->m_iOldPos = pt.y;

      ReleaseDC( mHwnd, hdc );
    }
  }

  return 0L;
}
