#ifndef DCXMLPARSER_H
#define DCXMLPARSER_H

#include "dcxmlincludes.h"

#define DCXML_ID_OFFSET 2000U

struct DcxmlControlAttributes final
{
	UINT m_iID{};
	UINT m_iParentID{};
	int m_iEval{};

	const char* m_sElem{ nullptr };
	const char* m_sParentelem{ nullptr };
	const char* m_sParenttype{ nullptr };
	const char* m_sType{ nullptr };
	const char* m_sSTclass{ nullptr };
	const char* m_sWeight{ nullptr };
	const char* m_sHeight{ nullptr };
	const char* m_sDropdown{ nullptr };
	const char* m_sWidth{ nullptr };
	const char* m_sMargin{ nullptr };
	const char* m_sStyles{ nullptr };
	const char* m_sCaption{ nullptr };
	const char* m_sTooltip{ nullptr };
	const char* m_sCascade{ nullptr };
	const char* m_sIcon{ nullptr };
	const char* m_sTFlags{ nullptr };
	const char* m_sIntegral{ nullptr };
	const char* m_sState{ nullptr };
	const char* m_sIndent{ nullptr };
	const char* m_sSrc{ nullptr };
	const char* m_sCells{ nullptr };
	const char* m_sRebarMinHeight{ nullptr };
	const char* m_sRebarMinWidth{ nullptr };
	const char* m_sIconsize{ nullptr };
	const char* m_sFontstyle{ nullptr };
	const char* m_sCharset{ nullptr };
	const char* m_sFontsize{ nullptr };
	const char* m_sFontname{ nullptr };
	const char* m_sBorder{ nullptr };
	const char* m_sCursor{ nullptr };
	const char* m_sBgcolour{ nullptr };
	const char* m_sTextbgcolour{ nullptr };
	const char* m_sTextcolour{ nullptr };
	const char* m_sGradientstart{ nullptr };
	const char* m_sGradientend{ nullptr };
	const char* m_sDisabledsrc{ nullptr };
	const char* m_sHoversrc{ nullptr };
	const char* m_sSelectedsrc{ nullptr };
	const char* m_sBuddyID{ nullptr };
};

struct DcxmlControlTemplates final
{
	const TiXmlElement* m_pTemplateRef{ nullptr };
	int									m_iTemplateRefcCla{};
	const char* m_sTemplateRefclaPath{ nullptr };
	std::map<const char*, const char*>	m_mTemplate_vars;
};

struct DcxmlCLA final
{
	const char* m_claPath{ nullptr };
	const char* m_claPathx{ nullptr };
	bool m_bResetCLA{ false };
};

class DcxmlParser final
{
public:
	//constructor
	DcxmlParser() noexcept {};
	~DcxmlParser() noexcept = default;

	DcxmlParser(const DcxmlParser&) = delete;
	DcxmlParser(DcxmlParser&&) = delete;
	DcxmlParser& operator =(const DcxmlParser&) = delete;
	DcxmlParser& operator =(DcxmlParser&&) = delete;

	bool ParseXML(const TString& tsFilePath, const TString& tsDialogMark, const TString& DialogName, const bool verbose, const bool autoClose);
	void parseAttributes() noexcept;
	void parseAttributes(const TiXmlElement* const tElement) noexcept;
	void parseControl();
	TString parseCLA(const int cCla);

	void setStyle(const TiXmlElement* const style);
	void parseStyle(int depth = 0);
	void parseIcons(int depth = 0);
	void parseItems(const TiXmlElement* const element, const UINT depth = 0, const char* const itemPath = "");
	void parseTemplate(const UINT dialogDepth = 0, const char* const claPath = "root", const UINT passedid = DCXML_ID_OFFSET);
	void parseDialog(const UINT depth = 0, const char* claPath = "root", const UINT passedid = DCXML_ID_OFFSET, const bool ignoreParent = false);

	void setDialog(const TString& tsDialogMark) noexcept;
	void setDialogMark(const TString& v) { m_tsDialogMark = v; }
	void setDialogName(const TString& v) { m_tsDialogName = v; }

	void setRootElement(const TiXmlElement* const ti) noexcept { m_pRootElement = ti; }
	void setDialogsElement(const TiXmlElement* const ti) noexcept { m_pDialogsElement = ti; }
	void setDialogElement(const TiXmlElement* const ti) noexcept { m_pDialogElement = ti; }

	void loadDocument();
	void loadDialog();
	void loadDialogElement();

	void setZlayered(const bool b) noexcept { m_bZlayered = b; }

	GSL_SUPPRESS(lifetime.1) DcxDialog* getDialog() const noexcept { return m_pDcxDialog; }

	const TString& getDialogMark() const noexcept { return m_tsDialogMark; }
	const TString& getDialogName() const noexcept { return m_tsDialogName; }
	const TString& getFilePath() const noexcept { return m_tsFilePath; }

	GSL_SUPPRESS(lifetime.1) const TiXmlElement* getRootElement() const noexcept { return m_pRootElement; }
	GSL_SUPPRESS(lifetime.1) const TiXmlElement* getDialogsElement() const noexcept { return m_pDialogsElement; }
	GSL_SUPPRESS(lifetime.1) const TiXmlElement* getDialogElement() const noexcept { return m_pDialogElement; }

	//GSL_SUPPRESS(lifetime.1) const TiXmlDocument * getDocument () const noexcept { return &m_xmlDocument; }
	GSL_SUPPRESS(lifetime.1) gsl::strict_not_null<const TiXmlDocument*> getDocument() const noexcept { return gsl::make_strict_not_null(&m_xmlDocument); }

	const bool& getLoaded() const noexcept { return m_bLoadSuccess; }
	const bool& getZlayered() const noexcept { return m_bZlayered; }

private:

	void isVerbose(const bool b) noexcept { m_bVerbose = b; }
	void isAutoClose(const bool b) noexcept { m_bAutoClose = b; }
	void setFilePath(const TString& tsFile) { m_tsFilePath = tsFile; }

	const bool& isVerbose() const noexcept { return m_bVerbose; }
	const bool& isAutoClose() const noexcept { return m_bAutoClose; }

	UINT parseId(const TiXmlElement* const idElement);

	void registerId(const TiXmlElement* const idElement, const UINT iNewID);

	void xml_xdialog(const TCHAR* const sSwitch, const TString& sArgs);
	void xml_xdid(const UINT cid, const TCHAR* const sSwitch, const TString& sArgs);

	void xdialogEX(const TCHAR* const sw, const TCHAR* const dFormat, ...);
	void xdidEX(const UINT cid, const TCHAR* const sw, const TCHAR* const dFormat, ...);

	const TiXmlElement* m_pElement{ nullptr }; //!< current Element
	const TiXmlElement* m_pParent{ nullptr }; //!< current Element's Parent
	UINT m_iControls{}; //!< Simple counter for controls

	//Attribute vars
	DcxmlControlAttributes m_Attributes;

	// Templates
	DcxmlControlTemplates m_Templates;

	//CLA variables
	DcxmlCLA m_CLA;

	bool m_bLoadSuccess{ false };
	bool m_bVerbose{ false };
	bool m_bAutoClose{ false };
	bool m_bZlayered{ false };

	TString m_tsFilePath;
	TString m_tsDialogMark;
	TString m_tsDialogName;
	DcxDialog* m_pDcxDialog{ nullptr };
	const TiXmlElement* m_pRootElement{ nullptr };
	const TiXmlElement* m_pDialogsElement{ nullptr };
	const TiXmlElement* m_pDialogElement{ nullptr };
	TiXmlDocument m_xmlDocument;
};
#endif
