/*!
 * \file layoutcellfixed.h
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

#ifndef _LAYOUTCELLFIXED_H_
#define _LAYOUTCELLFIXED_H_

#include "Classes/layout/layoutcell.h"

/*!
 * \brief blah
 *
 * blah
 */

class LayoutCellFixed : public LayoutCell {

public:

	/*!
	* \brief blah
	*
	* blah
	*/
	enum FixedType: UINT
	{
		HEIGHT=1,         //!< Fixed Size in Height
		WIDTH=HEIGHT<<1,  //!< Fixed Size in Width
		BOTH=HEIGHT|WIDTH //!< Fixed Size in Both
	};

	LayoutCellFixed() = delete;
	LayoutCellFixed(const LayoutCellFixed &) = delete;
	LayoutCellFixed &operator =(const LayoutCellFixed &) = delete;	// No assignments!

	explicit LayoutCellFixed(DcxControl * dcxc, const FixedType nType = BOTH);
	explicit LayoutCellFixed( const HWND mHwnd, const FixedType nType = BOTH );
	explicit LayoutCellFixed(const RECT & rc, const FixedType nType = BOTH);
	LayoutCellFixed(const HWND mHwnd, const RECT & rc, const FixedType nType = BOTH);
	virtual ~LayoutCellFixed( );

	virtual void LayoutChild() final;
	virtual HDWP ExecuteLayout( const HDWP hdwp ) final;
	virtual void getMinMaxInfo( CellMinMaxInfo *const pCMMI ) const final;
	virtual void toXml(TiXmlElement *const xml) final;
	virtual TiXmlElement * toXml(void) final;

	const CellType getType( ) const noexcept final;

protected:

	FixedType m_nType; //!< Fixed Cell Type

};

#endif // _LAYOUTCELLFIXED_H_

