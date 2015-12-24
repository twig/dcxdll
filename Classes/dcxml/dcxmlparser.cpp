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
DcxmlParser::DcxmlParser() :
	loadSuccess(false),
	d_Host(nullptr),
	root(nullptr),
	dialogs(nullptr), dialog(nullptr),
	element(nullptr), parent(nullptr),
	controls(0),
	zlayered(0),
	id(0), parentid(0),
	elem(nullptr),	parentelem(nullptr), parenttype(nullptr),	type(nullptr),
	STclass(nullptr),
	weight(nullptr), height(nullptr), dropdown(nullptr), width(nullptr), margin(nullptr),
	styles(nullptr),
	caption(nullptr),
	tooltip(nullptr),
	cascade(nullptr),
	icon(nullptr),
	tFlags(nullptr),
	integral(nullptr), state(nullptr), indent(nullptr),
	src(nullptr),
	cells(nullptr),
	rebarMinHeight(nullptr), rebarMinWidth(nullptr),
	iconsize(nullptr),
	fontstyle(nullptr), charset(nullptr), fontsize(nullptr), fontname(nullptr),
	border(nullptr),
	cursor(nullptr),
	bgcolour(nullptr), textbgcolour(nullptr), textcolour(nullptr),
	gradientend(nullptr), gradientstart(nullptr),
	disabledsrc(nullptr), hoversrc(nullptr), selectedsrc(nullptr),
	templateRef(nullptr), templateRefcCla(0), templateRefclaPath(nullptr),
	eval(0),
	temp(nullptr),
	g_claPath(nullptr), g_claPathx(nullptr), g_resetcla(0),
	_verbose(false), _autoClose(false), _zlayered(false), _dcxDialog(nullptr), _rootElement(nullptr), _dialogElement(nullptr), _dialogsElement(nullptr)
{
}
DcxmlParser::~DcxmlParser() {
}

bool DcxmlParser::ParseXML(const TString &tsFilePath,const TString &tsDialogMark,const TString &tsDialogName, const bool verbose, const bool autoClose)
{
	this->loadSuccess = true;

	this->isVerbose(verbose);
	this->isAutoClose(autoClose);

	this->setFilePath(tsFilePath);

	this->setDialogMark(tsDialogMark);
	this->setDialogName(tsDialogName);

	//if (!this->loadDialog() || !this->loadDocument() || !this->loadDialogElement() )
	//{
	//	this->loadSuccess = false;
	//	if ((this->getDialog() != nullptr) && this->isAutoClose())
	//		mIRCLinker::execex(TEXT("/xdialog -x %s"),tsDialogName.to_chr());
	//
	//	return false;
	//}
	try {
		this->loadDialog();
		this->loadDocument();
		this->loadDialogElement();

		const auto tx = this->getDialogElement();

		mIRCLinker::execex(TEXT("/dialog -s %s %i %i %i %i"), //!< Sets the dialog size.
			tsDialogMark.to_chr(),
			queryIntAttribute(tx, "x", -1),
			queryIntAttribute(tx, "y", -1),
			queryIntAttribute(tx, "w", -1),
			queryIntAttribute(tx, "h", -1)
			);

		this->setZlayered(false);

		const auto tCascade = queryAttribute(tx, "cascade", "v");
		const auto tMargin = queryAttribute(tx, "margin", "0 0 0 0");
		const auto tCaption = queryAttribute(tx, "caption", tsDialogName.c_str());
		const auto tBorder = tx->Attribute("border");
		this->controls = 0;
		//margin = (temp = element->Attribute("padding")) ? temp : "0 0 0 0";

		if (tBorder != nullptr) //!< set border ONLY if defined on <dialog>
			this->xdialogEX(TEXT("-b"), TEXT("+%S"), tBorder);

		mIRCLinker::execex(TEXT("//dialog -t %s %S"), tsDialogMark.to_chr(), tCaption);
		this->xdialogEX(TEXT("-l"), TEXT("root \t +p%S 0 0 0 0"), tCascade);
		this->xdialogEX(TEXT("-l"), TEXT("space root \t + %S"), tMargin);

		this->parseDialog(); //!< Parse <dialog> children onto the dialog

		if (tx->Attribute("center") != nullptr) //!< Centers the dialog
			mIRCLinker::execex(TEXT("/dialog -r %s"), tsDialogMark.to_chr());

		//This "Shite" is to activate the first zlayer, added a check if this command starts returning an error
		if (this->getZlayered())
			this->xdialogEX(TEXT("-z"), TEXT("+s 1"));

		mIRCLinker::execex(TEXT("/.timer 1 0 %s %s ready"), this->getDialog()->getAliasName().to_chr(), tsDialogMark.to_chr()); //!< Tell user that dcxml is finished, & they can do a cla update or whatever.
		return this->loadSuccess;
	}
	catch (std::exception)
	{
		this->loadSuccess = false;
		if ((this->getDialog() != nullptr) && this->isAutoClose())
			mIRCLinker::execex(TEXT("/xdialog -x %s"), tsDialogName.to_chr());
		throw;
	}
}

void DcxmlParser::setDialog(const TString &tsDialogMark) { this->_dcxDialog = Dcx::Dialogs.getDialogByName(tsDialogMark);	}

