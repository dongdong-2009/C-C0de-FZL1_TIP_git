// DataStoreUpdaterATSAlarmQueueItem.cpp: implementation of the DataStoreUpdaterATSAlarmQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#include "DataStoreUpdaterMMSJobCommand.h"

namespace TA_Base_Bus
{
	DataStoreUpdaterMMSJobCommand* DataStoreUpdaterMMSJobCommand::m_Instance=NULL;
	//TA_Base_Core::NonReEntrantThreadLockable DataStoreUpdaterMMSJobCommand::m_singletonLock;
	DataStoreUpdaterMMSJobCommand* DataStoreUpdaterMMSJobCommand::getInstance()
	{
		if (m_Instance!=NULL)
		{
			return m_Instance;
		}		
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance==NULL)
		{
			m_Instance=new DataStoreUpdaterMMSJobCommand();
			m_Instance->interInitialize();
		}
		return m_Instance;
	}
	void DataStoreUpdaterMMSJobCommand::release()
	{
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance!=NULL)
		{
			//m_Instance->terminateAndWait();
			delete m_Instance;
			m_Instance=NULL;
		}		 
	}

	DataStoreUpdaterMMSJobCommand::DataStoreUpdaterMMSJobCommand()//:DataStoreUpdater<long, ATSProfileInfo,ATSAlarmQueueItemManager>("Command")
	{
	
		m_isInitialized=FALSE;			

	}

	DataStoreUpdaterMMSJobCommand::~DataStoreUpdaterMMSJobCommand()
	{
	  
	}
	void DataStoreUpdaterMMSJobCommand::Initialize(std::vector< boost::shared_ptr<AlarmMmsJobCommand> >& batch)
	{	
		if(m_isInitialized)
		{
			return;
		}
		//m_Manager=new AlarmMMSJobCommandManager(AlarmMMSJobCommandManager::MaxItemCount, AlarmMMSJobCommandManager::TotalSizeLength);
		//m_Manager->InitializeMemory(batch);
		m_isInitialized=TRUE;
	}



	
}
