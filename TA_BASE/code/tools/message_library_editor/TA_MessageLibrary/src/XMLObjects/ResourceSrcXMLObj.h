#pragma once

#include "XMLDefines.h"

class ResourceSrcXMLObj
{
public:

	MSXML2::IXMLDOMDocument2Ptr m_spDOMDoc;
	MSXML2::IXMLDOMNodePtr m_spResIDsNode;
	
	CString m_sRCSource, m_sHeaderSource, m_sStringTableSegment;

	ResourceSrcXMLObj(void);
	~ResourceSrcXMLObj(void);

	bool LoadSourceDoc();
	bool LoadSourceStrings();

	bool GetLanguageIdentifier(LCID lcidLanguage, CString& sOutput, LPCTSTR sSubQuery);

	inline bool GetLanguageDefinition(LCID lcidLanguage, CString& sOutput) 
	{ return GetLanguageIdentifier(lcidLanguage, sOutput, XMLNODE_RC_SRC_DEFINITION);}

	inline bool GetLanguageName(LCID lcidLanguage, CString& sOutput)
	{ return GetLanguageIdentifier(lcidLanguage, sOutput, XMLNODE_RC_SRC_NAME);}

	inline bool GetLanguageCodePage(LCID lcidLanguage, CString& sOutput)
	{ return GetLanguageIdentifier(lcidLanguage, sOutput, XMLNODE_RC_SRC_CODEPAGE);}

	inline bool GetLanguageAfxTarget(LCID lcidLanguage, CString& sOutput)
	{ return GetLanguageIdentifier(lcidLanguage, sOutput, XMLNODE_RC_SRC_AFXTARGET);}
};
