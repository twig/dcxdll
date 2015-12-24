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
 * � ScriptsDB.org - 2006
 */
#include "defines.h"
#include "layoutcellfixed.h"
#include <windowsx.h>

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCellFixed::LayoutCellFixed(DcxControl * dcxc, const FixedType nType)
: LayoutCell(dcxc)
, m_nType(nType)
{
}

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCellFixed::LayoutCellFixed(const HWND mHwnd, const FixedType nType)
: LayoutCell(mHwnd)
, m_nType(nType)
{
}

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCellFixed::LayoutCellFixed(const RECT & rc, const FixedType nType)
: LayoutCell(nullptr, rc)
, m_nType(nType)
{
}

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCellFixed::LayoutCellFixed(const HWND mHwnd, const RECT & rc, const FixedType nType)
: LayoutCell(mHwnd, rc)
, m_nType(nType)
{
}

/*!
 * \brief Destructor
 *
 * blah
 */

LayoutCellFixed::~LayoutCellFixed() {

}

/*!
 * \brief blah
 *
 * blah
 */

const LayoutCell::CellType LayoutCellFixed::getType() const {

	return FIXED;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellFixed::LayoutChild() {

}


TiXmlElement * LayoutCellFixed::toXml(void) {
	if (this->m_BaseControl == nullptr)
		return nullptr;

	auto base = this->m_BaseControl->toXml();
	if (dcx_testflag(this->m_nType,HEIGHT))
		base->SetAttribute("height", this->m_rcWindow.bottom - this->m_rcWindow.top);
	if (dcx_testflag(this->m_nType,WIDTH))
		base->SetAttribute("width", this->m_rcWindow.right - this->m_rcWindow.left);
	return base;
}

void LayoutCellFixed::toXml(TiXmlElement * xml) {
	if (this->m_BaseControl != nullptr)
		this->m_BaseControl->toXml(xml);
	if (dcx_testflag(this->m_nType, HEIGHT))
		xml->SetAttribute("height", this->m_rcWindow.bottom - this->m_rcWindow.top);
	if (dcx_testflag(this->m_nType, WIDTH))
		xml->SetAttribute("width", this->m_rcWindow.right - this->m_rcWindow.left);
}

/*!
 * \brief blah
 *
 * blah
 */

HDWP LayoutCellFixed::ExecuteLayout(const HDWP hdwp) {

	auto hdwpdef = hdwp;

	if (this->m_Hwnd != nullptr && IsWindow(this->m_Hwnd)) {

		RECT rc;
		this->getClientRect(rc);

		hdwpdef = DeferWindowPos(hdwpdef, this->m_Hwnd, nullptr, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
		//hdwpdef = DeferWindowPos( hdwpdef, this->m_Hwnd, nullptr, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOOWNERZORDER );
	}
	return hdwpdef;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellFixed::getMinMaxInfo(CellMinMaxInfo * pCMMI) const
{

	if (this->isVisible()) {
		RECT rc;
		CopyRect(&rc, &this->m_rcWindow);

		//pCMMI->m_MinSize.x = this->m_rcBorders.left + this->m_rcBorders.right;
		//pCMMI->m_MinSize.y = this->m_rcBorders.top + this->m_rcBorders.bottom;

		if (dcx_testflag(m_nType, WIDTH))
		{
			if (m_nType != BOTH)
				pCMMI->m_MinSize.y = this->m_rcBorders.top + this->m_rcBorders.bottom;

			pCMMI->m_MinSize.x = rc.right - rc.left;
			pCMMI->m_MaxSize.x = pCMMI->m_MinSize.x;

		}

		if (dcx_testflag(m_nType, HEIGHT))
		{
			if (m_nType != BOTH)
				pCMMI->m_MinSize.x = this->m_rcBorders.left + this->m_rcBorders.right;

			pCMMI->m_MinSize.y = rc.bottom - rc.top;
			pCMMI->m_MaxSize.y = pCMMI->m_MinSize.y;
		}

		pCMMI->m_MinSize.x = max(pCMMI->m_MinSize.x, 0);
		pCMMI->m_MinSize.y = max(pCMMI->m_MinSize.y, 0);
		pCMMI->m_MaxSize.x = min(pCMMI->m_MaxSize.x, GetSystemMetrics(SM_CXMAXTRACK));
		pCMMI->m_MaxSize.y = min(pCMMI->m_MaxSize.y, GetSystemMetrics(SM_CYMAXTRACK));
	}
	else
		ZeroMemory(pCMMI, sizeof(CellMinMaxInfo));
}