void DcxmlParser::loadDocument()
{
	const auto tsPath(this->getFilePath());

	if (!Dcx::isFile(tsPath.to_chr()))
		throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("File \"%s\" does not exist or is in use by another process "), this->getFilePath().to_chr()));

	TiXmlBase::SetCondenseWhiteSpace(false);
	TiXmlDocument doc(tsPath.c_str());
	this->loadSuccess = doc.LoadFile();
	if (!this->loadSuccess)
		throw std::runtime_error(Dcx::dcxGetFormattedString(TEXT("XML error in \"%s\" (row %i column %i) %S"), tsPath.to_chr(), doc.ErrorRow(), doc.ErrorCol(), doc.ErrorDesc()));

	this->_document = doc; 
}

void DcxmlParser::loadDialog()
{
	this->setDialog(this->getDialogMark());
	if (this->getDialog() == nullptr)
		throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("No such dialog (\"%s\") has been marked with dcx."),this->getDialogMark().to_chr()));
}

void DcxmlParser::loadDialogElement()
{
	this->setRootElement(this->getDocument()->FirstChildElement("dcxml"));
	if (this->getRootElement() == nullptr)
		throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("The document element should be <dcxml> in \"%s\""), this->getFilePath().to_chr()));

	this->setDialogsElement(this->getRootElement()->FirstChildElement("dialogs"));
	if (this->getDialogsElement() == nullptr) 
		throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Theres no <dialogs> child for <dcxml> in \"%s\""), this->getFilePath().to_chr()));

	/*
	* This finds the Dialog element. 
	* Important Note:
	* The for loop has an ugly side assigment: it sets this->element 
	* which is used as a recursion cursor for this->parseDialog()
	*/
	for(this->element = this->getDialogsElement()->FirstChildElement("dialog"); this->element != nullptr; this->element = this->element->NextSiblingElement("dialog"))
	{
		if (0==lstrcmpA(this->element->Attribute("name"), this->getDialogName().c_str()))
		{
			this->setDialogElement(this->element);
			return;
		}
	}
	throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Theres no <dialog> element with attribute name=\"%s\" in \"%s\""), this->getDialogName().to_chr(), this->getFilePath().to_chr()));
}

void DcxmlParser::parseAttributes() {
	parseAttributes(element);
}

void DcxmlParser::parseAttributes(const TiXmlElement *const tElement) {
	elem = tElement->Value();
	parentelem = parent->Value();
	parenttype = queryAttribute(parent, "type", "panel");
	type = queryAttribute(element,"type","panel");
	STclass = queryAttribute(element,"class","");
	weight = queryAttribute(tElement, "weight", "1");
	height = queryAttribute(tElement, "height", "0");
	if (0 == lstrcmpA(elem, "comboex") || 0 == lstrcmpA(elem, "colorcombo"))
		dropdown = queryAttribute(tElement, "dropdown", "100");
	else
		dropdown = nullptr;

	width = queryAttribute(tElement, "width", "0");
	margin = queryAttribute(tElement, "margin", "0 0 0 0");
	styles = queryAttribute(tElement, "styles", "");
	temp = tElement->Attribute("caption");
	if (temp == nullptr)
		temp = tElement->GetText();
	caption = (temp != nullptr) ? temp : "";
	tooltip = queryAttribute(tElement, "tooltip", "");
	cascade = queryAttribute(tElement, "cascade", "");
	icon = queryAttribute(tElement, "icon", "0");
	integral = queryAttribute(tElement, "integral", "0");
	state = queryAttribute(tElement, "state", "0");
	indent = queryAttribute(tElement, "indent", "0");
	//flags attribute defaults different per type/item
	tFlags = tElement->Attribute("flags");
	src = queryAttribute(tElement, "src", "");
	cells = queryAttribute(tElement, "cells", "-1");
	rebarMinHeight = queryAttribute(tElement, "minheight", "0");
	rebarMinWidth = queryAttribute(tElement, "minwidth", "0");
	iconsize = queryAttribute(tElement, "iconsize", "16");
	eval = queryIntAttribute(tElement, "eval");

	fontstyle = queryAttribute(tElement, "fontstyle", "d");
	charset = queryAttribute(tElement, "charset", "ansi");
	fontsize = queryAttribute(tElement, "fontsize", "");
	fontname = queryAttribute(tElement, "fontname", "");
	border = queryAttribute(tElement, "border", "");
	cursor = queryAttribute(tElement, "cursor", "arrow");
	bgcolour = queryAttribute(tElement, "bgcolour", "0");
	textbgcolour = queryAttribute(tElement, "textbgcolour", "");
	textcolour = queryAttribute(tElement, "textcolour", "0");

	gradientstart = queryAttribute(tElement, "gradientstart", "");
	gradientend = queryAttribute(tElement, "gradientend", "");

	disabledsrc = queryAttribute(tElement, "disabledsrc", "");
	hoversrc = queryAttribute(tElement, "hoversrc", "");
	selectedsrc = queryAttribute(tElement, "selectedsrc", "");
}

