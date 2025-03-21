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

LayoutCell::LayoutCell() noexcept
	: LayoutCell(HWND())
{
}

LayoutCell::LayoutCell(const HWND mHwnd) noexcept
	: LayoutCell(mHwnd, RECT())
{
	if (m_Hwnd)
	{
		//GetWindowRect(m_Hwnd, &m_rcWindow);

		const Dcx::dcxWindowRect rc(m_Hwnd, GetParent(m_Hwnd));
		m_rcWindow = rc.CopyRect();
	}
}

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
	{
		m_Hwnd = dcxc->getHwnd();

		if (m_Hwnd)
		{
			//GetWindowRect(m_Hwnd, &m_rcWindow);

			const Dcx::dcxWindowRect rc(m_Hwnd, GetParent(m_Hwnd));
			m_rcWindow = rc.CopyRect();
		}
	}
}

void LayoutCell::setParent(LayoutCell* const p_Cell) noexcept
{
	m_Parent = p_Cell;
}

void LayoutCell::setSibling(LayoutCell* const p_Cell) noexcept
{
	m_NextSibling = p_Cell;
}

LayoutCell* LayoutCell::getFirstChild() const noexcept
{
	return m_FirstChild;
}

LayoutCell* LayoutCell::getParent() const noexcept
{
	return m_Parent;
}

LayoutCell* LayoutCell::getNextSibling() const noexcept
{
	return m_NextSibling;
}

bool LayoutCell::setRect(RECT& rc) noexcept
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
		return false;

	m_rcWindow = rc;
	return true;
}

const RECT& LayoutCell::getRect() const noexcept
{
	return m_rcWindow;
}

RECT LayoutCell::getClientRect() const noexcept
{
	RECT rc = getRect();

	rc.left += this->m_rcBorders.left;
	rc.right -= this->m_rcBorders.right;
	rc.top += this->m_rcBorders.top;
	rc.bottom -= this->m_rcBorders.bottom;

	return rc;
}

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

void LayoutCell::setBorder(const int& nBorder) noexcept
{
	const RECT rc{ nBorder, nBorder, nBorder, nBorder };
	setBorder(rc);
}

//void LayoutCell::getBorder(RECT& rc) const noexcept
//{
//	rc = m_rcBorders;
//}

const RECT& LayoutCell::getBorder() const noexcept
{
	return m_rcBorders;
}

const bool LayoutCell::isVisible() const noexcept
{
	return (m_Hwnd == nullptr || (IsWindow(m_Hwnd) && IsWindowVisible(m_Hwnd)));
}
