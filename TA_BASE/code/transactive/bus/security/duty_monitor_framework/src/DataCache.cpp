/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DataCache.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/03/10 10:15:27 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * This class stores duty-related configuration data.
  * If queries are made on data that is not currently stored, the database
  * is looked up and added to the storage.
  * DataCache subscribes to online updates, but does not refresh its data
  * when updates are received. The updates are simply passed on to the
  * attached observers.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"
#include "core/data_access_interface/src/ProfileHelper.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h" // Jinhua++ for lauching issue
namespace
{
	const std::string UNALLOCATED_OPERATOR_NAME("<Unallocated>");
	const std::string UNALLOCATED_LOCATION_NAME("<Unallocated>");
    const std::string UNALLOCATED_REGION_NAME("<Unallocated>");
	const std::string UNALLOCATED_SUBSYSTEM_NAME("<Unallocated>");
}

using namespace TA_Base_Bus;


DataCache::DataCache() :
    m_operatorsLoaded(false),
    m_profilesLoaded(false),
    m_actionGroupsLoaded(false),
    m_locationsLoaded(false),
    m_regionsLoaded(false),
    m_subsystemsLoaded(false),
    m_operatorLookup(m_operatorMap, TA_Base_Core::OperatorAccessFactory::getInstanceNoInit(),
                     &TA_Base_Core::OperatorAccessFactory::getOperator, &TA_Base_Core::OperatorAccessFactory::getAllOperatorsAndRegions),
    m_profileLookup(m_profileMap, TA_Base_Core::ProfileAccessFactory::getInstance(),
                    &TA_Base_Core::ProfileAccessFactory::getProfile, &TA_Base_Core::ProfileAccessFactory::getAllProfiles),
    m_actionGrpLookup(m_actionGrpMap, TA_Base_Core::ActionGroupAccessFactory::getInstance(),
                      &TA_Base_Core::ActionGroupAccessFactory::getActionGroup, &TA_Base_Core::ActionGroupAccessFactory::getAllActionGroups),
    m_locationLookup(m_locationMap, TA_Base_Core::LocationAccessFactory::getInstance(),
                     &TA_Base_Core::LocationAccessFactory::getLocationByKey, &TA_Base_Core::LocationAccessFactory::getAllLocations),
    m_regionLookup(m_regionMap, TA_Base_Core::RegionAccessFactory::getInstance(),
                   &TA_Base_Core::RegionAccessFactory::getRegionByKey, &TA_Base_Core::RegionAccessFactory::getAllRegions),
    m_subsystemLookup(m_subsystemMap, TA_Base_Core::SubsystemAccessFactory::getInstance(),
                      &TA_Base_Core::SubsystemAccessFactory::getSubsystemByKey, 
					  &TA_Base_Core::SubsystemAccessFactory::getAllSubsystems),

	m_workstationLookup(m_workstationMap,TA_Base_Core::ConsoleAccessFactory::getInstance(),
						&TA_Base_Core::ConsoleAccessFactory::getConsoleByKey,
						&TA_Base_Core::ConsoleAccessFactory::getAllConsoles),
    m_dataUpdate(NULL),
	m_operatorAccessFactory(TA_Base_Core::OperatorAccessFactory::getInstanceNoInit())
{
    FUNCTION_ENTRY("DataCache()");

    TA_Base_Core::ActionGroupAccessFactory::getInstance().getAllActionGroupNames(m_actionGroupNames);
    TA_Base_Core::OnlineUpdateListener& oul = TA_Base_Core::OnlineUpdateListener::getInstance();

    oul.registerAllInterests(TA_Base_Core::OPERATOR, *this);
    oul.registerAllInterests(TA_Base_Core::PROFILE, *this);
    oul.registerAllInterests(TA_Base_Core::ACTION_GROUP, *this);
    oul.registerAllInterests(TA_Base_Core::LOCATION, *this);
    oul.registerAllInterests(TA_Base_Core::REGION, *this);
    oul.registerAllInterests(TA_Base_Core::SUBSYSTEM, *this);

    FUNCTION_EXIT;
}


