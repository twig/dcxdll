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

class BaseCell
{
public:
	enum class CellType
		: UINT
	{
		FIXED,
		FILL,
		PANE
	};

	BaseCell() noexcept {};
	virtual ~BaseCell() {};
	BaseCell(const BaseCell&) = delete;
	BaseCell& operator =(const BaseCell&) = delete;
	BaseCell(BaseCell&&) = delete;
	BaseCell& operator =(BaseCell&&) = delete;

	virtual bool setRect(RECT& rc) noexcept = 0;
	virtual const RECT& getRect() const noexcept = 0;
	virtual RECT getClientRect() const noexcept = 0;
	virtual void setBorder(const RECT& rc) noexcept = 0;
	virtual void setBorder(const int& nBorder) noexcept = 0;
	virtual void getBorder(RECT& rc) const noexcept = 0;
	virtual const RECT& getBorder() const noexcept = 0;
	virtual void LayoutChild() noexcept = 0;
	virtual HDWP ExecuteLayout(const HDWP hdwp) = 0;
	virtual void getMinMaxInfo(CellMinMaxInfo* const pCMMI) const noexcept = 0;
	virtual void toXml(TiXmlElement* const xml) = 0;
	virtual TiXmlElement* toXml(void) = 0;
	virtual void setSibling(BaseCell* const p_Cell) noexcept = 0;
	virtual void setParent(BaseCell* const p_Cell) noexcept = 0;
	virtual const CellType getType() const noexcept = 0;
	virtual BaseCell* getFirstChild() const noexcept = 0;
	virtual BaseCell* getParent() const noexcept = 0;
	virtual BaseCell* getNextSibling() const noexcept = 0;
	virtual const bool isVisible() const noexcept = 0;
	virtual const size_t& size(void) const noexcept = 0;
	virtual const bool		empty(void) const noexcept = 0;
};
class TestCell
	:BaseCell
{
public:

	TestCell() = default;

	bool operator==(const TestCell& other) const = default;

	const CellType getType() const noexcept override {
		return CellType::FIXED;
	}

};

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

	/// <summary>
	/// Set this cells size.
	/// </summary>
	/// <param name="rc"></param>
	/// <returns></returns>
	bool setRect(RECT& rc) noexcept;

	/// <summary>
	/// Get this cells size.
	/// </summary>
	/// <returns></returns>
	const RECT& getRect() const noexcept;

	/// <summary>
	/// Get cells client area rect (adjusted to included borders)
	/// </summary>
	/// <returns></returns>
	RECT getClientRect() const noexcept;

	/// <summary>
	/// Set this cells border sizes.
	/// </summary>
	/// <param name="rc"></param>
	void setBorder(const RECT& rc) noexcept;

	/// <summary>
	/// Set this cells border size.
	/// </summary>
	/// <param name="nBorder"></param>
	void setBorder(const int& nBorder) noexcept;

	//void getBorder(RECT& rc) const noexcept;

	/// <summary>
	/// Get this cells border sizes.
	/// </summary>
	/// <returns></returns>
	const RECT& getBorder() const noexcept;

	virtual void LayoutChild() noexcept = 0;
	virtual HDWP ExecuteLayout(const HDWP hdwp) = 0;
	virtual void getMinMaxInfo(CellMinMaxInfo* const pCMMI) const noexcept = 0;
	virtual void toXml(TiXmlElement* const xml) = 0;
	virtual TiXmlElement* toXml(void) = 0;

	/// <summary>
	/// Set sibling for this cells.
	/// </summary>
	/// <param name="p_Cell"></param>
	void setSibling(LayoutCell* const p_Cell) noexcept;

	/// <summary>
	/// Set this cells parent cell.
	/// </summary>
	/// <param name="p_Cell"></param>
	void setParent(LayoutCell* const p_Cell) noexcept;

	/// <summary>
	/// Get this cells type.
	/// </summary>
	/// <returns></returns>
	virtual const CellType getType() const noexcept = 0;

	/// <summary>
	/// Get this cells first child cell.
	/// </summary>
	/// <returns></returns>
	LayoutCell* getFirstChild() const noexcept;

	/// <summary>
	/// Get this cells parent cell.
	/// </summary>
	/// <returns></returns>
	LayoutCell* getParent() const noexcept;

	/// <summary>
	/// Get the next sibling.
	/// </summary>
	/// <returns></returns>
	LayoutCell* getNextSibling() const noexcept;

	/// <summary>
	/// Is this cell visible?
	/// </summary>
	/// <returns>true/false</returns>
	const bool isVisible() const noexcept;

	/// <summary>
	/// Get the number of child cells this cell has.
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const size_t& size(void) const noexcept { return m_iCount; }

	/// <summary>
	/// Is this cell empty (has no children).
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const bool		empty(void) const noexcept { return (m_iCount == 0); }

	/// <summary>
	/// Get this cells minimum size.
	/// </summary>
	/// <returns></returns>
	const SIZE& getMinSize() const noexcept { return m_szMin; }

	/// <summary>
	/// Set this cells minimum size.
	/// </summary>
	/// <param name="sz"></param>
	void setMinSize(const SIZE& sz) noexcept { m_szMin = sz; }

protected:

	HWND m_Hwnd{}; //!< Cell Control Window Handle (nullptr if Container Cell)

	RECT m_rcBorders{}; //!< Border defining rectangle
	RECT m_rcWindow{};  //!< Available Window defining rectangle including Borders
	SIZE m_szMin{};		//!< Min Width & height for this element.
	DcxControl* m_BaseControl{ nullptr };

	LayoutCell* m_Parent{ nullptr }; //!< Used for navigation
	LayoutCell* m_FirstChild{ nullptr }; //!< Used for navigation
	LayoutCell* m_NextSibling{ nullptr }; //!< Used for navigation

	size_t		m_iCount{};	//!< Total number of child cells for this cell.
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _LAYOUTCELL_H_
