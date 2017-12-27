// DataStoreUpdaterAlarmAlarmACKCommand.cpp: implementation of the DataStoreUpdaterAlarmAlarmACKCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "core/synchronisation/src/ThreadGuard.h"
#include "DataStoreUpdaterAlarmACKCommand.h"

using namespace I_ALARM_PROVIDER;

namespace TA_Base_Bus
{
	DataStoreUpdaterAlarmACKCommand* DataStoreUpdaterAlarmACKCommand::m_Instance=NULL;
	TA_Base_Core::NonReEntrantThreadLockable DataStoreUpdaterAlarmACKCommand::m_singletonLock;
    DataStoreUpdaterAlarmACKCommand* DataStoreUpdaterAlarmACKCommand::getInstance()
	{
		if (m_Instance!=NULL)
		{
			return m_Instance;
		}		
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance==NULL)
		{
			m_Instance=new DataStoreUpdaterAlarmACKCommand();
			m_Instance->interInitialize();
		}
		return m_Instance;
	}
	void DataStoreUpdaterAlarmACKCommand::release()
	{
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance!=NULL)
		{
			//m_Instance->terminateAndWait();
			delete m_Instance;
			m_Instance=NULL;
		}		 
	}
	
	DataStoreUpdaterAlarmACKCommand::DataStoreUpdaterAlarmACKCommand()//:DataStoreUpdater<long, AlarmACKCommand,AlarmACKCommandManager>("Command")
	{
		 
		m_isInitialized=FALSE;			
		
	}
	
	DataStoreUpdaterAlarmACKCommand::~DataStoreUpdaterAlarmACKCommand()
	{
		 		  
	}
	void DataStoreUpdaterAlarmACKCommand::Initialize(std::vector< boost::shared_ptr<AlarmACKCommand> >& batch)
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
