#include "..\stdafx.h"
#include "..\TA_MessageLibrary.h"

#include "..\GlobalFunctions.h"
#include "..\MainFrm.h"
#include "..\MessageGrid\MessageGridDoc.h"

#include "msgextractor.h"
#include "MessageAccessor.h"
#include "ParameterAccessor.h"

// The greatest number of parameters in the old spreadsheet (was 7 on 06/05/2004)
static const int LEGACY_MAX_PARAM_COUNT = 10;

static LPCTSTR EXCEL_SHEET_INFO	= _T("Information");
static LPCTSTR EXCEL_SHEET_WARNING = _T("Warning");
static LPCTSTR EXCEL_SHEET_ERROR = _T("Error");

//static LPCTSTR EXCEL_SELECT_MSG_STATEMENT = _T("SELECT `Unique ID`, `Message`, `Parameter(s)`, `Obsolete` FROM `%s$`");
static LPCTSTR EXCEL_SELECT_MSG_STATEMENT = _T("SELECT `Unique ID`, `Message`, `Parameter(s)` FROM `%s$`");
static LPCTSTR EXCEL_SELECT_PARAM_STATEMENT = _T("SELECT `Parameter Name`, `Description` FROM `Parameter List$`");

static LPCTSTR EXCEL_CONNECTION_STRING = _T("Provider=Microsoft.Jet.OLEDB.4.0;User ID=Admin;Data Source=%s;Mode=Share Deny None;Extended Properties=Excel 8.0;Jet OLEDB:System database=\"\";Jet OLEDB:Registry Path=\"\";Jet OLEDB:Database Password=\"\";Jet OLEDB:Engine Type=35;Jet OLEDB:Database Locking Mode=0;Jet OLEDB:Global Partial Bulk Ops=2;Jet OLEDB:Global Bulk Transactions=1;Jet OLEDB:New Database Password=\"\";Jet OLEDB:Create System Database=False;Jet OLEDB:Encrypt Database=False;Jet OLEDB:Don't Copy Locale on Compact=False;Jet OLEDB:Compact Without Replica Repair=False;Jet OLEDB:SFP=False");

static LPCTSTR EXCEL_REGPATH = _T("Software\\Microsoft\\Jet\\4.0\\Engines\\Excel");
static LPCTSTR EXCEL_REGPATH_KEY = _T("TypeGuessRows");



MsgExtractor::MsgExtractor(void)
: m_pResourceHeader(NULL), m_pRCFile(NULL), m_uiStringCount(0), 
	m_dwPrevExcelGuessRows(0), m_pResourceSourceObj(NULL), m_lcidLanguageToExport(0),
	m_bIncludeENA(true), m_pProgressWnd(NULL)
{
	m_sSourcePath.Empty();
	m_sTargetPath.Empty();
	m_sTargetRCPath.Empty();
	m_sTargetHPath.Empty();

}

MsgExtractor::~MsgExtractor(void)
{
	if(m_pResourceSourceObj)
	{
		delete m_pResourceSourceObj;
		m_pResourceSourceObj = NULL;
	}
	if(m_pRCFile)
	{
		m_pRCFile->Flush();
		m_pRCFile->Close();
		delete m_pRCFile;
		m_pRCFile = NULL;
	}

	if(m_pResourceHeader)
	{
		m_pResourceHeader->Flush();
		m_pResourceHeader->Close();
		delete m_pResourceHeader;
		m_pResourceHeader = NULL;
	}
}