/* parseControl() : if current element is a control perform some extra commands*/
void DcxmlParser::parseControl() { 
	if (element->Attribute("zlayer") != nullptr) {
		this->xdialogEX(TEXT("-z"),TEXT("+a %i"),id);
		this->setZlayered(true);
	}
	//        padding = (temp = element->Attribute("padding")) ? temp : "0 0 0 0";
	const TString tsParent(parenttype);
	static const TString poslist(TEXT("divider toolbar button treeview comboex list listview radio link check box ipaddress webctrl text edit richedit pbar statusbar image"));
#if TSTRING_TEMPLATES
	const auto nType = poslist.findtok(tsParent, 1);
#else
	const auto nType = poslist.findtok(tsParent.to_chr(), 1);
#endif

	switch (nType)
	{
	case 1:		//	divider
		xdidEX(parentid, TEXT("-v"), TEXT("%d"), queryIntAttribute(element, "width", 0));
		break;
	case 3:		//	button
		xdidEX(id, TEXT("-l"), TEXT("%S"), iconsize);
	case 8:		//	radio
	case 9:		//	link
	case 10:	//	check
	case 11:	//	box
		if (caption != nullptr)
			xdidEX(id, TEXT("-t"), TEXT("%S"), caption);
		break;
	case 2:		//	toolbar
	case 4:		//	treeview
		xdidEX(id, TEXT("-l"), TEXT("%S"), iconsize);
	case 5:		//	comboex
	case 6:		//	list
	case 7:		//	listview
		this->parseItems(element);
		break;
	case 12:	//	ipaddress
		this->xdidEX(id, TEXT("-a"), TEXT("%S"), caption);
		break;
	case 13:	//	webctrl
		this->xdidEX(id, TEXT("-n"), TEXT("%S"), src);
		break;
	case 14:	//	text
		if (caption != nullptr) {
			TString mystring(caption);

			if (mystring.left(2) == TEXT("\r\n"))
				mystring = mystring.right(-2);
			else if (mystring[0] == TEXT('\n'))
				mystring = mystring.right(-1);

			mystring -= TEXT("\t"); // remove all '\t' from text.

			int textspace = 0;

#if TSTRING_PARTS
			TString printstring;
			for (const auto &tsTmp: mystring)
			{
#if TSTRING_TEMPLATES
				printstring.addtok(tsTmp);
#else
				printstring.addtok(tsTmp.to_chr());
#endif
				if (printstring.len() > (MIRC_BUFFER_SIZE_CCH - 100)) {
					this->xdidEX(id, TEXT("-a"), TEXT("%i %s"), textspace, printstring.to_chr());
					printstring.clear();	// = TEXT("");
					textspace = 1;
				}
			}
			if (!printstring.empty())
				this->xdidEX(id, TEXT("-a"), TEXT("%i %s"), textspace, printstring.to_chr());
#else
			TString printstring;
			for (auto tsTmp(mystring.getfirsttok(1)); !tsTmp.empty(); tsTmp = mystring.getnexttok())
			{
#if TSTRING_TEMPLATES
				printstring.addtok(tsTmp);
#else
				printstring.addtok(tsTmp.to_chr());
#endif
				if (printstring.len() > (MIRC_BUFFER_SIZE_CCH - 100)) {
					this->xdidEX(id, TEXT("-a"), TEXT("%i %s"), textspace, printstring.to_chr());
					printstring.clear();	// = TEXT("");
					textspace = 1;
				}
			}
			if (!printstring.empty())
				this->xdidEX(id, TEXT("-a"), TEXT("%i %s"), textspace, printstring.to_chr());
#endif
		}
		break;
	case 15:	//	edit
	case 16:	//	richedit
		if (caption != nullptr) {
			TString mystring(caption);

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
#if TSTRING_PARTS
#if TSTRING_ITERATOR
			for (auto itStart = mystring.begin(TEXT("\r\n")), itEnd = mystring.end(); itStart != itEnd; ++itStart)
			{
				line++;
				this->xdidEX(id, TEXT("-i"), TEXT("%u %s"), line, (*itStart).to_chr());
			}
#else
			mystring.split(TEXT("\r\n"));
			for (const auto &tsTmp: mystring)
			{
				line++;
				this->xdidEX(id, TEXT("-i"), TEXT("%u %s"), line, tsTmp.to_chr());
			}
#endif
#else
			for (auto tsTmp(mystring.getfirsttok(1, TEXT("\r\n"))); !tsTmp.empty(); tsTmp = mystring.getnexttok(TEXT("\r\n")))
			{
				line++;
				this->xdidEX(id, TEXT("-i"), TEXT("%u %s"), line, tsTmp.to_chr());
			}
#endif
		}
		break;
	case 17:	//	pbar
		if (caption != nullptr)
			this->xdidEX(id, TEXT("-i"), TEXT("%S"), caption);
		break;
	case 18:	//	statusbar
		this->xdidEX(id, TEXT("-l"), TEXT("%S"), cells);
		this->parseItems(element);
		break;
	case 19:	//	image
		this->xdidEX(id, TEXT("-i"), TEXT("+%S %S"), ((tFlags != nullptr) ? tFlags : ""), src);
	default:	//	unknown?!?!?!
		break;
	}

	disabledsrc = queryAttribute(element, "disabledsrc", "");
	hoversrc = queryAttribute(element, "hoversrc", "");
}

/* xdialogEX(switch,format[,args[]]) : performs an xdialog command internally or through mIRC */
void DcxmlParser::xdialogEX(const TCHAR *const sw,const TCHAR *const dFormat, ...) { 
	va_list args;
	TString txt;

	va_start(args, dFormat);
	txt.tvprintf(dFormat, args);
	va_end(args);

#if DCX_DEBUG_OUTPUT
	if (this->isVerbose())
		mIRCLinker::execex(TEXT("/echo -a dcxml debug: /xdialog %s %s %s"),sw,this->getDialogMark().to_chr(),txt.to_chr());
#endif

	if (eval > 0) mIRCLinker::execex(TEXT("//xdialog %s %s %s"), sw, this->getDialogMark().to_chr(), txt.to_chr());
	else this->getDialog()->parseCommandRequestEX(TEXT("%s %s %s"), this->getDialogMark().to_chr(), sw, txt.to_chr());
}

