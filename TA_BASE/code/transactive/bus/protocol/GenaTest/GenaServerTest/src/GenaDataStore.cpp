//#include "StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "GenaDataStore.h"

GenaDataStore::GenaDataStore(void)
{
}

GenaDataStore::~GenaDataStore(void)
{
}

T_GENATopicDataList GenaDataStore::getAllData(const std::string& topicName )
{
	FUNCTION_ENTRY("getAllData");
	T_GENATopicDataList objRetVal;

	T_DatatStoreIter itr = m_dataStoreMap.find(topicName);
	if(m_dataStoreMap.end() != itr)
	{
		/*T_GENATopicDataList tempDataList = itr->second;
		for (int nCtr=0; nCtr< tempDataList.size(); nCtr++)
		{
			GENATopicDataPtr pData = tempDataList[nCtr];

			GENATopicDataPtr pNewData(new GENATopicData(pData->getObjectID(), pData->getTopicName()));
			pNewData->setTopicURI(pData->getTopicURI());
			pNewData->setUpdateType(GENA_BIRTH);
			pNewData->setAttributes(pData->getAttributes());

			objRetVal.push_back(pNewData);
		}*/
		objRetVal = itr->second;
	}
	FUNCTION_EXIT;
	return objRetVal;
}

void GenaDataStore::synchronizeBegin(const std::string& topicName)
{
	FUNCTION_ENTRY("synchronizeBegin");
	FUNCTION_EXIT;
}

void GenaDataStore::synchronizeEnd(const std::string& topicName)
{
	FUNCTION_ENTRY("synchronizeEnd");
	FUNCTION_EXIT;
}

void GenaDataStore::updateDataImpl(const std::string& topicName, GENATopicDataPtr pData)
{
	FUNCTION_ENTRY("updateDataImpl");

	T_DatatStoreIter itr = m_dataStoreMap.find(topicName);

	if(itr == m_dataStoreMap.end())
	{
		T_GENATopicDataList DataListVect;
		std::string test = pData->getTopicName();
		DataListVect.push_back(pData);
		m_dataStoreMap[topicName]= DataListVect;
	}
	else
	{
		itr->second.push_back(pData);
	}
	
	FUNCTION_EXIT;
}