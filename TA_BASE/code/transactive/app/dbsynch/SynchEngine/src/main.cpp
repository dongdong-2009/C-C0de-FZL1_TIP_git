// SynchEnginer.cpp : Defines the entry point for the console application.
//

#include <signal.h>

#include "UniqueCheck.h"
#include "SyncAQMsgManager.h" 
#include "core/utilities/src/RunParams.h"
#include "SynModeMonitor.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DbSyncException.h"
#include "UtilityFun.h"

using namespace TA_DB_Sync;

bool g_b_main_continue = true;
void usr_signal(int SigNo)
{
	if (SigNo == SIGINT)
	{
		g_b_main_continue = false;
	}
	
#ifndef WIN32
	if (SigNo == SIGHUP)
	{
	}
#endif


}



int main( int argc, char* argv[] )
{ 
	   
	signal(SIGINT, usr_signal);
#ifndef WIN32
	signal(SIGHUP, usr_signal);	//close putty
#endif

	bool bSet = TA_Base_Core::RunParams::getInstance().parseCmdLine(argc, argv);
	if (!bSet) return 0;
	
	std::string strMode = TA_Base_Core::RunParams::getInstance().get(defCmdLine_Mode);
	std::string strConfigFile = TA_Base_Core::RunParams::getInstance().get(defCmdLine_DefaultFile);	
	std::string strSynID = TA_Base_Core::RunParams::getInstance().get(defCmdLine_SynID);	
		
	TA_DB_Sync::CSynModeMonitor* pSynModeMonitor = NULL;

	//--mode=active
	//strMode = "Control";
	if (strMode.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
			"Error message: %s", "the running mode of the SynchEngine is not set.");
		exit(TA_AQ_Lib::Ret_Failed);
	}

	//	TA_DB_Sync::g_UniqueCheck();	

	if (!strSynID.empty())
	{
		TA_AQ_Lib::trim(strSynID);
		pSynModeMonitor = new TA_DB_Sync::CSynModeMonitor(strSynID);
	}
	else
	{
		pSynModeMonitor = new TA_DB_Sync::CSynModeMonitor();
	}

	TA_AQ_Lib::trim(strMode);
	pSynModeMonitor->setModeData(strMode);
	pSynModeMonitor->start();	

	try
	{
		TA_DB_Sync::CSyncAQMsgManager::getInstance().setSynchMode(strMode);
		TA_DB_Sync::CSyncAQMsgManager::getInstance().startSynchMsg(strConfigFile);
	}
	catch (DbSyncException&)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
			"Error Code: %d, Error message: %s", Err_Config_Invalid, defErrMsg(Err_Config_Invalid));
	}	

	pSynModeMonitor->terminateAndWait();
	DEF_DELETE(pSynModeMonitor);
	TA_DB_Sync::CSyncAQMsgManager::removeInstance();

	return 0;
}

