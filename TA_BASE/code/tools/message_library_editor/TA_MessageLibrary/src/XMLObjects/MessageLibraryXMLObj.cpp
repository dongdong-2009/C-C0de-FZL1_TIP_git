#include "..\stdafx.h"

#include <Lmcons.h>

#include "..\TA_MessageLibrary.h"
#include "MessageLibraryxmlobj.h"

const unsigned long maxResIDs = 65535; // max WORD

MessageLibraryXMLObj::MessageLibraryXMLObj(void)
: m_spDOMDoc(NULL), m_spMessageRoot(NULL), m_spParamRoot(NULL), m_IDsPool(65535)
{
}

MessageLibraryXMLObj::~MessageLibraryXMLObj(void)
{
	if(m_spDOMDoc)
	{
		m_spDOMDoc.Release();
		m_spDOMDoc = NULL;
	}
}

bool MessageLibraryXMLObj::CreateMessageNode(Message::enumMsgTypes MsgType, CString& sMessage, CString& sResID, 
									  CStringArray& rgsParameters, bool bObsolete, bool bAuditEvent)
{
	_variant_t vtValue;
	VARIANT vtNodeElement;
	vtNodeElement.intVal = MSXML2::NODE_ELEMENT;
	vtNodeElement.vt = VT_INT;

	_bstr_t bsNamespace = m_spMessageRoot->GetnamespaceURI();
	MSXML2::IXMLDOMNodePtr spMessage = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG, bsNamespace);
	
	MSXML2::IXMLDOMNodePtr spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_ID, bsNamespace);
	vtValue.uiVal = m_IDsPool.borrowID();
	vtValue.vt = VT_UI2;
	spChild->put_nodeTypedValue(vtValue);
	spMessage->appendChild(spChild);
    
    spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_RESID, bsNamespace);
	vtValue.Clear();
	vtValue = sResID;
	spChild->put_nodeTypedValue(vtValue);
	spMessage->appendChild(spChild);

   
	spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_TYPE, bsNamespace);
	vtValue.Clear();
	vtValue.uiVal = MsgType;
	vtValue.vt = VT_UI2;
	spChild->put_nodeTypedValue(vtValue);
	spMessage->appendChild(spChild);

	spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_HELPID, bsNamespace);
	vtValue.Clear();
	vtValue.ulVal = 0;
	vtValue.vt = VT_UI4;
	spChild->put_nodeTypedValue(vtValue);
	spMessage->appendChild(spChild);

	spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_STYLE, bsNamespace);
	vtValue.Clear();
	vtValue.vt = VT_UI4;
	switch(MsgType)
	{
		case Message::enumMsgTypes::InfoMessage: vtValue.ulVal = 64; break;
		case Message::enumMsgTypes::WarningMessage: vtValue.ulVal = 48; break;
		case Message::enumMsgTypes::ErrorMessage: vtValue.ulVal = 16; break;
	}
	
	spChild->put_nodeTypedValue(vtValue);
	spMessage->appendChild(spChild);

	spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_STRING, bsNamespace);
	vtValue.Clear();
	vtValue = (LPCTSTR)sMessage;
	spChild->put_nodeTypedValue(vtValue);
	
	MSXML2::IXMLDOMElementPtr spElement = spMessage->appendChild(spChild);
	vtValue.Clear();
	vtValue = (LPCTSTR)_T("3081");
	spElement->setAttribute(XMLNODE_MSG_LOCALE, vtValue);

	for(INT_PTR i = 0, iCount = rgsParameters.GetSize(); i < iCount; i++)
	{
		spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_PARAM, bsNamespace);
		vtValue.Clear();
		vtValue = (LPCTSTR)rgsParameters[i];
		spChild->put_nodeTypedValue(vtValue);

		spMessage->appendChild(spChild);
	}
	spElement = m_spMessageRoot->appendChild(spMessage);
	vtValue.Clear();
	vtValue = (LPCTSTR)(bObsolete) ? XMLVAL_TRUE : XMLVAL_FALSE;
	spElement->setAttribute(XMLNODE_MSG_OBSOLETE, vtValue);

	vtValue.Clear();
	vtValue = (LPCTSTR)(bAuditEvent) ? XMLVAL_TRUE : XMLVAL_FALSE;
	spElement->setAttribute(XMLNODE_MSG_AUDIT, vtValue);

	return true;
}

