#ifndef DCXMLPARSER_H
#define DCXMLPARSER_H

#include "dcxmlincludes.h"

class DcxmlParser {

public:
	//constructor
	DcxmlParser() = default;
	~DcxmlParser() = default;

	DcxmlParser(const DcxmlParser &) = delete;
	DcxmlParser &operator =(const DcxmlParser &) = delete;	// No assignments!

	bool ParseXML(const TString &tsFilePath, const TString &tsDialogMark,const TString &DialogName,const bool verbose, const bool autoClose);
	void parseAttributes();
	void parseAttributes(const TiXmlElement *const tElement);
	void parseControl();
	TString parseCLA(const int cCla);

	void setStyle(const TiXmlElement *const style);
	void parseStyle(int depth = 0);
	void parseIcons(int depth = 0);
	void parseItems(const TiXmlElement *const element, const UINT depth = 0, const char *const itemPath = "");
	void parseTemplate(const UINT dialogDepth=0, const char *const claPath = "root", const UINT passedid = 2000);
	void parseDialog(const UINT depth=0,const char *claPath = "root",const UINT passedid = 2000,const bool ignoreParent = false);

	void setDialog(const TString &tsDialogMark);
	void setDialogMark (const TString &v) { m_tsDialogMark = v; }
	void setDialogName (const TString &v) { m_tsDialogName = v; }

	void setRootElement (const TiXmlElement *const ti) noexcept { m_pRootElement = ti; }
	void setDialogsElement (const TiXmlElement *const ti) noexcept { m_pDialogsElement = ti; }
	void setDialogElement (const TiXmlElement *const ti) noexcept { m_pDialogElement = ti; }

	void loadDocument();
	void loadDialog();
	void loadDialogElement();

	void setZlayered (const bool b) noexcept { m_bZlayered = b; }

	DcxDialog *getDialog () const noexcept { return m_pDcxDialog; }

	const TString &getDialogMark () const noexcept { return m_tsDialogMark; }
	const TString &getDialogName () const noexcept { return m_tsDialogName; }
	const TString &getFilePath () const noexcept { return m_tsFilePath; }

	const TiXmlElement* getRootElement () const noexcept { return m_pRootElement; }
	const TiXmlElement* getDialogsElement () const noexcept { return m_pDialogsElement; }
	const TiXmlElement* getDialogElement () const noexcept { return m_pDialogElement; }

	const TiXmlDocument * getDocument () const noexcept { return &m_xmlDocument; }

	const bool &getLoaded() const noexcept { return m_bLoadSuccess; }
	const bool &getZlayered() const noexcept { return m_bZlayered; }

private:

	void isVerbose(const bool b) noexcept { m_bVerbose = b; }
	void isAutoClose(const bool b) noexcept { m_bAutoClose = b; }
	void setFilePath(const TString &tsFile) { m_tsFilePath = tsFile; }

	const bool &isVerbose() const noexcept { return m_bVerbose; }
	const bool &isAutoClose() const noexcept { return m_bAutoClose; }

	int mIRCEvalToUnsignedInt(const TString &value);
	int parseId(const TiXmlElement *const idElement);

	void registerId(const TiXmlElement *const idElement, const int iNewID);

	void xml_xdialog(const TCHAR *const sSwitch, const TCHAR *const sArgs);
	void xml_xdid(const int cid, const TCHAR *const sSwitch, const TCHAR *const sArgs);

	void xdialogEX(const TCHAR *const sw, const TCHAR *const dFormat, ...);
	void xdidEX(const int cid, const TCHAR *const sw, const TCHAR *const dFormat, ...);

	const TiXmlElement *m_pElement; //!< current Element
	const TiXmlElement *m_pParent; //!< current Element's m_pParent
	int m_iControls; //!< Simple counter for controls

	//Attribute vars
	int m_iID;
	int m_iParentID;
	const char *m_sElem;
	const char *m_sParentelem;
	const char *m_sParenttype;
	const char *m_sType;
	const char *m_sSTclass;
	const char *m_sWeight;
	const char *m_sHeight;
	const char *m_sDropdown;
	const char *m_sWidth;
	const char *m_sMargin;
	const char *m_sStyles;
	const char *m_sCaption;
	const char *m_sTooltip;
	const char *m_sCascade;
	const char *m_sIcon;
	const char *m_sTFlags;
	const char *m_sIntegral;
	const char *m_sState;
	const char *m_sIndent;
	const char *m_sSrc;
	const char *m_sCells;
	const char *m_sRebarMinHeight;
	const char *m_sRebarMinWidth;
	const char *m_sIconsize;
	const char *m_sFontstyle;
	const char *m_sCharset;
	const char *m_sFontsize;
	const char *m_sFontname;
	const char *m_sBorder;
	const char *m_sCursor;
	const char *m_sBgcolour;
	const char *m_sTextbgcolour;
	const char *m_sTextcolour;
	const char *m_sGradientstart;
	const char *m_sGradientend;
	const char *m_sDisabledsrc;
	const char *m_sHoversrc;
	const char *m_sSelectedsrc;
	/*
	const char *elementProperties [] = "type","STclass","weigth","height","width","margin","styles","caption","tooltip",
	"cascade","icon","tFlags""integral","state","indent","src",	"cells","rebarMinHeight",
	"rebarMinWidth","iconsize","fontstyle","charset","fontsize","fontname","border","cursor","bgcolour",
	"textbgcolour","textcolour","gradientstart","gradientend","disabledsrc","hoversrc","selectedsrc"
	];*/
	const TiXmlElement* m_pTemplateRef;
	int m_iTemplateRefcCla;
	const char *m_sTemplateRefclaPath;
	std::map<const char*, const char*> m_mTemplate_vars;

	int m_iEval;

	//tempvar to dump attribute values in;
	//const char *m_sTemp;

	//CLA variables
	const char *g_claPath;
	const char *g_claPathx;
	int g_resetcla;

	bool m_bLoadSuccess;
	bool m_bVerbose;
	bool m_bAutoClose;
	bool m_bZlayered;

	TString m_tsFilePath;
	TString m_tsDialogMark;
	TString m_tsDialogName;
	DcxDialog* m_pDcxDialog;
	const TiXmlElement *m_pRootElement;
	const TiXmlElement *m_pDialogsElement;
	const TiXmlElement *m_pDialogElement;
	TiXmlDocument m_xmlDocument;
};
#endif
