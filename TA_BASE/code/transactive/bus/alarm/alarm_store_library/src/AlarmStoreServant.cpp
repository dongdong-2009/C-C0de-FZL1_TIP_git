/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmStoreServant.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 17:50:12 $
  * Last modified by:  $Author: Noel $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmStoreServant.cpp
//  Implementation of the Class AlarmStoreServant
//  Created on:      24-Jan-2004 04:12:03 PM
///////////////////////////////////////////////////////////

#pragma warning(disable:4284)   // return type for 'identifier::operator->' is not a UDT or reference to a UDT
#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#include <winsock2.h>  // Used by getHostName
#include "AlarmStoreServant.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/alarm/shared_alarm/src/SharedAlarm.h"

#include "core/data_access_interface/entity_access/src/AlarmAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreReceiverATSAlarmInfo.h"
#include "AlarmFilterSet.h"

// Filters
#include "AlarmFilterByProfile.h"
#include "AlarmFilterByDate.h"
#include "AlarmFilterByType.h"
#include "AlarmFilterByAssociatedEntityKey.h"
#include "AlarmFilterByAssociatedEntityName.h"
#include "AlarmFilterByLocation.h"
#include "AlarmFilterByOperator.h"
#include "AlarmFilterBySeverity.h"
#include "AlarmFilterByState.h"
#include "AlarmFilterBySubsystem.h"
#include "AlarmFilterByAssetName.h"
#include "AlarmFilterByDescription.h"
//#include "SharedAlarmAccessFactory.h"
#include "AlarmAccessAdaptor.h"
#include "AlarmFilterBySubsystemType.h"
#include "AlarmFilterBySystemType.h"
#include "AlarmFilterByOMType.h"

#include "ace/OS_NS_time.h"

#include "core/synchronisation/src/ProcessGuard.h"
#include "core/synchronisation/src/NonReEntrantProcessLockable.h"
#include "core/synchronisation/src/IProcessLockable.h"
// Sorting
#include "AlarmStoreMonitor.h"

const int REBUILD_ALARM_ACTION	= 0;
const std::string PARAM_NOREMAPSHAREDMEM = "NoRemapSharedMem";
const std::string PARAM_MAXALLALARMCOUNT = "MaxAllAlarmCount";

namespace // unnamed
{
	// Ensure that the sort set includes the time somewhere. This is necessary to ensure that
	// inserts into the alarm store are deterministic (eg. if the sort criteria is severity and
	// there are many alarms with the same severity as the alarm to be inserted then unless the
	// time is also used to sort the alarms, the alarm will be inserted into a 'random' position).
	// PW3460
	TA_Base_Core::IAlarmStoreCorbaDef::SortInfo convertToTimedSortSet(const TA_Base_Core::IAlarmStoreCorbaDef::SortInfo& sort)
	{
		// Does this sort set include the time?
		bool sortIncludesTime = false;
		unsigned int origLength = sort.length();
		for(unsigned int i=0; i<origLength; ++i)
		{
			if(sort[i].type == TA_Base_Core::IAlarmStoreCorbaDef::SORT_TIME)
			{
				sortIncludesTime = true;
				break;
			}
		}

		if(sortIncludesTime)
		{
			// Nothing to do
			return sort;
		}
		else
		{
			// Create a new sort set, with the time as the last sort entry
			
			// Copy the original sequence and increase the length by one
			TA_Base_Core::IAlarmStoreCorbaDef::SortInfo newSortSet(sort);
			newSortSet.length(origLength+1);

			// Append new sort by ascending time
			newSortSet[origLength].type = TA_Base_Core::IAlarmStoreCorbaDef::SORT_TIME;
			newSortSet[origLength].direction = TA_Base_Core::IAlarmStoreCorbaDef::SORT_DESCENDING;

			return newSortSet;
		}
	}

} // unnamed


using namespace TA_Base_Bus;


//***************
// Construction *
//***************


