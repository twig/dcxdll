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

class LayoutCellFixed final
	: public LayoutCell
{
public:

	/*!
	* \brief blah
	*
	* blah
	*/
	enum class FixedType
		: UINT
	{
		HEIGHT=1,         //!< Fixed Size in Height
		WIDTH=HEIGHT<<1,  //!< Fixed Size in Width
		BOTH=HEIGHT|WIDTH //!< Fixed Size in Both
	};
	template <typename T>
	friend constexpr FixedType operator &(const FixedType& eStyle, const T& dStyle) noexcept
	{
		return static_cast<FixedType>(static_cast<DWORD>(eStyle)& static_cast<DWORD>(dStyle));
	}

	LayoutCellFixed() = delete;
	LayoutCellFixed(const LayoutCellFixed &) = delete;
	GSL_SUPPRESS(c.128) LayoutCellFixed &operator =(const LayoutCellFixed &) = delete;	// No assignments!
	LayoutCellFixed(LayoutCellFixed &&) = delete;
	GSL_SUPPRESS(c.128) LayoutCellFixed &operator =(LayoutCellFixed &&) = delete;

	explicit LayoutCellFixed(DcxControl * dcxc, const FixedType nType = FixedType::BOTH) noexcept;
	explicit LayoutCellFixed( const HWND mHwnd, const FixedType nType = FixedType::BOTH ) noexcept;
	explicit LayoutCellFixed(const RECT & rc, const FixedType nType = FixedType::BOTH) noexcept;
	LayoutCellFixed(const HWND mHwnd, const RECT & rc, const FixedType nType = FixedType::BOTH) noexcept;
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

