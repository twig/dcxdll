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
	loadSuccess(false)
{

}
DcxmlParser::~DcxmlParser() {

}

bool DcxmlParser::ParseXML(const char *file,const char *mark,const char *dialogName, const bool verbose, const bool autoClose)
{
	this->loadSuccess = true;
	this->isVerbose(verbose);
	this->isAutoClose(autoClose);

	this->setFilePath(file);

	this->setDialogMark(mark);
	this->setDialogName(dialogName);

	if (!this->loadDialog() || !this->loadDocument() || !this->loadDialogElement())
	{
		this->loadSuccess = false;
		if ((this->getDialog() != NULL) && this->isAutoClose())
			Dcx::mIRC.execex("/xdialog -x %s",this->getDialogName());

		return false;
	}

	Dcx::mIRC.execex("/dialog -s %s %i %i %i %i", //!< Sets the dialog size.
		this->getDialogMark(),
		this->queryIntAttribute(this->getDialogElement(),"x",-1),
		this->queryIntAttribute(this->getDialogElement(),"y",-1),
		this->queryIntAttribute(this->getDialogElement(),"w",-1),
		this->queryIntAttribute(this->getDialogElement(),"h",-1)
		);

	this->setZlayered(false);

	const char *cascade = this->queryAttribute(this->getDialogElement(),"cascade","v");
	const char *margin = this->queryAttribute(this->getDialogElement(),"margin","0 0 0 0");
	const char *caption = this->queryAttribute(this->getDialogElement(),"caption",this->getDialogName());
	const char *border = this->getDialogElement()->Attribute("border");
	this->controls = 0;
	//margin = (temp = element->Attribute("padding")) ? temp : "0 0 0 0";

	if (this->getDialogElement()->Attribute("border") != NULL) //!< set border ONLY if defined on <dialog>
		this->xdialogEX("-b","+%s",border);

	Dcx::mIRC.execex("//dialog -t %s %s", this->getDialogMark(), caption); 
	this->xdialogEX("-l","root \t +p%s 0 0 0 0", cascade);
	this->xdialogEX("-l","space root \t + %s", margin);

	this->parseDialog(); //!< Parse <dialog> children onto the dialog

	if (this->getDialogElement()->Attribute("center") != NULL) //!< Centers the dialog
		Dcx::mIRC.execex("/dialog -r %s", this->getDialogMark());

	//This "Shite" is to activate the first zlayer, added a check if this command starts returning an error
	if (this->getZlayered())
		this->xdialogEX("-z","+s 1");

	Dcx::mIRC.execex("/.timer 1 0 %s %s ready", this->getDialog()->getAliasName().to_chr(), this->getDialogMark()); //!< Tell user that dcxml is finished, & they can do a cla update or whatever.
	return this->loadSuccess;
}

void DcxmlParser::setDialog(const char *dialogMark) { this->_dcxDialog = Dcx::Dialogs.getDialogByName(dialogMark);	}

bool DcxmlParser::loadDocument()
{
	if (!Dcx::isFile(this->getFilePath()))
	{
		Dcx::errorex("/dcxml", "File \"%s\" does not exist or is in use by another process ", this->getFilePath());
		return false;
	}
	TiXmlBase::SetCondenseWhiteSpace(false); 
	TiXmlDocument doc(this->getFilePath());
	this->loadSuccess = doc.LoadFile();
	if (!this->loadSuccess) { 
		Dcx::errorex("/dcxml", "XML error in \"%s\" (row %i column %i) %s",
			this->getFilePath(),
			this->getDocument()->ErrorRow(),
			this->getDocument()->ErrorCol(),
			this->getDocument()->ErrorDesc());
		return false;
	}
	this->_document = doc; 
	return true;
}

bool DcxmlParser::loadDialog()
{
	this->setDialog(this->getDialogMark());
	if (this->getDialog() == NULL)
	{
		Dcx::errorex("/dcxml", "No such dialog (\"%s\") has been marked with dcx.",this->getDialogMark());
		return false;
	}
	return true;
}
bool DcxmlParser::loadDialogElement()
{
	this->setRootElement(this->getDocument()->FirstChildElement("dcxml"));
	if (this->getRootElement() == NULL)
	{
		Dcx::errorex("/dcxml", "The document element should be <dcxml> in \"%s\"", this->getFilePath());
		return false;
	}
	this->setDialogsElement(this->getRootElement()->FirstChildElement("dialogs"));
	if (this->getDialogsElement() == NULL) 
	{ 
		Dcx::errorex("/dcxml", "Theres no <dialogs> child for <dcxml> in \"%s\"", this->getFilePath());
		return false;
	}
	/*
	* This finds the Dialog element. 
	* Important Note:
	* The for loop has an ugly side assigment: it sets this->element 
	* which is used as a recursion cursor for this->parseDialog()
	*/
	for(this->element = this->getDialogsElement()->FirstChildElement("dialog"); this->element != NULL; this->element = this->element->NextSiblingElement("dialog"))
	{
		if (0==lstrcmp(this->element->Attribute("name"), this->getDialogName()))
		{
			this->setDialogElement(this->element);
			return true;
		}
	}
	Dcx::errorex("/dcxml", "Theres no <dialog> element with attribute name=\"%s\" in \"%s\"", this->getDialogName(), this->getFilePath());
	return false;
}