AlarmStoreServant::AlarmStoreServant(const std::string &storeEntityName, AlarmStoreUpdateListener& listener)
				  : //ISessionUpdateObserver(listener.getSessionUpdateSubject()),
                    //IDutyUpdateObserver(listener.getDutyUpdateSubject()),
					IRightsChangeObserver(),
                    m_alarmAgentInterfaceManager(NULL),
				    m_alarmStoreManager(NULL),
					m_rightsLibrary(NULL),
					m_rebuildAlarmThread(NULL),
					m_aliaveTimeStamp(0),
					//m_TimeBefore(0),
					m_pCheckWorker(NULL),
					m_ATSAlarmObserver(NULL)
{
	FUNCTION_ENTRY("Constructor");

	try{
	// TD #10075
	// Build the rights library here instead of inside AlarmFilterByProfile
	TA_Base_Bus::RightsLibraryFactory rlf;
    m_rightsLibrary = rlf.buildRightsLibrary();
	
	m_alarmStoreManager = new AlarmStoreManager(m_rightsLibrary);
	TA_ASSERT(m_alarmStoreManager!=NULL,"Unable to create store manager.");

	m_alarmAgentInterfaceManager = new AlarmAgentInterfaceManager(*m_alarmStoreManager);
	TA_ASSERT(m_alarmAgentInterfaceManager!=NULL,"Unable to create index callback manager.");

	//TA_Base_Core::IEntityDataList  agentList;
	TA_Base_Core::RunParams::getInstance().set("ResolveTimeoutMs", "20000");
	//agentList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(TA_Base_Core::AlarmAgentEntityData::getStaticType(),false);
	 

	//Move the following initialize functions to m_rebuildAlarmThread  --luohuirong
	m_rebuildAlarmThread = new AlarmStoreServant::ActionThread(*this, REBUILD_ALARM_ACTION);
	m_rebuildAlarmThread->start();

	// start the monitor thread
	if (TA_Base_Core::RunParams::getInstance().isSet("DisableAlarmStoreMonitor"))
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DisableAlarmStoreMonitor");		
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "EnableAlarmStoreMonitor");
		AlarmStoreMonitor::getInstance().start();
	}

	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "cought an exception in AlarmStoreServant::AlarmStoreServant ");
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AlarmStoreServant::AlarmStoreServant out");
	FUNCTION_EXIT;
}

void AlarmStoreServant::init()
{
	FUNCTION_ENTRY("AlarmStoreServant::init");
	try
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "======AlarmStoreServant::init====");
		TA_Base_Core::IEntityDataList  agentList;
		//TA_Base_Core::RunParams::getInstance().set("ResolveTimeoutMs", "20000");
		agentList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(TA_Base_Core::AlarmAgentEntityData::getStaticType(),false);

		bool alwaysRemapSharedMem = TA_Base_Core::RunParams::getInstance().isSet(PARAM_NOREMAPSHAREDMEM.c_str())?false:true;
		ULONG ulMaxAllAlarmCount = MAXALLALARMCOUNT;
		if (TA_Base_Core::RunParams::getInstance().isSet(PARAM_MAXALLALARMCOUNT.c_str()))
		{
			ulMaxAllAlarmCount = atol(TA_Base_Core::RunParams::getInstance().get(PARAM_MAXALLALARMCOUNT.c_str()).c_str());
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "====== call AlarmAccessAdaptor::getInstance().init() begin ==== alwaysRemapSharedMem=%d", (int)alwaysRemapSharedMem);
		AlarmAccessAdaptor::getInstance().init(ulMaxAllAlarmCount, alwaysRemapSharedMem);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "====== call AlarmAccessAdaptor::getInstance().init() end ====");

		//station should only receive alarm from its location, luohuirong  15/12/2009
		unsigned long locationkey=0;
		std::string   locationname="";
		m_isOCCLocation=false;	 
		std::string hostname="";		
		TA_Base_Core::IConsole* console=NULL;
		m_currentLoginLocaitonKey=0;

		char arrhostName[512];
		int hNameLength = 512;

		if( gethostname(arrhostName, hNameLength) != 0 )
		{
			//TA_THROW( SessionException("Host name could not be retrieved so the console cannot be found") );
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Host name could not be retrieved so the console cannot be found");
		}
		hostname = arrhostName;

		try
		{
			console=TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromAddress(hostname);
		}	 
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "this console is not configured on DB");
		}
		if (console!=NULL)
		{
			locationname=console->getLocationName();
			locationkey=console->getLocation();
			m_currentLoginLocaitonKey=locationkey;
			delete console;
		}
		std::transform(locationname.begin(), locationname.end(), locationname.begin(),(int(*)(int))toupper);
		if (locationname=="OCC")
		{
			m_isOCCLocation=true;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "OCC MFT Alarm store server");
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Station MFT Alarm store server, %s", locationname.c_str());
		}

		// DTL_Alarm_Display_Filter
		m_alarmStoreManager->setSystemDisplayFilter(m_currentLoginLocaitonKey);
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "====== call DataStoreReceiverATSAlarmInfo::getInstance()->AddObserver begin ====");
		m_ATSAlarmObserver= new ATSAlarmObserver(m_alarmStoreManager, m_currentLoginLocaitonKey);
		m_ATSAlarmObserver->start();
		//DataStoreReceiverATSAlarmInfo::getInstance()->AddObserver(m_ATSAlarmObserver);
		//DataStoreReceiverATSAlarmInfo::getInstance()->start();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "====== call DataStoreReceiverATSAlarmInfo::getInstance()->AddObserver end ====");
		//LocationAccessFactory::getLocationByKey
		// Ensure each is monitored
		unsigned int iLoop;
		m_alarmStoreManager->m_lLoadAgents =0; //= agentList.size();
		for (iLoop=0;iLoop<agentList.size();iLoop++)
		{		
			TA_Base_Core::IEntityData* agent = agentList.at(iLoop);		
			if (NULL == agent)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL == agent");
				continue;
			}		
			if (m_isOCCLocation || locationkey==agent->getLocation())
			{
				m_alarmStoreManager->m_lLoadAgents++;			
			}	 
		}	  

		for (iLoop=0;iLoop<agentList.size();iLoop++)
		{		
			TA_Base_Core::IEntityData* agent = agentList.at(iLoop);		
			if (NULL == agent)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL == agent");
				continue;
			}
			m_allAlarmAgentNameMap.insert(AlarmAgentNameMap::value_type(agent->getLocation(), agent->getName()));
			if (m_isOCCLocation || locationkey==agent->getLocation())
			{			

				m_alarmAgentInterfaceManager->startAgentMonitoring(agent->getName(),agent->getLocation(),true);
			}
			delete agent;
			agent = NULL;
		}
		agentList.clear();

		//register for duty change
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "====== call rightsLibrary.registerObserver() ====");
		if (m_rightsLibrary !=NULL)	m_rightsLibrary->registerObserver(*this);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "====== call activateServant() begin ====");
		// Register with Corba
		activateServant(); //WithName(storeEntityName);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "====== call activateServant() end ====");

		// start check work here
		m_pCheckWorker = new CheckWorker(this);

		// bootstrap the object as well... Knowing that the AlarmStoreService has already pinned our port
		TA_Base_Core::CorbaUtil::getInstance().setServantKey( this, 
			TA_Base_Core::IAlarmStoreCorbaDef::DEFAULT_ALARM_STORE_NAME );
        Sleep(300);
		AlarmAccessAdaptor::getInstance().notifyClientRemap();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "======AlarmStoreServant::out====");
	}
	catch(...)
	{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "caught an exception in AlarmStoreServant::AlarmStoreServant ");
	}
	FUNCTION_EXIT;
}


