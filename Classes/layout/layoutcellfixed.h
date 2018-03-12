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

class LayoutCellFixed
	: public LayoutCell
{
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
	LayoutCellFixed(LayoutCellFixed &&) = delete;
	LayoutCellFixed &operator =(LayoutCellFixed &&) = delete;

	explicit LayoutCellFixed(DcxControl * dcxc, const FixedType nType = BOTH) noexcept;
	explicit LayoutCellFixed( const HWND mHwnd, const FixedType nType = BOTH ) noexcept;
	explicit LayoutCellFixed(const RECT & rc, const FixedType nType = BOTH) noexcept;
	LayoutCellFixed(const HWND mHwnd, const RECT & rc, const FixedType nType = BOTH) noexcept;
	~LayoutCellFixed() noexcept {};

	void LayoutChild() noexcept final {};
	HDWP ExecuteLayout( const HDWP hdwp ) noexcept final;
	void getMinMaxInfo( CellMinMaxInfo *const pCMMI ) const noexcept final;
	void toXml(TiXmlElement *const xml) final;
	TiXmlElement * toXml(void) final;

	const CellType getType( ) const noexcept final;

protected:

	FixedType m_nType; //!< Fixed Cell Type

};

#endif // _LAYOUTCELLFIXED_H_

