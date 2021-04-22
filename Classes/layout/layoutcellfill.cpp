/*!
 * \file layoutcellfill.cpp
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
#include "layoutcellfill.h"
#include <windowsx.h>

 /*!
  * \brief Constructor
  *
  * blah
  */

  //LayoutCellFill::LayoutCellFill()
  //: LayoutCell()
  //{
  //}
  //
  ///*!
  // * \brief Constructor
  // *
  // * blah
  // */
  //
  //LayoutCellFill::LayoutCellFill(const HWND mHwnd)
  //: LayoutCell(mHwnd)
  //{
  //}
  //
  ///*!
  // * \brief Constructor
  // *
  // * blah
  // */
  //
  //LayoutCellFill::LayoutCellFill(DcxControl * dcxc)
  //: LayoutCell(dcxc)
  //{
  //}
  //
  ///*!
  // * \brief Destructor
  // *
  // * blah
  // */
  //
  //LayoutCellFill::~LayoutCellFill() {
  //
  //}

  /*!
   * \brief blah
   *
   * blah
   */

const LayoutCell::CellType LayoutCellFill::getType() const noexcept
{
	return CellType::FILL;
}

/*!
 * \brief blah
 *
 * blah
 */

 //void LayoutCellFill::LayoutChild()
 //{
 //}

 /*!
  * \brief blah
  *
  * blah
  */

HDWP LayoutCellFill::ExecuteLayout(const HDWP hdwp) noexcept
{
	auto hdwpdef = hdwp;

	if (m_Hwnd && IsWindow(m_Hwnd))
	{
		RECT rc{};
		this->getClientRect(rc);

		hdwpdef = DeferWindowPos(hdwpdef, m_Hwnd, nullptr, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
		//hdwpdef = DeferWindowPos( hdwpdef, m_Hwnd, nullptr, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOOWNERZORDER );
		//hdwpdef = DeferWindowPos( hdwpdef, m_Hwnd, nullptr, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
	return hdwpdef;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellFill::getMinMaxInfo(CellMinMaxInfo* const pCMMI) const noexcept
{
	if (this->isVisible())
	{
		pCMMI->m_MinSize.x = this->m_rcBorders.left + this->m_rcBorders.right;
		pCMMI->m_MinSize.y = this->m_rcBorders.top + this->m_rcBorders.bottom;

		pCMMI->m_MinSize.x = std::max(pCMMI->m_MinSize.x, 0L);
		pCMMI->m_MinSize.y = std::max(pCMMI->m_MinSize.y, 0L);
		pCMMI->m_MaxSize.x = std::min(pCMMI->m_MaxSize.x, static_cast<LONG>(GetSystemMetrics(SM_CXMAXTRACK)));
		pCMMI->m_MaxSize.y = std::min(pCMMI->m_MaxSize.y, static_cast<LONG>(GetSystemMetrics(SM_CYMAXTRACK)));
	}
	else
		ZeroMemory(pCMMI, sizeof(CellMinMaxInfo));
}

void LayoutCellFill::toXml(TiXmlElement* const xml)
{
	if (this->m_BaseControl)
		this->m_BaseControl->toXml(xml);

	if (m_rcBorders.top != 0 || m_rcBorders.bottom != 0 || m_rcBorders.left != 0 || m_rcBorders.right != 0)
	{
		TString margin;

		//margin.tsprintf(TEXT("%d %d %d %d"), m_rcBorders.left, m_rcBorders.top, m_rcBorders.right, m_rcBorders.bottom);
		//_ts_snprintf(margin, TEXT("%d %d %d %d"), m_rcBorders.left, m_rcBorders.top, m_rcBorders.right, m_rcBorders.bottom);
		margin.addtok(m_rcBorders.left);
		margin.addtok(m_rcBorders.top);
		margin.addtok(m_rcBorders.right);
		margin.addtok(m_rcBorders.bottom);

		xml->SetAttribute("margin", margin.c_str());
	}
}


TiXmlElement* LayoutCellFill::toXml(void)
{
	if (this->m_BaseControl)
	{
		const auto xml = this->m_BaseControl->toXml();

		if (xml && (m_rcBorders.top != 0 || m_rcBorders.bottom != 0 || m_rcBorders.left != 0 || m_rcBorders.right != 0))
		{
			TString margin;

			//margin.tsprintf(TEXT("%d %d %d %d"), m_rcBorders.left, m_rcBorders.top, m_rcBorders.right, m_rcBorders.bottom);
			//_ts_snprintf(margin, TEXT("%d %d %d %d"), m_rcBorders.left, m_rcBorders.top, m_rcBorders.right, m_rcBorders.bottom);
			margin.addtok(m_rcBorders.left);
			margin.addtok(m_rcBorders.top);
			margin.addtok(m_rcBorders.right);
			margin.addtok(m_rcBorders.bottom);

			xml->SetAttribute("margin", margin.c_str());
		}
		return xml;
	}
	return nullptr;
}