DataCache::~DataCache()
{
    FUNCTION_ENTRY("~DataCache()");

    TA_Base_Core::OnlineUpdateListener& oul = TA_Base_Core::OnlineUpdateListener::getInstance();

    oul.deregisterAllInterests(TA_Base_Core::OPERATOR, *this);
    oul.deregisterAllInterests(TA_Base_Core::PROFILE, *this);
    oul.deregisterAllInterests(TA_Base_Core::ACTION_GROUP, *this);
    oul.deregisterAllInterests(TA_Base_Core::LOCATION, *this);
    oul.deregisterAllInterests(TA_Base_Core::REGION, *this);
    oul.deregisterAllInterests(TA_Base_Core::SUBSYSTEM, *this);

    TA_Base_Core::ThreadGuard operatorGuard(m_operatorLock);

    for (OperatorMap::iterator operatorIt = m_operatorMap.begin(); operatorIt != m_operatorMap.end(); operatorIt++)
    {
        delete operatorIt->second;
        operatorIt->second = NULL;
    }

    m_operatorMap.clear();

    TA_Base_Core::ThreadGuard profileGuard(m_profileLock);

    for (ProfileMap::iterator profileIt = m_profileMap.begin(); profileIt != m_profileMap.end(); profileIt++)
    {
        delete profileIt->second;
        profileIt->second = NULL;
    }

    m_profileMap.clear();

    TA_Base_Core::ThreadGuard actionGrpGuard(m_actionGrpLock);

    for (ActionGroupMap::iterator actionGrpIt = m_actionGrpMap.begin(); actionGrpIt != m_actionGrpMap.end(); actionGrpIt++)
    {
        delete actionGrpIt->second;
        actionGrpIt->second = NULL;
    }

    m_actionGrpMap.clear();

    TA_Base_Core::ThreadGuard locationGuard(m_locationLock);

    for (LocationMap::iterator locationIt = m_locationMap.begin(); locationIt != m_locationMap.end(); locationIt++)
    {
        delete locationIt->second;
        locationIt->second = NULL;
    }

    m_locationMap.clear();

    TA_Base_Core::ThreadGuard regionGuard(m_regionLock);

    for (RegionMap::iterator regionIt = m_regionMap.begin(); regionIt != m_regionMap.end(); regionIt++)
    {
        delete regionIt->second;
        regionIt->second = NULL;
    }

    m_regionMap.clear();

    TA_Base_Core::ThreadGuard subsystemGuard(m_subsystemLock);

    for (SubsystemMap::iterator subsystemIt = m_subsystemMap.begin(); subsystemIt != m_subsystemMap.end(); subsystemIt++)
    {
        delete subsystemIt->second;
        subsystemIt->second = NULL;
    }

    m_subsystemMap.clear();

	TA_Base_Core::ThreadGuard workstationGuard(m_workstationLock);

	for(WorkstationMap::iterator workstationItr = m_workstationMap.begin(); workstationItr!= m_workstationMap.end();workstationItr ++)
	{
		delete workstationItr->second;
		workstationItr->second = NULL;
	}
	m_workstationMap.clear();

    TA_Base_Core::ThreadGuard dataUpdateGuard(m_dataUpdateLock);

    delete m_dataUpdate;
    m_dataUpdate = NULL;

	//TD18095,jianghp; do not call cleanUp here, or it will hang there
	//DutyDataStore::cleanUp();
    FUNCTION_EXIT;
}


OperatorKeyList DataCache::getOperatorKeys()
{
    FUNCTION_ENTRY("getOperatorKeys()");

    TA_Base_Core::ThreadGuard operatorGuard(m_operatorLock);

    if (!m_operatorsLoaded)
    {
        try
        {
            m_operatorLookup.populate();        
            m_operatorsLoaded = true;
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve configured operators.", ex.what()));
			TA_THROW(DutyMonitorFrameworkException("获取已配置的用户信息失败", ex.what()));
        }
        catch (...)
        {
            //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve configured operators."));
			TA_THROW(DutyMonitorFrameworkException("获取已配置的用户信息失败"));
        }
    }

    bool ignoreSystemOperators(DutyDataStore::ignoreSystemOperators());

    OperatorKeyList operatorKeys;
    operatorKeys.reserve(m_operatorMap.size());

    for (OperatorMap::const_iterator operatorIt = m_operatorMap.begin(); operatorIt != m_operatorMap.end(); operatorIt++)
    {
        // Discard any unwanted operators before returning the keys
        if (ignoreSystemOperators && operatorIt->second->isSystemOperator() ||
            UNALLOCATED_OPERATOR_NAME == operatorIt->second->getName())
        {
            continue;
        }

        operatorKeys.push_back(operatorIt->first);
    }

    FUNCTION_EXIT;
    return operatorKeys;
}


ProfileKeyList DataCache::getProfileKeys()
{
    FUNCTION_ENTRY("getProfileKeys()");

    TA_Base_Core::ThreadGuard profileGuard(m_profileLock);

    if (!m_profilesLoaded)
    {
        try
        {
            m_profileLookup.populate();
            m_profilesLoaded = true;
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve configured profiles.", ex.what()));
			TA_THROW(DutyMonitorFrameworkException("获取已配置的属性信息失败", ex.what()));
        }
        catch (...)
        {
            //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve configured profiles."));
			TA_THROW(DutyMonitorFrameworkException("获取已配置的属性信息失败"));
        }
    }

    ProfileKeyList profileKeys;
    profileKeys.reserve(m_profileMap.size());

    for (ProfileMap::const_iterator profileIt = m_profileMap.begin(); profileIt != m_profileMap.end(); profileIt++)
    {
        profileKeys.push_back(profileIt->first);
    }

    FUNCTION_EXIT;
    return profileKeys;
}


ActionGroupKeyList DataCache::getActionGroupKeys()
{
    FUNCTION_ENTRY("getActionGroupKeys()");
    
    std::vector<unsigned long> groupKeys;

	std::map<unsigned long, std::string>::iterator it = m_actionGroupNames.begin();
	for (it = m_actionGroupNames.begin(); it != m_actionGroupNames.end(); ++it)
	{
		groupKeys.push_back(it->first);
	}
    //TA_Base_Core::ActionGroupAccessFactory::getInstance().getAllActionGroupKeys(groupKeys);
    FUNCTION_EXIT;

    return groupKeys;

    /*
    TA_Base_Core::ThreadGuard actionGrpGuard(m_actionGrpLock);

    if (!m_actionGroupsLoaded)
    {
        try
        {
            m_actionGrpLookup.populate();
            m_actionGroupsLoaded = true;
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            TA_THROW(DutyMonitorFrameworkException("Failed to retrieve configured action groups.", ex.what()));
        }
        catch (...)
        {
            TA_THROW(DutyMonitorFrameworkException("Failed to retrieve configured action groups."));
        }
    }

    ActionGroupKeyList actionGrpKeys;
    actionGrpKeys.reserve(m_actionGrpMap.size());

    for (ActionGroupMap::const_iterator actionGrpIt = m_actionGrpMap.begin(); actionGrpIt != m_actionGrpMap.end(); actionGrpIt++)
    {
        actionGrpKeys.push_back(actionGrpIt->first);
    }

    FUNCTION_EXIT;
    return actionGrpKeys;
    */
}