bool MsgExtractor::ExtractMsgsFromExcel(CWnd* pProgressWnd)
{
	m_pProgressWnd = pProgressWnd;
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());

	UpdateProgress(IDS_APP_EXCEL_PROGRESS_LOAD_DOC);
	pDoc->m_messageXMLObj.TruncateMessageDoc();

	UpdateProgress(IDS_APP_EXCEL_PROGRESS_REGISTRY);
	if(!SetGuessRowSizeParameter())
		return false;

	UpdateProgress(IDS_APP_EXCEL_PROGRESS_CONNECT);
	if(!OpenDataSource())
		return false;

	if(	!ExtractMsgsFromExcel(EXCEL_SHEET_INFO, Message::InfoMessage) ||
		!ExtractMsgsFromExcel(EXCEL_SHEET_WARNING, Message::WarningMessage) ||
		!ExtractMsgsFromExcel(EXCEL_SHEET_ERROR, Message::ErrorMessage) )
	{
		CloseDataSource();
		SetGuessRowSizeParameter(true);

		return false;
	}

	UpdateProgress(IDS_APP_EXCEL_PROGRESS_PARAMS);
	if(!ExtractParamsFromExcel())
	{
		CloseDataSource();
		SetGuessRowSizeParameter(true);
	}

	CloseDataSource();
	SetGuessRowSizeParameter(true);

	UpdateProgress(IDS_APP_EXCEL_PROGRESS_COMPLETE);

    pDoc->SetModifiedFlag();
	
	return (pDoc->m_messageXMLObj.SaveMessageDoc());
}

bool MsgExtractor::ExtractMsgsFromExcel(LPCTSTR szWorkSheet, Message::enumMsgTypes MsgType)
{
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());
	CString sMessage, sResID, sSQLQuery, sToken, sProgress;
	CStringArray rgParameters;

    
	sProgress.Format(IDS_APP_EXCEL_PROGRESS_WORKSHEET, szWorkSheet);
	UpdateProgress(sProgress);

	sSQLQuery.Format(EXCEL_SELECT_MSG_STATEMENT, szWorkSheet);

	CCommand<CAccessor<MessageAccessor> > command;
	command.ClearRecordMemory();
	HRESULT hr = command.Open(m_sessionExcel, sSQLQuery);
	if(FAILED(hr))
	{
		AtlTraceErrorRecords(hr);
		return false;
	}

	command.ClearRecordMemory();

	hr = command.MoveFirst();
	if(FAILED(hr))
	{
		AtlTraceErrorRecords(hr);
		command.Close();
		return false;
	}

	do
	{
		sMessage = command.m_Message;
		if(sMessage.IsEmpty())
		{
			hr = command.MoveNext();
			continue;
		}

        
		sToken = command.m_UniqueID;
		while(sToken.GetLength() < 9) // pad with zeros to fix existing id errors
		{
			sToken.Insert(4, _T("0"));
		}
		        
        // Replace '-' with '_'
        sToken.Replace(_T('-'), _T('_'));
        // Prepend 'IDS_'
        sResID = _T("IDS_") + sToken;

            		
        rgParameters.RemoveAll();

		if(_tcslen(command.m_Parameters) > 0)
		{
			LPTSTR pszToken = _tcstok(command.m_Parameters, _T("\n,")); //SL
			while(pszToken)
			{
				rgParameters.Add(pszToken);
				pszToken = _tcstok(NULL, _T("\n,"));
			}
		}
		
		if(!ConvertExcelStyle(sMessage, rgParameters))
		{
			CString sError;
			sError.Format(IDS_APP_EXCEL_MSG_ERROR, szWorkSheet, command.m_UniqueID);
			AfxMessageBox(sError, MB_ICONERROR);
			return false;
		}

		if(!pDoc->m_messageXMLObj.CreateMessageNode(MsgType, sMessage, sResID, rgParameters, false, false))
		{
			command.Close();
			return false;
		}

		hr = command.MoveNext();
		if(FAILED(hr))
		{
			AtlTraceErrorRecords(hr);
		}
	}
	while(hr != DB_S_ENDOFROWSET && SUCCEEDED(hr));

	command.Close();
	
	return true;
}