const char *DcxmlParser::queryAttribute(const TiXmlElement *element,const char *attribute,const char *defaultValue) const
{
	const char *t = element->Attribute(attribute);
	return (t != NULL) ? t : defaultValue;
}
int DcxmlParser::queryIntAttribute(const TiXmlElement *element,const char *attribute,const int defaultValue) const
{
	int integer = defaultValue;
	return (element->QueryIntAttribute(attribute,&integer) == TIXML_SUCCESS) ? integer : defaultValue;
}

void DcxmlParser::parseAttributes() {
	elem = element->Value();
	parentelem = parent->Value();
	parenttype = queryAttribute(parent,"type","panel");
	type = queryAttribute(element,"type","panel");
	STclass = queryAttribute(element,"class","");
	weigth = queryAttribute(element,"weight","1");
	height = queryAttribute(element, "height", "0");
	width = queryAttribute(element, "width", "0");
	margin = queryAttribute(element,"margin", "0 0 0 0");
	styles = queryAttribute(element,"styles", "");
	caption = element->Attribute("caption");
	if (caption == NULL) {
		temp = element->GetText();
		caption = (temp != NULL) ? temp : "";
	}
	tooltip = queryAttribute(element, "tooltip", "");
	cascade = queryAttribute(element, "cascade", "");
	icon = queryAttribute(element, "icon", "0");
	integral = queryAttribute(element, "integral", "0");
	state = queryAttribute(element, "state", "0");
	indent = queryAttribute(element, "indent", "0");
	//flags attribute defaults different per type/item
	tFlags = element->Attribute("flags");
	src = queryAttribute(element, "src", "");
	cells = queryAttribute(element, "cells", "-1");
	rebarMinHeight = queryAttribute(element, "minheight", "0");
	rebarMinWidth = queryAttribute(element, "minwidth", "0");
	iconsize = queryAttribute(element, "iconsize", "16");
	eval = queryIntAttribute(element, "eval", 0);

	fontstyle = queryAttribute(element, "fontstyle", "d");
	charset = queryAttribute(element, "charset", "ansi");
	fontsize = queryAttribute(element, "fontsize","");
	fontname = queryAttribute(element, "fontname", "");
	border = queryAttribute(element, "border", "");
	cursor = queryAttribute(element, "cursor", "arrow");
	bgcolour = queryAttribute(element, "bgcolour", "0");
	textbgcolour = queryAttribute(element, "textbgcolour", "");
	textcolour = queryAttribute(element, "textcolour", "0");

	gradientstart = queryAttribute(element, "gradientstart", "");
	gradientend = queryAttribute(element, "gradientend", "");

	disabledsrc = queryAttribute(element, "disabledsrc", "");
	hoversrc = queryAttribute(element, "hoversrc", "");
	selectedsrc = queryAttribute(element, "selectedsrc", "");
}
void DcxmlParser::parseAttributes(const TiXmlElement* element) {
	elem = element->Value();
	parentelem = parent->Value();
	parenttype = queryAttribute(parent, "type", "panel");
	temp = element->Attribute("weight");
	weigth = (temp != NULL) ? temp : "1";
	temp = element->Attribute("height");
	height = (temp != NULL) ? temp : "0";
	temp = element->Attribute("width");
	width = (temp != NULL) ? temp : "0";
	temp = element->Attribute("margin");
	margin = (temp != NULL) ? temp : "0 0 0 0";
	temp = element->Attribute("styles");
	styles = (temp != NULL) ? temp : "";
	temp = element->Attribute("caption");
	if (temp == NULL)
		temp = element->GetText();
	caption = (temp != NULL) ? temp : "";
	temp = element->Attribute("tooltip");
	tooltip = (temp != NULL) ? temp : "";
	temp = element->Attribute("cascade");
	cascade = (temp != NULL) ? temp : "";
	temp = element->Attribute("icon");
	icon = (temp != NULL) ? temp : "0";
	temp = element->Attribute("integral");
	integral = (temp != NULL) ? temp : "0";
	temp = element->Attribute("state");
	state = (temp != NULL) ? temp : "0";
	temp = element->Attribute("indent");
	indent = (temp != NULL) ? temp : "0";
	//flags attribute defaults different per type/item
	tFlags = element->Attribute("flags");
	temp = element->Attribute("src");
	src = (temp != NULL) ? temp : "";
	temp = element->Attribute("cells");
	cells = (temp != NULL) ? temp : "-1";
	temp = element->Attribute("minheight");
	rebarMinHeight = (temp != NULL) ? temp : "0";
	temp = element->Attribute("minwidth");
	rebarMinWidth = (temp != NULL) ? temp : "0";
	temp = element->Attribute("iconsize");
	iconsize = (temp != NULL) ? temp : "16";
	eval = (element->QueryIntAttribute("eval",&eval) == TIXML_SUCCESS) ? eval : 0;

	temp = element->Attribute("fontstyle");
	fontstyle = (temp != NULL) ? temp : "d";
	temp = element->Attribute("charset");
	charset = (temp != NULL) ? temp : "ansi";
	temp = element->Attribute("fontsize");
	fontsize = (temp != NULL) ? temp : "";
	temp = element->Attribute("fontname");
	fontname = (temp != NULL) ? temp : "";
	temp = element->Attribute("border");
	border = (temp != NULL) ? temp : "";
	temp = element->Attribute("cursor");
	cursor = (temp != NULL) ? temp : "arrow";
	temp = element->Attribute("bgcolour");
	bgcolour = (temp != NULL) ? temp : "0";
	temp = element->Attribute("textbgcolour");
	textbgcolour = (temp != NULL) ? temp : "";
	temp = element->Attribute("textcolour");
	textcolour = (temp != NULL) ? temp : "0";

	temp = element->Attribute("gradientstart");
	gradientstart = (temp != NULL) ? temp : "";
	temp = element->Attribute("gradientend");
	gradientend = (temp != NULL) ? temp : "";

	temp = element->Attribute("disabledsrc");
	disabledsrc = (temp != NULL) ? temp : "";
	temp = element->Attribute("hoversrc");
	hoversrc = (temp != NULL) ? temp : "";
	temp = element->Attribute("selectedsrc");
	selectedsrc = (temp != NULL) ? temp : "";
}
/* parseControl() : if current element is a control perform some extra commands*/
void DcxmlParser::parseControl() { 
	//DcxControl *control = this->getDialog()->getControlByID(id + mIRC_ID_OFFSET);
	if (element->Attribute("zlayer") != NULL) { 
		this->xdialogEX("-z","+a %i",id);
		this->setZlayered(true);
	}
	//        padding = (temp = element->Attribute("padding")) ? temp : "0 0 0 0";

	if (0==lstrcmp(parenttype, "divider")) {
		temp = element->Attribute("width");
		xdidEX(parentid,"-v","%s",((temp != NULL) ? temp : "100"));
	}
	if (((0==lstrcmp(type, "toolbar")) || (0==lstrcmp(type, "button"))) || (0==lstrcmp(type, "treeview")))
			xdidEX(id,"-l","%s",iconsize);
	if ((0==lstrcmp(type, "toolbar")) || (0==lstrcmp(type, "treeview")) || (0==lstrcmp(type, "comboex")) || (0==lstrcmp(type, "list")) || (0==lstrcmp(type, "listview")))
		this->parseItems(element);

	if (((((0==lstrcmp(type, "box")) || (0==lstrcmp(type, "check"))) || (0==lstrcmp(type, "link"))) || (0==lstrcmp(type, "radio"))) || (0==lstrcmp(type, "button")))
	{
		if (caption != NULL) xdidEX(id,"-t","%s",caption);
	}
	if ((0==lstrcmp(type, "ipaddress")) && (caption != NULL))
		this->xdidEX(id,"-a","%s",caption);
	if ((0==lstrcmp(type, "webctrl")) && (src != NULL))
		this->xdidEX(id,"-n","%s",src);
	else if (0==lstrcmp(type, "text")) { 
		if (caption != NULL) { 
			TString mystring(caption);
			if (mystring.left(2) == "\r\n") mystring = mystring.right(-2);
			else if (mystring.left(1) == "\n") mystring = mystring.right(-1);
			//mystring.replace("\t","");
			mystring -= "\t";
			TString printstring;
			int textspace = 0;
			while(mystring.gettok(1) != "") { 
				printstring.addtok(mystring.gettok(1).to_chr());
				if (printstring.len() > (MIRC_BUFFER_SIZE_CCH -100)) { 
					this->xdidEX(id,"-a","%i %s",textspace,printstring.gettok(1,-1).to_chr());
					printstring = "";
					textspace = 1;
				}
				mystring.deltok(1);
			}
			if (printstring != "") { 
				this->xdidEX(id,"-a","%i %s",textspace,printstring.gettok(1,-1).to_chr());
			}
		}
	}
	else if (0==lstrcmp(type, "edit")) { 
		if (caption != NULL) { 
			TString mystring(caption);
			if (mystring.left(2) == "\r\n")
				mystring = mystring.right(-2);
			else if (mystring.left(1) == "\n")
				mystring = mystring.right(-1);
			//mystring.replace("\t","");
			mystring -= "\t";
			int line = 0;
			while(mystring.gettok(1,"\r\n") != "") { 
				line++;
				this->xdidEX(id,"-i","%i %s",line,mystring.gettok(1,"\r\n").to_chr());
				mystring.deltok(1,"\r\n");
			}
		}
	}
	else if (0==lstrcmp(type, "richedit")) { 
		if (caption != NULL) {
			TString mystring(caption);
			if (mystring.left(2) == "\r\n") mystring = mystring.right(-2);
			else if (mystring.left(1) == "\n") mystring = mystring.right(-1);
			//mystring.replace("\t","");
			mystring -= "\t";
			mystring.replace("\\c","");
			mystring.replace("\\b","");
			mystring.replace("\\r","");
			int line = 0;
			while(mystring.gettok(1,"\r\n") != "") { 
				line++;
				this->xdidEX(id,"-i","%i %s",line,mystring.gettok(1,"\r\n").to_chr());
				mystring.deltok(1,"\r\n");
			}
		}
	}
	else if (0==lstrcmp(type, "pbar")) { 
		if (caption != NULL)
			this->xdidEX(id,"-i","%s",caption);
	}
	else if (0==lstrcmp(type, "image"))
		this->xdidEX(id,"-i","+%s %s",((tFlags) ? tFlags : ""),src);
	else if (0==lstrcmp(type, "statusbar")) { 
		this->xdidEX(id,"-l","%s",cells);
		this->parseItems(element);
	}
	temp = element->Attribute("disabledsrc");
	disabledsrc = (temp != NULL) ? temp : "";
	temp = element->Attribute("hoversrc");
	hoversrc = (temp != NULL) ? temp : "";
}
/* xdialogEX(switch,format[,args[]]) : performs an xdialog command internally or through mIRC */
void DcxmlParser::xdialogEX(const char *sw,const char *dFormat, ...) {
	va_list args;
	TString txt;

	va_start(args, dFormat);
	txt.tvprintf(dFormat, &args);
	va_end(args);

	if (this->isVerbose())
		Dcx::mIRC.execex("/echo -a dcxml debug: /xdialog %s %s %s",sw,this->getDialogMark(),txt.to_chr());

	if (eval > 0) Dcx::mIRC.execex("//xdialog %s %s %s",sw,this->getDialogMark(),txt.to_chr());
	else this->getDialog()->parseCommandRequestEX("%s %s %s",this->getDialogMark(),sw,txt.to_chr());
}
/* xdidEX(controlId,switch,format[,args[]]) : performs an xdid command internally or through mIRC on the specified id */
void DcxmlParser::xdidEX(const int id,const char *sw,const char *dFormat, ...) {
	va_list args;
	TString txt;

	va_start(args, dFormat);
	txt.tvprintf(dFormat, &args);
	va_end(args);

	if (this->isVerbose())
		Dcx::mIRC.execex("/echo -a dcxml debug: /xdid %s %s %i %s",sw,this->getDialogMark(),id,txt.to_chr());

	if (eval > 0) Dcx::mIRC.execex("//xdid %s %s %i %s",sw,this->getDialogMark(),id,txt.to_chr());
	else this->getDialog()->parseComControlRequestEX(id,"%s %i %s %s",this->getDialogMark(),id,sw,txt.to_chr());
}
/* parseCLA(int numberOfClaControlsInCurrentBranch) : parses control and pane elements and applies the right CLA commands */
TString DcxmlParser::parseCLA(const int cCla) { 
	if (0==lstrcmp(elem, "control")) { 
		if ((0==lstrcmp(type, "panel")) || (0==lstrcmp(type, "box"))) {
			this->xdidEX(id,"-l","root \t +p%s 0 0 0 0",cascade);
			this->xdidEX(id,"-l","space root \t + %s",margin);
			g_resetcla = 1;
		}
		const char * fHeigth = "";
		const char * fWidth = "";
		const char * fixed = "l";
		if (element->Attribute("height") != NULL) { fHeigth = "v"; fixed = "f"; weigth = "0"; }
		if (element->Attribute("width") != NULL) { fWidth = "h"; fixed = "f"; weigth = "0"; }
		if (0==lstrcmp(parentelem, "dialog"))
			this->xdialogEX("-l","cell %s \t +%s%s%si %i %s %s %s",	g_claPath,fixed,fHeigth,fWidth,id,weigth,width,height);
		else if (0==lstrcmp(parentelem, "control")) {
			const char *t_type = parent->Attribute("type");
			if ((t_type != NULL) && (parentid > 0)) {
				if (0==lstrcmp(t_type, "panel"))
					this->xdidEX(parentid,"-l","cell %s \t +%s%s%si %i %s %s %s", g_claPath,fixed,fHeigth,fWidth,id,weigth,width,height); 
				else if (0==lstrcmp(t_type, "box"))
					this->xdidEX(parentid,"-l","cell %s \t +%s%s%si %i %s %s %s", g_claPath,fixed,fHeigth,fWidth,id,weigth,width,height); 
			}
		}
	}
	else if (0==lstrcmp(elem, "pane")) {
		if (0==lstrcmp(parentelem, "dialog"))
			this->xdialogEX("-l","cell %s \t +p%s 0 %s 0 0",g_claPath,cascade,weigth);
		else if (0==lstrcmp(parentelem, "control")) {
			if ((parenttype != NULL) && (parentid > 0)) {
				if (0==lstrcmp(parenttype, "panel"))
					this->xdidEX(parentid,"-l","cell %s \t +p%s 0 %s 0 0",g_claPath,cascade,weigth);
				else if (0==lstrcmp(parenttype, "box"))
					this->xdidEX(parentid,"-l","cell %s \t +p%s 0 %s 0 0",g_claPath,cascade,weigth);
			}
		}
	}
	TString claPathx;
	if (g_resetcla > 0)
		claPathx = "root";
	else if (0==lstrcmp(g_claPath, "root"))
		claPathx.tsprintf("%i",cCla);
	else
		claPathx.tsprintf("%s %i",g_claPath,cCla);

	if (element->Attribute("margin") != NULL) {
		if (0==lstrcmp(parentelem, "dialog"))
			this->xdialogEX("-l","space %s \t + %s",claPathx.to_chr(),margin);
		else
			this->xdidEX(parentid,"-l","space %s \t + %s",g_claPath,margin);
	}
	g_resetcla = 0;
	return claPathx;
}
/* setStyle(TiXmlElement*) : Applies the styles described on the element found by parseStyle() */
void DcxmlParser::setStyle(const TiXmlElement* style) {
	//style attributes evaluate by default unless eval="0" is set on the element explicitly

	eval = (style->QueryIntAttribute("eval",&eval) == TIXML_SUCCESS) ? eval : 1;

	//font
	temp = style->Attribute("fontstyle");
	fontstyle = (temp != NULL) ? temp : "d";
	temp = style->Attribute("charset");
	charset = (temp != NULL) ? temp : "ansi";
	temp = style->Attribute("fontsize");
	fontsize = (temp != NULL) ? temp : "";
	temp = style->Attribute("fontname");
	fontname = (temp != NULL) ? temp : "";
	if ((style->Attribute("fontsize") != NULL) || (style->Attribute("fontname") != NULL))
		this->xdidEX(id,"-f","+%s %s %s %s", fontstyle, charset, fontsize, fontname);
	//border
	temp = style->Attribute("border");
	border = (temp != NULL) ? temp : "";
	if (temp != NULL)
		this->xdidEX(id,"-x","+%s",border);
	//colours
	temp = style->Attribute("cursor");
	cursor = (temp != NULL) ? temp : "arrow";
	temp = style->Attribute("bgcolour");
	bgcolour = (temp != NULL) ? temp : "";
	temp = style->Attribute("textbgcolour");
	textbgcolour = (temp != NULL) ? temp : "";
	temp = style->Attribute("textcolour");
	textcolour = (temp != NULL) ? temp : "";
	if (style->Attribute("bgcolour") != NULL) { 
		this->xdidEX(id,"-C","+b %s",bgcolour);
		if (0==lstrcmp(type, "pbar")) 
		{
			this->xdidEX(id,"-k","%s",bgcolour);
			this->xdidEX(id,"-U","%s","");
		}
	}
	if (style->Attribute("textbgcolour") != NULL) 
	{
		this->xdidEX(id,"-C","+k %s",textbgcolour);
		if (0==lstrcmp(type, "pbar")) 
		{
			this->xdidEX(id,"-c","%s",textbgcolour);
			this->xdidEX(id,"-U","%s","");
		}
	}
	else if (style->Attribute("bgcolour") != NULL)
		this->xdidEX(id,"-C","+k %s",bgcolour); 
	if (style->Attribute("textcolour") != NULL) 
	{
		xdidEX(id,"-C","+t %s",textcolour);
		if (0==lstrcmp(type, "pbar")) 
		{
			this->xdidEX(id,"-q","%s",textcolour);
			this->xdidEX(id,"-U","%s","");
		}
	}

	if (style->Attribute("gradientstart") != NULL)
		this->xdidEX(id,"-C","+g %s",gradientstart);
	if (style->Attribute("gradientend") != NULL)
		this->xdidEX(id,"-C","+G %s",gradientend);

	//cursor
	if (style->Attribute("cursor") != NULL)
		this->xdidEX(id,"-J","+r %s",cursor);    

	//iconsize
	if (style->Attribute("iconsize") != NULL) 
	{
		if (((0==lstrcmp(type, "toolbar")) || (0==lstrcmp(type, "button"))) || (0==lstrcmp(type, "treeview"))) 
			this->xdidEX(id,"-l","%s",iconsize);
	}
	if (0==lstrcmp(type, "button"))
	{
		if (element->Attribute("bgcolour") == NULL)
			bgcolour = "65280";
		if (element->Attribute("src") != NULL)
			this->xdidEX(id,"-k","+n %s %s",bgcolour,src);
		if (element->Attribute("disabledsrc") != NULL)
			this->xdidEX(id,"-k","+n %s %s",bgcolour,disabledsrc);
		if (element->Attribute("hoversrc") != NULL)
			this->xdidEX(id,"-k","+n %s %s",bgcolour,hoversrc);
		if (element->Attribute("selectedsrc") != NULL)
			this->xdidEX(id,"-k","+n %s %s",bgcolour,hoversrc);
	}
}

