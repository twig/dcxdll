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

#include "../../defines.h"
#include "../tinyxml/tinyxml.h"
#include "dcxml.h"
#include "../dcxdialog.h"
#include "../dcxdialogcollection.h"
#include "../dcxcontrol.h"
#include <map>
#include "../xpopup/xpopupmenumanager.h"

extern DcxDialogCollection Dialogs;
/*
dcxml [-FLAGS] [NAME] [DNAME] "[PATH]"
*/
class DCXML {
public:
    DcxDialog *d_Host;
    TiXmlElement* root; //!< dcxml root element
    TiXmlElement* dialogs; //!< dcxml dialogs collection element
    TiXmlElement* dialog; //!< dialog element
    TiXmlElement* element; //!< current Element
    TiXmlElement* parent; //!< current Element's parent
    TString dname;
    int controls; //!< Simple counter for controls
    int zlayered; //!< Simple check if dialog has zlayers
    //Attribute vars
    int id;
    int parentid;
    const char *elem;
    const char *parentelem;
    const char *parenttype;
    const char *type;
    const char *STclass;
    const char *weigth;
    const char *height;
    const char *width;
    const char *margin;
    const char *styles;
    const char *caption;
    const char *tooltip;
    const char *cascade;
    const char *icon;
    const char *tFlags;
    const char *integral;
    const char *state;
    const char *indent;
    const char *src;
    const char *cells;
    const char *rebarMinHeight;
    const char *rebarMinWidth;
    const char *iconsize;
    const char *fontstyle;
    const char *charset;
    const char *fontsize;
    const char *fontname;
    const char *border;
    const char *cursor;
    const char *bgcolour;
    const char *textbgcolour;
    const char *textcolour;
	const char *gradientstart;
	const char *gradientend;
    const char *disabledsrc;
    const char *hoversrc;
    const char *selectedsrc;

    TiXmlElement* templateRef;
    int templateRefcCla;
    const char *templateRefclaPath;
	std::map<const char*, const char*> template_vars;
    int eval;
    


    //tempvar to dump attribute values in;
    const char *temp;
    TString cmd;
    DCXML()
        : zlayered(0)
        , root(0)
        , dialogs(NULL)
        , dialog(NULL)
        , element(0)
        , controls(0)
    {
    }
	
    //CLA variables
    const char *g_claPath;
    const char *g_claPathx;
    int g_resetcla;