LocationKeyList DataCache::getLocationKeys()
{
    FUNCTION_ENTRY("getLocationKeys()");

    TA_Base_Core::ThreadGuard locationGuard(m_locationLock);

    if (!m_locationsLoaded)
    {
        try
        {
            m_locationLookup.populate();
            m_locationsLoaded = true;
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve configured locations.", ex.what()));
			TA_THROW(DutyMonitorFrameworkException("获取已配置的位置信息失败", ex.what()));
        }
        catch (...)
        {
            //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve configured locations."));
			TA_THROW(DutyMonitorFrameworkException("获取已配置的位置信息失败"));
        }
    }

    LocationKeyList locationKeys;
    locationKeys.reserve(m_locationMap.size());

    for (LocationMap::const_iterator locationIt = m_locationMap.begin(); locationIt != m_locationMap.end(); locationIt++)
    {
        if (UNALLOCATED_LOCATION_NAME != locationIt->second->getName())
        {
            locationKeys.push_back(locationIt->first);
        }
    }

    FUNCTION_EXIT;
    return locationKeys;
}


RegionKeyList DataCache::getRegionKeys()
{
    FUNCTION_ENTRY("getRegionKeys()");

    TA_Base_Core::ThreadGuard regionGuard(m_regionLock);

    if (!m_regionsLoaded)
    {
        try
        {
            m_regionLookup.populate();
            m_regionsLoaded = true;
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve configured regions.", ex.what()));
			TA_THROW(DutyMonitorFrameworkException("获取已配置的位置信息失败", ex.what()));
        }
        catch (...)
        {
            //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve configured regions."));
			TA_THROW(DutyMonitorFrameworkException("获取已配置的位置信息失败"));
        }
    }

    RegionKeyList regionKeys;

    regionKeys.reserve(m_regionMap.size());

    for (RegionMap::const_iterator regionIt = m_regionMap.begin(); regionIt != m_regionMap.end(); regionIt++)
    {
        if (UNALLOCATED_REGION_NAME != regionIt->second->getName())
        {
            regionKeys.push_back(regionIt->first);
        }
    }

    FUNCTION_EXIT;
    return regionKeys;
}


SubsystemKeyList DataCache::getSubsystemKeys()
{
    FUNCTION_ENTRY("getSubsystemKeys()");

    TA_Base_Core::ThreadGuard subsystemGuard(m_subsystemLock);

    if (!m_subsystemsLoaded)
    {
        try
        {
            m_subsystemLookup.populate();
            m_subsystemsLoaded = true;
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve configured subsystems.", ex.what()));
			TA_THROW(DutyMonitorFrameworkException("获取已配置的子系统信息失败", ex.what()));
        }
        catch (...)
        {
            //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve configured subsystems."));
			TA_THROW(DutyMonitorFrameworkException("获取已配置的子系统信息失败"));
        }
    }

    bool ignoreAppSubsystems(DutyDataStore::ignoreAppSubsystems());

    SubsystemKeyList subsystemKeys;
    subsystemKeys.reserve(m_subsystemMap.size());

    for (SubsystemMap::const_iterator subsystemIt = m_subsystemMap.begin(); subsystemIt != m_subsystemMap.end(); subsystemIt++)
    {
        // Discard any unwanted subsystems before returning the keys
        if (ignoreAppSubsystems && !subsystemIt->second->isPhysical() ||
            UNALLOCATED_SUBSYSTEM_NAME == subsystemIt->second->getName())
        {
            continue;
        }

        subsystemKeys.push_back(subsystemIt->first);
    }

    FUNCTION_EXIT;
    return subsystemKeys;
}


std::string DataCache::getOperatorName(OperatorKey key)
{
//    FUNCTION_ENTRY("getOperatorName(OperatorKey key)");

    TA_Base_Core::ThreadGuard operatorGuard(m_operatorLock);

    TA_Base_Core::IOperator& operatr = getOperator(key);

//    FUNCTION_EXIT;
    return operatr.getName();
}


std::string DataCache::getOperatorDescription(OperatorKey key)
{
//    FUNCTION_ENTRY("getOperatorDescription(OperatorKey key)");

    TA_Base_Core::ThreadGuard operatorGuard(m_operatorLock);

    TA_Base_Core::IOperator& operatr = getOperator(key);

//    FUNCTION_EXIT;
    return operatr.getDescription();
}


std::string DataCache::getProfileName(ProfileKey key)
{
//    FUNCTION_ENTRY("getProfileName(ProfileKey key)");

    TA_Base_Core::ThreadGuard profileGuard(m_profileLock);

    // TD10468
    std::string profileName;
    try
    {
        TA_Base_Core::IProfile& profile = getProfile(key);
        profileName = profile.getName();
    }
    catch (TA_Base_Core::TransactiveException& ex)
    {
        // if it's a deleted profile, we still need to get the required data from database
        // because it might be a deleted profile with an active session
        try
        {
            profileName = TA_Base_Core::ProfileAccessFactory::getInstance().getDeletedProfileName(key);
        }
        catch (TA_Base_Core::TransactiveException&)
        {
            throw ex;
        }
    }

//    FUNCTION_EXIT;
    return profileName;
}

std::string DataCache::getProfileDisplayName(ProfileKey key)
{
//    FUNCTION_ENTRY("getProfileName(ProfileKey key)");

    TA_Base_Core::ThreadGuard profileGuard(m_profileLock);

    // TD10468
    std::string profileName;
    try
    {
        TA_Base_Core::IProfile& profile = getProfile(key);
        profileName = profile.getDisplayName();
    }
    catch (TA_Base_Core::TransactiveException& ex)
    {
        // if it's a deleted profile, we still need to get the required data from database
        // because it might be a deleted profile with an active session
        try
        {
            profileName = TA_Base_Core::ProfileAccessFactory::getInstance().getDeletedProfileName(key);
        }
        catch (TA_Base_Core::TransactiveException&)
        {
            throw ex;
        }
    }

//    FUNCTION_EXIT;
    return profileName;
}

