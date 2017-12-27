// DataStoreUpdaterISCSAlarmQueueItem.cpp: implementation of the DataStoreUpdaterISCSAlarmQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#include "DataStoreUpdaterISCSProfile.h"
using namespace I_ALARM_PROVIDER;

namespace TA_Base_Bus
{
	DataStoreUpdaterISCSProfile* DataStoreUpdaterISCSProfile::m_Instance=NULL;
	//TA_Base_Core::NonReEntrantThreadLockable DataStoreUpdaterISCSProfile::m_singletonLock;
	DataStoreUpdaterISCSProfile* DataStoreUpdaterISCSProfile::getInstance()
	{
		if (m_Instance!=NULL)
		{
			return m_Instance;
		}		
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance==NULL)
		{
			m_Instance=new DataStoreUpdaterISCSProfile();
			m_Instance->interInitialize();
		}
		return m_Instance;
	}
	void DataStoreUpdaterISCSProfile::release()
	{
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance!=NULL)
		{
			//m_Instance->terminateAndWait();
			delete m_Instance;
			m_Instance=NULL;
		}		 
	}

	DataStoreUpdaterISCSProfile::DataStoreUpdaterISCSProfile()//:DataStoreUpdater<long, ISCSProfileInfo,ISCSAlarmQueueItemManager>("Command")
	{	 
		m_isInitialized=FALSE;			
	}

	DataStoreUpdaterISCSProfile::~DataStoreUpdaterISCSProfile()
	{
		 	  
	}
	void DataStoreUpdaterISCSProfile::Initialize(std::vector< boost::shared_ptr<ISCSProfileInfo> >& batch)
	{	
		if(m_isInitialized)
		{
			return;
		}
		//TA_Base_Core::ThreadGuard guard(m_initilizedLock);
		if(m_isInitialized)
		{
			return;
		}
	 
		m_isInitialized=TRUE;
	}
 
}
