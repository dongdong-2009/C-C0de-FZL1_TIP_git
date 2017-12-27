// DataStoreUpdaterAlarmAlarmACKCommand.cpp: implementation of the DataStoreUpdaterAlarmAlarmACKCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "core/synchronisation/src/ThreadGuard.h"
#include "DataStoreUpdaterAlarmRequest.h"

using namespace I_ALARM_PROVIDER;

namespace TA_Base_Bus
{
	DataStoreUpdaterAlarmRequest* DataStoreUpdaterAlarmRequest::m_Instance=NULL;
	TA_Base_Core::NonReEntrantThreadLockable DataStoreUpdaterAlarmRequest::m_singletonLock;
    DataStoreUpdaterAlarmRequest* DataStoreUpdaterAlarmRequest::getInstance()
	{
		if (m_Instance!=NULL)
		{
			return m_Instance;
		}		
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance==NULL)
		{
			m_Instance=new DataStoreUpdaterAlarmRequest();
			m_Instance->interInitialize();
		}
		return m_Instance;
	}

	void DataStoreUpdaterAlarmRequest::release()
	{
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance!=NULL)
		{
			//m_Instance->terminateAndWait();
			delete m_Instance;
			m_Instance=NULL;
		}		 
	}
	
	DataStoreUpdaterAlarmRequest::DataStoreUpdaterAlarmRequest()//:DataStoreUpdater<long, AlarmACKCommand,AlarmACKCommandManager>("Command")
	{
		 
		m_isInitialized=FALSE;			
		
	}
	
	DataStoreUpdaterAlarmRequest::~DataStoreUpdaterAlarmRequest()
	{
		 		  
	}
	void DataStoreUpdaterAlarmRequest::Initialize(std::vector< boost::shared_ptr<I_ALARM_PROVIDER::AlarmRequest> >& batch)
	{	
		if(m_isInitialized)
		{
			return;
		}
		TA_Base_Core::ThreadGuard guard(m_initilizedLock);
		if(m_isInitialized)
		{
			return;
		}
        
		m_isInitialized=TRUE;
        //setBatchingProperties(2,20,40);
		//start();
		
	}
	
	
	
	 
}
