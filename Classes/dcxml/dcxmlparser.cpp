/*!
 * \file dcxml.cpp
 * \brief blah
 *
 * TODO: rename all variables to use the same convention (camelCase)
 *
 * \author Martijn Laarman ( Mpdreamz at mirc dot net )
 * \version 2.2
 *
 * \b Revisions
 *        -Completely rewrote DCXML to be more OOP
 *		  -
 *
 * © ScriptsDB.org - 2006
 */
#include "defines.h"
#include "dcxmlparser.h"

 /*
 dcxml [-FLAGS] [DNAME] [DATASET] "[PATH]"
 */
bool DcxmlParser::ParseXML(const TString& tsFilePath, const TString& tsDialogMark, const TString& tsDialogName, const bool verbose, const bool autoClose)
{
	m_bLoadSuccess = true;

	isVerbose(verbose);
	isAutoClose(autoClose);

	setFilePath(tsFilePath);

	setDialogMark(tsDialogMark);
	setDialogName(tsDialogName);

	try {
		loadDialog();
		loadDocument();
		loadDialogElement();

		const auto tx = getDialogElement();

		if (!tx)
			throw std::runtime_error("Unable to get dialog element");

		//mIRCLinker::execex(TEXT("/dialog -s %s %i %i %i %i"), //!< Sets the dialog size.
		//	tsDialogMark.to_chr(),
		//	queryIntAttribute(tx, "x", -1),
		//	queryIntAttribute(tx, "y", -1),
		//	queryIntAttribute(tx, "w", -1),
		//	queryIntAttribute(tx, "h", -1)
		//	);
		mIRCLinker::exec(TEXT("/dialog -s % % % % %"), //!< Sets the dialog size.
			tsDialogMark,
			queryIntAttribute(tx, "x", -1),
			queryIntAttribute(tx, "y", -1),
			queryIntAttribute(tx, "w", -1),
			queryIntAttribute(tx, "h", -1)
		);

		setZlayered(false);

		const auto tCascade = queryAttribute(tx, "cascade", "v");
		const auto tMargin = queryAttribute(tx, "margin", "0 0 0 0");
		const auto tCaption = queryAttribute(tx, "caption", tsDialogName.c_str());
		const auto tBorder = tx->Attribute("border");

		m_iControls = 0;

		if (tBorder) //!< set border ONLY if defined on <dialog>
			xdialogEX(TEXT("-b"), TEXT("+%S"), tBorder);

		mIRCLinker::exec(TEXT("//dialog -t % %"), tsDialogMark, tCaption);
		xdialogEX(TEXT("-l"), TEXT("root \t +p%S 0 0 0 0"), tCascade);
		xdialogEX(TEXT("-l"), TEXT("space root \t + %S"), tMargin);

		parseDialog(); //!< Parse <dialog> children onto the dialog

		if (tx->Attribute("center")) //!< Centers the dialog
			mIRCLinker::exec(TEXT("/dialog -r %"), tsDialogMark);

		//This "Shite" is to activate the first zlayer, added a check if this command starts returning an error
		if (getZlayered())
			xdialogEX(TEXT("-z"), TEXT("+s 1"));

		mIRCLinker::exec(TEXT("/.timer 1 0 % % ready"), getDialog()->getAliasName(), tsDialogMark); //!< Tell user that dcxml is finished, & they can do a cla update or whatever.
		return m_bLoadSuccess;
	}
	catch (const std::exception)
	{
		m_bLoadSuccess = false;
		//if ((getDialog() != nullptr) && isAutoClose())
		//	mIRCLinker::execex(TEXT("/xdialog -x %s"), tsDialogName.to_chr());
		if ((getDialog()) && isAutoClose())
			mIRCLinker::exec(TEXT("/xdialog -x %"), tsDialogName);
		throw;
	}
}

void DcxmlParser::setDialog(const TString& tsDialogMark) noexcept
{
	m_pDcxDialog = Dcx::Dialogs.getDialogByName(tsDialogMark);
}

void DcxmlParser::loadDocument()
{
	auto tsPath(getFilePath());

	if (!IsFile(tsPath))
		throw Dcx::dcxException(TEXT("File \"%\" does not exist or is in use by another process "), getFilePath());

	TiXmlBase::SetCondenseWhiteSpace(false);
	TiXmlDocument doc(tsPath.c_str());
	m_bLoadSuccess = doc.LoadFile();
	if (!m_bLoadSuccess)
		throw Dcx::dcxException(TEXT("XML error in \"%\" (row % column %) %"), tsPath, doc.ErrorRow(), doc.ErrorCol(), doc.ErrorDesc());

	m_xmlDocument = doc;
}

void DcxmlParser::loadDialog()
{
	setDialog(getDialogMark());
	if (!getDialog())
		throw Dcx::dcxException(TEXT("No such dialog (\"%\") has been marked with dcx."), getDialogMark());
}

void DcxmlParser::loadDialogElement()
{
	setRootElement(getDocument()->FirstChildElement("dcxml"));
	if (!getRootElement())
		throw Dcx::dcxException(TEXT("The document element should be <dcxml> in \"%\""), getFilePath());

	setDialogsElement(getRootElement()->FirstChildElement("dialogs"));
	if (!getDialogsElement())
		throw Dcx::dcxException(TEXT("Theres no <dialogs> child for <dcxml> in \"%\""), getFilePath());

	/*
	* This finds the Dialog m_pElement.
	* Important Note:
	* The for loop has an ugly side assigment: it sets this->m_pElement
	* which is used as a recursion cursor for this->parseDialog()
	*/
	for (m_pElement = getDialogsElement()->FirstChildElement("dialog"); m_pElement; m_pElement = m_pElement->NextSiblingElement("dialog"))
	{
		if (getDialogName() == m_pElement->Attribute("name"))
		{
			setDialogElement(m_pElement);
			return;
		}
	}
	throw Dcx::dcxException(TEXT("Theres no <dialog> element with attribute name=\"%\" in \"%\""), getDialogName(), getFilePath());
}

void DcxmlParser::parseAttributes() noexcept
{
	parseAttributes(m_pElement);
}

