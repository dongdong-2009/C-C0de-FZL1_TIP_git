#include "..\stdAfx.h"

#include "Resourcesrcxmlobj.h"

#include "..\TA_MessageLibrary.h"

ResourceSrcXMLObj::ResourceSrcXMLObj(void)
: m_spDOMDoc(NULL)
{
	m_sRCSource.Empty();
	m_sHeaderSource.Empty();
	m_sStringTableSegment.Empty();
}

ResourceSrcXMLObj::~ResourceSrcXMLObj(void)
{
	if(m_spDOMDoc)
	{
		m_spDOMDoc.Release();
		m_spDOMDoc = NULL;
	}
}

bool ResourceSrcXMLObj::LoadSourceDoc()
{
	CLSID clsid;
	HRESULT hr = CLSIDFromProgID(_T("MSXML2.DOMDocument"), &clsid);
	if(FAILED(hr))
		return false;

	hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, _uuidof(MSXML2::IXMLDOMDocument2), (void**)&m_spDOMDoc); 
	if(FAILED(hr))
		return false;

	//m_spDOMDoc->setProperty("SelectionLanguage", "XPath");
//	m_spDOMDoc->setProperty("preserveWhiteSpace", VARIANT_TRUE);

	if(!m_spDOMDoc->load((LPCTSTR)GetApp()->m_sResourceSrcPath))
	{

		MSXML2::IXMLDOMParseErrorPtr spError =  m_spDOMDoc->GetparseError();

		CString sError;
		sError.Format(_T("Error:\nCode = 0x%x\nSource = Line : %ld; Char : %ld\nError Description = %s\n"),
			spError->errorCode, spError->linepos, (LPCTSTR)spError->reason);

		AfxMessageBox(sError);

		return false;
	}

	m_spResIDsNode = m_spDOMDoc->selectSingleNode(_T("/") XMLNODE_SRC_ROOT _T("/") XMLNODE_RC_SRC_IDENTIFIERS);

	return LoadSourceStrings();
}

bool ResourceSrcXMLObj::LoadSourceStrings()
{
	//MSXML2::IXMLDOMCDATASectionPtr spData = 
	MSXML2::IXMLDOMNodePtr spNode = 
		m_spDOMDoc->selectSingleNode(_T("/") XMLNODE_SRC_ROOT _T("/") XMLNODE_SRC_RC _T("/") XMLNODE_SRC_DATA);
	MSXML2::IXMLDOMCDATASectionPtr spData = spNode->GetfirstChild();
	m_sRCSource = (LPCTSTR)spData->Getdata();

	spNode = m_spDOMDoc->selectSingleNode(_T("/") XMLNODE_SRC_ROOT _T("/") XMLNODE_SRC_HEADER _T("/") XMLNODE_SRC_DATA);
	spData = spNode->GetfirstChild();
	m_sHeaderSource = (LPCTSTR)spData->Getdata();

	spNode = m_spDOMDoc->selectSingleNode(_T("/") XMLNODE_SRC_ROOT _T("/") XMLNODE_SRC_STRING_TABLE _T("/") XMLNODE_SRC_DATA);
	spData = spNode->GetfirstChild();
	m_sStringTableSegment = (LPCTSTR)spData->Getdata();

	return true;
}

bool ResourceSrcXMLObj::GetLanguageIdentifier(LCID lcidLanguage, CString& sOutput, LPCTSTR sSubQuery)
{
	CString sQuery;
	sQuery.Format(XPATH_RC_SRC_IDENTIFIER, lcidLanguage);
	_variant_t vtValue;

	MSXML2::IXMLDOMNodePtr spID = m_spResIDsNode->selectSingleNode((LPCTSTR)sQuery);
	
	MSXML2::IXMLDOMNodePtr spItem = spID->selectSingleNode(sSubQuery);
	if(spItem == NULL)
	{
		CString sError;
		sError.Format(_T("Failed to find language identifer '%s'"), sSubQuery);
		AfxMessageBox(sError);

		return false;
	}

	vtValue = spItem->GetnodeTypedValue();
	sOutput = vtValue.bstrVal;

	return true;
}

