/*!
 * \file layoutcell.h
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

#ifndef _LAYOUTCELL_H_
#define _LAYOUTCELL_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
//#include "defines.h"

#include <vector>

/*!
 * \brief blah
 *
 * blah
 */

typedef struct CellMinMaxInfo {

	POINT m_MinSize; //!< Minimum Cell Size
	POINT m_MaxSize; //!< Maximum Cell Size

} CellMinMaxInfo;

/*!
 * \brief blah
 *
 * blah
 */
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class LayoutCell {

public:

	/*!
	* \brief blah
	*
	* blah
	*/
	enum CellType: UINT {

		FIXED,
		FILL,
		PANE
	};

	LayoutCell( );
	LayoutCell(const LayoutCell &) = delete;
	explicit LayoutCell( const HWND mHwnd );
	LayoutCell( const HWND mHwnd, const RECT & rc );
	explicit LayoutCell( DcxControl * dcxc );
	virtual ~LayoutCell( );

	LayoutCell &operator =(const LayoutCell &) = delete;	// No assignments!

	void setRect(RECT & rc);
	void getRect( RECT & rc ) const;
	void getClientRect( RECT & rc ) const;
	void setBorder( const RECT & rc );
	void setBorder( const UINT nBorder );
	void getBorder( RECT & rc ) const;

	virtual void LayoutChild( ) = 0;
	virtual HDWP ExecuteLayout( const HDWP hdwp ) = 0;
	virtual void getMinMaxInfo( CellMinMaxInfo * pCMMI ) const = 0;
	virtual void toXml(TiXmlElement * xml) = 0;
	virtual TiXmlElement * toXml(void) = 0;

	void setSibling( LayoutCell * p_Cell );
	void setParent( LayoutCell * p_Cell );

	virtual const CellType getType( ) const = 0;

	LayoutCell * getFirstChild( ) const;
	LayoutCell * getParent( ) const;
	LayoutCell * getNextSibling( ) const;

	const bool isVisible( ) const;

	const size_t	&size(void) const noexcept { return this->m_iCount; }
	const bool		empty(void) const noexcept { return (this->m_iCount == 0); }

protected:

	HWND m_Hwnd; //!< Cell Control Window Handle (nullptr if Container Cell)

	RECT m_rcBorders; //!< Border defining rectangle
	RECT m_rcWindow;  //!< Available Window defining rectangle including Borders
	DcxControl * m_BaseControl;

	LayoutCell * m_Parent; //!< Used for navigation
	LayoutCell * m_FirstChild; //!< Used for navigation
	LayoutCell * m_NextSibling; //!< Used for navigation

	size_t		m_iCount;
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _LAYOUTCELL_H_