void DcxmlParser::parseAttributes(const TiXmlElement* const tElement) noexcept
{
	if (!tElement || !m_pParent || !m_pElement)
		return;

	m_Attributes.m_sElem = tElement->Value();
	m_Attributes.m_sParentelem = m_pParent->Value();
	m_Attributes.m_sParenttype = queryAttribute(m_pParent, "type", "panel");
	m_Attributes.m_sType = queryAttribute(m_pElement, "type", "panel");
	m_Attributes.m_sSTclass = queryAttribute(m_pElement, "class");
	m_Attributes.m_sWeight = queryAttribute(tElement, "weight", "1");
	m_Attributes.m_sHeight = queryAttribute(tElement, "height", "0");
	m_Attributes.m_sDropdown = nullptr;

	if (const auto sElemHash = dcx_hash(m_Attributes.m_sElem); (sElemHash == "comboex"_hash) || (sElemHash == "colorcombo"_hash) || (sElemHash == "multicombo"_hash))
		m_Attributes.m_sDropdown = queryAttribute(tElement, "dropdown", "100");

	m_Attributes.m_sWidth = queryAttribute(tElement, "width", "0");
	m_Attributes.m_sMargin = queryAttribute(tElement, "margin", "0 0 0 0");
	m_Attributes.m_sStyles = queryAttribute(tElement, "styles", "");

	m_Attributes.m_sCaption = tElement->Attribute("caption");
	if (!m_Attributes.m_sCaption)
		m_Attributes.m_sCaption = tElement->GetText();

	m_Attributes.m_sTooltip = queryAttribute(tElement, "tooltip");
	m_Attributes.m_sCascade = queryAttribute(tElement, "cascade");
	m_Attributes.m_sIcon = queryAttribute(tElement, "icon", "0");
	m_Attributes.m_sIntegral = queryAttribute(tElement, "integral", "0");
	m_Attributes.m_sState = queryAttribute(tElement, "state", "0");
	m_Attributes.m_sIndent = queryAttribute(tElement, "indent", "0");
	//flags attribute defaults different per type/item
	m_Attributes.m_sTFlags = tElement->Attribute("flags");
	m_Attributes.m_sSrc = queryAttribute(tElement, "src");
	m_Attributes.m_sCells = queryAttribute(tElement, "cells", "-1");
	m_Attributes.m_sRebarMinHeight = queryAttribute(tElement, "minheight", "0");
	m_Attributes.m_sRebarMinWidth = queryAttribute(tElement, "minwidth", "0");
	m_Attributes.m_sIconsize = queryAttribute(tElement, "iconsize", "16");
	m_Attributes.m_iEval = queryIntAttribute(tElement, "eval");

	m_Attributes.m_sFontstyle = queryAttribute(tElement, "fontstyle", "d");
	m_Attributes.m_sCharset = queryAttribute(tElement, "charset", "ansi");
	m_Attributes.m_sFontsize = queryAttribute(tElement, "fontsize");
	m_Attributes.m_sFontname = queryAttribute(tElement, "fontname");
	m_Attributes.m_sBorder = queryAttribute(tElement, "border");
	m_Attributes.m_sCursor = queryAttribute(tElement, "cursor", "arrow");
	m_Attributes.m_sBgcolour = queryAttribute(tElement, "bgcolour", "0");
	m_Attributes.m_sTextbgcolour = queryAttribute(tElement, "textbgcolour");
	m_Attributes.m_sTextcolour = queryAttribute(tElement, "textcolour", "0");

	m_Attributes.m_sGradientstart = queryAttribute(tElement, "gradientstart");
	m_Attributes.m_sGradientend = queryAttribute(tElement, "gradientend");

	m_Attributes.m_sDisabledsrc = queryAttribute(tElement, "disabledsrc");
	m_Attributes.m_sHoversrc = queryAttribute(tElement, "hoversrc");
	m_Attributes.m_sSelectedsrc = queryAttribute(tElement, "selectedsrc");

	m_Attributes.m_sBuddyID = queryAttribute(tElement, "buddyid");
}

/* parseControl() : if current m_pElement is a control perform some extra commands*/
void DcxmlParser::parseControl()
{
	if (!m_pElement)
		return;

	if (m_pElement->Attribute("zlayer"))
	{
		xdialogEX(TEXT("-z"), TEXT("+a %i"), m_Attributes.m_iID);
		setZlayered(true);
	}

	switch (const auto nType = dcx_hash(m_Attributes.m_sType); nType)
	{
	case "divider"_hash:		//	divider
		xdidEX(m_Attributes.m_iID, TEXT("-v"), TEXT("%d"), queryIntAttribute(m_pElement, "width", 0));
		break;
	case "button"_hash:		//	button
		xdidEX(m_Attributes.m_iID, TEXT("-l"), TEXT("%S"), m_Attributes.m_sIconsize);
		[[fallthrough]];
	case "radio"_hash:		//	radio
		[[fallthrough]];
	case "link"_hash:		//	link
		[[fallthrough]];
	case "check"_hash:	//	check
		[[fallthrough]];
	case "box"_hash:	//	box
		if (m_Attributes.m_sCaption)
			xdidEX(m_Attributes.m_iID, TEXT("-t"), TEXT("%S"), m_Attributes.m_sCaption);
		break;
	case "toolbar"_hash:		//	toolbar
		[[fallthrough]];
	case "treeview"_hash:		//	treeview
		xdidEX(m_Attributes.m_iID, TEXT("-l"), TEXT("%S"), m_Attributes.m_sIconsize);
		[[fallthrough]];
	case "comboex"_hash:		//	comboex
		[[fallthrough]];
	case "list"_hash:			//	list
		[[fallthrough]];
	case "multicombo"_hash:		//	multicombo
		[[fallthrough]];
	case "listview"_hash:		//	listview
		parseItems(m_pElement);
		break;
	case "ipaddress"_hash:	//	ipaddress
		if (m_Attributes.m_sCaption)
			xdidEX(m_Attributes.m_iID, TEXT("-a"), TEXT("%S"), m_Attributes.m_sCaption);
		break;
	case "webctrl"_hash:	//	webctrl
		if (m_Attributes.m_sSrc)
			xdidEX(m_Attributes.m_iID, TEXT("-n"), TEXT("%S"), m_Attributes.m_sSrc);
		break;
	case "text"_hash:	//	text
		if (m_Attributes.m_sCaption)
		{
			TString mystring(m_Attributes.m_sCaption);

			if (mystring.left(2) == TEXT("\r\n"))
				mystring = mystring.right(-2);
			else if (mystring[0] == TEXT('\n'))
				mystring = mystring.right(-1);

			mystring -= TEXT('\t'); // remove all '\t' from text.

			int textspace = 0;

			TString printstring;
			for (const auto& tsTmp : mystring)
			{
				printstring.addtok(tsTmp);
				if (printstring.len() > (MIRC_BUFFER_SIZE_CCH - 100))
				{
					xdidEX(m_Attributes.m_iID, TEXT("-a"), TEXT("%i %s"), textspace, printstring.to_chr());
					printstring.clear();	// = TEXT("");
					textspace = 1;
				}
			}
			if (!printstring.empty())
				xdidEX(m_Attributes.m_iID, TEXT("-a"), TEXT("%i %s"), textspace, printstring.to_chr());
		}
		break;
	case "edit"_hash:	//	edit
		[[fallthrough]];
	case "richedit"_hash:	//	richedit
		if (m_Attributes.m_sCaption)
		{
			TString mystring(m_Attributes.m_sCaption);

			if (mystring.left(2) == TEXT("\r\n"))
				mystring = mystring.right(-2);
			else if (mystring[0] == TEXT('\n'))
				mystring = mystring.right(-1);

			mystring -= TEXT('\t'); // remove all tabs from text.  Ook: can crash here for some reason, needs looked at
			if (nType == "richedit"_hash)
			{	// richedit, converts \c -> ctrl-k, \b -> ctrl-b, \r -> ctrl-r, \u -> ctrl-u, \i -> ctrl-i, \o -> ctrl-o
				mystring.replace(TEXT("\\c"), TEXT('\x03'));	// mirc colours
				mystring.replace(TEXT("\\b"), TEXT('\x02'));	// bold
				mystring.replace(TEXT("\\r"), TEXT('\x16'));	// reverse
				mystring.replace(TEXT("\\u"), TEXT('\x1F'));	// underline
				mystring.replace(TEXT("\\i"), TEXT('\x1D'));	// italics
				mystring.replace(TEXT("\\o"), TEXT('\x0F'));	// ctrl-o
			}
			{
				UINT line = 0U;
				const auto itEnd = mystring.end();
				for (auto itStart = mystring.begin(TEXT("\r\n")); itStart != itEnd; ++itStart)
				{
					++line;
					xdidEX(m_Attributes.m_iID, TEXT("-i"), TEXT("%u %s"), line, (*itStart).to_chr());
				}
			}
		}
		break;
	case "pbar"_hash:	//	pbar
		if (m_Attributes.m_sCaption)
			xdidEX(m_Attributes.m_iID, TEXT("-i"), TEXT("%S"), m_Attributes.m_sCaption);
		break;
	case "statusbar"_hash:	//	statusbar
		xdidEX(m_Attributes.m_iID, TEXT("-l"), TEXT("%S"), m_Attributes.m_sCells);
		parseItems(m_pElement);
		break;
	case "image"_hash:	//	image
		if (m_Attributes.m_sSrc)
			xdidEX(m_Attributes.m_iID, TEXT("-i"), TEXT("+%S %S"), ((m_Attributes.m_sTFlags) ? m_Attributes.m_sTFlags : ""), m_Attributes.m_sSrc);
		break;
	case "updown"_hash:	//	updown
		if (m_Attributes.m_sBuddyID)
			xdidEX(m_Attributes.m_iID, TEXT("-c"), TEXT("%S"), m_Attributes.m_sBuddyID);
		break;
	default:	//	unknown?!?!?!
		break;
	}

	m_Attributes.m_sDisabledsrc = queryAttribute(m_pElement, "disabledsrc");
	m_Attributes.m_sHoversrc = queryAttribute(m_pElement, "hoversrc");
}

