// Information.h : Declaration of the CInformationMessages

#pragma once

class MessageAccessor
{
public:

	TCHAR m_UniqueID[32];
	TCHAR m_Message[1024];
	TCHAR m_Parameters[256];

	BEGIN_COLUMN_MAP(MessageAccessor)
		COLUMN_ENTRY_LENGTH_STATUS(1, m_UniqueID,	m_dwUniqueIDLength,		m_dwUniqueIDStatus)
		COLUMN_ENTRY_LENGTH_STATUS(2, m_Message,	m_dwMessageLength,		m_dwMessageStatus)
		COLUMN_ENTRY_LENGTH_STATUS(3, m_Parameters,	m_dwParametersLength,	m_dwParametersStatus)
	END_COLUMN_MAP()

	DWORD m_dwMessageStatus, m_dwObsoleteStatus, m_dwParametersStatus, m_dwUniqueIDStatus;
	DWORD m_dwMessageLength, m_dwObsoleteLength, m_dwParametersLength, m_dwUniqueIDLength;


	void GetRowsetProperties(CDBPropSet* pPropSet)
	{
		pPropSet->AddProperty(DBPROP_CANFETCHBACKWARDS, true);
		pPropSet->AddProperty(DBPROP_CANSCROLLBACKWARDS, true);
	}

	void ClearRecordMemory()
	{
		memset(this, 0, sizeof(*this));
	}
};


