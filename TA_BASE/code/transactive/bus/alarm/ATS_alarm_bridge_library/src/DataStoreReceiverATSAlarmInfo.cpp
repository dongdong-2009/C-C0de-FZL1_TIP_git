// DataStoreReceiverATSAlarmInfo.cpp: implementation of the DataStoreReceiverATSAlarmInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "DataStoreReceiverATSAlarmInfo.h"
 
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{

	//std::string, ATSAlarmQueueItem, ATSAlarmInfoManager
	
    DataStoreReceiverATSAlarmInfo* DataStoreReceiverATSAlarmInfo::m_Instance=NULL;
 
    DataStoreReceiverATSAlarmInfo* DataStoreReceiverATSAlarmInfo::getInstance()
	{
		if (m_Instance!=NULL)
		{
			return m_Instance;
		}
		 
		if (m_Instance==NULL)
		{
			m_Instance=new DataStoreReceiverATSAlarmInfo();	
			m_Instance->initialize();
		}
		return m_Instance;
	}
	void DataStoreReceiverATSAlarmInfo::release()
	{
		 
		if (m_Instance!=NULL)
		{
			delete m_Instance;
			m_Instance=NULL;
		}
	}
	DataStoreReceiverATSAlarmInfo::DataStoreReceiverATSAlarmInfo()//:DataStoreReceiver<unsigned long, SessionCache, SessionCacheManager>("Session")
	{
		
	}
	
	DataStoreReceiverATSAlarmInfo::~DataStoreReceiverATSAlarmInfo()
	{
		
	}
	
}