std::string DataCache::getActionGroupName(ActionGroupKey key)
{
//    FUNCTION_ENTRY("getActionGroupName(ActionGroupKey key)");
/*
    TA_Base_Core::ThreadGuard actionGrpGuard(m_actionGrpLock);

    TA_Base_Core::IActionGroup& actionGroup = getActionGroup(key);
*/
    /*std::map<unsigned long, std::string>::iterator it;
    if ((it=m_actionGroupNames.find(key))!=m_actionGroupNames.end())
    {
        return it->second;
    }
	else
	{
        TA_Base_Core::ActionGroupAccessFactory::getInstance().getAllActionGroupNames(m_actionGroupNames);
        std::string rt = TA_Base_Core::ActionGroupAccessFactory::getInstance().getActionGroupName(key);
        return rt;
    }*/

	std::string actionGrpName("");
	std::map<unsigned long, std::string>::iterator it = m_actionGroupNames.find(key);
	if (it != m_actionGroupNames.end())
	{
		actionGrpName = it->second;
	}

	return actionGrpName;
//    FUNCTION_EXIT;

}

std::string DataCache::getActionGroupDisplayName(ActionGroupKey key)
{
//    FUNCTION_ENTRY("getActionGroupName(ActionGroupKey key)");

    TA_Base_Core::ThreadGuard actionGrpGuard(m_actionGrpLock);

    TA_Base_Core::IActionGroup& actionGroup = getActionGroup(key);
	
	return actionGroup.getDisplayName();

//    FUNCTION_EXIT;

}



std::string DataCache::getLocationName(LocationKey key)
{
//    FUNCTION_ENTRY("getLocationName(LocationKey key)");

    TA_Base_Core::ThreadGuard locationGuard(m_locationLock);

    TA_Base_Core::ILocation& location = getLocation(key);
    
    // The authentication subsystem returns the "unallocated" location for central
    // profiles. Until this behaviour is modified, any location name lookups on the
    // "unallocated" location key will return the name "Central". 
    if (UNALLOCATED_LOCATION_NAME == location.getName())
    {
//        FUNCTION_EXIT;
        return "Central";
    }

//    FUNCTION_EXIT;
    return location.getName();
}

std::string DataCache::getLocationDisplayName(LocationKey key)
{
	 FUNCTION_ENTRY("getLocationDisplayName(LocationKey key)");

	TA_Base_Core::ThreadGuard locationGuard(m_locationLock);

	TA_Base_Core::ILocation& location = getLocation(key);

	// The authentication subsystem returns the "unallocated" location for central
	// profiles. Until this behaviour is modified, any location name lookups on the
	// "unallocated" location key will return the name "Central". 
	if (UNALLOCATED_LOCATION_NAME == location.getName())
	{
		//        FUNCTION_EXIT;
		return "Central";
	}

	FUNCTION_EXIT;
	return location.getDisplayName();
}

std::string DataCache::getRegionName(RegionKey key)
{
//    FUNCTION_ENTRY("getRegionName(RegionKey key)");

    TA_Base_Core::ThreadGuard regionGuard(m_regionLock);

    TA_Base_Core::IRegion& region = getRegion(key);

//    FUNCTION_EXIT;
    return region.getName();
}

std::string DataCache::getRegionDisplayName(RegionKey key)
{
	//    FUNCTION_ENTRY("getRegionName(RegionKey key)");

	TA_Base_Core::ThreadGuard regionGuard(m_regionLock);

	TA_Base_Core::IRegion& region = getRegion(key);

	//    FUNCTION_EXIT;
	return region.getDisplayName();
}


std::string DataCache::getSubsystemName(SubsystemKey key)
{
//    FUNCTION_ENTRY("getSubsystemName(SubsystemKey key)");

    TA_Base_Core::ThreadGuard subsystemGuard(m_subsystemLock);

    TA_Base_Core::ISubsystem& subsystem = getSubsystem(key);

//    FUNCTION_EXIT;
    return subsystem.getName();
}

std::string DataCache::getSubsystemDisplayName(SubsystemKey key)
{
//    FUNCTION_ENTRY("getSubsystemName(SubsystemKey key)");

    TA_Base_Core::ThreadGuard subsystemGuard(m_subsystemLock);

    TA_Base_Core::ISubsystem& subsystem = getSubsystem(key);

//    FUNCTION_EXIT;
    return subsystem.getDisplayName();
}

std::string DataCache::getWorkstationName(WorkstationKey key)
{
	TA_Base_Core::ThreadGuard workstationGuard(m_workstationLock);

	TA_Base_Core::IConsole&  console = getWorkstation(key);

	return console.getName();
}


