/*!
 * \file layoutmanager.h
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

#ifndef _LAYOUTMANAGER_H_
#define _LAYOUTMANAGER_H_

#include "Classes/layout/layoutcell.h"
#include "Classes/tstring/tstring.h"

/*!
 * \brief blah
 *
 * blah
 */
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class LayoutManager {

public:

	LayoutManager( );
	explicit LayoutManager( HWND mHwnd );
	virtual ~LayoutManager( );

	BOOL updateLayout( RECT & rc );

	void setRoot( LayoutCell * p_Root );
	LayoutCell * getRoot( ) const;

	LayoutCell * getCell( const TString & path ) const;

	static LayoutCell * parsePath( const TString & path, LayoutCell * hParent, const int depth );

protected:

	HWND m_Hwnd; //!< Dialog Window Handle

	LayoutCell * m_pRoot; //!< Root LayoutCell Element

};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _LAYOUTMANAGER_H_