/* parseStyle(recursionDepth) : Simple recursive method to cascade find the right style to apply to an element */
void DcxmlParser::parseStyle(int depth) { 
	if (depth > 2)
		return;
	depth++;
	const TiXmlElement* styles = NULL;
	const TiXmlElement* style = NULL;
	const TiXmlElement* ClassElement = 0;
	const TiXmlElement* TypeElement = 0;
	const TiXmlElement* IdElement = 0;
	if (depth == 3)
		style = this->element;
	else if (depth == 1)
		styles = this->getDialogsElement()->FirstChildElement("styles");
	else if (depth == 2)
		styles = this->getDialogElement()->FirstChildElement("styles");
	if (styles != NULL) {
		style = styles->FirstChildElement("all");
		if (style != NULL)
			setStyle(style);
		for( style = styles->FirstChildElement("style"); style != NULL; style = style->NextSiblingElement()) {
			if (0==lstrcmp(style->Attribute("class"), STclass))
				ClassElement = style;
			if (0==lstrcmp(style->Attribute("type"), type))
				TypeElement = style;
			if (parseId(style) == id)
				IdElement = style;
		}
		if (IdElement != NULL)
			style = IdElement;
		else if (ClassElement != NULL)
			style = ClassElement;
		else if (TypeElement != NULL)
			style = TypeElement;
	}
	if (style != NULL)
		this->setStyle(style);
	this->parseStyle(depth);
}
/* parseIcons(recursionDepth) : Simple recursive method to cascade find the right icons to apply to an element */
void DcxmlParser::parseIcons(int depth) { 
	if (depth > 1)
		return;
	depth++;
	const TiXmlElement* icons = 0;
	const TiXmlElement* ClassElement = 0;
	const TiXmlElement* TypeElement = 0;
	const TiXmlElement* IdElement = 0;
	if (depth == 1)
		icons = this->getDialogElement()->FirstChildElement("icons");
	else if (depth == 2)
		icons = this->getDialogsElement()->FirstChildElement("icons");
	if (icons != NULL) {
		const TiXmlElement* icon;
		for( icon = icons->FirstChildElement("icon"); icon != NULL; icon = icon->NextSiblingElement()) {
			if (0==lstrcmp(icon->Attribute("class"), STclass)) ClassElement = icon;
			if (0==lstrcmp(icon->Attribute("type"), type)) TypeElement = icon;
			int t_id = this->parseId(icon);
			if (t_id == id) IdElement = icon;
		}
		if (IdElement != NULL)
			icon = IdElement;
		else if (ClassElement != NULL)
			icon = ClassElement;
		else if (TypeElement != NULL)
			icon = TypeElement;
		else
			icon = NULL;

		if (icon != NULL) {
			temp = icon->Attribute("flags");
			const char *flags = (temp != NULL) ? temp : "ndhs";
			temp = icon->Attribute("index");
			const char *index = (temp != NULL) ? temp : "0";
			const char *src = icon->Attribute("src");
			int indexmin = (icon->QueryIntAttribute("indexmin",&indexmin) == TIXML_SUCCESS) ? indexmin : 0;
			int indexmax = (icon->QueryIntAttribute("indexmax",&indexmax) == TIXML_SUCCESS) ? indexmax : -1;
			if (src != NULL) {
				if (indexmin <= indexmax) 
					//method sucks but looping in C++ is WAYYY too fast for mIRC
				{
					Dcx::mIRC.execex("//var %%x = %i | while (%%x <= %i ) { xdid -w %s %i +%s %%x %s | inc %%x }", indexmin,indexmax,this->getDialogMark(),id,flags,src);
				}
				else 
					Dcx::mIRC.execex("//xdid -w %s %i +%s %s %s",this->getDialogMark(),id,flags,index,src);
			}
			else
			{
				for(const TiXmlElement *iconchild = icon->FirstChildElement("icon"); iconchild != NULL; iconchild = iconchild->NextSiblingElement()) {
					temp = iconchild->Attribute("flags");
					const char *tflags = (temp != NULL) ? temp : "ndhs";
					temp = iconchild->Attribute("index");
					const char *tindex = (temp != NULL) ? temp : "0";
					const char *tsrc = iconchild->Attribute("src");
					if (tsrc != NULL)
						Dcx::mIRC.execex("//xdid -w %s %i +%s %s %s",this->getDialogMark(),id,tflags,tindex,tsrc);
				}
			}
		}
	}
	else
		parseIcons(depth);
}
/* parseItems(XmlElement,recursionDepth,itemPath) : recursively applies items for a control */
void DcxmlParser::parseItems(const TiXmlElement* element,int depth,char *itemPath) { 
	int item = 0;
	int cell = 0;

	for(const TiXmlElement *child = element->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		const char *childelem = child->Value();
		if (0==lstrcmp(childelem, "columns"))
		{
			if (0==lstrcmp(type, "listview"))
			{
				TString arguments;
				TString buffer;
				for(const TiXmlElement *column = child->FirstChildElement("column"); column != NULL; column = column->NextSiblingElement("column") ) 
				{
					const char *width = column->Attribute("width");
					const char *caption = column->Attribute("caption");
					const char *flags = column->Attribute("flags");
					const char *icon = column->Attribute("icon");
					if (width == NULL)
						width = "0";
					if (caption == NULL)
						caption = "";
					if (flags == NULL)
						flags = "l";
					if (icon == NULL)
						icon = "0";

					buffer.tsprintf("+%s %s %s %s ",flags,icon,width,caption);
					arguments.addtok(buffer.to_chr(),"\t");
				}
				if (arguments.numtok() > 0)
					this->xdidEX(id,"-t","%s",arguments.to_chr());
			}
			if (0==lstrcmp(childelem, "dataset"))
			{
				//DcxListView *listView = this->d_Host->getControlByID(id);
			}
		}


		if ((0==lstrcmp(childelem, "item")) || (0==lstrcmp(childelem, "control")))
			cell++;
		//fill all required parameters with attributes or default values
		this->parseAttributes(child);
		if (0==lstrcmp(childelem, "item")) { 
			item++;            
			if (0==lstrcmp(type, "toolbar"))
				this->xdidEX(id,"-a","0 +%s %s %s %s %s \t %s", ((tFlags) ? tFlags : "a"),width,icon,textcolour,caption,tooltip);
			else if (0==lstrcmp(type, "comboex"))
				this-> xdidEX(id,"-a","0 %s %s %s 0 %s", indent,icon,icon,caption);
			else if (0==lstrcmp(type, "list"))
				this->xdidEX(id,"-a","0 %s", caption);
			else if (0==lstrcmp(type, "statusbar"))
				this->xdidEX(id,"-t","%i +%s %s %s \t %s", cell,((tFlags) ? tFlags : "f"),icon,caption,tooltip);
			else if (0==lstrcmp(type, "treeview")) { 
				char pathx[100];
				wnsprintf(pathx, 100, "%s %i",itemPath,item);
				this->xdidEX(id,"-a","%s \t +%s %s %s 0 %s %s %s %s %s \t %s", pathx,((tFlags) ? tFlags : "a"),icon,icon,state,integral,textcolour,bgcolour,caption,tooltip);
				this->parseItems(child,depth,pathx);
			}
		}
	}
}

