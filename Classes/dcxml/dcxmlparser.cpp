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
#include "dcxmlincludes.h"
#include "dcxmlparser.h"

/*
dcxml [-FLAGS] [DNAME] [DATASET] "[PATH]"
*/
//DcxmlParser::DcxmlParser() :
//	loadSuccess(false),
//	d_Host(nullptr),
//	root(nullptr),
//	m_pDialogs(nullptr), m_pDialog(nullptr),
//	m_pElement(nullptr), m_pParent(nullptr),
//	controls(0),
//	zlayered(0),
//	m_iID(0), m_iParentID(0),
//	m_sElem(nullptr),	m_sParentelem(nullptr), m_sParenttype(nullptr),	m_sType(nullptr),
//	m_sSTclass(nullptr),
//	m_sWeight(nullptr), m_sHeight(nullptr), m_sDropdown(nullptr), m_sWidth(nullptr), m_sMargin(nullptr),
//	m_sStyles(nullptr),
//	m_sCaption(nullptr),
//	m_sTooltip(nullptr),
//	m_sCascade(nullptr),
//	m_sIcon(nullptr),
//	m_sTFlags(nullptr),
//	m_sIntegral(nullptr), m_sState(nullptr), m_sIndent(nullptr),
//	m_sSrc(nullptr),
//	m_sCells(nullptr),
//	m_sRebarMinHeight(nullptr), m_sRebarMinWidth(nullptr),
//	m_sIconsize(nullptr),
//	m_sFontstyle(nullptr), m_sCharset(nullptr), m_sFontsize(nullptr), m_sFontname(nullptr),
//	m_sBorder(nullptr),
//	m_sCursor(nullptr),
//	m_sBgcolour(nullptr), m_sTextbgcolour(nullptr), m_sTextcolour(nullptr),
//	m_sGradientend(nullptr), m_sGradientstart(nullptr),
//	m_sDisabledsrc(nullptr), m_sHoversrc(nullptr), m_sSelectedsrc(nullptr),
//	m_pTemplateRef(nullptr), m_iTemplateRefcCla(0), m_sTemplateRefclaPath(nullptr),
//	m_iEval(0),
//	m_sTemp(nullptr),
//	g_claPath(nullptr), g_claPathx(nullptr), g_bResetCLA(false),
//	m_bVerbose(false), m_bAutoClose(false), m_bZlayered(false), m_pDcxDialog(nullptr), m_pRootElement(nullptr), m_pDialogElement(nullptr), m_pDialogsElement(nullptr)
//{
//}
//DcxmlParser::~DcxmlParser() {
//}

bool DcxmlParser::ParseXML(const TString &tsFilePath,const TString &tsDialogMark,const TString &tsDialogName, const bool verbose, const bool autoClose)
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

		mIRCLinker::execex(TEXT("/dialog -s %s %i %i %i %i"), //!< Sets the dialog size.
			tsDialogMark.to_chr(),
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
		//m_sMargin = (m_sTemp = m_pElement->Attribute("padding")) ? m_sTemp : "0 0 0 0";

		if (tBorder != nullptr) //!< set border ONLY if defined on <dialog>
			xdialogEX(TEXT("-b"), TEXT("+%S"), tBorder);

		mIRCLinker::execex(TEXT("//dialog -t %s %S"), tsDialogMark.to_chr(), tCaption);
		xdialogEX(TEXT("-l"), TEXT("root \t +p%S 0 0 0 0"), tCascade);
		xdialogEX(TEXT("-l"), TEXT("space root \t + %S"), tMargin);

		parseDialog(); //!< Parse <dialog> children onto the dialog

		if (tx->Attribute("center") != nullptr) //!< Centers the dialog
			mIRCLinker::execex(TEXT("/dialog -r %s"), tsDialogMark.to_chr());

		//This "Shite" is to activate the first zlayer, added a check if this command starts returning an error
		if (getZlayered())
			xdialogEX(TEXT("-z"), TEXT("+s 1"));

		mIRCLinker::execex(TEXT("/.timer 1 0 %s %s ready"), getDialog()->getAliasName().to_chr(), tsDialogMark.to_chr()); //!< Tell user that dcxml is finished, & they can do a cla update or whatever.
		return m_bLoadSuccess;
	}
	catch (std::exception)
	{
		m_bLoadSuccess = false;
		if ((getDialog() != nullptr) && isAutoClose())
			mIRCLinker::execex(TEXT("/xdialog -x %s"), tsDialogName.to_chr());
		throw;
	}
}

void DcxmlParser::setDialog(const TString &tsDialogMark) {
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
	if (getDialog() == nullptr)
		throw Dcx::dcxException(TEXT("No such dialog (\"%\") has been marked with dcx."), getDialogMark());
}

void DcxmlParser::loadDialogElement()
{
	setRootElement(getDocument()->FirstChildElement("dcxml"));
	if (getRootElement() == nullptr)
		throw Dcx::dcxException(TEXT("The document element should be <dcxml> in \"%\""), getFilePath());

	setDialogsElement(getRootElement()->FirstChildElement("dialogs"));
	if (getDialogsElement() == nullptr)
		throw Dcx::dcxException(TEXT("Theres no <dialogs> child for <dcxml> in \"%\""), getFilePath());

	/*
	* This finds the Dialog m_pElement. 
	* Important Note:
	* The for loop has an ugly side assigment: it sets this->m_pElement 
	* which is used as a recursion cursor for this->parseDialog()
	*/
	for(m_pElement = getDialogsElement()->FirstChildElement("dialog"); m_pElement != nullptr; m_pElement = m_pElement->NextSiblingElement("dialog"))
	{
		if (getDialogName() == m_pElement->Attribute("name"))
		{
			setDialogElement(m_pElement);
			return;
		}
	}
	throw Dcx::dcxException(TEXT("Theres no <dialog> element with attribute name=\"%\" in \"%\""), getDialogName(), getFilePath());
}

void DcxmlParser::parseAttributes() {
	parseAttributes(m_pElement);
}

void DcxmlParser::parseAttributes(const TiXmlElement *const tElement) {
	__assume(tElement != nullptr);

	m_sElem = tElement->Value();
	m_sParentelem = m_pParent->Value();
	m_sParenttype = queryAttribute(m_pParent, "type", "panel");
	m_sType = queryAttribute(m_pElement,"type","panel");
	m_sSTclass = queryAttribute(m_pElement,"class","");
	m_sWeight = queryAttribute(tElement, "weight", "1");
	m_sHeight = queryAttribute(tElement, "height", "0");
	m_sDropdown = nullptr;

#if DCX_USE_HASHING
	const auto sElemHash = dcx_hash(m_sElem);
	if ((sElemHash == "comboex"_hash) || (sElemHash == "colorcombo"_hash))
		m_sDropdown = queryAttribute(tElement, "dropdown", "100");
#else
	if ((0 == ts_strcmp(m_sElem, "comboex")) || (0 == ts_strcmp(m_sElem, "colorcombo")))
		m_sDropdown = queryAttribute(tElement, "dropdown", "100");
#endif

	m_sWidth = queryAttribute(tElement, "width", "0");
	m_sMargin = queryAttribute(tElement, "margin", "0 0 0 0");
	m_sStyles = queryAttribute(tElement, "styles", "");

	m_sCaption = tElement->Attribute("caption");
	if (m_sCaption == nullptr)
		m_sCaption = tElement->GetText();

	m_sTooltip = queryAttribute(tElement, "tooltip", "");
	m_sCascade = queryAttribute(tElement, "cascade", "");
	m_sIcon = queryAttribute(tElement, "icon", "0");
	m_sIntegral = queryAttribute(tElement, "integral", "0");
	m_sState = queryAttribute(tElement, "state", "0");
	m_sIndent = queryAttribute(tElement, "indent", "0");
	//flags attribute defaults different per type/item
	m_sTFlags = tElement->Attribute("flags");
	m_sSrc = queryAttribute(tElement, "src", "");
	m_sCells = queryAttribute(tElement, "cells", "-1");
	m_sRebarMinHeight = queryAttribute(tElement, "minheight", "0");
	m_sRebarMinWidth = queryAttribute(tElement, "minwidth", "0");
	m_sIconsize = queryAttribute(tElement, "iconsize", "16");
	m_iEval = queryIntAttribute(tElement, "eval");

	m_sFontstyle = queryAttribute(tElement, "fontstyle", "d");
	m_sCharset = queryAttribute(tElement, "charset", "ansi");
	m_sFontsize = queryAttribute(tElement, "fontsize", "");
	m_sFontname = queryAttribute(tElement, "fontname", "");
	m_sBorder = queryAttribute(tElement, "border", "");
	m_sCursor = queryAttribute(tElement, "cursor", "arrow");
	m_sBgcolour = queryAttribute(tElement, "bgcolour", "0");
	m_sTextbgcolour = queryAttribute(tElement, "textbgcolour", "");
	m_sTextcolour = queryAttribute(tElement, "textcolour", "0");

	m_sGradientstart = queryAttribute(tElement, "gradientstart", "");
	m_sGradientend = queryAttribute(tElement, "gradientend", "");

	m_sDisabledsrc = queryAttribute(tElement, "disabledsrc", "");
	m_sHoversrc = queryAttribute(tElement, "hoversrc", "");
	m_sSelectedsrc = queryAttribute(tElement, "selectedsrc", "");
}