bool MessageLibraryXMLObj::CreateMessageNode(Message& msg)
{
	_variant_t vtValue;
	CString sTemp;
	LCID locale;
	VARIANT vtNodeElement;
	vtNodeElement.intVal = MSXML2::NODE_ELEMENT;
	vtNodeElement.vt = VT_INT;
	MSXML2::IXMLDOMElementPtr spElement = NULL;

	_bstr_t bsNamespace = m_spMessageRoot->GetnamespaceURI();
	MSXML2::IXMLDOMNodePtr spMessage = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG, bsNamespace);
	
	MSXML2::IXMLDOMNodePtr spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_ID, bsNamespace);
	vtValue.ulVal = msg.m_ulID;
	vtValue.vt = VT_UI2;
	spChild->put_nodeTypedValue(vtValue);
	spMessage->appendChild(spChild);

    spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_RESID, bsNamespace);
	vtValue.Clear();
	vtValue = msg.m_ResID;
	spChild->put_nodeTypedValue(vtValue);
	spMessage->appendChild(spChild);

	spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_TYPE, bsNamespace);
	vtValue.Clear();
	vtValue.uiVal = msg.m_usMessageType;
	vtValue.vt = VT_UI2;
	spChild->put_nodeTypedValue(vtValue);
	spMessage->appendChild(spChild);

	spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_HELPID, bsNamespace);
	vtValue.Clear();
	vtValue.ulVal = msg.m_ulHelpID;
	vtValue.vt = VT_UI4;
	spChild->put_nodeTypedValue(vtValue);
	spMessage->appendChild(spChild);

	spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_STYLE, bsNamespace);
	vtValue.Clear();
	vtValue.ulVal = msg.m_ulMsgBoxStyle;
	vtValue.vt = VT_UI4;
	spChild->put_nodeTypedValue(vtValue);
	spMessage->appendChild(spChild);
	
	POSITION pos = msg.m_mapMessages.GetStartPosition();
	while(pos)
	{
		msg.m_mapMessages.GetNextAssoc(pos, locale, sTemp);
		
		spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_STRING, bsNamespace);
		vtValue.Clear();
		vtValue = (LPCTSTR)sTemp;
		spChild->put_nodeTypedValue(vtValue);

		spElement = spMessage->appendChild(spChild);
		vtValue.Clear();
		sTemp.Format(_T("%d"), locale);
		vtValue = (LPCTSTR)sTemp;
		spElement->setAttribute(XMLNODE_MSG_LOCALE, vtValue);
	}

	for(INT_PTR i = 0, iCount = msg.m_rgsParameters.GetSize(); i < iCount; i++)
	{
		spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_PARAM, bsNamespace);
		vtValue.Clear();
		vtValue = (LPCTSTR)msg.m_rgsParameters[i];
		spChild->put_nodeTypedValue(vtValue);

		spMessage->appendChild(spChild);
	}
	
	// Insert the node
	spElement = m_spMessageRoot->appendChild(spMessage);

	vtValue.Clear();
	vtValue = (LPCTSTR)(msg.m_bObsolete) ? XMLVAL_TRUE : XMLVAL_FALSE;
	spElement->setAttribute(XMLNODE_MSG_OBSOLETE, vtValue);

	vtValue.Clear();
	vtValue = (LPCTSTR)(msg.m_bAuditEvent) ? XMLVAL_TRUE : XMLVAL_FALSE;
	spElement->setAttribute(XMLNODE_MSG_AUDIT, vtValue);

	return true;
}

