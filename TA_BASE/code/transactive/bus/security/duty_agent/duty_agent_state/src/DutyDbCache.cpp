/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyDbCache.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Implements DutyDbCache.
  */

#include "DutyAgentState.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/security/duty_agent/duty_agent_state/src/SubsystemState.h"

namespace TA_Base_Bus
{
	const std::string DutyDbCacheImpl::UNALLOCATED_SUBSYSTEM_NAME("<Unallocated>");
	const std::string DutyDbCacheImpl::UNALLOCATED_LOCATION_NAME ("<Unallocated>");
	const std::string DutyDbCacheImpl::UNALLOCATED_REGION_NAME   ("<Unallocated>");
	const std::string DutyDbCacheImpl::CONTROL_STATION_NAME      ("ControlStation");

	DutyDbCacheImpl::DutyDbCacheImpl() :
		SeObserver<TA_Base_Bus::AgentModeSubjectImpl>(*TA_Base_Bus::AgentModeSubject::instance()),
		SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl>(*TA_Base_Bus::AgentTerminateSubject::instance()),
		m_profileLookup  (m_profileMap  ,TA_Base_Core::ProfileAccessFactory::getInstance()  ,&TA_Base_Core::ProfileAccessFactory::getProfile         ),
		m_operatorLookup (m_operatorMap ,TA_Base_Core::OperatorAccessFactory::getInstance() ,&TA_Base_Core::OperatorAccessFactory::getOperator       ),
		m_subsystemLookup(m_subsystemMap,TA_Base_Core::SubsystemAccessFactory::getInstance(),&TA_Base_Core::SubsystemAccessFactory::getSubsystemByKey),
		m_locationLookup (m_locationMap ,TA_Base_Core::LocationAccessFactory::getInstance() ,&TA_Base_Core::LocationAccessFactory::getLocationByKey  ),
		m_regionLookup   (m_regionMap   ,TA_Base_Core::RegionAccessFactory::getInstance()   ,&TA_Base_Core::RegionAccessFactory::getRegionByKey      ),
		m_entityLookup   (m_entityMap   ,TA_Base_Core::EntityAccessFactory::getInstance()   ,&TA_Base_Core::EntityAccessFactory::getEntity           ),
		m_actionGroupLookup(m_actionGroupMap,TA_Base_Core::ActionGroupAccessFactory::getInstance(), &TA_Base_Core::ActionGroupAccessFactory::getActionGroup ),
		m_controlStationKey(0),
		m_updater(NULL)
	{
		{
			ACE_Guard<ACE_Thread_Mutex> guard(m_locationLock);
			m_locationLookup.set( TA_Base_Core::LocationAccessFactory::getInstance().getAllAgentLocations() );
		}
		{
			ACE_Guard<ACE_Thread_Mutex> guard(m_regionLock);
			m_regionLookup.set( TA_Base_Core::RegionAccessFactory::getInstance().getAllRegions() );
		}
		{
			ACE_Guard<ACE_Thread_Mutex> guard(m_subsystemLock);
			m_subsystemLookup.set( TA_Base_Core::SubsystemAccessFactory::getInstance().getAllPhysicalSubsystems() );
		}
		
 		{
 			ACE_Guard<ACE_Thread_Mutex> guard(m_profileLock);
 			m_profileLookup.set(TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles());
 		}
 
 		{
 			ACE_Guard<ACE_Thread_Mutex> guard(m_operatorLock);
 			m_operatorLookup.set(TA_Base_Core::OperatorAccessFactory::getInstance().getAllOperators());
 		}
		{
			ACE_Guard<ACE_Thread_Mutex> guard(m_actionGroupLock);
			m_actionGroupLookup.set(TA_Base_Core::ActionGroupAccessFactory::getInstance().getAllActionGroups());
		}
		{
 			ACE_Guard<ACE_Thread_Mutex> guard(m_rightsLock);
			std::vector<TA_Base_Core::IRights*> rights =  TA_Base_Core::RightsAccessFactory::getInstance().getAllRights(true);
			std::vector<TA_Base_Core::IRights*>::iterator end = rights.end();
			for(std::vector<TA_Base_Core::IRights*>::iterator itr =rights.begin(); itr != end; ++itr )
 			{
				TA_Base_Core::IRights* pRight = (*itr);
				m_rightsBook.insert(RightsBook::value_type(RightsPK(pRight->getProfileKey(), pRight->getSubsystemKey(), pRight->getSubsystemStateKey()),
					 pRight->getActionGroupKey()));			
				delete pRight ;
				pRight = NULL;
 			}

		}
		std::auto_ptr<TA_Base_Core::ControlStation> cs( dynamic_cast<TA_Base_Core::ControlStation*>
			(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(CONTROL_STATION_NAME)) );

		m_controlStationKey = cs->getKey();

		/* Cache action group*/
// 		SubsystemMap::iterator subIter;
// 		ProfileMap::iterator proIter;
// 
// 		unsigned long subsystemStateIter;
// 		unsigned long subsystemStateBegin = TA_Base_Bus::SubsystemState::State::SS_NORMAL;
// 		unsigned long subsystemStateEnd = TA_Base_Bus::SubsystemState::State::SS_DEGRADED;
// 
// 		//the following operations take too long times
//  		DASType::DbKey actionGroup=DASConst::NO_ACTION_GROUP;
//  		bool isControl=false;
//  
//  		for ( subIter = m_subsystemMap.begin(); subIter != m_subsystemMap.end(); ++subIter )
//  		{
//  			for ( subsystemStateIter = 0; subsystemStateIter <= 2; ++subsystemStateIter)
//  			{
//  				for( proIter = m_profileMap.begin(); proIter != m_profileMap.end(); ++proIter )
//  				{
//  					proIter->second->getActionGroup( subIter->first,
//  						subsystemStateIter,
//  						actionGroup,
//  						isControl );
//  				}
//  			}
//  		}
	}

