#include "../../defines.h"
#include "../tinyxml/tinyxml.h"
#include "dcxml.h"
/*
dcxml [-FLAGS] [NAME] [DNAME] "[PATH]"
*/
class DCXML {
public:
	TiXmlElement* root; //!< dcxml root element
	TiXmlElement* dialogs; //!< dcxml dialogs collection element
	TiXmlElement* dialog; //!< dialog element
	TiXmlElement* element; //!< current Element
	TiXmlElement* parent; //!< current Element's parent
	TString dname;
	int controls; //!< Simple counter for controls

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
	const char *colour;
	const char *icon;
	const char *tFlags;
	const char *integral;
	const char *bgcolour;
	const char *state;
	const char *indent;
	const char *src;
	const char *cells;
	const char *rebarMinHeight;
	const char *rebarMinWidth;
	const char *iconsize;

	//tempvar to dump attribute values in;
	const char *temp;
	TString cmd;

	//CLA variables
	const char *g_claPath;
	const char *g_claPathx;
	int g_resetcla;

	DCXML() {
		root = 0; 
		dialogs = 0; 
		dialog = 0; 
		element = 0;
		controls = 0;
	}
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
		caption = (temp = element->Attribute("caption")) ? temp : element->GetText();
		tooltip = (temp = element->Attribute("tooltip")) ? temp : "";
		cascade = (temp = element->Attribute("cascade")) ? temp : "";
		colour = (temp = element->Attribute("colour")) ? temp : "0";
		icon = (temp = element->Attribute("icon")) ? temp : "0";
		integral = (temp = element->Attribute("integral")) ? temp : "0";
		bgcolour = (temp = element->Attribute("bgcolour")) ? temp : "0";
		state = (temp = element->Attribute("state")) ? temp : "0";
		indent = (temp = element->Attribute("indent")) ? temp : "0";
		//flags attribute defaults different per type/item
		tFlags = element->Attribute("flags");
		src = (temp = element->Attribute("src")) ? temp : "";
		cells = (temp = element->Attribute("cells")) ? temp : "-1";
		rebarMinHeight = (temp = element->Attribute("minheight")) ? temp : "0";
		rebarMinWidth = (temp = element->Attribute("minwidth")) ? temp : "0";
		iconsize = (temp = element->Attribute("iconsize")) ? temp : "16";
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
		caption = (temp = element->Attribute("caption")) ? temp : element->GetText();
		tooltip = (temp = element->Attribute("tooltip")) ? temp : "";
		cascade = (temp = element->Attribute("cascade")) ? temp : "";
		colour = (temp = element->Attribute("colour")) ? temp : "0";
		icon = (temp = element->Attribute("icon")) ? temp : "0";
		integral = (temp = element->Attribute("integral")) ? temp : "0";
		bgcolour = (temp = element->Attribute("bgcolour")) ? temp : "0";
		state = (temp = element->Attribute("state")) ? temp : "0";
		indent = (temp = element->Attribute("indent")) ? temp : "0";
		//flags attribute defaults different per type/item
		tFlags = element->Attribute("flags");
		src = (temp = element->Attribute("src")) ? temp : "";
		cells = (temp = element->Attribute("cells")) ? temp : "-1";
		rebarMinHeight = (temp = element->Attribute("minheight")) ? temp : "0";
		rebarMinWidth = (temp = element->Attribute("minwidth")) ? temp : "0";
		iconsize = (temp = element->Attribute("iconsize")) ? temp : "16";
	}

	/* parseControl() : if current element is a control perform some extra commands*/
	void parseControl() { 
		//zlayer control 
		if (element->Attribute("zlayer")) mIRCcomEX("/xdialog -z %s +a %i",dname.to_chr(),id);
		if ((0==lstrcmp(parenttype, "divider")) && (element->Attribute("width"))) {
			const char *width = (temp = element->Attribute("width")) ? temp : "100";
			mIRCcomEX("/echo -a xdid -v %s %i %s",dname.to_chr(),parentid,width);
		}
		if ((0==lstrcmp(type, "toolbar")) || (0==lstrcmp(type, "button"))) { 
			mIRCcomEX("//xdid -l %s %i %s",dname.to_chr(),id,iconsize);
			parseItems(element);
		}
		else if (0==lstrcmp(type, "treeview")) parseItems(element);
		else if (0==lstrcmp(type, "comboex")) parseItems(element);
		else if (0==lstrcmp(type, "list")) parseItems(element);
		
		if (((((0==lstrcmp(type, "box")) || (0==lstrcmp(type, "check")))
			|| (0==lstrcmp(type, "link"))) || (0==lstrcmp(type, "radio"))) || (0==lstrcmp(type, "button")))
			 { 
				 if (caption) 
					 mIRCcomEX("//xdid -t %s %i %s",dname.to_chr(),id,caption);
		}
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
						mIRCcomEX("//xdid -a %s %i %i %s",dname.to_chr(),id,textspace,printstring.gettok(1,-1).to_chr());
						printstring = "";
						textspace = 1;
					}
					mystring.deltok(1," ");
				}
				if (printstring != "") { 
					mIRCcomEX("//xdid -a %s %i %i %s",dname.to_chr(),id,textspace,printstring.gettok(1,-1).to_chr());
				}
			}
		}
		else if (0==lstrcmp(type, "edit")) { 
			if (caption) { 
				TString mystring(caption);
				if (mystring.left(2) == "\r\n") mystring = mystring.right(-2);
				else if (mystring.left(1) == "\n") mystring = mystring.right(-1);
				mystring.replace("\t","");
				mystring.replace("\n","\r\n");
				TString printstring;
				while(mystring.gettok(1," ") != "") { 
					printstring.addtok(mystring.gettok(1," ").to_chr());
					if (printstring.len() > 800) { 
						mIRCcomEX("//xdid -a %s %i %s",dname.to_chr(),id,printstring.gettok(1,-1).to_chr());
						printstring = "";
					}
					mystring.deltok(1," ");
				}
				if (printstring != "") { 
					mIRCcomEX("//xdid -a %s %i %s",dname.to_chr(),id,printstring.gettok(1,-1).to_chr());
				}
			}
		}
		else if (0==lstrcmp(type, "pbar")) { 
			if (caption) {
				mIRCcomEX("//xdid -i %s %i %s",dname.to_chr(),id,caption);
			}
		}
		else if (0==lstrcmp(type, "image")) { 
			const char *flags = (tFlags) ? tFlags : "";
			mIRCcomEX("//xdid -i %s %i +%s %s",dname.to_chr(),id,flags,src);
		}
		else if (0==lstrcmp(type, "statusbar")) { 
			mIRCcomEX("//xdid -l %s %i %s",dname.to_chr(),id,cells);
			parseItems(element);
		}
	}

	/* parseCLA(int cCla) parses control and pane elements and applies the right CLA commands */
	TString parseCLA(int cCla) { 
		if (0==lstrcmp(elem, "control")) { 
			if ((0==lstrcmp(type, "panel")) || (0==lstrcmp(type, "box"))) {
				mIRCcomEX("//xdid -l %s %i root $chr(9) +p%s 0 0 0 0", dname.to_chr(),id,cascade);
				mIRCcomEX("//xdid -l %s %i space root $chr(9) + %s", dname.to_chr(),id,margin);
				g_resetcla = 1;
			}
			const char * fHeigth = "";
			const char * fWidth = "";
			const char * fixed = "l";
			if (element->Attribute("height")) { fHeigth = "v"; fixed = "f"; }
			if (element->Attribute("width")) { fWidth = "h"; fixed = "f"; }
			if (0==lstrcmp(parentelem, "dialog")) 
				mIRCcomEX("//xdialog -l %s cell %s \t +%s%s%si %i %s %s %s",
					dname.to_chr(),g_claPath,fixed,fHeigth,fWidth,id,weigth,width,height); 

			else if (0==lstrcmp(parentelem, "control")) {
				if ((parent->Attribute("type")) && (parentid)) {
					if (0==lstrcmp(parent->Attribute("type"), "panel")) { 
						mIRCcomEX("//xdid -l %s %i cell %s \t +%s%s%si %i %s %s %s",
							dname.to_chr(),parentid,g_claPath,fixed,fHeigth,fWidth,id,weigth,width,height); 
					}
					else if (0==lstrcmp(parent->Attribute("type"), "box")) { 
						mIRCcomEX("//xdid -l %s %i cell %s \t +%s%s%si %i %s %s %s",
							dname.to_chr(),parentid,g_claPath,fixed,fHeigth,fWidth,id,weigth,width,height); 
					}
				}
			}
		}
		else if (0==lstrcmp(elem, "pane")) {
				if (0==lstrcmp(parentelem, "dialog"))
					mIRCcomEX("//xdialog -l %s cell %s \t +p%s 0 %s 0 0", dname.to_chr(),g_claPath,cascade,weigth);
				if (0==lstrcmp(parentelem, "control")) {
					if ((parenttype) && (parentid)) {
						if (0==lstrcmp(parenttype, "panel"))
							mIRCcomEX("//xdid -l %s %i cell %s \t +p%s 0 %s 0 0", dname.to_chr(),parentid,g_claPath,cascade,weigth);
						else if (0==lstrcmp(parenttype, "box"))
							mIRCcomEX("//xdid -l %s %i cell %s \t +p%s 0 %s 0 0", dname.to_chr(),parentid,g_claPath,cascade,weigth);
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
		if (margin) {
			if (0==lstrcmp(parentelem, "dialog"))
				mIRCcomEX("//xdialog -l %s space %s $chr(9) + %s", dname.to_chr(),claPathx,margin);
			else 
				mIRCcomEX("//xdid -l %s %i space %s $chr(9) + %s", dname.to_chr(),id,claPathx,margin);
	
		}
		g_resetcla = 0;
		return TString(claPathx);
	}


	
	void parseStyle(int depth = 0) { 
		if (depth > 2) return;
		depth++;
		TiXmlElement* styles = 0;
		TiXmlElement* style = 0;
		TiXmlElement* ClassElement = 0;
		TiXmlElement* TypeElement = 0;
		TiXmlElement* IdElement = 0;
		if (depth == 1) style = element;
		if (depth == 2) styles = dialogs->FirstChildElement("styles");
		if (depth == 3) styles = dialog->FirstChildElement("styles");
		if (styles) {
			for( style = styles->FirstChildElement("style"); style; style = style->NextSiblingElement()) {
				if (0==lstrcmp(style->Attribute("class"), STclass)) ClassElement = style;
				if (0==lstrcmp(style->Attribute("type"), type)) TypeElement = style;
				int t_id = (style->QueryIntAttribute("id",&t_id) == TIXML_SUCCESS) ? t_id : 0;
				if (t_id == id) IdElement = style;
			}
			if (IdElement) style = IdElement;
			else if (ClassElement) style = ClassElement;
			else if (TypeElement) style = TypeElement;
			else style = styles->FirstChildElement("all");
		}
		if (style) { 
			//font
			const char *fontstyle = (temp = style->Attribute("fontstyle")) ? temp : "d";
			const char *charset = (temp = style->Attribute("charset")) ? temp : "ansi";
			const char *fontsize = (temp = style->Attribute("fontsize")) ? temp : "";
			const char *fontname = (temp = style->Attribute("fontname")) ? temp : "";
			mIRCcomEX("//xdid -f %s %i +%s %s %s %s",
				dname.to_chr(),id,fontstyle,charset,fontsize,fontname);
			//border
			const char *border = (temp = style->Attribute("border")) ? temp : "";
			if (style->Attribute("border")) mIRCcomEX("/xdid -x %s %i +%s",dname.to_chr(),id,border);
			//colours
			const char *bgcolour = (temp = style->Attribute("bgcolour")) ? temp : "";
			const char *textbgcolour = (temp = style->Attribute("textbgcolour")) ? temp : "";
			const char *textcolour = (temp = style->Attribute("textcolour")) ? temp : "";
			if (style->Attribute("bgcolour")) { 
				mIRCcomEX("//xdid -C %s %i +b %s",dname.to_chr(),id,bgcolour);
				if (0==lstrcmp(type, "pbar")) mIRCcomEX("/xdid -U %s %i",dname.to_chr(),id);
			}
			if (style->Attribute("textbgcolour")) { 
				mIRCcomEX("//xdid -C %s %i +k %s",dname.to_chr(),id,textbgcolour);
			}
			else if (style->Attribute("bgcolour")) 
				mIRCcomEX("//xdid -C %s %i +k %s",dname.to_chr(),id,bgcolour); 
			if (style->Attribute("textcolour")) 
				mIRCcomEX("//xdid -C %s %i +t %s",dname.to_chr(),id,textcolour);

			//cursor
			if (style->Attribute("cursor")) {
				const char *cursor = (temp = style->Attribute("cursor")) ? temp : "arrow";
				mIRCcomEX("//xdid -J %s %i +r %s",dname.to_chr(),id,cursor);	
			}

		}
		parseStyle(depth);
	}

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
				int t_id = (icon->QueryIntAttribute("id",&t_id) == TIXML_SUCCESS) ? t_id : 0;
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
				int indexmin = (icon->QueryIntAttribute("indexmin",&indexmin) == TIXML_SUCCESS) ? indexmin : 1;
				int indexmax = (icon->QueryIntAttribute("indexmax",&indexmax) == TIXML_SUCCESS) ? indexmax : 0;
				if (src) { 
					if (indexmin <= indexmax) 
							mIRCcomEX("//var %%x = %i | while (%%x <= %i ) { xdid -w %s %i +%s %%x %s | inc %%x }",
								indexmin,indexmax,dname.to_chr(),id,flags,src);
					else 
						mIRCcomEX("//xdid -w %s %i +%s %s %s",dname.to_chr(),id,flags,index,src);
				}
			}
		}
		else parseIcons(depth);
	}
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
					mIRCcomEX("//xdid -a %s %i 0 +%s %s %s %s %s \t %s",
						dname.to_chr(),id,flags,width,icon,colour,caption,tooltip);
				}
				else if (0==lstrcmp(type, "comboex")) { 
					mIRCcomEX("//xdid -a %s %i 0 %s %s %s 0 %s",
						dname.to_chr(),id,indent,icon,icon,caption);
				}
				else if (0==lstrcmp(type, "list")) { 
					mIRCcomEX("//xdid -a %s %i 0 %s",
						dname.to_chr(),id,caption);
				}
				else if (0==lstrcmp(type, "statusbar")) { 
					const char *flags = (tFlags) ? tFlags : "f";
					mIRCcomEX("//xdid -t %s %i %i +%s %s %s \t %s",
						dname.to_chr(),id,cell,flags,icon,caption,tooltip);
				}
				else if (0==lstrcmp(type, "treeview")) { 
					const char *flags = (tFlags) ? tFlags : "a";
					char buffer [100];
					char * pathx = 0;
					wsprintf (buffer, "%s %i",itemPath,item);
					pathx = buffer;
					mIRCcomEX("//xdid -a %s %i %s \t +%s %s %s 0 %s %s %s %s %s \t %s",
						dname.to_chr(),id,pathx,flags,icon,icon,state,integral,colour,bgcolour,caption,tooltip);
					parseItems(child,depth,pathx);
				}
			}
		}
	}
	void parseDialog(int depth=0,const char *claPath = "root",int passedid = 2000) { 
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
			parent = child->Parent()->ToElement();
			element = child->ToElement();
			
			//STEP 2: PARSE ATTRIBUTES OF ELEMENTS
			parseAttributes();
			
			//dont itterate over unneccessary items
			if ((0==lstrcmp(elem, "control")) || (0==lstrcmp(elem, "pane"))) cCla++;
			else continue;
			//asign ID 
			if (0==lstrcmp(elem, "control")) { 
				controls++;
				 id = (element->QueryIntAttribute("id",&id) == TIXML_SUCCESS) ? id : 2000 - controls;
			}
			else id = passedid;

			//assign parent CONTROL of element
			while (parent) { 
				if (0==lstrcmp(parentelem, "pane")) { 
					parent = parent->Parent()->ToElement();
					parentelem = parent->Value();
				}
				else break;	
			}
			parenttype = (temp = parent->Attribute("type")) ? temp : "panel";
			parentid = (parent->QueryIntAttribute("id",&parentid) == TIXML_SUCCESS) ? parentid : passedid;
			
			//STEP 3: IF CONTROL CREATE IT AND ITS ITEMS
			if (0==lstrcmp(elem, "control")) {
				control++;
				//check how to insert the control in the parent Control/Dialog
				//If parentNode is pane loop untill parentNode is not a pane
				if (0==lstrcmp(parentelem, "dialog")) { 
					mIRCcomEX("//xdialog -c %s %i %s 0 0 %s %s %s",dname.to_chr(),id,type,width,height,styles);
				}
				else if (0==lstrcmp(parentelem, "control")) { 
					if (0==lstrcmp(parenttype, "panel"))
						mIRCcomEX("//xdid -c %s %i %i %s 0 0 %s %s %s", dname.to_chr(),parentid,id,type,width,height,styles);

					else if (0==lstrcmp(parenttype, "tab"))
						mIRCcomEX("//xdid -a %s %i 0 0 %s $chr(9) %i %s 0 0 %s %s %s $chr(9) %s",
							dname.to_chr(),parentid,caption,id,type,width,height,styles,tooltip);

					else if (((0==lstrcmp(parenttype, "pager")) || (0==lstrcmp(parenttype, "box"))) && (control == 1))
						mIRCcomEX("//xdid -c %s %i %i %s 0 0 %s %s %s", dname.to_chr(),parentid,id,type,width,height,styles);

					else if (0==lstrcmp(parenttype, "divider") && (control <= 2)) {
							if (control == 1)
								mIRCcomEX("//xdid -l %s %i %s 0 $chr(9) %i %s 0 0 %s %s %s", dname.to_chr(),parentid,width,id,type,width,height,styles);
							else if (control == 2)
								mIRCcomEX("//xdid -r %s %i %s 0 $chr(9) %i %s 0 0 %s %s %s", dname.to_chr(),parentid,width,id,type,width,height,styles);
					}

					else if (0==lstrcmp(parenttype, "rebar")) { 
						const char *flags = (tFlags) ? tFlags : "ceg";
						mIRCcomEX("//xdid -a %s %i 0 +%s %s %s %s %s %s %s \t %i %s 0 0 %s %s %s $chr(9) %s",
							dname.to_chr(),parentid,flags,rebarMinWidth,rebarMinHeight,width,icon,colour,caption,
							id,type,width,height,styles,tooltip);
					}
					else if (0==lstrcmp(parenttype, "stacker")) 
						mIRCcomEX("//xdid -a %s %i 0 + %s %s %s \t %i %s 0 0 %s %s %s",
							dname.to_chr(),parentid,colour,bgcolour,caption,id,type,width,height,styles);
					
					else if (0==lstrcmp(parenttype, "statusbar"))
						mIRCcomEX("//xdid -t %s %i %i +c %s %i %s 0 0 0 0 %s",
							dname.to_chr(),parentid,cell,icon,id,type,styles);
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
			parseDialog(depth+1,claPathx.to_chr(),id);  
		}
	} 

protected:

};
/* /dcxml -[d|p]  */
mIRC(dcxml) {
	TString input(data);
	int numtok = input.numtok( );
	
	if (numtok < 3) {
		DCXError("/dcxml", "Insuffient parameters");
		return 1;
	}

	TString flags(input.gettok( 1 ));
	flags.trim();

	if ((flags[0] != '-') || (flags.len() < 2)) {
		DCXError("/dcxml","No Flags Found");
		return 0;
	}

	if ((flags.find('p', 0) == 0) && (flags.find('d', 0) == 0)) { 
		DCXError("/dcxml","Unknown Flags");
		return 0;
	}
	DCXML oDcxml;
	TiXmlBase::SetCondenseWhiteSpace( false ); 
	TiXmlDocument doc(input.gettok(2,"\"").to_chr());
	bool dcxmlFile = doc.LoadFile();
	if (!dcxmlFile) { 
		DCXError("/dcxml","Could Not Load File");
		return 0;
	}
	oDcxml.root = doc.FirstChildElement("dcxml");
	if (!oDcxml.root) {
		DCXError("/dcxml","Root Element Is Not DCXML");
		return 0;
	}
	
	if (flags.find('d', 0)) { 

		oDcxml.dialogs = oDcxml.root->FirstChildElement("dialogs");
		if (!oDcxml.dialogs) { 
			DCXError("/dcxml","No Dialogs Group");
			return 0;
		}
		for( oDcxml.element = oDcxml.dialogs->FirstChildElement("dialog"); oDcxml.element; oDcxml.element = oDcxml.element->NextSiblingElement() ) {
			const char *name = oDcxml.element->Attribute("name");
			if (0==strcmp(name, input.gettok(2," ").to_chr())) { 
				oDcxml.dialog = oDcxml.element;
				break;
			}
		}
		if (!oDcxml.dialog) { 
			DCXError("/dcxml","Dialog name not found in <dialogs>");
			return 0;
		}
		oDcxml.dname = input.gettok(3," ").to_chr();
		oDcxml.temp = oDcxml.dialog->Attribute("cascade");
		const char *cascade = (oDcxml.temp = oDcxml.dialog->Attribute("cascade")) ? oDcxml.temp : "v";
		oDcxml.temp = oDcxml.dialog->Attribute("margin");
		const char *margin = (oDcxml.temp = oDcxml.dialog->Attribute("margin")) ? oDcxml.temp : "0 0 0 0";
		const char *caption = (oDcxml.temp = oDcxml.dialog->Attribute("caption")) ? oDcxml.temp : oDcxml.dname.to_chr();
		const char *border = oDcxml.dialog->Attribute("border");
		
		if (border) mIRCcomEX("/xdialog -b %s +%s", oDcxml.dname.to_chr(), border);
		if (caption) mIRCcomEX("/dialog -t %s %s",oDcxml.dname.to_chr(), caption);
		mIRCcomEX("/xdialog -l %s root \t +p%s 0 0 0 0",oDcxml.dname.to_chr(), cascade);
		mIRCcomEX("/xdialog -l %s space root \t + %s", oDcxml.dname.to_chr(), margin);
		oDcxml.parseDialog();
		mIRCcomEX("/.timer 1 0 xdialog -l %s update",oDcxml.dname);
		mIRCcomEX("/xdialog -z %s +s 1",oDcxml.dname.to_chr());

		return 1;
	}
	return 1;
}