/* xdidEX(controlId,switch,format[,args[]]) : performs an xdid command internally or through mIRC on the specified id */
void DcxmlParser::xdidEX(const int cid,const TCHAR *const sw,const TCHAR *const dFormat, ...) { 
	va_list args;
	TString txt;

	va_start(args, dFormat);
	txt.tvprintf(dFormat, args);
	va_end(args);

	if (this->isVerbose())
		mIRCLinker::execex(TEXT("/echo -a dcxml debug: /xdid %s %s %i %s"),sw,this->getDialogMark().to_chr(),cid,txt.to_chr());

	if (eval > 0) mIRCLinker::execex(TEXT("//xdid %s %s %i %s"),sw,this->getDialogMark().to_chr(),cid,txt.to_chr());
	else this->getDialog()->parseComControlRequestEX(cid,TEXT("%s %i %s %s"),this->getDialogMark().to_chr(),cid,sw,txt.to_chr());
}

/* parseCLA(int numberOfClaControlsInCurrentBranch) : parses control and pane elements and applies the right CLA commands */
TString DcxmlParser::parseCLA(const int cCla) { 
	if (0==lstrcmpA(elem, "control")) {
		if ((0==lstrcmpA(type, "panel")) || (0==lstrcmpA(type, "box"))) {
			this->xdidEX(id,TEXT("-l"),TEXT("root \t +p%S 0 0 0 0"),cascade);
			this->xdidEX(id,TEXT("-l"),TEXT("space root \t + %S"),margin);
			g_resetcla = 1;
		}
		const char * fHeigth = "";
		const char * fWidth = "";
		const char * fixed = "l";
		if (element->Attribute("height") != nullptr) { fHeigth = "v"; fixed = "f"; weight = "0"; }
		if (element->Attribute("width") != nullptr) { fWidth = "h"; fixed = "f"; weight = "0"; }
		if (0==lstrcmpA(parentelem, "dialog"))
			this->xdialogEX(TEXT("-l"),TEXT("cell %S \t +%S%S%Si %i %S %S %S"),	g_claPath,fixed,fHeigth,fWidth,id,weight,width,height);
		else if (0==lstrcmpA(parentelem, "control")) {
			const auto t_type = parent->Attribute("type");
			if ((t_type != nullptr) && (parentid > 0)) {
				if (0==lstrcmpA(t_type, "panel"))
					this->xdidEX(parentid,TEXT("-l"),TEXT("cell %S \t +%S%S%Si %i %S %S %S"), g_claPath,fixed,fHeigth,fWidth,id,weight,width,height); 
				else if (0==lstrcmpA(t_type, "box"))
					this->xdidEX(parentid,TEXT("-l"),TEXT("cell %S \t +%S%S%Si %i %S %S %S"), g_claPath,fixed,fHeigth,fWidth,id,weight,width,height); 
			}
		}
	}
	else if (0==lstrcmpA(elem, "pane")) {
		if (0==lstrcmpA(parentelem, "dialog"))
			this->xdialogEX(TEXT("-l"),TEXT("cell %S \t +p%S 0 %S 0 0"),g_claPath,cascade,weight);
		else if (0==lstrcmpA(parentelem, "control")) {
			if ((parenttype != nullptr) && (parentid > 0)) {
				if (0==lstrcmpA(parenttype, "panel"))
					this->xdidEX(parentid,TEXT("-l"),TEXT("cell %S \t +p%S 0 %S 0 0"),g_claPath,cascade,weight);
				else if (0==lstrcmpA(parenttype, "box"))
					this->xdidEX(parentid,TEXT("-l"),TEXT("cell %S \t +p%S 0 %S 0 0"),g_claPath,cascade,weight);
			}
		}
	}
	TString claPathx;
	if (g_resetcla > 0)
		claPathx = TEXT("root");
	else if (0==lstrcmpA(g_claPath, "root"))
		claPathx.tsprintf(TEXT("%i"),cCla);
	else
		claPathx.tsprintf(TEXT("%S %i"),g_claPath,cCla);

	if (element->Attribute("margin") != nullptr) {
		if (0==lstrcmpA(parentelem, "dialog"))
			this->xdialogEX(TEXT("-l"),TEXT("space %s \t + %S"),claPathx.to_chr(),margin);
		else
			this->xdidEX(parentid,TEXT("-l"),TEXT("space %S \t + %S"),g_claPath,margin);
	}
	g_resetcla = 0;
	return claPathx;
}

