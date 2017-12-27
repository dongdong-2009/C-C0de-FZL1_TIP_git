// MessageGridDoc.cpp : implementation of the MessageGridDoc class
//

#include "..\stdafx.h"
#include "..\TA_MessageLibrary.h"
#include "..\UserMessages.h"

#include <fstream>
#include <iostream>

#include "MessageGridDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static LPCTSTR RESOURCE_ID_INFO = _T("IDS_UI_");
static LPCTSTR RESOURCE_ID_WARNING = _T("IDS_UW_");
static LPCTSTR RESOURCE_ID_ERROR = _T("IDS_UE_");

// MessageGridDoc

IMPLEMENT_DYNCREATE(MessageGridDoc, CDocument)

BEGIN_MESSAGE_MAP(MessageGridDoc, CDocument)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
    ON_COMMAND(ID_FILE_EXPORT, OnExport)
END_MESSAGE_MAP()


// MessageGridDoc construction/destruction

MessageGridDoc::MessageGridDoc()
	:m_messageXMLObj()
{
	m_rgsObsolete.Add(XMLVAL_TRUE);
	m_rgsObsolete.Add(XMLVAL_FALSE);

	CString sTemp;
	sTemp.LoadString(IDS_MSG_TYPE_INFO);
	m_rgsInfoTypes.Add(sTemp);
	sTemp.LoadString(IDS_MSG_TYPE_WARNING);
	m_rgsInfoTypes.Add(sTemp);
	sTemp.LoadString(IDS_MSG_TYPE_ERROR);
	m_rgsInfoTypes.Add(sTemp);
}

MessageGridDoc::~MessageGridDoc()
{
	m_rgsObsolete.RemoveAll();
	m_rgsInfoTypes.RemoveAll();
}

BOOL MessageGridDoc::OnNewDocument()
{
	bool bLoop = true;
	MessageLibraryApp* pApp = GetApp();

	if (!CDocument::OnNewDocument())
		return FALSE;

	HRESULT hr = m_messageXMLObj.LoadMessageDoc(pApp->m_sXMLSrcPath);
	while(bLoop)
	{
		switch(hr)
		{
			case INET_E_RESOURCE_NOT_FOUND:
				{
					CFileDialog dlg(TRUE, _T(".xml"), NULL, OFN_FILEMUSTEXIST|OFN_SHAREAWARE, _T("XML Files (*.xml)|*.xml||"));
					if(dlg.DoModal() == IDOK)
					{
						pApp->m_sXMLSrcPath = dlg.GetPathName();
						hr = m_messageXMLObj.LoadMessageDoc(pApp->m_sXMLSrcPath);
					}
					else
						return false;
				}break;
			case ERROR_SUCCESS:
				{
					bLoop = false;

					SetPathName(pApp->m_sXMLSrcPath);
					SetTitle(pApp->m_sXMLSrcPath);

					CheckWriteAccess();
				}break;
			default:
				return false;
		}
	}

	return true;
}

// MessageGridDoc serialization

void MessageGridDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// MessageGridDoc diagnostics

#ifdef _DEBUG
void MessageGridDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void MessageGridDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// MessageGridDoc commands

BOOL MessageGridDoc::OnOpenDocument(LPCTSTR /*lpszPathName*/)
{
	// The path parameter is ignored. The command line has already been parsed
    // and the path to the XML document is stored in one of the app's member variables.
    // This is because multiple file names (eg: RC and XML) may have been
    // specified on the cmd line and MFC does not handle this at all.

    
	POSITION pos = GetFirstViewPosition();
	if(pos != NULL)
	{
		CView* pView = GetNextView(pos);
		pView->SendMessage(UM_EMPTY_MSG_GRID, NULL, NULL);
	}

	SetPathName(theApp.getXmlSrcPath());

    SetTitle(theApp.getXmlSrcPath());

	CheckWriteAccess();

	return SUCCEEDED(m_messageXMLObj.LoadMessageDoc(theApp.getXmlSrcPath()));
}

bool MessageGridDoc::SaveChanges()
{
		
	if(m_messageXMLObj.SaveMessageDoc())
	{
		SetModifiedFlag(FALSE);
		return true;
	}

	return false;
}

BOOL MessageGridDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	return (!SaveChanges());
}

