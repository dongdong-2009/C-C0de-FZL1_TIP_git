/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_DBSync/TA_BASE/transactive/app/event/event_viewer/src/DatabaseAccess.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2014/07/18 15:34:16 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * DatabaseAccess is a class that places all database code for loading filter dialog boxes in the one location. It translates
 * the database retrieved values into "understandable" C++ values.
 */

#include "app/event/event_viewer/src/stdafx.h"
#include "app/event/event_viewer/src/EventViewer.h"
#include "app/event/event_viewer/src/DatabaseAccess.h"

// Operators
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"

// Severity
#include "core/data_access_interface/src/IAlarmSeverityData.h"
#include "core/data_access_interface/src/AlarmSeverityAccessFactory.h"

// Message (Event) Type
#include "core/data_access_interface/src/IMessageTypeData.h"
#include "core/data_access_interface/src/MessageTypeAccessFactory.h"

// Alarm Type
#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"

// Location
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

// Subsystem
#include "core/data_access_interface/src/ISubsystem.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"

//system
#include "core/data_access_interface/src/SystemData.h"
#include "core/data_access_interface/src/SystemAccessFactory.h"

// Event Colours
#include "core/data_access_interface/src/IEventColourData.h"
#include "core/data_access_interface/src/EventColourAccessFactory.h"

// Alarm States
#include "core/data_access_interface/src/AlarmStateData.h"
#include "core/data_access_interface/src/AlarmStateAccessFactory.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const std::string DatabaseAccess::AUDIT_CHANNEL_NAME = "Audit";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DatabaseAccess* DatabaseAccess::m_instance = 0;
TA_Base_Core::ReEntrantThreadLockable DatabaseAccess::m_singletonLock;

DatabaseAccess::DatabaseAccess()
{
	m_systemsLoaded = false;
}


DatabaseAccess::~DatabaseAccess()
{
    // Empty any of our member maps.
    std::map<unsigned long,TA_Base_Core::ISubsystem*>::iterator ssIt;
    for ( ssIt=m_subsystems.begin(); ssIt!=m_subsystems.end(); ssIt++ )
    {
        delete ssIt->second;
        ssIt->second = NULL;
    }
    m_subsystems.clear();

    std::map<unsigned long,TA_Base_Core::ILocation*>::iterator locIt;
    for ( locIt=m_locations.begin(); locIt!=m_locations.end(); locIt++ )
    {
        delete locIt->second;
        locIt->second = NULL;
    }
    m_locations.clear();

	std::map<unsigned long,TA_Base_Core::AlarmStateData*>::iterator alarmStateIt;
	for ( alarmStateIt=m_alarmStates.begin(); alarmStateIt!=m_alarmStates.end(); alarmStateIt++ )
	{
		delete alarmStateIt->second;
		alarmStateIt->second = NULL;
	}
	m_alarmStates.clear();


	m_alarmStatesNameKey.clear();

    std::vector<TA_Base_Core::IMessageTypeData*>::iterator etIt;
    for ( etIt=m_eventTypes.begin(); etIt!=m_eventTypes.end(); etIt++)
    {
        delete *etIt;
        *etIt = NULL;
    }
    m_eventTypes.clear();

	// to do: need to change the ISubsystem to a new type
	std::map<unsigned long,TA_Base_Core::SystemData*>::iterator sIt;
	for ( sIt=m_systems.begin(); sIt!=m_systems.end(); sIt++ )
	{
		delete sIt->second;
		sIt->second = NULL;
	}
	m_systems.clear();


}


DatabaseAccess& DatabaseAccess::getInstance()
{
	TA_Base_Core::ThreadGuard guard(m_singletonLock);
	
	if( m_instance == 0 )
	{
		m_instance = new DatabaseAccess();
	}
    return  *m_instance;

}

