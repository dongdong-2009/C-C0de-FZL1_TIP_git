#include "bus/scada/wf_global_inhibit/src/WFGlobalInhibitionManager.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/wf_global_inhibit/src/WFDefs.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "bus/scada/common_library/src/CommonDefs.h"

namespace TA_Base_Bus
{
 	
	// Static variables
	WFGlobalInhibitionManager* WFGlobalInhibitionManager::m_pInstance = NULL;
	TA_Base_Core::NonReEntrantThreadLockable WFGlobalInhibitionManager::m_singletonLock;
	
	WFGlobalInhibitionManager& WFGlobalInhibitionManager::getInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);
		if (NULL == m_pInstance)
		{
			m_pInstance = new WFGlobalInhibitionManager;
		}
		return *m_pInstance;
	}

	void WFGlobalInhibitionManager::freeInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	// constructor
	WFGlobalInhibitionManager::WFGlobalInhibitionManager(void)
	:
	m_initialized(false)
	{
		TA_Base_Core::IEntityDataList entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithAddressLikeToken(
			TA_Base_Core::DataPointEntityData::getStaticType(),
			TA_Base_Bus::WF_GLOBAL_INHIBIT_ADDRESS);
		
		if (1 == entityList.size())
		{
			m_initialized = true;
			std::string strAgentName = entityList[0]->getAgentName();
			m_objInhibitAgent.setCorbaName(strAgentName, strAgentName + TA_Base_Bus::WF_GLOBAL_INHIBIT_INTERFACE);
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "WF Global Inhibit data point must be one data point.");
		}
		
		//clear all of the entitydata.
		for (unsigned int i = 0 ; i < entityList.size(); ++i)
		{
			delete entityList[i];
		}
		
	}

	// destructor
	WFGlobalInhibitionManager::~WFGlobalInhibitionManager(void)
	{
	}

	bool WFGlobalInhibitionManager::getWFGlobalInhibitState(bool& globalInhibitState) 
	{
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		// Allow all exceptions to propogate
		if (false == m_initialized)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "WF Global Inhibit CORBA Interface is not initialized");
			return false;
		}	
		
		CORBA::Boolean bRetVal;
		try
		{
			CORBA_CALL_RETURN( bRetVal, m_objInhibitAgent, getWFGlobalInhibit, () );
			globalInhibitState =  static_cast< bool >( bRetVal ); 
		}
		catch ( TA_Base_Core::OperationModeException& expOpModeException )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "OperationModeException", "getWFGlobalInhibit");
			return false;
		}
		catch ( CORBA::SystemException& expCorbaException )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "SystemException", "getWFGlobalInhibit");
			return false;
		}
		catch ( TA_Base_Core::ObjectResolutionException& expObjectResolution )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "ObjectResolutionException", "getWFGlobalInhibit");
			return false;
		}
		catch (const TA_Base_Core::DatabaseErrorException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseErrorException", "getWFGlobalInhibit");
			return false;
		}		
		catch( ... )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown Exception", "getWFGlobalInhibit");
			return false;
		}		
		return true;
	}

	bool WFGlobalInhibitionManager::setWFGlobalInhibitState(bool bInhibit, std::string& strSessionID)
	{
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		if (false == m_initialized)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "WF Global Inhibit CORBA Interface is not initialized");
			return false;
		}	
		
		bool bRetVal = false;
		try
		{
			CORBA_CALL( m_objInhibitAgent,setWFGlobalInhibit,( bInhibit, strSessionID.c_str() ) );

			bRetVal = true;
		}
		catch ( TA_Base_Core::OperationModeException& expOpModeException )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "OperationModeException", "setWFGlobalInhibit");
			return false;
		}
		catch ( CORBA::SystemException& expCorbaException )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "SystemException", "setWFGlobalInhibit");
			return false;
		}
		catch ( TA_Base_Core::ObjectResolutionException& expObjectResolution )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "ObjectResolutionException", "setWFGlobalInhibit");
			return false;
		}
		catch (const TA_Base_Core::AccessDeniedException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "AccessDeniedException", "setWFGlobalInhibit");
			return false;
		}
		catch (const TA_Base_Core::DatabaseErrorException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseErrorException", "setWFGlobalInhibit");
			return false;
		}
		catch (...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown Exception", "setWFGlobalInhibit");
			return false;
		}

		return bRetVal;
	}
	
	bool WFGlobalInhibitionManager::verifyPassoword(std::string& strSessionID,std::string& strPassword)
	{
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		if (false == m_initialized)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "WF Global Inhibit CORBA Interface is not initialized");
			return false;
		}	
		
		bool bRet;
		try
		{
			CORBA_CALL( m_objInhibitAgent,
				verifyPassword,
				( strSessionID.c_str(),strPassword.c_str() ) );

			return true; 
		}
		catch ( TA_Base_Core::OperationModeException& expOpModeException )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "OperationModeException", "verifyPassoword");
			return false;
		}
		catch ( CORBA::SystemException& expCorbaException )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "SystemException", "verifyPassoword");
			return false;
		}
		catch ( TA_Base_Core::ObjectResolutionException& expObjectResolution )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "ObjectResolutionException", "verifyPassoword");
			return false;
		}
		catch (const TA_Base_Core::AccessDeniedException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "AccessDeniedException", "verifyPassoword");
			return false;

		}
		catch (const TA_Base_Core::DatabaseErrorException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseErrorException", "verifyPassoword");
			return false;
		}
		catch (...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown Exception", "verifyPassoword");
			return false;
		}
	}
}

