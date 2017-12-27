#pragma once

#include "..\Message.h"
#include "..\XMLObjects\XMLDefines.h"
#include "..\XMLObjects\ResourceSrcXMLObj.h"
#include "..\UnicodeIOFile\UnicodeIOFile.h"

class MsgExtractor
{
public:
	MsgExtractor(void);
	~MsgExtractor(void);

	//enum enumMsgTypes {InfoMessage = 1, WarningMessage, ErrorMessage} _enumMsgTypes;

	// Excel to XML
	bool ExtractMsgsFromExcel(CWnd* pProgressWnd);
	bool ExtractMsgsFromExcel(LPCTSTR szWorkSheet, Message::enumMsgTypes MsgType);
	bool ConvertExcelStyle(CString& sMessage, CStringArray& rgsParameters);
	bool ExtractParamsFromExcel();

	// The Excel provider checks the registry to determine the number of rows to scan when
	// determining the size of the string buffer to use. The default is 8, so any rows that
	// have a length greater than 256 after row 8 will be truncated. Set to 0, to scan 18384. 
	// Reset to remove performance hit. See MSDN Q281517.
	bool SetGuessRowSizeParameter(bool bReset = false);
	DWORD m_dwPrevExcelGuessRows; // Store previous value for reset

	// Excel Connection
	CDataSource m_dataSourceExcel;
	CSession m_sessionExcel;
	bool OpenDataSource();
	void CloseDataSource();

	// XML to rc
	ResourceSrcXMLObj* m_pResourceSourceObj;
	LCID m_lcidLanguageToExport;
	bool m_bIncludeENA;

	CWnd* m_pProgressWnd; // control to display progress messages
	void UpdateProgress(UINT uiStringID);
	void UpdateProgress(LPCTSTR pszProgress);

	bool ExportMessagesToRC();
	bool CreateStringResource(MSXML2::IXMLDOMNodePtr& spMessage, LCID lcidStringTableLang);
	bool CreateResourceHeader();
	bool CreateResourceIdentifiers(MSXML2::IXMLDOMNodePtr& spMessage);
	bool OutputResourceHeader(LPCTSTR szResourceID, LPCTSTR szResourceValue);
	bool OutputStringTable(LPCTSTR sResourceID, LPCTSTR sAusMessage);
	bool CreateStringTable(LCID lcidStringTableLang);
	bool EndStringTable();
	bool ParseResourceSource(CString& sTop, CString& sMiddle, CString& sBottom);
	bool ParseStringTableSource(CString& sTop, CString& sBottom, LCID lcidStringTableLang);
	bool ParseHeaderSource(CString& sTop, CString& sBottom);
	void FormatEscapeSequences(CString& sMessage);

	UINT m_uiStringCount;

	CString m_sSourcePath, m_sTargetPath, m_sTargetRCPath, m_sTargetHPath;
	bool CreateOutputFiles();
	bool CloseOutputFiles();
	CStdioFile* m_pResourceHeader;
	UnicodeIOFile* m_pRCFile;
};
