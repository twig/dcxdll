#include "../../defines.h"
#include "../tinyxml/tinyxml.h"
#include "../dcxdialog.h"
#include "../dcxdialogcollection.h"
#include "../dcxcontrol.h"
#include <map>
#include "../xpopup/xpopupmenumanager.h"


class DCXML {
public:
	//constructor
	DCXML();
	DCXML(const char *filePath, const char *dialogMark,const char *dialogName);
	//virtual ~DCXML( );
	void parseAttributes();
	void parseAttributes(TiXmlElement* element);
	void parseControl();
	TString parseCLA(const int cCla);
	bool loadSuccess;

	void setStyle(TiXmlElement* style);
	void parseStyle(int depth = 0);
	void parseIcons(int depth = 0);
	void parseItems(TiXmlElement* element,int depth = 0,char *itemPath = "");
	void parseTemplate(int dialogDepth=0,const char *claPath = "root",int passedid = 2000);
	void parseDialog(int depth=0,const char *claPath = "root",int passedid = 2000,int ignoreParent = 0);
	
	const char *queryAttribute(TiXmlElement *element,const char *attribute,const char *defaultValue = "");
	int queryIntAttribute(TiXmlElement *element,const char *attribute,int defaultValue = 0);

	void setDialog(const char *dialogMark);
	void setDialogMark (const char *v) { this->_dialogMark = v; }
	void setDialogName (const char *v) { this->_dialogName = v; }
	void setRootElement (TiXmlElement * element) { this->_rootElement = element; }
	void setDialogsElement (TiXmlElement * element) { this->_dialogsElement = element; }
	void setDialogElement (TiXmlElement * element) { this->_dialogElement = element; }
	void setDocument (const char *filePath) 
	{
		TiXmlBase::SetCondenseWhiteSpace( false ); 
		TiXmlDocument doc(filePath);
		this->loadSuccess = doc.LoadFile();
		this->_document = doc; 
	}
	void setZlayered (bool b) { this->_zlayered = b; }

	DcxDialog *getDialog () { return this->_dcxDialog; }
	bool getZlayered () { return this->_zlayered; }
	const char *getDialogMark () { return this->_dialogMark; }
	const char *getDialogName () { return this->_dialogName; }

	TiXmlElement* getRootElement () { return this->_rootElement; }
	TiXmlElement* getDialogsElement () { return this->_dialogsElement; }
	TiXmlElement* getDialogElement () { return this->_dialogElement; }
	
	TiXmlDocument * getDocument () { return &this->_document; }

    DcxDialog *d_Host;
    TiXmlElement *root; //!< dcxml root element
    TiXmlElement *dialogs; //!< dcxml dialogs collection element
    TiXmlElement *dialog; //!< dialog element
    TiXmlElement *element; //!< current Element
    TiXmlElement *parent; //!< current Element's parent
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
	/*
	const char *elementProperties [] = "type","STclass","weigth","height","width","margin","styles","caption","tooltip",
		"cascade","icon","tFlags""integral","state","indent","src",	"cells","rebarMinHeight",
		"rebarMinWidth","iconsize","fontstyle","charset","fontsize","fontname","border","cursor","bgcolour",
		"textbgcolour","textcolour","gradientstart","gradientend","disabledsrc","hoversrc","selectedsrc"
	];*/
    TiXmlElement* templateRef;
    int templateRefcCla;
    const char *templateRefclaPath;
	std::map<const char*, const char*> template_vars;

    int eval;
    
    //tempvar to dump attribute values in;
    const char *temp;
    TString cmd;
	
    //CLA variables
    const char *g_claPath;
    const char *g_claPathx;
    int g_resetcla;
private:
	void xdialogEX(const char *sw,const char *dFormat, ...);
	int mIRCEvalToUnsignedInt (const char *value);
	void registerId(TiXmlElement *idElement,int id);
	int parseId(TiXmlElement* idElement);
	void xdidEX(int id,const char *sw,const char *dFormat, ...);
	
	//TiXmlElement *getDialogElement () { return this->_dialogElement; }
	typedef std::map<const char, const char> AttributesMap;

	AttributesMap _attributesMap;

	bool _zlayered;
	const char *_dialogMark, *_dialogName;
	DcxDialog* _dcxDialog;
	TiXmlElement *_rootElement;
	TiXmlElement *_dialogsElement;
	TiXmlElement *_dialogElement;
	TiXmlDocument _document;
};