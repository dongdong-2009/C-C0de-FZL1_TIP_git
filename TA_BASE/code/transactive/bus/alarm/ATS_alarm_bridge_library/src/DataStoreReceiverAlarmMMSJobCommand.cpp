// DataStoreReceiverAlarmMMSJobCommand.cpp: implementation of the DataStoreReceiverAlarmMMSJobCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "DataStoreReceiverAlarmMMSJobCommand.h"


//#include "core/utilities/src/DebugUtil.h"
//using TA_Base_Core::DebugUtil;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{
	
    DataStoreReceiverAlarmMMSJobCommand* DataStoreReceiverAlarmMMSJobCommand::m_Instance=NULL;
	//TA_Base_Core::NonReEntrantThreadLockable DataStoreReceiverAlarmMMSJobCommand::m_singletonLock;
    DataStoreReceiverAlarmMMSJobCommand* DataStoreReceiverAlarmMMSJobCommand::getInstance()
	{
		if (m_Instance!=NULL)
		{
			return m_Instance;
		}
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance==NULL)
		{
			m_Instance=new DataStoreReceiverAlarmMMSJobCommand();	
			m_Instance->initialize();
		}
		return m_Instance;
	}
	void DataStoreReceiverAlarmMMSJobCommand::release()
	{
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance!=NULL)
		{
			delete m_Instance;
			m_Instance=NULL;
		}
	}
	DataStoreReceiverAlarmMMSJobCommand::DataStoreReceiverAlarmMMSJobCommand()//:DataStoreReceiver<long, CommandCache, CommandCacheManager>("Command")
	{
		
	}
	
	DataStoreReceiverAlarmMMSJobCommand::~DataStoreReceiverAlarmMMSJobCommand()
	{
		
	}
// 	
// 	CommandCacheManager* DataStoreReceiverAlarmMMSJobCommand::getDataManager()
// 	{
// 		return new CommandCacheManager(CommandCacheManager::MaxItemCount,CommandCacheManager::TotalSizeLength);
// 		//return NULL;
// 	}
	
	 
		
	 
	
	
}