/* xdialogEX(switch,format[,args[]]) : performs an xdialog command internally or through mIRC */
GSL_SUPPRESS(es.47)
GSL_SUPPRESS(type.3)
GSL_SUPPRESS(f.55)
void DcxmlParser::xdialogEX(const TCHAR* const sw, const TCHAR* const dFormat, ...)
{
	if (!sw || !dFormat)
		return;

	TString txt;

	{
		va_list args{};

		va_start(args, dFormat);
		txt.tvprintf(dFormat, args);
		va_end(args);
	}

	xml_xdialog(sw, txt);
}

void DcxmlParser::xml_xdialog(const TCHAR* const sSwitch, const TString& sArgs)
{
#if DCX_DEBUG_OUTPUT
	if (isVerbose())
		mIRCLinker::echo(TEXT("dcxml debug: /xdialog % % %"), sSwitch, getDialogMark(), sArgs);
#endif

	if (m_Attributes.m_iEval > 0) mIRCLinker::exec(TEXT("//xdialog % % %"), sSwitch, getDialogMark(), sArgs);
	else {
		if (auto d = getDialog(); d)
			d->parseCommandRequestEX(TEXT("%s %s %s"), getDialogMark().to_chr(), sSwitch, sArgs.to_chr());
	}
}

/* xdidEX(controlId,switch,args) : performs an xdid command internally or through mIRC on the specified ID */
void DcxmlParser::xml_xdid(const UINT cid, const TCHAR* const sSwitch, const TString& sArgs)
{
#if DCX_DEBUG_OUTPUT
	if (isVerbose())
		mIRCLinker::exec(TEXT("/echo -a dcxml debug: /xdid % % % %"), sSwitch, getDialogMark(), cid, sArgs);
#endif

	if (m_Attributes.m_iEval > 0) mIRCLinker::exec(TEXT("//xdid % % % %"), sSwitch, getDialogMark(), cid, sArgs);
	else {
		//getDialog()->parseComControlRequestEX(cid, TEXT("%s %u %s %s"), getDialogMark().to_chr(), cid, sSwitch, sArgs.to_chr());
		if (auto d = getDialog(); d)
			d->parseComControlRequestEX(cid, TEXT("%s %u %s %s"), getDialogMark().to_chr(), cid, sSwitch, sArgs.to_chr());
	}
}

/* xdidEX(controlId,switch,format[,args[]]) : performs an xdid command internally or through mIRC on the specified ID */
GSL_SUPPRESS(es.47)
GSL_SUPPRESS(type.3)
GSL_SUPPRESS(f.55)
void DcxmlParser::xdidEX(const UINT cid, const TCHAR* const sw, const TCHAR* const dFormat, ...)
{
	TString txt;

	{
		va_list args{};

		va_start(args, dFormat);
		txt.tvprintf(dFormat, args);
		va_end(args);
	}

	xml_xdid(cid, sw, txt);
}