BOOL MessageGridDoc::SaveModified()
{
	if(IsModified())
	{
		CString sText;
		sText.Format(IDS_APP_SAVE_CHANGES, GetApp()->m_sXMLSrcPath);

		switch(AfxMessageBox(sText, MB_ICONINFORMATION|MB_YESNOCANCEL))
		{
		case IDYES:
			{
				if(!SaveChanges())
					return FALSE;
			}break;
		case IDNO:
			{
				SetModifiedFlag(FALSE);
			}break;
		default:
			return FALSE;
		}
	}

	return TRUE;
}


void MessageGridDoc::OnFileSave()
{
	SaveChanges();
}

void MessageGridDoc::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsModified());
}

void MessageGridDoc::OnFileSaveAs()
{
	CFileDialog dlg(FALSE, _T(".xml"), NULL, OFN_FILEMUSTEXIST|OFN_SHAREAWARE, _T("XML Files (*.xml)|*.xml||"));
	if(dlg.DoModal() == IDOK)
	{
		GetApp()->m_sXMLSrcPath = dlg.GetPathName();
        SaveChanges();	
        SetPathName(GetApp()->m_sXMLSrcPath);
        SetTitle(GetApp()->m_sXMLSrcPath);        
	}
}

void MessageGridDoc::OnExport()
{
    // TODO: Add code here
    CFileDialog dlg(FALSE, _T(".txt"), NULL, OFN_FILEMUSTEXIST|OFN_SHAREAWARE, _T("Text Files (*.txt)|*.txt||"));
	if(dlg.DoModal() == IDOK)
	{
        CWaitCursor wait;

        CStdioFile file( (LPCTSTR)(dlg.GetPathName()), CFile::modeCreate
       | CFile::modeWrite | CFile::typeText );
        MSXML2::IXMLDOMNodeListPtr spMessages = m_messageXMLObj.m_spMessageRoot->selectNodes(XMLNODE_MSG);

        // Column titles
        file.WriteString( XMLNODE_MSG_ID );
        file.WriteString( _T("\t") );
        file.WriteString( XMLNODE_MSG_TYPE );
        file.WriteString( _T("\t") );
        file.WriteString( XMLNODE_MSG_RESID );
        file.WriteString( _T("\t") );
        file.WriteString( XMLNODE_MSG_HELPID );
        file.WriteString( _T("\t") );
        file.WriteString( XMLNODE_MSG_STYLE );
        file.WriteString( _T("\t") );
        file.WriteString( XMLNODE_MSG_AUDIT );
        file.WriteString( _T("\t") );
        file.WriteString( XMLNODE_MSG_STRING );
        file.WriteString( _T("\t") );
        file.WriteString( XMLNODE_MSG_PARAM );
        file.WriteString( _T("\n") );


    	for(long i = 0, iCount = spMessages->Getlength(); i < iCount; i++)
    	{
            // Get the node
    		MSXML2::IXMLDOMNodePtr& spMessage = spMessages->Getitem(i);

            CString sID, sType, sLocale, sObsolete, sAuditEvent, sResourceID;
	        _variant_t vtValue;

          	MSXML2::IXMLDOMNodePtr spChild = spMessage->selectSingleNode(_T("@") XMLNODE_MSG_OBSOLETE); //SL
	        vtValue = spChild->GetnodeTypedValue();

            // don't save obsolete messages
            sObsolete = vtValue.bstrVal;
	        if(sObsolete == XMLVAL_TRUE)
	        {
			    continue;
	        }

	        spChild = spMessage->selectSingleNode(XMLNODE_MSG_ID);
	        vtValue = spChild->GetnodeTypedValue();
	        sID = vtValue.bstrVal;
            file.WriteString( sID );
            file.WriteString( _T("\t") );

	        vtValue.Clear();
	        spChild = spMessage->selectSingleNode(XMLNODE_MSG_TYPE);
	        vtValue = spChild->GetnodeTypedValue();
	        sType = vtValue.bstrVal;

	        if(sType == _T("1")) //SL
		        file.WriteString( m_rgsInfoTypes[0] );
	        else if(sType == _T("2")) //SL
		        file.WriteString( m_rgsInfoTypes[1] );
	        else if(sType == _T("3")) //SL
		        file.WriteString( m_rgsInfoTypes[2] );
            file.WriteString( _T("\t") );

	        vtValue.Clear();
	        spChild = spMessage->selectSingleNode(XMLNODE_MSG_RESID);
            // TODO remove the following transitional if block
            if (0 != spChild)
            {
	            vtValue = spChild->GetnodeTypedValue();
	            sResourceID = vtValue.bstrVal;
            }
            else
            {
                sResourceID = "NA";
            }
            
            file.WriteString( sResourceID );
            file.WriteString( _T("\t") );
	        
	        vtValue.Clear();
	        spChild = spMessage->selectSingleNode(XMLNODE_MSG_HELPID);
	        vtValue = spChild->GetnodeTypedValue();
	        file.WriteString( vtValue.bstrVal );
            file.WriteString( _T("\t") );
	        
	        vtValue.Clear();
	        spChild = spMessage->selectSingleNode(XMLNODE_MSG_STYLE);
	        vtValue = spChild->GetnodeTypedValue();
	        file.WriteString( vtValue.bstrVal );
            file.WriteString( _T("\t") );
	        
	        vtValue.Clear();
	        spChild = spMessage->selectSingleNode(_T("@") XMLNODE_MSG_AUDIT);
	        vtValue = spChild->GetnodeTypedValue();
	        file.WriteString( vtValue.bstrVal );
            file.WriteString( _T("\t") );

	        MSXML2::IXMLDOMElementPtr spString = NULL;
	        MSXML2::IXMLDOMNodeListPtr spStrings = spMessage->selectNodes(XMLNODE_MSG_STRING);
            int lIndex = 0;
            int lCount = 0;
	        LCID locale = 0;
	        for(lIndex = 0, lCount = spStrings->Getlength(); lIndex < lCount; lIndex++)
	        {
		        vtValue.Clear();
		        spString = spStrings->Getitem(lIndex);
		        vtValue = spString->getAttribute(XMLNODE_MSG_LOCALE);
		        sLocale = vtValue.bstrVal;
		        locale = _ttol(sLocale);

		        vtValue.Clear();
		        vtValue = spString->GetnodeTypedValue();
		        file.WriteString( vtValue.bstrVal );
		        //gvItem.col = GetColumnIndex(locale);
	        }

            file.WriteString( _T("\t") );
            // do the parameters
            MSXML2::IXMLDOMNodeListPtr spParams = spMessage->selectNodes(XMLNODE_MSG_PARAM);
	        long l = 0;
            for(l = 0, lCount = spParams->Getlength(); l < lCount; l++)
	        {
                if (l>0)
                {
                    file.WriteString(_T(", "));
                }
		        vtValue.Clear();
		        spChild = spParams->Getitem(l);
		        vtValue = spChild->GetnodeTypedValue();
		        CString temp;
                temp.Format(_T("{%d}=%s"), l, vtValue.bstrVal);
                file.WriteString( temp );
	        }
            
            file.WriteString( _T("\n") );
            file.Flush();
    	}
        file.Close();
    }
}

