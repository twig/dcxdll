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
 * � ScriptsDB.org - 2006
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

typedef struct CellMinMaxInfo
{
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

class LayoutCell
{
public:

	enum class CellType
		: UINT
	{
		FIXED,
		FILL,
		PANE
	};

	LayoutCell(const LayoutCell&) = delete;
	LayoutCell& operator =(const LayoutCell&) = delete;	// No assignments!
	LayoutCell(LayoutCell&&) = delete;
	LayoutCell& operator =(LayoutCell&&) = delete;

	LayoutCell() noexcept;
	explicit LayoutCell(const HWND mHwnd) noexcept;
	LayoutCell(const HWND mHwnd, const RECT& rc) noexcept;
	explicit LayoutCell(DcxControl* dcxc) noexcept;

	virtual ~LayoutCell() noexcept {};

	bool setRect(RECT& rc) noexcept;
	const RECT& getRect() const noexcept;
	RECT getClientRect() const noexcept;
	void setBorder(const RECT& rc) noexcept;
	void setBorder(const int& nBorder) noexcept;
	void getBorder(RECT& rc) const noexcept;
	const RECT& getBorder() const noexcept;

	virtual void LayoutChild() noexcept = 0;
	virtual HDWP ExecuteLayout(const HDWP hdwp) = 0;
	virtual void getMinMaxInfo(CellMinMaxInfo* const pCMMI) const noexcept = 0;
	virtual void toXml(TiXmlElement* const xml) = 0;
	virtual TiXmlElement* toXml(void) = 0;

	void setSibling(LayoutCell* const p_Cell) noexcept;
	void setParent(LayoutCell* const p_Cell) noexcept;

	virtual const CellType getType() const noexcept = 0;

	LayoutCell* getFirstChild() const noexcept;
	LayoutCell* getParent() const noexcept;
	LayoutCell* getNextSibling() const noexcept;

	const bool isVisible() const noexcept;

	const size_t& size(void) const noexcept { return m_iCount; }
	const bool		empty(void) const noexcept { return (m_iCount == 0); }

protected:

	HWND m_Hwnd; //!< Cell Control Window Handle (nullptr if Container Cell)

	RECT m_rcBorders; //!< Border defining rectangle
	RECT m_rcWindow;  //!< Available Window defining rectangle including Borders
	DcxControl* m_BaseControl{ nullptr };

	LayoutCell* m_Parent{ nullptr }; //!< Used for navigation
	LayoutCell* m_FirstChild{ nullptr }; //!< Used for navigation
	LayoutCell* m_NextSibling{ nullptr }; //!< Used for navigation

	size_t		m_iCount{};
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _LAYOUTCELL_H_