AlarmStoreServant::~AlarmStoreServant()
{
	FUNCTION_ENTRY("Destructor");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmStoreServant Destructor] Entry");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmStoreServant Destructor] Step1");
	if(m_rebuildAlarmThread != NULL)
	{
		m_rebuildAlarmThread->terminateAndWait();
		delete m_rebuildAlarmThread;
		m_rebuildAlarmThread = NULL;
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmStoreServant Destructor] Step2");
	if ( m_pCheckWorker != NULL )
	{
		m_pCheckWorker->terminateAndWait();
		delete m_pCheckWorker;
		m_pCheckWorker = NULL;
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmStoreServant Destructor] Step3");
	deactivateServant();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmStoreServant Destructor] Step4");
	m_alarmAgentInterfaceManager->stopAllAgentMonitoring();

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmStoreServant Destructor] Step7");
	delete m_rightsLibrary;
	m_rightsLibrary = NULL;

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmStoreServant Destructor] Step8");
	delete m_alarmAgentInterfaceManager;
	m_alarmAgentInterfaceManager = NULL;

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmStoreServant Destructor] Step9");
	delete m_alarmStoreManager;
	m_alarmStoreManager = NULL;

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmStoreServant Destructor] Step10");
	AlarmAccessAdaptor::getInstance().clean();
	AlarmAccessAdaptor::freeInstance();

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmStoreServant Destructor] Step11");
	if (NULL != m_ATSAlarmObserver)
	{
		delete m_ATSAlarmObserver;
		m_ATSAlarmObserver = NULL;
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmStoreServant Destructor] Exit");
	AlarmStoreMonitor::getInstance().removeInstance();
	FUNCTION_EXIT;
}

//*****************************
// IDL Implementation Methods *
//*****************************

void AlarmStoreServant::setStoreMonitorAllLocations()
{
	FUNCTION_ENTRY("setStoreMonitorAllLocations");
	TA_Base_Core::IEntityDataList  agentList;

	LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			 "AlarmsStoreServant has been called to monitor all locations");

	AlarmAgentNameMap::iterator	itr = m_allAlarmAgentNameMap.begin();

	// Ensure each is monitored
	for (; itr != m_allAlarmAgentNameMap.end(); itr++ )
	{
		m_alarmAgentInterfaceManager->startAgentMonitoring(itr->second,itr->first,false);
	}

	FUNCTION_EXIT;
}

void AlarmStoreServant::setStoreMonitorLocation(unsigned long location)
{
	FUNCTION_ENTRY("setStoreMonitorLocation");

	TA_Base_Core::IEntityDataList  agentList;
	LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			 "AlarmsStoreServant has been called to monitor location %d", location);

	// Stop all agent monitoring except the local one specified
	std::vector<std::string> excludeList;
	excludeList.push_back(m_allAlarmAgentNameMap[location]);
	m_alarmAgentInterfaceManager->stopAllAgentMonitoringExcept(excludeList);

	// ensure local agent is connected
	m_alarmAgentInterfaceManager->startAgentMonitoring(m_allAlarmAgentNameMap[location],location,false);

	FUNCTION_EXIT;
}