void MessageGridDoc::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

bool MessageGridDoc::DeleteMessage(ULONG ulMessageID)
{
	if(m_messageXMLObj.DeleteMessageNode(ulMessageID))
	{
		
		SetModifiedFlag();

		return true;
	}
	else
		return false;
}

bool MessageGridDoc::AddNewMessage(Message& msg)
{
	if(m_messageXMLObj.CreateMessageNode(msg))
	{
		SetModifiedFlag();

		return true;
	}
	else
		return false;
}

bool MessageGridDoc::ModifyMessage(Message& msg)
{
	if(m_messageXMLObj.ModifyMessageNode(msg))
	{
		SetModifiedFlag();

		return true;
	}
	return false;
}


bool MessageGridDoc::ModifyParameter(CString& sParam, CString& sType, bool bObsolete, 
									  CString& sDescription, bool bNewParam)
{
	CString sComment;

	if(bNewParam)
	{
		if(m_messageXMLObj.CreateParameterNode(sParam, sDescription, sType, bObsolete))
		{
			SetModifiedFlag();

			return true;
		}
	}
	else
	{
		if(m_messageXMLObj.ModifyParameterNode(sParam, sDescription, sType, bObsolete))
		{
			SetModifiedFlag();

			return true;
		}
	}

	return false;
}


