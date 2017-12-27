// DataStoreUpdaterATSAlarmQueueItem.cpp: implementation of the DataStoreUpdaterATSAlarmQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#include "DataStoreUpdaterACKCommand.h"

namespace TA_Base_Bus
{
	DataStoreUpdaterACKCommand* DataStoreUpdaterACKCommand::m_Instance=NULL;
	//TA_Base_Core::NonReEntrantThreadLockable DataStoreUpdaterACKCommand::m_singletonLock;
	DataStoreUpdaterACKCommand* DataStoreUpdaterACKCommand::getInstance()
	{
		if (m_Instance!=NULL)
		{
			return m_Instance;
		}		
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance==NULL)
		{
			m_Instance=new DataStoreUpdaterACKCommand();
			m_Instance->interInitialize();
		}
		return m_Instance;
	}
	void DataStoreUpdaterACKCommand::release()
	{
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance!=NULL)
		{
			//m_Instance->terminateAndWait();
			delete m_Instance;
			m_Instance=NULL;
		}		 
	}

	DataStoreUpdaterACKCommand::DataStoreUpdaterACKCommand()//:DataStoreUpdater<long, ATSProfileInfo,ATSAlarmQueueItemManager>("Command")
	{
		//m_Manager=NULL;
		m_isInitialized=FALSE;			

	}

	DataStoreUpdaterACKCommand::~DataStoreUpdaterACKCommand()
	{
// 		if (m_Manager!=NULL)
// 		{
// 			delete m_Manager;
// 			m_Manager=NULL;
// 		}		  
	}
	void DataStoreUpdaterACKCommand::Initialize(std::vector< boost::shared_ptr<AlarmACKCommand> >& batch)
	{	
		if(m_isInitialized)
		{
			return;
		}
		//m_Manager=new AlarmACKCommandManager(AlarmACKCommandManager::MaxItemCount, AlarmACKCommandManager::TotalSizeLength);
		//m_Manager->InitializeMemory(batch);
		m_isInitialized=TRUE;
	}



// 	bool DataStoreUpdaterACKCommand::onDispatchObjects(std::vector<boost::shared_ptr<AlarmACKCommand> >&  batch)
// 	{       
// 		try
// 		{
// 			m_Manager->processEvent(batch);
// 		}	 
// 		catch(...)
// 		{
// 			//LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Caught an unknown exception");								
// 		}		
// 		return TRUE;
// 	}
}