/* parseCLA(int numberOfClaControlsInCurrentBranch) : parses control and pane elements and applies the right CLA commands */
TString DcxmlParser::parseCLA(const int cCla)
{
	const auto sParentelemHash = dcx_hash(m_Attributes.m_sParentelem);

	if (const auto sElemHash = dcx_hash(m_Attributes.m_sElem); sElemHash == "control"_hash)
	{
		if (const auto sTypeHash = dcx_hash(m_Attributes.m_sType); ((sTypeHash == "panel"_hash) || (sTypeHash == "box"_hash)))
		{
			xdidEX(m_Attributes.m_iID, TEXT("-l"), TEXT("root \t +p%S 0 0 0 0"), m_Attributes.m_sCascade);
			xdidEX(m_Attributes.m_iID, TEXT("-l"), TEXT("space root \t + %S"), m_Attributes.m_sMargin);
			m_CLA.m_bResetCLA = true;
		}
		const char* fHeigth = "";
		const char* fWidth = "";
		const char* fixed = "l";
		if (m_pElement->Attribute("height"))
		{
			fHeigth = "v";
			fixed = "f";
			m_Attributes.m_sWeight = "0";
		}
		if (m_pElement->Attribute("width"))
		{
			fWidth = "h";
			fixed = "f";
			m_Attributes.m_sWeight = "0";
		}

		if (sParentelemHash == "dialog"_hash)
			xdialogEX(TEXT("-l"), TEXT("cell %S \t +%S%S%Si %u %S %S %S"), m_CLA.m_claPath, fixed, fHeigth, fWidth, m_Attributes.m_iID, m_Attributes.m_sWeight, m_Attributes.m_sWidth, m_Attributes.m_sHeight);
		else if (sParentelemHash == "control"_hash)
		{
			if (m_pParent)
			{
				if (const char* const t_type = m_pParent->Attribute("type"); ((t_type) && (m_Attributes.m_iParentID > 0)))
				{
					if (const auto t_typeHash = dcx_hash(t_type); ((t_typeHash == "panel"_hash) || (t_typeHash == "box"_hash)))
						xdidEX(m_Attributes.m_iParentID, TEXT("-l"), TEXT("cell %S \t +%S%S%Si %u %S %S %S"), m_CLA.m_claPath, fixed, fHeigth, fWidth, m_Attributes.m_iID, m_Attributes.m_sWeight, m_Attributes.m_sWidth, m_Attributes.m_sHeight);
				}
			}
		}
	}
	else if (sElemHash == "pane"_hash)
	{
		if (sParentelemHash == "dialog"_hash)
			xdialogEX(TEXT("-l"), TEXT("cell %S \t +p%S 0 %S 0 0"), m_CLA.m_claPath, m_Attributes.m_sCascade, m_Attributes.m_sWeight);
		else if (sParentelemHash == "control"_hash)
		{
			if ((m_Attributes.m_sParenttype) && (m_Attributes.m_iParentID > 0))
			{
				if (const auto sParenttypeHash = dcx_hash(m_Attributes.m_sParenttype); ((sParenttypeHash == "panel"_hash) || (sParenttypeHash == "box"_hash)))
					xdidEX(m_Attributes.m_iParentID, TEXT("-l"), TEXT("cell %S \t +p%S 0 %S 0 0"), m_CLA.m_claPath, m_Attributes.m_sCascade, m_Attributes.m_sWeight);
			}
		}
	}

	TString claPathx;

	if (m_CLA.m_bResetCLA)
		claPathx = TEXT("root");
	else
	{
		if (0 != ts_strcmp(m_CLA.m_claPath, "root"))
			claPathx = m_CLA.m_claPath;

		claPathx.addtok(cCla);
	}

	if (m_pElement->Attribute("margin"))
	{
		if (sParentelemHash == "dialog"_hash)
			xdialogEX(TEXT("-l"), TEXT("space %s \t + %S"), claPathx.to_chr(), m_Attributes.m_sMargin);
		else
			xdidEX(m_Attributes.m_iParentID, TEXT("-l"), TEXT("space %S \t + %S"), m_CLA.m_claPath, m_Attributes.m_sMargin);
	}
	m_CLA.m_bResetCLA = false;
	return claPathx;
}

/* setStyle(TiXmlElement*) : Applies the styles described on the m_pElement found by parseStyle() */
void DcxmlParser::setStyle(const TiXmlElement* const style)
{
	if (!style)
		return;

	//style attributes evaluate by default unless eval="0" is set on the m_pElement explicitly

	m_Attributes.m_iEval = queryIntAttribute(style, "eval", 1);

	//font
	m_Attributes.m_sFontstyle = queryAttribute(style, "fontstyle", "d");
	m_Attributes.m_sCharset = queryAttribute(style, "charset", "ansi");
	m_Attributes.m_sFontsize = queryAttribute(style, "fontsize");
	m_Attributes.m_sFontname = queryAttribute(style, "fontname");
	if ((style->Attribute("fontsize")) || (style->Attribute("fontname")))
		xdidEX(m_Attributes.m_iID, TEXT("-f"), TEXT("+%S %S %S %S"), m_Attributes.m_sFontstyle, m_Attributes.m_sCharset, m_Attributes.m_sFontsize, m_Attributes.m_sFontname);
	//border
	m_Attributes.m_sBorder = queryAttribute(style, "border");
	if (!_ts_isEmpty(m_Attributes.m_sBorder))
		xdidEX(m_Attributes.m_iID, TEXT("-x"), TEXT("+%S"), m_Attributes.m_sBorder);

	//m_sCursor = queryAttribute(style, "cursor", "arrow");
	m_Attributes.m_sCursor = style->Attribute("cursor");	// can be null

	//colours
	m_Attributes.m_sBgcolour = queryAttribute(style, "bgcolour");
	m_Attributes.m_sTextbgcolour = queryAttribute(style, "textbgcolour");
	m_Attributes.m_sTextcolour = queryAttribute(style, "textcolour");

	const auto sTypeHash = dcx_hash(m_Attributes.m_sType);

	if (!_ts_isEmpty(m_Attributes.m_sBgcolour))
	{
		xdidEX(m_Attributes.m_iID, TEXT("-C"), TEXT("+b %S"), m_Attributes.m_sBgcolour);
		if (sTypeHash == "pbar"_hash)
		{
			xdidEX(m_Attributes.m_iID, TEXT("-k"), TEXT("%S"), m_Attributes.m_sBgcolour);
			xml_xdid(m_Attributes.m_iID, TEXT("-U"), TEXT(""));
		}
	}

	if (!_ts_isEmpty(m_Attributes.m_sTextbgcolour))
	{
		xdidEX(m_Attributes.m_iID, TEXT("-C"), TEXT("+k %S"), m_Attributes.m_sTextbgcolour);
		if (sTypeHash == "pbar"_hash)
		{
			xdidEX(m_Attributes.m_iID, TEXT("-c"), TEXT("%S"), m_Attributes.m_sTextbgcolour);
			xml_xdid(m_Attributes.m_iID, TEXT("-U"), TEXT(""));
		}
	}
	else if (!_ts_isEmpty(m_Attributes.m_sBgcolour))
		xdidEX(m_Attributes.m_iID, TEXT("-C"), TEXT("+k %S"), m_Attributes.m_sBgcolour);

	if (!_ts_isEmpty(m_Attributes.m_sTextcolour))
	{
		xdidEX(m_Attributes.m_iID, TEXT("-C"), TEXT("+t %S"), m_Attributes.m_sTextcolour);
		if (sTypeHash == "pbar"_hash)
		{
			xdidEX(m_Attributes.m_iID, TEXT("-q"), TEXT("%S"), m_Attributes.m_sTextcolour);
			xml_xdid(m_Attributes.m_iID, TEXT("-U"), TEXT(""));
		}
	}

	if (!_ts_isEmpty(m_Attributes.m_sGradientstart))
		xdidEX(m_Attributes.m_iID, TEXT("-C"), TEXT("+g %S"), m_Attributes.m_sGradientstart);
	if (!_ts_isEmpty(m_Attributes.m_sGradientstart))
		xdidEX(m_Attributes.m_iID, TEXT("-C"), TEXT("+G %S"), m_Attributes.m_sGradientend);

	//cursor
	if (!_ts_isEmpty(m_Attributes.m_sCursor))
		xdidEX(m_Attributes.m_iID, TEXT("-J"), TEXT("+r %S"), m_Attributes.m_sCursor);	// Ook: assumes cursor is a resource, needs changed.

	//iconsize
	if (style->Attribute("iconsize"))
	{
		if (((sTypeHash == "toolbar"_hash) || (sTypeHash == "button"_hash)) || (sTypeHash == "treeview"_hash))
			xdidEX(m_Attributes.m_iID, TEXT("-l"), TEXT("%S"), m_Attributes.m_sIconsize);
	}
	if (sTypeHash == "button"_hash)
	{
		if (m_pElement)
		{
			if (!m_pElement->Attribute("bgcolour"))
				m_Attributes.m_sBgcolour = "65280";
			if (m_pElement->Attribute("src"))
				xdidEX(m_Attributes.m_iID, TEXT("-k"), TEXT("+n %S %S"), m_Attributes.m_sBgcolour, m_Attributes.m_sSrc);
			if (m_pElement->Attribute("disabledsrc"))
				xdidEX(m_Attributes.m_iID, TEXT("-k"), TEXT("+d %S %S"), m_Attributes.m_sBgcolour, m_Attributes.m_sDisabledsrc);
			if (m_pElement->Attribute("hoversrc"))
				xdidEX(m_Attributes.m_iID, TEXT("-k"), TEXT("+h %S %S"), m_Attributes.m_sBgcolour, m_Attributes.m_sHoversrc);
			if (m_pElement->Attribute("selectedsrc"))
				xdidEX(m_Attributes.m_iID, TEXT("-k"), TEXT("+s %S %S"), m_Attributes.m_sBgcolour, m_Attributes.m_sSelectedsrc);
		}
	}
}

