/* \file layoutcellfixed.cpp
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
#include "layoutcellfixed.h"
#include <windowsx.h>

 /*!
  * \brief Constructor
  *
  * blah
  */

LayoutCellFixed::LayoutCellFixed(DcxControl * dcxc, const FixedType nType) noexcept
	: LayoutCell(dcxc)
	, m_nType(nType)
{
}

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCellFixed::LayoutCellFixed(const HWND mHwnd, const FixedType nType) noexcept
	: LayoutCell(mHwnd)
	, m_nType(nType)
{
}

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCellFixed::LayoutCellFixed(const RECT & rc, const FixedType nType) noexcept
	: LayoutCell(nullptr, rc)
	, m_nType(nType)
{
}

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCellFixed::LayoutCellFixed(const HWND mHwnd, const RECT & rc, const FixedType nType) noexcept
	: LayoutCell(mHwnd, rc)
	, m_nType(nType)
{
}

/*!
 * \brief Destructor
 *
 * blah
 */

//LayoutCellFixed::~LayoutCellFixed() noexcept
//{
//}

/*!
 * \brief blah
 *
 * blah
 */

const LayoutCell::CellType LayoutCellFixed::getType() const noexcept
{
	return CellType::FIXED;
}

/*!
 * \brief blah
 *
 * blah
 */

//void LayoutCellFixed::LayoutChild() noexcept
//{
//}


TiXmlElement * LayoutCellFixed::toXml(void)
{
	if (!this->m_BaseControl)
		return nullptr;

	const auto base = this->m_BaseControl->toXml();

	if (base)
	{
		if (dcx_testflag(this->m_nType, FixedType::HEIGHT))
			base->SetAttribute("height", this->m_rcWindow.bottom - this->m_rcWindow.top);
		if (dcx_testflag(this->m_nType, FixedType::WIDTH))
			base->SetAttribute("width", this->m_rcWindow.right - this->m_rcWindow.left);
	}
	return base;
}

void LayoutCellFixed::toXml(TiXmlElement *const xml)
{
	if (this->m_BaseControl)
		this->m_BaseControl->toXml(xml);

	if (dcx_testflag(this->m_nType, FixedType::HEIGHT))
		xml->SetAttribute("height", this->m_rcWindow.bottom - this->m_rcWindow.top);
	if (dcx_testflag(this->m_nType, FixedType::WIDTH))
		xml->SetAttribute("width", this->m_rcWindow.right - this->m_rcWindow.left);
}

/*!
 * \brief blah
 *
 * blah
 */

HDWP LayoutCellFixed::ExecuteLayout(const HDWP hdwp) noexcept
{
	auto hdwpdef = hdwp;

	if (m_Hwnd && IsWindow(m_Hwnd))
	{
		RECT rc{};
		this->getClientRect(rc);

		hdwpdef = DeferWindowPos(hdwpdef, m_Hwnd, nullptr, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
		//hdwpdef = DeferWindowPos( hdwpdef, m_Hwnd, nullptr, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOOWNERZORDER );
	}
	return hdwpdef;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellFixed::getMinMaxInfo(CellMinMaxInfo *const pCMMI) const noexcept
{
	if (!pCMMI)
		return;

	if (this->isVisible())
	{
		RECT rc{};
		getRect(rc);

		//pCMMI->m_MinSize.x = this->m_rcBorders.left + this->m_rcBorders.right;
		//pCMMI->m_MinSize.y = this->m_rcBorders.top + this->m_rcBorders.bottom;

		if (dcx_testflag(m_nType, FixedType::WIDTH))
		{
			if (m_nType != FixedType::BOTH)
				pCMMI->m_MinSize.y = this->m_rcBorders.top + this->m_rcBorders.bottom;

			pCMMI->m_MinSize.x = rc.right - rc.left;
			pCMMI->m_MaxSize.x = pCMMI->m_MinSize.x;

		}

		if (dcx_testflag(m_nType, FixedType::HEIGHT))
		{
			if (m_nType != FixedType::BOTH)
				pCMMI->m_MinSize.x = this->m_rcBorders.left + this->m_rcBorders.right;

			pCMMI->m_MinSize.y = rc.bottom - rc.top;
			pCMMI->m_MaxSize.y = pCMMI->m_MinSize.y;
		}

		pCMMI->m_MinSize.x = std::max(pCMMI->m_MinSize.x, 0L);
		pCMMI->m_MinSize.y = std::max(pCMMI->m_MinSize.y, 0L);
		//pCMMI->m_MaxSize.x = std::min(pCMMI->m_MaxSize.x, gsl::narrow_cast<LONG>(GetSystemMetrics(SM_CXMAXTRACK)));
		//pCMMI->m_MaxSize.y = std::min(pCMMI->m_MaxSize.y, gsl::narrow_cast<LONG>(GetSystemMetrics(SM_CYMAXTRACK)));
		pCMMI->m_MaxSize.x = std::min(pCMMI->m_MaxSize.x, gsl::narrow_cast<LONG>(Dcx::DpiModule.dcxGetWindowMetrics(m_Hwnd, SM_CXMAXTRACK)));
		pCMMI->m_MaxSize.y = std::min(pCMMI->m_MaxSize.y, gsl::narrow_cast<LONG>(Dcx::DpiModule.dcxGetWindowMetrics(m_Hwnd, SM_CYMAXTRACK)));
	}
	else
		ZeroMemory(pCMMI, sizeof(CellMinMaxInfo));
}

