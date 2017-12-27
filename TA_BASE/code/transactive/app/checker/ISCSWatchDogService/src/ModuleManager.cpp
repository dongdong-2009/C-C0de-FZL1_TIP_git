// ModuleManager.cpp: implementation of the ModuleManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ModuleManager.h"
//#include "AlarmStoreWatcher.h"
#include "AlarmStoreCmdWatcher.h"
#include "OpcBridgeWatcher.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DebugUtilMacros.h"

#pragma warning(disable : 4786)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ModuleManager::ModuleManager()
{
}

ModuleManager::~ModuleManager()
{
	std::map<unsigned int, WatchedModule*>::iterator itMap = m_ModuleMap.begin();
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"ModuleManager destruction, module count:%d!", m_ModuleMap.size() );
	for ( ; itMap != m_ModuleMap.end(); ++itMap )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"ModuleManager clean module [%s]!", itMap->second->getServiceName().c_str() );
		delete itMap->second;
		itMap->second = NULL;
	}
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ModuleManager destructed!!!" );
}
void ModuleManager::stopAll()
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"ModuleManager::stopAll() start..." );
	std::map<unsigned int, WatchedModule*>::iterator itMap = m_ModuleMap.begin();
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"ModuleManager destruction, module count:%d!", m_ModuleMap.size() );
	for ( ; itMap != m_ModuleMap.end(); ++itMap )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"ModuleManager clean module [%s]!", itMap->second->getServiceName().c_str() );
		itMap->second->stopAll();
	}
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"ModuleManager::stopAll() finished" );
}

void ModuleManager::createModule( const unsigned int moduleID )
{
	WatchedModule* pModule = NULL;
	std::map<unsigned int, WatchedModule*>::iterator itMap = m_ModuleMap.find(moduleID);
	if( itMap != m_ModuleMap.end() )
	{
		pModule = itMap->second;
	}
	// create new module
	switch( moduleID )
	{
	case MODULEID_OPCBRIDGE:
		pModule = new OpcBridgeWatcher();
		break;
		
	case MODULEID_ALARMSTORE:
		// test use, with a GUI
// 		if ( pModule == NULL )
// 		{
// 			AlarmStoreWatcher* pAlarm = new AlarmStoreWatcher();
// 		}
// 		else
// 		{
// 			pModule->show();
// 		}
		break;
	case MODULEID_ALARMSTORE_CMD:
		pModule = new AlarmStoreCmdWatcher();
		break;
	default:
// 		MessageBox(NULL, _T("Invalid Module"), _T("CreateModule"), MB_OK|MB_ICONERROR);
	    break;
	}
	if( pModule != NULL )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"ModuleManager insert module:[%s]", pModule->getServiceName().c_str() );
		m_ModuleMap.insert(std::make_pair(moduleID, pModule));
	}
}