// load all the list from DB to cache for EventViewer filter dialog.
void DatabaseAccess::load4FilterDlg()
{
	

// 	loadEventTypes();
 	loadAlarmTypes();

	loadSystems();//Load ISCS system data into DatabaseAccess::m_systems
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "load System finished.");

	loadSubsystems(); //Load ISCS subsystem data into DatabaseAccess::m_subsystems
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "load Subystem finished.");

	loadLocations();//Load ISCS location data into DatabaseAccess::m_locations
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "load location finished.");

	loadOperators();//Load ISCS operator data into DatabaseAccess::
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "load operator finished.");

	loadAlarmStates();//Load ISCS alarm state
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "load alarm state finished.");

	loadAlarmSeveritys();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "load alarm severity finished.");

	loadEventColours();
}

std::vector<std::pair<unsigned long,std::string> > DatabaseAccess::getOperators()
{
    TA_Base_Core::ThreadGuard guard(m_cachelock);
	loadOperators();
	std::vector<std::pair<unsigned long,std::string> > operatorDetails;

        
    for (unsigned int i = 0;i < m_operators.size();i++)
    {
        // For each operator, create a pair of data and put it on the vector
		std::pair<unsigned long, std::string> op;
		op.first = m_operators[i]->getKey();
		op.second = m_operators[i]->getName();
		operatorDetails.push_back(op); 
    }
    
    return operatorDetails;
}

std::vector<std::pair<unsigned long,std::string> > DatabaseAccess::getSystems()
{
	TA_Base_Core::ThreadGuard guard(m_cachelock);
	loadSystems();

	std::vector<std::pair<unsigned long,std::string> > systemDetails;

	std::map<unsigned long,TA_Base_Core::SystemData*>::iterator itr;
	for ( itr=m_systems.begin(); itr!=m_systems.end(); itr++)
	{
		// For each operator, create a pair of data and put it on the vector
		std::pair<unsigned long, std::string> sys;
		sys.first = itr->second->getPkey();
		sys.second = itr->second->getName();
		systemDetails.push_back(sys); 
		
	}

	return systemDetails;
}


std::vector<std::pair<unsigned long,std::string> > DatabaseAccess::getSeverities()
{
    TA_Base_Core::ThreadGuard guard(m_cachelock);
	loadAlarmSeveritys();
	std::vector<std::pair<unsigned long,std::string> > alarmSeverityDetails;

    //std::vector<TA_Base_Core::IAlarmSeverityData*> severities = TA_Base_Core::AlarmSeverityAccessFactory::getInstance().getAllAlarmSeverities();

    for (unsigned int i = 0;i < m_severitys.size();i++)
    {
        // For each severity, create a pair of data and put it on the vector
        std::pair<unsigned long, std::string> severity;
        severity.first = m_severitys[i]->getKey();
        severity.second = m_severitys[i]->getName();
        alarmSeverityDetails.push_back(severity);           
    }

    return alarmSeverityDetails;
}

std::vector<std::pair<unsigned long,std::string> > DatabaseAccess::getAlarmStates()
{
	TA_Base_Core::ThreadGuard guard(m_cachelock);
	loadAlarmStates();

	std::vector<std::pair<unsigned long,std::string> > alarmStateDetails;

	std::map<unsigned long, TA_Base_Core::AlarmStateData*>::iterator itr;
	for (itr=m_alarmStates.begin(); itr!=m_alarmStates.end(); itr++)
	{
		// For each event type, create a pair of data and put it on the vector
		std::pair<unsigned long, std::string> alarmstate;
		alarmstate.first = itr->second->getTypeKey();  //get typekey(i.e. eventTypeKey)
		alarmstate.second = itr->second->getName();
		alarmStateDetails.push_back(alarmstate);
		
	}

	return alarmStateDetails;
}