/* parseControl() : if current m_pElement is a control perform some extra commands*/
void DcxmlParser::parseControl() { 
#if DCX_USE_HASHING
	if (m_pElement->Attribute("zlayer") != nullptr) {
		xdialogEX(TEXT("-z"), TEXT("+a %i"), m_iID);
		setZlayered(true);
	}

	//const auto nType = dcx_hash(m_sParenttype);
	const auto nType = dcx_hash(m_sType);
	switch (nType)
	{
	case "divider"_hash:		//	divider
		//xdidEX(m_iParentID, TEXT("-v"), TEXT("%d"), queryIntAttribute(m_pElement, "width", 0));
		xdidEX(m_iID, TEXT("-v"), TEXT("%d"), queryIntAttribute(m_pElement, "width", 0));
		break;
	case "button"_hash:		//	button
		xdidEX(m_iID, TEXT("-l"), TEXT("%S"), m_sIconsize);
	case "radio"_hash:		//	radio
	case "link"_hash:		//	link
	case "check"_hash:	//	check
	case "box"_hash:	//	box
		if (m_sCaption != nullptr)
			xdidEX(m_iID, TEXT("-t"), TEXT("%S"), m_sCaption);
		break;
	case "toolbar"_hash:		//	toolbar
	case "treeview"_hash:		//	treeview
		xdidEX(m_iID, TEXT("-l"), TEXT("%S"), m_sIconsize);
	case "comboex"_hash:		//	comboex
	case "list"_hash:		//	list
	case "listview"_hash:		//	listview
		parseItems(m_pElement);
		break;
	case "ipaddress"_hash:	//	ipaddress
		if (m_sCaption != nullptr)
			xdidEX(m_iID, TEXT("-a"), TEXT("%S"), m_sCaption);
		break;
	case "webctrl"_hash:	//	webctrl
		if (m_sSrc != nullptr)
			xdidEX(m_iID, TEXT("-n"), TEXT("%S"), m_sSrc);
		break;
	case "text"_hash:	//	text
		if (m_sCaption != nullptr) {
			TString mystring(m_sCaption);

			if (mystring.left(2) == TEXT("\r\n"))
				mystring = mystring.right(-2);
			else if (mystring[0] == TEXT('\n'))
				mystring = mystring.right(-1);

			mystring -= TEXT('\t'); // remove all '\t' from text.

			int textspace = 0;

			TString printstring;
			for (const auto &tsTmp : mystring)
			{
				printstring.addtok(tsTmp);
				if (printstring.len() > (MIRC_BUFFER_SIZE_CCH - 100)) {
					xdidEX(m_iID, TEXT("-a"), TEXT("%i %s"), textspace, printstring.to_chr());
					printstring.clear();	// = TEXT("");
					textspace = 1;
				}
			}
			if (!printstring.empty())
				xdidEX(m_iID, TEXT("-a"), TEXT("%i %s"), textspace, printstring.to_chr());
		}
		break;
	case "edit"_hash:	//	edit
	case "richedit"_hash:	//	richedit
		if (m_sCaption != nullptr) {
			TString mystring(m_sCaption);

			if (mystring.left(2) == TEXT("\r\n"))
				mystring = mystring.right(-2);
			else if (mystring[0] == TEXT('\n'))
				mystring = mystring.right(-1);

			mystring -= TEXT('\t'); // remove all tabs from text.  Ook: can crash here for some reason, needs looked at
			//if (nType == "richedit"_hash) {	// richedit
			//	mystring -= TEXT("\\c");
			//	mystring -= TEXT("\\b");
			//	mystring -= TEXT("\\r");
			//}
			if (nType == "richedit"_hash) {	// richedit, converts \c -> ctrl-k, \b -> ctrl-b, \r -> ctrl-r
				mystring.replace(TEXT("\\c"), TEXT('\3'));
				mystring.replace(TEXT("\\b"), TEXT('\2'));
				mystring.replace(TEXT("\\r"), TEXT('\r'));
			}
			UINT line = 0U;
			for (auto itStart = mystring.begin(TEXT("\r\n")), itEnd = mystring.end(); itStart != itEnd; ++itStart)
			{
				++line;
				xdidEX(m_iID, TEXT("-i"), TEXT("%u %s"), line, (*itStart).to_chr());
			}
		}
		break;
	case "pbar"_hash:	//	pbar
		if (m_sCaption != nullptr)
			xdidEX(m_iID, TEXT("-i"), TEXT("%S"), m_sCaption);
		break;
	case "statusbar"_hash:	//	statusbar
		xdidEX(m_iID, TEXT("-l"), TEXT("%S"), m_sCells);
		parseItems(m_pElement);
		break;
	case "image"_hash:	//	image
		if (m_sSrc != nullptr)
			xdidEX(m_iID, TEXT("-i"), TEXT("+%S %S"), ((m_sTFlags != nullptr) ? m_sTFlags : ""), m_sSrc);
	default:	//	unknown?!?!?!
		break;
	}

	m_sDisabledsrc = queryAttribute(m_pElement, "disabledsrc", "");
	m_sHoversrc = queryAttribute(m_pElement, "hoversrc", "");
#else
	if (m_pElement->Attribute("zlayer") != nullptr) {
		xdialogEX(TEXT("-z"),TEXT("+a %u"),m_iID);
		setZlayered(true);
	}
	//        padding = (m_sTemp = m_pElement->Attribute("padding")) ? m_sTemp : "0 0 0 0";
	//const TString tsParent(m_sParenttype);
	const TString tsParent(m_sType);
	static const TString poslist(TEXT("divider toolbar button treeview comboex list listview radio link check box ipaddress webctrl text edit richedit pbar statusbar image"));
	const auto nType = poslist.findtok(tsParent, 1);

	switch (nType)
	{
	case 1:		//	divider
		xdidEX(m_iID, TEXT("-v"), TEXT("%d"), queryIntAttribute(m_pElement, "width", 0));
		break;
	case 3:		//	button
		xdidEX(m_iID, TEXT("-l"), TEXT("%S"), m_sIconsize);
	case 8:		//	radio
	case 9:		//	link
	case 10:	//	check
	case 11:	//	box
		if (m_sCaption != nullptr)
			xdidEX(m_iID, TEXT("-t"), TEXT("%S"), m_sCaption);
		break;
	case 2:		//	toolbar
	case 4:		//	treeview
		xdidEX(m_iID, TEXT("-l"), TEXT("%S"), m_sIconsize);
	case 5:		//	comboex
	case 6:		//	list
	case 7:		//	listview
		parseItems(m_pElement);
		break;
	case 12:	//	ipaddress
		if (m_sCaption != nullptr)
			xdidEX(m_iID, TEXT("-a"), TEXT("%S"), m_sCaption);
		break;
	case 13:	//	webctrl
		xdidEX(m_iID, TEXT("-n"), TEXT("%S"), m_sSrc);
		break;
	case 14:	//	text
		if (m_sCaption != nullptr) {
			TString mystring(m_sCaption);

			if (mystring.left(2) == TEXT("\r\n"))
				mystring = mystring.right(-2);
			else if (mystring[0] == TEXT('\n'))
				mystring = mystring.right(-1);

			mystring -= TEXT("\t"); // remove all '\t' from text.

			int textspace = 0;

			TString printstring;
			for (const auto &tsTmp: mystring)
			{
				printstring.addtok(tsTmp);
				if (printstring.len() > (MIRC_BUFFER_SIZE_CCH - 100)) {
					xdidEX(m_iID, TEXT("-a"), TEXT("%i %s"), textspace, printstring.to_chr());
					printstring.clear();	// = TEXT("");
					textspace = 1;
				}
			}
			if (!printstring.empty())
				xdidEX(m_iID, TEXT("-a"), TEXT("%i %s"), textspace, printstring.to_chr());
		}
		break;
	case 15:	//	edit
	case 16:	//	richedit
		if (m_sCaption != nullptr) {
			TString mystring(m_sCaption);

			if (mystring.left(2) == TEXT("\r\n"))
				mystring = mystring.right(-2);
			else if (mystring[0] == TEXT('\n'))
				mystring = mystring.right(-1);

			mystring -= TEXT("\t"); // remove all tabs from text.
			if (nType == 16) {	// richedit
				mystring -= TEXT("\\c");
				mystring -= TEXT("\\b");
				mystring -= TEXT("\\r");
			}
			UINT line = 0;
			for (auto itStart = mystring.begin(TEXT("\r\n")), itEnd = mystring.end(); itStart != itEnd; ++itStart)
			{
				line++;
				xdidEX(m_iID, TEXT("-i"), TEXT("%u %s"), line, (*itStart).to_chr());
			}
		}
		break;
	case 17:	//	pbar
		if (m_sCaption != nullptr)
			xdidEX(m_iID, TEXT("-i"), TEXT("%S"), m_sCaption);
		break;
	case 18:	//	statusbar
		xdidEX(m_iID, TEXT("-l"), TEXT("%S"), m_sCells);
		parseItems(m_pElement);
		break;
	case 19:	//	image
		if (m_sSrc != nullptr)
			xdidEX(m_iID, TEXT("-i"), TEXT("+%S %S"), ((m_sTFlags != nullptr) ? m_sTFlags : ""), m_sSrc);
	default:	//	unknown?!?!?!
		break;
	}

	m_sDisabledsrc = queryAttribute(m_pElement, "disabledsrc", "");
	m_sHoversrc = queryAttribute(m_pElement, "hoversrc", "");
#endif
}

