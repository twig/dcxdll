/*!
 * \file dcxlist.cpp
 * \brief blah
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.1
 *
 * \b Revisions
 *	1.1
 *		Added Visual Studio 2005 specific code. Ook
 *
 * © ScriptsDB.org - 2006
 */
#include "defines.h"
#include "Classes/mirc/dcxlist.h"
#include "Classes/dcxdialog.h"
#include "Dcx.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxList::DcxList(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WindowExStyle::ClientEdge,
		DCX_LISTCLASS,
		ws.m_Styles | WindowStyle::Child,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);

	// Check for "draglist" style
	if (styles.istok(TEXT("draglist")))
	{
		// if its multiple select, cant use
		if (this->isStyle(static_cast<WindowStyle>(LBS_MULTIPLESEL)))
			throw Dcx::dcxException("Cannot apply draglist style with multi style");

		if (!MakeDragList(m_Hwnd))
			throw Dcx::dcxException("Error applying draglist style");

		m_iDragList = RegisterWindowMessage(DRAGLISTMSGSTRING);

		this->getParentDialog()->RegisterDragList(this);
	}

	DragAcceptFiles(m_Hwnd, TRUE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxList::~DcxList()
{
	if (const auto pd = this->getParentDialog(); pd)
		pd->UnregisterDragList(this);
}

const TString DcxList::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, LBS_DISABLENOSCROLL))
		styles.addtok(TEXT("noscroll"));
	if (dcx_testflag(Styles, LBS_MULTIPLESEL))
		styles.addtok(TEXT("multi"));
	if (dcx_testflag(Styles, LBS_EXTENDEDSEL))
		styles.addtok(TEXT("extsel"));
	if (dcx_testflag(Styles, LBS_NOINTEGRALHEIGHT))
		styles.addtok(TEXT("nointegral"));
	if (dcx_testflag(Styles, LBS_NOSEL))
		styles.addtok(TEXT("nosel"));
	if (dcx_testflag(Styles, LBS_SORT))
		styles.addtok(TEXT("sort"));
	if (dcx_testflag(Styles, LBS_USETABSTOPS))
		styles.addtok(TEXT("tabs"));
	if (dcx_testflag(Styles, LBS_MULTICOLUMN))
		styles.addtok(TEXT("multicol"));
	if (dcx_testflag(Styles, WS_VSCROLL))
		styles.addtok(TEXT("vsbar"));
	if (dcx_testflag(Styles, WS_HSCROLL))
		styles.addtok(TEXT("hsbar"));
	if (!this->m_bUseDrawInsert)
		styles.addtok(TEXT("dragline"));
	if (!dcx_testflag(Styles, LBS_OWNERDRAWFIXED))
		styles.addtok(TEXT("noformat"));

	return styles;
}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxList::parseControlStyles(const TString& tsStyles)
{
	dcxWindowStyles ws;

	ws.m_Styles |= LBS_NOTIFY | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"noscroll"_hash:
			ws.m_Styles |= LBS_DISABLENOSCROLL;
			break;
		case L"multi"_hash:
			ws.m_Styles |= LBS_MULTIPLESEL;
			break;
		case L"extsel"_hash:
			ws.m_Styles |= LBS_EXTENDEDSEL;
			break;
		case L"nointegral"_hash:
			ws.m_Styles |= LBS_NOINTEGRALHEIGHT;
			break;
		case L"nosel"_hash:
			ws.m_Styles |= LBS_NOSEL;
			break;
		case L"sort"_hash:
			ws.m_Styles |= LBS_SORT;
			break;
		case L"tabs"_hash:
			ws.m_Styles |= LBS_USETABSTOPS;
			break;
		case L"multicol"_hash:
			ws.m_Styles |= LBS_MULTICOLUMN;
			break;
		case L"vsbar"_hash:
			ws.m_Styles |= WS_VSCROLL;
			break;
		case L"hsbar"_hash:
			ws.m_Styles |= WS_HSCROLL;
			break;
		case L"dragline"_hash:
			this->m_bUseDrawInsert = false;
			break;
		case L"noformat"_hash: // dont remove from here
			ws.m_Styles &= static_cast<DWORD>(~LBS_OWNERDRAWFIXED);
			break;
			//case L"shadow"_hash: // looks crap
			//	this->m_bShadowText = true;
		default:
			break;
		}
	}

	return parseGeneralControlStyles(tsStyles, ws);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

 //TString DcxList::parseInfoRequest(const TString& input) const
 //{
 //	return TString();
 //}

