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

#include "Classes/layout/layoutcellfill.h"
#include "Classes/layout/layoutcellfixed.h"
#include "Classes/layout/layoutcellpane.h"
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

	LayoutManager &operator =(const LayoutManager &) = delete;	// No assignments!

	const bool updateLayout(RECT & rc);

	void setRoot( LayoutCell * p_Root );
	LayoutCell * getRoot( ) const;

	LayoutCell * getCell( const TString & path ) const;

	void AddCell(const TString &input, const UINT iOffset = 3);

	const size_t &size(void) const noexcept { return this->m_iCount; }
	const bool empty(void) const noexcept { return (this->m_pRoot == nullptr); }

	static LayoutCell * parsePath(const TString & path, const LayoutCell *const hParent, const UINT depth);
	static const UINT parseLayoutFlags(const TString & flags);

	// this is here for refrence only
	//typedef class std::vector<LayoutCell *>::iterator iterator;
	//typedef class std::vector<LayoutCell *>::const_iterator const_iterator;
	//
	//iterator begin() { return m_data.begin(); }
	//const_iterator begin() const { return m_data.begin(); }
	//const_iterator cbegin() const { return m_data.cbegin(); }
	//iterator end() { return m_data.end(); }
	//const_iterator end() const { return m_data.end(); }
	//const_iterator cend() const { return m_data.cend(); }

protected:

	HWND m_Hwnd; //!< Dialog Window Handle

	LayoutCell * m_pRoot; //!< Root LayoutCell Element
	size_t		m_iCount;
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _LAYOUTMANAGER_H_