    /* parseAttributes() : parses attributes of the current element */
    void parseAttributes() {
        elem = element->Value();
        parentelem = parent->Value();
        parenttype = (temp = parent->Attribute("type")) ? temp : "panel";
        type = (temp = element->Attribute("type")) ? temp : "panel";
        STclass = (temp = element->Attribute("class")) ? temp : "";
        weigth = (temp = element->Attribute("weight")) ? temp : "1";
        height = (temp = element->Attribute("height")) ? temp : "0";
        width = (temp = element->Attribute("width")) ? temp : "0";
        margin = (temp = element->Attribute("margin")) ? temp : "0 0 0 0";
        styles = (temp = element->Attribute("styles")) ? temp : "";
		caption = (temp = element->Attribute("caption")) ? temp : (temp = element->GetText()) ? temp : "";
		tooltip = (temp = element->Attribute("tooltip")) ? temp : "";
        cascade = (temp = element->Attribute("cascade")) ? temp : "";
        icon = (temp = element->Attribute("icon")) ? temp : "0";
        integral = (temp = element->Attribute("integral")) ? temp : "0";
        state = (temp = element->Attribute("state")) ? temp : "0";
        indent = (temp = element->Attribute("indent")) ? temp : "0";
        //flags attribute defaults different per type/item
        tFlags = element->Attribute("flags");
        src = (temp = element->Attribute("src")) ? temp : "";
        cells = (temp = element->Attribute("cells")) ? temp : "-1";
        rebarMinHeight = (temp = element->Attribute("minheight")) ? temp : "0";
        rebarMinWidth = (temp = element->Attribute("minwidth")) ? temp : "0";
        iconsize = (temp = element->Attribute("iconsize")) ? temp : "16";
        eval = (element->QueryIntAttribute("eval",&eval) == TIXML_SUCCESS) ? eval : 0;

        fontstyle = (temp = element->Attribute("fontstyle")) ? temp : "d";
        charset = (temp = element->Attribute("charset")) ? temp : "ansi";
        fontsize = (temp = element->Attribute("fontsize")) ? temp : "";
        fontname = (temp = element->Attribute("fontname")) ? temp : "";
        border = (temp = element->Attribute("border")) ? temp : "";
        cursor = (temp = element->Attribute("cursor")) ? temp : "arrow";
        bgcolour = (temp = element->Attribute("bgcolour")) ? temp : "0";
        textbgcolour = (temp = element->Attribute("textbgcolour")) ? temp : "";
        textcolour = (temp = element->Attribute("textcolour")) ? temp : "0";

		gradientstart = (temp = element->Attribute("gradientstart")) ? temp : "";
		gradientend = (temp = element->Attribute("gradientend")) ? temp : "";

        disabledsrc = (temp = element->Attribute("disabledsrc")) ? temp : "";
        hoversrc = (temp = element->Attribute("hoversrc")) ? temp : "";
        selectedsrc = (temp = element->Attribute("selectedsrc")) ? temp : "";
    }
    void parseAttributes(TiXmlElement* element) {
        elem = element->Value();
        parentelem = parent->Value();
        parenttype = (temp = parent->Attribute("type")) ? temp : "panel";
        weigth = (temp = element->Attribute("weight")) ? temp : "1";
        height = (temp = element->Attribute("height")) ? temp : "0";
        width = (temp = element->Attribute("width")) ? temp : "0";
        margin = (temp = element->Attribute("margin")) ? temp : "0 0 0 0";
        styles = (temp = element->Attribute("styles")) ? temp : "";
        caption = (temp = element->Attribute("caption")) ? temp : (temp = element->GetText()) ? temp : "";
        tooltip = (temp = element->Attribute("tooltip")) ? temp : "";
        cascade = (temp = element->Attribute("cascade")) ? temp : "";
        icon = (temp = element->Attribute("icon")) ? temp : "0";
        integral = (temp = element->Attribute("integral")) ? temp : "0";
        state = (temp = element->Attribute("state")) ? temp : "0";
        indent = (temp = element->Attribute("indent")) ? temp : "0";
        //flags attribute defaults different per type/item
        tFlags = element->Attribute("flags");
        src = (temp = element->Attribute("src")) ? temp : "";
        cells = (temp = element->Attribute("cells")) ? temp : "-1";
        rebarMinHeight = (temp = element->Attribute("minheight")) ? temp : "0";
        rebarMinWidth = (temp = element->Attribute("minwidth")) ? temp : "0";
        iconsize = (temp = element->Attribute("iconsize")) ? temp : "16";
        eval = (element->QueryIntAttribute("eval",&eval) == TIXML_SUCCESS) ? eval : 0;

        fontstyle = (temp = element->Attribute("fontstyle")) ? temp : "d";
        charset = (temp = element->Attribute("charset")) ? temp : "ansi";
        fontsize = (temp = element->Attribute("fontsize")) ? temp : "";
        fontname = (temp = element->Attribute("fontname")) ? temp : "";
        border = (temp = element->Attribute("border")) ? temp : "";
        cursor = (temp = element->Attribute("cursor")) ? temp : "arrow";
        bgcolour = (temp = element->Attribute("bgcolour")) ? temp : "0";
        textbgcolour = (temp = element->Attribute("textbgcolour")) ? temp : "";
        textcolour = (temp = element->Attribute("textcolour")) ? temp : "0";

		gradientstart = (temp = element->Attribute("gradientstart")) ? temp : "";
		gradientend = (temp = element->Attribute("gradientend")) ? temp : "";

        disabledsrc = (temp = element->Attribute("disabledsrc")) ? temp : "";
        hoversrc = (temp = element->Attribute("hoversrc")) ? temp : "";
        selectedsrc = (temp = element->Attribute("selectedsrc")) ? temp : "";
    }
    /* parseControl() : if current element is a control perform some extra commands*/
    void parseControl() { 
		DcxControl *control = this->d_Host->getControlByID(id + mIRC_ID_OFFSET);
        if (element->Attribute("zlayer")) { 
            xdialogEX("-z","+a %i",id);
            zlayered = 1;
        }
        if ((0==lstrcmp(parenttype, "divider")) && (element->Attribute("width"))) {
            const char *width = (temp = element->Attribute("width")) ? temp : "100";
            xdidEX(parentid,"-v","%s",width);
        }
        if (((0==lstrcmp(type, "toolbar")) || (0==lstrcmp(type, "button")))
            || (0==lstrcmp(type, "treeview"))) { 
            xdidEX(id,"-l","%s",iconsize);
        }
        if (0==lstrcmp(type, "toolbar")) parseItems(element);
        else if (0==lstrcmp(type, "treeview")) parseItems(element);
        else if (0==lstrcmp(type, "comboex")) parseItems(element);
        else if (0==lstrcmp(type, "list")) parseItems(element);
        
        if (((((0==lstrcmp(type, "box")) || (0==lstrcmp(type, "check")))
            || (0==lstrcmp(type, "link"))) || (0==lstrcmp(type, "radio"))) || (0==lstrcmp(type, "button")))
             { 
                 if (caption) xdidEX(id,"-t","%s",caption);
        }
        if ((0==lstrcmp(type, "ipaddress")) && (caption))
            xdidEX(id,"-a","%s",caption);
        if ((0==lstrcmp(type, "webctrl")) && (src))
            xdidEX(id,"-n","%s",src);
        else if (0==lstrcmp(type, "text")) { 
            if (caption) { 
                TString mystring(caption);
                if (mystring.left(2) == "\r\n") mystring = mystring.right(-2);
                else if (mystring.left(1) == "\n") mystring = mystring.right(-1);
                mystring.replace("\t","");
                TString printstring;
                int textspace = 0;
                while(mystring.gettok(1," ") != "") { 
                    printstring.addtok(mystring.gettok(1," ").to_chr());
                    if (printstring.len() > 800) { 
                        xdidEX(id,"-a","%i %s",textspace,printstring.gettok(1,-1).to_chr());
                        printstring = "";
                        textspace = 1;
                    }
                    mystring.deltok(1," ");
                }
                if (printstring != "") { 
                    xdidEX(id,"-a","%i %s",textspace,printstring.gettok(1,-1).to_chr());
                }
            }
        }
        else if (0==lstrcmp(type, "edit")) { 
            if (caption) { 
                TString mystring(caption);
                if (mystring.left(2) == "\r\n") mystring = mystring.right(-2);
                else if (mystring.left(1) == "\n") mystring = mystring.right(-1);
                mystring.replace("\t","");
                int line = 0;
                while(mystring.gettok(1,"\r\n") != "") { 
                    line++;
                    xdidEX(id,"-i","%i %s",line,mystring.gettok(1,"\r\n").to_chr());
                    mystring.deltok(1,"\r\n");
                }
            }
        }
        else if (0==lstrcmp(type, "richedit")) { 
            if (caption) { 
                TString mystring(caption);
                if (mystring.left(2) == "\r\n") mystring = mystring.right(-2);
                else if (mystring.left(1) == "\n") mystring = mystring.right(-1);
                mystring.replace("\t","");
                mystring.replace("\\c","");
                mystring.replace("\\b","");
                mystring.replace("\\r","");
                int line = 0;
                while(mystring.gettok(1,"\r\n") != "") { 
                    line++;
                    xdidEX(id,"-i","%i %s",line,mystring.gettok(1,"\r\n").to_chr());
                    mystring.deltok(1,"\r\n");
                }
            }
        }
        else if (0==lstrcmp(type, "pbar")) { 
            if (caption) {
                xdidEX(id,"-i","%s",caption);
            }
        }
        else if (0==lstrcmp(type, "image")) { 
            const char *flags = (tFlags) ? tFlags : "";
            xdidEX(id,"-i","+%s %s",flags,src);
        }
        else if (0==lstrcmp(type, "statusbar")) { 
            xdidEX(id,"-l","%s",cells);
            parseItems(element);
        }
        disabledsrc = (temp = element->Attribute("disabledsrc")) ? temp : "";
        hoversrc = (temp = element->Attribute("hoversrc")) ? temp : "";
    }
	/* xdialogEX(switch,format[,args[]]) : performs an xdialog command internally or trough mIRC */
    void xdialogEX(const char *sw,const char *dFormat, ...) { 
            va_list args;
            va_start(args, dFormat);
            int cnt = _vscprintf(dFormat, args);
            char *txt = new char[cnt +1];
            vsprintf(txt, dFormat, args );
            va_end(args);
            if (eval) mIRCcomEX("//xdialog %s %s %s",sw,dname.to_chr(),txt);
            else d_Host->parseCommandRequestEX("%s %s %s",dname.to_chr(),sw,txt);
            delete [] txt;
    }
	/* xdidEX(controlId,switch,format[,args[]]) : performs an xdid command internally or trough mIRC on the specified id */
    void xdidEX(int id,const char *sw,const char *dFormat, ...) { 
            va_list args;
            va_start(args, dFormat);
            int cnt = _vscprintf(dFormat, args);
            char *txt = new char[cnt +1];
            vsprintf(txt, dFormat, args );
            va_end(args);
            if (eval) mIRCcomEX("//xdid %s %s %i %s",sw,dname.to_chr(),id,txt);
            else d_Host->parseComControlRequestEX(id,"%s %i %s %s",dname.to_chr(),id,sw,txt);
            delete [] txt;
    }
    /* parseCLA(int numberOfClaControlsInCurrentBranch) : parses control and pane elements and applies the right CLA commands */
    TString parseCLA(const int cCla) { 
        if (0==lstrcmp(elem, "control")) { 
            if ((0==lstrcmp(type, "panel")) || (0==lstrcmp(type, "box"))) {
                xdidEX(id,"-l","root \t +p%s 0 0 0 0",cascade);
                xdidEX(id,"-l","space root \t + %s",margin);
                g_resetcla = 1;
            }
            const char * fHeigth = "";
            const char * fWidth = "";
            const char * fixed = "l";
            if (element->Attribute("height")) { fHeigth = "v"; fixed = "f"; weigth = "0"; }
            if (element->Attribute("width")) { fWidth = "h"; fixed = "f"; weigth = "0"; }
            if (0==lstrcmp(parentelem, "dialog"))
            {
                xdialogEX("-l","cell %s \t +%s%s%si %i %s %s %s",
                    g_claPath,fixed,fHeigth,fWidth,id,weigth,width,height);
            }
            else if (0==lstrcmp(parentelem, "control")) {
                if ((parent->Attribute("type")) && (parentid)) {
                    if (0==lstrcmp(parent->Attribute("type"), "panel")) { 
                        xdidEX(parentid,"-l","cell %s \t +%s%s%si %i %s %s %s",
                            g_claPath,fixed,fHeigth,fWidth,id,weigth,width,height); 
                    }
                    else if (0==lstrcmp(parent->Attribute("type"), "box")) { 
                        xdidEX(parentid,"-l","cell %s \t +%s%s%si %i %s %s %s",
                            g_claPath,fixed,fHeigth,fWidth,id,weigth,width,height); 
                    }
                }
            }
        }
        else if (0==lstrcmp(elem, "pane")) {
            if (0==lstrcmp(parentelem, "dialog")) { 
                xdialogEX("-l","cell %s \t +p%s 0 %s 0 0",g_claPath,cascade,weigth);
            }
            if (0==lstrcmp(parentelem, "control")) {
                if ((parenttype) && (parentid)) {
                    if (0==lstrcmp(parenttype, "panel"))
                        xdidEX(parentid,"-l","cell %s \t +p%s 0 %s 0 0",g_claPath,cascade,weigth);
                    else if (0==lstrcmp(parenttype, "box"))
                        xdidEX(parentid,"-l","cell %s \t +p%s 0 %s 0 0",g_claPath,cascade,weigth);
                }
            }
        }
        char buffer [100];
        const char * claPathx = 0;
        if (g_resetcla) {
            lstrcpy(buffer, "root");
            claPathx = buffer;
        }
        else if (0==lstrcmp(g_claPath, "root")) {
            wsprintf (buffer, "%i",cCla);
            claPathx = buffer;
        }
        else { 
            wsprintf (buffer, "%s %i",g_claPath,cCla);
            claPathx = buffer;
        }
        if (element->Attribute("margin")) {
            if (0==lstrcmp(parentelem, "dialog"))
                xdialogEX("-l","space %s \t + %s",claPathx,margin);
            else xdidEX(parentid,"-l","space %s \t + %s",g_claPath,margin);
        }
        g_resetcla = 0;
        return TString(claPathx);
    }
    /* setStyle(TiXmlElement*) : Applies the styles described on the element found by parseStyle() */
    void setStyle(TiXmlElement* style) {
        //style attributes evaluate by default unless eval="0" is set on the element explicitly
		eval = (style->QueryIntAttribute("eval",&eval) == TIXML_SUCCESS) ? eval : 1;
		//font
        fontstyle = (temp = style->Attribute("fontstyle")) ? temp : "d";
        charset = (temp = style->Attribute("charset")) ? temp : "ansi";
        fontsize = (temp = style->Attribute("fontsize")) ? temp : "";
        fontname = (temp = style->Attribute("fontname")) ? temp : "";
        if ((style->Attribute("fontsize")) || (style->Attribute("fontname")))
            xdidEX(id,"-f","+%s %s %s %s",
                fontstyle,charset,fontsize,fontname);
        //border
        border = (temp = style->Attribute("border")) ? temp : "";
        if (style->Attribute("border")) xdidEX(id,"-x","+%s",border);
        //colours
        cursor = (temp = style->Attribute("cursor")) ? temp : "arrow";
        bgcolour = (temp = style->Attribute("bgcolour")) ? temp : "";
        textbgcolour = (temp = style->Attribute("textbgcolour")) ? temp : "";
        textcolour = (temp = style->Attribute("textcolour")) ? temp : "";
        if (style->Attribute("bgcolour")) { 
            xdidEX(id,"-C","+b %s",bgcolour);
            if (0==lstrcmp(type, "pbar")) 
			{
				xdidEX(id,"-k","%s",bgcolour);
				xdidEX(id,"-U","%s","");
			}
        }
        if (style->Attribute("textbgcolour")) 
		{
			xdidEX(id,"-C","+k %s",textbgcolour);
			if (0==lstrcmp(type, "pbar")) 
			{
				xdidEX(id,"-c","%s",textbgcolour);
				xdidEX(id,"-U","%s","");
			}
		}
        else if (style->Attribute("bgcolour")) xdidEX(id,"-C","+k %s",bgcolour); 
        if (style->Attribute("textcolour")) 
		{
			xdidEX(id,"-C","+t %s",textcolour);
			if (0==lstrcmp(type, "pbar")) 
			{
				xdidEX(id,"-q","%s",textcolour);
				xdidEX(id,"-U","%s","");
			}
		}
		
		if (style->Attribute("gradientstart")) xdidEX(id,"-C","+g %s",gradientstart);
		if (style->Attribute("gradientend")) xdidEX(id,"-C","+G %s",gradientend);

        //cursor
        if (style->Attribute("cursor")) xdidEX(id,"-J","+r %s",cursor);    

        //iconsize
        if (style->Attribute("iconsize")) 
        { 
            if (((0==lstrcmp(type, "toolbar")) || (0==lstrcmp(type, "button")))
                || (0==lstrcmp(type, "treeview"))) 
            { 
                xdidEX(id,"-l","%s",iconsize);
            }
        }
        if (0==lstrcmp(type, "button"))
        {
            if (!element->Attribute("bgcolour")) bgcolour = "65280";
            if (element->Attribute("src"))
            {
                xdidEX(id,"-k","+n %s %s",bgcolour,src);
            }
            if (element->Attribute("disabledsrc"))
            {
                xdidEX(id,"-k","+n %s %s",bgcolour,disabledsrc);
            }
            if (element->Attribute("hoversrc"))
            {
                xdidEX(id,"-k","+n %s %s",bgcolour,hoversrc);
            }
            if (element->Attribute("selectedsrc"))
            {
                xdidEX(id,"-k","+n %s %s",bgcolour,hoversrc);
            }
        }
    }