void AlarmStoreServant::setStoreMonitorLocations(const TA_Base_Core::IAlarmStoreCorbaDef::LocationList& locations)
{
	FUNCTION_ENTRY("setStoreMonitorLocations");
	if (m_isOCCLocation)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "setStoreMonitorLocations OCC at=%d", m_currentLoginLocaitonKey);		
		return;		
	}
	unsigned long locationkey=0;
	if(locations.length()==1)
	{		
		for (unsigned int iLoop=0;iLoop<locations.length();iLoop++)
		{
		    locationkey=locations[iLoop];			
		}
        if (locationkey!=m_currentLoginLocaitonKey)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "start to stop monitor at=%d", m_currentLoginLocaitonKey);		
			m_alarmAgentInterfaceManager->stopAllAgentMonitoring();
			AlarmAgentNameMap::iterator  iter=m_allAlarmAgentNameMap.find(locationkey);
			if (iter!=m_allAlarmAgentNameMap.end())
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "start to monitor at=%d", locationkey);		
				m_alarmAgentInterfaceManager->startAgentMonitoring(iter->second,iter->first,false);
				m_currentLoginLocaitonKey=locationkey;
			}

		}
	}
	else
	{
		unsigned int iLoop=0;
		std::vector<std::string> agentNames;
		for (iLoop=0;iLoop<locations.length();iLoop++)
		{
			locationkey=locations[iLoop];	
			AlarmAgentNameMap::iterator   iter=m_allAlarmAgentNameMap.find(locationkey);
            if (iter!=m_allAlarmAgentNameMap.end())
			{
				agentNames.push_back(iter->second);
			}
		}
		m_alarmAgentInterfaceManager->stopAllAgentMonitoringExcept(agentNames);

		for (iLoop=0;iLoop<locations.length();iLoop++)
		{
			locationkey=locations[iLoop];	
			AlarmAgentNameMap::iterator   iter=m_allAlarmAgentNameMap.find(locationkey);
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "start to monitor multiple at=%d", locationkey);		
            if (iter!=m_allAlarmAgentNameMap.end())
			{
				m_alarmAgentInterfaceManager->startAgentMonitoring(iter->second,iter->first,false);
			}
		}
		

	}



// 	std::vector<std::string>   agentList;
// 	
// 	// Get all the required agents
// 	for (unsigned int iLoop=0;iLoop<locations.length();iLoop++)
// 	{
// 		agentList.push_back(m_alarmAgentNameMap[locations[iLoop]]);
// 	}
// 
// 	// Stop all agent monitoring except the local one specified by the locations
// 	m_alarmAgentInterfaceManager->stopAllAgentMonitoringExcept(agentList);
// 
// 	// ensure agents are connected
// 	for (iLoop=0;iLoop<agentList.size();iLoop++)
// 	{
// 		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
// 			 "AlarmsStoreServant has been called to monitor location %d", locations[iLoop]);
// 		m_alarmAgentInterfaceManager->startAgentMonitoring(m_alarmAgentNameMap[locations[iLoop]], locations[iLoop]);
// 	}
	FUNCTION_EXIT;
}


void AlarmStoreServant::setSessionId(const char* sessionId)
{
	FUNCTION_ENTRY("setSessionId");
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Session ID is changing to %s", sessionId);
    m_alarmStoreManager->setSessionId(sessionId);
	FUNCTION_EXIT;
}


TA_Base_Core::IAlarmStoreCorbaDef::IndexReference AlarmStoreServant::createIndex(const TA_Base_Core::IAlarmStoreCorbaDef::FilterInfo& filter,
									const TA_Base_Core::IAlarmStoreCorbaDef::SortInfo& sort, 
								//	TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback,
									unsigned long cs_pos,
									bool isHighestSeverityNeeded,
									bool isDefault)
{
	FUNCTION_ENTRY("createIndex");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AlarmStoreServant is called to createIndex.");
    long lRetVal = 0; //New Alarm Store

	// Create the new filter set from the corba filter information
	AlarmFilterSet* filterSet = createFilter(filter);
   
	if (filterSet==NULL)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "createIndex, failed to run createFilter");
		return lRetVal ;
	}

	// Ensure that the sort set includes the time somewhere. This is necessary to ensure that
	// inserts into the alarm store are deterministic (eg. if the sort criteria is severity and
	// there are many alarms with the same severity as the alarm to be inserted then unless the
	// time is also used to sort the alarms, the alarm will be inserted into a 'random' position).
	// PW3460
	TA_Base_Core::IAlarmStoreCorbaDef::SortInfo timedSortSet = convertToTimedSortSet(sort);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"AlarmStoreServant call to createIndex");
    AlarmAccessAdaptor::getInstance().buildIndexWithFilter(filterSet, &AlarmFilterSet::passesFilters, cs_pos, isDefault);
	lRetVal = 1;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "NewAlarmStore:::AlarmStoreServant is called to buildIndexWithDuty&buildIndexWithFilter.");

	FUNCTION_EXIT;
	return lRetVal;
}