bool MessageGridDoc::AddNewLanguage(LCID lcidNewLang)
{
	if(m_messageXMLObj.AddNewLanguage(lcidNewLang))
	{
		SetModifiedFlag(TRUE);

		return true;
	}

	return false;
}

// TODO THESE 2 FUNCTIONS SHOULDN"T BE NEEDED
/*const bool MessageGridDoc::FormatResourceID(LPCTSTR pszMsgType, const CString& sID, CString& sResourceID)
{
	if(_tcscmp(pszMsgType, _T("1")) == 0 || _tcscmp(pszMsgType, _T("Information")) == 0)
		sResourceID = RESOURCE_ID_INFO;
	else if(_tcscmp(pszMsgType, _T("2")) == 0 || _tcscmp(pszMsgType, _T("Warning")) == 0)
		sResourceID = RESOURCE_ID_WARNING;
	else if(_tcscmp(pszMsgType, _T("3")) == 0 || _tcscmp(pszMsgType, _T("Error")) == 0)
		sResourceID = RESOURCE_ID_ERROR;
	else
		return false;

	sResourceID += sID;

	return true;
}

const bool MessageGridDoc::FormatResourceID(const Message::enumMsgTypes iMsgType, const ULONG ulMessageID, CString& sResourceID)
{
	CString sID;

	switch(iMsgType)
	{
		case Message::enumMsgTypes::InfoMessage: 
			sResourceID = RESOURCE_ID_INFO; 
			break;
		case Message::enumMsgTypes::WarningMessage: 
			sResourceID = RESOURCE_ID_WARNING; 
			break;
		case Message::enumMsgTypes::ErrorMessage: 
			sResourceID = RESOURCE_ID_ERROR; 
			break;
		default:
			return false;
	}

	sID.Format(_T("%ld"), ulMessageID);
	sResourceID += sID;

	return true;
}*/

Message::enumMsgErrors MessageGridDoc::ValidateMessage(ULONG ulMessageID, LCID lcidStringLang)
{
	Message message;
	
	if(ulMessageID == 0)
		return Message::enumMsgErrors::errorOK;
	
	if(!m_messageXMLObj.ConstructMessage(ulMessageID, message))
		return Message::enumMsgErrors::errorOK;
	
	int iArgCount = message.m_rgsParameters.GetSize();
	int iStart = 0, iFinish = 0, iIndex = 0, iParamIndex = 0, iRightBraceCount = 0;
	CString sFormat, sParamIndex, sSubString, sMessage;
	bool bFindRightBrace = true;
	
	if(!message.m_mapMessages.Lookup(lcidStringLang, sMessage) || sMessage == _T(""))
		return Message::enumMsgErrors::errorOK;
		
		
	iStart = sMessage.Find(_T("{"), iStart); //SL
	if(iStart < 0 && iArgCount > 0)
		return Message::enumMsgErrors::errorParamCount;
	
	while(iStart >= 0)
	{
		// If not at the end of the string check for overriding character
		if(iStart < (sMessage.GetLength() - 1) && sMessage[iStart + 1] == '{') //SL
		{
			iStart += 2;
			if(iStart < sMessage.GetLength())
			{
				iStart = sMessage.Find(_T("{"), iStart); //SL
				continue;
			}
			else
				break;
		}
		
		iFinish = iStart + 1;
		iRightBraceCount = 0;
		bFindRightBrace = true;
		
		while(bFindRightBrace)
		{
			iFinish = sMessage.Find(_T("}"), iFinish); //SL
			if(iFinish < 0)
				return Message::enumMsgErrors::errorMalformed;
		
			// Check for overriding character
			if(iFinish < (sMessage.GetLength() - 1) && sMessage[iFinish + 1] == '}') //SL
			{
				iRightBraceCount++;
				iFinish++;
			}
			else
				bFindRightBrace = false; // stop loop
		}
		
		// odd number of right braces (iRightBraceCount is one less than actual count)
		if(iRightBraceCount % 2 == 0)
			iFinish -= iRightBraceCount;
		
		// lose the {} characters
		sSubString = sMessage.Mid(iStart + 1, iFinish - iStart - 1);
		sFormat.Empty();
		int iComma = 0;
		iComma = sSubString.Find(_T(","), iComma); //SL
		if(iComma > 0)
		{
			sFormat = sSubString.Right(sSubString.GetLength() - iComma);
			sParamIndex = sSubString.Left(iComma);
		}
		else
			sParamIndex = sSubString;
		
		iParamIndex = _ttoi(sParamIndex);
		if(iParamIndex < 0 || iParamIndex >= iArgCount)
			return Message::enumMsgErrors::errorPlaceMarkerCount;
		
		// Perform Substitution
		sMessage.Delete(iStart, iFinish - iStart + 1);
		
		iStart = sMessage.Find(_T("{"), iStart); //SL
	} //while
	
	return Message::enumMsgErrors::errorOK;
}

