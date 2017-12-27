/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/TA_BASE/transactive/app/checker/ISCSWatchDogService/src/WatchDogService.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2009/02/26 19:46:43 $
  * Last modified by:  $Author: bjteamleads $
  *
*/
// AlarmStoreService.cpp : Defines the entry point for the console application.
//



#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif // defined _MSC_VER

#include "bus/generic_service/src/ServiceMain.h"
#include "WatchDogServiceUser.h"
#include "GenericService.h"


template<> TA_Base_App::GenericService<TA_Base_App::WatchDogServiceUser>* TA_Base_App::GenericService<TA_Base_App::WatchDogServiceUser>::m_singleton = NULL;
template<> TA_Base_Core::ReEntrantThreadLockable TA_Base_App::GenericService<TA_Base_App::WatchDogServiceUser>::m_singletonLock;


int main(int argc, char* argv[])
{
	TA_Base_Core::RunParams::getInstance().set("DebugFile", "C:\\WatchoDog.log");
	TA_Base_App::GenericService<TA_Base_App::WatchDogServiceUser>::getInstance().initialise(std::string("WatchDogService"));
	TA_Base_App::GenericService<TA_Base_App::WatchDogServiceUser>::getInstance().run(argc, argv);
	TA_Base_App::GenericService<TA_Base_App::WatchDogServiceUser>::removeInstance();
	
	return 0;
}