bool DatabaseAccess::findAlarmStateName(std::string name)
{
	if(m_alarmStatesNameKey.find(name)!=m_alarmStatesNameKey.end())
	{
		return true;
	}
	else 
	{
		return false;
	}
}
// std::map<std::string , unsigned long > DatabaseAccess::getAlarmStatesMap_StringKey()
// {
// 	TA_Base_Core::ThreadGuard guard(m_cachelock);
// 	loadAlarmStates();
// 
// 	std::map<std::string , unsigned long > alarmStateMap_strKey;
// 
// 	std::map<unsigned long, TA_Base_Core::AlarmStateData*>::iterator itr;
// 	for (itr=m_alarmStates.begin(); itr!=m_alarmStates.end(); itr++)
// 	{
// 		// For each event type, create a pair of data and put it on the vector
// 		std::pair<std::string , unsigned long > alarmstate;
// 		alarmstate.first = itr->second->getName(); 
// 		alarmstate.second = itr->second->getKey();
// 		alarmStateMap_strKey.insert(alarmstate);
// 
// 	}
// 
// 	return alarmStateMap_strKey;
// }

std::vector<std::pair<unsigned long,std::string> > DatabaseAccess::getEventTypes(unsigned long eventLevel)
{
    TA_Base_Core::ThreadGuard guard(m_cachelock);
	loadEventTypes();

    std::vector<std::pair<unsigned long,std::string> > eventTypeDetails;

    for (unsigned int i = 0;i < m_eventTypes.size();i++)
    {
        // Only load the event type if the view level is less than or equal to the current event level
        if(m_eventTypes[i]->getViewLevel() <= eventLevel)
        {
            // For each event type, create a pair of data and put it on the vector
            std::pair<unsigned long, std::string> eventType;

            // The Message Type Access factories pass their NUMERIC keys in and out as STRINGS - so need to convert the message type
            // STRING key to a number before storing it (in the MessageAccessFactory, it will accept a number for this).
            eventType.first = m_eventTypes[i]->getKey();
                eventType.second = m_eventTypes[i]->getDisplayName();
            eventTypeDetails.push_back(eventType);
        }
    }

    return eventTypeDetails;
}


std::vector<std::pair<unsigned long,std::string> > DatabaseAccess::getAlarmTypes()
{
    TA_Base_Core::ThreadGuard guard(m_cachelock);
	loadAlarmTypes();
	std::vector<std::pair<unsigned long,std::string> > alarmTypeDetails;

    for (unsigned int i = 0;i < m_alarmTypes.size();i++)
    {
        // For each alarm type, create a pair of data and put it on the vector
        std::pair<unsigned long, std::string> alarmType;

        // The Alarm Type Access factories pass their NUMERIC keys in and out as STRINGS - so need to convert the Alarm type
        // STRING key to a number before storing it (in the AlarmAccessFactory, it will accept a number for this).
        //alarmType.first = strtoul(alarmTypes[i]->getKey().c_str(),NULL,10);
        alarmType.first = m_alarmTypes[i]->getKey();
            alarmType.second = m_alarmTypes[i]->getDisplayName();
        alarmTypeDetails.push_back(alarmType);         
    }

    return alarmTypeDetails;
}


std::vector<std::pair<unsigned long,std::string> > DatabaseAccess::getLocationsKeyNamePairs()
{
    TA_Base_Core::ThreadGuard guard(m_cachelock);
	loadLocations();

    std::vector<std::pair<unsigned long,std::string> > locationDetails;

    std::map<unsigned long,TA_Base_Core::ILocation*>::iterator locIt;
    for ( locIt=m_locations.begin(); locIt!=m_locations.end(); locIt++ )
    {
        // For each location, create a pair of data and put it on the vector
        std::pair<unsigned long, std::string> location;
        
        location.first = locIt->second->getKey();
            location.second = locIt->second->getDisplayName();
        locationDetails.push_back(location);
    }
	return locationDetails;
}