void AlarmStoreServant::setStorePreFilter(const TA_Base_Core::IAlarmStoreCorbaDef::FilterInfo& filter)
{
	FUNCTION_ENTRY("setStorePreFilter");
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"AlarmStoreServant::setStorePreFilter come in ");
	// Create the new filter set from the corba filter information
	AlarmFilterSet* filterSet = createFilter(filter);

	if (filterSet==NULL)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "setStorePreFilter, failed to run createFilter");
		return;
	}

	bool profileChanged=false;
	{
		TA_Base_Core::NonReEntrantProcessLockable DutyLock(DUTYCACHELOCK);  
		TA_Base_Core::ProcessGuard   DutyGuard (DutyLock );
		profileChanged=m_alarmStoreManager->hasPrefilterChanged(*filterSet);
		// Check if the pre-filter is the same
		if (profileChanged)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				 "New pre-filter, is reloading alarms for all locations");    
			// Create the index in the store
			m_alarmStoreManager->setPreFilter(filterSet);			
		}
		else
		{
			// Delete the unwanted filter set
			delete filterSet;
			filterSet = NULL;
			// No change !
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				 "Pre-filter the same, therefore not applied.");

		}
	}

	if (profileChanged)
	{
		TA_Base_Core::NonReEntrantProcessLockable DutyLock(DUTYCACHELOCK);  
		TA_Base_Core::ProcessGuard   DutyGuard (DutyLock );		
		// Reload all alarms from the agents
		//	m_alarmAgentInterfaceManager->reloadAllAlarms();
		/*------------------------------------- New Alarm Store -----------------------------------------*/
		//TA_Base_Bus::TA_SharedAlarm	cs;
		//cs.buildIndexWithDuty(m_alarmStoreManager,&AlarmStoreManager::passesPrefilter);		
		/*-----------------------------------------------------------------------------------------------*/
        AlarmAccessAdaptor::getInstance().buildIndexWithDuty(m_alarmStoreManager);
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				 "NewAlarmStore:::New pre-filter, reloaded alarms for all locations,buildIndexWithDuty");
	}

	FUNCTION_EXIT;
}

void AlarmStoreServant::dutyChanged(const std::string&  session)
{
	FUNCTION_ENTRY("dutyChanged");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AlarmStoreServant dutyChanged. Session id changed. Param session id %s.", session.c_str());
    
// 	bool sessionChanged=false;
//     TA_Base_Core::NonReEntrantProcessLockable DutyLock(DUTYCACHELOCK);  
//  	TA_Base_Core::ProcessGuard   DutyGuard (DutyLock );
// 	// Notify the alarm store of this change and find out whether the pre-filter changed
//     if (m_alarmStoreManager->sessionDetailsChanged(session))
//     {
		// If the pre-filter has been updated, then need to reload all alarms from the agents
		// m_alarmAgentInterfaceManager->reloadAllAlarms(); 
		/*-----------------------------------------New Alarm Store-------------------------------------------*/
		//TA_Base_Bus::TA_SharedAlarm	cs;
		//cs.buildIndexWithDuty(m_alarmStoreManager,&AlarmStoreManager::passesPrefilter);
		// yanrong++: reduce access to rights agent.
		if(m_alarmStoreManager->m_preFilterSet != NULL)
		{
			m_alarmStoreManager->m_preFilterSet->resetFilters();
		}
		 
		// ++yanrong.
		//xinyang++ CL-20937.a  CL-20899
		//m_rightsLibrary->resetCache(); // Reset Cache for rightsLibary,Obtains the current all alarms the filtration condition. GF check visible of alarm based on right only.
		//CL-20937.a
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"begin NewAlarmStore:::buildIndexWithDuty for duty changed. Param session id %s.", session.c_str());
		AlarmAccessAdaptor::getInstance().buildIndexWithDuty(m_alarmStoreManager);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"end NewAlarmStore:::buildIndexWithDuty for duty changed. Param session id %s.", session.c_str());
		/*---------------------------------------------------------------------------------------------------*/
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Reloaded all alarms for duty changed. Param session id %s.", session.c_str());
//     }

	FUNCTION_EXIT;
}