	DutyDbCacheImpl::~DutyDbCacheImpl()
	{
		{
			ProfileMap::iterator it;

			ACE_Guard<ACE_Thread_Mutex> guard(m_profileLock);
			for( it = m_profileMap.begin(); it != m_profileMap.end(); it++ )
			{
				delete it->second;
				it->second = NULL;
			}
			m_profileMap.clear();
		}
		{
			OperatorMap::iterator it;

			ACE_Guard<ACE_Thread_Mutex> guard(m_operatorLock);
			for( it = m_operatorMap.begin(); it != m_operatorMap.end(); it++ )
			{
				delete it->second;
				it->second = NULL;
			}
			m_operatorMap.clear();
		}
		{
			SubsystemMap::iterator it;

			ACE_Guard<ACE_Thread_Mutex> guard(m_subsystemLock);
			for( it = m_subsystemMap.begin(); it != m_subsystemMap.end(); it++ )
			{
				delete it->second;
				it->second = NULL;
			}
			m_subsystemMap.clear();
		}
		{
			LocationMap::iterator it;

			ACE_Guard<ACE_Thread_Mutex> guard(m_locationLock);
			for( it = m_locationMap.begin(); it != m_locationMap.end(); it++ )
			{
				delete it->second;
				it->second = NULL;
			}
			m_locationMap.clear();
		}
		{
			RegionMap::iterator it;

			ACE_Guard<ACE_Thread_Mutex> guard(m_regionLock);
			for( it = m_regionMap.begin(); it != m_regionMap.end(); it++ )
			{
				delete it->second;
				it->second = NULL;
			}
			m_regionMap.clear();
		}
		{
			EntityMap::iterator it;

			ACE_Guard<ACE_Thread_Mutex> guard(m_entityLock);
			for( it = m_entityMap.begin(); it != m_entityMap.end(); it++ )
			{
				delete it->second;
				it->second = NULL;
			}
			m_entityMap.clear();
		}

		if( NULL != m_updater )
		{
			delete m_updater;
			m_updater = NULL;
		}
	}