/* setStyle(TiXmlElement*) : Applies the styles described on the element found by parseStyle() */
void DcxmlParser::setStyle(const TiXmlElement *const style) {
	//style attributes evaluate by default unless eval="0" is set on the element explicitly

	eval = queryIntAttribute(style,"eval",1);

	//font
	fontstyle = queryAttribute(style,"fontstyle","d");
	charset = queryAttribute(style,"charset","ansi");
	fontsize = queryAttribute(style,"fontsize","");
	fontname = queryAttribute(style,"fontname","");
	if ((style->Attribute("fontsize") != nullptr) || (style->Attribute("fontname") != nullptr))
		this->xdidEX(id,TEXT("-f"),TEXT("+%S %S %S %S"), fontstyle, charset, fontsize, fontname);
	//border
	border = queryAttribute(style,"border","");
	if (lstrlenA(border))
		this->xdidEX(id,TEXT("-x"),TEXT("+%S"),border);
	//colours
	cursor = queryAttribute(style,"cursor","arrow");
	bgcolour = queryAttribute(style,"bgcolour","");
	textbgcolour = queryAttribute(style,"textbgcolour","");
	textcolour = queryAttribute(style,"textcolour","");
	if (style->Attribute("bgcolour") != nullptr) {
		this->xdidEX(id,TEXT("-C"),TEXT("+b %s"),bgcolour);
		if (0==lstrcmpA(type, "pbar")) 
		{
			this->xdidEX(id,TEXT("-k"),TEXT("%S"),bgcolour);
			this->xdidEX(id,TEXT("-U"),TEXT("%S"),TEXT(""));
		}
	}
	if (style->Attribute("textbgcolour") != nullptr)
	{
		this->xdidEX(id,TEXT("-C"),TEXT("+k %S"),textbgcolour);
		if (0==lstrcmpA(type, "pbar"))
		{
			this->xdidEX(id,TEXT("-c"),TEXT("%S"),textbgcolour);
			this->xdidEX(id,TEXT("-U"),TEXT("%S"),TEXT(""));
		}
	}
	else if (style->Attribute("bgcolour") != nullptr)
		this->xdidEX(id,TEXT("-C"),TEXT("+k %S"),bgcolour); 
	if (style->Attribute("textcolour") != nullptr)
	{
		xdidEX(id,TEXT("-C"),TEXT("+t %S"),textcolour);
		if (0==lstrcmpA(type, "pbar"))
		{
			this->xdidEX(id,TEXT("-q"),TEXT("%S"),textcolour);
			this->xdidEX(id,TEXT("-U"),TEXT("%S"),TEXT(""));
		}
	}

	if (style->Attribute("gradientstart") != nullptr)
		this->xdidEX(id,TEXT("-C"),TEXT("+g %S"),gradientstart);
	if (style->Attribute("gradientend") != nullptr)
		this->xdidEX(id,TEXT("-C"),TEXT("+G %S"),gradientend);

	//cursor
	if (style->Attribute("cursor") != nullptr)
		this->xdidEX(id,TEXT("-J"),TEXT("+r %S"),cursor);

	//iconsize
	if (style->Attribute("iconsize") != nullptr)
	{
		if (((0==lstrcmpA(type, "toolbar")) || (0==lstrcmpA(type, "button"))) || (0==lstrcmpA(type, "treeview")))
			this->xdidEX(id,TEXT("-l"),TEXT("%S"),iconsize);
	}
	if (0==lstrcmpA(type, "button"))
	{
		if (element->Attribute("bgcolour") == nullptr)
			bgcolour = "65280";
		if (element->Attribute("src") != nullptr)
			this->xdidEX(id,TEXT("-k"),TEXT("+n %S %S"),bgcolour,src);
		if (element->Attribute("disabledsrc") != nullptr)
			this->xdidEX(id,TEXT("-k"),TEXT("+n %S %S"),bgcolour,disabledsrc);
		if (element->Attribute("hoversrc") != nullptr)
			this->xdidEX(id,TEXT("-k"),TEXT("+n %S %S"),bgcolour,hoversrc);
		if (element->Attribute("selectedsrc") != nullptr)
			this->xdidEX(id,TEXT("-k"),TEXT("+n %S %S"),bgcolour,hoversrc);
	}
}