bool MessageLibraryXMLObj::ModifyMessageNode(Message& msg)
{
	_variant_t vtValue;
	CString sQuery, sTemp;
	LCID locale;
	sQuery.Format(XPATH_MSG_USING_ID, msg.m_ulID);
	_bstr_t bsNamespace = m_spMessageRoot->GetnamespaceURI();
	VARIANT vtNodeElement;
	vtNodeElement.intVal = MSXML2::NODE_ELEMENT;
	vtNodeElement.vt = VT_INT;

	MSXML2::IXMLDOMNodePtr spChild = NULL;
	MSXML2::IXMLDOMNodePtr spMessage = m_spMessageRoot->selectSingleNode((LPCTSTR)sQuery);
	if(spMessage == NULL)
		return false;

	spChild = spMessage->selectSingleNode(XMLNODE_MSG_RESID);
    if(spChild == NULL)
        return false;
    sTemp = msg.m_ResID;
    vtValue = (LPCTSTR)sTemp;
    spChild->PutnodeTypedValue(vtValue);

    spChild = spMessage->selectSingleNode(_T("@") XMLNODE_MSG_OBSOLETE);
	if(spChild == NULL)
		return false;
	vtValue.Clear();
    sTemp = (msg.m_bObsolete) ? XMLVAL_TRUE : XMLVAL_FALSE;
	vtValue = (LPCTSTR)sTemp;
	spChild->PutnodeTypedValue(vtValue);

	spChild = spMessage->selectSingleNode(_T("@") XMLNODE_MSG_AUDIT);
	if(spChild == NULL)
		return false;
	sTemp = (msg.m_bAuditEvent) ? XMLVAL_TRUE : XMLVAL_FALSE;
	vtValue = (LPCTSTR)sTemp;
	spChild->PutnodeTypedValue(vtValue);

	spChild = spMessage->selectSingleNode(XMLNODE_MSG_TYPE);
	if(spChild == NULL)
		return false;
	vtValue.Clear();
	vtValue.uiVal = msg.m_usMessageType;
	vtValue.vt = VT_UI2;
	spChild->put_nodeTypedValue(vtValue);

	spChild = spMessage->selectSingleNode(XMLNODE_MSG_HELPID);
	if(spChild == NULL)
		return false;
	vtValue.Clear();
	vtValue.ulVal = msg.m_ulHelpID;
	vtValue.vt = VT_UI4;
	spChild->put_nodeTypedValue(vtValue);

	spChild = spMessage->selectSingleNode(XMLNODE_MSG_STYLE);
	if(spChild == NULL)
		return false;
	vtValue.Clear();
	vtValue.ulVal = msg.m_ulMsgBoxStyle;
	vtValue.vt = VT_UI4;
	spChild->put_nodeTypedValue(vtValue);

	// Strings
	POSITION pos = msg.m_mapMessages.GetStartPosition();
	while(pos)
	{
		msg.m_mapMessages.GetNextAssoc(pos, locale, sTemp);
		
		sQuery.Format(XPATH_MSG_STR_USING_LOCALE, locale);
		spChild = spMessage->selectSingleNode((LPCTSTR)sQuery);
		if(spChild == NULL)
			return false;
		vtValue.Clear();
		vtValue = (LPCTSTR)sTemp;
		spChild->put_nodeTypedValue(vtValue);
	}

	// Parameters
	// Remove existing and then add back in
	MSXML2::IXMLDOMNodeListPtr spParamList = spMessage->selectNodes(XMLNODE_MSG_PARAM);
	for(long l = 0, lCount = spParamList->Getlength(); l < lCount; l++)
		spMessage->removeChild(spParamList->Getitem(l));

	for(INT_PTR i = 0, iCount = msg.m_rgsParameters.GetSize(); i < iCount; i++)
	{
		spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_PARAM, bsNamespace);
		vtValue.Clear();
		vtValue = (LPCTSTR)msg.m_rgsParameters[i];
		spChild->put_nodeTypedValue(vtValue);

		spMessage->appendChild(spChild);
	}

	return true;
}