	void DutyDbCacheImpl::update( AgentModeSubjectImpl& modeUpdate )
	{
		if( RPARAM_CONTROL == modeUpdate.getMode() )
		{
			if( m_updater == NULL )
			{
				m_updater = new DutyDbCacheUpdater(*this);
			}
		}
		else
		{
			if( m_updater != NULL )
			{
				delete m_updater;
				m_updater = NULL;
			}
		}
	}


	void DutyDbCacheImpl::update( AgentTerminateSubjectImpl& terminateUpdate )
	{
		if( m_updater != NULL )
		{
			delete m_updater;
			m_updater = NULL;
		}

		SeObserver<AgentModeSubjectImpl>::detach();
		SeObserver<AgentTerminateSubjectImpl>::detach();
	}

	IDutyPolicy::PROFILE_TYPE DutyDbCacheImpl::getProfileType( const DASType::DbKey& profileKey )
	{
		FUNCTION_ENTRY("getProfileType");

		ACE_Guard<ACE_Thread_Mutex> guard(m_profileLock);

        // TD10468
        TA_Base_Core::IProfile* profile = NULL;
        try
        {
    		profile =  m_profileLookup.lookup( profileKey );
        }
        catch (TA_Base_Core::TransactiveException&)
        {
            // if it's a deleted profile, we still need to get the required data from database
            // because it might be a deleted profile with an active session
            unsigned long profileType = TA_Base_Core::ProfileAccessFactory::getInstance().getDeletedRawProfileType(profileKey);
            if ( profileType != TA_Base_Core::ProfileHelper::LOCAL_PROFILE_TYPE )
            {
                FUNCTION_EXIT;
			    return( IDutyPolicy::PT_CENTRAL );
            }
            FUNCTION_EXIT;
            return( IDutyPolicy::PT_LOCAL );
        }

		if( 0 == profile )
		{
			std::ostringstream msg;
			msg << "No profile found matching key " << profileKey;

			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
		}

		if( profile->isExclusive() )
		{
			FUNCTION_EXIT;
			return( IDutyPolicy::PT_CENTRAL );
		}

		FUNCTION_EXIT;
		return( IDutyPolicy::PT_LOCAL );
	}

	DASType::DbKey DutyDbCacheImpl::getRawProfileType( const DASType::DbKey& profileKey )
	{
		FUNCTION_ENTRY("getRawProfileType");

		ACE_Guard<ACE_Thread_Mutex> guard(m_profileLock);

        // TD10468
        TA_Base_Core::IProfile* profile = NULL;
        try
        {
    		profile =  m_profileLookup.lookup( profileKey );
        }
        catch (TA_Base_Core::TransactiveException&)
        {
            // if it's a deleted profile, we still need to get the required data from database
            // because it might be a deleted profile with an active session
            unsigned long profileType = TA_Base_Core::ProfileAccessFactory::getInstance().getDeletedRawProfileType(profileKey);
            FUNCTION_EXIT;
            return profileType;
        }

		if( 0 == profile )
		{
			std::ostringstream msg;
			msg << "No profile found matching key " << profileKey;

			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
		}

		FUNCTION_EXIT;
		return( profile->getType() );
	}

	std::string DutyDbCacheImpl::getProfileTypeName( const DASType::DbKey& profileKey )
	{
		if( IDutyPolicy::PT_CENTRAL == getProfileType(profileKey) )
		{
			return(DASConst::CENTRAL);
		}

		return(DASConst::LOCAL);
	}

	DASType::DbKey DutyDbCacheImpl::getProfileGroup( const DASType::DbKey& profileKey )
	{
		FUNCTION_ENTRY("getProfileGroup");

		ACE_Guard<ACE_Thread_Mutex> guard(m_profileLock);

        // TD10468
        TA_Base_Core::IProfile* profile = NULL;
        try
        {
    		profile =  m_profileLookup.lookup( profileKey );
        }
        catch (TA_Base_Core::TransactiveException&)
        {
            // if it's a deleted profile, we still need to get the required data from database
            // because it might be a deleted profile with an active session
            unsigned long profileGroup = TA_Base_Core::ProfileAccessFactory::getInstance().getDeletedProfileGroup(profileKey);
            FUNCTION_EXIT;
            return profileGroup;
        }

		if( 0 == profile )
		{
			std::ostringstream msg;
			msg << "No profile found matching key " << profileKey;

			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
		}

		FUNCTION_EXIT;

		return( profile->getAccessControlGroup() );
	}
	