/* xdialogEX(switch,format[,args[]]) : performs an xdialog command internally or through mIRC */
void DcxmlParser::xdialogEX(const TCHAR *const sw,const TCHAR *const dFormat, ...) { 
	TString txt;

	{
		va_list args;

		va_start(args, dFormat);
		txt.tvprintf(dFormat, args);
		va_end(args);
	}

	xml_xdialog(sw, txt.to_chr());
}

void DcxmlParser::xml_xdialog(const TCHAR * const sSwitch, const TCHAR * const sArgs)
{
#if DCX_DEBUG_OUTPUT
	if (isVerbose())
		mIRCLinker::execex(TEXT("/echo -a dcxml debug: /xdialog %s %s %s"), sSwitch, getDialogMark().to_chr(), sArgs);
#endif

	if (m_iEval > 0) mIRCLinker::execex(TEXT("//xdialog %s %s %s"), sSwitch, getDialogMark().to_chr(), sArgs);
	else getDialog()->parseCommandRequestEX(TEXT("%s %s %s"), getDialogMark().to_chr(), sSwitch, sArgs);
}

/* xdidEX(controlId,switch,args) : performs an xdid command internally or through mIRC on the specified ID */
void DcxmlParser::xml_xdid(const UINT cid, const TCHAR *const sSwitch, const TCHAR *const sArgs)
{
#if DCX_DEBUG_OUTPUT
	if (isVerbose())
		mIRCLinker::execex(TEXT("/echo -a dcxml debug: /xdid %s %s %u %s"), sSwitch, getDialogMark().to_chr(), cid, sArgs);
#endif

	if (m_iEval > 0) mIRCLinker::execex(TEXT("//xdid %s %s %u %s"), sSwitch, getDialogMark().to_chr(), cid, sArgs);
	else getDialog()->parseComControlRequestEX(cid, TEXT("%s %u %s %s"), getDialogMark().to_chr(), cid, sSwitch, sArgs);
}

/* xdidEX(controlId,switch,format[,args[]]) : performs an xdid command internally or through mIRC on the specified ID */
void DcxmlParser::xdidEX(const UINT cid, const TCHAR *const sw, const TCHAR *const dFormat, ...) {
	TString txt;

	{
		va_list args;

		va_start(args, dFormat);
		txt.tvprintf(dFormat, args);
		va_end(args);
	}

	xml_xdid(cid, sw, txt.to_chr());
}

/* parseCLA(int numberOfClaControlsInCurrentBranch) : parses control and pane elements and applies the right CLA commands */
TString DcxmlParser::parseCLA(const int cCla)
{
#if DCX_USE_HASHING
	const auto sParentelemHash = dcx_hash(m_sParentelem);
	const auto sElemHash = dcx_hash(m_sElem);

	if (sElemHash == "control"_hash) {
		const auto sTypeHash = dcx_hash(m_sType);
		if ((sTypeHash == "panel"_hash) || (sTypeHash == "box"_hash)) {
			xdidEX(m_iID, TEXT("-l"), TEXT("root \t +p%S 0 0 0 0"), m_sCascade);
			xdidEX(m_iID, TEXT("-l"), TEXT("space root \t + %S"), m_sMargin);
			g_bResetCLA = true;
		}
		const char * fHeigth = "";
		const char * fWidth = "";
		const char * fixed = "l";
		if (m_pElement->Attribute("height") != nullptr) { fHeigth = "v"; fixed = "f"; m_sWeight = "0"; }
		if (m_pElement->Attribute("width") != nullptr) { fWidth = "h"; fixed = "f"; m_sWeight = "0"; }

		if (sParentelemHash == "dialog"_hash)
			xdialogEX(TEXT("-l"), TEXT("cell %S \t +%S%S%Si %u %S %S %S"), g_claPath, fixed, fHeigth, fWidth, m_iID, m_sWeight, m_sWidth, m_sHeight);
		else if (sParentelemHash == "control"_hash) {
			const char *const t_type = m_pParent->Attribute("type");
			if ((t_type != nullptr) && (m_iParentID > 0)) {
				const auto t_typeHash = dcx_hash(t_type);
				if ((t_typeHash == "panel"_hash) || (t_typeHash == "box"_hash))
					xdidEX(m_iParentID, TEXT("-l"), TEXT("cell %S \t +%S%S%Si %u %S %S %S"), g_claPath, fixed, fHeigth, fWidth, m_iID, m_sWeight, m_sWidth, m_sHeight);
			}
		}
	}
	else if (sElemHash == "pane"_hash) {
		if (sParentelemHash == "dialog"_hash)
			xdialogEX(TEXT("-l"), TEXT("cell %S \t +p%S 0 %S 0 0"), g_claPath, m_sCascade, m_sWeight);
		else if (sParentelemHash == "control"_hash) {
			if ((m_sParenttype != nullptr) && (m_iParentID > 0)) {

				const auto sParenttypeHash = dcx_hash(m_sParenttype);

				if ((sParenttypeHash == "panel"_hash) || (sParenttypeHash == "box"_hash))
					xdidEX(m_iParentID, TEXT("-l"), TEXT("cell %S \t +p%S 0 %S 0 0"), g_claPath, m_sCascade, m_sWeight);
			}
		}
	}
	TString claPathx;

	if (g_bResetCLA)
		claPathx = TEXT("root");
	else
	{
		if (0 != ts_strcmp(g_claPath, "root"))
			claPathx = g_claPath;

		claPathx.addtok(cCla);
	}

	if (m_pElement->Attribute("margin") != nullptr) {
		if (sParentelemHash == "dialog"_hash)
			xdialogEX(TEXT("-l"), TEXT("space %s \t + %S"), claPathx.to_chr(), m_sMargin);
		else
			xdidEX(m_iParentID, TEXT("-l"), TEXT("space %S \t + %S"), g_claPath, m_sMargin);
	}
	g_bResetCLA = false;
	return claPathx;
#else
	if (0 == ts_strcmp(m_sElem, "control")) {
		if ((0 == ts_strcmp(m_sType, "panel")) || (0 == ts_strcmp(m_sType, "box"))) {
			xdidEX(m_iID,TEXT("-l"),TEXT("root \t +p%S 0 0 0 0"),m_sCascade);
			xdidEX(m_iID,TEXT("-l"),TEXT("space root \t + %S"),m_sMargin);
			g_bResetCLA = true;
		}
		const char * fHeigth = "";
		const char * fWidth = "";
		const char * fixed = "l";
		if (m_pElement->Attribute("height") != nullptr) { fHeigth = "v"; fixed = "f"; m_sWeight = "0"; }
		if (m_pElement->Attribute("width") != nullptr) { fWidth = "h"; fixed = "f"; m_sWeight = "0"; }
		if (0 == ts_strcmp(m_sParentelem, "dialog"))
			xdialogEX(TEXT("-l"),TEXT("cell %S \t +%S%S%Si %u %S %S %S"),	g_claPath,fixed,fHeigth,fWidth,m_iID,m_sWeight,m_sWidth,m_sHeight);
		else if (0 == ts_strcmp(m_sParentelem, "control")) {
			const auto t_type = m_pParent->Attribute("type");
			if ((t_type != nullptr) && (m_iParentID > 0)) {
				if (0 == ts_strcmp(t_type, "panel"))
					xdidEX(m_iParentID,TEXT("-l"),TEXT("cell %S \t +%S%S%Si %u %S %S %S"), g_claPath,fixed,fHeigth,fWidth,m_iID,m_sWeight,m_sWidth,m_sHeight); 
				else if (0 == ts_strcmp(t_type, "box"))
					xdidEX(m_iParentID,TEXT("-l"),TEXT("cell %S \t +%S%S%Si %u %S %S %S"), g_claPath,fixed,fHeigth,fWidth,m_iID,m_sWeight,m_sWidth,m_sHeight); 
			}
		}
	}
	else if (0 == ts_strcmp(m_sElem, "pane")) {
		if (0 == ts_strcmp(m_sParentelem, "dialog"))
			xdialogEX(TEXT("-l"),TEXT("cell %S \t +p%S 0 %S 0 0"),g_claPath,m_sCascade,m_sWeight);
		else if (0 == ts_strcmp(m_sParentelem, "control")) {
			if ((m_sParenttype != nullptr) && (m_iParentID > 0)) {
				if (0 == ts_strcmp(m_sParenttype, "panel"))
					xdidEX(m_iParentID,TEXT("-l"),TEXT("cell %S \t +p%S 0 %S 0 0"),g_claPath,m_sCascade,m_sWeight);
				else if (0 == ts_strcmp(m_sParenttype, "box"))
					xdidEX(m_iParentID,TEXT("-l"),TEXT("cell %S \t +p%S 0 %S 0 0"),g_claPath,m_sCascade,m_sWeight);
			}
		}
	}
	TString claPathx;
	//if (g_bResetCLA)
	//	claPathx = TEXT("root");
	//else if (0==lstrcmpA(g_claPath, "root"))
	//	claPathx.tsprintf(TEXT("%i"),cCla);
	//else
	//	claPathx.tsprintf(TEXT("%S %i"),g_claPath,cCla);

	if (g_bResetCLA)
		claPathx = TEXT("root");
	else
	{
		if (0 != ts_strcmp(g_claPath, "root"))
			claPathx = g_claPath;

		claPathx.addtok(cCla);
	}

	if (m_pElement->Attribute("margin") != nullptr) {
		if (0 == ts_strcmp(m_sParentelem, "dialog"))
			xdialogEX(TEXT("-l"),TEXT("space %s \t + %S"),claPathx.to_chr(),m_sMargin);
		else
			xdidEX(m_iParentID,TEXT("-l"),TEXT("space %S \t + %S"),g_claPath,m_sMargin);
	}
	g_bResetCLA = false;
	return claPathx;
#endif
}

