/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_DBSync/TA_BASE/transactive/app/event/event_viewer/src/DatabaseAccess.h $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2014/07/18 15:34:16 $
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
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class ISubsystem;   // forward declaration
    class ILocation;   // forward declaration
    class IMessageTypeData;
	class IEventColourData;
	class IAlarmSeverityData;
	class IAlarmTypeData;
	class IOperator;
	class AlarmStateData;
	class SystemData;
}
struct ISCSSubsystemItem
{
	ISCSSubsystemItem(unsigned long k, std::string s, unsigned long systemKey) : pkey(k), subsystemName(s) , system_key(systemKey){}


	bool operator<(const ISCSSubsystemItem& rhs)
	{
		return subsystemName.compare(rhs.subsystemName) < 0;
	}

	unsigned long	pkey;
	std::string		subsystemName;
	unsigned long   system_key;
};



class DatabaseAccess  
{
public:

   static const std::string AUDIT_CHANNEL_NAME;


   static DatabaseAccess& getInstance();

   void load4FilterDlg();



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


	std::vector<std::pair<unsigned long,std::string> > getSystems(); // add this for 955

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
    std::vector<std::pair<unsigned long,std::string> > getLocationKeyDispNamePairs();
    
	std::vector<std::pair<unsigned long,std::string> > DatabaseAccess::getLocationsKeyNamePairs();
    
	/**
     * getSubsystems
     *
     * @return A vector containing key,name combinations for all subsystems.
     * NOTE: While a vector<pair> is essentially a map, it is used in the code *as* a vector, and is hence returned 
     * as one.
     */
    std::vector<ISCSSubsystemItem>  getSubsystems();

	std::vector<std::pair<unsigned long,std::string> > getAlarmStates();

	bool findAlarmStateName(std::string name);


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
                             std::vector<unsigned long>& physicalSubsystems, std::vector<unsigned long>& atsSubsystems );
// 	void DatabaseAccess::separateSystems( const std::vector<unsigned long>& systems,std::vector<unsigned long>& ISCSsystems,
// 		                    std::vector<unsigned long>& atsSystems);

    /**
     * getSubsystemName
     *
     * @param subsystemId The subsystem key for which to retrieve the name
     *
     * @return The name of the subsystem with the specified key
     */
    const std::string getSubsystemName(const unsigned long subsystemId);

	const std::string getSystemName(const unsigned long systemId);

	const std::string getAlarmStateName(const unsigned long alarmStateID);

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
     * constructor
     */
	DatabaseAccess();

    /**
     * destrcutor
     */
	virtual ~DatabaseAccess();


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

	void loadAlarmStates();

	void loadSystems();

    /**
     * loadEventColours
     *
     * Populates the event colour vector if required.
     */
    void loadEventColours();


	 /**
     * loadOperators
     *
     * Populates the operators vector if required.
     */
    void loadOperators();

	/**
     * loadAlarmTypes
     *
     * Populates the alarm types vector if required.
     */
    void loadAlarmTypes();

	/**
     * loadAlarmSeveritys
     *
     * Populates the alram severitys vector if required.
     */
    void loadAlarmSeveritys();
    // Vectors of subsystems and locations. These hold the database objects once loaded, so the do not need to be loaded
    // for every query.
    std::map<unsigned long,TA_Base_Core::ISubsystem*> m_subsystems;
	// add this for 955 . to do: need to change ISubsystem to some new interface
    std::map<unsigned long,TA_Base_Core::SystemData*> m_systems;   

	// we will not check the size of m_systems to judge the loading is done or not. 
    // Instead, we use this boolean to record the loading of system table is done or not. 
	// This is to avoid multiple times of loading when the system table is empty.
	bool m_systemsLoaded; 
    std::map<unsigned long,TA_Base_Core::ILocation*> m_locations;
    std::vector<TA_Base_Core::IMessageTypeData*> m_eventTypes;  // this will not be useful for 955
	std::map<unsigned long, TA_Base_Core::AlarmStateData*> m_alarmStates;
	std::map<std::string , unsigned long > m_alarmStatesNameKey;
	 
	std::vector<TA_Base_Core::IAlarmSeverityData*>  m_severitys;
	std::vector<TA_Base_Core::IOperator*>   m_operators;
	std::vector<TA_Base_Core::IAlarmTypeData*>  m_alarmTypes;

	// map of name and foreground/background colours for display
	std::map<std::string, TA_Base_Core::IEventColourData*> m_eventColours;
	 static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;
	 static DatabaseAccess* m_instance;

	 TA_Base_Core::ReEntrantThreadLockable m_cachelock;
};

#endif // !defined(AFX_DATABASEACCESS_H__CA3F2438_7F1E_4890_89F6_4A87A2B9E3A1__INCLUDED_)
