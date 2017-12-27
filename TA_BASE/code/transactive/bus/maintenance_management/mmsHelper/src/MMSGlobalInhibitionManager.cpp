#include "MMSGlobalInhibitionManager.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/entity_access/src/MmsConnectiontEntityData.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"


namespace TA_Base_Bus
{
	// Static variables
	MMSGlobalInhibitionManager* MMSGlobalInhibitionManager::m_pInstance = NULL;
	TA_Base_Core::NonReEntrantThreadLockable MMSGlobalInhibitionManager::m_singletonLock;
	
	MMSGlobalInhibitionManager& MMSGlobalInhibitionManager::getInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);
		if (NULL == m_pInstance)
		{
			m_pInstance = new MMSGlobalInhibitionManager;
		}
		return *m_pInstance;
	}

	void MMSGlobalInhibitionManager::freeInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	// constructor
	MMSGlobalInhibitionManager::MMSGlobalInhibitionManager(void)
	{
		try
		{
			TA_Base_Core::IEntityDataList entityArray = 
				TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType( 
				TA_Base_Core::MmsConnectiontEntityData::getStaticType());

			if (entityArray.size() != 1)
			{
				// Throw a data exception, connection entity not set up in database
				TA_THROW(TA_Base_Core::DataException("There can be only one Entity NAME", TA_Base_Core::DataException::NOT_UNIQUE,"NAME"));
			}

			m_objInhibitAgent.setEntityName(entityArray[0]->getName(), true);
			
			// Clean up
			for (TA_Base_Core::IEntityDataList::iterator it = entityArray.begin(); it != entityArray.end(); ++it)
			{
				delete (*it);
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred while retreiving MMSAgent name");
		}
	}

	// destructor
	MMSGlobalInhibitionManager::~MMSGlobalInhibitionManager(void)
	{
	}

	bool MMSGlobalInhibitionManager::getGlobalMmsInhibit() 
	{
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		// Allow all exceptions to propogate
		CORBA::Boolean bRetVal;
		CORBA_CALL_RETURN( bRetVal, m_objInhibitAgent, getGlobalMmsInhibit, () );
		return static_cast< bool >( bRetVal ); 
	}

	bool MMSGlobalInhibitionManager::setGlobalMmsInhibit(bool bInhibit, std::string& strSessionID)
	{
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		bool bRetVal = false;
		try
		{
			CORBA_CALL( m_objInhibitAgent,
				setGlobalMmsInhibit,
				( bInhibit, strSessionID.c_str() ) );

			bRetVal = true;
		}
		catch (const TA_Base_Core::AccessDeniedException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "AccessDeniedException", "setGlobalMmsInhibit");
		}
		catch (const TA_Base_Core::DatabaseErrorException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseErrorException", "setGlobalMmsInhibit");
		}
		catch (...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "setGlobalMmsInhibit");
		}

		return bRetVal;
	}
}

