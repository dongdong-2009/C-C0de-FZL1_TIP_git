/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmAgentInterfaceManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmAgentInterfaceManager.cpp
//  Implementation of the Class AlarmAgentInterfaceManager
//  Created on:      24-Jan-2004 04:16:58 PM
///////////////////////////////////////////////////////////

#include "AlarmAgentInterfaceManager.h"
#include "AlarmAgentInterface.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
//#include "SharedAlarmAccessFactory.h"
#include "AlarmAccessAdaptor.h"
#include <sstream>

using namespace TA_Base_Bus;


/**
 * Constructor
 * @param callback
 * 
 */
AlarmAgentInterfaceManager::AlarmAgentInterfaceManager(IAlarmUpdate& callback)
						   : m_callback(callback), m_myLocation(0)
{
	FUNCTION_ENTRY("Constructor");

    std::stringstream locationString;
    locationString << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
    TA_ASSERT(!locationString.str().empty(), "Location key RunParam not set");
    locationString >> m_myLocation;

	FUNCTION_EXIT;
}

AlarmAgentInterfaceManager::~AlarmAgentInterfaceManager()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
	stopAllAgentMonitoring();
}


/**
 * start the monitoring a specified agent.  It will have no effect if the agent is
 * already being monitored.
 * @param agentEntityName    Entity name of alarm agent
 * 
 */
void AlarmAgentInterfaceManager::startAgentMonitoring(const std::string &agentEntityName,long location, bool alarmStoreStart)
{
	FUNCTION_ENTRY("startAgentMonitoring");

	TA_Base_Core::ThreadGuard guard(m_interfaceListLock);

	AlarmAgentInterface*  alarmInterface;

	// Check if agent already monitored
	for (unsigned int iLoop=0;iLoop<m_alarmAgentInterfaceList.size();iLoop++)
	{
		alarmInterface = m_alarmAgentInterfaceList.at(iLoop);
		if (agentEntityName.compare(alarmInterface->getAgentEntityName()) == 0)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			     "Alarms from location %ld already monitored on agent %s",location,agentEntityName.c_str());
			return;
		}
	}

	// Not there so add a new one
	LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
		 "Monitoring of alarms from location %ld starting on agent %s",location,agentEntityName.c_str());

	alarmInterface = new AlarmAgentInterface(m_callback, agentEntityName, m_myLocation, location,alarmStoreStart);
	
	TA_ASSERT(alarmInterface!=NULL,"Unable to new an alarm interface");
	
	m_alarmAgentInterfaceList.push_back(alarmInterface);
	alarmInterface->start();

	FUNCTION_EXIT;
}


/**
 * Stop the monitoring of all alarm agents that are currently being monitored.
 */
void AlarmAgentInterfaceManager::stopAllAgentMonitoring()
{
	FUNCTION_ENTRY("stopAllAgentMonitoring");

	TA_Base_Core::ThreadGuard guard(m_interfaceListLock);
     unsigned int iLoop=0;
	 //improve the stop speed of alarm store
	for (iLoop=0;iLoop<m_alarmAgentInterfaceList.size();iLoop++)
	{
		AlarmAgentInterface*  alarmInterface = m_alarmAgentInterfaceList.at(iLoop);
		alarmInterface->terminate();
	}

	// Release each interface in the collection
	for (iLoop=0;iLoop<m_alarmAgentInterfaceList.size();iLoop++)
	{
		AlarmAgentInterface*  alarmInterface = m_alarmAgentInterfaceList.at(iLoop);

		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
		 "Monitoring of alarms stopping on agent %s",alarmInterface->getAgentEntityName().c_str());
        unsigned long agentLocation=0;
		try
		{
			
			agentLocation=alarmInterface->getAgentLocationKey();
//			unsigned long rc = SharedAlarmAccessFactory::getInstance().removeLocationAlarms(agentLocation);
			unsigned long rc = AlarmAccessAdaptor::getInstance().removeLocationAlarms(agentLocation);
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stopAgent rc= %d Monitor locationKey=%d",rc, agentLocation);
		}  		 
		catch (...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "stopAllAgent Monitor locationKey=%d",  agentLocation);
		}

		delete alarmInterface;
	}
	// Clear the collection
	m_alarmAgentInterfaceList.clear();

	FUNCTION_EXIT;
}


/**
 * This will stop all agent monitoring except for the location specified.  This
 * will be used when switching from all agents to a single agent mode.
 */
void AlarmAgentInterfaceManager::stopAllAgentMonitoringExcept(const std::vector<std::string> &agentEntityName)
{
	FUNCTION_ENTRY("stopAllAgentMonitoringExcept");

	TA_Base_Core::ThreadGuard guard(m_interfaceListLock);

	// Release each interface in the collection
	std::vector<AlarmAgentInterface*>::iterator  it = m_alarmAgentInterfaceList.begin();
	while (it!=m_alarmAgentInterfaceList.end())
	{
		AlarmAgentInterface*  alarmInterface = (*it);
		std::string thisAgent = alarmInterface->getAgentEntityName();

		// Check if this agent is in the list of those to leave running
		bool foundExclude = false;
		std::vector<std::string>::const_iterator excludeIT = agentEntityName.begin();
		while (excludeIT!=agentEntityName.end())
		{
			if (*excludeIT==thisAgent)
			{
				foundExclude = true;
				break;
			}

            excludeIT++;
		}

		if (!foundExclude)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			 "Monitoring of alarms stopping on agent %s",alarmInterface->getAgentEntityName().c_str());
			unsigned long agentLocation=0;
			agentLocation=alarmInterface->getAgentLocationKey();
			// Not in the exclude list so delete it
			delete alarmInterface;
			alarmInterface = NULL;
			it = m_alarmAgentInterfaceList.erase(it);

			try
			{
//				unsigned long rc = SharedAlarmAccessFactory::getInstance().removeLocationAlarms(agentLocation);
				unsigned long rc = AlarmAccessAdaptor::getInstance().removeLocationAlarms(agentLocation);
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stopAgent rc= %d Monitor locationKey=%d",rc, agentLocation);
			}  		 
			catch (...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "stopAgent Monitor locationKey=%d",  agentLocation);
			}
		}
		else
		{
			it++;
		}
	}

	FUNCTION_EXIT;
}



/** 
  * AlarmAgentInterfaceManager::reloadAllAlarms
  *
  * This method will reload alarms from each of the monitored agents.
  *
  */
void AlarmAgentInterfaceManager::reloadAllAlarms()
{
	FUNCTION_ENTRY("reloadAllAlarms");

	// Reload each interface
	for (unsigned int iLoop=0;iLoop<m_alarmAgentInterfaceList.size();iLoop++)
	{
		AlarmAgentInterface*  alarmInterface = m_alarmAgentInterfaceList.at(iLoop);
		alarmInterface->reloadAlarms();
	}

	FUNCTION_EXIT;
}