bool MessageGridDoc::CheckWriteAccess()
{
    DWORD dwAttributes = ::GetFileAttributes(GetApp()->m_sXMLSrcPath);
	if(dwAttributes & FILE_ATTRIBUTE_READONLY)
	{
        CString sTitle;
		sTitle = GetTitle();
		sTitle += _T(" (Read Only)");
		SetTitle(sTitle);

		return false;
	}
	else
	{
		return true;
	}
}

bool MessageGridDoc::ModifyMessageObsolete(ULONG ulMessageID, LPCTSTR pszObsolete)
{
	bool bObsolete = (pszObsolete == XMLVAL_TRUE);

	if(m_messageXMLObj.ModifyMessageObsolete(ulMessageID, bObsolete))
	{
		SetModifiedFlag();

		return true;
	}
	return false;
}

bool MessageGridDoc::ModifyMessageType(ULONG ulMessageID, LPCTSTR pszMsgType)
{
	int iType = 0;
	for(iType = 0; iType < m_rgsInfoTypes.GetSize(); iType++)
	{
		if(m_rgsInfoTypes[iType] == pszMsgType)
			break;
	}
	
	if(m_messageXMLObj.ModifyMessageType(ulMessageID, (Message::enumMsgTypes)(iType + 1)))
	{
		SetModifiedFlag();

		return true;
	}

	return false;
}

bool MessageGridDoc::ModifyMessageHelpID(ULONG ulMessageID, LPCTSTR pszHelpID)
{
	ULONG ulHelpID = _ttol(pszHelpID);

	if(m_messageXMLObj.ModifyMessageHelpID(ulMessageID, ulHelpID))
	{
		SetModifiedFlag();

		return true;
	}

	return false;
}

bool MessageGridDoc::ModifyMessageStyle(ULONG ulMessageID, LPCTSTR pszStyleID)
{
	ULONG ulStyle = _ttol(pszStyleID);

	if(m_messageXMLObj.ModifyMessageStyle(ulMessageID, ulStyle))
	{
		SetModifiedFlag();

		return true;
	}

	return false;
}

bool MessageGridDoc::ModifyMessageAuditEvent(ULONG ulMessageID, LPCTSTR pszAuditEvent)
{
	if(m_messageXMLObj.ModifyMessageObsolete(ulMessageID, (pszAuditEvent == XMLVAL_TRUE)))
	{
		SetModifiedFlag();

		return true;
	}
	return false;
}

bool MessageGridDoc::ModifyMessageString(ULONG ulMessageID, LCID lcidLang, LPCTSTR pszString)
{
	if(m_messageXMLObj.ModifyMessageString(ulMessageID, lcidLang, pszString))
	{
		SetModifiedFlag();

		return true;
	}
	return false;
}

bool MessageGridDoc::DeleteParameter(const CString& sParam)
{
	if(m_messageXMLObj.DeleteParameterNode(sParam))
	{
		SetModifiedFlag();

		return true;
	}
	return false;
}

bool MessageGridDoc::CreateMessageForClipboard(const ULONG ulMessageID, CString& sText)
{
	Message msg;
	if(!m_messageXMLObj.ConstructMessage(ulMessageID, msg))
		return false;

	sText = _T("TA_Bus::TransActiveMessage userMsg;\r\n");
	for(int i = 0; i < msg.m_rgsParameters.GetSize(); i++)
	{
		if(i == 0)
			sText += _T("userMsg");

		sText += _T(" << ");
		sText += msg.m_rgsParameters[i];
	}
	
	if(i != 0)
		sText += _T(";\r\n");

	sText += _T("UINT selectedButton = userMsg.showMsgBox(");
	sText += msg.m_ResID;
	sText += _T(");");
	
	return true;
}