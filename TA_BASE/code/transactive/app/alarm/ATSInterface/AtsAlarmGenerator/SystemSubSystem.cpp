#include "StdAfx.h"
#include "SystemSubSystem.h"
#include <atlconv.h>

#ifdef _DEBUG
	#import "../../../../../../sdk/win32_nd/ATSBridgeUtil.tlb" named_guids
#else
	#import "../../../../../../sdk/win32_nr/ATSBridgeUtil.tlb" named_guids
#endif

using namespace ATSBridgeUtilLib;

// Initialize to NULL
CSystemSubSystem* CSystemSubSystem::m_pInstance = NULL;

// @function: Constructor
CSystemSubSystem::CSystemSubSystem(void)
{
	USES_CONVERSION;
	CoInitialize(0); // Initialize COM mechanism
	{
		CComPtr<IATSEventSystemList> pEventSystems;
		CComPtr<IATSStringList> pEventTypes;
		CComPtr<IATSStringList> pAlarmTypes;
		CComPtr<IATSBridgeUtility> pATSBridgeUtility;
		HRESULT hr = pATSBridgeUtility.CoCreateInstance(CLSID_ATSBridgeUtility, NULL, CLSCTX_INPROC_SERVER);
		long err;
		err = pATSBridgeUtility->DoEventInfoQuery ( & pEventSystems, & pEventTypes, & pAlarmTypes );
		if(err == 0 /*ATS_QUERY_OK*/)
		{
			for(int i=0; i<pEventSystems->GetSize(); i++)
			{
				CComPtr<IATSEventSystem> pATSEventSystem;
				CComPtr<IATSStringList> pSubSystemList;
				SystemSubSystem* pobjSysSubSystem = NULL;
				
				pobjSysSubSystem = new SystemSubSystem;
				pEventSystems->GetItem(i,&pATSEventSystem);
				pobjSysSubSystem->strSystemName = _com_util::ConvertBSTRToString(pATSEventSystem->Name);
 				pATSEventSystem->GetSubSystemList( &pSubSystemList);

				for( int j=0; j<pSubSystemList->GetSize(); j++)
				{
					CComBSTR bstrSubSystemName;
					bstrSubSystemName.Attach(pSubSystemList->GetItem(j));
					std::string strSubSystemName = _com_util::ConvertBSTRToString(bstrSubSystemName);

					pobjSysSubSystem->subSystems.push_back(strSubSystemName);
				}
				
				m_objSystemList.push_back(pobjSysSubSystem);
			}
		}
	}

	CoUninitialize(); // Uninitialize COM mechanism
}

// @function: Destructor
CSystemSubSystem::~CSystemSubSystem(void)
{
}

// @function: getInstance

CSystemSubSystem& CSystemSubSystem::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CSystemSubSystem();
	}
	return *m_pInstance;
}

// @function:	freeInstace
// @purpose:	This function is responsible for release the instance of this singleton class
// @Return :	none
void CSystemSubSystem::freeInstance()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}


void CSystemSubSystem::getSystemList(STRING_LIST& systemList)
{
	systemList.clear();

	for (int nIdx = 0; nIdx < (int)m_objSystemList.size(); nIdx++)
	{
		std::string strSystemName;
		strSystemName = m_objSystemList[nIdx]->strSystemName.c_str();
		systemList.push_back(strSystemName);
	}
}

void CSystemSubSystem::getAllSubSystemList(STRING_LIST& subSystemList)
{
	subSystemList.clear();
	for (int nIdx = 0; nIdx < (int)m_objSystemList.size(); nIdx++)
	{
		for (int nSubIdx = 0; nSubIdx < (int)(m_objSystemList[nIdx]->subSystems.size()); nSubIdx++)
		{
			std::string strSubSystemName;
			strSubSystemName = m_objSystemList[nIdx]->subSystems[nSubIdx].c_str();
			subSystemList.push_back(strSubSystemName);
		}
	}
}

void CSystemSubSystem::getSubSystemList(std::string& strSystem, STRING_LIST& subSystemList)
{
	subSystemList.clear();
	for (int nIdx = 0; nIdx < (int)m_objSystemList.size(); nIdx++)
	{
		if (_strcmpi(m_objSystemList[nIdx]->strSystemName.c_str(), strSystem.c_str()) == 0)
		{
			for (int nSubIdx = 0; nSubIdx < (int)(m_objSystemList[nIdx]->subSystems.size()); nSubIdx++)
			{
				std::string strSubSystemName;
				strSubSystemName = m_objSystemList[nIdx]->subSystems[nSubIdx].c_str();
				subSystemList.push_back(strSubSystemName);
			}
		}
	}
}