	/* parseStyle(recursionDepth) : Simple recursive method to cascade find the right style to apply to an element */
    void parseStyle(int depth = 0) { 
        if (depth > 2) return;
        depth++;
        TiXmlElement* styles = 0;
        TiXmlElement* style = 0;
        TiXmlElement* ClassElement = 0;
        TiXmlElement* TypeElement = 0;
        TiXmlElement* IdElement = 0;
        if (depth == 3) style = element;
        if (depth == 1) styles = dialogs->FirstChildElement("styles");
        if (depth == 2) styles = dialog->FirstChildElement("styles");
        if (styles) {
            if (style = styles->FirstChildElement("all")) setStyle(style);
            style = 0;
            for( style = styles->FirstChildElement("style"); style; style = style->NextSiblingElement()) {
                if (0==lstrcmp(style->Attribute("class"), STclass)) ClassElement = style;
                if (0==lstrcmp(style->Attribute("type"), type)) TypeElement = style;
                int t_id = parseId(style);
                if (t_id == id) IdElement = style;
            }
            if (IdElement) style = IdElement;
            else if (ClassElement) style = ClassElement;
            else if (TypeElement) style = TypeElement;
        }
        if (style) setStyle(style);
        parseStyle(depth);
    }
    int parseId(TiXmlElement* idElement)
	{
		int id = (idElement->QueryIntAttribute("id",&id) == TIXML_SUCCESS) ? id : 0;
		if (id > 0) return id;
		const char *attributeIdValue = (temp = idElement->Attribute("id")) ? temp : "0";
		TString buf((UINT)32);
		mIRCevalEX(buf.to_chr(), 32, attributeIdValue,"");
		id = buf.to_int();
		return (id > 0) ? id : 0;
	}