	std::string DutyDbCacheImpl::getProfileName( const DASType::DbKey& profileKey )
	{
		FUNCTION_ENTRY("getProfileName");

		ACE_Guard<ACE_Thread_Mutex> guard(m_profileLock);

        // TD10468
        TA_Base_Core::IProfile* profile = NULL;
        try
        {
    		profile =  m_profileLookup.lookup( profileKey );
        }
        catch (TA_Base_Core::TransactiveException&)
        {
            // if it's a deleted profile, we still need to get the required data from database
            // because it might be a deleted profile with an active session
            std::string name = TA_Base_Core::ProfileAccessFactory::getInstance().getDeletedProfileName(profileKey);
            FUNCTION_EXIT;
			return name;
        }

		if( 0 == profile )
		{
			std::ostringstream msg;
			msg << "No profile found matching key " << profileKey;

			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
		}

		FUNCTION_EXIT;

		return( profile->getName() );
	}


	// Note this is very inefficient code if it is called more than a few times. This is currently
	// not the case. However, if that does become the case then a "getAssociatedRegionKeys" should
	// be added to IOperator and the keys should be cached by OperatorHelper.
	void DutyDbCacheImpl::getLocationRegions( const DASType::DbKey& location, DASType::RegionSet& regions )
	{
		std::vector<TA_Base_Core::IRegion*> regionsVector;
		regionsVector = TA_Base_Core::RegionAccessFactory::getInstance().getRegionsByLocation(location);
		
		std::vector<TA_Base_Core::IRegion*>::const_iterator it;

		for( it=regionsVector.begin(); it!=regionsVector.end(); it++ )
		{
			regions.insert((*it)->getKey() );
			delete *it;
		}
	}

	void DutyDbCacheImpl::getAllLocationKeys( DASType::LocationSet& locations )
	{
		FUNCTION_ENTRY("getAllLocationKeys");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
			"begin DutyDbCacheImpl::getAllLocationKeys(), location map size = %d", m_locationMap.size());

		ACE_Guard<ACE_Thread_Mutex> guard(m_locationLock);
		
		//m_locationLookup.set( TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations() );

		LocationMap::iterator locIt;

		bool foundUnallocated = false;

