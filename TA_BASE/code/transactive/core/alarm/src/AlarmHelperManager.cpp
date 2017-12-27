/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmHelperManager.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/02/04 10:03:05 $
  * Last modified by:  $Author: Noel $
  * 
  * description
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmHelperBase.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
	// Initialise statics
	TA_Base_Core::NonReEntrantThreadLockable AlarmHelperManager::m_singletonLock;

	AlarmHelperManager* AlarmHelperManager::m_theAlarmHelperManager = NULL;

	AlarmHelperManager::~AlarmHelperManager()
	{
		FUNCTION_ENTRY("AlarmHelperManager::~AlarmHelperManager()");
		//do not call cleanUp here, jianghp
		//TD17994,jianghp
		delete m_alarmHelper;
		m_alarmHelper = NULL;
		
		delete m_nonUniqueAlarmHelper;
		m_nonUniqueAlarmHelper = NULL;
		
		delete m_alarmModificationHelper;
		m_alarmModificationHelper = NULL;
		//TD17994,jianghp	
		FUNCTION_EXIT;
	}

	void AlarmHelperManager::cleanUp()
	{
		FUNCTION_ENTRY("AlarmHelperManager::cleanUp()");
		if ( NULL != m_alarmHelperBase )
		{
			// Stop the alarm helper base
			m_alarmHelperBase->terminateAndWait();

			// Clean up the alarm helper base
			delete m_alarmHelperBase;
			m_alarmHelperBase = NULL;
		}

		// Clean up the alarm helper manager
		TA_Base_Core::ThreadGuard guard(m_singletonLock); //protect: threads release this object at same time.
		if (NULL != m_theAlarmHelperManager) {
			delete m_theAlarmHelperManager;
			m_theAlarmHelperManager = NULL;
		}
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "finishing clean the singlton" );
		FUNCTION_EXIT;
	}


	AlarmHelperManager& AlarmHelperManager::getInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);

		if ( m_theAlarmHelperManager == NULL )
		{
			m_theAlarmHelperManager = new AlarmHelperManager();
		}

		return *m_theAlarmHelperManager;
	}


	AlarmHelper& AlarmHelperManager::getAlarmHelper()
	{
		// Create the alarm helper base if it has not yet been created
		initialiseAlarmHelperBase();

		TA_ASSERT(NULL != m_alarmHelperBase, "AlarmHelperBase was NULL");

		//TD17994,jianghp
		if (NULL == m_alarmHelper)
		{
			m_alarmHelper = new AlarmHelper(*m_alarmHelperBase);
		}
		return *m_alarmHelper;
	}


	NonUniqueAlarmHelper& AlarmHelperManager::getNonUniqueAlarmHelper()
	{
		// Create the alarm helper base if it has not yet been created
		initialiseAlarmHelperBase();

		TA_ASSERT(NULL != m_alarmHelperBase, "AlarmHelperBase was NULL");
		//TD17994,jianghp
		if (NULL == m_nonUniqueAlarmHelper)
		{
			m_nonUniqueAlarmHelper = new NonUniqueAlarmHelper(*m_alarmHelperBase);
		}
		return *m_nonUniqueAlarmHelper;
	}


	AlarmModificationHelper& AlarmHelperManager::getModificationAlarmHelper()
	{
		// Create the alarm helper base if it has not yet been created
		initialiseAlarmHelperBase();

		TA_ASSERT(NULL != m_alarmHelperBase, "AlarmHelperBase was NULL");
		//TD17994,jianghp
		if (NULL == m_alarmModificationHelper)
		{
			m_alarmModificationHelper = new AlarmModificationHelper(*m_alarmHelperBase);
		}
        return *m_alarmModificationHelper;
    }


	void AlarmHelperManager::initialiseAlarmHelperBase()
	{
		// Guard the construction of the AlarmHelperBase
		TA_Base_Core::ThreadGuard guard( m_alarmHelperBaseLock );

		// Check to see if the helper is already created
		if ( NULL == m_alarmHelperBase )
		{
			// Create it if not
			m_alarmHelperBase = new AlarmHelperBase();

			m_alarmHelperBase->start();
		}
	}


	AlarmHelperManager::AlarmHelperManager()
		: m_alarmHelperBase( NULL ), m_alarmHelper(NULL), m_nonUniqueAlarmHelper(NULL), m_alarmModificationHelper(NULL)
	{
	}


}; // namespace TA_Base_Core