/* parseStyle(recursionDepth) : Simple recursive method to cascade find the right style to apply to an element */
void DcxmlParser::parseStyle(int depth) { 
	if (depth > 2)
		return;
	depth++;
	const TiXmlElement* tiStyles = nullptr;
	const TiXmlElement* style = nullptr;
	const TiXmlElement* ClassElement = nullptr;
	const TiXmlElement* TypeElement = nullptr;
	const TiXmlElement* IdElement = nullptr;
	if (depth == 3)
		style = this->element;
	else if (depth == 1)
		tiStyles = this->getDialogsElement()->FirstChildElement("styles");
	else if (depth == 2)
		tiStyles = this->getDialogElement()->FirstChildElement("styles");

	if (tiStyles != nullptr) {
		style = tiStyles->FirstChildElement("all");
		if (style != nullptr)
			setStyle(style);
		for( style = tiStyles->FirstChildElement("style"); style != nullptr; style = style->NextSiblingElement()) {
			if (0==lstrcmpA(style->Attribute("class"), STclass))
				ClassElement = style;
			if (0==lstrcmpA(style->Attribute("type"), type))
				TypeElement = style;
			if (parseId(style) == id)
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
		this->setStyle(style);
	this->parseStyle(depth);
}

/* parseIcons(recursionDepth) : Simple recursive method to cascade find the right icons to apply to an element */
void DcxmlParser::parseIcons(int depth) { 
	if (depth > 1)
		return;
	depth++;
	const TiXmlElement* icons = nullptr;
	const TiXmlElement* ClassElement = nullptr;
	const TiXmlElement* TypeElement = nullptr;
	const TiXmlElement* IdElement = nullptr;
	if (depth == 1)
		icons = this->getDialogElement()->FirstChildElement("icons");
	else if (depth == 2)
		icons = this->getDialogsElement()->FirstChildElement("icons");

	if (icons != nullptr) {
		const TiXmlElement* tiIcon;

		for( tiIcon = icons->FirstChildElement("icon"); icon != nullptr; tiIcon = tiIcon->NextSiblingElement()) {
			if (0==lstrcmpA(tiIcon->Attribute("class"), STclass))
				ClassElement = tiIcon;
			if (0==lstrcmpA(tiIcon->Attribute("type"), type))
				TypeElement = tiIcon;
			const auto t_id = this->parseId(tiIcon);
			if (t_id == id)
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
					mIRCLinker::execex(TEXT("//var %%x = %i | while (%%x <= %i ) { xdid -w %s %i +%S %%x %S | inc %%x }"), indexmin,indexmax,this->getDialogMark().to_chr(),id,flags,tIconSrc);
				}
				else 
					mIRCLinker::execex(TEXT("//xdid -w %s %i +%S %S %S"),this->getDialogMark().to_chr(),id,flags,index,tIconSrc);
			}
			else
			{
				for (auto iconchild = tiIcon->FirstChildElement("icon"); iconchild != nullptr; iconchild = iconchild->NextSiblingElement()) {
					const auto tflags = queryAttribute(iconchild, "flags", "n");
					const auto tindex = queryAttribute(iconchild, "index", "0");
					const auto tsrc = iconchild->Attribute("src");
					if (tsrc != nullptr)
						mIRCLinker::execex(TEXT("//xdid -w %s %i +%S %S %S"),this->getDialogMark().to_chr(),id,tflags,tindex,tsrc);
				}
			}
		}
	}
	else
		parseIcons(depth);
}