/* parseTemplate(recursionDepth,claPath,firstFreeControlId) : finds a template and parses it into the current dialog */
void DcxmlParser::parseTemplate(int dialogDepth,const char *claPath,const int passedid)
{
	const TiXmlElement *found = NULL;
	const TiXmlElement *lookIn = this->getDialogsElement()->FirstChildElement("templates");

	if (lookIn == NULL)
		return;

	for (const TiXmlElement *Template = lookIn->FirstChildElement("template"); Template != NULL; Template = Template->NextSiblingElement()) 
	{
		if (0==lstrcmp(Template->Attribute("name"), element->Attribute("name")))
		{ 
			found = Template;
			break;
		}
	}
	if (found != NULL) 
	{
		element = found;
		this->parseDialog(dialogDepth,claPath,passedid,1);
	}
}
/* parseDialog(recursionDepth,claPath,firstFreeControlId,ignoreParentFlag) : finds a template and parses it into the current dialog */
void DcxmlParser::parseDialog(int depth,const char *claPath,const int passedid,const int ignoreParent) { 
	const TiXmlElement* child = NULL;
	int control = 0;
	g_claPath = NULL;
	g_claPathx = NULL;
	g_resetcla = 0;
	int cCla = 0;
	int cell = 0;
	for( child = this->element->FirstChildElement(); child != NULL; child = child->NextSiblingElement() ) {
		cell++;
		//STEP 1: SET ELEMENT AND PARENTELEMENT
		if (ignoreParent < 1)
			this->parent = child->Parent()->ToElement();
		this->element = child->ToElement();

		//STEP 2: PARSE ATTRIBUTES OF ELEMENTS
		this->parseAttributes();

		//dont itterate over unneccessary items
		if (0==lstrcmp(elem, "calltemplate")) 
		{
			if (0!=lstrcmp("template",element->Parent()->ToElement()->Value()))
			{
				cCla++;
				templateRef = element;
				templateRefcCla = cCla;
				char t_buffer [100];
				const char * t_claPathx = NULL;
				wnsprintf(t_buffer, 100, "%i",cCla);
				t_claPathx = t_buffer;
				const char* name;
				const char* value;
				for (const TiXmlAttribute *attribute = element->FirstAttribute(); attribute != NULL; attribute = attribute->Next()) 
				{ 
					name = attribute->Name(); 
					value = attribute->Value();
					if (0==lstrcmp(name, "name")) continue;
					this->template_vars[name] = value;
				} 
				std::map<const char*,const char*>::iterator iter;   
				for( iter = this->template_vars.begin(); iter != this->template_vars.end(); iter++ ) {
					Dcx::mIRC.execex("//set %%%s %s",iter->first,iter->second);
				}
				templateRefclaPath = t_claPathx;
				this->parseTemplate(depth,claPath,passedid);
				templateRef = 0;
				for( iter = this->template_vars.begin(); iter != this->template_vars.end(); iter++ ) {
					Dcx::mIRC.execex("//unset %%%s",iter->first);
				}
				this->template_vars.clear();
			}
			continue;
		}
		if ((0==lstrcmp(elem, "control")) || (0==lstrcmp(elem, "pane")))
			cCla++;
		else
			continue;

		//asign ID 
		if (0==lstrcmp(elem, "control")) 
		{ 
			controls++;
			id = this->parseId(element);
			id = (id > 0) ? id : 2000 + controls;
			this->registerId(element,id); // does this twice??
			this->registerId(element,id);
		}
		else
			id = passedid;

		//assign parent CONTROL of element
		while (parent != NULL) {
			if (0==lstrcmp(parentelem, "template")) 
			{
				parent = templateRef->Parent()->ToElement();
				parentelem = templateRef->Parent()->Value();
				cCla = templateRefcCla;
				claPath = templateRefclaPath;
			}
			if (0==lstrcmp(parentelem, "pane")) 
			{ 
				parent = parent->Parent()->ToElement();
				parentelem = parent->Value();
			}
			else break;    
		}
		parenttype = parent->Attribute("type");
		if (parenttype == NULL)
			parenttype = "panel";
		parentid = this->parseId(parent);
		parentid = (parentid > 0) ? parentid : passedid;
		//IF TEMPLATE ELEMENT REROUTE TO TEMPLATE DEFINITION


		//STEP 3: IF CONTROL CREATE IT AND ITS ITEMS
		if (0==lstrcmp(elem, "control")) {

			control++;

			//check how to insert the control in the parent Control/Dialog
			//If parentNode is pane loop untill parentNode is not a pane
			if (0==lstrcmp(parentelem, "dialog"))
				xdialogEX("-c","%i %s 0 0 %s %s %s", id,type,width,height,styles);
			else if (0==lstrcmp(parentelem, "control")) { 
				if (0==lstrcmp(parenttype, "panel"))
					this->xdidEX(parentid,"-c","%i %s 0 0 %s %s %s", id,type,width,height,styles);
				else if (0==lstrcmp(parenttype, "box"))
					this->xdidEX(parentid,"-c","%i %s 0 0 %s %s %s", id,type,width,height,styles);
				else if (0==lstrcmp(parenttype, "tab"))
					this->xdidEX(parentid,"-a","0 %s %s \t %i %s 0 0 %s %s %s \t %s", icon,caption,id,type,width,height,styles,tooltip);
				else if (((0==lstrcmp(parenttype, "pager")) || (0==lstrcmp(parenttype, "box"))) && (control == 1))
					this->xdidEX(parentid,"-c","%i %s 0 0 %s %s %s", id,type,width,height,styles);
				else if (0==lstrcmp(parenttype, "divider") && (control <= 2)) {
					if (control == 1)
						this->xdidEX(parentid,"-l","%s 0 \t %i %s 0 0 %s %s %s", width,id,type,width,height,styles);
					else if (control == 2)
						this->xdidEX(parentid,"-r","%s 0 \t %i %s 0 0 %s %s %s", width,id,type,width,height,styles);
				}
				else if (0==lstrcmp(parenttype, "rebar"))
					this->xdidEX(parentid,"-a","0 +%s %s %s %s %s %s %s \t %i %s 0 0 %s %s %s \t %s", ((tFlags) ? tFlags : "ceg"),rebarMinWidth,rebarMinHeight,width,icon,textcolour,caption,id,type,width,height,styles,tooltip);
				else if (0==lstrcmp(parenttype, "stacker")) 
					this->xdidEX(parentid,"-a","0 + %s %s %s \t %i %s 0 0 %s %s %s", textcolour,bgcolour,caption,id,type,width,height,styles);
				else if (0==lstrcmp(parenttype, "statusbar"))
					this->xdidEX(parentid,"-t","%i +c %s %i %s 0 0 0 0 %s", cell,icon,id,type,styles);
			}


		}
		//Set CLA for control or pane
		g_claPath = claPath;
		TString claPathx(this->parseCLA(cCla));

		//Perform some control specific commands
		if (0==lstrcmp(elem, "control")) {
			this->parseControl();
			this->parseIcons();	
			this->parseStyle();

		}
		//char *claPathx = "root";
		//Dcx::mIRC.execex("//echo -a clapath:%s",claPathx);
		this->parseDialog(depth+1,claPathx.to_chr(),id,0);  
	}
} 

