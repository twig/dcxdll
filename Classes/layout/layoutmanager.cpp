/*!
 * \file layoutmanager.cpp
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
#include "layoutmanager.h"

LayoutManager::LayoutManager(HWND mHwnd) noexcept
	: m_Hwnd(mHwnd)
{
}

const bool LayoutManager::updateLayout(RECT& rc)
{
	//if (empty())
	//	return false;
	//
	//auto hdwp = BeginDeferWindowPos(gsl::narrow_cast<int>(m_iCount));
	//
	//if (hdwp == nullptr)
	//{
	//	DCX_DEBUG(mIRCLinker::debug, TEXT("updateLayout()"), TEXT("BeginDeferWindowPos() failed"));
	//	return false;
	//}
	//
	//m_pRoot->setRect(rc);
	//m_pRoot->LayoutChild();
	//hdwp = m_pRoot->ExecuteLayout(hdwp);
	//
	//if (hdwp == nullptr)
	//{
	//	DCX_DEBUG(mIRCLinker::debug, TEXT("updateLayout()"), TEXT("DeferWindowPos() failed"));
	//	return false;
	//}
	//return (EndDeferWindowPos(hdwp) != FALSE);

	if (empty())
		return false;

	//if (empty())
	//{
	//	TileWindows(m_Hwnd, MDITILE_HORIZONTAL, &rc, 0, nullptr);
	//	return false;
	//}

	//m_pRoot->setRect(rc);
	//m_pRoot->LayoutChild();
	//return (EndDeferWindowPos(m_pRoot->ExecuteLayout(BeginDeferWindowPos(gsl::narrow_cast<int>(m_iCount)))) != FALSE);

	const auto bRedraw = m_pRoot->setRect(rc);
	m_pRoot->LayoutChild();
	std::ignore = (EndDeferWindowPos(m_pRoot->ExecuteLayout(BeginDeferWindowPos(gsl::narrow_cast<int>(m_iCount)))) != FALSE);
	return bRedraw;
}

void LayoutManager::setRoot(std::unique_ptr<LayoutCell> p_Root) noexcept
{
	// clean memory in case we use more than once
	//delete m_pRoot;
	//m_pRoot = p_Root;

	m_pRoot = std::move(p_Root);
}

LayoutCell* LayoutManager::getCell(const TString& path) const
{
	return parsePath(path, m_pRoot.get(), 1);
}

LayoutCell* LayoutManager::parsePath(const TString& path, const LayoutCell* const hParent, const UINT depth)
{
	if (!hParent)
		return nullptr;

	const auto k = path.gettok(gsl::narrow_cast<int>(depth)).to_<UINT>();

	if (k < 1)
		return nullptr;

	auto hCurrentCell = hParent->getFirstChild();

	if (!hCurrentCell)
		return nullptr;

	UINT i = 1;
	const auto n = path.numtok();

	do {
		if (i == k)
		{
			// finished tree search and found
			if (depth == n)
				return hCurrentCell;

			return LayoutManager::parsePath(path, hCurrentCell, depth + 1);
		}

		++i;

	} while ((hCurrentCell = hCurrentCell->getNextSibling()) != nullptr);

	return nullptr;
}

void LayoutManager::AddCell(const TString& input, const UINT iOffset, const DcxDialog * dialog)
{
	const auto tsInput(input.getfirsttok(1, TSTABCHAR));
	const auto p2(input.getnexttok(TSTABCHAR).trim());

	const auto com = std::hash<TString>{}(tsInput.getfirsttok(gsl::narrow_cast<int>(iOffset)).trim());		// 3
	const auto path(tsInput.getlasttoks().trim());	// 4
	const TString tsFlags(p2.getfirsttok(1).trim());
	if (tsFlags[0] != TEXT('+'))
		throw DcxExceptions::dcxInvalidFlag();

	const auto iflags = parseLayoutFlags(tsFlags);

	const TString tsID(p2.getnexttok());

	const auto WGT = p2.getnexttok().to_<UINT>();	// tok 3
	const auto W = p2.getnexttok().to_<INT>();	// tok 4
	const auto H = p2.getnexttok().to_<INT>();	// tok 5

	const auto bPathRoot = (path == TEXT("root"));

	if ((com == TEXT("root"_hash)) || com == TEXT("cell"_hash))
	{
		const auto ID = (dialog) ? dialog->NameToUserID(tsID) : tsID.to_<UINT>();
		const auto cHwnd = GetDlgItem(m_Hwnd, gsl::narrow_cast<int>(mIRC_ID_OFFSET + ID));

		std::unique_ptr<LayoutCell> p_Cell = nullptr;

		// LayoutCellPane
		if (dcx_testflag(iflags, CLATypes::LAYOUTPANE))
		{
			if (dcx_testflag(iflags, CLATypes::LAYOUTHORZ))
				p_Cell = std::make_unique<LayoutCellPane>(LayoutCellPane::PaneType::HORZ);
			else
				p_Cell = std::make_unique<LayoutCellPane>(LayoutCellPane::PaneType::VERT);
		} // if (dcx_testflag(flags, CLATypes::LAYOUTPANE))
		  // LayoutFill Cell
		else if (dcx_testflag(iflags, CLATypes::LAYOUTFILL))
		{
			if (dcx_testflag(iflags, CLATypes::LAYOUTID))
			{
				if (!cHwnd || !IsWindow(cHwnd))
					throw Dcx::dcxException("Cell Fill -> Invalid ID");

				p_Cell = std::make_unique<LayoutCellFill>(cHwnd);
			}
			else
				p_Cell = std::make_unique<LayoutCellFill>();
		} // else if (dcx_testflag(flags, LAYOUTFILL))
		  // LayoutCellFixed
		else if (dcx_testflag(iflags, CLATypes::LAYOUTFIXED))
		{
			auto type = LayoutCellFixed::FixedType::WIDTH;

			if (dcx_testflag(iflags, CLATypes::LAYOUTVERT))
			{
				if (dcx_testflag(iflags, CLATypes::LAYOUTHORZ))
					type = LayoutCellFixed::FixedType::BOTH;
				else
					type = LayoutCellFixed::FixedType::HEIGHT;
			}

			// Defined Rectangle
			if (dcx_testflag(iflags, CLATypes::LAYOUTDIM))
			{
				const RECT rc{ 0,0,W,H };

				if (dcx_testflag(iflags, CLATypes::LAYOUTID))
				{
					if (!cHwnd || !IsWindow(cHwnd))
						throw Dcx::dcxException("Cell Fixed -> Invalid ID");

					p_Cell = std::make_unique<LayoutCellFixed>(cHwnd, rc, type);
				}
				else
					p_Cell = std::make_unique<LayoutCellFixed>(rc, type);

			}
			// No defined Rectangle
			else {
				if (dcx_testflag(iflags, CLATypes::LAYOUTID))
				{
					if (!cHwnd || !IsWindow(cHwnd))
						throw Dcx::dcxException("Cell Fixed -> Invalid ID");

					p_Cell = std::make_unique<LayoutCellFixed>(cHwnd, type);
				}
				else
					throw Dcx::dcxException("Missing control ID flag, can't create cell");
			} //else
		} // else if (dcx_testflag(flags, LAYOUTFIXED))
		else
			throw Dcx::dcxException("Unknown Cell Type");

		if (!p_Cell)
			throw Dcx::dcxException("Unable to Create Cell");

		if (dcx_testflag(iflags, CLATypes::LAYOUTMIN))
		{
			const RECT rc = p_Cell->getClientRect();
			const SIZE sz{ (rc.right - rc.left), (rc.bottom - rc.top) };

			p_Cell->setMinSize(sz);
		}

		//if (com == TEXT("root"_hash))
		//	setRoot(p_Cell.release());
		if (com == TEXT("root"_hash))
			setRoot(std::move(p_Cell));
		// if ( com == TEXT("root") )
		else if (com == TEXT("cell"_hash))
		{
			auto p_GetCell = getRoot();

			if (!bPathRoot)
				p_GetCell = getCell(path);

			if (!p_GetCell)
				throw DcxExceptions::dcxInvalidPath();

			if (p_GetCell->getType() != LayoutCell::CellType::PANE)
				throw Dcx::dcxException("Invalid parent Cell");

			//const auto p_PaneCell = (LayoutCellPane *)p_GetCell;
			const auto p_PaneCell = reinterpret_cast<LayoutCellPane*>(p_GetCell);
			p_PaneCell->addChild(p_Cell.release(), WGT);
		} // else if ( com == TEXT("cell") )

		++m_iCount;

	} // if ( com ==  TEXT("root") || com == TEXT("cell") )
	else if (com == TEXT("space"_hash))
	{
		auto p_GetCell = getRoot();

		if (!bPathRoot)
			p_GetCell = getCell(path);

		if (!p_GetCell)
			throw DcxExceptions::dcxInvalidPath();

		const auto _left = tsID.to_<int>();
		const RECT rc{ _left, gsl::narrow_cast<int>(WGT), W, H };

		if (dcx_testflag(iflags, CLATypes::LAYOUTMIN))
		{
			SIZE szOld = p_GetCell->getMinSize();
			const auto& rcOld = p_GetCell->getBorder();
			if (szOld.cx > 0)
				szOld.cx -= (rcOld.left + rcOld.right);
			if (szOld.cy > 0)
				szOld.cy -= (rcOld.bottom + rcOld.top);

			const SIZE sz{ (szOld.cx + rc.left + rc.right), (szOld.cy + rc.bottom + rc.top) };

			p_GetCell->setMinSize(sz);
		}

		p_GetCell->setBorder(rc);
	} // else if ( com == TEXT("space") )
	else
		throw DcxExceptions::dcxInvalidCommand();
}

const CLATypes LayoutManager::parseLayoutFlags(const TString& flags) noexcept
{
	const XSwitchFlags xflags(flags);
	CLATypes iFlags{};

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('f')])
		iFlags |= CLATypes::LAYOUTFIXED;
	if (xflags[TEXT('h')])
		iFlags |= CLATypes::LAYOUTHORZ;
	if (xflags[TEXT('i')])
		iFlags |= CLATypes::LAYOUTID;
	if (xflags[TEXT('l')])
		iFlags |= CLATypes::LAYOUTFILL;
	if (xflags[TEXT('p')])
		iFlags |= CLATypes::LAYOUTPANE;
	if (xflags[TEXT('v')])
		iFlags |= CLATypes::LAYOUTVERT;
	if (xflags[TEXT('w')])
		iFlags |= CLATypes::LAYOUTDIM;
	if (xflags[TEXT('P')])
		iFlags |= CLATypes::LAYOUTFIXEDPOS;
	if (xflags[TEXT('M')])
		iFlags |= CLATypes::LAYOUTMIN;

	return iFlags;
}