/* parseItems(XmlElement,recursionDepth,itemPath) : recursively applies items for a control */
void DcxmlParser::parseItems(const TiXmlElement *const tiElement,const UINT depth, const char *itemPath) { 
	auto item = 0;
	auto cell = 0;

	for (auto child = tiElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
	{
		const auto childelem = child->Value();
		if (0==lstrcmpA(childelem, "columns"))
		{
			if (0==lstrcmpA(type, "listview"))
			{
				TString tsArguments;
				TString tsBuffer;
				for (auto column = child->FirstChildElement("column"); column != nullptr; column = column->NextSiblingElement("column"))
				{
					const auto local_tWidth = queryAttribute(column, "width", "0");
					const auto local_tCaption = queryAttribute(column, "caption", "");
					const auto local_tFlags = queryAttribute(column, "flags", "l");
					const auto local_tIcon = queryAttribute(column, "icon", "0");

#if TSTRING_TEMPLATES
					tsBuffer.tsprintf(TEXT("+%S %S %S %S "), local_tFlags, local_tIcon, local_tWidth, local_tCaption);
					tsArguments.addtok(tsBuffer, TEXT('\t'));
#else
					tsBuffer.tsprintf(TEXT("+%S %S %S %S "), local_tFlags, local_tIcon, local_tWidth, local_tCaption);
					tsArguments.addtok(tsBuffer.to_chr(), TEXT("\t"));
#endif
				}
				if (!tsArguments.empty())
					this->xdidEX(id,TEXT("-t"),TEXT("%s"),tsArguments.to_chr());
			}
			//if (0==lstrcmpA(childelem, "dataset"))
			//{
			//	//auto listView = this->d_Host->getControlByID(id);
			//}
		}


		if ((0==lstrcmpA(childelem, "item")) || (0==lstrcmpA(childelem, "control")))
			cell++;
		//fill all required parameters with attributes or default values
		this->parseAttributes(child);
		if (0==lstrcmpA(childelem, "item")) {
			item++;
			if (0==lstrcmpA(type, "toolbar"))
				this->xdidEX(id,TEXT("-a"),TEXT("0 +%S %S %S %S %S \t %S"), ((tFlags) ? tFlags : "a"),width,icon,textcolour,caption,tooltip);
			else if (0==lstrcmpA(type, "comboex"))
				this-> xdidEX(id,TEXT("-a"),TEXT("0 %S %S %S 0 %S"), indent,icon,icon,caption);
			else if (0==lstrcmpA(type, "list"))
				this->xdidEX(id,TEXT("-a"),TEXT("0 %S"), caption);
			else if (0==lstrcmpA(type, "statusbar"))
				this->xdidEX(id,TEXT("-t"),TEXT("%i +%S %S %S \t %S"), cell,((tFlags) ? tFlags : "f"),icon,caption,tooltip);
			else if (0==lstrcmpA(type, "treeview")) { 
				// Ook: recursive loop needs looked at, use a TString reference object instead of multiple char[]'s for itemPath
				//itemPath += item;
				//this->xdidEX(id, TEXT("-a"), TEXT("%s \t +%S %S %S 0 %S %S %S %S %S \t %S"), itemPath.to_chr(), ((tFlags) ? tFlags : "a"), icon, icon, state, integral, textcolour, bgcolour, caption, tooltip);
				//this->parseItems(child, depth, itemPath);
				char pathx[100];
				wnsprintfA(pathx, 100, "%s %i",itemPath,item);
				this->xdidEX(id,TEXT("-a"),TEXT("%S \t +%S %S %S 0 %S %S %S %S %S \t %S"), pathx,((tFlags) ? tFlags : "a"),icon,icon,state,integral,textcolour,bgcolour,caption,tooltip);
				this->parseItems(child,depth,pathx);
			}
		}
	}
}

/* parseTemplate(recursionDepth,claPath,firstFreeControlId) : finds a template and parses it into the current dialog */
void DcxmlParser::parseTemplate(const UINT dialogDepth,const char *const claPath,const UINT passedid)
{
	//const TiXmlElement *found = nullptr;
	const auto lookIn = this->getDialogsElement()->FirstChildElement("templates");

	if (lookIn == nullptr)
		return;

	for (auto Template = lookIn->FirstChildElement("template"); Template != nullptr; Template = Template->NextSiblingElement())
	{
		if (0==lstrcmpA(Template->Attribute("name"), element->Attribute("name")))
		{ 
			//found = Template;
			//break;
			element = Template;
			this->parseDialog(dialogDepth, claPath, passedid, 1);
			break;
		}
	}
	//if (found != nullptr)
	//{
	//	element = found;
	//	this->parseDialog(dialogDepth,claPath,passedid,1);
	//}
}

/* parseDialog(recursionDepth,claPath,firstFreeControlId,ignoreParentFlag) : finds a template and parses it into the current dialog */
void DcxmlParser::parseDialog(const UINT depth,const char *claPath,const UINT passedid,const bool ignoreParent) { 
	int control = 0, cCla = 0, cell = 0;
	g_claPath = nullptr;
	g_claPathx = nullptr;
	g_resetcla = 0;
	for( auto child = this->element->FirstChildElement(); child != nullptr; child = child->NextSiblingElement() ) {
		cell++;
		//STEP 1: SET ELEMENT AND PARENTELEMENT
		if (!ignoreParent)
			this->parent = child->Parent()->ToElement();
		this->element = child->ToElement();

		//STEP 2: PARSE ATTRIBUTES OF ELEMENTS
		this->parseAttributes();

		//dont itterate over unneccessary items
		if (0==lstrcmpA(elem, "calltemplate")) 
		{
			if (0!=lstrcmpA("template",element->Parent()->ToElement()->Value()))
			{
				cCla++;
				templateRef = element;
				templateRefcCla = cCla;
				char t_buffer [100];
				const char * t_claPathx = 0;
				wnsprintfA(t_buffer, 100, "%i",cCla);
				t_claPathx = t_buffer;
				const char *value;
				for (auto attribute = element->FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
				{
					const auto name = attribute->Name();
					value = attribute->Value();
					if (0==lstrcmpA(name, "name")) continue;
					this->template_vars[name] = value;
				}
				for (auto iter = this->template_vars.begin(); iter != this->template_vars.end(); ++iter) {
					mIRCLinker::execex(TEXT("//set %%%S %S"),iter->first,iter->second);
				}
				templateRefclaPath = t_claPathx;
				this->parseTemplate(depth,claPath,passedid);
				templateRef = 0;
				for (auto iter = this->template_vars.begin(); iter != this->template_vars.end(); ++iter) {
					mIRCLinker::execex(TEXT("//unset %%%S"),iter->first);
				}
				this->template_vars.clear();
			}
			continue;
		}
		if ((0==lstrcmpA(elem, "control")) || (0==lstrcmpA(elem, "pane"))) cCla++;
		else continue;

		//asign ID 
		if (0==lstrcmpA(elem, "control")) 
		{ 
			controls++;
			id = this->parseId(element);
			if (id <= 0)
				id = 2000 + controls;
			this->registerId(element,id); // Ook: does this twice??
			//this->registerId(element,id);
		}
		else
			id = passedid;

		//assign parent CONTROL of element
		while (parent != nullptr) {
			if (0==lstrcmpA(parentelem, "template")) 
			{
				parent = templateRef->Parent()->ToElement();
				parentelem = templateRef->Parent()->Value();
				cCla = templateRefcCla;
				claPath = templateRefclaPath;
			}
			if (0==lstrcmpA(parentelem, "pane")) 
			{ 
				parent = parent->Parent()->ToElement();
				parentelem = parent->Value();
			}
			else break;    
		}
		if (parent != nullptr)
			parenttype = parent->Attribute("type");
		else
			parenttype = nullptr;

		if (parenttype == nullptr)
			parenttype = "panel";
		parentid = this->parseId(parent);
		if (parentid <= 0)
			parentid = passedid;
		//IF TEMPLATE ELEMENT REROUTE TO TEMPLATE DEFINITION


		//STEP 3: IF CONTROL CREATE IT AND ITS ITEMS
		if (0==lstrcmpA(elem, "control")) {

			control++;

			//check how to insert the control in the parent Control/Dialog
			//If parentNode is pane loop untill parentNode is not a pane
			if (0==lstrcmpA(parentelem, "dialog"))
				xdialogEX(TEXT("-c"),TEXT("%i %S 0 0 %S %S %S"), id,type,width,(dropdown != nullptr ? dropdown : height),styles);
			else if (0==lstrcmpA(parentelem, "control")) {
				if (0==lstrcmpA(parenttype, "panel"))
					this->xdidEX(parentid,TEXT("-c"),TEXT("%i %S 0 0 %S %S %S"), id,type,width,(dropdown != nullptr ? dropdown : height),styles);
				else if (0==lstrcmpA(parenttype, "box"))
					this->xdidEX(parentid,TEXT("-c"),TEXT("%i %S 0 0 %S %S %S"), id,type,width,(dropdown != nullptr ? dropdown : height),styles);
				else if (0==lstrcmpA(parenttype, "tab"))
					this->xdidEX(parentid,TEXT("-a"),TEXT("0 %S %S \t %i %S 0 0 %S %S %S \t %S"), icon,caption,id,type,width,(dropdown != nullptr ? dropdown : height),styles,tooltip);
				else if (((0==lstrcmpA(parenttype, "pager")) || (0==lstrcmpA(parenttype, "box"))) && (control == 1))
					this->xdidEX(parentid,TEXT("-c"),TEXT("%i %S 0 0 %S %S %S"), id,type,width,(dropdown != nullptr ? dropdown : height),styles);
				else if (0==lstrcmpA(parenttype, "divider") && (control <= 2)) {
					if (control == 1)
						this->xdidEX(parentid,TEXT("-l"),TEXT("%S 0 \t %i %S 0 0 %S %S %S"), width,id,type,width,(dropdown != nullptr ? dropdown : height),styles);
					else if (control == 2)
						this->xdidEX(parentid,TEXT("-r"),TEXT("%S 0 \t %i %S 0 0 %S %S %S"), width,id,type,width,(dropdown != nullptr ? dropdown : height),styles);
				}
				else if (0==lstrcmpA(parenttype, "rebar")) { 
					const char *flags = (tFlags) ? tFlags : "ceg";
					this->xdidEX(parentid,TEXT("-a"),TEXT("0 +%S %S %S %S %S %S %S \t %i %S 0 0 %S %S %S \t %S"), flags,rebarMinWidth,rebarMinHeight,width,icon,textcolour,caption,id,type,width,(dropdown != nullptr ? dropdown : height),styles,tooltip);
				}
				else if (0==lstrcmpA(parenttype, "stacker")) 
					this->xdidEX(parentid,TEXT("-a"),TEXT("0 + %S %S %S \t %i %S 0 0 %S %S %S"), textcolour,bgcolour,caption,id,type,width,(dropdown != nullptr ? dropdown : height),styles);
				else if (0==lstrcmpA(parenttype, "statusbar"))
					this->xdidEX(parentid,TEXT("-t"),TEXT("%i +c %S %i %S 0 0 0 0 %S"), cell,icon,id,type,styles);
			}
		}
		//Set CLA for control or pane
		g_claPath = claPath;
		const auto claPathx(this->parseCLA(cCla));

		//Perform some control specific commands
		if (0==lstrcmpA(elem, "control")) {
			this->parseControl();
			this->parseIcons();	
			this->parseStyle();

		}
		//char *claPathx = "root";
		//mIRCLinker::execex(TEXT("//echo -a clapath:%s"),claPathx);
		this->parseDialog(depth+1,claPathx.c_str(),id);
	}
} 

// NB: never returns a ZERO, other code relies on this.
int DcxmlParser::mIRCEvalToUnsignedInt (const char *const value)
{
	//Todo: method returns -1 for failure which odd for a *ToUnsignedInt method.
	const TString buf(value);
	__int64 iNum;
	mIRCLinker::iEval(&iNum, buf.to_chr());
	return (int)((iNum > 0) ? iNum : -1);
}

void DcxmlParser::registerId(const TiXmlElement *const idElement,const int iNewID)
{
	int elementId;
	if (idElement->QueryIntAttribute("id",&elementId) != TIXML_SUCCESS) //<! id attr. is not an int
	{
		const auto elementNamedId = idElement->Attribute("id");
		if (elementNamedId != nullptr) {
			if (this->mIRCEvalToUnsignedInt(elementNamedId) < 0) //<! id attr. doesn't evaluate to an int
			{
				this->getDialog()->namedIds[elementNamedId] = iNewID;
			}
		}
	}
}

int DcxmlParser::parseId(const TiXmlElement *const idElement)
{
	if (idElement == nullptr)
		return 0;

	//<! if id attribute is already integer return it
	int local_id = 0;
	if (idElement->QueryIntAttribute("id", &local_id) == TIXML_SUCCESS)
	{
		// found ID as a number,  if its not a negative, return it.
		return max(local_id, 0);
	}

	const auto attributeIdValue = idElement->Attribute("id");
	if (attributeIdValue != nullptr)
	{
		// got ID attrib, evaluate it to try & resolve to a number.
		local_id = mIRCEvalToUnsignedInt(attributeIdValue);
		// if ID is > zero return it.
		if (local_id > 0)
			return local_id;

		//Otherwise if it's a namedId return it .find(attributeIdValue) never returned :(;

		for (const auto &x : this->getDialog()->getNamedIds())
		{
			if (x.first == attributeIdValue)
				return x.second;
		}
	}

	//const TString attributeIdValue(idElement->Attribute("id"));
	//if (!attributeIdValue.empty())
	//{
	//	// got ID attrib, evaluate it to try & resolve to a number.
	//	local_id = mIRCEvalToUnsignedInt(attributeIdValue.c_str());
	//	// if ID is > zero return it.
	//	if (local_id > 0)
	//		return local_id;

	//	auto it = this->getDialog()->getNamedIds().find(attributeIdValue);
	//	if (it != this->getDialog()->getNamedIds().end())
	//		return it->second;
	//}
	return 0;
}