HRESULT MessageLibraryXMLObj::LoadMessageDoc(LPCTSTR strPath)
{
	CLSID clsid;
	HRESULT hr = CLSIDFromProgID(_T("MSXML2.DOMDocument"), &clsid);
	if(FAILED(hr))
		return hr;

	// if not already created
    if (0 == m_spDOMDoc)
    {
        hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, _uuidof(MSXML2::IXMLDOMDocument2), (void**)&m_spDOMDoc); 
	    if(FAILED(hr))
		    return hr;
    }

	
	if(!m_spDOMDoc->load(strPath))
	{

		MSXML2::IXMLDOMParseErrorPtr spError =  m_spDOMDoc->GetparseError();
		CString sError;
		hr = spError->errorCode;

		switch(hr)
		{
			case INET_E_RESOURCE_NOT_FOUND:
			{
				sError.Format(IDS_APP_XMLDOC_MISSING, strPath);
			}break;
			default:
				sError.Format(IDS_APP_XMLDOC_ERROR, strPath, hr, spError->line, spError->linepos, 
								(LPCTSTR)spError->reason);
		}
		AfxMessageBox(sError, MB_ICONERROR);

		return spError->errorCode;
	}

    m_IDsPool.returnAllIDs();

	m_spMessageRoot = m_spDOMDoc->selectSingleNode(XMLNODE_ROOT _T("/") XMLNODE_MSG_ROOT);
	m_spParamRoot = m_spDOMDoc->selectSingleNode(XMLNODE_ROOT _T("/") XMLNODE_PARAM_ROOT);
	
	GetSupportedLocales();

    // go through the messages, extract their IDs and book them out
    // of the IDs pool
    MSXML2::IXMLDOMNodeListPtr spMessages = m_spMessageRoot->selectNodes(XMLNODE_MSG);
    for(long i = 0, iCount = spMessages->Getlength(); i < iCount; i++)
	{
        MSXML2::IXMLDOMNodePtr spChild = spMessages->Getitem(i)->selectSingleNode(XMLNODE_MSG_ID);
	    _variant_t vtValue = spChild->GetnodeTypedValue();
	    CString sID = vtValue.bstrVal;
        if (! m_IDsPool.borrowID(_ttol(sID)))
        {
            CString err;
            err.Format(_T("The document contains either duplicate or out of range IDs: %ld"),
                _ttol(sID));
            AfxMessageBox(err, MB_ICONERROR);
            return E_INVALIDARG;
        }   
    }

    return S_OK;
}

void MessageLibraryXMLObj::TruncateMessageDoc(void)
{
    ASSERT(0 != m_spMessageRoot);
    ASSERT(0 != m_spParamRoot);

	long l = 0;
	MSXML2::IXMLDOMNodeListPtr spChildren = m_spMessageRoot->selectNodes(XMLNODE_MSG);
	for(l  = spChildren->Getlength() - 1; l  >= 0; l --)
    {
		m_spMessageRoot->removeChild(spChildren->Getitem(l));
    }

	spChildren = m_spParamRoot->selectNodes(XMLNODE_PARAM);
	for(l  = spChildren->Getlength() - 1; l  >= 0; l --)
    {
		m_spParamRoot->removeChild(spChildren->Getitem(l));
    }

    m_IDsPool.returnAllIDs();
}

bool MessageLibraryXMLObj::SaveMessageDoc()
{
	CString sError;
	MessageLibraryApp* pApp = GetApp();

	try
	{
		HRESULT hr = m_spDOMDoc->save((LPCTSTR)pApp->m_sXMLSrcPath);

		if(FAILED(hr))
		{
			MSXML2::IXMLDOMParseErrorPtr spError =  m_spDOMDoc->GetparseError();

			
			sError.Format(IDS_APP_XMLDOC_SAVE_ERROR, pApp->m_sXMLSrcPath, spError->errorCode, spError->line, 
							spError->linepos, (LPCTSTR)spError->reason);

			AfxMessageBox(sError);

			return false;
		}
	}
	catch(_com_error e)
	{
		sError.Format(IDS_APP_XMLDOC_SAVE_EXCEPTION, pApp->m_sXMLSrcPath, e.Error(), e.ErrorMessage());
		AfxMessageBox(sError);

		return false;
	}

	return true;	
}

