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

LayoutManager::LayoutManager()
: LayoutManager(NULL)
{
}

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutManager::LayoutManager(HWND mHwnd)
: m_pRoot(NULL)
, m_Hwnd(mHwnd)
, m_iCount(0)
{
}

/*!
 * \brief Destructor
 *
 * blah
 */

LayoutManager::~LayoutManager() {

}

/*!
 * \brief blah
 *
 * blah
 */

BOOL LayoutManager::updateLayout(RECT & rc) {

	if (this->empty())
		return FALSE;

	HDWP hdwp = BeginDeferWindowPos(this->m_iCount);

	if (hdwp == NULL) {
		DCX_DEBUG(mIRCLinker::debug, TEXT("updateLayout()"), TEXT("BeginDeferWindowPos() failed"));
		return FALSE;
	}

	this->m_pRoot->setRect(rc);
	this->m_pRoot->LayoutChild();
	hdwp = this->m_pRoot->ExecuteLayout(hdwp);

	if (hdwp == NULL) {
		DCX_DEBUG(mIRCLinker::debug, TEXT("updateLayout()"), TEXT("DeferWindowPos() failed"));
		return FALSE;
	}
	return EndDeferWindowPos(hdwp);
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutManager::setRoot(LayoutCell * p_Root) {

	// clean memory in case we use more than once
	delete this->m_pRoot;

	this->m_pRoot = p_Root;
}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell * LayoutManager::getRoot() const {

	return this->m_pRoot;
}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell * LayoutManager::getCell(const TString & path) const {

	return this->parsePath(path, this->m_pRoot, 1);
}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell * LayoutManager::parsePath(const TString & path, LayoutCell * hParent, const UINT depth) {

	if (hParent == NULL)
		return NULL;

	UINT i = 1;
	const UINT n = path.numtok();
	const UINT k = path.gettok(depth).to_int();
	LayoutCell * hCurrentCell;

	if (k < 1)
		return NULL;

	hCurrentCell = hParent->getFirstChild();

	if (hCurrentCell == NULL)
		return NULL;

	do {

		if (i == k) {

			// finished tree search and found
			if (depth == n)
				return hCurrentCell;

			return LayoutManager::parsePath(path, hCurrentCell, depth + 1);
		}

		i++;

	} while ((hCurrentCell = hCurrentCell->getNextSibling()) != NULL);

	return NULL;
}

void LayoutManager::AddCell(const TString &input, const UINT iOffset)
{
	const TString tsInput(input.getfirsttok(1, TSTAB));
	const TString p2(input.getnexttok(TSTAB).trim());

	const TString com(tsInput.gettok(iOffset).trim());		// 3
	const TString path(tsInput.gettok(iOffset + 1, -1).trim());	// 4

	const UINT iflags = this->parseLayoutFlags(p2.getfirsttok(1));
	const UINT ID = p2.getnexttok().to_int();	// tok 2
	const UINT WGT = p2.getnexttok().to_int();	// tok 3
	const UINT W = p2.getnexttok().to_int();	// tok 4
	const UINT H = p2.getnexttok().to_int();	// tok 5

	if (com == TEXT("root") || com == TEXT("cell")) {
		HWND cHwnd = GetDlgItem(this->m_Hwnd, mIRC_ID_OFFSET + ID);

		LayoutCell * p_Cell = NULL;

		// LayoutCellPane
		if (iflags & LAYOUTPANE) {

			if (iflags & LAYOUTHORZ)
				p_Cell = new LayoutCellPane(LayoutCellPane::HORZ);
			else
				p_Cell = new LayoutCellPane(LayoutCellPane::VERT);
		} // if ( flags & LAYOUTPANE )
		// LayoutFill Cell
		else if (iflags & LAYOUTFILL) {
			if (iflags & LAYOUTID) {
				if (cHwnd != NULL && IsWindow(cHwnd))
					p_Cell = new LayoutCellFill(cHwnd);
				else
					throw TEXT("Cell Fill -> Invalid ID");
			}
			else {
				p_Cell = new LayoutCellFill();
			}
		} // else if ( flags & LAYOUTFILL )
		// LayoutCellFixed
		else if (iflags & LAYOUTFIXED) {

			LayoutCellFixed::FixedType type;

			if (iflags & LAYOUTVERT && iflags & LAYOUTHORZ)
				type = LayoutCellFixed::BOTH;
			else if (iflags & LAYOUTVERT)
				type = LayoutCellFixed::HEIGHT;
			else
				type = LayoutCellFixed::WIDTH;

			// Defined Rectangle
			if (iflags & LAYOUTDIM) {

				RECT rc;
				SetRect(&rc, 0, 0, W, H);

				if (iflags & LAYOUTID) {

					if (cHwnd != NULL && IsWindow(cHwnd))
						p_Cell = new LayoutCellFixed(cHwnd, rc, type);
					else
						throw TEXT("Cell Fixed -> Invalid ID");
				}
				else
					p_Cell = new LayoutCellFixed(rc, type);

			}
			// No defined Rectangle
			else {

				if (iflags & LAYOUTID) {

					if (cHwnd != NULL && IsWindow(cHwnd))
						p_Cell = new LayoutCellFixed(cHwnd, type);
					else
						throw TEXT("Cell Fixed -> Invalid ID");
				}
			} //else
		} // else if ( flags & LAYOUTFIXED )
		else
			throw TEXT("Unknown Cell Type");

		if (com == TEXT("root")) {

			if (p_Cell != NULL)
				this->setRoot(p_Cell);

		} // if ( com == TEXT("root") )
		else if (com == TEXT("cell")) {

			if (p_Cell != NULL) {

				LayoutCell * p_GetCell;

				if (path == TEXT("root"))
					p_GetCell = this->getRoot();
				else
					p_GetCell = this->getCell(path);

				if (p_GetCell == NULL) {
					delete p_Cell;
					throw TEXT("Invalid item path");
				}

				if (p_GetCell->getType() == LayoutCell::PANE) {
					LayoutCellPane * p_PaneCell = reinterpret_cast<LayoutCellPane *>(p_GetCell);
					p_PaneCell->addChild(p_Cell, WGT);
				}
			}
		} // else if ( com == TEXT("cell") )
		this->m_iCount++;
	} // if ( com ==  TEXT("root") || com == TEXT("cell") )
	else if (com == TEXT("space")) {

		LayoutCell * p_GetCell;

		if (path == TEXT("root"))
			p_GetCell = this->getRoot();
		else
			p_GetCell = this->getCell(path);

		if (p_GetCell == NULL)
			throw TEXT("Invalid item path");
		else {

			RECT rc;
			SetRect(&rc, ID, WGT, W, H);
			p_GetCell->setBorder(rc);
		}
	} // else if ( com == TEXT("space") )
}

const UINT LayoutManager::parseLayoutFlags(const TString & flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('f')])
		iFlags |= LAYOUTFIXED;
	if (xflags[TEXT('h')])
		iFlags |= LAYOUTHORZ;
	if (xflags[TEXT('i')])
		iFlags |= LAYOUTID;
	if (xflags[TEXT('l')])
		iFlags |= LAYOUTFILL;
	if (xflags[TEXT('p')])
		iFlags |= LAYOUTPANE;
	if (xflags[TEXT('v')])
		iFlags |= LAYOUTVERT;
	if (xflags[TEXT('w')])
		iFlags |= LAYOUTDIM;

	return iFlags;
}