RegionKeyList DataCache::getAssociatedRegionKeys(OperatorKey key)
{
    FUNCTION_ENTRY("getAssociatedRegionKeys(OperatorKey key)");

    RegionKeyList associatedRegionKeys;

    /*try
    {
       TA_Base_Core::ThreadGuard operatorGuard(m_operatorLock);

       associatedRegionKeys = getOperator(key).getAssociatedRegionKeys();
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::ostringstream msg;
        msg << "Failed to retrieve regions associated with operator key " << key;

        TA_THROW(DutyMonitorFrameworkException(msg.str(), ex.what()));
    }
    catch (...)
    {
        std::ostringstream msg;
        msg << "Failed to retrieve regions associated with operator key " << key;

        TA_THROW(DutyMonitorFrameworkException(msg.str()));
    }*/
	
	m_operatorAccessFactory.getRegionByOperator(key, associatedRegionKeys);
	
    // Ensure the associated region keys are valid. Throw away all associations with the "unallocated" region.
    for (RegionKeyList::iterator rgnKeyIt = associatedRegionKeys.begin(); rgnKeyIt != associatedRegionKeys.end();)
    {
        try
        {
            TA_Base_Core::ThreadGuard regionGuard(m_regionLock);
            
            if (UNALLOCATED_REGION_NAME == getRegion(*rgnKeyIt).getName())
            {
                rgnKeyIt = associatedRegionKeys.erase(rgnKeyIt);
                continue;
            }

            rgnKeyIt++;
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            std::ostringstream msg;
            //msg << "Operator matching key " << key << " is associated with an invalid region key " << *rgnKeyIt;
			msg << "操作员" << key << "所分配的是一个无效的值" << *rgnKeyIt;

            TA_THROW(DutyMonitorFrameworkException(msg.str(), ex.what()));
        }
        catch (...)
        {
            std::ostringstream msg;
			//msg << "Operator matching key " << key << " is associated with an invalid region key " << *rgnKeyIt;
			msg << "操作员" << key << "所分配的是一个无效的值" << *rgnKeyIt;

            TA_THROW(DutyMonitorFrameworkException(msg.str()));
        }
    }

    FUNCTION_EXIT;
    return associatedRegionKeys;
}


SubsystemKeyList DataCache::getAssociatedSubsystemKeys(RegionKey key)
{
    FUNCTION_ENTRY("getAssociatedSubsystemKeys(RegionKey key)");

    SubsystemKeyList associatedSubsystemKeys;

    try
    {
        bool ignoreAppSubsystems(DutyDataStore::ignoreAppSubsystems());

        // Subsystem-region assignments are not cached.
        typedef std::vector<TA_Base_Core::ISubsystem*> SubsystemList;
        SubsystemList subsystems(TA_Base_Core::SubsystemAccessFactory::getInstance().getSubsystemsByRegion(key));
        associatedSubsystemKeys.reserve(subsystems.size());

        for (SubsystemList::iterator subsystemIt = subsystems.begin(); subsystemIt != subsystems.end(); subsystemIt++)
        {
            TA_Base_Core::ThreadGuard subsystemGuard(m_subsystemLock);
            
            // Ensure the given subsystem key is valid.
            TA_Base_Core::ISubsystem& subsystem = getSubsystem((*subsystemIt)->getKey());

            // Discard any unwanted subsystem associations.
            if (ignoreAppSubsystems && !subsystem.isPhysical() ||
                UNALLOCATED_SUBSYSTEM_NAME == subsystem.getName())
            {
                continue;
            }

            associatedSubsystemKeys.push_back((*subsystemIt)->getKey());

            delete *subsystemIt;  // no longer of use
            *subsystemIt = NULL;
        }

        subsystems.clear();
    }
    catch (const DutyMonitorFrameworkException& ex)
    {
        std::ostringstream msg;
        //msg << "Region matching key " << key << " is associated with an invalid subsystem key.";
		msg << "位置" << key << "所分配的子系统是无效子系统";

        TA_THROW(DutyMonitorFrameworkException(msg.str(), ex.what()));
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::ostringstream msg;
        //msg << "Failed to retrieve subsystems associated with region key " << key;
		msg << "未能获取到位置" << key<<"相关的子系统";

        TA_THROW(DutyMonitorFrameworkException(msg.str(), ex.what()));
    }
    catch (...)
    {
        std::ostringstream msg;
        //msg << "Failed to retrieve subsystems associated with region key " << key;
		msg << "未能获取到位置" << key<<"相关的子系统";

        TA_THROW(DutyMonitorFrameworkException(msg.str()));
    }
     
    FUNCTION_EXIT;
    return associatedSubsystemKeys;    
}


SubsystemKeyList DataCache::getAccessibleSubsystemKeys(ProfileKey key)
{
//    FUNCTION_ENTRY("getAccessibleSubsystemKeys(ProfileKey key)");

    SubsystemKeyList accessibleSubsystemKeys;
    
    bool isSystemProfile = false;
    {

    }

    try
    {
       TA_Base_Core::ThreadGuard profileGuard(m_profileLock);

       accessibleSubsystemKeys = getProfile(key).getAccessibleSubsystemKeys();
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        // TD10468
        // if it's a deleted profile there will be no accessibleSubSystemKeys.
        try
        {
            isSystemProfile = TA_Base_Core::ProfileAccessFactory::getInstance().getDeletedProfileIsSystemProfile(key);
        }
        catch (TA_Base_Core::TransactiveException&)
        {
            std::ostringstream msg;
            //msg << "Failed to retrieve accessible subsystems associated with profile key " << key;
			msg << "未能获取到属性" << key<<"所分配可使用的子系统";

            TA_THROW(DutyMonitorFrameworkException(msg.str(), ex.what()));
        }
    }
    catch (...)
    {
        std::ostringstream msg;
		//msg << "Failed to retrieve accessible subsystems associated with profile key " << key;
		msg << "未能获取到属性" << key<<"所分配可使用的子系统";

        TA_THROW(DutyMonitorFrameworkException(msg.str()));
    }

    bool ignoreAppSubsystems(DutyDataStore::ignoreAppSubsystems());

    // Ensure the acessible subsystem keys are valid. Throw away any unwanted subsystems.
    for (SubsystemKeyList::iterator subKeyIt = accessibleSubsystemKeys.begin(); subKeyIt != accessibleSubsystemKeys.end();)
    {
        try
        {
            TA_Base_Core::ThreadGuard subsystemGuard(m_subsystemLock);

            TA_Base_Core::ISubsystem& subsystem = getSubsystem(*subKeyIt);
            
            if (ignoreAppSubsystems && !subsystem.isPhysical() ||
                UNALLOCATED_SUBSYSTEM_NAME == subsystem.getName())
            {
                subKeyIt = accessibleSubsystemKeys.erase(subKeyIt);
                continue;
            }

            subKeyIt++;
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            std::ostringstream msg;
            //msg << "Profile matching key " << key << " is associated with an invalid subsystem key " << *subKeyIt;
			msg << "属性" << key << "所分配的子系统" << *subKeyIt<<"是无效的";

            TA_THROW(DutyMonitorFrameworkException(msg.str(), ex.what()));
        }
        catch (...)
        {
            std::ostringstream msg;
			//msg << "Profile matching key " << key << " is associated with an invalid subsystem key " << *subKeyIt;
			msg << "属性" << key << "所分配的子系统" << *subKeyIt<<"是无效的";

            TA_THROW(DutyMonitorFrameworkException(msg.str()));
        }
    }

//    FUNCTION_EXIT;
    return accessibleSubsystemKeys;
}


