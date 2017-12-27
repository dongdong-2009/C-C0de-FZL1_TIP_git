/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/event/event_viewer/src/DatabaseAccess.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2009/12/03 15:46:08 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * DatabaseAccess is a class that places all database code for loading filter dialog boxes in the one location. It translates
 * the database retrieved values into "understandable" C++ values.
 */

#include "app/soe/soe_event_viewer/src/stdafx.h"
#include "app/soe/soe_event_viewer/src/EventViewer.h"
#include "app/soe/soe_event_viewer/src/DatabaseAccess.h"

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

// Event Colours
#include "core/data_access_interface/src/IEventColourData.h"
#include "core/data_access_interface/src/EventColourAccessFactory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const std::string DatabaseAccess::AUDIT_CHANNEL_NAME = "Audit";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DatabaseAccess::DatabaseAccess()
{

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

    std::vector<TA_Base_Core::IMessageTypeData*>::iterator etIt;
    for ( etIt=m_eventTypes.begin(); etIt!=m_eventTypes.end(); etIt++)
    {
        delete *etIt;
        *etIt = NULL;
    }
    m_eventTypes.clear();
}


std::vector<std::pair<unsigned long,std::string> > DatabaseAccess::getOperators()
{
    std::vector<std::pair<unsigned long,std::string> > operatorDetails;
    try
    {
        std::vector<TA_Base_Core::IOperator*> operators = TA_Base_Core::OperatorAccessFactory::getInstance().getAllOperatorsAndAttributes();

        for (unsigned int i = 0;i < operators.size();i++)
        {
            // For each operator, create a pair of data and put it on the vector
            std::pair<unsigned long, std::string> op;
            try
            {
                op.first = operators[i]->getKey();
                op.second = operators[i]->getName();
            }
            catch(...)
            {
                // If there are any problems with this particular operator, then try the next one
                continue;
            }
            operatorDetails.push_back(op);

            // Then delete the operator object
            delete operators[i];
            operators[i] = NULL;
        }
    }
    catch(...)
    {
        // If a database error occurs, return the vector at whatever point we got up to (so if it's empty, the 
        // operators list will be empty too).
    }

    return operatorDetails;
}


std::vector<std::pair<unsigned long,std::string> > DatabaseAccess::getSeverities()
{
    std::vector<std::pair<unsigned long,std::string> > alarmSeverityDetails;
    try
    {
        std::vector<TA_Base_Core::IAlarmSeverityData*> severities = TA_Base_Core::AlarmSeverityAccessFactory::getInstance().getAllAlarmSeverities();

        for (unsigned int i = 0;i < severities.size();i++)
        {
            // For each severity, create a pair of data and put it on the vector
            std::pair<unsigned long, std::string> severity;
            severity.first = severities[i]->getKey();
            severity.second = severities[i]->getName();
            alarmSeverityDetails.push_back(severity);

            // Then delete the severity object
            delete severities[i];
            severities[i] = NULL;
        }
    }
    catch(...)
    {
        // If a database error occurs, return the vector at whatever point we got up to (so if it's empty, the 
        // severity list will be empty too).
    }

    return alarmSeverityDetails;
}


std::vector<std::pair<unsigned long,std::string> > DatabaseAccess::getEventTypes(unsigned long eventLevel)
{
    loadEventTypes();

    std::vector<std::pair<unsigned long,std::string> > eventTypeDetails;
    try
    {
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
    }
    catch(...)
    {
        // If a database error occurs, return the vector at whatever point we got up to (so if it's empty, the 
        // severity list will be empty too).
    }

    return eventTypeDetails;
}