/* setStyle(TiXmlElement*) : Applies the styles described on the m_pElement found by parseStyle() */
void DcxmlParser::setStyle(const TiXmlElement *const style) {
#if DCX_USE_HASHING
	//style attributes evaluate by default unless eval="0" is set on the m_pElement explicitly

	m_iEval = queryIntAttribute(style, "eval", 1);

	//font
	m_sFontstyle = queryAttribute(style, "fontstyle", "d");
	m_sCharset = queryAttribute(style, "charset", "ansi");
	m_sFontsize = queryAttribute(style, "fontsize", "");
	m_sFontname = queryAttribute(style, "fontname", "");
	if ((style->Attribute("fontsize") != nullptr) || (style->Attribute("fontname") != nullptr))
		xdidEX(m_iID, TEXT("-f"), TEXT("+%S %S %S %S"), m_sFontstyle, m_sCharset, m_sFontsize, m_sFontname);
	//border
	m_sBorder = queryAttribute(style, "border", "");
	if (!_ts_isEmpty(m_sBorder))
		xdidEX(m_iID, TEXT("-x"), TEXT("+%S"), m_sBorder);
	//colours
	//m_sCursor = queryAttribute(style, "cursor", "arrow");
	m_sCursor = style->Attribute("cursor");	// can be null
	m_sBgcolour = queryAttribute(style, "bgcolour", "");
	m_sTextbgcolour = queryAttribute(style, "textbgcolour", "");
	m_sTextcolour = queryAttribute(style, "textcolour", "");

	const auto sTypeHash = std::hash<const char *>{}(m_sType);

	if (!_ts_isEmpty(m_sBgcolour)) {
		xdidEX(m_iID, TEXT("-C"), TEXT("+b %S"), m_sBgcolour);
		if (sTypeHash == "pbar"_hash)
		{
			xdidEX(m_iID, TEXT("-k"), TEXT("%S"), m_sBgcolour);
			xml_xdid(m_iID, TEXT("-U"), TEXT(""));
		}
	}

	if (!_ts_isEmpty(m_sTextbgcolour))
	{
		xdidEX(m_iID, TEXT("-C"), TEXT("+k %S"), m_sTextbgcolour);
		if (sTypeHash == "pbar"_hash)
		{
			xdidEX(m_iID, TEXT("-c"), TEXT("%S"), m_sTextbgcolour);
			xml_xdid(m_iID, TEXT("-U"), TEXT(""));
		}
	}
	else if (!_ts_isEmpty(m_sBgcolour))
		xdidEX(m_iID, TEXT("-C"), TEXT("+k %S"), m_sBgcolour);

	if (!_ts_isEmpty(m_sTextcolour))
	{
		xdidEX(m_iID, TEXT("-C"), TEXT("+t %S"), m_sTextcolour);
		if (sTypeHash == "pbar"_hash)
		{
			xdidEX(m_iID, TEXT("-q"), TEXT("%S"), m_sTextcolour);
			xml_xdid(m_iID, TEXT("-U"), TEXT(""));
		}
	}

	if (!_ts_isEmpty(m_sGradientstart))
		xdidEX(m_iID, TEXT("-C"), TEXT("+g %S"), m_sGradientstart);
	if (!_ts_isEmpty(m_sGradientstart))
		xdidEX(m_iID, TEXT("-C"), TEXT("+G %S"), m_sGradientend);

	//cursor
	if (!_ts_isEmpty(m_sCursor))
		xdidEX(m_iID, TEXT("-J"), TEXT("+r %S"), m_sCursor);	// Ook: assumes cursor is a resource, needs changed.

	//iconsize
	if (style->Attribute("iconsize") != nullptr)
	{
		if (((sTypeHash == "toolbar"_hash) || (sTypeHash == "button"_hash)) || (sTypeHash == "treeview"_hash))
			xdidEX(m_iID, TEXT("-l"), TEXT("%S"), m_sIconsize);
	}
	if (sTypeHash == "button"_hash)
	{
		if (m_pElement->Attribute("bgcolour") == nullptr)
			m_sBgcolour = "65280";
		if (m_pElement->Attribute("src") != nullptr)
			xdidEX(m_iID, TEXT("-k"), TEXT("+n %S %S"), m_sBgcolour, m_sSrc);
		if (m_pElement->Attribute("disabledsrc") != nullptr)
			xdidEX(m_iID, TEXT("-k"), TEXT("+d %S %S"), m_sBgcolour, m_sDisabledsrc);
		if (m_pElement->Attribute("hoversrc") != nullptr)
			xdidEX(m_iID, TEXT("-k"), TEXT("+h %S %S"), m_sBgcolour, m_sHoversrc);
		if (m_pElement->Attribute("selectedsrc") != nullptr)
			xdidEX(m_iID, TEXT("-k"), TEXT("+s %S %S"), m_sBgcolour, m_sSelectedsrc);
	}
#else
	//style attributes evaluate by default unless eval="0" is set on the m_pElement explicitly

	m_iEval = queryIntAttribute(style,"eval",1);

	//font
	m_sFontstyle = queryAttribute(style,"fontstyle","d");
	m_sCharset = queryAttribute(style,"charset","ansi");
	m_sFontsize = queryAttribute(style,"fontsize","");
	m_sFontname = queryAttribute(style,"fontname","");
	if ((style->Attribute("fontsize") != nullptr) || (style->Attribute("fontname") != nullptr))
		xdidEX(m_iID,TEXT("-f"),TEXT("+%S %S %S %S"), m_sFontstyle, m_sCharset, m_sFontsize, m_sFontname);
	//border
	m_sBorder = queryAttribute(style,"border","");
	if (!_ts_isEmpty(m_sBorder))
		xdidEX(m_iID,TEXT("-x"),TEXT("+%S"),m_sBorder);
	//colours
	m_sCursor = queryAttribute(style,"cursor","arrow");
	m_sBgcolour = queryAttribute(style,"bgcolour","");
	m_sTextbgcolour = queryAttribute(style,"textbgcolour","");
	m_sTextcolour = queryAttribute(style,"textcolour","");

	const refString<const char, -1> refsType(m_sType);

	if (style->Attribute("bgcolour") != nullptr) {
		xdidEX(m_iID,TEXT("-C"),TEXT("+b %S"),m_sBgcolour);
		if (refsType == "pbar")
		{
			xdidEX(m_iID,TEXT("-k"),TEXT("%S"),m_sBgcolour);
			xml_xdid(m_iID,TEXT("-U"),TEXT(""));
		}
	}
	if (style->Attribute("textbgcolour") != nullptr)
	{
		xdidEX(m_iID,TEXT("-C"),TEXT("+k %S"),m_sTextbgcolour);
		if (refsType == "pbar")
		{
			xdidEX(m_iID,TEXT("-c"),TEXT("%S"),m_sTextbgcolour);
			xml_xdid(m_iID,TEXT("-U"),TEXT(""));
		}
	}
	else if (style->Attribute("bgcolour") != nullptr)
		xdidEX(m_iID,TEXT("-C"),TEXT("+k %S"),m_sBgcolour); 
	if (style->Attribute("textcolour") != nullptr)
	{
		xdidEX(m_iID,TEXT("-C"),TEXT("+t %S"),m_sTextcolour);
		if (refsType == "pbar")
		{
			xdidEX(m_iID,TEXT("-q"),TEXT("%S"),m_sTextcolour);
			xml_xdid(m_iID,TEXT("-U"),TEXT(""));
		}
	}

	if (style->Attribute("gradientstart") != nullptr)
		xdidEX(m_iID,TEXT("-C"),TEXT("+g %S"),m_sGradientstart);
	if (style->Attribute("gradientend") != nullptr)
		xdidEX(m_iID,TEXT("-C"),TEXT("+G %S"),m_sGradientend);

	//cursor
	if (style->Attribute("cursor") != nullptr)
		xdidEX(m_iID,TEXT("-J"),TEXT("+r %S"),m_sCursor);

	//iconsize
	if (style->Attribute("iconsize") != nullptr)
	{
		if (((refsType == "toolbar") || (refsType == "button")) || (refsType == "treeview"))
			xdidEX(m_iID,TEXT("-l"),TEXT("%S"),m_sIconsize);
	}
	if (refsType == "button")
	{
		if (m_pElement->Attribute("bgcolour") == nullptr)
			m_sBgcolour = "65280";
		if (m_pElement->Attribute("src") != nullptr)
			xdidEX(m_iID,TEXT("-k"),TEXT("+n %S %S"),m_sBgcolour,m_sSrc);
		if (m_pElement->Attribute("disabledsrc") != nullptr)
			xdidEX(m_iID,TEXT("-k"),TEXT("+d %S %S"),m_sBgcolour,m_sDisabledsrc);
		if (m_pElement->Attribute("hoversrc") != nullptr)
			xdidEX(m_iID,TEXT("-k"),TEXT("+h %S %S"),m_sBgcolour,m_sHoversrc);
		if (m_pElement->Attribute("selectedsrc") != nullptr)
			xdidEX(m_iID,TEXT("-k"),TEXT("+s %S %S"),m_sBgcolour,m_sSelectedsrc);
	}
#endif
}