bool DataCache::isSystemOperator(OperatorKey key)
{
//    FUNCTION_ENTRY("isSystemOperator(OperatorKey key)");

    TA_Base_Core::ThreadGuard operatorGuard(m_operatorLock);

    //TD15069
    bool isSystemOperator = false;
    try
    {
        TA_Base_Core::IOperator& operatr = getOperator(key);
        isSystemOperator = operatr.isSystemOperator();
    }
    catch (TA_Base_Core::TransactiveException& ex)
    {
        // if it's a deleted Operator, we still need to get the required data from database
        // because it might be a deleted Operator with an active session
        try
        {
            isSystemOperator = TA_Base_Core::OperatorAccessFactory::getInstance().getDeletedOperatorIsSystemOperator(key);
        }
        catch (TA_Base_Core::TransactiveException&)
        {
            throw ex;
        }
    }

//    FUNCTION_EXIT;
	//return operatr.isSystemOperator();
    return isSystemOperator;
}


bool DataCache::isAppSubsystem(SubsystemKey key)
{
//    FUNCTION_ENTRY("isAppSubsystem(SubsystemKey key)");

    TA_Base_Core::ThreadGuard subsystemGuard(m_subsystemLock);

    TA_Base_Core::ISubsystem& subsystem = getSubsystem(key);

//    FUNCTION_EXIT;
    return !subsystem.isPhysical();
}

bool DataCache::isExclusiveSubSystem(SubsystemKey key)
{
//    FUNCTION_ENTRY("isAppSubsystem(SubsystemKey key)");

    TA_Base_Core::ThreadGuard subsystemGuard(m_subsystemLock);

    TA_Base_Core::ISubsystem& subsystem = getSubsystem(key);

//    FUNCTION_EXIT;
    return  subsystem.isExclusive();
}


bool DataCache::isSystemProfile(ProfileKey key)
{
//    FUNCTION_ENTRY("isSystemProfile(ProfileKey key)");

    TA_Base_Core::ThreadGuard profileGuard(m_profileLock);

    // TD10468
    bool isSystemProfile = false;
    try
    {
        TA_Base_Core::IProfile& profile = getProfile(key);
        isSystemProfile = profile.isSystemProfile();
    }
    catch (TA_Base_Core::TransactiveException& ex)
    {
        // if it's a deleted profile, we still need to get the required data from database
        // because it might be a deleted profile with an active session
        try
        {
            isSystemProfile = TA_Base_Core::ProfileAccessFactory::getInstance().getDeletedProfileIsSystemProfile(key);
        }
        catch (TA_Base_Core::TransactiveException&)
        {
            throw ex;
        }
    }

//    FUNCTION_EXIT;
    return isSystemProfile;
}


bool DataCache::isCentralProfile(ProfileKey key)
{
//    FUNCTION_ENTRY("isCentralProfile(ProfileKey key)");

    TA_Base_Core::ThreadGuard profileGuard(m_profileLock);

    // TD10468
    bool isExclusiveProfile = false;
    try
    {
        TA_Base_Core::IProfile& profile = getProfile(key);
        isExclusiveProfile = profile.isExclusive();
    }
    catch (TA_Base_Core::TransactiveException& ex)
    {
        // if it's a deleted profile, we still need to get the required data from database
        // because it might be a deleted profile with an active session
        try
        {
            unsigned long rawProfileType = TA_Base_Core::ProfileAccessFactory::getInstance().getDeletedRawProfileType(key);
            if ( rawProfileType != TA_Base_Core::ProfileHelper::LOCAL_PROFILE_TYPE )
            {
                isExclusiveProfile = true;
            }
        }
        catch (TA_Base_Core::TransactiveException&)
        {
            throw ex;
        }
    }


//    FUNCTION_EXIT;
    return isExclusiveProfile;
}


const DataUpdate* DataCache::getDataUpdate()
{
    FUNCTION_ENTRY("getDataUpdate()");

    TA_Base_Core::ThreadGuard dataUpdateGuard(m_dataUpdateLock);
    
    FUNCTION_EXIT;
    return m_dataUpdate;
}