// NB: never returns a ZERO, other code relies on this.
int DcxmlParser::mIRCEvalToUnsignedInt (const char *value)
{
	//Todo: method returns -1 for failure which odd for a *ToUnsignedInt method.
	//TString buf;
	//Dcx::mIRC.tsEval(buf, value);
	//int id = buf.to_int();
	//return (id > 0) ? id : -1;
	__int64 id;
	Dcx::mIRC.iEval(&id, value);
	return (int)((id > 0) ? id : -1);
}
void DcxmlParser::registerId(const TiXmlElement *idElement,const int id)
{
	//int elementId;
	//if (idElement->QueryIntAttribute("id",&elementId) != TIXML_SUCCESS) //<! id attr. is not an int
	//{
	//	const char *elementNamedId = (temp = idElement->Attribute("id")) ? temp : "";
	//	if (this->mIRCEvalToUnsignedInt(elementNamedId) < 0) //<! id attr. doesn't evaluate to an int
	//	{
	//		this->getDialog()->namedIds[elementNamedId] = id;
	//	}
	//}
	int elementId;
	if (idElement->QueryIntAttribute("id",&elementId) != TIXML_SUCCESS) //<! id attr. is not an int
	{
		const char *elementNamedId = idElement->Attribute("id");
		if (elementNamedId != NULL) {
			if (this->mIRCEvalToUnsignedInt(elementNamedId) < 0) //<! id attr. doesn't evaluate to an int
			{
				this->getDialog()->namedIds[elementNamedId] = id;
			}
		}
	}
}
int DcxmlParser::parseId(const TiXmlElement* idElement)
{
	//<! if id attribute is already integer return it
	int id = 0;
	if (idElement->QueryIntAttribute("id",&id) == TIXML_SUCCESS)
	{
		// found ID as a number,  if its not a negative, return it.
		if (id >= 0)
			return id;
		// id was a negative so return 0
		return 0;
	}

	//<! else try to evaluate the value to see if it ends up as an id;
	//const char *attributeIdValue = (temp = idElement->Attribute("id")) ? temp : "0";
	//id = mIRCEvalToUnsignedInt(attributeIdValue);
	//if (id > 0)
	//	return id;

	//TString value(attributeIdValue);

	////Otherwise if it's a namedId return it .find(attributeIdValue) never returned :(;
	//for(IntegerHash::const_iterator it = this->getDialog()->namedIds.begin(); it != this->getDialog()->namedIds.end(); ++it)
	//	if (it->first == value)
	//		return it->second;	

	//return 0;

	const char *attributeIdValue = idElement->Attribute("id");
	if (attributeIdValue != NULL)
	{
		// got ID attrib, evaluate it to try & resolve to a number.
		id = mIRCEvalToUnsignedInt(attributeIdValue);
		// if ID is > zero return it.
		if (id > 0)
			return id;

		//TString value(attributeIdValue);

		//Otherwise if it's a namedId return it .find(attributeIdValue) never returned :(;
		for(IntegerHash::const_iterator it = this->getDialog()->namedIds.begin(); it != this->getDialog()->namedIds.end(); ++it)
		{
			if (it->first == attributeIdValue)
				return it->second;
		}

	}
	return 0;
}