/* parseStyle(recursionDepth) : Simple recursive method to cascade find the right style to apply to an m_pElement */
void DcxmlParser::parseStyle(int depth)
{
	if (depth > 2)
		return;

	++depth;

	const TiXmlElement* tiStyles{ nullptr };
	const TiXmlElement* style{ nullptr };
	if (depth == 3)
		style = m_pElement;
	else if (depth == 1)
		tiStyles = getDialogsElement()->FirstChildElement("styles");	// find styles for ALL dialogs
	else if (depth == 2)
		tiStyles = getDialogElement()->FirstChildElement("styles");		// find styles for THIS dialog

	if (tiStyles)
	{
		style = tiStyles->FirstChildElement("all");		// find styles for ALL controls
		if (style != nullptr)
			setStyle(style);

		const TiXmlElement* ClassElement{ nullptr };
		const TiXmlElement* TypeElement{ nullptr };
		const TiXmlElement* IdElement{ nullptr };

		for (style = tiStyles->FirstChildElement("style"); style; style = style->NextSiblingElement())
		{
			if (const auto ctmp = style->Attribute("class"); ctmp)
			{
				if (0 == ts_strcmp(ctmp, m_Attributes.m_sSTclass))
					ClassElement = style;
			}

			if (const auto ctmp = style->Attribute("type"); ctmp)
			{
				if (0 == ts_strcmp(ctmp, m_Attributes.m_sType))
					TypeElement = style;
			}
			if (parseId(style) == m_Attributes.m_iID)
				IdElement = style;
		}
		if (IdElement)
			style = IdElement;
		else if (ClassElement)
			style = ClassElement;
		else if (TypeElement)
			style = TypeElement;
	}
	if (style)
		setStyle(style);
	parseStyle(depth);
}

/* parseIcons(recursionDepth) : Simple recursive method to cascade find the right icons to apply to an m_pElement */
void DcxmlParser::parseIcons(int depth)
{
	if (depth > 1)
		return;

	++depth;

	const TiXmlElement* icons{ nullptr };
	if (depth == 1)
		icons = getDialogElement()->FirstChildElement("icons");
	else if (depth == 2)
		icons = getDialogsElement()->FirstChildElement("icons");

	if (icons)
	{
		const TiXmlElement* tiIcon{ nullptr };
		const TiXmlElement* IdElement{ nullptr };
		const TiXmlElement* TypeElement{ nullptr };
		const TiXmlElement* ClassElement{ nullptr };

		for (tiIcon = icons->FirstChildElement("icon"); tiIcon; tiIcon = tiIcon->NextSiblingElement())
		{
			if (const auto ctmp = tiIcon->Attribute("class"); ctmp)
			{
				if (0 == ts_strcmp(ctmp, m_Attributes.m_sSTclass))
					ClassElement = tiIcon;
			}

			if (const auto ctmp = tiIcon->Attribute("type"); ctmp)
			{
				if (0 == ts_strcmp(ctmp, m_Attributes.m_sType))
					TypeElement = tiIcon;
			}

			if (m_Attributes.m_iID == parseId(tiIcon))
				IdElement = tiIcon;
		}

		if (IdElement)
			tiIcon = IdElement;
		else if (ClassElement)
			tiIcon = ClassElement;
		else if (TypeElement)
			tiIcon = TypeElement;
		else
			tiIcon = nullptr;

		if (tiIcon)
		{
			const auto flags = queryAttribute(tiIcon, "flags", "n");
			const auto index = queryAttribute(tiIcon, "index", "0");
			const auto tIconSrc = tiIcon->Attribute("src");
			const auto indexmin = queryIntAttribute(tiIcon, "indexmin", 0);
			const auto indexmax = queryIntAttribute(tiIcon, "indexmax", -1);
			if (tIconSrc)
			{
				// Ook: change this to call /xdid -w with an index range
				if (indexmin <= indexmax)
					//method sucks but looping in C++ is WAYYY too fast for mIRC
				{
					// NB: last %x does NOT have the % escaped, this is due to how _ts_sprintf() works when u have more % than args.
					mIRCLinker::exec(TEXT("//var \\%x = % | while (\\%x <= % ) { xdid -w % % +% \\%x % | inc %x }"), indexmin, indexmax, getDialogMark(), m_Attributes.m_iID, flags, tIconSrc);
					//mIRCLinker::exec(TEXT("//xdid -w % % +% %-% %"), getDialogMark(), m_Attributes.m_iID, flags, indexmin, indexmax, tIconSrc);
				}
				else
					mIRCLinker::exec(TEXT("//xdid -w % % +% % %"), getDialogMark(), m_Attributes.m_iID, flags, index, tIconSrc);
			}
			else
			{
				for (auto iconchild = tiIcon->FirstChildElement("icon"); iconchild; iconchild = iconchild->NextSiblingElement())
				{
					const auto tflags = queryAttribute(iconchild, "flags", "n");
					const auto tindex = queryAttribute(iconchild, "index", "0");

					if (const auto tsrc = iconchild->Attribute("src"); tsrc)
						mIRCLinker::exec(TEXT("//xdid -w % % +% % %"), getDialogMark(), m_Attributes.m_iID, tflags, tindex, tsrc);
				}
			}
		}
	}
	else
		parseIcons(depth);
}

