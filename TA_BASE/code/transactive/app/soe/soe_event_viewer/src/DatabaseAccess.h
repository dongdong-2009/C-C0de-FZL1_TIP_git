/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/src/DatabaseAccess.h $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2009/12/03 15:46:08 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * DatabaseAccess is a class that places all database code for loading filter dialog boxes in the one location. It translates
 * the database retrieved values into "understandable" C++ values.
 */

#if !defined(AFX_DATABASEACCESS_H__CA3F2438_7F1E_4890_89F6_4A87A2B9E3A1__INCLUDED_)
#define AFX_DATABASEACCESS_H__CA3F2438_7F1E_4890_89F6_4A87A2B9E3A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <vector>

namespace TA_Base_Core
{
    class ISubsystem;   // forward declaration
    class ILocation;   // forward declaration
    class IMessageTypeData;
	class IEventColourData;
}

class DatabaseAccess  
{
public:

   static const std::string AUDIT_CHANNEL_NAME;

    /**
     * constructor
     */
	DatabaseAccess();

    /**
     * destrcutor
     */
	virtual ~DatabaseAccess();

    /**
     * getOperators
     *
     * @return A vector containing key,name combinations for all operators.
     * NOTE: While a vector<pair> is essentially a map, it is used in the code *as* a vector, and is hence returned 
     * as one.
     */
    std::vector<std::pair<unsigned long,std::string> > getOperators();

    /**
     * getSeverities
     *
     * @return A vector containing key,name combinations for all severities.
     * NOTE: While a vector<pair> is essentially a map, it is used in the code *as* a vector, and is hence returned 
     * as one.
     */
    std::vector<std::pair<unsigned long,std::string> > getSeverities();

    /**
     * getEventTypes
     *
     * @return A vector containing key,name combinations for event types less than or equal to the given level.
     * NOTE: While a vector<pair> is essentially a map, it is used in the code *as* a vector, and is hence returned 
     * as one.
     */
    std::vector<std::pair<unsigned long,std::string> > getEventTypes(unsigned long eventLevel);

    /**
     * getAlarmTypes
     *
     * @return A vector containing key,name combinations for all alarm types.
     * NOTE: While a vector<pair> is essentially a map, it is used in the code *as* a vector, and is hence returned 
     * as one.
     */
    std::vector<std::pair<unsigned long,std::string> > getAlarmTypes();

    /**
     * getLocations
     *
     * @return A vector containing key,name combinations for all locations.
     * NOTE: While a vector<pair> is essentially a map, it is used in the code *as* a vector, and is hence returned 
     * as one.
     */
    std::vector<std::pair<unsigned long,std::string> > getLocations();

    /**
     * getSubsystems
     *
     * @return A vector containing key,name combinations for all subsystems.
     * NOTE: While a vector<pair> is essentially a map, it is used in the code *as* a vector, and is hence returned 
     * as one.
     */
    std::vector<std::pair<unsigned long,std::string> > getSubsystems();

    /**
     * getEventColours
     *
     * @return A map containing name and IEventColourData for all event colours.
     * NOTE: While a vector<pair> is essentially a map, it is used in the code *as* a vector, and is hence returned 
     * as one.
     */
	std::map<std::string, TA_Base_Core::IEventColourData*> getEventColours();

    /**
     * separateSubsystems
     *
     * Separates the provided list of subsystems into application and physical.
     *
     * @param subsystems The list of subsystems to separate.
     * @param applicationSubsystems The list where application subsystems will be placed.
     * @param physicalSubsystems The list where physical subsystems will be placed.
     */
    void separateSubsystems( const std::vector<unsigned long>& subsystems, std::vector<unsigned long>& applicationSubsystems,
                             std::vector<unsigned long>& physicalSubsystems );

    /**
     * getSubsystemName
     *
     * @param subsystemId The subsystem key for which to retrieve the name
     *
     * @return The name of the subsystem with the specified key
     */
    const std::string getSubsystemName(const unsigned long subsystemId);

    /**
     * getLocationName
     *
     * @param subsystemId The location key for which to retrieve the name
     *
     * @return The name of the location with the specified key
     */
    const std::string getLocationName(const unsigned long locationId);

private:
    /**
     * loadSubsystems
     *
     * Populates the subsystem vector if required.
     */
    void loadSubsystems();

    /**
     * loadLocations
     *
     * Populates the location vector if required.
     */
    void loadLocations();

    /**
     * loadEventTypes
     *
     * Populates the event type vector if required.
     */
    void loadEventTypes();

    /**
     * loadEventColours
     *
     * Populates the event colour vector if required.
     */
    void loadEventColours();

    // Vectors of subsystems and locations. These hold the database objects once loaded, so the do not need to be loaded
    // for every query.
    std::map<unsigned long,TA_Base_Core::ISubsystem*> m_subsystems;
    std::map<unsigned long,TA_Base_Core::ILocation*> m_locations;
    std::vector<TA_Base_Core::IMessageTypeData*> m_eventTypes;

	// map of name and foreground/background colours for display
	std::map<std::string, TA_Base_Core::IEventColourData*> m_eventColours;
};

#endif // !defined(AFX_DATABASEACCESS_H__CA3F2438_7F1E_4890_89F6_4A87A2B9E3A1__INCLUDED_)
