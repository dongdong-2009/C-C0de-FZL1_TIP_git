// MessageGridDoc.h : interface of the MessageGridDoc class
//
#pragma once

#include "..\XMLObjects\XMLDefines.h"
#include "..\XMLObjects\MessageLibraryXMLObj.h"

class MessageGridDoc : public CDocument
{
protected: // create from serialization only
	MessageGridDoc();
	DECLARE_DYNCREATE(MessageGridDoc)

// Attributes
public:
	MessageLibraryXMLObj m_messageXMLObj;
	
	CStringArray m_rgsObsolete;
	CStringArray m_rgsInfoTypes;

// Operations
public:

	bool DeleteMessage(ULONG ulMessageID);
	
	bool AddNewMessage(Message& msg);
	bool ModifyMessage(Message& msg);
	bool ModifyParameter(CString& sParam, CString& sType, bool bObsolete, 
							CString& sDescription, bool bNewParam);
	bool DeleteParameter(const CString& sParam);
	bool SaveChanges();
	
	bool AddNewLanguage(LCID lcidNewLang);
	bool CreateMessageForClipboard(const ULONG ulMessageID, CString& sText);

	Message::enumMsgErrors ValidateMessage(ULONG ulMessageID, LCID lcidStringLang);

	bool CheckWriteAccess();

	bool ModifyMessageType(ULONG ulMessageID, LPCTSTR pszMsgType);
	bool ModifyMessageHelpID(ULONG ulMessageID, LPCTSTR pszHelpID);
	bool ModifyMessageStyle(ULONG ulMessageID, LPCTSTR pszStyleID);
	bool ModifyMessageObsolete(ULONG ulMessageID, LPCTSTR pszObsolete);
	bool ModifyMessageAuditEvent(ULONG ulMessageID, LPCTSTR pszAuditEvent);
	bool ModifyMessageString(ULONG ulMessageID, LCID lcidLang, LPCTSTR pszString);

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL SaveModified();
// Implementation
public:
	virtual ~MessageGridDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
    afx_msg void OnExport();
};