	/* parseIcons(recursionDepth) : Simple recursive method to cascade find the right icons to apply to an element */
    void parseIcons(int depth = 0) { 
        if (depth > 1) return;
        depth++;
        TiXmlElement* icons = 0;
        TiXmlElement* icon = 0;
        TiXmlElement* ClassElement = 0;
        TiXmlElement* TypeElement = 0;
        TiXmlElement* IdElement = 0;
        if (depth == 1) icons = dialog->FirstChildElement("icons");
        if (depth == 2) icons = dialogs->FirstChildElement("icons");
        if (icons) {
            for( icon = icons->FirstChildElement("icon"); icon; icon = icon->NextSiblingElement()) {
                if (0==lstrcmp(icon->Attribute("class"), STclass)) ClassElement = icon;
                if (0==lstrcmp(icon->Attribute("type"), type)) TypeElement = icon;
                int t_id = parseId(icon);
                if (t_id == id) IdElement = icon;
            }
            if (IdElement) icon = IdElement;
            else if (ClassElement) icon = ClassElement;
            else if (TypeElement) icon = TypeElement;
            else icon = 0;
            if (icon) { 
                const char *flags = (temp = icon->Attribute("flags")) ? temp : "ndhs";
                const char *index = (temp = icon->Attribute("index")) ? temp : "0";;
                const char *src = icon->Attribute("src");
                int indexmin = (icon->QueryIntAttribute("indexmin",&indexmin) == TIXML_SUCCESS) ? indexmin : 0;
                int indexmax = (icon->QueryIntAttribute("indexmax",&indexmax) == TIXML_SUCCESS) ? indexmax : -1;
                if (src) { 
                    if (indexmin <= indexmax) 
                        //method sucks but looping in C++ is WAYYY too fast for mIRC
                            mIRCcomEX("//var %%x = %i | while (%%x <= %i ) { xdid -w %s %i +%s %%x %s | inc %%x }",
                                indexmin,indexmax,dname.to_chr(),id,flags,src);
                    else 
                        mIRCcomEX("//xdid -w %s %i +%s %s %s",dname.to_chr(),id,flags,index,src);
                }
            }
        }
        else parseIcons(depth);
    }
	/* parseItems(XmlElement,recursionDepth,itemPath) : recursively applies items for a control */
    void parseItems(TiXmlElement* element,int depth = 0,char *itemPath = "") { 
        int item = 0;
        int cell = 0;
        TiXmlElement* child = 0;
        for( child = element->FirstChildElement(); child; child = child->NextSiblingElement() ) {
            const char *childelem = child->Value();
            if ((0==lstrcmp(childelem, "item")) || (0==lstrcmp(childelem, "control")))
                cell++;
            //fill all required parameters with attributes or default values
            parseAttributes(child);
            if (0==lstrcmp(childelem, "item")) { 
                item++;            
                if (0==lstrcmp(type, "toolbar")) { 
                    const char *flags = (tFlags) ? tFlags : "a";
                    xdidEX(id,"-a","0 +%s %s %s %s %s \t %s",
                        flags,width,icon,textcolour,caption,tooltip);
                }
                else if (0==lstrcmp(type, "comboex")) { 
                    xdidEX(id,"-a","0 %s %s %s 0 %s",
                        indent,icon,icon,caption);
                }
                else if (0==lstrcmp(type, "list")) { 
                    xdidEX(id,"-a","0 %s",
                        caption);
                }
                else if (0==lstrcmp(type, "statusbar")) { 
                    const char *flags = (tFlags) ? tFlags : "f";
                    xdidEX(id,"-t","%i +%s %s %s \t %s",
                        cell,flags,icon,caption,tooltip);
                }
                else if (0==lstrcmp(type, "treeview")) { 
                    const char *flags = (tFlags) ? tFlags : "a";
                    char buffer [100];
                    char * pathx = 0;
                    wsprintf (buffer, "%s %i",itemPath,item);
                    pathx = buffer;
                    xdidEX(id,"-a","%s \t +%s %s %s 0 %s %s %s %s %s \t %s",
                        pathx,flags,icon,icon,state,integral,textcolour,bgcolour,caption,tooltip);
                    parseItems(child,depth,pathx);
                }
            }
        }
    }

