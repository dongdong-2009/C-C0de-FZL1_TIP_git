#ifndef LOCSUBDBCACHE_H_INCLUDED
#define LOCSUBDBCACHE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyDbCache.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Caches all the information the Duty Agent requires from the database.
  */


namespace TA_Base_Bus
{
	class DutyDbCacheUpdater;

	struct RightsPK
	{
		unsigned long profile_key;
		unsigned long subsystem_key;
		unsigned long subsystem_state_key;
		RightsPK(	unsigned long profile_key,
			unsigned long subsystem_key,
			unsigned long subsystem_state_key):
		profile_key(profile_key),
			subsystem_key(subsystem_key),
			subsystem_state_key(subsystem_state_key)
		{
		}
		
		bool operator<( const RightsPK& rhs) const
		{
			if(profile_key < rhs.profile_key)
			{
				return true;
			}

			if(profile_key == rhs.profile_key)
			{
				if(subsystem_key < rhs.subsystem_key)
				{
					return true;
				}
				if(subsystem_key == rhs.subsystem_key)
				{
					if(subsystem_state_key < rhs.subsystem_state_key)
					{
						return true;
					}
				}
			}
			
			return false;
		}
	};

	class DutyDbCacheImpl : public SeObserver<AgentModeSubjectImpl>,
		                    public SeObserver<AgentTerminateSubjectImpl>
	{
	public:

		friend class ACE_Singleton<DutyDbCacheImpl, ACE_Recursive_Thread_Mutex>;
		friend class DutyDbCacheUpdater;

		// Observer updates
		virtual void update( AgentModeSubjectImpl& modeUpdate );
		virtual void update( AgentTerminateSubjectImpl& terminateUpdate );

		// Constants
	
		// Profiles
		IDutyPolicy::PROFILE_TYPE getProfileType    ( const DASType::DbKey& profileKey );
		DASType::DbKey           getRawProfileType ( const DASType::DbKey& profileKey );
		DASType::DbKey           getProfileGroup   ( const DASType::DbKey& profileKey );
		std::string              getProfileName    ( const DASType::DbKey& profileKey );
		std::string              getProfileTypeName( const DASType::DbKey& profileKey );

		IDutyPolicy::RIGHTS_TYPE getRightsType( const DASType::DbKey& profileKey,
			const DASType::DbKey& subsystemKey, const SubsystemState::State& subState );

		std::string getRightsName( const DASType::DbKey& profileKey,
			const DASType::DbKey& subsystemKey, const SubsystemState::State& subState );
		
		DASType::DbKey getRights( const DASType::DbKey& profileKey,
			const DASType::DbKey& subsystemKey, const SubsystemState::State& subState );

		// Locations
		void getLocationRegions( const DASType::DbKey& location, DASType::RegionSet& regions );
		void getAllLocationKeys( DASType::LocationSet& locations );
		bool locationIsOcc( const std::string& locationName );
		std::string getLocationName      ( const DASType::DbKey& location );
		std::string getLocationDisplayName( const DASType::DbKey& location );
		// Regions
		void        getAllSubsystemKeys( const DASType::DbKey& regionKey, DASType::SubsystemSet& subsystems );
		std::string getRegionName      ( const DASType::DbKey& regionKey );
		std::string getRegionDisplayName( const DASType::DbKey& regionKey );
		void        buildRegionMap     ( DASType::ReverseRegionMap& regionMap );
		void        getAllRegionKeys   ( DASType::RegionSet& regions );

		// Subsystems
		IDutyPolicy::SUBSYSTEM_TYPE getSubsystemType( const DASType::DbKey& subsystemKey );
		std::string                getSubsystemName( const DASType::DbKey& subsystemKey );

		// Rights
		bool dutyMayChange(const DASType::DbKey& profileKey, const DASType::DbKey& subsystemKey, 
			const SubsystemState::State& preState, const SubsystemState::State& postState );
			
		// Operators
		std::string  getOperatorName( const DASType::DbKey& operatorKey );
		void getOperatorRegions( const DASType::DbKey& operatorKey, DASType::RegionSet& regions );

		// GUI Entities
		ACE_Time_Value getPopupTimeout();

	private:

		static const std::string UNALLOCATED_SUBSYSTEM_NAME;
		static const std::string UNALLOCATED_LOCATION_NAME;
		static const std::string UNALLOCATED_REGION_NAME;
		static const std::string CONTROL_STATION_NAME;

		// Only we can construct and destruct ourselves
		DutyDbCacheImpl();
		~DutyDbCacheImpl();

		// Don't want to copy a Singleton
		DutyDbCacheImpl( const DutyDbCacheImpl& );
		DutyDbCacheImpl& operator=( const DutyDbCacheImpl& );

		// Profiles
		typedef std::map<DASType::DbKey, TA_Base_Core::IProfile*> ProfileMap;
		ProfileMap m_profileMap;
		
		DbCacheLookup<DASType::DbKey, TA_Base_Core::IProfile, TA_Base_Core::ProfileAccessFactory> m_profileLookup;

		ACE_Thread_Mutex m_profileLock;

		// Operators
		typedef std::map<DASType::DbKey, TA_Base_Core::IOperator*> OperatorMap;
		OperatorMap m_operatorMap;
		
		DbCacheLookup<DASType::DbKey, TA_Base_Core::IOperator, TA_Base_Core::OperatorAccessFactory> m_operatorLookup;

		ACE_Thread_Mutex m_operatorLock;

		// Subsystems
		typedef std::map<DASType::DbKey, TA_Base_Core::ISubsystem*> SubsystemMap;
		SubsystemMap m_subsystemMap;
		
		DbCacheLookup<DASType::DbKey, TA_Base_Core::ISubsystem, TA_Base_Core::SubsystemAccessFactory> m_subsystemLookup;

		ACE_Thread_Mutex m_subsystemLock;

		// Locations
		typedef std::map<DASType::DbKey, TA_Base_Core::ILocation*> LocationMap;
		LocationMap m_locationMap;
		
		DbCacheLookup<DASType::DbKey, TA_Base_Core::ILocation, TA_Base_Core::LocationAccessFactory> m_locationLookup;

		ACE_Thread_Mutex m_locationLock;


		// Regions
		typedef std::map<DASType::DbKey, TA_Base_Core::IRegion*> RegionMap;
		RegionMap m_regionMap;
		
		DbCacheLookup<DASType::DbKey, TA_Base_Core::IRegion, TA_Base_Core::RegionAccessFactory> m_regionLookup;

		ACE_Thread_Mutex m_regionLock;

		//Action Group
		typedef std::map<DASType::DbKey, TA_Base_Core::IActionGroup*> ActionGroupMap;
		ActionGroupMap m_actionGroupMap;

		DbCacheLookup<DASType::DbKey, TA_Base_Core::IActionGroup, TA_Base_Core::ActionGroupAccessFactory> m_actionGroupLookup;
		ACE_Thread_Mutex m_actionGroupLock;

		typedef std::map<RightsPK, unsigned long> RightsBook;
 		RightsBook m_rightsBook;
 		ACE_Thread_Mutex m_rightsLock;
		
		// Entities
		typedef std::map<DASType::DbKey, TA_Base_Core::IEntityData*> EntityMap;
		EntityMap m_entityMap;

		DbCacheLookup<DASType::DbKey, TA_Base_Core::IEntityData, TA_Base_Core::EntityAccessFactory> m_entityLookup;
		
		ACE_Thread_Mutex m_entityLock;

		DASType::DbKey m_controlStationKey;

		// Online updates
		DutyDbCacheUpdater* m_updater;
	};

	typedef ACE_Singleton<DutyDbCacheImpl, ACE_Recursive_Thread_Mutex> DutyDbCache;
}

#endif // LOCSUBDBCACHE_H_INCLUDED