/* parseItems(XmlElement,recursionDepth,itemPath) : recursively applies items for a control */
void DcxmlParser::parseItems(const TiXmlElement* const tiElement, const UINT depth, const char* const itemPath)
{
	if (!tiElement)
		return;

	auto item = 0, cell = 0;
	const auto sTypeHash = dcx_hash(m_Attributes.m_sType);

	for (auto child = tiElement->FirstChildElement(); child; child = child->NextSiblingElement())
	{
		const auto childelem = child->Value();
		const auto childelemHash = dcx_hash(childelem);

		if (childelemHash == "columns"_hash)
		{
			if (sTypeHash == "listview"_hash)
			{
				TString tsArguments;
				TString tsBuffer;
				for (auto column = child->FirstChildElement("column"); column; column = column->NextSiblingElement("column"))
				{
					//_ts_snprintf(tsBuffer, TEXT("+%S %S %S %S "),
					//	queryAttribute(column, "flags", "l"),
					//	queryAttribute(column, "icon", "0"),
					//	queryAttribute(column, "width", "0"),
					//	queryAttribute(column, "caption"));
					//
					//tsArguments.addtok(tsBuffer, TEXT('\t'));

					tsBuffer.tsprintf(TEXT("+%S %S %S %S "),
						queryAttribute(column, "flags", "l"),
						queryAttribute(column, "icon", "0"),
						queryAttribute(column, "width", "0"),
						queryAttribute(column, "caption"));

					tsArguments.addtok(tsBuffer, TEXT('\t'));

					//_ts_sprintf(tsBuffer, TEXT("+% % % % "),
					//	queryAttribute(column, "flags", "l"),
					//	queryAttribute(column, "icon", "0"),
					//	queryAttribute(column, "width", "0"),
					//	queryAttribute(column, "caption"));
					//
					//tsArguments.addtok(tsBuffer,TEXT('\t'));
				}
				if (!tsArguments.empty())
					xdidEX(m_Attributes.m_iID, TEXT("-t"), TEXT("%s"), tsArguments.to_chr());
			}
			//if (childelemHash == "dataset"_hash)
			//{
			//	//auto listView = this->d_Host->getControlByID(m_Attributes.m_iID);
			//}
		}


		if ((childelemHash == "item"_hash) || (childelemHash == "control"_hash))
			++cell;
		//fill all required parameters with attributes or default values
		parseAttributes(child);
		if (childelemHash == "item"_hash)
		{
			++item;
			if (sTypeHash == "toolbar"_hash)
				xdidEX(m_Attributes.m_iID, TEXT("-a"), TEXT("0 +%S %S %S %S %S \t %S"), ((m_Attributes.m_sTFlags) ? m_Attributes.m_sTFlags : "a"), m_Attributes.m_sWidth, m_Attributes.m_sIcon, m_Attributes.m_sTextcolour, m_Attributes.m_sCaption, m_Attributes.m_sTooltip);
			else if (sTypeHash == "comboex"_hash)
				xdidEX(m_Attributes.m_iID, TEXT("-a"), TEXT("0 %S %S %S 0 %S"), m_Attributes.m_sIndent, m_Attributes.m_sIcon, m_Attributes.m_sIcon, m_Attributes.m_sCaption);
			else if (sTypeHash == "list"_hash)
			{
				if (m_Attributes.m_sCaption)
					xdidEX(m_Attributes.m_iID, TEXT("-a"), TEXT("0 %S"), m_Attributes.m_sCaption);
			}
			else if (sTypeHash == "statusbar"_hash)
			{
				if (m_Attributes.m_sCaption)
					xdidEX(m_Attributes.m_iID, TEXT("-t"), TEXT("%i +%S %S %S \t %S"), cell, ((m_Attributes.m_sTFlags) ? m_Attributes.m_sTFlags : "f"), m_Attributes.m_sIcon, m_Attributes.m_sCaption, m_Attributes.m_sTooltip);
			}
			else if (sTypeHash == "treeview"_hash)
			{
				// Ook: recursive loop needs looked at, use a TString reference object instead of multiple char[]'s for itemPath
				//itemPath += item;
				//xdidEX(m_Attributes.m_iID, TEXT("-a"), TEXT("%s \t +%S %S %S 0 %S %S %S %S %S \t %S"), itemPath.to_chr(), ((m_sTFlags) ? m_sTFlags : "a"), m_sIcon, m_sIcon, m_sState, m_sIntegral, m_sTextcolour, m_Attributes.m_sBgcolour, m_sCaption, m_sTooltip);
				//parseItems(child, depth, itemPath);
				char pathx[100]{};
				//wnsprintfA(&pathx[0], gsl::narrow_cast<int>(Dcx::countof(pathx)), "%s %i", itemPath, item);
				//_ts_snprintf(&pathx[0], gsl::narrow_cast<int>(Dcx::countof(pathx)), "%s %i", itemPath, item);
				_ts_snprintf(&pathx[0], std::size(pathx), "%s %i", itemPath, item);
				if (m_Attributes.m_sCaption)
					xdidEX(m_Attributes.m_iID, TEXT("-a"), TEXT("%S \t +%S %S %S 0 %S %S %S %S %S \t %S"), &pathx[0], ((m_Attributes.m_sTFlags) ? m_Attributes.m_sTFlags : "a"), m_Attributes.m_sIcon, m_Attributes.m_sIcon, m_Attributes.m_sState, m_Attributes.m_sIntegral, m_Attributes.m_sTextcolour, m_Attributes.m_sBgcolour, m_Attributes.m_sCaption, m_Attributes.m_sTooltip);
				parseItems(child, depth, &pathx[0]);
			}
		}
	}
}

/* parseTemplate(recursionDepth,claPath,firstFreeControlId) : finds a template and parses it into the current dialog */
void DcxmlParser::parseTemplate(const UINT dialogDepth, const char* const claPath, const UINT passedid)
{
	// find templates element
	const auto lookIn = getDialogsElement()->FirstChildElement("templates");

	if (!lookIn)
		return;	// no templates found, exit

	// iterate through all templates
	//for (auto Template = lookIn->FirstChildElement("template"); Template != nullptr; Template = Template->NextSiblingElement())
	//{
	//	if (0 == ts_strcmp(Template->Attribute("name"), m_pElement->Attribute("name")))
	//	{ 
	//		m_pElement = Template;
	//		parseDialog(dialogDepth, claPath, passedid, 1);
	//		break;
	//	}
	//}

	if (!m_pElement)
		return;

	const refString<const char> sTmp(m_pElement->Attribute("name"));

	for (auto Template = lookIn->FirstChildElement("template"); Template; Template = Template->NextSiblingElement())
	{
		if (sTmp == Template->Attribute("name"))
		{
			m_pElement = Template;
			parseDialog(dialogDepth, claPath, passedid, 1);
			break;
		}
	}
}

