#pragma once

// From the base of the original Excel spreadsheet

class Message
{
public:
	enum enumMsgTypes {InfoMessage = 1, WarningMessage, ErrorMessage} _enumMsgTypes;
	
	enum enumMsgErrors {
		errorOK = 0, // No errors where found
		errorMalformed, // The message is malformed
		errorParamCount, // There are more parameters than place markers (Warning Only)
		errorPlaceMarkerCount, // There are more place markers than parameters
		errorSpelling // The string contains a spelling mistake
	} _enumMsgErrors;

	Message(void);
	~Message(void);

	Message(const Message& msg);

	CMap<LCID, LCID, CString, CString&> m_mapMessages;
	ULONG m_ulID, m_ulHelpID, m_ulMsgBoxStyle;
    CString m_ResID;
	USHORT m_usMessageType;
	CStringArray m_rgsParameters;
	bool m_bObsolete, m_bAuditEvent;
};