// TA_Base_Core::IOnlineUpdatable interface implementation
void DataCache::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
    FUNCTION_ENTRY("processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)");

    try
    {
        // Do not propagate updates concerning system operators or non-physical systems 
        // if clients are not interested in them.
        switch (updateEvent.getType())
        {
            case TA_Base_Core::OPERATOR:
                if (DutyDataStore::ignoreSystemOperators() && isSystemOperator(updateEvent.getKey()))
                {
                    FUNCTION_EXIT;
                    return;
                }

            case TA_Base_Core::SUBSYSTEM:
                if (DutyDataStore::ignoreAppSubsystems() && isAppSubsystem(updateEvent.getKey()))
                {
                    FUNCTION_EXIT;
                    return;
                }

            default:
                ;
        }

        {
            TA_Base_Core::ThreadGuard dataUpdateGuard(m_dataUpdateLock);

            TA_ASSERT(m_dataUpdate == NULL, "Stored data update pointer is not NULL");
                      
            // Create a new data update based on the supplied details.
            m_dataUpdate = new DataUpdate;
            m_dataUpdate->updateType = updateEvent.getModifications();
            m_dataUpdate->itemType = updateEvent.getType();
            m_dataUpdate->itemKey = updateEvent.getKey();
        }

        // Let observers know there's a new update waiting to be picked up.
        notify(*this);

        {
            TA_Base_Core::ThreadGuard dataUpdateGuard(m_dataUpdateLock);
                         
            // All observers should now have processed the update.
            delete m_dataUpdate;
            m_dataUpdate = NULL;
        }
    }
    catch (const DutyMonitorFrameworkException& ex)
    {
        // Possibly invalid item key.
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                    "Ignoring config update for item type %d (%s)", updateEvent.getType(), ex.what());
    }              
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                    "Ignoring config update for item type %d", updateEvent.getType());
    }              

    FUNCTION_EXIT;
}


TA_Base_Core::IOperator& DataCache::getOperator(OperatorKey key)
{
//    FUNCTION_ENTRY("getOperator(OperatorKey key)");

    TA_Base_Core::ThreadGuard operatorGuard(m_operatorLock);

    TA_Base_Core::IOperator* operatr = NULL;

    try
    {
        operatr = m_operatorLookup.lookup(key);

        if (operatr == NULL)
        {
            TA_THROW(std::exception());
        }
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::ostringstream msg;
        //msg << "Failed to retrieve operator data matching key " << key;
		msg << "未能获取到操作员" << key<<"的数据信息";

        TA_THROW(DutyMonitorFrameworkException(msg.str(), ex.what()));
    }
    catch (...)
    {
        std::ostringstream msg;
		//msg << "Failed to retrieve operator data matching key " << key;
		msg << "未能获取到操作员" << key<<"的数据信息";

        TA_THROW(DutyMonitorFrameworkException(msg.str()));
    }
    
//    FUNCTION_EXIT;
    return *operatr;
}


TA_Base_Core::IProfile& DataCache::getProfile(ProfileKey key)
{
//    FUNCTION_ENTRY("getProfile(ProfileKey key)");

    TA_Base_Core::ThreadGuard profileGuard(m_profileLock);

    TA_Base_Core::IProfile* profile = NULL;

    try
    {
        profile = m_profileLookup.lookup(key);

        if (profile == NULL)
        {
            TA_THROW(std::exception());
        }
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::ostringstream msg;
        //msg << "Failed to retrieve profile data matching key " << key;
		msg << "未能获取到属性" << key<<"的数据信息";

        TA_THROW(DutyMonitorFrameworkException(msg.str(), ex.what()));
    }
    catch (...)
    {
        std::ostringstream msg;
        msg << "Failed to retrieve profile data matching key " << key;
		//msg << "Failed to retrieve profile data matching key " << key;
		msg << "未能获取到属性" << key<<"的数据信息";

        TA_THROW(DutyMonitorFrameworkException(msg.str()));
    }
    
//    FUNCTION_EXIT;
    return *profile;
}


TA_Base_Core::IActionGroup& DataCache::getActionGroup(ActionGroupKey key)
{
//    FUNCTION_ENTRY("getActionGroup(ActionGroupKey key)");

    TA_Base_Core::ThreadGuard actionGrpGuard(m_actionGrpLock);

    TA_Base_Core::IActionGroup* actionGroup = NULL;

    try
    {
        actionGroup = m_actionGrpLookup.lookup(key);

        if (actionGroup == NULL)
        {
            TA_THROW(std::exception());
        }
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::ostringstream msg;
        //msg << "Failed to retrieve action group data matching key " << key;
		msg << "未能获取到操作权限集" << key<<"的数据信息";

        TA_THROW(DutyMonitorFrameworkException(msg.str(), ex.what()));
    }
    catch (...)
    {
        std::ostringstream msg;
		//msg << "Failed to retrieve action group data matching key " << key;
		msg << "未能获取到操作权限集" << key<<"的数据信息";

        TA_THROW(DutyMonitorFrameworkException(msg.str()));
    }
    
//    FUNCTION_EXIT;
    return *actionGroup;
}


TA_Base_Core::ILocation& DataCache::getLocation(LocationKey key)
{
//    FUNCTION_ENTRY("getLocation(LocationKey key)");

    TA_Base_Core::ThreadGuard locationGuard(m_locationLock);

    TA_Base_Core::ILocation* location = NULL;

    try
    {
        location = m_locationLookup.lookup(key);

        if (location == NULL)
        {
            TA_THROW(std::exception());
        }
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::ostringstream msg;
        //msg << "Failed to retrieve location data matching key " << key;
		msg << "未能获取到位置" << key<<"的数据信息";

        TA_THROW(DutyMonitorFrameworkException(msg.str(), ex.what()));
    }
    catch (...)
    {
        std::ostringstream msg;
		//msg << "Failed to retrieve location data matching key " << key;
		msg << "未能获取到位置" << key<<"的数据信息";

        TA_THROW(DutyMonitorFrameworkException(msg.str()));
    }
    
//    FUNCTION_EXIT;
    return *location;
}