std::vector<std::pair<unsigned long,std::string> > DatabaseAccess::getLocationKeyDispNamePairs()
{
	TA_Base_Core::ThreadGuard guard(m_cachelock);
	loadLocations();

	std::vector<std::pair<unsigned long,std::string> > locationDetails;

	std::map<unsigned long,TA_Base_Core::ILocation*>::iterator locIt;
	for ( locIt=m_locations.begin(); locIt!=m_locations.end(); locIt++ )
	{
		// For each location, create a pair of data and put it on the vector
		std::pair<unsigned long, std::string> location;

		location.first = locIt->second->getKey();
		location.second = locIt->second->getDisplayName();
		locationDetails.push_back(location);
	}

	return locationDetails;
}


//<pkey, <name, system_Key>>
std::vector<ISCSSubsystemItem> DatabaseAccess::getSubsystems()  
{
    TA_Base_Core::ThreadGuard guard(m_cachelock);
	loadSubsystems();

    std::vector<ISCSSubsystemItem> subsystemDetails;

    std::map<unsigned long,TA_Base_Core::ISubsystem*>::iterator ssIt;
    for ( ssIt=m_subsystems.begin(); ssIt!=m_subsystems.end(); ssIt++ )
    {
        // For each subsystem, create a pair of data and put it on the vector
        ISCSSubsystemItem subsystem(ssIt->second->getKey(),  ssIt->second->getDisplayName(),ssIt->second->getSystemKey());
        subsystemDetails.push_back(subsystem);
    }


    return subsystemDetails;
}

// std::vector<ISCSSubsystemItem> DatabaseAccess::getSubsystemsBySystemkeys(std::vector<unsigned long>systemKeys)
// {
// 	TA_Base_Core::ThreadGuard guard(m_cachelock);
// 	loadSubsystems();
// 
// 	std::vector<ISCSSubsystemItem> subsystemDetails;
// 	try
// 	{
// 		std::map<unsigned long,TA_Base_Core::ISubsystem*>::iterator ssIt;
// 		for ( ssIt=m_subsystems.begin(); ssIt!=m_subsystems.end(); ssIt++ )
// 		{
// 			// For each subsystem, create a pair of data and put it on the vector
// 			ISCSSubsystemItem subsystem;
// 
// 			subsystem.pkey = ssIt->second->getKey();
// 			subsystem.subsystemName = ssIt->second->getName();
// 			subsystem.system_key = ssIt->second->getSystemKey();
// 			subsystemDetails.push_back(subsystem);
// 		}
// 	}
// 	catch(...)
// 	{
// 		// If a database error occurs, return the vector at whatever point we got up to (so if it's empty, the 
// 		// severity list will be empty too).
// 	}
// 
// 	return subsystemDetails;
// 
// // 	std::map<unsigned long,TA_Base_Core::ISubsystem*> filtedSubsystems;
// // 		try
// // 		{
// // 			std::vector<TA_Base_Core::ISubsystem*> filtedSubsystemsMap = TA_Base_Core::SubsystemAccessFactory::getInstance()
// // 				.getSubsystemBySystemKeys(systemKeys);
// // 			std::vector<TA_Base_Core::ISubsystem*>::iterator ssIt;
// // 			for ( ssIt=filtedSubsystemsMap.begin(); ssIt!=filtedSubsystemsMap.end(); ssIt++ )
// // 			{
// // 				filtedSubsystems.insert( std::map<unsigned long,TA_Base_Core::ISubsystem*>::value_type( (*ssIt)->getKey(), *ssIt ) );
// // 			}
// // 		}
// // 		catch(...)
// // 		{
// // 			// Just do nothing...
// // 		}
// // 
// // 
// // 	std::vector<std::pair<unsigned long,std::string> > subsystemDetails;
// // 	try
// // 	{
// // 		std::map<unsigned long,TA_Base_Core::ISubsystem*>::iterator ssIt;
// // 		for ( ssIt=filtedSubsystems.begin(); ssIt!=filtedSubsystems.end(); ssIt++ )
// // 		{
// // 			// For each subsystem, create a pair of data and put it on the vector
// // 			std::pair<unsigned long, std::string> subsystem;
// // 
// // 			subsystem.first = ssIt->second->getKey();
// // 			subsystem.second = ssIt->second->getName();
// // 			subsystemDetails.push_back(subsystem);
// // 		}
// // 	}
// // 	catch(...)
// // 	{
// // 		// If a database error occurs, return the vector at whatever point we got up to (so if it's empty, the 
// // 		// severity list will be empty too).
// // 	}
// // 
// // 	return subsystemDetails;
// }
// 