bool MessageLibraryXMLObj::ConstructMessage(unsigned long MsgID, Message& message)
{
	long l = 0, lCount = 0;
	_variant_t vtValue, vtLocale;
	CString sTemp, sQuery;
	sQuery.Format(XPATH_MSG_USING_ID, MsgID);

	MSXML2::IXMLDOMNodePtr spMessage = m_spMessageRoot->selectSingleNode((LPCTSTR)sQuery);
	if(spMessage == NULL)
		return false;

	MSXML2::IXMLDOMNodePtr spChild = spMessage->selectSingleNode(_T("@") XMLNODE_MSG_OBSOLETE);
	if(spChild == NULL)
		message.m_bObsolete = false;
	else
	{
		vtValue = spChild->GetnodeTypedValue();
		sTemp = vtValue.bstrVal;
		message.m_bObsolete = (sTemp == XMLVAL_TRUE);
		vtValue.Clear();
	}

	spChild = spMessage->selectSingleNode(_T("@") XMLNODE_MSG_AUDIT);
	if(spChild == NULL)
		message.m_bAuditEvent = false;
	else
	{
		vtValue = spChild->GetnodeTypedValue();
		sTemp = vtValue.bstrVal;
		message.m_bAuditEvent = (sTemp == XMLVAL_TRUE);
		vtValue.Clear();
	}

	spChild = spMessage->selectSingleNode(XMLNODE_MSG_RESID);
	vtValue = spChild->GetnodeTypedValue();
	sTemp = vtValue.bstrVal;
	message.m_ResID = sTemp;

    vtValue.Clear();
    spChild = spMessage->selectSingleNode(XMLNODE_MSG_ID);
    vtValue = spChild->GetnodeTypedValue();
    sTemp = vtValue.bstrVal;
    message.m_ulID = _ttol(sTemp);


	vtValue.Clear();
	spChild = spMessage->selectSingleNode(XMLNODE_MSG_TYPE);
	vtValue = spChild->GetnodeTypedValue();
	sTemp = vtValue.bstrVal;
	message.m_usMessageType = _ttoi(sTemp);

	vtValue.Clear();
	spChild = spMessage->selectSingleNode(XMLNODE_MSG_HELPID);
	vtValue = spChild->GetnodeTypedValue();
	sTemp = vtValue.bstrVal;
	message.m_ulHelpID = _ttol(sTemp);

	vtValue.Clear();
	spChild = spMessage->selectSingleNode(XMLNODE_MSG_STYLE);
	vtValue = spChild->GetnodeTypedValue();
	sTemp = vtValue.bstrVal;
	message.m_ulMsgBoxStyle = _ttol(sTemp);

	vtValue.Clear();
	MSXML2::IXMLDOMElementPtr spString = NULL;
	LCID locale = 0;
	MSXML2::IXMLDOMNodeListPtr spStrings = spMessage->selectNodes(XMLNODE_MSG_STRING);
	for(l = 0, lCount = spStrings->Getlength(); l < lCount; l++)
	{
		vtValue.Clear();
		spString = spStrings->Getitem(l);
		vtLocale = spString->getAttribute(XMLNODE_MSG_LOCALE);
		sTemp = vtLocale.bstrVal;
		locale = _ttoi(sTemp);
		vtValue = spString->GetnodeTypedValue();
		sTemp = vtValue.bstrVal;
		message.m_mapMessages.SetAt(locale, sTemp);
	}
	
	MSXML2::IXMLDOMNodeListPtr spParams = spMessage->selectNodes(XMLNODE_MSG_PARAM);
	for(l = 0, lCount = spParams->Getlength(); l < lCount; l++)
	{
		vtValue.Clear();
		spChild = spParams->Getitem(l);
		vtValue = spChild->GetnodeTypedValue();
		sTemp = vtValue.bstrVal;
		message.m_rgsParameters.Add(sTemp);
	}

	return true;
}