/* parseStyle(recursionDepth) : Simple recursive method to cascade find the right style to apply to an m_pElement */
void DcxmlParser::parseStyle(int depth) { 
	if (depth > 2)
		return;

	++depth;

	const TiXmlElement* tiStyles = nullptr;
	const TiXmlElement* style = nullptr;
	if (depth == 3)
		style = m_pElement;
	else if (depth == 1)
		tiStyles = getDialogsElement()->FirstChildElement("styles");	// find styles for ALL dialogs
	else if (depth == 2)
		tiStyles = getDialogElement()->FirstChildElement("styles");		// find styles for THIS dialog

	if (tiStyles != nullptr) {
		style = tiStyles->FirstChildElement("all");		// find styles for ALL controls
		if (style != nullptr)
			setStyle(style);

		const TiXmlElement* ClassElement = nullptr;
		const TiXmlElement* TypeElement = nullptr;
		const TiXmlElement* IdElement = nullptr;

		for( style = tiStyles->FirstChildElement("style"); style != nullptr; style = style->NextSiblingElement()) {
			auto ctmp = style->Attribute("class");
			if (ctmp != nullptr)
			{
				if (0 == ts_strcmp(ctmp, m_sSTclass))
					ClassElement = style;
			}
			ctmp = style->Attribute("type");
			if (ctmp != nullptr)
			{
				if (0 == ts_strcmp(ctmp, m_sType))
					TypeElement = style;
			}
			if (parseId(style) == m_iID)
				IdElement = style;
		}
		if (IdElement != nullptr)
			style = IdElement;
		else if (ClassElement != nullptr)
			style = ClassElement;
		else if (TypeElement != nullptr)
			style = TypeElement;
	}
	if (style != nullptr)
		setStyle(style);
	parseStyle(depth);
}

/* parseIcons(recursionDepth) : Simple recursive method to cascade find the right icons to apply to an m_pElement */
void DcxmlParser::parseIcons(int depth) { 
	if (depth > 1)
		return;

	++depth;

	const TiXmlElement* icons = nullptr;
	if (depth == 1)
		icons = getDialogElement()->FirstChildElement("icons");
	else if (depth == 2)
		icons = getDialogsElement()->FirstChildElement("icons");

	if (icons != nullptr) {
		const TiXmlElement* tiIcon = nullptr;
		const TiXmlElement* IdElement = nullptr;
		const TiXmlElement* TypeElement = nullptr;
		const TiXmlElement* ClassElement = nullptr;

		for( tiIcon = icons->FirstChildElement("icon"); tiIcon != nullptr; tiIcon = tiIcon->NextSiblingElement()) {
			auto ctmp = tiIcon->Attribute("class");
			if (ctmp != nullptr)
			{
				if (0 == ts_strcmp(ctmp, m_sSTclass))
					ClassElement = tiIcon;
			}
			ctmp = tiIcon->Attribute("type");
			if (ctmp != nullptr)
			{
				if (0 == ts_strcmp(ctmp, m_sType))
					TypeElement = tiIcon;
			}
			const auto t_id = parseId(tiIcon);
			if (t_id == m_iID)
				IdElement = tiIcon;
		}

		if (IdElement != nullptr)
			tiIcon = IdElement;
		else if (ClassElement != nullptr)
			tiIcon = ClassElement;
		else if (TypeElement != nullptr)
			tiIcon = TypeElement;
		else
			tiIcon = nullptr;

		if (tiIcon != nullptr) {
			const auto flags = queryAttribute(tiIcon, "flags", "n");
			const auto index = queryAttribute(tiIcon, "index", "0");
			const auto tIconSrc = tiIcon->Attribute("src");
			const auto indexmin = queryIntAttribute(tiIcon, "indexmin", 0);
			const auto indexmax = queryIntAttribute(tiIcon, "indexmax", -1);
			if (tIconSrc != nullptr) {
				// Ook: change this to call /xdid -w with an index range
				if (indexmin <= indexmax) 
					//method sucks but looping in C++ is WAYYY too fast for mIRC
				{
					mIRCLinker::execex(TEXT("//var %%x = %i | while (%%x <= %i ) { xdid -w %s %u +%S %%x %S | inc %%x }"), indexmin,indexmax,getDialogMark().to_chr(),m_iID,flags,tIconSrc);
					//mIRCLinker::execex(TEXT("//xdid -w %s %u +%S %i-%i %S"), getDialogMark().to_chr(), m_iID, flags, indexmin, indexmax, tIconSrc);
				}
				else 
					mIRCLinker::execex(TEXT("//xdid -w %s %u +%S %S %S"),getDialogMark().to_chr(),m_iID,flags,index,tIconSrc);
			}
			else
			{
				for (auto iconchild = tiIcon->FirstChildElement("icon"); iconchild != nullptr; iconchild = iconchild->NextSiblingElement()) {
					const auto tflags = queryAttribute(iconchild, "flags", "n");
					const auto tindex = queryAttribute(iconchild, "index", "0");
					const auto tsrc = iconchild->Attribute("src");
					if (tsrc != nullptr)
						mIRCLinker::execex(TEXT("//xdid -w %s %u +%S %S %S"),getDialogMark().to_chr(),m_iID,tflags,tindex,tsrc);
				}
			}
		}
	}
	else
		parseIcons(depth);
}

