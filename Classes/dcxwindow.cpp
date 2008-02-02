/*!
 * \file dcxwindow.cpp
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

#include "../defines.h"
#include "dcxwindow.h"

/*!
 * \brief Constructor
 *
 * \param mHwnd Window Handle
 * \param mID Window ID
 */

DcxWindow::DcxWindow( const HWND mHwnd, const UINT mID ) : m_Hwnd( mHwnd ), m_ID( mID ) {

}

/*!
 * \brief Constructor
 *
 * \param mID Window ID
 */

DcxWindow::DcxWindow( const UINT mID ) : m_ID( mID ) {

}

/*!
 * \brief Destructor
 *
 * Destructor
 */

DcxWindow::~DcxWindow( ) {


}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxWindow::isStyle( const LONG Styles ) const {

	if ( GetWindowStyle( this->m_Hwnd ) & Styles )
		return TRUE;

	return FALSE;
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::removeStyle( const LONG Styles ) {

	LONG winStyles = GetWindowStyle( this->m_Hwnd );
	return SetWindowLong( this->m_Hwnd, GWL_STYLE, winStyles &= ~Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::addStyle( const LONG Styles ) {

	LONG winStyles = GetWindowStyle( this->m_Hwnd );
	return SetWindowLong( this->m_Hwnd, GWL_STYLE, winStyles |= Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::setStyle( const LONG Styles ) {

	return SetWindowLong( this->m_Hwnd, GWL_STYLE, Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxWindow::isExStyle( const LONG Styles ) const {

	if ( GetWindowExStyle( this->m_Hwnd ) & Styles )
		return TRUE;

	return FALSE;
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::removeExStyle( const LONG Styles ) {

	LONG winStyles = GetWindowExStyle( this->m_Hwnd );
	return SetWindowLong( this->m_Hwnd, GWL_EXSTYLE, winStyles &= ~Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::addExStyle( const LONG Styles ) {

	LONG winStyles = GetWindowExStyle( this->m_Hwnd );
	return SetWindowLong( this->m_Hwnd, GWL_EXSTYLE, winStyles |= Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::setExStyle( const LONG Styles ) {

	return SetWindowLong( this->m_Hwnd, GWL_EXSTYLE, Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxWindow::getID( ) const {

	return this->m_ID;
}

/*!
 * \brief blah
 *
 * blah
 */

HWND DcxWindow::getHwnd( ) const {

	return this->m_Hwnd;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxWindow::redrawWindow( ) {

	RedrawWindow( this->m_Hwnd, NULL, NULL, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE/*|RDW_FRAME|RDW_UPDATENOW*/ );
}

/*
 * Redraw Window using a buffered method to avoid flicker of children etc..
 * Vista Style has issues with this, as it relies on the WM_PAINT, anyway, its not needed for Vista style.
 * Composited windows already have buffered draw.
*/
// TODO: this changes the border style of children upon resize of dialog.
void DcxWindow::redrawBufferedWindow( ) {

	if (this->isExStyle(WS_EX_COMPOSITED)) {
		this->redrawWindow();
		return;
	}

	HDC hdc = GetWindowDC(this->m_Hwnd);

	if (hdc != NULL) {
		RECT rc;
		GetWindowRect(this->m_Hwnd, &rc);

		HDC *hBuffer = CreateHDCBuffer(hdc, &rc);

		if (hBuffer != NULL) {
			SendMessage(this->m_Hwnd, WM_PRINT, (WPARAM)*hBuffer, PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE | PRF_ERASEBKGND);

			BitBlt(hdc, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), *hBuffer, 0,0, SRCCOPY);

			DeleteHDCBuffer(hBuffer);
		}
		ReleaseDC(this->m_Hwnd, hdc);
		ValidateRect(this->m_Hwnd, NULL);
	}
}