std::map<std::string, TA_Base_Core::IEventColourData*> DatabaseAccess::getEventColours()
{

	loadEventColours();
	return m_eventColours;
}

// void DatabaseAccess::separateSystems( const std::vector<unsigned long>& systems,
// 										std::vector<unsigned long>& ISCSsystems,
// 										std::vector<unsigned long>& atsSystems)
// {
// 	TA_Base_Core::ThreadGuard guard(m_cachelock);
// 
// 	// Make sure the 'out' vectors are empty.
// 	ISCSsystems.clear();
// 	atsSystems.clear();
// 
// 	// Iterate through the list.
// 	std::vector<unsigned long>::const_iterator ssIt;
// 	for ( ssIt=systems.begin(); ssIt!=systems.end(); ssIt++ )
// 	{
// 		if( (*ssIt) < 0)
// 		{
// 			atsSystems.push_back(*ssIt);
// 		}
// 		else
// 		{
// 			ISCSsystems.push_back(*ssIt);
// 		}
// 	}
// }


void DatabaseAccess::separateSubsystems( const std::vector<unsigned long>& subsystems,
                                         std::vector<unsigned long>& applicationSubsystems,
                                         std::vector<unsigned long>& physicalSubsystems,
										 std::vector<unsigned long>& atsSubsystems)
{
    TA_Base_Core::ThreadGuard guard(m_cachelock);
	loadSubsystems();

    // Make sure the 'out' vectors are empty.
    applicationSubsystems.clear();
    physicalSubsystems.clear();
	atsSubsystems.clear();

    // Iterate through the list.
    std::vector<unsigned long>::const_iterator ssIt;
    for ( ssIt=subsystems.begin(); ssIt!=subsystems.end(); ssIt++ )
    {
			std::map<unsigned long,TA_Base_Core::ISubsystem*>::iterator it = m_subsystems.find( *ssIt );
			if ( it != m_subsystems.end() )
			{

			   if ( it->second->isPhysical() )
			   {
				 physicalSubsystems.push_back( it->first );
			   }
			   else
			  {
				 applicationSubsystems.push_back( it->first );
			   }
			}else
			{
				atsSubsystems.push_back( *ssIt );
			}

    }
}


const std::string DatabaseAccess::getSubsystemName(const unsigned long subsystemId)
{
    TA_Base_Core::ThreadGuard guard(m_cachelock);
	loadSubsystems();

    std::map<unsigned long,TA_Base_Core::ISubsystem*>::iterator it = m_subsystems.find( subsystemId );
    if ( it != m_subsystems.end() )
    {
        return it->second->getName();
    }

    return "";
}

const std::string DatabaseAccess::getSystemName(const unsigned long systemId)
{
	TA_Base_Core::ThreadGuard guard(m_cachelock);
	loadSystems();

	std::map<unsigned long,TA_Base_Core::SystemData*>::iterator it = m_systems.find( systemId );
	if ( it != m_systems.end() )
	{
		return it->second->getName();
	}

	return "";
}

const std::string DatabaseAccess::getAlarmStateName(const unsigned long alarmStateID)
{
	TA_Base_Core::ThreadGuard guard(m_cachelock);
	loadAlarmStates();

	std::map<unsigned long,TA_Base_Core::AlarmStateData*>::iterator it = m_alarmStates.find( alarmStateID );
	if ( it != m_alarmStates.end() )
	{
		return it->second->getName();
	}

	return "";
}