void AlarmStoreServant::dutyUpdate(std::map<unsigned long, std::list<unsigned long> >& LocationSubSytemList)
{
	FUNCTION_ENTRY("dutyUpdate");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AlarmStoreServant::dutyUpdate coming in. updating %d locations", LocationSubSytemList.size());

	AlarmAccessAdaptor::getInstance().UpdateDutyIndex(LocationSubSytemList, m_alarmStoreManager);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AlarmStoreServant::dutyUpdate coming out");
	FUNCTION_EXIT;
}


void AlarmStoreServant::removeDuty()
{
	FUNCTION_ENTRY("removeDuty");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AlarmStoreServant::removeDuty coming in");

	//AlarmAccessAdaptor::getInstance().clean();
	AlarmAccessAdaptor::getInstance().removeISCSDutyAlarms();

	m_alarmStoreManager->removePrefilter();

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AlarmStoreServant::removeDuty coming out");
	FUNCTION_EXIT;
}

void AlarmStoreServant::OnRightsChange(std::string sessionId, ERightsChangeType changeType, DutyChanges& changes )
{       
	std::string dutyChangeSession=sessionId.c_str();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AlarmStoreServant OnRightsChange. Session id changed. Param session id %s, ChangeType %d", dutyChangeSession.c_str(), (int)changeType);

	TA_Base_Core::NonReEntrantProcessLockable DutyLock(DUTYCACHELOCK);  
	TA_Base_Core::ProcessGuard   DutyGuard (DutyLock );

	// Notify the alarm store of this change and find out whether the pre-filter changed
	if (m_alarmStoreManager->sessionDetailsChanged(sessionId))
	{		
		switch(changeType)
		{
		case RC_SessionBegin:
			// Todo: Check if we have previously process the session begin
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SessionBegin Called with session ID = %s", sessionId.c_str());
			break;
		case RC_SessionEnd:
			// Remove the SessionID and all the filters
			removeDuty();
			break;
		case RC_SessionOverrideBegin:
		case RC_SessionOverrideEnd:
			// Perform a complete Duty Change //
			dutyChanged(sessionId);

			break;
		case RC_DutyUpdate:
		case RC_DutyRecovery:
			// Perform duty change based on the location and subsystem
			dutyUpdate(changes);

			break;
		default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown changeType has been passed: changeType = %d", (int)changeType);
			break;
		}
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AlarmStoreServant OnRightsChange. Session id changed. Param session id %s.", dutyChangeSession.c_str());
}

// Consider removing
void AlarmStoreServant::sessionOverrideBegin( const std::string& session, unsigned long profile )
{
	FUNCTION_ENTRY("sessionOverrideBegin");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "sessionOverrideBegin called. Session id: %s and profile id: %ul passed as param.", session.c_str(), profile);

    // Can just use the duty changed method to handle this - it carries out the same work
    dutyChanged(session);

	FUNCTION_EXIT;

}

// Consider removing
void AlarmStoreServant::sessionOverrideEnd( const std::string& session, unsigned long profile )
{
	FUNCTION_ENTRY("sessionOverrideEnd");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "sessionOverrideEnd called. Session id: %s and profile id: %ul passed as param.", session.c_str(), profile);

    // Can just use the duty changed method to handle this - it carries out the same work
    dutyChanged(session);

	FUNCTION_EXIT;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AlarmStoreServant::newAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarmDetail)
{
	FUNCTION_ENTRY("newAlarm");
	AlarmAccessAdaptor::getInstance().insertAlarm(*(const_cast<TA_Base_Core::AlarmDetailCorbaDef*>(&(alarmDetail))), true);
	FUNCTION_EXIT;
}

void AlarmStoreServant::updateAlarm(const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate)
{
	FUNCTION_ENTRY("updateAlarm");
	try
	{
	   AlarmAccessAdaptor::getInstance().updateAlarm(alarmUpdate);
	} 
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "updateAlarm error");
	}
	FUNCTION_EXIT;
}

void AlarmStoreServant::RemoveAlarms(const TA_Base_Core::StringSequence& p_alarms)
{
	FUNCTION_ENTRY("RemoveAlarms");
	for( unsigned int i = 0; i < p_alarms.length(); i++ )
	{
        std::string alarmID(p_alarms[i]);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RemoveAlarms ID=%s",alarmID.c_str());
        AlarmAccessAdaptor::getInstance().RemoveAlarm(alarmID.c_str());
	}
	FUNCTION_EXIT;
}

void AlarmStoreServant::ActionThread::run()
{
	FUNCTION_ENTRY("ActionThread::run");
	try
	{	 		 
	    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "setPriority of thread to HIGH");
		setPriority(2);
	    m_parent.init();		 
	}	
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "====== call SharedAlarmAccessFactory::getInstance().init()  ====");
	}
	