/* parseDialog(recursionDepth,claPath,firstFreeControlId,ignoreParentFlag) : finds a template and parses it into the current dialog */
void DcxmlParser::parseDialog(const UINT depth, const char* claPath, const UINT passedid, const bool ignoreParent)
{
	auto control = 0, cCla = 0, cell = 0;
	m_CLA.m_claPath = nullptr;
	m_CLA.m_claPathx = nullptr;
	m_CLA.m_bResetCLA = false;

	if (!m_pElement)
		return;

	for (auto child = m_pElement->FirstChildElement(); child; child = child->NextSiblingElement())
	{
		++cell;
		//STEP 1: SET ELEMENT AND PARENTELEMENT
		if (!ignoreParent)
			m_pParent = child->Parent()->ToElement();
		m_pElement = child->ToElement();

		//STEP 2: PARSE ATTRIBUTES OF ELEMENTS
		parseAttributes();

		const auto sElemHash = dcx_hash(m_Attributes.m_sElem);

		//dont itterate over unneccessary items
		if (sElemHash == "calltemplate"_hash)
		{
			if (const refString<const char> refParentVal(m_pElement->Parent()->ToElement()->Value()); refParentVal != "template")
			{
				++cCla;
				m_Templates.m_pTemplateRef = m_pElement;
				m_Templates.m_iTemplateRefcCla = cCla;

				sString<100> t_buffer;
				const char* t_claPathx = t_buffer;
				//wnsprintfA(t_buffer, static_cast<int>(t_buffer.size()), "%i", cCla);
				_ts_snprintf(t_buffer, "%i", cCla);

				//for (auto attribute = m_pElement->FirstAttribute(); attribute; attribute = attribute->Next())
				//{
				//	const refString<const char, -1> refName(attribute->Name());
				//	if (refName == "name")
				//		continue;
				//	m_mTemplate_vars[refName] = attribute->Value();
				//}
				//for (auto iter = m_mTemplate_vars.begin(); iter != m_mTemplate_vars.end(); ++iter)
				//{
				//	//mIRCLinker::execex(TEXT("//set %%%S %S"), iter->first, iter->second);
				//	mIRCLinker::exec(TEXT("//set \\%% %"), iter->first, iter->second);
				//}
				//m_sTemplateRefclaPath = t_claPathx;
				//parseTemplate(depth, claPath, passedid);
				//m_pTemplateRef = nullptr;
				//for (auto iter = m_mTemplate_vars.begin(); iter != m_mTemplate_vars.end(); ++iter)
				//{
				//	//mIRCLinker::execex(TEXT("//unset %%%S"), iter->first);
				//	mIRCLinker::exec(TEXT("//unset \\%%"), iter->first);
				//}

				if (auto attrlist = m_pElement->FirstAttribute(); attrlist)
				{
					for (const auto& attribute : *attrlist)
					{
						//if (const refString<const char> refName(attribute->Name()); refName)
						//{
						//	if (refName == "name")
						//		continue;
						//	m_mTemplate_vars[refName] = attribute->Value();
						//}

						if (attribute)
						{
							if (const auto refName = attribute->Name(); refName)
							{
								if (ts_strcmp(refName, "name") == 0)
									continue;
								m_Templates.m_mTemplate_vars[refName] = attribute->Value();
							}
						}
					}
				}
				for (const auto& x : m_Templates.m_mTemplate_vars)
				{
					mIRCLinker::exec(TEXT("//set \\%% %"), x.first, x.second);
				}
				m_Templates.m_sTemplateRefclaPath = t_claPathx;
				parseTemplate(depth, claPath, passedid);
				m_Templates.m_pTemplateRef = nullptr;
				for (const auto& x : m_Templates.m_mTemplate_vars)
				{
					mIRCLinker::exec(TEXT("//unset \\%%"), x.first);
				}
				m_Templates.m_mTemplate_vars.clear();
			}
			continue;
		}
		if ((sElemHash == "control"_hash) || (sElemHash == "pane"_hash))
			++cCla;
		else
			continue;

		//assign ID 
		if (sElemHash == "control"_hash)
		{
			++m_iControls;
			m_Attributes.m_iID = parseId(m_pElement);

			if (m_Attributes.m_iID == 0)
				m_Attributes.m_iID = DCXML_ID_OFFSET + m_iControls;	// this is user id, so will later have mIRC_ID_OFFSET added to it.

			registerId(m_pElement, m_Attributes.m_iID);
		}
		else
			m_Attributes.m_iID = passedid;

		//assign m_pParent CONTROL of m_pElement
		if (m_Attributes.m_sParentelem)
		{
			while (m_pParent)
			{
				if (0 == ts_strcmp(m_Attributes.m_sParentelem, "template"))
				{
					if (m_Templates.m_pTemplateRef)
					{
						const gsl::not_null<const TiXmlNode*> xNodeTmp(m_Templates.m_pTemplateRef->Parent());
						m_pParent = xNodeTmp->ToElement();
						m_Attributes.m_sParentelem = xNodeTmp->Value();
						cCla = m_Templates.m_iTemplateRefcCla;
						claPath = m_Templates.m_sTemplateRefclaPath;
					}
				}
				else if (0 == ts_strcmp(m_Attributes.m_sParentelem, "pane"))
				{
					m_pParent = m_pParent->Parent()->ToElement();
					if (m_pParent)
						m_Attributes.m_sParentelem = m_pParent->Value();
				}
				else break;
			}
		}
		if (m_pParent)
			m_Attributes.m_sParenttype = queryAttribute(m_pParent, "type", "panel");
		else
			m_Attributes.m_sParenttype = "panel";

		m_Attributes.m_iParentID = parseId(m_pParent);
		if (m_Attributes.m_iParentID <= 0)
			m_Attributes.m_iParentID = passedid;
		//IF TEMPLATE ELEMENT REROUTE TO TEMPLATE DEFINITION


		//STEP 3: IF CONTROL CREATE IT AND ITS ITEMS
		if (sElemHash == "control"_hash)
		{
			++control;
			// control now always > 0

			//check how to insert the control in the m_pParent Control/Dialog
			//If parentNode is pane loop untill parentNode is not a pane
			const auto sParentelemHash = dcx_hash(m_Attributes.m_sParentelem);

			if (sParentelemHash == "dialog"_hash)
				xdialogEX(TEXT("-c"), TEXT("%u %S 0 0 %S %S %S"),
					m_Attributes.m_iID, m_Attributes.m_sType, m_Attributes.m_sWidth,
					(m_Attributes.m_sDropdown ? m_Attributes.m_sDropdown : m_Attributes.m_sHeight), m_Attributes.m_sStyles);
			else if (sParentelemHash == "control"_hash)
			{
				switch (dcx_hash(m_Attributes.m_sParenttype))
				{
				case "pager"_hash:
					if (control != 1)
						break;
					[[fallthrough]];
				case "panel"_hash:
					[[fallthrough]];
				case "box"_hash:
					xdidEX(m_Attributes.m_iParentID, TEXT("-c"), TEXT("%u %S 0 0 %S %S %S"),
						m_Attributes.m_iID, m_Attributes.m_sType, m_Attributes.m_sWidth,
						(m_Attributes.m_sDropdown ? m_Attributes.m_sDropdown : m_Attributes.m_sHeight), m_Attributes.m_sStyles);
					break;
				case "tab"_hash:
					xdidEX(m_Attributes.m_iParentID, TEXT("-a"), TEXT("0 %S %S \t %u %S 0 0 %S %S %S \t %S"),
						m_Attributes.m_sIcon, m_Attributes.m_sCaption, m_Attributes.m_iID, m_Attributes.m_sType, m_Attributes.m_sWidth,
						(m_Attributes.m_sDropdown ? m_Attributes.m_sDropdown : m_Attributes.m_sHeight), m_Attributes.m_sStyles, m_Attributes.m_sTooltip);
					break;
				case "divider"_hash:
					if (control <= 2)
					{
						// <= 2 so MUST be either 1 or 2, can't be zero
						if (control == 1)
							xdidEX(m_Attributes.m_iParentID, TEXT("-l"), TEXT("%S 0 \t %u %S 0 0 %S %S %S"),
								m_Attributes.m_sWidth, m_Attributes.m_iID, m_Attributes.m_sType, m_Attributes.m_sWidth,
								(m_Attributes.m_sDropdown ? m_Attributes.m_sDropdown : m_Attributes.m_sHeight), m_Attributes.m_sStyles);
						else
							xdidEX(m_Attributes.m_iParentID, TEXT("-r"), TEXT("%S 0 \t %u %S 0 0 %S %S %S"),
								m_Attributes.m_sWidth, m_Attributes.m_iID, m_Attributes.m_sType, m_Attributes.m_sWidth,
								(m_Attributes.m_sDropdown ? m_Attributes.m_sDropdown : m_Attributes.m_sHeight), m_Attributes.m_sStyles);
					}
					break;
				case "rebar"_hash:
				{
					const char* flags = (m_Attributes.m_sTFlags) ? m_Attributes.m_sTFlags : "ceg";
					xdidEX(m_Attributes.m_iParentID, TEXT("-a"), TEXT("0 +%S %S %S %S %S %S %S \t %u %S 0 0 %S %S %S \t %S"), flags,
						m_Attributes.m_sRebarMinWidth, m_Attributes.m_sRebarMinHeight, m_Attributes.m_sWidth, m_Attributes.m_sIcon, m_Attributes.m_sTextcolour,
						m_Attributes.m_sCaption, m_Attributes.m_iID, m_Attributes.m_sType, m_Attributes.m_sWidth,
						(m_Attributes.m_sDropdown ? m_Attributes.m_sDropdown : m_Attributes.m_sHeight), m_Attributes.m_sStyles, m_Attributes.m_sTooltip);
				}
				break;
				case "stacker"_hash:
					xdidEX(m_Attributes.m_iParentID, TEXT("-a"), TEXT("0 + %S %S %S \t %u %S 0 0 %S %S %S"),
						m_Attributes.m_sTextcolour, m_Attributes.m_sBgcolour, m_Attributes.m_sCaption, m_Attributes.m_iID, m_Attributes.m_sType, m_Attributes.m_sWidth,
						(m_Attributes.m_sDropdown ? m_Attributes.m_sDropdown : m_Attributes.m_sHeight), m_Attributes.m_sStyles);
					break;
				case "statusbar"_hash:
					xdidEX(m_Attributes.m_iParentID, TEXT("-t"), TEXT("%i +c %S %u %S 0 0 0 0 %S"), cell,
						m_Attributes.m_sIcon, m_Attributes.m_iID, m_Attributes.m_sType, m_Attributes.m_sStyles);
					break;

				//Ook: MUST add MultiCombo !!

				default:
					break;
				}
			}
		}
		//Set CLA for control or pane
		m_CLA.m_claPath = claPath;
		const auto claPathx(parseCLA(cCla));

		//Perform some control specific commands
		if (sElemHash == "control"_hash)
		{
			parseControl();
			parseIcons();
			parseStyle();
		}
		//char *claPathx = "root";
		//mIRCLinker::execex(TEXT("//echo -a clapath:%s"),claPathx);
		parseDialog(depth + 1, claPathx.c_str(), m_Attributes.m_iID);
	}
}

