/*!
 * \file layoutcell.cpp
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
#include "defines.h"
#include "Classes/layout/layoutcell.h"
#include "Classes/dcxdialog.h"
#include "Dcx.h"

 /*!
  * \brief Constructor
  *
  * blah
  */

  //LayoutCell::LayoutCell()
  //: m_Hwnd(nullptr)
  //, m_Parent(nullptr)
  //, m_FirstChild(nullptr)
  //, m_NextSibling(nullptr)
  //, m_BaseControl(nullptr)
  //, m_iCount(0)
  //{
  //	SetRectEmpty(&this->m_rcBorders);
  //	SetRectEmpty(&this->m_rcWindow);
  //}

LayoutCell::LayoutCell() noexcept
	: LayoutCell(HWND())
{
}

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCell::LayoutCell(const HWND mHwnd) noexcept
	: LayoutCell(mHwnd, RECT())
{
	if (m_Hwnd)
		GetWindowRect(m_Hwnd, &m_rcWindow);
}

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCell::LayoutCell(const HWND mHwnd, const RECT& rc) noexcept
	: m_Hwnd(mHwnd)
	, m_rcWindow(rc)
{
	SetRectEmpty(&m_rcBorders);

	if (mHwnd)
	{
		if (Dcx::Dialogs.getDialogByHandle(mHwnd) == nullptr)
		{
			if (const auto* const d = Dcx::Dialogs.getDialogByChildHandle(mHwnd); d)
				m_BaseControl = d->getControlByHWND(mHwnd);
		}
	}
}

LayoutCell::LayoutCell(DcxControl* dcxc) noexcept
	: LayoutCell()
{
	m_BaseControl = dcxc;

	if (dcxc)
		m_Hwnd = dcxc->getHwnd();

	if (m_Hwnd)
		GetWindowRect(m_Hwnd, &m_rcWindow);
}
/*!
 * \brief Destructor
 *
 * blah
 */

 //LayoutCell::~LayoutCell() noexcept
 //{
 //}

 /*!
  * \brief blah
  *
  * blah
  */

void LayoutCell::setParent(LayoutCell* const p_Cell) noexcept
{
	m_Parent = p_Cell;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCell::setSibling(LayoutCell* const p_Cell) noexcept
{
	m_NextSibling = p_Cell;
}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell* LayoutCell::getFirstChild() const noexcept
{
	return m_FirstChild;
}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell* LayoutCell::getParent() const noexcept
{
	return m_Parent;
}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell* LayoutCell::getNextSibling() const noexcept
{
	return m_NextSibling;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCell::setRect(RECT& rc) noexcept
{
	CellMinMaxInfo cmmi{ {0,0},{rc.right - rc.left,rc.bottom - rc.top} };

	//CellMinMaxInfo cmmi;
	//cmmi.m_MinSize.x = 0;
	//cmmi.m_MinSize.y = 0;
	//cmmi.m_MaxSize.x = rc.right - rc.left;
	//cmmi.m_MaxSize.y = rc.bottom - rc.top;

	getMinMaxInfo(&cmmi);

	if (rc.right - rc.left < cmmi.m_MinSize.x)
		rc.right = rc.left + cmmi.m_MinSize.x;
	if (rc.right - rc.left > cmmi.m_MaxSize.x)
		rc.right = rc.left + cmmi.m_MaxSize.x;
	if (rc.bottom - rc.top < cmmi.m_MinSize.y)
		rc.bottom = rc.top + cmmi.m_MinSize.y;
	if (rc.bottom - rc.top > cmmi.m_MaxSize.y)
		rc.bottom = rc.top + cmmi.m_MaxSize.y;

	if (EqualRect(&rc, &m_rcWindow))
		return;

	m_rcWindow = rc;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCell::getRect(RECT& rc) const noexcept
{
	rc = m_rcWindow;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCell::getClientRect(RECT& rc) const noexcept
{
	getRect(rc);

	rc.left += this->m_rcBorders.left;
	rc.right -= this->m_rcBorders.right;
	rc.top += this->m_rcBorders.top;
	rc.bottom -= this->m_rcBorders.bottom;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCell::setBorder(const RECT& rc) noexcept
{
	// remove old borders
	this->m_rcWindow.right -= this->m_rcBorders.left + this->m_rcBorders.right;
	this->m_rcWindow.bottom -= this->m_rcBorders.top + this->m_rcBorders.bottom;

	// new borders
	CopyRect(&this->m_rcBorders, &rc);

	// re-add new border settings
	this->m_rcWindow.right += this->m_rcBorders.left + this->m_rcBorders.right;
	this->m_rcWindow.bottom += this->m_rcBorders.top + this->m_rcBorders.bottom;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCell::setBorder(const int& nBorder) noexcept
{
	const RECT rc{ nBorder, nBorder, nBorder, nBorder };
	setBorder(rc);
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCell::getBorder(RECT& rc) const noexcept
{
	rc = m_rcBorders;
}

/*!
 * \brief blah
 *
 * blah
 */

const bool LayoutCell::isVisible() const noexcept
{
	return (m_Hwnd == nullptr || (IsWindow(m_Hwnd) && IsWindowVisible(m_Hwnd)));
}
