/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_TIP/TA_BASE/transactive/app/checker/ISCSWatchDogService/src/WatchDogServiceUser.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/03/12 11:31:40 $
  * Last mofified by:  $Author: liuyang $
  *						copy from AlarmStroreService
 *
  * This class provides the Generic Agent User interface that is used by the system controller
  * It will manage the instance of the store servant.
  */

#if !defined(WATCH_DOG_SERVICE_USER)
#define WATCH_DOG_SERVICE_USER

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <map>
#include <vector>

#include "app/checker/ISCSWatchDogService/src/ModuleManager.h"

#include "core/process_management/src/UtilityInitialiser.h"
#include "core/utilities/src/DebugUtilInit.h"

// use GenericService.h
// we should implement our service in a thread and provide 
// stopService()	stop service
// startService()	for start main thread of our service

namespace TA_Base_App
{
	class WatchDogServiceUser 
    {

		public:

			/**
			  * Constructor
			  */
			WatchDogServiceUser(int argc, char* argv[])
				:m_pModuleManager(NULL)
			{
				TA_Base_Core::UtilityInitialiser::initialiseUtilities(argc, argv);
			}

			/**
			  * Destructor
			  */
			virtual ~WatchDogServiceUser()
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"WatchDogServiceUser desctruction called!!!");
				stopService();
			}

			virtual void stopService();
			virtual void startService();

		private:
			ModuleManager*				m_pModuleManager;
    };
}

#endif // !defined(WATCH_DOG_SERVICE_USER)
