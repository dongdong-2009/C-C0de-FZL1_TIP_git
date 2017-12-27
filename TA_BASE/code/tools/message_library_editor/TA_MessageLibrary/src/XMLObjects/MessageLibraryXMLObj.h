#pragma once

#include "XMLDefines.h"
#include "..\Message.h"
#include "..\UniqueIDsPool.h"

class MessageLibraryXMLObj
{
public:

	CArray<LCID, LCID> m_rgSupportedLocales;

	MessageLibraryXMLObj(void);
	virtual ~MessageLibraryXMLObj(void);

	MSXML2::IXMLDOMDocument2Ptr m_spDOMDoc;

	MSXML2::IXMLDOMNodePtr m_spMessageRoot;
	MSXML2::IXMLDOMNodePtr m_spParamRoot;
	//MSXML2::IXMLDOMNodePtr m_spLogRoot;

	// attempts to load the document at the specified path. Displays
    // an error message if it fails
    HRESULT LoadMessageDoc(LPCTSTR strPath);
    void TruncateMessageDoc(void); // delete all the messages from the current document.

	bool SaveMessageDoc();

	bool CreateMessageNode(Message::enumMsgTypes MsgType, CString& sMessage, CString& sUniqueID, 
							CStringArray& rgsParameters, bool bObsolete, bool bAuditEvent);
	bool CreateMessageNode(Message& msg);

	bool ModifyMessageNode(Message& msg);

	bool ConstructMessage(unsigned long MsgID, Message& message);

	bool CreateParameterNode(const CString& sParameter, const CString& sDescription, const CString& sType, const bool bObsolete);
	bool ModifyParameterNode(const CString& sParameter, const CString& sDescription, const CString& sType, const bool bObsolete);
	bool DeleteParameterNode(const CString& sParameter);

	bool GetSupportedLocales();

	bool AddNewLanguage(LCID lcidAddition);

	bool DeleteMessageNode(ULONG ulMessageID);

	ULONG GetNextMessageID();
    CString GetNextResourceID(const CString& sTemplate);
    
	bool ModifyMessageType(ULONG ulMessageID, Message::enumMsgTypes msgType);
	bool ModifyMessageHelpID(ULONG ulMessageID, ULONG ulHelpID);
	bool ModifyMessageStyle(ULONG ulMessageID, ULONG ulStyle);
	bool ModifyMessageObsolete(ULONG ulMessageID, bool bObsolete);
	bool ModifyMessageString(ULONG ulMessageID, LCID lcidLang, LPCTSTR pszString);

	bool CheckParamUsage(LPCTSTR pszParam, CArray<ULONG, ULONG>& rgsMessageUsage);

private:
    UniqueIDsPool m_IDsPool;
};