// 	while(!m_terminated)
// 	{
// 		m_wakeCondition.wait();
// 
// 		if(!m_terminated)
// 		{
// 			if(m_actionType == REBUILD_ALARM_ACTION)
// 			{
// 				if(m_parent.m_alarmStoreManager->m_preFilterSet != NULL)
// 				{
// 					m_parent.m_alarmStoreManager->m_preFilterSet->resetFilters();
// 				}
// 
// 				SharedAlarmAccessFactory::getInstance().buildIndexWithDuty(m_parent.m_alarmStoreManager, &AlarmStoreManager::passesPrefilter);
// 			}
// 		}
// 	}

	// this let thread can restart.
	m_terminated = false;
	FUNCTION_EXIT;
}

void AlarmStoreServant::ActionThread::terminate()
{
	m_terminated = true;
	//m_wakeCondition.signal();
}

// void AlarmStoreServant::ActionThread::activateAction()
// {
// 	m_wakeCondition.signal();
// }
///////////////////////////////////////////////////////////////////////////////////////////////////

//*******************
// Private Methods  *
//*******************


AlarmFilterSet* AlarmStoreServant::createFilter(const TA_Base_Core::IAlarmStoreCorbaDef::FilterInfo& filter)
{
	FUNCTION_ENTRY("createFilter");
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AlarmStoreServant::createFilte");
	AlarmFilterSet* filterSet = new AlarmFilterSet();  // Will be cleaned up by the alarm store manager
	TA_ASSERT(filterSet!=NULL,"Unable to create new filter set");

	for (unsigned int iLoop=0;iLoop<filter.length();iLoop++)
	{
		TA_Base_Core::IAlarmStoreCorbaDef::FilterEntry filterEntry = filter[iLoop]; // Corba filter details
		IAlarmFilter*              filterItem=NULL;                            // C++ filter item
		IAlarmFilter::EComparitor  comparitor;

		try
		{
			switch (filterEntry.compareOp)
			{
				case TA_Base_Core::IAlarmStoreCorbaDef::FilterComparison::COMPARE_LESS_THAN:
					comparitor = IAlarmFilter::COMPARE_LESS_THAN;
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FilterComparison::COMPARE_LESS_THAN_EQUAL:
					comparitor = IAlarmFilter::COMPARE_LESS_THAN_EQUAL;
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FilterComparison::COMPARE_EQUAL:
					comparitor = IAlarmFilter::COMPARE_EQUAL;
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FilterComparison::COMPARE_GREATER_THAN:
					comparitor = IAlarmFilter::COMPARE_GREATER_THAN;
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FilterComparison::COMPARE_GREATER_THAN_EQUAL:
					comparitor = IAlarmFilter::COMPARE_GREATER_THAN_EQUAL;
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FilterComparison::COMPARE_NOT_EQUAL:
					comparitor = IAlarmFilter::COMPARE_NOT_EQUAL;
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FilterComparison::COMPARE_CONTAINS:
					comparitor = IAlarmFilter::COMPARE_CONTAINS;
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FilterComparison::COMPARE_RANGE_INCLUSIVE:
					comparitor = IAlarmFilter::COMPARE_BETWEEN_INCLUSIVE;
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FilterComparison::COMPARE_RANGE_EXCLUSIVE:
					comparitor = IAlarmFilter::COMPARE_BETWEEN_EXCLUSIVE;
					break;
				default:
					TA_ASSERT(false,"Unknown IDL filter comparison.");
			}

			switch (filterEntry.parameters._d())
			{
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_PROFILE:	
					filterItem = new AlarmFilterByProfile(std::string(filterEntry.parameters.paramText()));
					(static_cast<AlarmFilterByProfile*>(filterItem))->setRightsLibrary(m_rightsLibrary);
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_ALARM_TYPE:
					filterItem = new AlarmFilterByType(comparitor,filterEntry.parameters.paramULong());
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_ALARM_TIME:	
					filterItem = new AlarmFilterByDate(comparitor,filterEntry.parameters.paramULong());
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_ALARM_TIME_RANGE:	
					filterItem = new AlarmFilterByDate(comparitor,filterEntry.parameters.paramDateRange().paramDate1,
						                               filterEntry.parameters.paramDateRange().paramDate2);
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_ASSOCIATED_ENTITY_KEY:	
					filterItem = new AlarmFilterByAssociatedEntityKey(comparitor,filterEntry.parameters.paramULong());
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_ASSOCIATED_ENTITY_NAME:	
					filterItem = new AlarmFilterByAssociatedEntityName(comparitor,std::string(filterEntry.parameters.paramText()));
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_ASSET_NAME:	
					filterItem = new AlarmFilterByAssetName(comparitor,std::string(filterEntry.parameters.paramText()));
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_DESCRIPTION:	
					filterItem = new AlarmFilterByDescription(comparitor,std::string(filterEntry.parameters.paramText()));
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_LOCATION:	
					filterItem = new AlarmFilterByLocation(comparitor,filterEntry.parameters.paramULong());
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_OPERATOR:	
					filterItem = new AlarmFilterByOperator(comparitor,filterEntry.parameters.paramText());
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_SEVERITY:	
					filterItem = new AlarmFilterBySeverity(comparitor,filterEntry.parameters.paramULong());
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_STATE:
					AlarmFilterByState::EAlarmState state;
					switch (filterEntry.parameters.paramState())
					{
						case TA_Base_Core::IAlarmStoreCorbaDef::STATE_ALL_OPEN: 
							state = AlarmFilterByState::ALL_OPEN;
							break;
						case TA_Base_Core::IAlarmStoreCorbaDef::STATE_OPEN_UNACKED: 
							state = AlarmFilterByState::OPEN_UNACKED;
							break;
						case TA_Base_Core::IAlarmStoreCorbaDef::STATE_OPEN_ACKED: 
							state = AlarmFilterByState::OPEN_ACKED;
							break;
						case TA_Base_Core::IAlarmStoreCorbaDef::STATE_CLOSED: 
							state = AlarmFilterByState::CLOSED;
							break;
						case TA_Base_Core::IAlarmStoreCorbaDef::STATE_ALL_UNACKED: 
							state = AlarmFilterByState::ALL_UNACKED;
							break;
						case TA_Base_Core::IAlarmStoreCorbaDef::STATE_ANY: 
							state = AlarmFilterByState::ANY;
							break;
						default:
							TA_ASSERT(false,"Invalid alarm state for filter.");
					};
					filterItem = new AlarmFilterByState(comparitor,state);
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_OMTYPE:
					AlarmFilterByOMType::EAlarmOMType OMType;
					switch (filterEntry.parameters.paramOMType())
					{	
						case TA_Base_Core::IAlarmStoreCorbaDef::OMTYPE_OPERATION: 
							OMType = AlarmFilterByOMType::OMTYPE_OPERATION;
							break;
						case TA_Base_Core::IAlarmStoreCorbaDef::OMTYPE_MAINTENANCE: 
							OMType = AlarmFilterByOMType::OMTYPE_MAINTENANCE;
							break;
						case TA_Base_Core::IAlarmStoreCorbaDef::OMTYPE_BOTH: 
							OMType = AlarmFilterByOMType::OMTYPE_BOTH;
							break;
						case TA_Base_Core::IAlarmStoreCorbaDef::OMTYPE_ALL: 
							OMType = AlarmFilterByOMType::OMTYPE_ALL;
							break;
						default:
							TA_ASSERT(false,"Invalid alarm state for filter.");
					};
					filterItem = new AlarmFilterByOMType(comparitor,OMType);
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_SUBSYSTEM:	
					filterItem = new AlarmFilterBySubSystem(comparitor,filterEntry.parameters.paramULong());
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_SUBSYSTEMTYPE:	
					filterItem = new AlarmFilterBySubsystemType(comparitor,filterEntry.parameters.paramText());
					break;
				case TA_Base_Core::IAlarmStoreCorbaDef::FILTER_SYSTEMTYPE:	
					filterItem = new AlarmFilterBySystemType(comparitor,filterEntry.parameters.paramText());
					break;		
				default:
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "default does not handle the cases");
			}

			TA_ASSERT(filterItem!=NULL,"Filter item not created.");
			filterSet->addFilter(filterItem);
		}
		catch(...)
		{
			throw TA_Base_Core::IAlarmStoreCorbaDef::InvalidFilterException();
		}
	}

	FUNCTION_EXIT;
	return filterSet;
}



CORBA::Long AlarmStoreServant::getAliveTimeStamp()
{
	FUNCTION_ENTRY("getAliveTimeStamp");
	FUNCTION_EXIT;
	return (CORBA::Long)m_aliaveTimeStamp;
}


bool AlarmStoreServant::check()
{
	FUNCTION_ENTRY("check");
	bool isAlive = true;
#if USE_WATCHDOG
	isAlive = AlarmAccessAdaptor::getInstance().aliveCheck();
#endif
	if ( isAlive )
	{
		ACE_OS::time(&m_aliaveTimeStamp);
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "AlarmStoreServant check, timestamp = %li", m_aliaveTimeStamp);

	FUNCTION_EXIT;
	return isAlive;
}

TA_Base_Core::AlarmDetailCorbaDef* AlarmStoreServant::getAlarm(const char* alarmID)
{
	FUNCTION_ENTRY("getAlarm");

	TA_Base_Core::AlarmDetailCorbaDef* alarm = AlarmAccessAdaptor::getInstance().getAlarm(alarmID);
	if (alarm==NULL)
	{
		// Not found!
		throw TA_Base_Core::IAlarmStoreCorbaDef::AlarmNotFoundException();
	}
	
	FUNCTION_EXIT;
	return alarm;
}