	/* parseTemplate(recursionDepth,claPath,firstFreeControlId) : finds a template and parses it into the current dialog */
    void parseTemplate(int dialogDepth=0,const char *claPath = "root",int passedid = 2000)
    {
        TiXmlElement* Template = 0;
        TiXmlElement* lookIn = 0;
        TiXmlElement* found = 0;
        lookIn = dialogs->FirstChildElement("templates");
        for (Template = lookIn->FirstChildElement("template");Template;Template = Template->NextSiblingElement()) 
        {
            if (0==lstrcmp(Template->Attribute("name"), element->Attribute("name")))
            { 
                found = Template;
                break;
            }
        }
        if (found) 
        {
            element = found;
            parseDialog(dialogDepth,claPath,passedid,1);
        }
    }
	/* parseDialog(recursionDepth,claPath,firstFreeControlId,ignoreParentFlag) : finds a template and parses it into the current dialog */
    void parseDialog(int depth=0,const char *claPath = "root",int passedid = 2000,int ignoreParent = 0) { 
        TiXmlElement* child = 0;
        int control = 0;
        g_claPath = 0;
        g_claPathx = 0;
        g_resetcla = 0;
        int cCla = 0;
        int cell = 0;
        for( child = element->FirstChildElement(); child; child = child->NextSiblingElement() ) {
            cell++;
            //STEP 1: SET ELEMENT AND PARENTELEMENT
            if (!ignoreParent) parent = child->Parent()->ToElement();
            element = child->ToElement();
            
            //STEP 2: PARSE ATTRIBUTES OF ELEMENTS
            parseAttributes();
            
            //dont itterate over unneccessary items
            if (0==lstrcmp(elem, "calltemplate")) 
            {
                if (0!=lstrcmp("template",element->Parent()->ToElement()->Value()))
                {
                    cCla++;
                    templateRef = element;
                    templateRefcCla = cCla;
                    char t_buffer [100];
                    const char * t_claPathx = 0;
                    wsprintf (t_buffer, "%i",cCla);
                    t_claPathx = t_buffer;
					TiXmlAttribute* attribute;
					const char* name;
					const char* value;
					for (attribute = element->FirstAttribute() ; attribute ; attribute = attribute->Next()) 
					{ 
						name = attribute->Name(); 
						value = attribute->Value();
						if (0==lstrcmp(name, "name")) continue;
						template_vars[name] = value;
					} 
					std::map<const char*,const char*>::iterator iter;   
					for( iter = template_vars.begin(); iter != template_vars.end(); iter++ ) {
						mIRCcomEX("//set %%%s %s",iter->first,iter->second);
					}
                    templateRefclaPath = t_claPathx;
                    parseTemplate(depth,claPath,passedid);
                    templateRef = 0;
					for( iter = template_vars.begin(); iter != template_vars.end(); iter++ ) {
						mIRCcomEX("//unset %%%s",iter->first);
					}
					template_vars.clear();
                }
                continue;
            }
			if ((0==lstrcmp(elem, "control")) || (0==lstrcmp(elem, "pane"))) cCla++;
            else continue;

            //asign ID 
            if (0==lstrcmp(elem, "control")) 
			{ 
                controls++;
                id = parseId(element);
				id = (id > 0) ? id : 2000 + controls;
            }
            else id = passedid;

            //assign parent CONTROL of element
            while (parent) {
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
            parenttype = (temp = parent->Attribute("type")) ? temp : "panel";
            parentid = parseId(parent);
			parentid = (parentid > 0) ? parentid : passedid;
            //IF TEMPLATE ELEMENT REROUTE TO TEMPLATE DEFINITION


            //STEP 3: IF CONTROL CREATE IT AND ITS ITEMS
			if (0==lstrcmp(elem, "control")) {

                control++;
                //check how to insert the control in the parent Control/Dialog
                //If parentNode is pane loop untill parentNode is not a pane
                if (0==lstrcmp(parentelem, "dialog")) { 
                    xdialogEX("-c","%i %s 0 0 %s %s %s",id,type,width,height,styles);
                }
                else if (0==lstrcmp(parentelem, "control")) { 
                    if (0==lstrcmp(parenttype, "panel"))
                        xdidEX(parentid,"-c","%i %s 0 0 %s %s %s",
                            id,type,width,height,styles);

                    else if (0==lstrcmp(parenttype, "box"))
                        xdidEX(parentid,"-c","%i %s 0 0 %s %s %s",
                            id,type,width,height,styles);

                    else if (0==lstrcmp(parenttype, "tab"))
                        xdidEX(parentid,"-a","0 %s %s \t %i %s 0 0 %s %s %s \t %s",
                            icon,caption,id,type,width,height,styles,tooltip);

                    else if (((0==lstrcmp(parenttype, "pager")) || (0==lstrcmp(parenttype, "box"))) && (control == 1))
                        xdidEX(parentid,"-c","%i %s 0 0 %s %s %s",
                            id,type,width,height,styles);

                    else if (0==lstrcmp(parenttype, "divider") && (control <= 2)) {
                            if (control == 1)
                                xdidEX(parentid,"-l","%s 0 \t %i %s 0 0 %s %s %s",
                                    width,id,type,width,height,styles);
                            else if (control == 2)
                                xdidEX(parentid,"-r","%s 0 \t %i %s 0 0 %s %s %s",
                                    width,id,type,width,height,styles);
                    }

                    else if (0==lstrcmp(parenttype, "rebar")) { 
                        const char *flags = (tFlags) ? tFlags : "ceg";
                        xdidEX(parentid,"-a","0 +%s %s %s %s %s %s %s \t %i %s 0 0 %s %s %s \t %s",
                            flags,rebarMinWidth,rebarMinHeight,width,icon,textcolour,caption,
                            id,type,width,height,styles,tooltip);
                    }
                    else if (0==lstrcmp(parenttype, "stacker")) 
                        xdidEX(parentid,"-a","0 + %s %s %s \t %i %s 0 0 %s %s %s",
                            textcolour,bgcolour,caption,id,type,width,height,styles);
                    
                    else if (0==lstrcmp(parenttype, "statusbar"))
                        xdidEX(parentid,"-t","%i +c %s %i %s 0 0 0 0 %s",
                            cell,icon,id,type,styles);
                }
                
                
            }
            //Set CLA for control or pane
            g_claPath = claPath;
            TString claPathx = parseCLA(cCla);

            //Perform some control specific commands
            if (0==lstrcmp(elem, "control")) {
                parseControl();
                parseIcons();
                parseStyle();
            }
            //char *claPathx = "root";
            //mIRCcomEX("//echo -a clapath:%s",claPathx);
            parseDialog(depth+1,claPathx.to_chr(),id,0);  
        }
    } 

protected:
	

};

/*
 * /dcxml -[SWITCH] [?NAME] [DATASET] [PARMS]
 *
 * -d = [DNAME] [DATASET] [FILENAME]
 * -p = [PNAME] [DATASET] [FILENAME]
 */
mIRC(dcxml) {
    TString input(data);
    int numtok = input.numtok();
    
    if (numtok < 3) {
        DCXError("/dcxml", "Insuffient parameters");
        return 1;
    }

	TiXmlBase::SetCondenseWhiteSpace( false ); 
	TiXmlDocument doc(input.gettok(2,"\"").to_chr());
	bool dcxmlFile = doc.LoadFile();

	if (!dcxmlFile) { 
		DCXErrorEX("/dcxml", "XML error (row %i column %i) %s",doc.ErrorRow(),doc.ErrorCol(),doc.ErrorDesc());
		return 0;
	}

	if (!doc.FirstChildElement("dcxml")) {
		DCXError("/dcxml", "Root Element Is Not 'dcxml'");
		return 0;
	}

    XSwitchFlags flags(input.gettok(1));

	// Parse XDialog XML.
    if (flags['d']) {
		DCXML oDcxml;

        oDcxml.dname = input.gettok(2).to_chr();
		oDcxml.d_Host = Dialogs.getDialogByName(oDcxml.dname);

		if (oDcxml.d_Host == NULL) {
			DCXError("/dcxml", "Host dialog has not been marked.");
            return 0;
		}

		oDcxml.root = doc.FirstChildElement("dcxml");

        oDcxml.dialogs = oDcxml.root->FirstChildElement("dialogs");
        if (!oDcxml.dialogs) { 
            DCXError("/dcxml","No 'dialogs' Group");
            return 0;
        }
        for( oDcxml.element = oDcxml.dialogs->FirstChildElement("dialog"); oDcxml.element; oDcxml.element = oDcxml.element->NextSiblingElement("dialog") ) {
            const char *name = oDcxml.element->Attribute("name");
            if (0==lstrcmp(name, input.gettok(3," ").to_chr())) { 
                oDcxml.dialog = oDcxml.element;
                break;
            }
        }
        if (!oDcxml.dialog) { 
            DCXError("/dcxml","Dialog name not found in <dialogs>");
            return 0;
        }

        const char *cascade = (oDcxml.temp = oDcxml.dialog->Attribute("cascade")) ? oDcxml.temp : "v";
        const char *margin = (oDcxml.temp = oDcxml.dialog->Attribute("margin")) ? oDcxml.temp : "0 0 0 0";
        const char *caption = (oDcxml.temp = oDcxml.dialog->Attribute("caption")) ? oDcxml.temp : oDcxml.dname.to_chr();
        const char *border = oDcxml.dialog->Attribute("border");
        if (border) oDcxml.xdialogEX("-b","+%s",border);
        mIRCcomEX("//dialog -t %s %s",oDcxml.dname.to_chr(),caption);
        oDcxml.xdialogEX("-l","root \t +p%s 0 0 0 0",cascade);
        oDcxml.xdialogEX("-l","space root \t + %s",margin);
        oDcxml.parseDialog();
        mIRCcomEX("/.timer 1 0 xdialog -l %s update",oDcxml.dname.to_chr());
        int h = (oDcxml.dialog->QueryIntAttribute("h",&h) == TIXML_SUCCESS) ? h : -1;
        int w = (oDcxml.dialog->QueryIntAttribute("w",&w) == TIXML_SUCCESS) ? w : -1;
        int x = (oDcxml.dialog->QueryIntAttribute("x",&x) == TIXML_SUCCESS) ? x : -1;
        int y = (oDcxml.dialog->QueryIntAttribute("y",&y) == TIXML_SUCCESS) ? y : -1;
        mIRCcomEX("/dialog -s %s %i %i %i %i", oDcxml.dname.to_chr(),x,y,w,h);
        if (oDcxml.dialog->Attribute("center"))
            mIRCcomEX("/dialog -r %s", oDcxml.dname.to_chr());
        //This "Shite" is to activate the first zlayer, added a check if this command starts returning an error
        if (oDcxml.zlayered) oDcxml.xdialogEX("-z","+s 1");
        return 1;
    }
	// Parse XPopup DCXML.
	else if (flags['p']) {
		TiXmlElement *popups = doc.FirstChildElement("dcxml")->FirstChildElement("popups");
		TiXmlElement *popup = NULL;
		TString popupName(input.gettok(2));
		TString popupDataset(input.gettok(3));
        
		if ((popupName == "mircbar") || (popupName == "mirc") || (popupName == "scriptpopup")) {
			DCXErrorEX("/dcxml", "Menu name '%s' is reserved.", popupName.to_chr());
			return 0;
		}

		// Couldnt find popups group.
        if (!popups) { 
            DCXError("/dcxml", "No 'popups' Group");
            return 0;
        }

		XPopupMenuManager::LoadPopupsFromXML(popups, popup, popupName, popupDataset);
		return 1;
	}
	// Unknown flags.
	else {
		DCXErrorEX("/dcxml", "Unknown flag %s", input.gettok(1).to_chr());
		return 0;
	}

    return 1;
}