bool MessageLibraryXMLObj::CreateParameterNode(const CString& sParameter, const CString& sDescription, 
											   const CString& sType, const bool bObsolete)
{
	_variant_t vtValue;
	VARIANT vtNodeElement;
	vtNodeElement.intVal = MSXML2::NODE_ELEMENT;
	vtNodeElement.vt = VT_INT;

	_bstr_t bsNamespace = m_spParamRoot->GetnamespaceURI();
	MSXML2::IXMLDOMElementPtr spParameter = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_PARAM, bsNamespace);

	MSXML2::IXMLDOMNodePtr spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_PARAM_NAME, bsNamespace);
	vtValue = (LPCTSTR)sParameter;
	spChild->PutnodeTypedValue(vtValue);
	spParameter->appendChild(spChild);

	spChild = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_PARAM_DESC, bsNamespace);
	vtValue.Clear();
	vtValue = (LPCTSTR)sDescription;
	spChild->PutnodeTypedValue(vtValue);
	spParameter->appendChild(spChild);

	vtValue.Clear();
	vtValue = (LPCTSTR)sType;
	spParameter->setAttribute(XMLNODE_PARAM_TYPE, vtValue);

	vtValue.Clear();
	vtValue = (LPCTSTR)((bObsolete) ? XMLVAL_TRUE : XMLVAL_FALSE);
	spParameter->setAttribute(XMLNODE_PARAM_OBSOLETE, vtValue);

	m_spParamRoot->appendChild(spParameter);

	return true;
}

bool MessageLibraryXMLObj::ModifyParameterNode(const CString& sParameter, const CString& sDescription, const CString& sType,
											   const bool bObsolete)
{
	_variant_t vtValue;
	CString sQuery;
	sQuery.Format(XPATH_PARAM_USING_NAME, sParameter);

	MSXML2::IXMLDOMElementPtr spParameter = m_spParamRoot->selectSingleNode((LPCTSTR)sQuery);
	if(spParameter == NULL)
		return false;

	// Name
	MSXML2::IXMLDOMNodePtr spChild = spParameter->selectSingleNode(XMLNODE_PARAM_NAME);
	vtValue = (LPCTSTR)sParameter;
	spChild->PutnodeTypedValue(vtValue);

	// Description
	spChild = spParameter->selectSingleNode(XMLNODE_PARAM_DESC);
	vtValue.Clear();
	vtValue = (LPCTSTR)sDescription;
	spChild->PutnodeTypedValue(vtValue);

	// Type
	spChild = spParameter->selectSingleNode(_T("@") XMLNODE_PARAM_TYPE); //SL
	vtValue.Clear();
	vtValue = (LPCTSTR)sType;
	spChild->PutnodeTypedValue(vtValue);

	// Obsolete
	spChild = spParameter->selectSingleNode(_T("@") XMLNODE_PARAM_OBSOLETE); //SL
	vtValue.Clear();
	vtValue = (LPCTSTR)((bObsolete) ? XMLVAL_TRUE : XMLVAL_FALSE);
	spChild->PutnodeTypedValue(vtValue);

	return true;
}


bool MessageLibraryXMLObj::GetSupportedLocales()
{
	bool bInsertLocale = false;
	CString sTemp;
	_variant_t vtValue;
	LCID locale;
	MSXML2::IXMLDOMAttributePtr spLocale = NULL;
	MSXML2::IXMLDOMNodeListPtr spLocales = m_spMessageRoot->selectNodes(_T("//@") XMLNODE_MSG_LOCALE);
	for(long l = 0, lCount = spLocales->Getlength(); l < lCount; l++)
	{
		bInsertLocale = true;
		spLocale = spLocales->Getitem(l);
		vtValue = spLocale->GetnodeTypedValue();
		sTemp = vtValue.bstrVal;
		locale = _ttoi(sTemp);
		
		for(int i = 0, iCount = m_rgSupportedLocales.GetSize(); i < iCount; i++)
		{
			if(locale == m_rgSupportedLocales[i])
			{
				bInsertLocale = false;
				break;
			}
		}

		if(bInsertLocale)
			m_rgSupportedLocales.Add(locale);
	}

	return true;
}