void DcxList::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	switch (const auto numtok = input.numtok(); std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP] [N]
	case L"text"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nSel = input.getnexttok().to_int() - 1;	// tok 4

		if (nSel < 0 || nSel >= ListBox_GetCount(m_Hwnd))
			throw DcxExceptions::dcxOutOfRange();

		if (const auto l = ListBox_GetTextLen(m_Hwnd, nSel); (l == LB_ERR || l >= MIRC_BUFFER_SIZE_CCH))
			throw Dcx::dcxException("String Too Long (Greater than Max chars)");

		ListBox_GetText(m_Hwnd, nSel, szReturnValue);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"hoveritem"_hash:
	{
		const auto i = Dcx::dcxListBox_GetHoverItem(m_Hwnd) + 1;
		_ts_snprintf(szReturnValue, TEXT("%d"), i);
	}
	break;
	// [NAME] [ID] [PROP] (N)
	case L"seltext"_hash:
	{
		auto nSel = -1;
		if (this->isStyle(WindowStyle::LBS_MultiSel) || this->isStyle(WindowStyle::LBS_ExtendedSel))
		{
			if (const auto n = ListBox_GetSelCount(m_Hwnd); n > 0)
			{
				auto p = std::make_unique<int[]>(gsl::narrow_cast<size_t>(n));
				ListBox_GetSelItems(m_Hwnd, n, p.get());

				// get a unique value
				if (numtok > 3)
				{
					const auto i = (input.getnexttok().to_int() - 1);	// tok 4

					if ((i < 0) || (i >= n))
						throw DcxExceptions::dcxOutOfRange();

					nSel = gsl::at(p, gsl::narrow_cast<size_t>(i));
				}
				else
					nSel = gsl::at(p, 0U);	// no item requested, so return the first selected item.
			}
		}
		// single select
		else
			nSel = ListBox_GetCurSel(m_Hwnd);

		if (nSel > -1)
		{
			if (const auto l = ListBox_GetTextLen(m_Hwnd, nSel); (l == LB_ERR && l >= MIRC_BUFFER_SIZE_CCH))
				throw Dcx::dcxException("String Too Long (Greater than Max chars)");

			ListBox_GetText(m_Hwnd, nSel, szReturnValue);
		}
	}
	break;
	// [NAME] [ID] [PROP]
	case L"num"_hash:
	{
		const auto i = ListBox_GetCount(m_Hwnd);
		_ts_snprintf(szReturnValue, TEXT("%d"), i);
	}
	break;
	// [NAME] [ID] [PROP] (N)
	case L"sel"_hash:
	{
		if (this->isStyle(WindowStyle::LBS_MultiSel) || this->isStyle(WindowStyle::LBS_ExtendedSel))
		{
			if (const auto n = ListBox_GetSelCount(m_Hwnd); n > 0)
			{
				auto p = std::make_unique<int[]>(gsl::narrow_cast<size_t>(n));
				ListBox_GetSelItems(m_Hwnd, n, p.get());

				TString path;

				// get a unique value
				if (numtok > 3)
				{
					if (const auto i = input.getnexttok().to_int(); i == 0)
						path += n;	// get total number of selected items
					else if ((i > 0) && (i <= n))
						path += (gsl::at(p, gsl::narrow_cast<size_t>(i) - 1U) + 1);
				}
				else {
					// get all items in a long comma seperated string

					for (auto i = decltype(n){0}; i < n; ++i)
						path.addtok((gsl::at(p, gsl::narrow_cast<size_t>(i)) + 1), TSCOMMACHAR);
				}
				if (path.len() > MIRC_BUFFER_SIZE_CCH)
					throw Dcx::dcxException("String too long");

				szReturnValue = path.to_chr();
			}
		}
		// single select
		else
		{
			const auto i = ListBox_GetCurSel(m_Hwnd) + 1;
			_ts_snprintf(szReturnValue, TEXT("%d"), i);
		}
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"tbitem"_hash:
	{
		RECT rc{};

		if (!GetClientRect(m_Hwnd, &rc))
			throw Dcx::dcxException("Unable to get client rect!");

		const auto top = ListBox_GetTopIndex(m_Hwnd);
		const auto height = ListBox_GetItemHeight(m_Hwnd, 0);

		auto bottom = top + ((rc.bottom - rc.top) / height);

		const auto count = ListBox_GetCount(m_Hwnd);

		if (bottom > count)
			bottom = count;

		_ts_snprintf(szReturnValue, TEXT("%d %d"), top, bottom);
	}
	break;
	// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N]
	case L"find"_hash:
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto matchtext(input.getfirsttok(2, TSTABCHAR).trim());
		const auto params(input.getnexttok(TSTABCHAR).trim());	// tok 3

		if (matchtext.empty())
			throw Dcx::dcxException("No Match text supplied");

		const auto SearchType = CharToSearchType(params++[0]);

		const auto N = params++.to_<UINT>();	// tok 2
		const auto nItems = ListBox_GetCount(m_Hwnd);

		dcxSearchData srch_data(matchtext, SearchType);

		// count total
		if (N == 0)
		{
			auto count = 0;

			for (auto i = decltype(nItems){0}; i < nItems; ++i)
			{
				if (matchItemText(i, srch_data))
					++count;
			}

			_ts_snprintf(szReturnValue, TEXT("%d"), count);
		}
		// find Nth matching
		else {
			auto count = decltype(N){0};

			for (auto i = decltype(nItems){0}; i < nItems; ++i)
			{
				if (matchItemText(i, srch_data))
					++count;

				// found Nth matching
				if (count == N)
				{
					++i;
					_ts_snprintf(szReturnValue, TEXT("%d"), i);
					return;
				}
			}
		} // else
	}
	break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
		break;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxList::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	//xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		ListBox_ResetContent(m_Hwnd);

	//xdid -a [NAME] [ID] [SWITCH] [N] [TEXT]
	//xdid -a -> [NAME] [ID] -a [N] [TEXT]
	if (flags[TEXT('a')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		auto nPos = input.getnexttok().to_int() - 1;	// tok 4
		const auto tsItem(input.getlasttoks());			// tok 5, -1

		if (nPos == -1)
			nPos = ListBox_GetCount(m_Hwnd);

		Dcx::dcxListBox_InsertString(m_Hwnd, nPos, tsItem);
	}
	//xdid -A [NAME] [ID] [SWITCH] [N] [+FLAGS] [TEXT]
	//xdid -A -> [NAME] [ID] -A [N] [+FLAGS] [TEXT]
	else if (flags[TEXT('A')])
	{
		//if (numtok < 6)
		//	throw DcxExceptions::dcxInvalidArguments();
		//
		//auto nPos = input.getnexttok().to_int() - 1;	// tok 4
		//
		//if (nPos == -1)
		//	nPos = ListBox_GetCount(m_Hwnd);
		//
		//const XSwitchFlags xOpts(input.getnexttok());		// tok 5
		//auto itemtext(input.getlasttoks().trim());		// tok 6, -1
		//TString tsRes;
		//const auto iItemToks = itemtext.numtok();
		//
		//if (!xOpts[TEXT('+')])
		//	throw DcxExceptions::dcxInvalidFlag();
		//
		//if (xOpts[TEXT('H')]) // [TEXT] == [table] [item]
		//{
		//	if (iItemToks != 2) // load single item from hash table by item name
		//		throw DcxExceptions::dcxInvalidArguments();
		//
		//	mIRCLinker::eval(tsRes, TEXT("$hget(%,%)"), itemtext.getfirsttok(1), itemtext.getnexttok());	// tok 1 then 2
		//
		//	if (Dcx::dcxListBox_InsertString(m_Hwnd, nPos, tsRes) < 0)
		//		throw Dcx::dcxException(TEXT("Error Adding item: %"), tsRes);
		//}
		//else if (xOpts[TEXT('n')]) // [TEXT] == [table] [N]
		//{
		//	if (iItemToks != 2) // load single item from hash table by index
		//		throw DcxExceptions::dcxInvalidArguments();
		//
		//	mIRCLinker::eval(tsRes, TEXT("$hget(%,%).data"), itemtext.getfirsttok(1), itemtext.getnexttok());	// tok 1 then 2
		//
		//	if (Dcx::dcxListBox_InsertString(m_Hwnd, nPos, tsRes) < 0)
		//		throw Dcx::dcxException(TEXT("Error Adding item: %"), tsRes);
		//}
		//else if (xOpts[TEXT('t')]) // [TEXT] == [table] [startN] [endN]
		//{
		//	if (iItemToks != 3) // add contents of a hash table to list
		//		throw DcxExceptions::dcxInvalidArguments();
		//
		//	const auto htable(itemtext.getfirsttok(1));
		//	auto startN = itemtext.getnexttok().to_int();	// tok 2
		//	auto endN = itemtext.getnexttok().to_int();		// tok 3
		//
		//	// get total items in table.
		//	mIRCLinker::eval(tsRes, TEXT("$hget(%,0).item"), htable);
		//	const auto max_items = tsRes.to_int();
		//
		//	// no items in table.
		//	if (max_items == 0)
		//		return;
		//
		//	// If neg number is given start from (last item) - startN
		//	if (startN < 0)
		//		startN = (max_items + startN);
		//
		//	// if start N < 1, make it 1. Allows 0 item. Or case where higher neg number was supplied than items avail.
		//	if (startN < 1)
		//		startN = 1;
		//
		//	// If neg number is given set end to (last item) - endN
		//	if (endN < 0)
		//		endN = (max_items + endN);
		//	// if endN > max or == 0, set to max, allows 0 item for end meaning all
		//	else if ((endN > max_items) || (endN == 0))
		//		endN = max_items;
		//
		//	// if endN < 1 set it to 1
		//	if (endN < 1)
		//		endN = 1;
		//
		//	// check endN comes after startN
		//	if (endN < startN)
		//		throw DcxExceptions::dcxOutOfRange();
		//
		//	this->setRedraw(FALSE);
		//	Auto({ this->setRedraw(TRUE); this->redrawWindow(); });
		//
		//	for (auto i = startN; i <= endN; ++i)
		//	{
		//		//mIRCLinker::tsEvalex(tsRes, TEXT("$hget(%s,%d).data"), htable.to_chr(), i);
		//		mIRCLinker::eval(tsRes, TEXT("$hget(%,%).data"), htable, i);
		//
		//		if (Dcx::dcxListBox_InsertString(m_Hwnd, nPos++, tsRes) < 0)
		//			throw Dcx::dcxException(TEXT("Error Adding item: %"), tsRes);
		//	}
		//}
		//else if (xOpts[TEXT('f')]) // [TEXT] == [startN] [endN] [filename]
		//{
		//	if (iItemToks < 3) // add contents of a file to list
		//		throw DcxExceptions::dcxInvalidArguments();
		//
		//	auto startN = itemtext.getfirsttok(1).to_int();
		//	auto endN = itemtext.getnexttok().to_int();	// tok 2
		//	auto filename(itemtext.getlasttoks());	// tok 3, -1
		//
		//	if (!IsFile(filename))
		//		throw Dcx::dcxException(TEXT("Unable To Access File: %"), itemtext);
		//
		//	const auto contents(readTextFile(filename));
		//	if (contents.empty())
		//		return;
//
		//	auto tok = TEXT("\r\n");
//
		//	auto max_lines = contents.numtok(tok);
		//	if (max_lines == 1U)
		//	{
		//		tok = TEXT("\n");
		//		max_lines = contents.numtok(tok);
		//	}
//
		//	// no data in file.
		//	if (max_lines == 0U)
		//		return;
//
		//	// If neg number is given start from (last line) - startN
		//	if (startN < 0)
		//		startN = (gsl::narrow_cast<int>(max_lines) + startN);
//
		//	// if start N < 1, make it 1. Allows 0 item. Or case where higher neg number was supplied than lines avail.
		//	if (startN < 1)
		//		startN = 1;
//
		//	// If neg number is given set end to (last line) - endN
		//	if (endN < 0)
		//		endN = (gsl::narrow_cast<int>(max_lines) + endN);
		//	// if endN > max or == 0, set to max, allows 0 for end meaning all
		//	else if ((endN > gsl::narrow_cast<int>(max_lines)) || (endN == 0))
		//		endN = gsl::narrow_cast<int>(max_lines);
//
		//	// if endN < 1 set it to 1
		//	if (endN < 1)
		//		endN = 1;
//
		//	// check endN comes after startN
		//	if (endN < startN)
		//		throw DcxExceptions::dcxOutOfRange();
//
		//	this->setRedraw(FALSE);
		//	Auto({ this->setRedraw(TRUE); this->redrawWindow(); });
//
		//	{
		//		const auto itEnd = contents.end();
		//		for (auto itStart = contents.begin(tok); itStart != itEnd; ++itStart)
		//		{
		//			itemtext = (*itStart);
//
		//			if (Dcx::dcxListBox_InsertString(m_Hwnd, nPos++, itemtext) < 0)
		//				throw Dcx::dcxException(TEXT("Error Adding item: %"), itemtext);
		//		}
		//	}
		//}
		//else if (xOpts[TEXT('T')]) // [TEXT] == [C] [text][c][text]......
		//{
		//	if (iItemToks < 2) // add tokens to list
		//		throw DcxExceptions::dcxInvalidArguments();
//
		//	TCHAR tok[2]{};
		//	tok[0] = gsl::narrow_cast<TCHAR>(itemtext.getfirsttok(1).to_int());
		//	tok[1] = 0;
		//	const auto contents(itemtext.getlasttoks());	// tok 2, -1
//
		//	setRedraw(FALSE);
		//	Auto({ this->setRedraw(TRUE); this->redrawWindow(); });
//
		//	{
		//		const auto itEnd = contents.end();
		//		for (auto itStart = contents.begin(&tok[0]); itStart != itEnd; ++itStart)
		//		{
		//			itemtext = (*itStart);
//
		//			if (Dcx::dcxListBox_InsertString(m_Hwnd, nPos++, itemtext) < 0)
		//				throw Dcx::dcxException(TEXT("Error Adding item: %"), itemtext);
		//		}
		//	}
		//}
		//else
		//{
		//	if (Dcx::dcxListBox_InsertString(m_Hwnd, nPos, itemtext) < 0)
		//		throw Dcx::dcxException(TEXT("Error Adding item: %"), itemtext);
		//}
		//// Now update the horizontal scroller
		//UpdateHorizExtent();

		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		auto nPos = input.getnexttok().to_int() - 1;	// tok 4
		const TString tsFlags(input.getnexttok());
		const TString tsArgs(input.getlasttoks().trim());

		if (nPos == -1)
			nPos = ListBox_GetCount(m_Hwnd);

		addItems(nPos, tsFlags, tsArgs);

		// Now update the horizontal scroller
		UpdateHorizExtent();
	}
	//xdid -c [NAME] [ID] [N,[N,[...]]]
	//xdid -c -> [NAME] [ID] -c [N,[N,[...]]]
	else if (flags[TEXT('c')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItems = ListBox_GetCount(m_Hwnd);

		if (this->isStyle(WindowStyle::LBS_MultiSel) || this->isStyle(WindowStyle::LBS_ExtendedSel))
		{
			const auto Ns(input.getnexttok());	// tok 4

			{
				const auto itEnd = Ns.end();
				for (auto itStart = Ns.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
				{
					const TString tsLine(*itStart);

					const auto [iStart, iEnd] = Dcx::getItemRange(tsLine, nItems);

					if ((iStart < 0) || (iEnd < 0) || (iStart >= nItems) || (iEnd >= nItems))
						throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

					for (auto nSel = iStart; nSel <= iEnd; ++nSel)
						ListBox_SetSel(m_Hwnd, TRUE, nSel);
				}
			}
		}
		else {
			auto nSel = (input.getnexttok().to_int() - 1);	// tok 4

			if (nSel == -1)
				nSel = nItems - 1;

			if (nSel > -1 && nSel < nItems)
				ListBox_SetCurSel(m_Hwnd, nSel);
		}
	}
	//xdid -d [NAME] [ID] [SWITCH] [N](,[N],[N1]-N2],...)
	//xdid -d -> [NAME] [ID] -d [N](,[N],[N1]-N2],...)
	//xdid -d -> [NAME] [ID] -d [N] [+flags] [match text]
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto Ns(input.getnexttok());			// tok 4

		const auto nItems = ListBox_GetCount(m_Hwnd);

		if (const XSwitchFlags xFlags(input.getnexttok()); xFlags[TEXT('+')])
		{
			// have flags, so its a match text delete
			const auto tsMatchText(input.getnexttok());

			//const auto SearchType = FlagsToSearchType(xFlags);
			//for (auto nPos = Ns.to_int(); nPos < nItems; ++nPos)
			//{
			//	if (this->matchItemText(nPos, tsMatchText, SearchType))
			//		ListBox_DeleteString(m_Hwnd, nPos--);		// NB: we do nPos-- here as a lines just been removed so we have to check the same nPos again
			//}

			dcxSearchData srch_data(tsMatchText, FlagsToSearchType(xFlags));

			for (auto nPos = Ns.to_int(); nPos < nItems; ++nPos)
			{
				if (this->matchItemText(nPos, srch_data))
					ListBox_DeleteString(m_Hwnd, nPos--);		// NB: we do nPos-- here as a lines just been removed so we have to check the same nPos again
			}
		}
		else {
			{
				const auto itEnd = Ns.end();
				for (auto itStart = Ns.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
				{
					const TString tsLine(*itStart);

					const auto [iStart, iEnd] = Dcx::getItemRange(tsLine, nItems);

					if ((iStart < 0) || (iEnd < 0) || (iStart >= nItems) || (iEnd >= nItems))
						throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

					for (auto nPos = iStart; nPos <= iEnd; ++nPos)
						ListBox_DeleteString(m_Hwnd, nPos);
				}
			}
		}
	}
	// Used to prevent invalid flag message.
	//xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')])
	{
	}
	//xdid -u [NAME] [ID] [SWITCH]
	else if (flags[TEXT('u')])
	{
		if (this->isStyle(WindowStyle::LBS_MultiSel) || this->isStyle(WindowStyle::LBS_ExtendedSel))
			ListBox_SetSel(m_Hwnd, FALSE, -1);
		else
			ListBox_SetCurSel(m_Hwnd, -1);
	}
	//xdid -m [NAME] [ID] [SWITCH] [+FLAGS] [N](,[N]...)
	//xdid -m -> [NAME] [ID] -m [+FLAGS] [N](,[N]...)
	else if (flags[TEXT('m')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const XSwitchFlags xflags(input.getnexttok());	// tok 4

		if (xflags[TEXT('w')])
			ListBox_SetColumnWidth(m_Hwnd, input.getnexttok().to_int());	// tok 5
		else if (xflags[TEXT('t')])
		{
			const auto Ns(input.getnexttok());	// tok 5

			if (const auto n = Ns.numtok(TSCOMMACHAR); n == 1)
			{
				const auto nTab = Ns.to_int();
				if (nTab < 0)
					ListBox_SetTabStops(m_Hwnd, 0, nullptr);
				else
					ListBox_SetTabStops(m_Hwnd, 1, &nTab);
			}
			else {
				auto tabs = std::make_unique<int[]>(n);

				{
					const auto itEnd = Ns.end();
					UINT i = 0;
					for (auto itStart = Ns.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
					{
						gsl::at(tabs, i++) = (*itStart).to_int();	// tok i
					}
				}

				ListBox_SetTabStops(m_Hwnd, n, tabs.get());
			}
		}
		else
			throw DcxExceptions::dcxInvalidFlag();
	}
	//xdid -o [NAME] [ID] [N] [TEXT]
	//xdid -o -> [NAME] [ID] -o [N] [TEXT]
	else if (flags[TEXT('o')])
	{
		auto nPos = input.getnexttok().to_int() - 1;	// tok 4

		if (nPos == -1)
			nPos = ListBox_GetCount(m_Hwnd) - 1;

		if (nPos < 0 && nPos >= ListBox_GetCount(m_Hwnd))
			throw DcxExceptions::dcxOutOfRange();

		ListBox_DeleteString(m_Hwnd, nPos);
		Dcx::dcxListBox_InsertString(m_Hwnd, nPos, input.getlasttoks());	// tok 5, -1
	}
	//xdid -z [NAME] [ID]
	// update horiz scrollbar
	else if (flags[TEXT('z')])
	{
		// Now update the horizontal scroller
		this->UpdateHorizExtent();
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxList::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	if (uMsg == m_iDragList)
	{
		bParsed = TRUE;

		dcxlParam(LPDRAGLISTINFO, dli);

		if (!dli)
			return 0L;

		const auto sel = ListBox_GetCurSel(m_Hwnd) + 1;
		//	TCHAR szRet[20] = { 0 };
	//
		//	switch (dli->uNotification)
		//	{
		//		// begin dragging item
		//	case DL_BEGINDRAG:
		//		{
		//			// callback DIALOG itemdragbegin THIS_ID DRAGGEDITEM
		//			evalAliasEx(szRet, Dcx::countof(szRet), TEXT("itemdragbegin,%u,%d"), getUserID(), sel);
	//
		//			// cancel drag event
		//			return (lstrcmpi(szRet, TEXT("nodrag")) != 0);
		//		}
		//		// cancel drag
		//	case DL_CANCELDRAG:
		//		{
		//			// callback DIALOG itemdragcancel THIS_ID DRAGGEDITEM
		//			evalAliasEx(szRet, Dcx::countof(szRet), TEXT("itemdragcancel,%u,%d"), getUserID(), sel);
			//
		//			if (m_bUseDrawInsert)
		//				getParentDialog()->redrawWindow();
		//			else
		//				redrawWindow();
		//		}
		//		break;
	//
		//		// current dragging, as mirc if it needs to change the cursor or not
		//	case DL_DRAGGING:
		//		{
		//			const auto item = LBItemFromPt(m_Hwnd, dli->ptCursor, TRUE);
	//
		//			if (m_bUseDrawInsert)
		//				DrawInsert(this->m_pParentHWND, m_Hwnd, item);
		//			else
		//				DrawDragLine(item);
	//
		//			// callback DIALOG itemdrag THIS_ID SEL_ITEM MOUSE_OVER_ITEM
		//			evalAliasEx(szRet, Dcx::countof(szRet), TEXT("itemdrag,%u,%d,%d"), getUserID(), sel, item +1);
	//
		//			if (szRet[0] != 0) // check for empty string first
		//			{
		//				if (lstrcmpi(szRet, TEXT("stop")) == 0)
		//					return DL_STOPCURSOR;
		//				else if (lstrcmpi(szRet, TEXT("copy")) == 0)
		//					return DL_COPYCURSOR;
		//			}
	//
		//			return DL_MOVECURSOR;
		//		}
		//		break;
	//
		//		// finish dragging
		//	case DL_DROPPED:
		//		{
		//			// callback DIALOG itemdragfinish THIS_ID SEL_ITEM MOUSE_OVER_ITEM
		//			const auto item = LBItemFromPt(m_Hwnd, dli->ptCursor, TRUE);
	//
		//			execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("itemdragfinish"), getUserID(), sel, item +1);
	//
		//			if (m_bUseDrawInsert)
		//				// refresh m_pParent to remove drawing leftovers
		//				getParentDialog()->redrawWindow();
		//			else
		//				redrawWindow();
		//		}
		//		break;
		//	}
	//
		//	return 0L;
		//}

		const stString<20> szRet;

		switch (dli->uNotification)
		{
			// begin dragging item
		case DL_BEGINDRAG:
		{
			// callback DIALOG itemdragbegin THIS_ID DRAGGEDITEM
			evalAliasEx(szRet, gsl::narrow_cast<int>(szRet.size()), TEXT("itemdragbegin,%u,%d"), getUserID(), sel);

			// cancel drag event
			return (szRet != TEXT("nodrag"));
		}
		// cancel drag
		case DL_CANCELDRAG:
		{
			// callback DIALOG itemdragcancel THIS_ID DRAGGEDITEM
			evalAliasEx(szRet, gsl::narrow_cast<int>(szRet.size()), TEXT("itemdragcancel,%u,%d"), getUserID(), sel);

			if (m_bUseDrawInsert)
				getParentDialog()->redrawWindow();
			else
				redrawWindow();
		}
		break;

		// current dragging, ask mirc if it needs to change the cursor or not
		case DL_DRAGGING:
		{
			const auto item = LBItemFromPt(m_Hwnd, dli->ptCursor, TRUE);

			if (m_bUseDrawInsert)
				DrawInsert(this->getParentHWND(), m_Hwnd, item);
			else
				DrawDragLine(item);

			// callback DIALOG itemdrag THIS_ID SEL_ITEM MOUSE_OVER_ITEM
			evalAliasEx(szRet, gsl::narrow_cast<int>(szRet.size()), TEXT("itemdrag,%u,%d,%d"), getUserID(), sel, item + 1);

			if (!szRet.empty()) // check for empty string first
			{
				if (szRet == TEXT("stop"))
					return DL_STOPCURSOR;
				else if (szRet == TEXT("copy"))
					return DL_COPYCURSOR;
			}

			return DL_MOVECURSOR;
		}
		break;

		// finish dragging
		case DL_DROPPED:
		{
			// callback DIALOG itemdragfinish THIS_ID SEL_ITEM MOUSE_OVER_ITEM
			const auto item = LBItemFromPt(m_Hwnd, dli->ptCursor, TRUE);

			execAliasEx(TEXT("itemdragfinish,%u,%d,%d"), getUserID(), sel, item + 1);

			if (m_bUseDrawInsert)
				// refresh m_pParent to remove drawing leftovers
				getParentDialog()->redrawWindow();
			else
				redrawWindow();
		}
		break;
		default:
			break;
		}

		return 0L;
	}

	switch (uMsg)
	{
	case WM_COMMAND:
	{
		if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
		{
			switch (Dcx::dcxHIWORD(wParam))
			{
			case LBN_SELCHANGE:
			{
#ifdef USE_FIX_01
				if (isExStyle(WindowExStyle::Transparent) && !getParentDialog()->isExStyle(WindowExStyle::Composited))
					redrawBufferedWindowClient();
#endif

				const auto nItem = ListBox_GetCurSel(m_Hwnd);

				if (this->isStyle(WindowStyle::LBS_MultiSel) || this->isStyle(WindowStyle::LBS_ExtendedSel))
				{
					if (ListBox_GetSel(m_Hwnd, nItem) > 0)
						this->execAliasEx(TEXT("sclick,%u,%d"), getUserID(), nItem + 1);
				}
				else if (nItem != LB_ERR)
					this->execAliasEx(TEXT("sclick,%u,%d"), getUserID(), nItem + 1);
			}
			break;

			case LBN_DBLCLK:
			{
				const auto nItem = ListBox_GetCurSel(m_Hwnd);

				if (this->isStyle(WindowStyle::LBS_MultiSel) || this->isStyle(WindowStyle::LBS_ExtendedSel))
				{
					if (ListBox_GetSel(m_Hwnd, nItem) > 0)
						this->execAliasEx(TEXT("dclick,%u,%d"), getUserID(), nItem + 1);
				}
				else if (nItem != LB_ERR)
					this->execAliasEx(TEXT("dclick,%u,%d"), getUserID(), nItem + 1);
			}
			break;
			default:
				break;
			} // switch ( HIWORD( wParam ) )
		}
	}
	break;
	case WM_DRAWITEM:
	{
		dcxlParam(LPDRAWITEMSTRUCT, lpDrawItem);

		if (!lpDrawItem)
			break;

		const auto len = ListBox_GetTextLen(lpDrawItem->hwndItem, lpDrawItem->itemID);
		if (len == LB_ERR)
			break;

		bParsed = TRUE;

		RECT rc{};
		COLORREF clrText = CLR_INVALID;

		CopyRect(&rc, &lpDrawItem->rcItem);
#ifdef USE_FIX_01
		if (isExStyle(WindowExStyle::Transparent) && !getParentDialog()->isExStyle(WindowExStyle::Composited))
			DrawParentsBackground(lpDrawItem->hDC, &rc);
		else if (!getBackClrBrush())
			FillRect(lpDrawItem->hDC, &rc, Dcx::dcxGetStockObject<HBRUSH>(WHITE_BRUSH));
		else
			DcxControl::DrawCtrlBackground(lpDrawItem->hDC, this, &rc);
#else
		if (!getBackClrBrush())
			FillRect(lpDrawItem->hDC, &rc, Dcx::dcxGetStockObject<HBRUSH>(WHITE_BRUSH));
		else
			DcxControl::DrawCtrlBackground(lpDrawItem->hDC, this, &rc);
#endif
		if (dcx_testflag(lpDrawItem->itemState, ODS_SELECTED))
		{
			// fill background with selected colour.
			FillRect(lpDrawItem->hDC, &rc, GetSysColorBrush(COLOR_HIGHLIGHT));
			// draw focus rect around selected item.
			DrawFocusRect(lpDrawItem->hDC, &rc);
			// set selected text colour.
			clrText = SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
		}
		else {
			// set text colour.
			if (const auto clr = getTextColor(); clr != CLR_INVALID)
				clrText = SetTextColor(lpDrawItem->hDC, clr);
		}

		if (len > 0)
		{ // Only do all this if theres any text to draw.
			//TString txt(gsl::narrow_cast<TString::size_type>(len + 1));
			//ListBox_GetText(lpDrawItem->hwndItem, lpDrawItem->itemID, txt.to_chr());

			const auto txt (Dcx::dcxListBox_GetText(lpDrawItem->hwndItem, lpDrawItem->itemID));

			rc.left += 2;

			const UINT style = (isStyle(WindowStyle::LBS_UseTabStops)) ? DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_EXPANDTABS : DT_LEFT | DT_VCENTER | DT_SINGLELINE;

			//calcStrippedRect(lpDrawItem->hDC, txt, style, &rc, false);
			//this->calcTextRect(lpDrawItem->hDC, txt, &rc, style); // causes drawing issues with multi column listboxes.

			this->ctrlDrawText(lpDrawItem->hDC, txt, &rc, style);
		}

		if (clrText != CLR_INVALID)
			SetTextColor(lpDrawItem->hDC, clrText);

		return TRUE;
	}
	break;

	//case WM_MEASUREITEM:
	//	{
	//		LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT) lParam;
	//		//lpMeasureItem->itemHeight = 16;
	//		//return TRUE;
	//	}
	//	break;

	//case WM_CTLCOLORLISTBOX:
	//{
	//}
	//break;

	default:
		break;
	}
	return 0L;
}

LRESULT DcxList::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_LBUTTONUP: // Prevents CommonMessage() handling of this.
	case WM_LBUTTONDBLCLK:
		break;

	case WM_VSCROLL:
		if (Dcx::dcxLOWORD(wParam) == SB_ENDSCROLL)
			this->execAliasEx(TEXT("scrollend,%u"), getUserID());
#ifdef USE_FIX_SCROLL
		// Taken & adapted from: https://forum.powerbasic.com/forum/user-to-user-discussions/source-code/782926-listbox-with-2-147-483-647-items
		// 
		//wParam	The HIWORD specifies the current position of the scroll box if the LOWORD is SB_THUMBPOSITION or SB_THUMBTRACK; otherwise, this word is not used.
		//			The LOWORD specifies a scroll bar value that indicates the user's scrolling request.This parameter can be one of the following values.
		//lParam	If the message is sent by a scroll bar control, this parameter is the handle to the scroll bar control.
		//			If the message is sent by a standard scroll bar, this parameter is zero.
		//if ((Dcx::dcxLOWORD(wParam) == SB_THUMBPOSITION) || (Dcx::dcxLOWORD(wParam) == SB_THUMBTRACK))
		if ((wParam & 0x0000fffe) == SB_THUMBPOSITION)
		{
			//'wParam AND ... is a quick way to test for SB_THUMBPOSITION(4) or SB_THUMBTRACK(5)
			SCROLLINFO si{};
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
			GetScrollInfo(m_Hwnd, SB_VERT, &si); // 'Get valid 32bit position
			si.nPos = si.nTrackPos;
			SetScrollInfo(m_Hwnd, SB_VERT, &si, TRUE); // 'Fix scrollbar thumb position avoiding the Listbox 16 bit engine.
			SendMessage(m_Hwnd, LB_SETTOPINDEX, si.nTrackPos, 0); // 'Scroll listbox
			bParsed = TRUE;
		}
#endif

#ifdef USE_FIX_01
		//if ((Dcx::dcxLOWORD(wParam) == SB_ENDSCROLL) && isExStyle(WindowExStyle::Transparent) && !getParentDialog()->isExStyle(WindowExStyle::Composited))
		//{
		//	bParsed = TRUE;
		//
		//	const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);
		//
		//	redrawBufferedWindowClient();
		//
		//	return lRes;
		//}
#endif
		break;

#ifdef USE_FIX_01
		//case WM_HSCROLL:
		//{
		//	if (isExStyle(WindowExStyle::Transparent) && !getParentDialog()->isExStyle(WindowExStyle::Composited))
		//	{
		//		bParsed = TRUE;
	//
		//		const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);
	//
		//		redrawBufferedWindowClient();
	//
		//		return lRes;
		//	}
		//}
		//break;
#endif

#ifdef USE_FIX_SCROLL
	case WM_KEYDOWN:
	{
		// Taken & adapted from: https://forum.powerbasic.com/forum/user-to-user-discussions/source-code/782926-listbox-with-2-147-483-647-items
		// 
		//'HOME and END travel only half the way with 2,147,483,646 items without the following
		if ((wParam == VK_HOME) || (wParam == VK_END))
		{
			const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);

			SCROLLINFO si{};

			if (wParam == VK_HOME)
				si.nPos = 0;
			else
				si.nPos = ListBox_GetCount(m_Hwnd) - 1;

			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_POS;
			SetScrollInfo(m_Hwnd, SB_VERT, &si, TRUE); // 'Fix scrollbar thumb position avoiding the Lisbox 16 bit engine.
			SendMessage(m_Hwnd, LB_SETTOPINDEX, si.nPos, 0); // 'Scroll listbox
			bParsed = TRUE;

			return lRes;
		}
	}
	break;
#endif

	case WM_MOUSEWHEEL:
		SendMessage(getParentDialog()->getHwnd(), uMsg, wParam, lParam);
		break;

	case WM_PAINT:
	{
		if (!IsAlphaBlend())
			break;

		if (!wParam)
		{
			PAINTSTRUCT ps{};

			auto hdc = BeginPaint(m_Hwnd, &ps);
			Auto(EndPaint(m_Hwnd, &ps));

			bParsed = TRUE;

			// Setup alpha blend if any.
			auto ai = this->SetupAlphaBlend(&hdc);
			Auto(this->FinishAlphaBlend(ai));

			return CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
		}
		else {
			bParsed = TRUE;

			auto hdc = reinterpret_cast<HDC>(wParam);

			// Setup alpha blend if any.
			auto ai = this->SetupAlphaBlend(&hdc);
			Auto(this->FinishAlphaBlend(ai));

			return CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
		}
	}
	break;

#ifdef USE_FIX_01
	// needed for odd bits at bottom/top of control where no item is shown.
	case WM_ERASEBKGND:
	{
		if (isExStyle(WindowExStyle::Transparent) && !getParentDialog()->isExStyle(WindowExStyle::Composited))
		{
			auto hdc = reinterpret_cast<HDC>(wParam);

			////DcxControl::DrawCtrlBackground();

			DrawParentsBackground(hdc);

			bParsed = TRUE;
			return TRUE;
		}
	}
	break;
#endif

	case WM_DESTROY:
	{
		delete this;
		bParsed = TRUE;
	}
	break;

	default:
		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}

// Draws a horizontal line to insert rather than the arrow
// Ported from http://www.vb-hellfire.de/knowlib/draglist.php
void DcxList::DrawDragLine(const int location) noexcept
{
	RECT rc{};

	if (!Dcx::dcxListBox_GetItemRect(m_Hwnd, location, &rc))
		return;

	if (location != m_iLastDrawnLine)
	{
		redrawWindow();
		m_iLastDrawnLine = location;
	}

	const auto hDC = GetDC(m_Hwnd);

	if (!hDC)
		return;
	Auto(ReleaseDC(m_Hwnd, hDC));

	if (const auto hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWTEXT)); hPen)
	{
		Auto(DeleteObject(hPen));

		const auto hOldPen = Dcx::dcxSelectObject<HPEN>(hDC, hPen);
		Auto(Dcx::dcxSelectObject<HPEN>(hDC, hOldPen));

		// get width
		const auto lWidth = (rc.right - rc.left);

		MoveToEx(hDC, 0, rc.top, nullptr);
		LineTo(hDC, lWidth, rc.top);
		MoveToEx(hDC, 0, rc.top - 1, nullptr);
		LineTo(hDC, lWidth, rc.top - 1);

		// Spitze links:
		MoveToEx(hDC, 0, rc.top - 3, nullptr);
		LineTo(hDC, 0, rc.top + 3);
		MoveToEx(hDC, 1, rc.top - 2, nullptr);
		LineTo(hDC, 1, rc.top + 2);

		// Spitze rechts:
		MoveToEx(hDC, lWidth - 1, rc.top - 3, nullptr);
		LineTo(hDC, lWidth - 1, rc.top + 3);
		MoveToEx(hDC, lWidth - 2, rc.top - 2, nullptr);
		LineTo(hDC, lWidth - 2, rc.top + 2);
	}
}

//bool DcxList::matchItemText(const int nItem, const TString &search, const DcxSearchTypes &SearchType) const
//{
//	const auto len = ListBox_GetTextLen(m_Hwnd, nItem);
//
//	if (len > 0) {
//		auto itemtext = std::make_unique<TCHAR[]>(len + 1);
//
//		ListBox_GetText(m_Hwnd, nItem, itemtext.get());
//
//		//switch (SearchType)
//		//{
//		//case DcxSearchTypes::SEARCH_R:	// regex match
//		//	return isRegexMatch(itemtext.get(), search.to_chr());
//		//case DcxSearchTypes::SEARCH_W:	// wildcard match
//		//	return TString(itemtext).iswm(search);
//		//case DcxSearchTypes::SEARCH_E:   // exact match
//		//	return (dcx_strncmp(itemtext.get(), search.to_chr(), len) == 0);
//		//}
//
//		return DcxListHelper::matchItemText(itemtext.get(), search, SearchType);
//	}
//	return false;
//}
//
//void DcxList::getItemRange(const TString &tsItems, const int nItemCnt, int *iStart_range, int *iEnd_range)
//{
//	//int iStart, iEnd;
//	//if (tsItems.numtok(TEXT('-')) == 2) {
//	//	iStart = tsItems.getfirsttok(1, TEXT('-')).to_int() -1;
//	//	iEnd = tsItems.getnexttok(TEXT('-')).to_int() -1;
//	//
//	//	if (iEnd == -1)	// special case
//	//		iEnd = nItemCnt -1;
//	//}
//	//else {
//	//	iEnd = tsItems.to_int() -1;
//	//
//	//	if (iEnd == -1)	// special case
//	//		iStart = iEnd = nItemCnt -1;
//	//	else
//	//		iStart = iEnd;
//	//}
//	//*iStart_range = iStart;
//	//*iEnd_range = iEnd;
//
//	std::tie(*iStart_range, *iEnd_range) = getItemRange(tsItems, nItemCnt);
//}
//
//std::pair<int, int> DcxList::getItemRange(const TString &tsItems, const int nItemCnt)
//{
//	int iStart, iEnd;
//	if (tsItems.numtok(TEXT('-')) == 2) {
//		iStart = tsItems.getfirsttok(1, TEXT('-')).to_int() - 1;
//		iEnd = tsItems.getnexttok(TEXT('-')).to_int() - 1;
//
//		if (iEnd == -1)	// special case
//			iEnd = nItemCnt - 1;
//	}
//	else {
//		iEnd = tsItems.to_int() - 1;
//
//		if (iEnd == -1)	// special case
//			iStart = iEnd = nItemCnt - 1;
//		else
//			iStart = iEnd;
//	}
//	return std::make_pair(iStart, iEnd);
//}

GSL_SUPPRESS(bounds.4)
GSL_SUPPRESS(con.4)
GSL_SUPPRESS(r.5)
bool DcxList::matchItemText(const int nItem, const TString& search, const DcxSearchTypes& SearchType) const
{
	const auto itemtext(Dcx::dcxListBox_GetText(m_Hwnd, nItem));

	return DcxSearchHelper::matchItemText(itemtext.to_chr(), search, SearchType);
}

bool DcxList::matchItemText(const int nItem, const dcxSearchData& srch_data) const
{
	const auto itemtext(Dcx::dcxListBox_GetText(m_Hwnd, nItem));

	return DcxSearchHelper::matchItemText(itemtext.to_chr(), srch_data);
}

//void DcxList::StrLenToExtent(int *nLineExtent)
//{ // Get Font sizes (best way i can find atm, if you know something better then please let me know)
//
//	HFONT hFont = this->getFont();
//
//	if (hFont == nullptr)
//		return;
//
//	HDC hdc = GetDC( m_Hwnd);
//
//	if (hdc == nullptr)
//		return;
//
//	TEXTMETRIC tm;
//
//	HFONT hOldFont = SelectFont(hdc, hFont);
//
//	GetTextMetrics(hdc, &tm);
//
//	SelectFont(hdc, hOldFont);
//
//	ReleaseDC( m_Hwnd, hdc);
//
//	// Multiply max str len by font average width + 1
//	*nLineExtent *= (tm.tmAveCharWidth + tm.tmOverhang);
//	// Add 2 * chars as spacer.
//	*nLineExtent += (tm.tmAveCharWidth * 2);
//	//*nLineExtent++;
//}

void DcxList::UpdateHorizExtent(const int nPos)
{
	if (nPos < -1)
		return;

	if (const auto hdc = GetDC(m_Hwnd); hdc)
	{
		Auto(ReleaseDC(m_Hwnd, hdc));

		HFONT hFont = getFont(), hOldFont = nullptr;
		SIZE sz{};

		const auto nHorizExtent = ListBox_GetHorizontalExtent(m_Hwnd);

		if (hFont)
			hOldFont = Dcx::dcxSelectObject<HFONT>(hdc, hFont);

		if (const auto itemtext(Dcx::dcxListBox_GetText(m_Hwnd, nPos)); !itemtext.empty())
		{
			if (GetTextExtentPoint32(hdc, itemtext.to_chr(), gsl::narrow_cast<int>(itemtext.len()), &sz))
			{
				if (sz.cx > nHorizExtent)
					ListBox_SetHorizontalExtent(m_Hwnd, sz.cx);
			}
		}
		if (hFont)
			Dcx::dcxSelectObject<HFONT>(hdc, hOldFont);

	}
}

int DcxList::addItems(int nPos, const TString& tsFlags, const TString& tsArgs)
{
	const XSwitchFlags xOpts(tsFlags);

	if (!xOpts[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	const auto iArgsToks = tsArgs.numtok();
	TString tsRes;

	if (xOpts[TEXT('H')]) // [TEXT] == [table] [item]
	{
		if (iArgsToks != 2) // load single item from hash table by item name
			throw DcxExceptions::dcxInvalidArguments();

		mIRCLinker::eval(tsRes, TEXT("$hget(%,%)"), tsArgs.getfirsttok(1), tsArgs.getnexttok());	// tok 1 then 2

		if (Dcx::dcxListBox_InsertString(m_Hwnd, nPos++, tsRes) < 0)
			throw Dcx::dcxException(TEXT("Error Adding item: %"), tsRes);
	}
	else if (xOpts[TEXT('n')]) // [TEXT] == [table] [N]
	{
		if (iArgsToks != 2) // load single item from hash table by index
			throw DcxExceptions::dcxInvalidArguments();

		mIRCLinker::eval(tsRes, TEXT("$hget(%,%).data"), tsArgs.getfirsttok(1), tsArgs.getnexttok());	// tok 1 then 2

		if (Dcx::dcxListBox_InsertString(m_Hwnd, nPos++, tsRes) < 0)
			throw Dcx::dcxException(TEXT("Error Adding item: %"), tsRes);
	}
	else if (xOpts[TEXT('t')]) // [TEXT] == [table] [startN] [endN]
	{
		if (iArgsToks != 3) // add contents of a hash table to list
			throw DcxExceptions::dcxInvalidArguments();

		const auto htable(tsArgs.getfirsttok(1));
		auto startN = tsArgs.getnexttok().to_int();	// tok 2
		auto endN = tsArgs.getnexttok().to_int();		// tok 3

		// get total items in table.
		mIRCLinker::eval(tsRes, TEXT("$hget(%,0).item"), htable);
		const auto max_items = tsRes.to_int();

		// no items in table.
		if (max_items == 0)
			return nPos;

		// If neg number is given start from (last item) - startN
		if (startN < 0)
			startN = (max_items + startN);

		// if start N < 1, make it 1. Allows 0 item. Or case where higher neg number was supplied than items avail.
		if (startN < 1)
			startN = 1;

		// If neg number is given set end to (last item) - endN
		if (endN < 0)
			endN = (max_items + endN);
		// if endN > max or == 0, set to max, allows 0 item for end meaning all
		else if ((endN > max_items) || (endN == 0))
			endN = max_items;

		// if endN < 1 set it to 1
		if (endN < 1)
			endN = 1;

		// check endN comes after startN
		if (endN < startN)
			throw DcxExceptions::dcxOutOfRange();

		this->setRedraw(FALSE);
		Auto({ this->setRedraw(TRUE); this->redrawWindow(); });

		for (auto i = startN; i <= endN; ++i)
		{
			mIRCLinker::eval(tsRes, TEXT("$hget(%,%).data"), htable, i);

			if (Dcx::dcxListBox_InsertString(m_Hwnd, nPos++, tsRes) < 0)
				throw Dcx::dcxException(TEXT("Error Adding item: %"), tsRes);
		}
	}
	else if (xOpts[TEXT('f')]) // [TEXT] == [startN] [endN] [filename]
	{
		if (iArgsToks < 3) // add contents of a file to list
			throw DcxExceptions::dcxInvalidArguments();

		auto startN = tsArgs.getfirsttok(1).to_int();
		auto endN = tsArgs.getnexttok().to_int();	// tok 2
		auto filename(tsArgs.getlasttoks());	// tok 3, -1

		if (!IsFile(filename))
			throw Dcx::dcxException(TEXT("Unable To Access File: %"), tsArgs);

		const auto contents(readTextFile(filename));
		if (contents.empty())
			return nPos;

		auto tok = TEXT("\r\n");

		auto max_lines = contents.numtok(tok);
		if (max_lines == 1U)
		{
			tok = TEXT("\n");
			max_lines = contents.numtok(tok);
		}

		// no data in file.
		if (max_lines == 0U)
			return nPos;

		// If neg number is given start from (last line) - startN
		if (startN < 0)
			startN = (gsl::narrow_cast<int>(max_lines) + startN);

		// if start N < 1, make it 1. Allows 0 item. Or case where higher neg number was supplied than lines avail.
		if (startN < 1)
			startN = 1;

		// If neg number is given set end to (last line) - endN
		if (endN < 0)
			endN = (gsl::narrow_cast<int>(max_lines) + endN);
		// if endN > max or == 0, set to max, allows 0 for end meaning all
		else if ((endN > gsl::narrow_cast<int>(max_lines)) || (endN == 0))
			endN = gsl::narrow_cast<int>(max_lines);

		// if endN < 1 set it to 1
		if (endN < 1)
			endN = 1;

		// check endN comes after startN
		if (endN < startN)
			throw DcxExceptions::dcxOutOfRange();

		this->setRedraw(FALSE);
		Auto({ this->setRedraw(TRUE); this->redrawWindow(); });

		{
			const auto itEnd = contents.end();
			for (auto itStart = contents.begin(tok); itStart != itEnd; ++itStart)
			{
				if (Dcx::dcxListBox_InsertString(m_Hwnd, nPos++, (*itStart)) < 0)
					throw Dcx::dcxException(TEXT("Error Adding item: %"), (*itStart));
			}
		}
	}
	else if (xOpts[TEXT('T')]) // [TEXT] == [C] [text][c][text]......
	{
		if (iArgsToks < 2) // add tokens to list
			throw DcxExceptions::dcxInvalidArguments();

		TCHAR tok[2]{};
		tok[0] = gsl::narrow_cast<TCHAR>(tsArgs.getfirsttok(1).to_int());
		tok[1] = 0;
		const auto contents(tsArgs.getlasttoks());	// tok 2, -1

		setRedraw(FALSE);
		Auto({ this->setRedraw(TRUE); this->redrawWindow(); });

		{
			const auto itEnd = contents.end();
			for (auto itStart = contents.begin(&tok[0]); itStart != itEnd; ++itStart)
			{
				if (Dcx::dcxListBox_InsertString(m_Hwnd, nPos++, (*itStart)) < 0)
					throw Dcx::dcxException(TEXT("Error Adding item: %"), (*itStart));
			}
		}
	}
	else
	{
		if (Dcx::dcxListBox_InsertString(m_Hwnd, nPos++, tsArgs) < 0)
			throw Dcx::dcxException(TEXT("Error Adding item: %"), tsArgs);
	}
	return nPos;
}

void DcxList::UpdateHorizExtent(void)
{
	const auto nTotalItems = ListBox_GetCount(m_Hwnd);
	auto nMaxStrlen = 0, iLongestItem = -1;

	for (auto i = decltype(nTotalItems){0}; i < nTotalItems; ++i)
	{
		if (const auto nLen = ListBox_GetTextLen(m_Hwnd, i); nLen > nMaxStrlen)
		{
			nMaxStrlen = nLen;
			iLongestItem = i;
		}
	}

	if (iLongestItem >= 0)
		UpdateHorizExtent(iLongestItem);
}

void DcxList::toXml(TiXmlElement* const xml) const
{
	if (!xml)
		return;

	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());

	{
		const int nCnt = ListBox_GetCount(m_Hwnd);

		if (nCnt < 1)
			return;

		GSL_SUPPRESS(con.4) TCHAR szBuf[MIRC_BUFFER_SIZE_CCH]{};

		for (int nItem{}; nItem < nCnt; ++nItem)
		{
			ListBox_GetText(m_Hwnd, nItem, &szBuf[0]);

			TiXmlElement xChild("item");
			xChild.SetAttribute("text", TString(szBuf).c_str());
			// save selection state?

			xml->InsertEndChild(xChild);
		}
	}
}

TiXmlElement* DcxList::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxList::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

	{
		int nPos{};

		//// this adds basic items
		//// <item text="line text">
		//for (auto xItem = xThis->FirstChildElement("item"); xItem; xItem = xItem->NextSiblingElement("item"))
		//{
		//	if (auto tmp = queryAttribute(xItem, "text"); tmp)
		//		Dcx::dcxListBox_InsertString(m_Hwnd, nPos++, tmp);
		//}

		// this adds similar to /xdid -A
		// <item text="args, contents depend on flags" flags="flags for how to add">
		// if adding a basic item, then flags are not needed.
		for (auto xItem = xThis->FirstChildElement("item"); xItem; xItem = xItem->NextSiblingElement("item"))
		{
			const TString tsArgs(queryAttribute(xItem, "text"));
			const TString tsFlags(queryAttribute(xItem, "flags","+"));

			nPos = addItems(nPos, tsFlags, tsArgs);
		}

		// Now update the horizontal scroller
		UpdateHorizExtent();
	}
}

LRESULT DcxList::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
