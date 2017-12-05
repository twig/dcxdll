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

/*!
 * \brief Constructor
 *
 * blah
 */

//LayoutManager::LayoutManager()
//: LayoutManager(nullptr)
//{
//}

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutManager::LayoutManager(HWND mHwnd)
: m_pRoot(nullptr)
, m_Hwnd(mHwnd)
, m_iCount(0)
{
}

/*!
 * \brief Destructor
 *
 * blah
 */

//LayoutManager::~LayoutManager() {
//
//}

/*!
 * \brief blah
 *
 * blah
 */

const bool LayoutManager::updateLayout(RECT & rc)
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

	m_pRoot->setRect(rc);
	m_pRoot->LayoutChild();
	return (EndDeferWindowPos(m_pRoot->ExecuteLayout(BeginDeferWindowPos(gsl::narrow_cast<int>(m_iCount)))) != FALSE);
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutManager::setRoot(std::unique_ptr<LayoutCell> p_Root)
{
	// clean memory in case we use more than once
	//delete m_pRoot;
	//m_pRoot = p_Root;

	m_pRoot = std::move(p_Root);
}

/*!
 * \brief blah
 *
 * blah
 */

//LayoutCell * LayoutManager::getRoot() const noexcept {
//
//	return m_pRoot;
//}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell * LayoutManager::getCell(const TString & path) const
{
	return parsePath(path, m_pRoot.get(), 1);
}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell * LayoutManager::parsePath(const TString & path, const LayoutCell *const hParent, const UINT depth)
{
	if (hParent == nullptr)
		return nullptr;

	const auto k = path.gettok(gsl::narrow_cast<int>(depth)).to_<UINT>();

	if (k < 1)
		return nullptr;

	auto hCurrentCell = hParent->getFirstChild();

	if (hCurrentCell == nullptr)
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

void LayoutManager::AddCell(const TString &input, const UINT iOffset)
{
	const auto tsInput(input.getfirsttok(1, TSTABCHAR));
	const auto p2(input.getnexttok(TSTABCHAR).trim());

	const auto com = std::hash<TString>{}(tsInput.getfirsttok(static_cast<int>(iOffset)).trim());		// 3
	const auto path(tsInput.getlasttoks().trim());	// 4

	const auto iflags = parseLayoutFlags(p2.getfirsttok(1));
	const auto ID = p2.getnexttok().to_<UINT>();	// tok 2
	const auto WGT = p2.getnexttok().to_<UINT>();	// tok 3
	const auto W = p2.getnexttok().to_<INT>();	// tok 4
	const auto H = p2.getnexttok().to_<INT>();	// tok 5

	const auto bPathRoot = (path == TEXT("root"));

	if ((com == TEXT("root"_hash)) || com == TEXT("cell"_hash))
	{
		auto cHwnd = GetDlgItem(m_Hwnd, static_cast<int>(mIRC_ID_OFFSET + ID));

		std::unique_ptr<LayoutCell> p_Cell = nullptr;

		// LayoutCellPane
		if (dcx_testflag(iflags, LAYOUTPANE))
		{
			if (dcx_testflag(iflags, LAYOUTHORZ))
				p_Cell = std::make_unique<LayoutCellPane>(LayoutCellPane::HORZ);
			else
				p_Cell = std::make_unique<LayoutCellPane>(LayoutCellPane::VERT);
		} // if (dcx_testflag(flags, LAYOUTPANE))
		  // LayoutFill Cell
		else if (dcx_testflag(iflags, LAYOUTFILL))
		{
			if (dcx_testflag(iflags, LAYOUTID))
			{
				if (cHwnd == nullptr || !IsWindow(cHwnd))
					throw Dcx::dcxException("Cell Fill -> Invalid ID");

				p_Cell = std::make_unique<LayoutCellFill>(cHwnd);
			}
			else
				p_Cell = std::make_unique<LayoutCellFill>();
		} // else if (dcx_testflag(flags, LAYOUTFILL))
		  // LayoutCellFixed
		else if (dcx_testflag(iflags, LAYOUTFIXED))
		{
			auto type = LayoutCellFixed::WIDTH;

			if (dcx_testflag(iflags, LAYOUTVERT))
			{
				if (dcx_testflag(iflags, LAYOUTHORZ))
					type = LayoutCellFixed::BOTH;
				else
					type = LayoutCellFixed::HEIGHT;
			}

			// Defined Rectangle
			if (dcx_testflag(iflags, LAYOUTDIM))
			{
				const RECT rc{0,0,W,H};

				if (dcx_testflag(iflags, LAYOUTID))
				{
					if (cHwnd == nullptr || !IsWindow(cHwnd))
						throw Dcx::dcxException("Cell Fixed -> Invalid ID");

					p_Cell = std::make_unique<LayoutCellFixed>(cHwnd, rc, type);
				}
				else
					p_Cell = std::make_unique<LayoutCellFixed>(rc, type);

			}
			// No defined Rectangle
			else {
				if (dcx_testflag(iflags, LAYOUTID))
				{
					if (cHwnd == nullptr || !IsWindow(cHwnd))
						throw Dcx::dcxException("Cell Fixed -> Invalid ID");

					p_Cell = std::make_unique<LayoutCellFixed>(cHwnd, type);
				}
				else
					throw Dcx::dcxException("Missing control ID flag, can't create cell");
			} //else
		} // else if (dcx_testflag(flags, LAYOUTFIXED))
		else
			throw Dcx::dcxException("Unknown Cell Type");

		if (p_Cell == nullptr)
			throw Dcx::dcxException("Unable to Create Cell");

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

			if (p_GetCell == nullptr)
				throw Dcx::dcxException("Invalid item path");

			if (p_GetCell->getType() != LayoutCell::PANE)
				throw Dcx::dcxException("Invalid parent Cell");

			auto p_PaneCell = (LayoutCellPane *)p_GetCell;
			p_PaneCell->addChild(p_Cell.release(), WGT);
		} // else if ( com == TEXT("cell") )

		++m_iCount;

	} // if ( com ==  TEXT("root") || com == TEXT("cell") )
	else if (com == TEXT("space"_hash))
	{
		auto p_GetCell = getRoot();

		if (!bPathRoot)
			p_GetCell = getCell(path);

		if (p_GetCell == nullptr)
			throw Dcx::dcxException("Invalid item path");

		const RECT rc{ gsl::narrow_cast<int>(ID), gsl::narrow_cast<int>(WGT), W, H };
		p_GetCell->setBorder(rc);
	} // else if ( com == TEXT("space") )
	else
		throw Dcx::dcxException("Invalid command");
}

const UINT LayoutManager::parseLayoutFlags(const TString & flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

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

	return iFlags;
}
