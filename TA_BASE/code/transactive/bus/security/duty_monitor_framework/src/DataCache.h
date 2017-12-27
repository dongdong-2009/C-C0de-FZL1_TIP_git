#if !defined(_DATACACHE_H__C49A8988_9D04_46AE_87C1_1F8A35FCC7AE__INCLUDED_)
#define _DATACACHE_H__C49A8988_9D04_46AE_87C1_1F8A35FCC7AE__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DataCache.h $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 10:20:13 $
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

namespace TA_Base_Bus
{
    class DataCache : public TA_Base_Core::IOnlineUpdatable,
                      public SeSubject<DataCache>
    {
    public:
        DataCache();
        virtual ~DataCache();

        /**
          * Returns the keys of all configured operators.
          * If ignoreSystemOperators on DutyDataStoreSingleton has been
          * enabled, the list will contain human operator keys only.
          * Throws a DutyMonitorFrameworkException if operator keys 
          * cannot be extracted from the database.
          *
          */
        OperatorKeyList getOperatorKeys();

        /**
          * Returns the keys of all configured profiles.
          * Throws a DutyMonitorFrameworkException if profile keys 
          * cannot be extracted from the database.
          *
          */
        ProfileKeyList getProfileKeys();

        /**
          * Returns the keys of all configured action groups.
          * Throws a DutyMonitorFrameworkException if action group keys 
          * cannot be extracted from the database.
          *
          */
        ActionGroupKeyList getActionGroupKeys();

        /**
          * Returns the keys of all configured locations.
          * Throws a DutyMonitorFrameworkException if location keys 
          * cannot be extracted from the database.
          *
          */
        LocationKeyList getLocationKeys();

        /**
          * Returns the keys of all configured regions.
          * Throws a DutyMonitorFrameworkException if region keys 
          * cannot be extracted from the database.
          *
          */
        RegionKeyList getRegionKeys();

        /**
          * Returns the keys of all configured physical subsystems.
          * If ignoreAppSubsystems on DutyDataStoreSingleton has been
          * enabled, the list will contain physical subsystem keys only.
          * Throws a DutyMonitorFrameworkException if subsystem keys 
          * cannot be extracted from the database.
          *
          */
        SubsystemKeyList getSubsystemKeys();

        /**
          * Returns the login name of the operator matching the specified key.
          * Throws a DutyMonitorFrameworkException if the operator with 
          * the specified key cannot be extracted from the database.
          *
          */
        std::string getOperatorName(OperatorKey key);

        /**
          * Returns the full name of the operator matching the specified key.
          * Throws a DutyMonitorFrameworkException if the operator with 
          * the specified key cannot be extracted from the database.
          *
          */
        std::string getOperatorDescription(OperatorKey key);


        /**
          * Returns the name of the profile matching the specified key.
          * Throws a DutyMonitorFrameworkException if the profile with 
          * the specified key cannot be extracted from the database.
          *
          */
        std::string getProfileName(ProfileKey key);

		
		/**
          * Returns the display name of the profile matching the specified key.
          * Throws a DutyMonitorFrameworkException if the profile with 
          * the specified key cannot be extracted from the database.
          *
          */
        std::string getProfileDisplayName(ProfileKey key);
		
        /**
          * Returns the name of the action group matching the specified key.
          * Throws a DutyMonitorFrameworkException if the action group with 
          * the specified key cannot be extracted from the database.
          *
          */
        std::string getActionGroupName(ActionGroupKey key);
		
		/**
          * Returns the display name of the action group matching the specified key.
          * Throws a DutyMonitorFrameworkException if the action group with 
          * the specified key cannot be extracted from the database.
          *
          */
        std::string getActionGroupDisplayName(ActionGroupKey key);


        /**
          * Returns the name of the location matching the specified key.
          * Throws a DutyMonitorFrameworkException if the location with 
          * the specified key cannot be extracted from the database.
          *
          */
        std::string getLocationName(LocationKey key);

		/**
          * Returns the name of the location matching the specified key.
          * Throws a DutyMonitorFrameworkException if the location with 
          * the specified key cannot be extracted from the database.
          *
          */
        std::string getLocationDisplayName(LocationKey key);

        /**
          * Returns the name of the region matching the specified key.
          * Throws a DutyMonitorFrameworkException if the region with 
          * the specified key cannot be extracted from the database.
          *
          */
        std::string getRegionName(RegionKey key);

		/**
          * Returns the name of the region matching the specified key.
          * Throws a DutyMonitorFrameworkException if the region with 
          * the specified key cannot be extracted from the database.
          *
          */
        std::string getRegionDisplayName(RegionKey key);

        /**
          * Returns the name of the subsystem matching the specified key.
          * Throws a DutyMonitorFrameworkException if the subsystem with 
          * the specified key cannot be extracted from the database.
          *
          */
        std::string getSubsystemName(SubsystemKey key);

        /**
          * Returns the display name of the subsystem matching the specified key.
          * Throws a DutyMonitorFrameworkException if the subsystem with
          * the specified key cannot be extracted from the database.
          *
          */
        std::string getSubsystemDisplayName(SubsystemKey key);
		/**
          * Returns the name of the console matching the specified key.
          * Throws a DutyMonitorFrameworkException if the region with 
          * the specified key cannot be extracted from the database.
          *
          */
        std::string getWorkstationName(WorkstationKey key);

        /**
          * Returns the keys of regions assigned to the given operator.
          * Throws a DutyMonitorFrameworkException if the operator with 
          * the specified key cannot be extracted from the database.
          *
          */
        RegionKeyList getAssociatedRegionKeys(OperatorKey key);

        /**
          * Returns the keys of subsystems configured for the given region.
          * If ignoreAppSubsystems on DutyDataStoreSingleton has been
          * enabled, the list will contain physical subsystem keys only.
          * Throws a DutyMonitorFrameworkException if subsystem assignments
          * for the region with the specified key cannot be extracted from
          * the database.
          *
          */
        SubsystemKeyList getAssociatedSubsystemKeys(RegionKey key);

		/**
		  * Returns the keys of all subsystems configured for all configured region.
		  * Throws a DutyMonitorFrameworkException if subsystem assignments
		  * for the region with the specified key cannot be extracted from 
		  * the database.
		  *
		  */
		SubsystemTreeMap getAllAssociatedSubsystemkeys();//jinhua++for duty lauchmanager                                                                   
        /**
          * Returns the keys of subsystems which the given profile has access
          * rights for. If ignoreAppSubsystems on DutyDataStoreSingleton has 
          * been enabled, the list will contain physical subsystem keys only.
          * Throws a DutyMonitorFrameworkException if the accessible subsystems
          * for the profile with the specified key cannot be extracted from
          * the database.
          *
          */    
        SubsystemKeyList getAccessibleSubsystemKeys(ProfileKey key);

        /**
          * Returns true if the operator with the specified key is a system one (agent).
          * Throws a DutyMonitorFrameworkException if the operator with 
          * the specified key cannot be extracted from the database.
          *
          */
        bool isSystemOperator(OperatorKey key);

        /**
          * Returns true if the subsystem with the specified key is an application one (non-physical).
          * Throws a DutyMonitorFrameworkException if the profile with 
          * the specified key cannot be extracted from the database.
          *
          */
        bool isAppSubsystem(SubsystemKey key);

        /**
          * Returns true if the subsystem with the specified key is an exclusive.
          * Throws a DutyMonitorFrameworkException if the profile with 
          * the specified key cannot be extracted from the database.
          *
          */		
		bool isExclusiveSubSystem(SubsystemKey key);
        /**
          * Returns true if the profile with the specified key is a system one.
          * Throws a DutyMonitorFrameworkException if the profile with 
          * the specified key cannot be extracted from the database.
          *
          */
        bool isSystemProfile(ProfileKey key);

        /**
          * Returns true if the profile with the specified key is a central/exclusive one.
          * Throws a DutyMonitorFrameworkException if the profile with 
          * the specified key cannot be extracted from the database.
          *
          */
        bool isCentralProfile(ProfileKey key);

        /**
          * Returns config data update generated as the result of an online update.
          * NULL when there is no update to be picked up.
          *
          */
        const DataUpdate* getDataUpdate();

        /**
          * TA_Base_Core::IOnlineUpdatable interface
          *
          */
        virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

    private:
        DataCache(const DataCache& dataCache);
        DataCache& operator=(const DataCache& dataCache);

        /**
          * The following helper methods return the appropriate data object 
          * corresponding to the specified key.
          * DutyMonitorFrameworkException is thrown if the object matching   
          * the key cannot be extracted from the database (e.g. the key is invalid).
          *
          */
        TA_Base_Core::IOperator& getOperator(OperatorKey key);
        TA_Base_Core::IProfile& getProfile(ProfileKey key);
        TA_Base_Core::IActionGroup& getActionGroup(ActionGroupKey key);
        TA_Base_Core::ILocation& getLocation(LocationKey key);
        TA_Base_Core::IRegion& getRegion(RegionKey key);
        TA_Base_Core::ISubsystem& getSubsystem(SubsystemKey key);
		TA_Base_Core::IConsole& getWorkstation(WorkstationKey key);
		std::vector<RegionSubsystemKey> getAllSubsystemsByAllRegion( const bool readWrite = false );//jinhua++


    private:
        typedef std::map<OperatorKey, TA_Base_Core::IOperator*> OperatorMap;
        typedef std::map<ProfileKey, TA_Base_Core::IProfile*> ProfileMap;
        typedef std::map<ActionGroupKey, TA_Base_Core::IActionGroup*> ActionGroupMap;
        typedef std::map<LocationKey, TA_Base_Core::ILocation*> LocationMap;
        typedef std::map<RegionKey, TA_Base_Core::IRegion*> RegionMap;
        typedef std::map<SubsystemKey, TA_Base_Core::ISubsystem*> SubsystemMap;
		typedef std::map<WorkstationKey, TA_Base_Core::IConsole*> WorkstationMap;


    private:
        bool m_operatorsLoaded;
        bool m_profilesLoaded;
        bool m_actionGroupsLoaded;
        bool m_locationsLoaded;
        bool m_regionsLoaded;
        bool m_subsystemsLoaded;

        OperatorMap m_operatorMap;
        DbCacheLookupEx<OperatorKey, TA_Base_Core::IOperator, TA_Base_Core::OperatorAccessFactory> m_operatorLookup;
		TA_Base_Core::ReEntrantThreadLockable m_operatorLock;

        ProfileMap m_profileMap;
        DbCacheLookupEx<ProfileKey, TA_Base_Core::IProfile, TA_Base_Core::ProfileAccessFactory> m_profileLookup;
        TA_Base_Core::ReEntrantThreadLockable m_profileLock;

        ActionGroupMap m_actionGrpMap;
        DbCacheLookupEx<ActionGroupKey, TA_Base_Core::IActionGroup, TA_Base_Core::ActionGroupAccessFactory> m_actionGrpLookup;
        TA_Base_Core::ReEntrantThreadLockable m_actionGrpLock;

        LocationMap m_locationMap;
        DbCacheLookupEx<LocationKey, TA_Base_Core::ILocation, TA_Base_Core::LocationAccessFactory> m_locationLookup;
        TA_Base_Core::ReEntrantThreadLockable m_locationLock;

        RegionMap m_regionMap;
        DbCacheLookupEx<RegionKey, TA_Base_Core::IRegion, TA_Base_Core::RegionAccessFactory> m_regionLookup;        
        TA_Base_Core::ReEntrantThreadLockable m_regionLock;

        SubsystemMap m_subsystemMap;
        DbCacheLookupEx<SubsystemKey, TA_Base_Core::ISubsystem, TA_Base_Core::SubsystemAccessFactory> m_subsystemLookup;        
        TA_Base_Core::ReEntrantThreadLockable m_subsystemLock;

		WorkstationMap m_workstationMap;
		DbCacheLookupEx<WorkstationKey,TA_Base_Core::IConsole,TA_Base_Core::ConsoleAccessFactory> m_workstationLookup;
		TA_Base_Core::ReEntrantThreadLockable m_workstationLock;

        DataUpdate* m_dataUpdate;
        TA_Base_Core::ReEntrantThreadLockable m_dataUpdateLock;

        std::map<unsigned long, std::string> m_actionGroupNames;
		TA_Base_Core::OperatorAccessFactory& m_operatorAccessFactory;
    };

}

#endif // !defined(_DATACACHE_H__C49A8988_9D04_46AE_87C1_1F8A35FCC7AE__INCLUDED_)