bool MessageLibraryXMLObj::AddNewLanguage(LCID lcidAddition)
{
	CString sTemp;
	MSXML2::IXMLDOMNodePtr spMessage = NULL;
	MSXML2::IXMLDOMNodePtr spPlaceMarker = NULL;
	MSXML2::IXMLDOMElementPtr spNewString = NULL;
	VARIANT vtNodeElement;
	vtNodeElement.intVal = MSXML2::NODE_ELEMENT;
	vtNodeElement.vt = VT_INT;
	_bstr_t bsNamespace = m_spMessageRoot->GetnamespaceURI();
	sTemp.Format(_T("%d"), lcidAddition);
	_variant_t vtValue((LPCTSTR)sTemp);

	_variant_t vtPlaceMarker;
	vtPlaceMarker.vt = VT_DISPATCH;

	spNewString = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_STRING, bsNamespace);
	spNewString->setAttribute(XMLNODE_MSG_LOCALE, vtValue);

	MSXML2::IXMLDOMNodeListPtr spMessages = m_spMessageRoot->selectNodes(XMLNODE_MSG);
	for(long l = 0, lCount = spMessages->Getlength(); l < lCount; l++)
	{
		spMessage = spMessages->Getitem(l);
		spPlaceMarker = spMessage->selectSingleNode(XMLNODE_MSG_STRING);
		spPlaceMarker->QueryInterface(IID_IDispatch, (void **)&vtPlaceMarker.pdispVal);

		spNewString = m_spDOMDoc->createNode(vtNodeElement, XMLNODE_MSG_STRING, bsNamespace);
		spNewString->setAttribute(XMLNODE_MSG_LOCALE, vtValue);

		spMessage->insertBefore(spNewString, vtPlaceMarker);
	}

	m_rgSupportedLocales.Add(lcidAddition);

	return true;
}

bool MessageLibraryXMLObj::DeleteMessageNode(ULONG ulMessageID)
{
	CString sQuery;
	sQuery.Format(XPATH_MSG_USING_ID, ulMessageID);
	MSXML2::IXMLDOMNodePtr spMessage = m_spMessageRoot->selectSingleNode((LPCTSTR)sQuery);
	if(spMessage == NULL)
		return false;

	m_spMessageRoot->removeChild(spMessage);

    m_IDsPool.returnID(ulMessageID);

	return true;
}

ULONG MessageLibraryXMLObj::GetNextMessageID()
{
	return m_IDsPool.borrowID();
}

CString MessageLibraryXMLObj::GetNextResourceID(const CString& sTemplate)
{
    CWaitCursor wait;

    // The following code expects the template string to be terminated with
    // four digits
    const unsigned long suffixLength = 4; // four digits
    unsigned long templateLength = sTemplate.GetLength();
    if (templateLength < suffixLength)
        return _T("IDS_UX_YYZZZZ");

    // the length of the portion that remains unchanged is 
    unsigned long prefixLength = templateLength - suffixLength;

    // get the bit of the ID that won't change
    CString sBase = sTemplate.Left(prefixLength);
    // and the ID that we're going to increment
    unsigned long i = _ttol(sTemplate.Mid(prefixLength));

    CString sResID; // the res id that we're going to return

    do
    {
        ++i; // increment the id
        sResID.Format(_T("%s%.4d"), sBase, i); // create the new resID

        // if it doesn't already exist we're done...
        CString sQuery;
        sQuery.Format(XPATH_MSG_USING_RESID, sResID);
        MSXML2::IXMLDOMNodePtr spMessage = m_spMessageRoot->selectSingleNode((LPCTSTR)sQuery);
	    if(spMessage == NULL)
        {
		    break;
        }
    } while (i <= 9999); // only exit when we've found an unused ID or reached allowed max

    return sResID;
}

