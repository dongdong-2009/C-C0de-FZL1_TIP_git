#pragma once

class ParameterAccessor
{
public:
	TCHAR m_ParameterName[128];
	TCHAR m_Description[1024];

	BEGIN_COLUMN_MAP(ParameterAccessor)
		COLUMN_ENTRY_LENGTH_STATUS(1, m_ParameterName, m_dwParameterLength, m_dwParameterStatus)
		COLUMN_ENTRY_LENGTH_STATUS(2, m_Description, m_dwDescriptionLength, m_dwDescriptionStatus)
	END_COLUMN_MAP()

	DWORD m_dwParameterStatus;
	DWORD m_dwDescriptionStatus;

	DWORD m_dwParameterLength;
	DWORD m_dwDescriptionLength;

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