void DcxmlParser::registerId(const TiXmlElement* const idElement, const UINT iNewID)
{
	if (!idElement)
		return;

	if (idElement->QueryIntAttribute("id").first != TiXmlReturns::TIXML_SUCCESS) //<! id attr. is not an int
	{
		if (const TString elementNamedId(idElement->Attribute("id")); !elementNamedId.empty())
		{
			if (mIRCLinker::uEval<UINT>(elementNamedId).has_value()) //<! id attr. doesn't evaluate to an int
				if (const auto d = getDialog(); d)
					d->AddNamedId(elementNamedId, iNewID + mIRC_ID_OFFSET);
		}
	}
}

UINT DcxmlParser::parseId(const TiXmlElement* const idElement)
{
	if (!idElement)
		return 0U;

	//<! if id attribute is already integer return it
	if (const auto [iStatus, local_id] = idElement->QueryIntAttribute("id"); iStatus == TiXmlReturns::TIXML_SUCCESS)
	{
		// found ID as a number,  if its not a negative, return it.
		GSL_SUPPRESS(lifetime) return gsl::narrow_cast<UINT>(std::max(local_id, 0));
	}

	if (const TString attributeIdValue(idElement->Attribute("id")); !attributeIdValue.empty())
	{
		// got ID attrib, evaluate it to try & resolve to a number.

		// if ID is > zero return it.

		if (const auto local_id = mIRCLinker::uEval<UINT>(attributeIdValue); local_id.has_value())
			GSL_SUPPRESS(lifetime) return local_id.value();

		// didn't evaluate to a number, so must be a name...
		if (const auto d = getDialog(); d)
			return d->NameToUserID(attributeIdValue);
	}
	return 0U;
}