bool MessageLibraryXMLObj::ModifyMessageObsolete(ULONG ulMessageID, bool bObsolete)
{
	CString sQuery;
	sQuery.Format(XPATH_MSG_OBSOLETE_USING_ID, ulMessageID);
	MSXML2::IXMLDOMNodePtr spObsolete = m_spMessageRoot->selectSingleNode((LPCTSTR)sQuery);
	if(spObsolete == NULL)
		return false;

	spObsolete->PutnodeTypedValue(_variant_t((bObsolete) ? XMLVAL_TRUE : XMLVAL_FALSE));

	return true;
}

bool MessageLibraryXMLObj::ModifyMessageType(ULONG ulMessageID, Message::enumMsgTypes msgType)
{
	CString sQuery, sTemp;
	sQuery.Format(XPATH_MSG_TYPE_USING_ID, ulMessageID);
	MSXML2::IXMLDOMNodePtr spType = m_spMessageRoot->selectSingleNode((LPCTSTR)sQuery);
	if(spType == NULL)
		return false;

	sTemp.Format(_T("%d"), msgType);
	spType->PutnodeTypedValue((LPCTSTR)sTemp);

	return true;
}

bool MessageLibraryXMLObj::ModifyMessageHelpID(ULONG ulMessageID, ULONG ulHelpID)
{
	CString sQuery, sTemp;
	sQuery.Format(XPATH_MSG_HELPID_USING_ID, ulMessageID);
	MSXML2::IXMLDOMNodePtr spHelpID = m_spMessageRoot->selectSingleNode((LPCTSTR)sQuery);
	if(spHelpID == NULL)
		return false;

	sTemp.Format(_T("%ld"), ulHelpID);
	spHelpID->PutnodeTypedValue((LPCTSTR)sTemp);

	return true;
}

bool MessageLibraryXMLObj::ModifyMessageStyle(ULONG ulMessageID, ULONG ulStyle)
{
	CString sQuery, sTemp;
	sQuery.Format(XPATH_MSG_STYLE_USING_ID, ulStyle);
	MSXML2::IXMLDOMNodePtr spStyle = m_spMessageRoot->selectSingleNode((LPCTSTR)sQuery);
	if(spStyle == NULL)
		return false;

	sTemp.Format(_T("%ld"), ulStyle);
	spStyle->PutnodeTypedValue((LPCTSTR)sTemp);

	return true;
}

bool MessageLibraryXMLObj::ModifyMessageString(ULONG ulMessageID, LCID lcidLang, LPCTSTR pszString)
{
	CString sQuery;
	sQuery.Format(XPATH_MSG_STR_USING_ID_LOCALE, ulMessageID, lcidLang);
	MSXML2::IXMLDOMNodePtr spString = m_spMessageRoot->selectSingleNode((LPCTSTR)sQuery);
	if(spString == NULL)
		return false;

	spString->PutnodeTypedValue(pszString);

	return true;
}

bool MessageLibraryXMLObj::CheckParamUsage(LPCTSTR pszParam, CArray<ULONG, ULONG>& rgsMessageUsage)
{
	_variant_t vtValue;
	CString sTemp, sQuery;
	sQuery.Format(XPATH_MSG_MSG_IDS_USING_PARAMS, pszParam);
	MSXML2::IXMLDOMNodePtr spMessageID = NULL;
	MSXML2::IXMLDOMNodeListPtr spList = m_spMessageRoot->selectNodes((LPCTSTR)sQuery);
	for(long l = 0, lCount = spList->Getlength(); l < lCount; l++)
	{
		spMessageID = spList->Getitem(l);
		vtValue = spMessageID->GetnodeTypedValue();
		sTemp = vtValue.bstrVal;
		rgsMessageUsage.Add(_ttol(sTemp));
	}

	return (rgsMessageUsage.GetSize() > 0);
}

bool MessageLibraryXMLObj::DeleteParameterNode(const CString& sParameter)
{
	CString sQuery;
	sQuery.Format(XPATH_PARAM_USING_NAME, sParameter);

	MSXML2::IXMLDOMNodePtr spParameterDef = m_spParamRoot->selectSingleNode((LPCTSTR)sQuery);
	if(spParameterDef == NULL)
		return false;

	m_spParamRoot->removeChild(spParameterDef);

	return true;
}