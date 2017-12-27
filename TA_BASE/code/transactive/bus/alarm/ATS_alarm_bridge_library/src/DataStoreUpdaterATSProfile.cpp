// DataStoreUpdaterATSAlarmQueueItem.cpp: implementation of the DataStoreUpdaterATSAlarmQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#include "DataStoreUpdaterATSProfile.h"
using namespace I_ALARM_PROVIDER;
namespace TA_Base_Bus
{
	DataStoreUpdaterATSProfile* DataStoreUpdaterATSProfile::m_Instance=NULL;
	//TA_Base_Core::NonReEntrantThreadLockable DataStoreUpdaterATSProfile::m_singletonLock;
	DataStoreUpdaterATSProfile* DataStoreUpdaterATSProfile::getInstance()
	{
		if (m_Instance!=NULL)
		{
			return m_Instance;
		}		
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance==NULL)
		{
			m_Instance=new DataStoreUpdaterATSProfile();
			m_Instance->interInitialize();
		}
		return m_Instance;
	}
	void DataStoreUpdaterATSProfile::release()
	{
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance!=NULL)
		{
			//m_Instance->terminateAndWait();
			delete m_Instance;
			m_Instance=NULL;
		}		 
	}

	DataStoreUpdaterATSProfile::DataStoreUpdaterATSProfile()//:DataStoreUpdater<long, ATSProfileInfo,ATSAlarmQueueItemManager>("Command")
	{
		 
		m_isInitialized=FALSE;			

	}

	DataStoreUpdaterATSProfile::~DataStoreUpdaterATSProfile()
	{
		 	  
	}
	void DataStoreUpdaterATSProfile::Initialize(std::vector< boost::shared_ptr<ExtProfileInfo> >& batch)
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