std::vector<std::pair<unsigned long,std::string> > DatabaseAccess::getAlarmTypes()
{
    std::vector<std::pair<unsigned long,std::string> > alarmTypeDetails;
    try
    {
        std::vector<TA_Base_Core::IAlarmTypeData*> alarmTypes = TA_Base_Core::AlarmTypeAccessFactory::getInstance().
            getAllAlarmTypesAndAttributes();

        for (unsigned int i = 0;i < alarmTypes.size();i++)
        {
            // For each alarm type, create a pair of data and put it on the vector
            std::pair<unsigned long, std::string> alarmType;

            // The Alarm Type Access factories pass their NUMERIC keys in and out as STRINGS - so need to convert the Alarm type
            // STRING key to a number before storing it (in the AlarmAccessFactory, it will accept a number for this).
            //alarmType.first = strtoul(alarmTypes[i]->getKey().c_str(),NULL,10);
            alarmType.first = alarmTypes[i]->getKey();
            alarmType.second = alarmTypes[i]->getDisplayName();
            alarmTypeDetails.push_back(alarmType);

            // Then delete the alarm type object
            delete alarmTypes[i];
            alarmTypes[i] = NULL;
        }
    }
    catch(...)
    {
        // If a database error occurs, return the vector at whatever point we got up to (so if it's empty, the 
        // severity list will be empty too).
    }

    return alarmTypeDetails;
}


std::vector<std::pair<unsigned long,std::string> > DatabaseAccess::getLocations()
{
    loadLocations();

    std::vector<std::pair<unsigned long,std::string> > locationDetails;
    try
    {
        std::map<unsigned long,TA_Base_Core::ILocation*>::iterator locIt;
        for ( locIt=m_locations.begin(); locIt!=m_locations.end(); locIt++ )
        {
            // For each location, create a pair of data and put it on the vector
            std::pair<unsigned long, std::string> location;
            
            location.first = locIt->second->getKey();
            location.second = locIt->second->getDisplayName();
            locationDetails.push_back(location);
        }
    }
    catch(...)
    {
        // If a database error occurs, return the vector at whatever point we got up to (so if it's empty, the 
        // severity list will be empty too).
    }

    return locationDetails;
}


std::vector<std::pair<unsigned long,std::string> > DatabaseAccess::getSubsystems()
{
    loadSubsystems();

    std::vector<std::pair<unsigned long,std::string> > subsystemDetails;
    try
    {
        std::map<unsigned long,TA_Base_Core::ISubsystem*>::iterator ssIt;
        for ( ssIt=m_subsystems.begin(); ssIt!=m_subsystems.end(); ssIt++ )
        {
            // For each subsystem, create a pair of data and put it on the vector
            std::pair<unsigned long, std::string> subsystem;
                       
            subsystem.first = ssIt->second->getKey();
            subsystem.second = ssIt->second->getDisplayName();
            subsystemDetails.push_back(subsystem);
        }
    }
    catch(...)
    {
        // If a database error occurs, return the vector at whatever point we got up to (so if it's empty, the 
        // severity list will be empty too).
    }

    return subsystemDetails;
}

std::map<std::string, TA_Base_Core::IEventColourData*> DatabaseAccess::getEventColours()
{
    try
    {
		loadEventColours();
    }
    catch(...)
    {
        // If a database error occurs, return the vector at whatever point we got up to (so if it's empty, the 
        // severity list will be empty too).
    }
	return m_eventColours;
}

void DatabaseAccess::separateSubsystems( const std::vector<unsigned long>& subsystems,
                                         std::vector<unsigned long>& applicationSubsystems,
                                         std::vector<unsigned long>& physicalSubsystems )
{
    loadSubsystems();

    // Make sure the 'out' vectors are empty.
    applicationSubsystems.clear();
    physicalSubsystems.clear();

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
        }
    }
}


const std::string DatabaseAccess::getSubsystemName(const unsigned long subsystemId)
{
    loadSubsystems();

    std::map<unsigned long,TA_Base_Core::ISubsystem*>::iterator it = m_subsystems.find( subsystemId );
    if ( it != m_subsystems.end() )
    {
        return it->second->getName();
    }

    return "";
}


const std::string DatabaseAccess::getLocationName(const unsigned long locationId)
{
    loadLocations();

    std::map<unsigned long,TA_Base_Core::ILocation*>::iterator it = m_locations.find( locationId );
    if ( it != m_locations.end() )
    {
        return it->second->getName();
    }

    return "";
}

void DatabaseAccess::loadSubsystems()
{
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
            // Just do nothing...
        }
    }
}

void DatabaseAccess::loadLocations()
{
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
            // Do nothing...
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
            // If a database error occurs, return the vector at whatever point we got up to (so if it's empty, the 
            // severity list will be empty too).
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
            // Do nothing...
        }
     }
}


