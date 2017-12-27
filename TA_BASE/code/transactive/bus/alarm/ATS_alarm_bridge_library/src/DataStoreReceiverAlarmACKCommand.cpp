// DataStoreReceiverAlarmACKCommand.cpp: implementation of the DataStoreReceiverAlarmACKCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "DataStoreReceiverAlarmACKCommand.h"


//#include "core/utilities/src/DebugUtil.h"
//using TA_Base_Core::DebugUtil;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{
	
    DataStoreReceiverAlarmACKCommand* DataStoreReceiverAlarmACKCommand::m_Instance=NULL;
	//TA_Base_Core::NonReEntrantThreadLockable DataStoreReceiverAlarmACKCommand::m_singletonLock;
    DataStoreReceiverAlarmACKCommand* DataStoreReceiverAlarmACKCommand::getInstance()
	{
		if (m_Instance!=NULL)
		{
			return m_Instance;
		}
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance==NULL)
		{
			m_Instance=new DataStoreReceiverAlarmACKCommand();	
			m_Instance->initialize();
		}
		return m_Instance;
	}
	void DataStoreReceiverAlarmACKCommand::release()
	{
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance!=NULL)
		{
			delete m_Instance;
			m_Instance=NULL;
		}
	}
	DataStoreReceiverAlarmACKCommand::DataStoreReceiverAlarmACKCommand()//:DataStoreReceiver<long, CommandCache, CommandCacheManager>("Command")
	{
		
	}
	
	DataStoreReceiverAlarmACKCommand::~DataStoreReceiverAlarmACKCommand()
	{
		
	}
 
	
	
	 
	
	
}