/* parseItems(XmlElement,recursionDepth,itemPath) : recursively applies items for a control */
void DcxmlParser::parseItems(const TiXmlElement *const tiElement,const UINT depth, const char *const itemPath) {
#if DCX_USE_HASHING
	auto item = 0, cell = 0;
	const auto sTypeHash = dcx_hash(m_sType);

	for (auto child = tiElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
	{
		const auto childelem = child->Value();
		const auto childelemHash = dcx_hash(childelem);

		if (childelemHash == "columns"_hash)
		{
			if (sTypeHash == "listview"_hash)
			{
				TString tsArguments;
				TString tsBuffer;
				for (auto column = child->FirstChildElement("column"); column != nullptr; column = column->NextSiblingElement("column"))
				{
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

					//tsArguments.addtok(tsBuffer,TEXT('\t'));
				}
				if (!tsArguments.empty())
					xdidEX(m_iID, TEXT("-t"), TEXT("%s"), tsArguments.to_chr());
			}
			//if (childelemHash == "dataset"_hash)
			//{
			//	//auto listView = this->d_Host->getControlByID(m_iID);
			//}
		}


		if ((childelemHash == "item"_hash) || (childelemHash == "control"_hash))
			++cell;
		//fill all required parameters with attributes or default values
		parseAttributes(child);
		if (childelemHash == "item"_hash) {
			++item;
			if (sTypeHash == "toolbar"_hash)
				xdidEX(m_iID, TEXT("-a"), TEXT("0 +%S %S %S %S %S \t %S"), ((m_sTFlags != nullptr) ? m_sTFlags : "a"), m_sWidth, m_sIcon, m_sTextcolour, m_sCaption, m_sTooltip);
			else if (sTypeHash == "comboex"_hash)
				xdidEX(m_iID, TEXT("-a"), TEXT("0 %S %S %S 0 %S"), m_sIndent, m_sIcon, m_sIcon, m_sCaption);
			else if (sTypeHash == "list"_hash)
			{
				if (m_sCaption != nullptr)
					xdidEX(m_iID, TEXT("-a"), TEXT("0 %S"), m_sCaption);
			}
			else if (sTypeHash == "statusbar"_hash)
			{
				if (m_sCaption != nullptr)
					xdidEX(m_iID, TEXT("-t"), TEXT("%i +%S %S %S \t %S"), cell, ((m_sTFlags != nullptr) ? m_sTFlags : "f"), m_sIcon, m_sCaption, m_sTooltip);
			}
			else if (sTypeHash == "treeview"_hash) {
					// Ook: recursive loop needs looked at, use a TString reference object instead of multiple char[]'s for itemPath
					//itemPath += item;
					//xdidEX(m_iID, TEXT("-a"), TEXT("%s \t +%S %S %S 0 %S %S %S %S %S \t %S"), itemPath.to_chr(), ((m_sTFlags) ? m_sTFlags : "a"), m_sIcon, m_sIcon, m_sState, m_sIntegral, m_sTextcolour, m_sBgcolour, m_sCaption, m_sTooltip);
					//parseItems(child, depth, itemPath);
					char pathx[100];
					wnsprintfA(&pathx[0], static_cast<int>(Dcx::countof(pathx)), "%s %i", itemPath, item);
					if (m_sCaption != nullptr)
						xdidEX(m_iID, TEXT("-a"), TEXT("%S \t +%S %S %S 0 %S %S %S %S %S \t %S"), &pathx[0], ((m_sTFlags != nullptr) ? m_sTFlags : "a"), m_sIcon, m_sIcon, m_sState, m_sIntegral, m_sTextcolour, m_sBgcolour, m_sCaption, m_sTooltip);
					parseItems(child, depth, &pathx[0]);
				}
		}
	}
#else
	auto item = 0;
	auto cell = 0;

	for (auto child = tiElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
	{
		const auto childelem = child->Value();
		if (0 == ts_strcmp(childelem, "columns"))
		{
			if (0 == ts_strcmp(m_sType, "listview"))
			{
				TString tsArguments;
				TString tsBuffer;
				for (auto column = child->FirstChildElement("column"); column != nullptr; column = column->NextSiblingElement("column"))
				{
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

					//tsArguments.addtok(tsBuffer,TEXT('\t'));
				}
				if (!tsArguments.empty())
					xdidEX(m_iID,TEXT("-t"),TEXT("%s"),tsArguments.to_chr());
			}
			//if (0 == ts_strcmp(childelem, "dataset"))
			//{
			//	//auto listView = this->d_Host->getControlByID(m_iID);
			//}
		}


		if ((0 == ts_strcmp(childelem, "item")) || (0 == ts_strcmp(childelem, "control")))
			++cell;
		//fill all required parameters with attributes or default values
		parseAttributes(child);
		if (0 == ts_strcmp(childelem, "item")) {
			++item;
			if (0 == ts_strcmp(m_sType, "toolbar"))
				xdidEX(m_iID,TEXT("-a"),TEXT("0 +%S %S %S %S %S \t %S"), ((m_sTFlags != nullptr) ? m_sTFlags : "a"),m_sWidth,m_sIcon,m_sTextcolour,m_sCaption,m_sTooltip);
			else if (0 == ts_strcmp(m_sType, "comboex"))
				xdidEX(m_iID,TEXT("-a"),TEXT("0 %S %S %S 0 %S"), m_sIndent,m_sIcon,m_sIcon,m_sCaption);
			else if (0 == ts_strcmp(m_sType, "list"))
			{
				if (m_sCaption != nullptr)
					xdidEX(m_iID, TEXT("-a"), TEXT("0 %S"), m_sCaption);
			}
			else if (0 == ts_strcmp(m_sType, "statusbar"))
			{
				if (m_sCaption != nullptr)
					xdidEX(m_iID, TEXT("-t"), TEXT("%i +%S %S %S \t %S"), cell, ((m_sTFlags != nullptr) ? m_sTFlags : "f"), m_sIcon, m_sCaption, m_sTooltip);
			}
			else if (0 == ts_strcmp(m_sType, "treeview")) {
				// Ook: recursive loop needs looked at, use a TString reference object instead of multiple char[]'s for itemPath
				//itemPath += item;
				//xdidEX(m_iID, TEXT("-a"), TEXT("%s \t +%S %S %S 0 %S %S %S %S %S \t %S"), itemPath.to_chr(), ((m_sTFlags) ? m_sTFlags : "a"), m_sIcon, m_sIcon, m_sState, m_sIntegral, m_sTextcolour, m_sBgcolour, m_sCaption, m_sTooltip);
				//parseItems(child, depth, itemPath);
				char pathx[100];
				wnsprintfA(&pathx[0], Dcx::countof(pathx), "%s %i",itemPath,item);
				if (m_sCaption != nullptr)
					xdidEX(m_iID,TEXT("-a"),TEXT("%S \t +%S %S %S 0 %S %S %S %S %S \t %S"), pathx,((m_sTFlags != nullptr) ? m_sTFlags : "a"),m_sIcon,m_sIcon,m_sState,m_sIntegral,m_sTextcolour,m_sBgcolour,m_sCaption,m_sTooltip);
				parseItems(child,depth,&pathx[0]);
			}
		}
	}
#endif
}

/* parseTemplate(recursionDepth,claPath,firstFreeControlId) : finds a template and parses it into the current dialog */
void DcxmlParser::parseTemplate(const UINT dialogDepth,const char *const claPath,const UINT passedid)
{
	// find templates element
	const auto lookIn = getDialogsElement()->FirstChildElement("templates");

	if (lookIn == nullptr)
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

	const refString<const char, -1> sTmp(m_pElement->Attribute("name"));

	for (auto Template = lookIn->FirstChildElement("template"); Template != nullptr; Template = Template->NextSiblingElement())
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
void DcxmlParser::parseDialog(const UINT depth,const char *claPath,const UINT passedid,const bool ignoreParent) {
#if DCX_USE_HASHING
	auto control = 0, cCla = 0, cell = 0;
	g_claPath = nullptr;
	g_claPathx = nullptr;
	g_bResetCLA = false;
	for (auto child = m_pElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
		++cell;
		//STEP 1: SET ELEMENT AND PARENTELEMENT
		if (!ignoreParent)
			m_pParent = child->Parent()->ToElement();
		m_pElement = child->ToElement();

		//STEP 2: PARSE ATTRIBUTES OF ELEMENTS
		parseAttributes();

		const auto sElemHash = dcx_hash(m_sElem);

		//dont itterate over unneccessary items
		if (sElemHash == "calltemplate"_hash)
		{
			const refString<const char, -1> refParentVal(m_pElement->Parent()->ToElement()->Value());
			if (refParentVal != "template")
			{
				++cCla;
				m_pTemplateRef = m_pElement;
				m_iTemplateRefcCla = cCla;

				sString <100> t_buffer;
				const char * t_claPathx = t_buffer;
				wnsprintfA(t_buffer, static_cast<int>(t_buffer.size()), "%i", cCla);

				for (auto attribute = m_pElement->FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
				{
					const refString<const char, -1> refName(attribute->Name());
					if (refName == "name")
						continue;
					m_mTemplate_vars[refName] = attribute->Value();
				}
				for (auto iter = m_mTemplate_vars.begin(); iter != m_mTemplate_vars.end(); ++iter) {
					mIRCLinker::execex(TEXT("//set %%%S %S"), iter->first, iter->second);
				}
				m_sTemplateRefclaPath = t_claPathx;
				parseTemplate(depth, claPath, passedid);
				m_pTemplateRef = nullptr;
				for (auto iter = m_mTemplate_vars.begin(); iter != m_mTemplate_vars.end(); ++iter) {
					mIRCLinker::execex(TEXT("//unset %%%S"), iter->first);
				}
				m_mTemplate_vars.clear();
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
			m_iID = parseId(m_pElement);
			
			if (m_iID == 0)
				m_iID = DCXML_ID_OFFSET + m_iControls;	// this is user id, so will later have mIRC_ID_OFFSET added to it.

			registerId(m_pElement, m_iID); // Ook: does this twice??
										   //registerId(m_pElement,m_iID);
		}
		else
			m_iID = passedid;

		//assign m_pParent CONTROL of m_pElement
		while (m_pParent != nullptr) {
			if (0 == ts_strcmp(m_sParentelem, "template"))
			{
				m_pParent = m_pTemplateRef->Parent()->ToElement();
				m_sParentelem = m_pTemplateRef->Parent()->Value();
				cCla = m_iTemplateRefcCla;
				claPath = m_sTemplateRefclaPath;
			}
			else if (0 == ts_strcmp(m_sParentelem, "pane"))
			{
				m_pParent = m_pParent->Parent()->ToElement();
				m_sParentelem = m_pParent->Value();
			}
			else break;
		}
		if (m_pParent != nullptr)
			m_sParenttype = queryAttribute(m_pParent, "type", "panel");
		else
			m_sParenttype = "panel";

		//if (m_pParent != nullptr)
		//	m_sParenttype = m_pParent->Attribute("type");
		//else
		//	m_sParenttype = nullptr;
		//
		//if (m_sParenttype == nullptr)
		//	m_sParenttype = "panel";

		m_iParentID = parseId(m_pParent);
		if (m_iParentID <= 0)
			m_iParentID = passedid;
		//IF TEMPLATE ELEMENT REROUTE TO TEMPLATE DEFINITION


		//STEP 3: IF CONTROL CREATE IT AND ITS ITEMS
		if (sElemHash == "control"_hash) {

			++control;
			// control now always > 0

			//check how to insert the control in the m_pParent Control/Dialog
			//If parentNode is pane loop untill parentNode is not a pane
			const auto sParentelemHash = dcx_hash(m_sParentelem);

			if (sParentelemHash == "dialog"_hash)
				xdialogEX(TEXT("-c"), TEXT("%u %S 0 0 %S %S %S"), m_iID, m_sType, m_sWidth, (m_sDropdown != nullptr ? m_sDropdown : m_sHeight), m_sStyles);
			else if (sParentelemHash == "control"_hash) {
				switch (dcx_hash(m_sParenttype))
				{
				case "pager"_hash:
					if (control != 1)
						break;
				case "panel"_hash:
				case "box"_hash:
					xdidEX(m_iParentID, TEXT("-c"), TEXT("%u %S 0 0 %S %S %S"), m_iID, m_sType, m_sWidth, (m_sDropdown != nullptr ? m_sDropdown : m_sHeight), m_sStyles);
					break;
				case "tab"_hash:
					xdidEX(m_iParentID, TEXT("-a"), TEXT("0 %S %S \t %u %S 0 0 %S %S %S \t %S"), m_sIcon, m_sCaption, m_iID, m_sType, m_sWidth, (m_sDropdown != nullptr ? m_sDropdown : m_sHeight), m_sStyles, m_sTooltip);
					break;
				case "divider"_hash:
					if (control <= 2) {
						// <= 2 so MUST be either 1 or 2, can't be zero
						if (control == 1)
							xdidEX(m_iParentID, TEXT("-l"), TEXT("%S 0 \t %u %S 0 0 %S %S %S"), m_sWidth, m_iID, m_sType, m_sWidth, (m_sDropdown != nullptr ? m_sDropdown : m_sHeight), m_sStyles);
						else
							xdidEX(m_iParentID, TEXT("-r"), TEXT("%S 0 \t %u %S 0 0 %S %S %S"), m_sWidth, m_iID, m_sType, m_sWidth, (m_sDropdown != nullptr ? m_sDropdown : m_sHeight), m_sStyles);
					}
					break;
				case "rebar"_hash:
				{
					const char *flags = (m_sTFlags) ? m_sTFlags : "ceg";
					xdidEX(m_iParentID, TEXT("-a"), TEXT("0 +%S %S %S %S %S %S %S \t %u %S 0 0 %S %S %S \t %S"), flags, m_sRebarMinWidth, m_sRebarMinHeight, m_sWidth, m_sIcon, m_sTextcolour, m_sCaption, m_iID, m_sType, m_sWidth, (m_sDropdown != nullptr ? m_sDropdown : m_sHeight), m_sStyles, m_sTooltip);
				}
				break;
				case "stacker"_hash:
					xdidEX(m_iParentID, TEXT("-a"), TEXT("0 + %S %S %S \t %u %S 0 0 %S %S %S"), m_sTextcolour, m_sBgcolour, m_sCaption, m_iID, m_sType, m_sWidth, (m_sDropdown != nullptr ? m_sDropdown : m_sHeight), m_sStyles);
					break;
				case "statusbar"_hash:
					xdidEX(m_iParentID, TEXT("-t"), TEXT("%i +c %S %u %S 0 0 0 0 %S"), cell, m_sIcon, m_iID, m_sType, m_sStyles);
				default:
					break;
				}
			}
		}
		//Set CLA for control or pane
		g_claPath = claPath;
		const auto claPathx(parseCLA(cCla));

		//Perform some control specific commands
		if (sElemHash == "control"_hash) {
			parseControl();
			parseIcons();
			parseStyle();
		}
		//char *claPathx = "root";
		//mIRCLinker::execex(TEXT("//echo -a clapath:%s"),claPathx);
		parseDialog(depth + 1, claPathx.c_str(), m_iID);
	}
#else
	int control = 0, cCla = 0, cell = 0;
	g_claPath = nullptr;
	g_claPathx = nullptr;
	g_bResetCLA = false;
	for( auto child = m_pElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement() ) {
		cell++;
		//STEP 1: SET ELEMENT AND PARENTELEMENT
		if (!ignoreParent)
			m_pParent = child->Parent()->ToElement();
		m_pElement = child->ToElement();

		//STEP 2: PARSE ATTRIBUTES OF ELEMENTS
		parseAttributes();

		//dont itterate over unneccessary items
		if (0==lstrcmpA(m_sElem, "calltemplate")) 
		{
			if (0!=lstrcmpA("template",m_pElement->Parent()->ToElement()->Value()))
			{
				++cCla;
				m_pTemplateRef = m_pElement;
				m_iTemplateRefcCla = cCla;
				
				//char t_buffer[100] = { 0 };
				//const char * t_claPathx = nullptr;
				//wnsprintfA(t_buffer, Dcx::countof(t_buffer), "%i",cCla);
				//t_claPathx = t_buffer;

				sString <100> t_buffer;
				const char * t_claPathx = t_buffer;
				wnsprintfA(t_buffer, t_buffer.size(), "%i", cCla);

				for (auto attribute = m_pElement->FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
				{
					//const auto name = attribute->Name();
					//auto value = attribute->Value();
					//if (0==lstrcmpA(name, "name")) continue;
					//m_mTemplate_vars[name] = value;

					const refString<const char, -1> refName(attribute->Name());
					if (refName == "name")
						continue;
					m_mTemplate_vars[refName] = attribute->Value();
				}
				for (auto iter = m_mTemplate_vars.begin(); iter != m_mTemplate_vars.end(); ++iter) {
					mIRCLinker::execex(TEXT("//set %%%S %S"),iter->first,iter->second);
				}
				m_sTemplateRefclaPath = t_claPathx;
				parseTemplate(depth,claPath,passedid);
				m_pTemplateRef = nullptr;
				for (auto iter = m_mTemplate_vars.begin(); iter != m_mTemplate_vars.end(); ++iter) {
					mIRCLinker::execex(TEXT("//unset %%%S"),iter->first);
				}
				m_mTemplate_vars.clear();
			}
			continue;
		}
		if ((0==lstrcmpA(m_sElem, "control")) || (0==lstrcmpA(m_sElem, "pane")))
			++cCla;
		else
			continue;

		//asign ID 
		if (0==lstrcmpA(m_sElem, "control")) 
		{ 
			++m_iControls;
			m_iID = parseId(m_pElement);
			if (m_iID <= 0)
				m_iID = 2000 + m_iControls;	// Ook: 2000+ ? not mIRC_ID_OFFSET + ?
			registerId(m_pElement,m_iID); // Ook: does this twice??
			//registerId(m_pElement,m_iID);
		}
		else
			m_iID = passedid;

		//assign m_pParent CONTROL of m_pElement
		while (m_pParent != nullptr) {
			if (0==lstrcmpA(m_sParentelem, "template")) 
			{
				m_pParent = m_pTemplateRef->Parent()->ToElement();
				m_sParentelem = m_pTemplateRef->Parent()->Value();
				cCla = m_iTemplateRefcCla;
				claPath = m_sTemplateRefclaPath;
			}
			if (0==lstrcmpA(m_sParentelem, "pane")) 
			{ 
				m_pParent = m_pParent->Parent()->ToElement();
				m_sParentelem = m_pParent->Value();
			}
			else break;    
		}
		if (m_pParent != nullptr)
			m_sParenttype = m_pParent->Attribute("type");
		else
			m_sParenttype = nullptr;

		if (m_sParenttype == nullptr)
			m_sParenttype = "panel";

		m_iParentID = parseId(m_pParent);
		if (m_iParentID <= 0)
			m_iParentID = passedid;
		//IF TEMPLATE ELEMENT REROUTE TO TEMPLATE DEFINITION


		//STEP 3: IF CONTROL CREATE IT AND ITS ITEMS
		if (0==lstrcmpA(m_sElem, "control")) {

			++control;

			//check how to insert the control in the m_pParent Control/Dialog
			//If parentNode is pane loop untill parentNode is not a pane
			if (0==lstrcmpA(m_sParentelem, "dialog"))
				xdialogEX(TEXT("-c"),TEXT("%i %S 0 0 %S %S %S"), m_iID,m_sType,m_sWidth,(m_sDropdown != nullptr ? m_sDropdown : m_sHeight),m_sStyles);
			else if (0==lstrcmpA(m_sParentelem, "control")) {
				if (0==lstrcmpA(m_sParenttype, "panel"))
					this->xdidEX(m_iParentID,TEXT("-c"),TEXT("%i %S 0 0 %S %S %S"), m_iID,m_sType,m_sWidth,(m_sDropdown != nullptr ? m_sDropdown : m_sHeight),m_sStyles);
				else if (0==lstrcmpA(m_sParenttype, "box"))
					this->xdidEX(m_iParentID,TEXT("-c"),TEXT("%i %S 0 0 %S %S %S"), m_iID,m_sType,m_sWidth,(m_sDropdown != nullptr ? m_sDropdown : m_sHeight),m_sStyles);
				else if (0==lstrcmpA(m_sParenttype, "tab"))
					this->xdidEX(m_iParentID,TEXT("-a"),TEXT("0 %S %S \t %i %S 0 0 %S %S %S \t %S"), m_sIcon,m_sCaption,m_iID,m_sType,m_sWidth,(m_sDropdown != nullptr ? m_sDropdown : m_sHeight),m_sStyles,m_sTooltip);
				else if ((0==lstrcmpA(m_sParenttype, "pager")) && (control == 1))
					this->xdidEX(m_iParentID,TEXT("-c"),TEXT("%i %S 0 0 %S %S %S"), m_iID,m_sType,m_sWidth,(m_sDropdown != nullptr ? m_sDropdown : m_sHeight),m_sStyles);
				else if (0==lstrcmpA(m_sParenttype, "divider") && (control <= 2)) {
					if (control == 1)
						xdidEX(m_iParentID,TEXT("-l"),TEXT("%S 0 \t %i %S 0 0 %S %S %S"), m_sWidth,m_iID,m_sType,m_sWidth,(m_sDropdown != nullptr ? m_sDropdown : m_sHeight),m_sStyles);
					else if (control == 2)
						xdidEX(m_iParentID,TEXT("-r"),TEXT("%S 0 \t %i %S 0 0 %S %S %S"), m_sWidth,m_iID,m_sType,m_sWidth,(m_sDropdown != nullptr ? m_sDropdown : m_sHeight),m_sStyles);
				}
				else if (0==lstrcmpA(m_sParenttype, "rebar")) { 
					const char *flags = (m_sTFlags) ? m_sTFlags : "ceg";
					xdidEX(m_iParentID,TEXT("-a"),TEXT("0 +%S %S %S %S %S %S %S \t %i %S 0 0 %S %S %S \t %S"), flags,m_sRebarMinWidth,m_sRebarMinHeight,m_sWidth,m_sIcon,m_sTextcolour,m_sCaption,m_iID,m_sType,m_sWidth,(m_sDropdown != nullptr ? m_sDropdown : m_sHeight),m_sStyles,m_sTooltip);
				}
				else if (0==lstrcmpA(m_sParenttype, "stacker")) 
					xdidEX(m_iParentID,TEXT("-a"),TEXT("0 + %S %S %S \t %i %S 0 0 %S %S %S"), m_sTextcolour,m_sBgcolour,m_sCaption,m_iID,m_sType,m_sWidth,(m_sDropdown != nullptr ? m_sDropdown : m_sHeight),m_sStyles);
				else if (0==lstrcmpA(m_sParenttype, "statusbar"))
					xdidEX(m_iParentID,TEXT("-t"),TEXT("%i +c %S %i %S 0 0 0 0 %S"), cell,m_sIcon,m_iID,m_sType,m_sStyles);
			}
		}
		//Set CLA for control or pane
		g_claPath = claPath;
		const auto claPathx(parseCLA(cCla));

		//Perform some control specific commands
		if (0==lstrcmpA(m_sElem, "control")) {
			parseControl();
			parseIcons();	
			parseStyle();
		}
		//char *claPathx = "root";
		//mIRCLinker::execex(TEXT("//echo -a clapath:%s"),claPathx);
		parseDialog(depth+1,claPathx.c_str(),m_iID);
	}
#endif
} 

// NB: never returns a ZERO, other code relies on this.
int DcxmlParser::mIRCEvalToUnsignedInt (const TString &value)
{
	//Todo: method returns -1 for failure which odd for a *ToUnsignedInt method.
	__int64 iNum;
	mIRCLinker::iEval(&iNum, value.to_chr());
	return (int)((iNum > 0) ? iNum : -1);
}

void DcxmlParser::registerId(const TiXmlElement *const idElement, const UINT iNewID)
{
	auto elementId = 0;
	if (idElement->QueryIntAttribute("id", &elementId) != TIXML_SUCCESS) //<! id attr. is not an int
	{
		const TString elementNamedId(idElement->Attribute("id"));
		if (!elementNamedId.empty()) {
			if (mIRCEvalToUnsignedInt(elementNamedId) < 0) //<! id attr. doesn't evaluate to an int
			{
				getDialog()->AddNamedId(elementNamedId, iNewID + mIRC_ID_OFFSET);
			}
		}
	}
}

UINT DcxmlParser::parseId(const TiXmlElement *const idElement)
{
	if (idElement == nullptr)
		return 0U;

	//<! if id attribute is already integer return it
	auto local_id = 0;
	if (idElement->QueryIntAttribute("id", &local_id) == TIXML_SUCCESS)
	{
		// found ID as a number,  if its not a negative, return it.
		return static_cast<UINT>(std::max(local_id, 0));
	}

	const TString attributeIdValue(idElement->Attribute("id"));
	if (!attributeIdValue.empty())
	{
		// got ID attrib, evaluate it to try & resolve to a number.
		local_id = mIRCEvalToUnsignedInt(attributeIdValue);
		// if ID is > zero return it.
		if (local_id > 0)
			return static_cast<UINT>(local_id);

		// didn't evaluate to a number, so must be a name...
		//auto it = getDialog()->getNamedIds().find(attributeIdValue);
		//if (it != getDialog()->getNamedIds().end())
		//	return it->second;

		return getDialog()->NameToUserID(attributeIdValue);
	}
	return 0U;
}

