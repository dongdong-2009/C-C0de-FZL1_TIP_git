// DataStoreUpdaterATSAlarmQueueItem.cpp: implementation of the DataStoreUpdaterATSAlarmQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#include "DataStoreUpdaterATSAlarmInfo.h"

namespace TA_Base_Bus
{
	DataStoreUpdaterATSAlarmInfo* DataStoreUpdaterATSAlarmInfo::m_Instance=NULL;
	//TA_Base_Core::NonReEntrantThreadLockable DataStoreUpdaterATSAlarmInfo::m_singletonLock;
    DataStoreUpdaterATSAlarmInfo* DataStoreUpdaterATSAlarmInfo::getInstance()
	{
		if (m_Instance!=NULL)
		{
			return m_Instance;
		}		
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance==NULL)
		{
			m_Instance=new DataStoreUpdaterATSAlarmInfo();
			m_Instance->interInitialize();
		}
		return m_Instance;
	}
	void DataStoreUpdaterATSAlarmInfo::release()
	{
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance!=NULL)
		{
			//m_Instance->terminateAndWait();
			delete m_Instance;
			m_Instance=NULL;
		}		 
	}
	
	DataStoreUpdaterATSAlarmInfo::DataStoreUpdaterATSAlarmInfo()//:DataStoreUpdater<long, ATSAlarmQueueItem,ATSAlarmQueueItemManager>("Command")
	{
		 
		m_isInitialized=FALSE;			
		
	}
	
	DataStoreUpdaterATSAlarmInfo::~DataStoreUpdaterATSAlarmInfo()
	{
		 	  
	}
	void DataStoreUpdaterATSAlarmInfo::Initialize(std::vector< boost::shared_ptr<ExAlarmQueueItem> >& batch)
	{	
		//TA_Base_Core::ThreadGuard guard(m_initilizedLock);
		if(m_isInitialized)
		{
			return;
		}
       
		m_isInitialized=TRUE;
       
	}
	
	
	
	 
}