bool MsgExtractor::CreateOutputFiles()
{
	m_sTargetHPath = m_sTargetRCPath;
	int iStart = m_sTargetHPath.ReverseFind('\\');//SL
	ASSERT(iStart > 0);
	m_sTargetHPath.Delete(iStart, m_sTargetHPath.GetLength() - iStart);
	AppendPath(m_sTargetHPath, _T("resource.h")); //SL
	
	try
	{
		m_pRCFile = new UnicodeIOFile(m_sTargetRCPath, CFile::modeCreate|CFile::modeReadWrite);
		m_pResourceHeader = new CStdioFile(m_sTargetHPath, CFile::modeCreate|CFile::modeReadWrite);
	}
	catch(CFileException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool MsgExtractor::OutputResourceHeader(LPCTSTR szResourceID, LPCTSTR szResourceValue)
{
	CString sOutput;
	sOutput.Format(_T("#define %s\t\t%s\n"), szResourceID, szResourceValue); //SL
	try
	{
		m_pResourceHeader->WriteString(sOutput);
	}
	catch(CFileException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool MsgExtractor::OutputStringTable(LPCTSTR sResourceID, LPCTSTR sAusMessage)
{
	try
	{
		if(m_uiStringCount == 0)
			m_pRCFile->WriteString(_T("STRINGTABLE DISCARDABLE\nBEGIN\n")); //SL

		CString sOutput;
		sOutput.Format(_T("\t%s\t\t\"%s\"\n"), sResourceID, sAusMessage); //SL
		m_pRCFile->WriteString(sOutput);

		if(m_uiStringCount >= 15)
		{
			m_pRCFile->WriteString(_T("END\n\n")); //SL
			m_uiStringCount = 0;
		}
		else
			m_uiStringCount++;
	}
	catch(CFileException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool MsgExtractor::CloseOutputFiles()
{
	try
	{
		m_pRCFile->Flush();
		m_pRCFile->Close();
		delete m_pRCFile;
		m_pRCFile = NULL;

		m_pResourceHeader->Flush();
		m_pResourceHeader->Close();
		delete m_pResourceHeader;
		m_pResourceHeader = NULL;
	}
	catch(CFileException* e)
	{
		e->ReportError();
		e->Delete();

		return false;
	}
	return true;
}

bool MsgExtractor::EndStringTable()
{
	if(m_uiStringCount != 0)
	{
		try
		{
			m_pRCFile->WriteString(_T("END\n\n")); //SL
			m_uiStringCount = 0;
		}
		catch(CFileException* e)
		{
			e->ReportError();
			e->Delete();
			return false;
		}
	}
	return true;
}

bool MsgExtractor::ExportMessagesToRC()
{
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());
	CString sTop, sMiddle, sBottom;

	if(!m_pResourceSourceObj)
	{
		m_pResourceSourceObj = new ResourceSrcXMLObj();
		if(!m_pResourceSourceObj->LoadSourceDoc())
			return false;
	}

	if(!CreateOutputFiles())
		return false;

	// RC file
	if(!ParseResourceSource(sTop, sMiddle, sBottom))
		return false;

	m_pRCFile->WriteString(sTop);

	if(!CreateStringTable(m_lcidLanguageToExport))
		return false;

	m_pRCFile->WriteString(sMiddle);

	if(m_bIncludeENA)
	{
		if(!CreateStringTable(3081))
			return false;
	}

	m_pRCFile->WriteString(sBottom);

	// Header file
	if(!CreateResourceHeader())
		return false;
	
	return CloseOutputFiles();
}

bool MsgExtractor::OpenDataSource()
{
	CString sInitString;
	sInitString.Format(EXCEL_CONNECTION_STRING,m_sSourcePath);
	_bstr_t bsInitString((LPCTSTR)sInitString);

	HRESULT hr = m_dataSourceExcel.OpenFromInitializationString(bsInitString);
	if(FAILED(hr))
	{
		AtlTraceErrorRecords(hr);
		return false;
	}

	hr = m_sessionExcel.Open(m_dataSourceExcel);
	if(FAILED(hr))
	{
		AtlTraceErrorRecords(hr);
		return false;
	}

	return true;
}

void MsgExtractor::CloseDataSource()
{
	m_sessionExcel.Close();
	m_dataSourceExcel.Close();
}

bool MsgExtractor::ConvertExcelStyle(CString& sMessage, CStringArray& rgsParameters)
{
	int iStart = 0, iFinish = 0, iParamIndex = 0, i = 0, iCount = 0;
	CString sParam, sPlaceMarker;

	// Remove horizontal elipse ...(U2026)
	// it doesn't work with narrow char RC output.
	iStart = sMessage.Find(TCHAR(0x2026), iStart);
	while(iStart >= 0)
	{
		sMessage.Delete(iStart);
		sMessage.Insert(iStart, _T("..."));
		iStart = sMessage.Find(TCHAR(0x2026), iStart);
	}

	// Remove old version of place marking eg %1, %2, etc
	for(i = 1; i < LEGACY_MAX_PARAM_COUNT; i++)
	{
		iStart = 0;
		sPlaceMarker.Format(_T("%%%d"), i); //SL
		iStart = sMessage.Find(sPlaceMarker, iStart);
		
		if(iStart < 0)
			continue;

		sMessage.Delete(iStart, sPlaceMarker.GetLength());
		sPlaceMarker.Format(_T("{%d}"), (i - 1)); //SL
		sMessage.Insert(iStart, sPlaceMarker);
		iStart += sPlaceMarker.GetLength();
	}

	for(i = 0, iCount = rgsParameters.GetSize(); i < iCount; i++)
	{
		for(int j = 1; j < LEGACY_MAX_PARAM_COUNT; j++)
		{
			iStart = 0;
			sPlaceMarker.Format(_T("%%%d"), j); //SL
			iStart = rgsParameters[i].Find(sPlaceMarker, 0);
			if(iStart < 0)
				continue;

			rgsParameters[i].Delete(iStart, sPlaceMarker.GetLength());
			rgsParameters[i].TrimLeft();
			
			// whitespace in the parameter column of excel is comming out as 0xA0
			rgsParameters[i].TrimLeft(TCHAR(0xA0));
		}
	}

	iStart = 0;

	do
	{
		iStart = sMessage.Find(_T("["), iStart); //SL
		if(iStart < 0)
			break;

		iFinish = iStart + 1;
		iFinish = sMessage.Find(_T("]"), iFinish); //SL
		if(iFinish < 0)
		{
			TRACE1("ConvertExcelStyle - message has invalid format - %s\n", sMessage);
			return false;
		}
		sParam = sMessage.Mid(iStart + 1, iFinish - (iStart + 1));
		if(rgsParameters.GetSize())
		{
			for(i = 0, iCount = rgsParameters.GetSize(); i < iCount; i++)
			{
				if(rgsParameters[i] == sParam)
					break;
			}

			if(i == iCount)
				iParamIndex = rgsParameters.Add(sParam);
			else
				iParamIndex = i;
		}
		else
			iParamIndex = rgsParameters.Add(sParam);

		sPlaceMarker.Format(_T("{%d}"), iParamIndex); //SL
		sMessage.Delete(iStart, (iFinish + 1) - iStart);
		sMessage.Insert(iStart, sPlaceMarker);
	} 
	while(iStart >= 0);

	return true;
}

bool MsgExtractor::SetGuessRowSizeParameter(bool bReset)
{
	// The Excel provider checks the registry to determine the number of rows to scan when
	// determining the size of the string buffer to use. The default is 8, so any rows that
	// have a length greater than 256 after row 8 will be truncated. Set to 0, to scan 18384. 
	// Reset to remove performance hit. See MSDN Q281517.

	// HKLM\Software\Microsoft\Jet\4.0\Engines\Excel\TypeGuessRows
	HKEY hKey = NULL;
	DWORD dwPrevValue = 0, dwDataLength = 0, dwDataType = REG_DWORD;
	LRESULT res = ERROR_SUCCESS;
	res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, EXCEL_REGPATH, NULL, KEY_QUERY_VALUE|KEY_SET_VALUE, &hKey);
	if(res != ERROR_SUCCESS)
	{
		WindowsError(res);
		return false;
	}
	dwDataLength = sizeof(dwPrevValue);
	res = RegQueryValueEx(hKey, EXCEL_REGPATH_KEY, NULL, &dwDataType, (LPBYTE)&dwPrevValue, &dwDataLength);
	if(res != ERROR_SUCCESS)
	{
		WindowsError(res);
		return false;
	}

	if(!bReset)
	{	
		DWORD dwTemp = 0;
		dwDataLength = sizeof(dwPrevValue);
		res = RegSetValueEx(hKey, EXCEL_REGPATH_KEY, NULL, REG_DWORD, (LPBYTE)&dwTemp, dwDataLength);
		if(res != ERROR_SUCCESS)
		{
            ::RegCloseKey(hKey);
			WindowsError(res);
			return false;
		}

		m_dwPrevExcelGuessRows = dwPrevValue;
	}
	else
	{
		dwDataLength = sizeof(m_dwPrevExcelGuessRows);
		res = RegSetValueEx(hKey, EXCEL_REGPATH_KEY, NULL, REG_DWORD, (LPBYTE)&m_dwPrevExcelGuessRows, dwDataLength);
		if(res != ERROR_SUCCESS)
		{
            ::RegCloseKey(hKey);
			WindowsError(res);
			return false;
		}
	}

    ::RegCloseKey(hKey);
	return true;
}

bool MsgExtractor::CreateStringResource(MSXML2::IXMLDOMNodePtr& spMessage, LCID lcidStringTableLang)
{
	CString sResourceID, sID, sMessage, sHelpID, sMsgStyle, sMsgType, sMsgText, sAuditEvent;
	CString sQuery;
	sQuery.Format(XPATH_MSG_STR_USING_LOCALE, lcidStringTableLang);
	_variant_t vtValue;

	MSXML2::IXMLDOMNodePtr spChild = spMessage->selectSingleNode(XMLNODE_MSG_ID);
	vtValue = spChild->GetnodeTypedValue();
	sID = vtValue.bstrVal;

	vtValue.Clear();
	spChild = spMessage->selectSingleNode(_T("@") XMLNODE_MSG_AUDIT);
	vtValue = spChild->GetnodeTypedValue();
	sAuditEvent = vtValue.bstrVal;
	if(sAuditEvent == XMLVAL_TRUE)
		sAuditEvent = _T("1");
	else
		sAuditEvent = _T("0");
	
	vtValue.Clear();
	spChild = spMessage->selectSingleNode((LPCTSTR)sQuery);
	vtValue = spChild->GetnodeTypedValue();
	sMessage = vtValue.bstrVal;

	vtValue.Clear();
	spChild = spMessage->selectSingleNode(XMLNODE_MSG_HELPID);
	vtValue = spChild->GetnodeTypedValue();
	sHelpID = vtValue.bstrVal;

	vtValue.Clear();
	spChild = spMessage->selectSingleNode(XMLNODE_MSG_STYLE);
	vtValue = spChild->GetnodeTypedValue();
	sMsgStyle = vtValue.bstrVal;

	vtValue.Clear();
	spChild = spMessage->selectSingleNode(XMLNODE_MSG_TYPE);
	vtValue = spChild->GetnodeTypedValue();
	sMsgType = vtValue.bstrVal;

	vtValue.Clear();
    spChild = spMessage->selectSingleNode(XMLNODE_MSG_RESID);
    vtValue = spChild->GetnodeTypedValue();
    sResourceID = vtValue.bstrVal;

	FormatEscapeSequences(sMessage);

	sMsgText.Format(_T("%s|%s|%s|%s"), sMsgStyle, sHelpID, sAuditEvent, sMessage);

	return OutputStringTable(sResourceID, sMsgText);
}

void MsgExtractor::FormatEscapeSequences(CString& sMessage)
{
	sMessage.Replace(_T("\n"), _T("\\n")); //SL
	sMessage.Replace(_T("\t"), _T("\\t")); //SL
	sMessage.Replace(_T("\""), _T("\"\"")); //SL
}

bool MsgExtractor::CreateResourceIdentifiers(MSXML2::IXMLDOMNodePtr& spMessage)
{
	MSXML2::IXMLDOMNodePtr spChild = spMessage->selectSingleNode(XMLNODE_MSG_ID);
	_variant_t vtValue = spChild->GetnodeTypedValue();
	CString sID = vtValue.bstrVal;

	vtValue.Clear();
	spChild = spMessage->selectSingleNode(XMLNODE_MSG_RESID);
	vtValue = spChild->GetnodeTypedValue();
	CString sResID = vtValue.bstrVal;
	
	return OutputResourceHeader(sResID, sID);
}

bool MsgExtractor::ExtractParamsFromExcel()
{
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());
	CString sQuery, sParameter, sDescription, sType(_T("string"));

	CCommand<CAccessor<ParameterAccessor> > command;
	HRESULT hr = command.Open(m_sessionExcel, EXCEL_SELECT_PARAM_STATEMENT);
	if(FAILED(hr))
	{
		AtlTraceErrorRecords(hr);
		return false;
	}

	command.ClearRecordMemory();

	hr = command.MoveFirst();
	if(FAILED(hr))
	{
		AtlTraceErrorRecords(hr);
		command.Close();
		return false;
	}

	do
	{
		sParameter = command.m_ParameterName;
		sDescription = command.m_Description;
		sQuery.Format(XPATH_PARAM_USING_NAME, sParameter);

		// Check if it already exists
		if(pDoc->m_messageXMLObj.m_spParamRoot->selectSingleNode((LPCTSTR)sQuery) == NULL)
		{
			if(!pDoc->m_messageXMLObj.CreateParameterNode(sParameter, sDescription, sType, false))
			{
				command.Close();
				return false;
			}
		}

		hr = command.MoveNext();
		if(FAILED(hr))
		{
			AtlTraceErrorRecords(hr);
		}
	}
	while(hr != DB_S_ENDOFROWSET && SUCCEEDED(hr));

	command.Close();
	
	return true;
}

bool MsgExtractor::CreateStringTable(LCID lcidStringTableLang)
{
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());
	CString sTop, sBottom;

	if(lcidStringTableLang != 3081)
	{
		if(!ParseStringTableSource(sTop, sBottom, lcidStringTableLang))
			return false;

		m_pRCFile->WriteString(sTop);
	}

	MSXML2::IXMLDOMNodePtr spMessage = NULL;
	MSXML2::IXMLDOMNodeListPtr spMessages = pDoc->m_messageXMLObj.m_spMessageRoot->selectNodes(XPATH_MSG_ACTIVE_MSGS);
	for(long l = 0, lCount = spMessages->Getlength(); l < lCount; l++)
	{
		spMessage = spMessages->Getitem(l);
		
		if(!CreateStringResource(spMessage, lcidStringTableLang))
			return false;
	}

	if(!EndStringTable())
		return false;

	if(lcidStringTableLang != 3081)
		m_pRCFile->WriteString(sBottom);

	return true;
}

bool MsgExtractor::ParseResourceSource(CString& sTop, CString& sMiddle, CString& sBottom)
{
	CString sError;
	int iStart = 0, iEnd = 0;
	CString sPlaceMarker = TAG_NEW_LANG_INSERT;
	iStart = m_pResourceSourceObj->m_sRCSource.Find(sPlaceMarker, iStart);

	if(iStart < 0)
	{
		sError.Format(IDS_APP_RESOURCE_PLACE_MARKER_ERROR, sPlaceMarker);
		AfxMessageBox(sError);
		return false;
	}

	sTop = m_pResourceSourceObj->m_sRCSource.Left(iStart);

	iStart += sPlaceMarker.GetLength();
	sPlaceMarker = TAG_ENA_LANG_INSERT;
	iEnd = m_pResourceSourceObj->m_sRCSource.Find(sPlaceMarker, iStart);
	if(iEnd < 0)
	{
		sError.Format(IDS_APP_RESOURCE_PLACE_MARKER_ERROR, sPlaceMarker);
		AfxMessageBox(sError);
		return false;
	}
	sMiddle = m_pResourceSourceObj->m_sRCSource.Mid(iStart, iEnd - iStart);

	iStart = iEnd + sPlaceMarker.GetLength();
	sBottom = m_pResourceSourceObj->m_sRCSource.Right(m_pResourceSourceObj->m_sRCSource.GetLength() - iStart);

	return true;
}

bool MsgExtractor::ParseStringTableSource(CString& sTop, CString& sBottom, LCID lcidStringTableLang)
{
	int iStart = 0, iEnd = 0;
	CString sIdentifier, sError, sPlacemarker(TAG_NEW_LANG_INSERT);

	iStart = m_pResourceSourceObj->m_sStringTableSegment.Find(sPlacemarker, iStart);
	if(iStart < 0)
	{
		sError.Format(IDS_APP_RESOURCE_PLACE_MARKER_ERROR, sPlacemarker);
		AfxMessageBox(sError);
		return false;
	}

	sTop = m_pResourceSourceObj->m_sStringTableSegment.Left(iStart);

	iEnd = iStart + sPlacemarker.GetLength();
	sBottom = m_pResourceSourceObj->m_sStringTableSegment.Right(m_pResourceSourceObj->m_sStringTableSegment.GetLength() - iEnd);

	// English Australia already has language markers in resource source
	if(lcidStringTableLang == 3081)
		return true;

	if(m_pResourceSourceObj->GetLanguageDefinition(lcidStringTableLang, sIdentifier))
		sTop.Replace(TAG_LANG_DEFINITION, sIdentifier);

	if(m_pResourceSourceObj->GetLanguageName(lcidStringTableLang, sIdentifier))
	{
		sTop.Replace(TAG_LANG_NAME, sIdentifier);
		sBottom.Replace(TAG_LANG_NAME, sIdentifier);
	}

	if(m_pResourceSourceObj->GetLanguageCodePage(lcidStringTableLang, sIdentifier))
		sTop.Replace(TAG_LANG_CODE_PAGE, sIdentifier);
	
	if(m_pResourceSourceObj->GetLanguageAfxTarget(lcidStringTableLang, sIdentifier))
		sTop.Replace(TAG_AFX_TARGET, sIdentifier);

	return true;
}

bool MsgExtractor::CreateResourceHeader()
{
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());
	CString sTop, sBottom;
	
	if(!ParseHeaderSource(sTop, sBottom))
		return false;

	m_pResourceHeader->WriteString(sTop);

	MSXML2::IXMLDOMNodePtr spMessage = NULL;
	MSXML2::IXMLDOMNodeListPtr spMessages = pDoc->m_messageXMLObj.m_spMessageRoot->selectNodes(XPATH_MSG_ACTIVE_MSGS);
	for(long l = 0, lCount = spMessages->Getlength(); l < lCount; l++)
	{
		spMessage = spMessages->Getitem(l);
		
		if(!CreateResourceIdentifiers(spMessage))
			return false;
	}

	m_pResourceHeader->WriteString(sBottom);

	return true;
}

bool MsgExtractor::ParseHeaderSource(CString& sTop, CString& sBottom)
{
	int iStart = 0, iEnd = 0;
	CString sError, sPlacemarker(TAG_INSERT_DEFINES);

	iStart = m_pResourceSourceObj->m_sHeaderSource.Find(sPlacemarker, iStart);
	if(iStart < 0)
	{
		sError.Format(IDS_APP_RESOURCE_PLACE_MARKER_ERROR, sPlacemarker);
		AfxMessageBox(sError);
		return false;
	}

	sTop = m_pResourceSourceObj->m_sHeaderSource.Left(iStart);

	iEnd = iStart + sPlacemarker.GetLength();

	sBottom = m_pResourceSourceObj->m_sHeaderSource.Right(m_pResourceSourceObj->m_sHeaderSource.GetLength() - iEnd);

	return true;
}

void MsgExtractor::UpdateProgress(UINT uiStringID)
{
	CString sProgress;
	sProgress.LoadString(uiStringID);

	UpdateProgress(sProgress);
}

void MsgExtractor::UpdateProgress(LPCTSTR pszProgress)
{
	if(!m_pProgressWnd || !m_pProgressWnd->IsWindowVisible())
		return;

	m_pProgressWnd->SetWindowText(pszProgress);
}