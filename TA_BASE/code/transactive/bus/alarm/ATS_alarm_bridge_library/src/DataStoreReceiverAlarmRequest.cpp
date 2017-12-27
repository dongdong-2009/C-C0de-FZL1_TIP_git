// DataStoreReceiverAlarmACKCommand.cpp: implementation of the DataStoreReceiverAlarmACKCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "DataStoreReceiverAlarmRequest.h"


//#include "core/utilities/src/DebugUtil.h"
//using TA_Base_Core::DebugUtil;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{
	
    DataStoreReceiverAlarmRequest* DataStoreReceiverAlarmRequest::m_Instance=NULL;
	//TA_Base_Core::NonReEntrantThreadLockable DataStoreReceiverAlarmACKCommand::m_singletonLock;
    DataStoreReceiverAlarmRequest* DataStoreReceiverAlarmRequest::getInstance()
	{
		if (m_Instance!=NULL)
		{
			return m_Instance;
		}
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance==NULL)
		{
			m_Instance=new DataStoreReceiverAlarmRequest();	
			m_Instance->initialize();
		}
		return m_Instance;
	}
	void DataStoreReceiverAlarmRequest::release()
	{
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance!=NULL)
		{
			delete m_Instance;
			m_Instance=NULL;
		}
	}
	DataStoreReceiverAlarmRequest::DataStoreReceiverAlarmRequest()//:DataStoreReceiver<long, CommandCache, CommandCacheManager>("Command")
	{
		
	}
	
	DataStoreReceiverAlarmRequest::~DataStoreReceiverAlarmRequest()
	{
		
	}
 
	
	
	 
	
	
}
