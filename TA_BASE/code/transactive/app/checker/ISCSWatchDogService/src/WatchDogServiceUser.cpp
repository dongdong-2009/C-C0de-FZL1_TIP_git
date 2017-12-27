/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_TIP/TA_BASE/transactive/app/checker/ISCSWatchDogService/src/WatchDogServiceUser.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/10/31 12:12:40 $
  * Last mofified by:  $Author: liuyang $
  *						copy from AlarmStroreService
  *
  * This class provides the Generic Agent User interface that is used by the system controller
  * It will do nothing in this implementation.
  */

#pragma warning(disable:4284)   // return type for 'identifier::operator->' is not a UDT or reference to a UDT
#pragma warning(disable:4503)

#include "WatchDogServiceUser.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_App;


void TA_Base_App::WatchDogServiceUser::stopService()
{
	if ( m_pModuleManager != NULL )
	{
		m_pModuleManager->stopAll();
		delete m_pModuleManager;
		m_pModuleManager = NULL;
	}
}

void TA_Base_App::WatchDogServiceUser::startService()
{
	m_pModuleManager = new ModuleManager();
	if ( m_pModuleManager != NULL )
	{
		m_pModuleManager->createModule( MODULEID_OPCBRIDGE );
//		m_pModuleManager->createModule( MODULEID_ALARMSTORE_CMD );
	}
}