TA_Base_Core::IRegion& DataCache::getRegion(RegionKey key)
{
//    FUNCTION_ENTRY("getRegion(RegionKey key)");

    TA_Base_Core::ThreadGuard regionGuard(m_regionLock);

    TA_Base_Core::IRegion* region = NULL;

    try
    {
        region = m_regionLookup.lookup(key);

        if (region == NULL)
        {
            TA_THROW(std::exception());
        }
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::ostringstream msg;
        //msg << "Failed to retrieve region data matching key " << key;
		msg << "未能获取到位置" << key<<"的数据信息";

        TA_THROW(DutyMonitorFrameworkException(msg.str(), ex.what()));
    }
    catch (...)
    {
        std::ostringstream msg;
		//msg << "Failed to retrieve region data matching key " << key;
		msg << "未能获取到位置" << key<<"的数据信息";

        TA_THROW(DutyMonitorFrameworkException(msg.str()));
    }
    
//    FUNCTION_EXIT;
    return *region;
}


TA_Base_Core::ISubsystem& DataCache::getSubsystem(SubsystemKey key)
{
//    FUNCTION_ENTRY("getSubsystem(SubsystemKey key)");

    TA_Base_Core::ThreadGuard subsystemGuard(m_subsystemLock);

    TA_Base_Core::ISubsystem* subsystem = NULL;

    try
    {
        subsystem = m_subsystemLookup.lookup(key);

        if (subsystem == NULL)
        {
            TA_THROW(std::exception());
        }
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::ostringstream msg;
        //msg << "Failed to retrieve subsystem data matching key " << key;
		msg << "未能获取到子系统" << key<<"的数据信息";

        TA_THROW(DutyMonitorFrameworkException(msg.str(), ex.what()));
    }
    catch (...)
    {
        std::ostringstream msg;
		//msg << "Failed to retrieve subsystem data matching key " << key;
		msg << "未能获取到子系统" << key<<"的数据信息";

        TA_THROW(DutyMonitorFrameworkException(msg.str()));
    }
    
//    FUNCTION_EXIT;
    return *subsystem;
}

TA_Base_Core::IConsole& DataCache::getWorkstation(WorkstationKey key)
{
	//    FUNCTION_ENTRY("getSubsystem(SubsystemKey key)");
	
    TA_Base_Core::ThreadGuard consoleGuard(m_workstationLock);
	
    TA_Base_Core::IConsole* console = NULL;
	
    try
    {
        console = m_workstationLookup.lookup(key);
		
        if (console == NULL)
        {
            TA_THROW(std::exception());
        }
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::ostringstream msg;
        //msg << "Failed to retrieve console data matching key " << key;
		msg << "未能获取到工作站" << key<<"的数据信息";
		
        TA_THROW(DutyMonitorFrameworkException(msg.str(), ex.what()));
    }
    catch (...)
    {
        std::ostringstream msg;
		//msg << "Failed to retrieve console data matching key " << key;
		msg << "未能获取到工作站" << key<<"的数据信息";
		
        TA_THROW(DutyMonitorFrameworkException(msg.str()));
    }
    
	//    FUNCTION_EXIT;
    return *console;
}

// Jinhua++ for launching issue
std::vector< RegionSubsystemKey > DataCache::getAllSubsystemsByAllRegion( const bool readWrite /*= false*/ )
{
	FUNCTION_ENTRY( "getAllSubsystemsByAllRegion" );
	/*std::ostringstream sql;
	sql << " select v.SEREGI_ID, v.PHYSICAL_SUBSYSTEM_KEY from SE_REG_SUB_V v, subsystem s "
		<< " where v.PHYSICAL_SUBSYSTEM_KEY = s.pkey "
		<< "  and s.is_physical = 1 ";*/
	
	std::vector< RegionSubsystemKey > subsystemKeylist;
	RegionSubsystemKey regionSubsystemKey;
	std::vector<std::string> columnNames;
	columnNames.push_back( "SEREGI_ID" );
	columnNames.push_back("PHYSICAL_SUBSYSTEM_KEY");
	
	TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::OnlineUpdatable_Cd, TA_Base_Core::Read);

	TA_Base_Core::SQLStatement sql;
	databaseConnection->prepareSQLStatement(sql, SUBSYSTEM_SELECT_18007);

	TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);
	
	// Loop for each row returned
	do
	{
		for (unsigned long i = 0; i < data->getNumRows(); i++)
		{
			// The getUnsignedLongData() call can throw an exception. Need to catch
			// it to do pointer clean up.
			try
			{
				if ( !readWrite)
				{
					regionSubsystemKey.regionKey = data->getUnsignedLongData( i, columnNames[0] );
					regionSubsystemKey.subsystemKey = data->getUnsignedLongData(i, columnNames[1] );
					subsystemKeylist.push_back( regionSubsystemKey );
				}
			}
			catch (const TA_Base_Core::DataException&)
			{
				// Clean up the data pointer
				delete data;
				data = NULL;
				throw;
			}
		}
		delete data;
		data = NULL;
	}
	while ( databaseConnection->moreData(data) );
	
	FUNCTION_EXIT;
	return subsystemKeylist;
	}


SubsystemTreeMap DataCache::getAllAssociatedSubsystemkeys()
{
	FUNCTION_ENTRY( "getAllAssociatedSubsystemkeys" );

	SubsystemTreeMap allSubsystemKeyMap;
	std::vector< RegionSubsystemKey > allsubsystemKeylist( getAllSubsystemsByAllRegion( ) );

	std::vector< RegionSubsystemKey >::iterator alliter = allsubsystemKeylist.begin();
	for (; alliter != allsubsystemKeylist.end(); ++alliter )
	{
		SubsystemTreeMap::iterator findIter = allSubsystemKeyMap.find( alliter->regionKey );
		if ( findIter == allSubsystemKeyMap.end() )
		{
			findIter = allSubsystemKeyMap.insert( SubsystemTreeMap::value_type( alliter->regionKey, SubsystemKeyList() ) ).first;
		}

		findIter->second.push_back( alliter->subsystemKey );
	}

	FUNCTION_EXIT;
	return allSubsystemKeyMap;
}
// ++Jinhua