		for( locIt = m_locationMap.begin(); locIt != m_locationMap.end(); locIt++ )
		{
			if( !foundUnallocated )
			{
				if( (locIt->second != NULL) && (UNALLOCATED_LOCATION_NAME == locIt->second->getName()) )
				{
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
						"begin DutyDbCacheImpl::getAllLocationKeys(), location pointer is not NULL");

					foundUnallocated = true;
					continue;
				}
				else
				{
					if ( locIt->second == NULL)
					{
						LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
							"begin DutyDbCacheImpl::getAllLocationKeys(), location pointer is NULL");
					}
				}
			}

			locations.insert( locIt->first );
		}

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyDbCacheImpl::getAllLocationKeys()");

		FUNCTION_EXIT;
	}

	bool DutyDbCacheImpl::locationIsOcc( const std::string& locationName )
	{
		LocationMap::iterator locIt = m_locationMap.begin();
		bool isOcc = false;

		while( (!isOcc) && (locIt != m_locationMap.end()) )
		{
			if( (locationName == locIt->second->getName()) &&
				(TA_Base_Core::ILocation::OCC == locIt->second->getLocationType()) )
			{
				isOcc = true;
			}
			locIt++;
		}

		return( isOcc );
	}

	void DutyDbCacheImpl::getAllSubsystemKeys( const DASType::DbKey& regionKey, DASType::SubsystemSet& subsystems )
	{
		FUNCTION_ENTRY("getAllSubsystemKeys");

		std::vector<TA_Base_Core::ISubsystem*> regionSubs =
			TA_Base_Core::SubsystemAccessFactory::getInstance().getSubsystemsByRegion(regionKey);

		std::vector<TA_Base_Core::ISubsystem*>::iterator subIt;

		bool foundUnallocated = false;

		for( subIt = regionSubs.begin(); subIt != regionSubs.end(); subIt++ )
		{
			if( !foundUnallocated )
			{
				if( UNALLOCATED_SUBSYSTEM_NAME == (*subIt)->getName() )
				{
					foundUnallocated = true;
					delete *subIt;
					*subIt = NULL;
					continue;
				}
			}

			subsystems.insert( (*subIt)->getKey() );

			// This is wasteful but there is currently no way to
			// simply get a list of keys.
			delete *subIt;
			*subIt = NULL;
		}

		FUNCTION_EXIT;
	}

	std::string DutyDbCacheImpl::getRegionName( const DASType::DbKey& regionKey )
	{
		FUNCTION_ENTRY("getRegionName");

		ACE_Guard<ACE_Thread_Mutex> guard(m_regionLock);

		TA_Base_Core::IRegion* region =  m_regionLookup.lookup( regionKey );

		if( 0 == region )
		{
			std::ostringstream msg;
			msg << "No region found matching key " << regionKey;

			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
		}

		FUNCTION_EXIT;

		return( region->getName() );
	}

	std::string DutyDbCacheImpl::getRegionDisplayName( const DASType::DbKey& regionKey )
	{
		FUNCTION_ENTRY("getRegionName");

		ACE_Guard<ACE_Thread_Mutex> guard(m_regionLock);

		TA_Base_Core::IRegion* region =  m_regionLookup.lookup( regionKey );

		if( 0 == region )
		{
			std::ostringstream msg;
			msg << "No region found matching key " << regionKey;

			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
		}

		FUNCTION_EXIT;

		return( region->getDisplayName() );
	}
		
	std::string DutyDbCacheImpl::getLocationName(const DASType::DbKey&  locationKey)
	{
		FUNCTION_ENTRY("getLocationName");
		
		ACE_Guard<ACE_Thread_Mutex> guard(m_locationLock);
		
		TA_Base_Core::ILocation* location =  m_locationLookup.lookup( locationKey );
		
		if( 0 == location )
		{
			std::ostringstream msg;
			msg << "No location found matching key " << locationKey;
			
			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
		}
		
		FUNCTION_EXIT;
		
		return( location->getName() );
	}

	std::string DutyDbCacheImpl::getLocationDisplayName(const DASType::DbKey&  locationKey)
	{
		FUNCTION_ENTRY("getLocationDisplayName");

		ACE_Guard<ACE_Thread_Mutex> guard(m_locationLock);

		TA_Base_Core::ILocation* location =  m_locationLookup.lookup( locationKey );

		if( 0 == location )
		{
			std::ostringstream msg;
			msg << "No location found matching key " << locationKey;

			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
		}

		FUNCTION_EXIT;

		return( location->getDisplayName() );
	}

	void DutyDbCacheImpl::buildRegionMap( DASType::ReverseRegionMap& regionMap )
	{
		FUNCTION_ENTRY("buildRegionMap");

		ACE_Guard<ACE_Thread_Mutex> guard(m_locationLock);

		LocationMap::iterator locIt;

		for( locIt = m_locationMap.begin(); locIt != m_locationMap.end(); locIt++ )
		{
			DASType::RegionSet regions;
			getLocationRegions( locIt->first, regions );

			DASType::RegionSet::const_iterator regIt;

			for( regIt=regions.begin(); regIt!=regions.end(); regIt++ )
			{
				regionMap.insert( DASType::ReverseRegionMap::value_type(*regIt, locIt->first) );
			}
		}

		FUNCTION_EXIT;
	}

	void DutyDbCacheImpl::getAllRegionKeys( DASType::RegionSet& regions )
	{
		FUNCTION_ENTRY("getAllRegionKeys");

		ACE_Guard<ACE_Thread_Mutex> guard(m_regionLock);
		
		//m_regionLookup.set( TA_Base_Core::RegionAccessFactory::getInstance().getAllRegions() );

		RegionMap::iterator regIt;

		bool foundUnallocated = false;

		for( regIt = m_regionMap.begin(); regIt != m_regionMap.end(); regIt++ )
		{
			if( !foundUnallocated )
			{
				if( UNALLOCATED_REGION_NAME == regIt->second->getName() )
				{
					foundUnallocated = true;
					continue;
				}
			}

			regions.insert( regIt->first );
		}

		FUNCTION_EXIT;
	}

	IDutyPolicy::SUBSYSTEM_TYPE DutyDbCacheImpl::getSubsystemType( const DASType::DbKey& subsystemKey )
	{
		FUNCTION_ENTRY("getSubsystemType");

		ACE_Guard<ACE_Thread_Mutex> guard(m_subsystemLock);

		TA_Base_Core::ISubsystem* subsystem =  m_subsystemLookup.lookup( subsystemKey );

		if( 0 == subsystem )
		{
			std::ostringstream msg;
			msg << "No subsystem found matching key " << subsystemKey;

			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
		}

		TA_Base_Bus::IDutyPolicy::SUBSYSTEM_TYPE rval(TA_Base_Bus::IDutyPolicy::ST_UNDEFINED);

		if( subsystem->isExclusive() )
		{
			rval = TA_Base_Bus::IDutyPolicy::ST_EXCLUSIVE;
		}
		else
		{
			rval = TA_Base_Bus::IDutyPolicy::ST_ADMISSIVE;
		}

		FUNCTION_EXIT;

		return( rval );
	}

	std::string DutyDbCacheImpl::getSubsystemName( const DASType::DbKey& subsystemKey )
	{
		FUNCTION_ENTRY("getSubsystemName");

		ACE_Guard<ACE_Thread_Mutex> guard(m_subsystemLock);

		TA_Base_Core::ISubsystem* subsystem =  m_subsystemLookup.lookup( subsystemKey );

		if( 0 == subsystem )
		{
			std::ostringstream msg;
			msg << "No subsystem found matching key " << subsystemKey;

			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
		}

		FUNCTION_EXIT;

		return( subsystem->getName() );
	}

	IDutyPolicy::RIGHTS_TYPE DutyDbCacheImpl::getRightsType( const DASType::DbKey& profileKey,
			const DASType::DbKey& subsystemKey, const SubsystemState::State& subState )
	{
		FUNCTION_ENTRY("getRightsType");

//		ACE_Guard<ACE_Thread_Mutex> guard(m_profileLock);

//         // TD10468
//         TA_Base_Core::IProfile* profile = NULL;
//         try
//         {
//     		profile =  m_profileLookup.lookup( profileKey );
//         }
//         catch (TA_Base_Core::TransactiveException&)
//         {
//             // if it's a deleted profile, we still need to get the required data from database
//             // because it might be a deleted profile with an active session
//             TA_Base_Core::ProfileAccessFactory::getInstance().getDeletedRawProfileType(profileKey);
//             // this profile is deleted
//             std::ostringstream msg;
//             msg << "profile: " << profileKey << " has been deleted!";
// 			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
//         }
// 
// 		if( 0 == profile )
// 		{
// 			std::ostringstream msg;
// 			msg << "No profile found matching key " << profileKey;
// 
// 			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
// 		}

		unsigned long actionGroupKey = this->getRights(profileKey,subsystemKey,subState);

		if(actionGroupKey == DASConst::NO_ACTION_GROUP ) return IDutyPolicy::RT_NONE;

		bool isControl  = false;
	


		ACE_Guard<ACE_Thread_Mutex> guard(m_actionGroupLock);

		TA_Base_Core::IActionGroup* pActionGroup = m_actionGroupLookup.lookup(actionGroupKey);

		if( pActionGroup != 0 )
		{
			isControl = pActionGroup->isControlType();

			if( isControl )
			{
				FUNCTION_EXIT;
				return( IDutyPolicy::RT_EXCLUSIVE );
			}
			else
			{
				FUNCTION_EXIT;
				return( IDutyPolicy::RT_ADMISSIVE );
			}
		}
		else
		{
			FUNCTION_EXIT;
			return( IDutyPolicy::RT_NONE );
		}

	}
	
	std::string DutyDbCacheImpl::getRightsName( const DASType::DbKey& profileKey,
		const DASType::DbKey& subsystemKey, const SubsystemState::State& subState )
	{
		FUNCTION_ENTRY("getRightsName");

// 		ACE_Guard<ACE_Thread_Mutex> guard(m_profileLock);
// 
//         // TD10468
//         TA_Base_Core::IProfile* profile = NULL;
//         try
//         {
//     		profile =  m_profileLookup.lookup( profileKey );
//         }
//         catch (TA_Base_Core::TransactiveException&)
//         {
//             // if it's a deleted profile, we still need to get the required data from database
//             // because it might be a deleted profile with an active session
//             TA_Base_Core::ProfileAccessFactory::getInstance().getDeletedRawProfileType(profileKey);
//             // this profile is deleted
// 			std::ostringstream msg;
// 			msg << "profile: " << profileKey << " has been deleted!";
// 
// 			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
//         }
// 
// 		if( 0 == profile )
// 		{
// 			std::ostringstream msg;
// 			msg << "No profile found matching key " << profileKey;
// 
// 			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
// 		}

		unsigned long actionGroupKey = this->getRights(profileKey, subsystemKey, subState);

		if( actionGroupKey ==DASConst::NO_ACTION_GROUP ) return "";

		ACE_Guard<ACE_Thread_Mutex> guard(m_actionGroupLock);
		TA_Base_Core::IActionGroup* pActionGroup = m_actionGroupLookup.lookup(actionGroupKey);

		if(pActionGroup == NULL) return "";

		FUNCTION_EXIT;
		return pActionGroup->getName();
	}

	DASType::DbKey DutyDbCacheImpl::getRights( const DASType::DbKey& profileKey,
			const DASType::DbKey& subsystemKey, const SubsystemState::State& subState )
	{
		FUNCTION_ENTRY("getRights");

// 		ACE_Guard<ACE_Thread_Mutex> guard(m_profileLock);
// 
// 		TA_Base_Core::IProfile* profile =  m_profileLookup.lookup( profileKey );
// 
// 		if( 0 == profile )
// 		{
// 			std::ostringstream msg;
// 			msg << "No profile found matching key " << profileKey;
// 
// 			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
// 		}
// 
// 		DASType::DbKey actionGroup=DASConst::NO_ACTION_GROUP;
// 		bool isControl=false;
// 
// 		FUNCTION_EXIT;
// 
// 		if( profile->getActionGroup(subsystemKey, subState, actionGroup, isControl) )
// 		{
// 			return(actionGroup);
// 		}
// 		else
// 		{
// 			return(DASConst::NO_ACTION_GROUP);
// 		}
		ACE_Guard<ACE_Thread_Mutex> guard(m_rightsLock);
		RightsBook::iterator itr = m_rightsBook.find(RightsPK(profileKey, subsystemKey,subState));
		if(itr != m_rightsBook.end())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "PK: %lu, SK:%lu, SSK:%lu, AGK:%lu",
				profileKey,subsystemKey,subState,itr->second);
			return itr->second;
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "PK: %lu, SK:%lu, SSK:%lu, AGK: NO_ACTION_GROUP",
				profileKey,subsystemKey,subState);
		return DASConst::NO_ACTION_GROUP;
	}

	bool DutyDbCacheImpl::dutyMayChange(const DASType::DbKey& profileKey, const DASType::DbKey& subsystemKey, 
		const SubsystemState::State& preState, const SubsystemState::State& postState )
	{
		const DASType::DbKey preRights  = getRights( profileKey, subsystemKey, preState  );
		const DASType::DbKey postRights = getRights( profileKey, subsystemKey, postState );

		//TD15346++
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"TD15346: Inside dutyMayChange(): PK:%lu, SK:%lu, CS:%lu, NS:%lu, CR:%lu, NR:%lu", 
			profileKey, subsystemKey, preState, postState, preRights, postRights );

		// IF the pre-transfer rights are not equal to the post-transfer rights THEN
		// duty may change
		//return( preRights != postRights );
		
		if ( preRights != postRights )
		{
			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: Inside dutyMayChange(): preRights != postRights RETURN TRUE");
			//++TD15346
			return true;
		}
		else if ((preState == SubsystemState::SS_DEGRADED) && (postState == SubsystemState::SS_NORMAL))
		{
			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: Inside dutyMayChange(): preState == SS_DEGRADED AND postState == SS_DEGRADED RETURN TRUE");
			//++TD15346
			return true;
		}
		else if ((preState == SubsystemState::SS_DELEGATED) && (postState == SubsystemState::SS_NORMAL))
		{
			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: Inside dutyMayChange(): preState == SS_DELEGATED AND postState == SS_DEGRADED RETURN TRUE");
			//++TD15346
			return true;
		}
		else if (postRights == DASConst::NO_ACTION_GROUP)
		{
			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: Inside dutyMayChange(): postRights == NO_ACTION_GROUP RETURN FALSE");
			//++TD15346
			return false;
		}
		else if (preState != postState)
		{
			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: dutyMayChange(): preState != postState RETURN TRUE");
			//++TD15346
			return true;
		}
		else
		{
			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: Inside dutyMayChange(): other case RETURN false");
			//++TD15346
			return false;
		}
		//++TD15346
	}
			
	std::string  DutyDbCacheImpl::getOperatorName( const DASType::DbKey& operatorKey )
	{
		FUNCTION_ENTRY("getOperatorName");

		ACE_Guard<ACE_Thread_Mutex> guard(m_operatorLock);

		TA_Base_Core::IOperator* op =  m_operatorLookup.lookup( operatorKey );

		if( 0 == op )
		{
			std::ostringstream msg;
			msg << "No operator found matching key " << operatorKey;

			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
		}

		FUNCTION_EXIT;

		return( op->getName() );
	}

	void DutyDbCacheImpl::getOperatorRegions( const DASType::DbKey& operatorKey, DASType::RegionSet& regions )
	{
		std::vector<unsigned long> regionsVector;

		ACE_Guard<ACE_Thread_Mutex> guard(m_operatorLock);

		TA_Base_Core::IOperator* op =  m_operatorLookup.lookup( operatorKey );

		if( 0 == op )
		{
			std::ostringstream msg;
			msg << "No operator found matching key " << operatorKey;

			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
		}

		regionsVector = op->getAssociatedRegionKeys();
		
		if( regionsVector.size() == 0 )
		{
			regions.insert( DASConst::NO_REGION );
			return;
		}

		std::vector<unsigned long>::const_iterator it;

		for( it=regionsVector.begin(); it!=regionsVector.end(); it++ )
		{
			regions.insert(*it);
		}
	}

	ACE_Time_Value DutyDbCacheImpl::getPopupTimeout()
	{
		FUNCTION_ENTRY("getPopupTimeout");

		ACE_Guard<ACE_Thread_Mutex> guard(m_entityLock);

		TA_Base_Core::ControlStation* cs = dynamic_cast<TA_Base_Core::ControlStation*>
			(m_entityLookup.lookup(m_controlStationKey));

		if( 0 == cs )
		{
			std::ostringstream msg;
			msg << "No Control Station found matching key " << m_controlStationKey;

			TA_THROW(TA_Base_Core::DutyAgentException(msg.str()));
		}

		ACE_Time_Value rval(cs->getAccessControlPopupTimeout(), 0);

		FUNCTION_EXIT;

		return( rval );
	}
}