const std::string DatabaseAccess::getLocationName(const unsigned long locationId)
{
    TA_Base_Core::ThreadGuard guard(m_cachelock);
	loadLocations();

    std::map<unsigned long,TA_Base_Core::ILocation*>::iterator it = m_locations.find( locationId );
    if ( it != m_locations.end() )
    {
        return it->second->getDisplayName();
    }

    return "";
}



void DatabaseAccess::loadSubsystems()
{
	TA_Base_Core::ThreadGuard guard(m_cachelock);
    // If there are no entries in the subsystems vector...
    if ( m_subsystems.empty() )
    {
        try
        {
            std::vector<TA_Base_Core::ISubsystem*> subsystems = TA_Base_Core::SubsystemAccessFactory::getInstance()
                .getAllSubsystems();
            std::vector<TA_Base_Core::ISubsystem*>::iterator ssIt;
            for ( ssIt=subsystems.begin(); ssIt!=subsystems.end(); ssIt++ )
            {
                m_subsystems.insert( std::map<unsigned long,TA_Base_Core::ISubsystem*>::value_type( (*ssIt)->getKey(), *ssIt ) );
            }
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DB error occurs in loadSubsystems().");
        }
    }
}

void DatabaseAccess::loadSystems()
{
	TA_Base_Core::ThreadGuard guard(m_cachelock);
	// If there are no entries in the subsystems vector...
	if ( m_systemsLoaded == false)
	{
		// to do: load system info from DB
		try
		{
			std::vector<TA_Base_Core::SystemData*> systems = TA_Base_Core::SystemAccessFactory::getInstance()
				.getAllSystems();
			std::vector<TA_Base_Core::SystemData*>::iterator sIt;
			for ( sIt=systems.begin(); sIt!=systems.end(); sIt++ )
			{
				m_systems.insert( std::map<unsigned long,TA_Base_Core::SystemData*>::value_type( (*sIt)->getPkey(), *sIt ) );
			}
			m_systemsLoaded = true;
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DB error occurs in loadSystems().");
		}
	}
}

void DatabaseAccess::loadLocations()
{
	TA_Base_Core::ThreadGuard guard(m_cachelock);
    // If there are no entries in the locations vector...
    if ( m_locations.empty() )
    {
        try
        {
            std::vector<TA_Base_Core::ILocation*> locations = TA_Base_Core::LocationAccessFactory::getInstance()
                .getAllLocations();
            std::vector<TA_Base_Core::ILocation*>::iterator locIt;
            for ( locIt=locations.begin(); locIt!=locations.end(); locIt++ )
            {
                m_locations.insert( std::map<unsigned long,TA_Base_Core::ILocation*>::value_type( (*locIt)->getKey(), *locIt ) );
            }
        }
        catch ( ... )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DB error occurs in loadLocations().");
        }
    }
}

void DatabaseAccess::loadEventTypes()
{
    if(m_eventTypes.empty())
    {
        try
        {
            std::vector<std::string> auditMessageFilter;
            auditMessageFilter.push_back(AUDIT_CHANNEL_NAME);

            m_eventTypes = TA_Base_Core::MessageTypeAccessFactory::getInstance().getAllMessageTypesAndAttributes(auditMessageFilter);
        }
        catch(...)
        {
             LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DB error occurs in loadEventTypes().");
        }
    }
}

void DatabaseAccess::loadAlarmStates()
{
	TA_Base_Core::ThreadGuard guard(m_cachelock);
	if(m_alarmStates.empty())
	{
		try
		{
			std::vector<TA_Base_Core::AlarmStateData*> alarmStates = TA_Base_Core::AlarmStateAccessFactory::getInstance()
				.getAlarmStateData();
			std::vector<TA_Base_Core::AlarmStateData*>::iterator alarmStateIt;
			std::pair<std::string,unsigned long> alarmstate;
			m_alarmStatesNameKey.clear();
			for ( alarmStateIt=alarmStates.begin(); alarmStateIt!=alarmStates.end(); alarmStateIt++ )
			{
				m_alarmStates.insert( std::map<unsigned long, TA_Base_Core::AlarmStateData*>::value_type( (*alarmStateIt)->getKey(), *alarmStateIt ) );
				
				alarmstate.first = (*alarmStateIt)->getName();  //get typekey(i.e. eventTypeKey)
				alarmstate.second = (*alarmStateIt)->getKey();
				m_alarmStatesNameKey.insert(alarmstate);
			}

		}
		catch ( ... )
		{
			 LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DB error occurs in loadAlarmStates().");
		}
	}

}

void DatabaseAccess::loadEventColours()
{
    if(m_eventColours.empty())
    {
        try
        {
            std::vector<TA_Base_Core::IEventColourData*> eventColours = TA_Base_Core::EventColourAccessFactory::getInstance()
				.getAllEventColours();
            std::vector<TA_Base_Core::IEventColourData*>::iterator eventColIt;
            for ( eventColIt=eventColours.begin(); eventColIt!=eventColours.end(); eventColIt++ )
            {
                m_eventColours.insert( std::map<std::string, TA_Base_Core::IEventColourData*>::value_type( (*eventColIt)->getName(), *eventColIt ) );
            }
        }
        catch ( ... )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DB error occurs in loadEventColours().");
        }
     }
}



void DatabaseAccess::loadOperators()
{
	TA_Base_Core::ThreadGuard guard(m_cachelock);
	if(m_operators.empty())
    {
        try
		{
			std::vector<TA_Base_Core::IOperator*> operators = TA_Base_Core::OperatorAccessFactory::getInstanceNoInit().getAllOperatorsInfo();
			
			for (unsigned int i = 0;i < operators.size();i++)
			{				 
				m_operators.push_back(operators[i]);			 
			}
		}
		catch(...)
		{
			// If a database error occurs, return the vector at whatever point we got up to (so if it's empty, the 
			// operators list will be empty too).
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DB error occurs in loadOperators().");
		}	     
     }	
}


void DatabaseAccess::loadAlarmTypes()
{
	TA_Base_Core::ThreadGuard guard(m_cachelock);
	if(m_alarmTypes.empty())
    {
        try
		{
			std::vector<TA_Base_Core::IAlarmTypeData*> alarmTypes = TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAllAlarmTypesAndAttributes();
			
			for (unsigned int i = 0;i < alarmTypes.size();i++)
			{
				// For each alarm type, create a pair of data and put it on the vector
				std::pair<unsigned long, std::string> alarmType;				
			 
				m_alarmTypes.push_back(alarmTypes[i]);			
			 
			}
		}
		catch(...)
		{
			// If a database error occurs, return the vector at whatever point we got up to (so if it's empty, the 
			// severity list will be empty too).
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DB error occurs in loadAlarmTypes().");
		}     
     }
	
}


void DatabaseAccess::loadAlarmSeveritys()
{
	TA_Base_Core::ThreadGuard guard(m_cachelock);
	if( m_severitys.empty())
    {
        try
		{
			std::vector<TA_Base_Core::IAlarmSeverityData*> severities = TA_Base_Core::AlarmSeverityAccessFactory::getInstance().getAllAlarmSeverities();
			
			for (unsigned int i = 0;i < severities.size();i++)
			{
				// For each severity, create a pair of data and put it on the vector			 
				m_severitys.push_back(severities[i]);			 
			}
		}
		catch(...)
		{
			// If a database error occurs, return the vector at whatever point we got up to (so if it's empty, the 
			// severity list will be empty too).
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DB error occurs in loadAlarmSeveritys().");
		}   
     }	
}

