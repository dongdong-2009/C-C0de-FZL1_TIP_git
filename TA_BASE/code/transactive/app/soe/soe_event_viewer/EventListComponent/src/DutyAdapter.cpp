/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/EventListComponent/src/DutyAdapter.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2009/12/03 15:46:08 $
  * Last modified by:  $Author: huirong.luo $
  *
  * This class provides an interface between the local duty agent and the DAI
  * responsible for obtaining events based on the provided filter.
  */

#include <algorithm>

#include "app/event/event_viewer/eventlistcomponent/src/DutyAdapter.h"

#include "core/data_access_interface/entity_access/src/DutyAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DutyAgentException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/ApplicationException.h"


using TA_Base_Core::DebugUtil;

DutyAdapter::DutyAdapter()
{
    // The location key must be present.
    std::string locationKeyStr = TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY );
    TA_ASSERT( 0 < locationKeyStr.size(), "Location key must be specified!" );

    // Convert the location key to a number.
    unsigned long locationKey;
    std::stringstream strToUl( locationKeyStr );
    strToUl >> locationKey;

    // Get the CorbaName for the duty agent at the current location.
    TA_Base_Core::CorbaNameList corbaNames = TA_Base_Core::EntityAccessFactory::getInstance()
        .getCorbaNamesOfTypeAtLocation( TA_Base_Core::DutyAgentEntityData::getStaticType(), locationKey,true);

    // There should be only one duty agent at each location.
    TA_ASSERT( 1 == corbaNames.size(), ("Need exactly one duty agent configured for this location ("+locationKeyStr+")").c_str() );

    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Using duty agent: %s.%s", corbaNames[0].getAgentName().c_str(), corbaNames[0].getObjectName().c_str() );

    // Initialise our DutyAgent object.
    m_dutyAgent.setCorbaName( corbaNames[0] );
}


DutyAdapter::~DutyAdapter()
{
    // Nothing to do.
}

TA_Base_Core::CombinedEventAccessFactory::FullDutyMap DutyAdapter::getFullDutyMap( const KeyList& locationKeys, const KeyList& physicalSubsystemKeys, const KeyList& applicationSubsystemKeys )
{
    FUNCTION_ENTRY("getDutyMap");

    DutyMap dutyMap;

	TA_Base_Core::CombinedEventAccessFactory::FullDutyMap finalDutyMap;

	finalDutyMap.clear();

	if( physicalSubsystemKeys.empty() && !applicationSubsystemKeys.empty())
	{
		return finalDutyMap;
	}


    // Make sure the provided map is in a known state.
    dutyMap.clear();

    try
    {
        // Get the current duty from the agent.
		// The variables for holding the duty information.
		TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence_var subsystemStateSeq;
		TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence_var subsystemDutySeq;
		TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence_var regionDutySeq;

		// Get the current session ID.
		std::string sessionId( TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ) );

		try
        {
		// Query the duty agent.
		CORBA_CALL( m_dutyAgent,
		            mgrGetDutyState,
		            ( sessionId.c_str(), subsystemStateSeq.out(), subsystemDutySeq.out(), regionDutySeq.out() ) );
            LOG_GENERIC( SourceInfo,DebugUtil::DebugTrace,"build up duty map finished" );
        }
        catch (...) 
        {
	        std::string reason( "An unknown exception was caught while DutyAdapter::queryDutyAgent()." );
	        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s", reason.c_str() );
	        throw TA_Base_Core::DutyAgentException( reason );
        }



		// Now iterate through all of the results and pull out the ones for our session.
		DutyMap actualDuty;
		unsigned int i = 0;
        for ( i=0; i<subsystemDutySeq->length(); i++ )
		{
			if ( sessionId == std::string( subsystemDutySeq[i].session ) )
			{
				DutyMap::iterator regionIt = actualDuty.find( subsystemDutySeq[i].regionKey );
				if ( regionIt == actualDuty.end() )
				{
				// Create an entry for this region.
				std::pair<DutyMap::iterator,bool> result = actualDuty.insert(
				DutyMap::value_type( subsystemDutySeq[i].regionKey, KeyList() ) );

				// Take note of the iterator.
				regionIt = result.first;
				}

				// Now add the subsystem in the correct position.
				KeyList::iterator subsystemInsertPt = std::find_if( regionIt->second.begin(), regionIt->second.end(),
				std::bind2nd( std::greater<unsigned long>(), subsystemDutySeq[i].subsystemKey ) );
				regionIt->second.insert( subsystemInsertPt, subsystemDutySeq[i].subsystemKey );
			}
		}

        if ( !locationKeys.empty() )
        {
            // The user specified a list of locations, so check the duty for each.
            KeyList::const_iterator locationIt;
            for ( locationIt=locationKeys.begin(); locationIt!=locationKeys.end(); locationIt++ )
            {
                DutyMap::iterator dutyIt = actualDuty.find( *locationIt );
                if ( dutyIt != actualDuty.end() )
                {
                    // The user has some duty at this location so check the subsystems.
                    KeyList subsystems = createSubsystemList( physicalSubsystemKeys, dutyIt->second );
                    if ( !subsystems.empty() )
                    {
                        dutyMap.insert( DutyMap::value_type( *locationIt, subsystems ) );
                    }
                }
            }
        }
        else
        {
            // The user wants to see all locations, so limit based on duty.
            DutyMap::iterator dutyIt;
            for ( dutyIt=actualDuty.begin(); dutyIt!=actualDuty.end(); dutyIt++ )
            {
                KeyList subsystems = createSubsystemList( physicalSubsystemKeys, dutyIt->second );
                if ( !subsystems.empty() )
                {
                    dutyMap.insert( DutyMap::value_type( dutyIt->first, subsystems ) );
                }
            }
        }

        // Dump debug information regarding the duty.

        // List what the user wants.
        std::stringstream wanted;
        wanted << "Locations: ";
        KeyList::const_iterator locIt;
        for ( locIt=locationKeys.begin(); locIt!=locationKeys.end(); locIt++ )
        {
            wanted << *locIt << " ";
        }
        wanted << ", Subsystems: ";
        KeyList::const_iterator ssIt;
        for ( ssIt=physicalSubsystemKeys.begin(); ssIt!=physicalSubsystemKeys.end(); ssIt++ )
        {
            wanted << *ssIt << " ";
        }

        // List what duty the user has.
        std::stringstream duty;
        DutyMap::iterator dutyIt;
        for ( dutyIt=actualDuty.begin(); dutyIt!=actualDuty.end(); dutyIt++ )
        {
            duty << "Location: " << dutyIt->first << ", Subsystems: ";
            for ( ssIt=dutyIt->second.begin(); ssIt!=dutyIt->second.end(); ssIt++ )
            {
                duty << *ssIt << " ";
            }
            duty << std::endl;
        }

        // List what the user will get.
        std::stringstream got;
        for ( dutyIt=dutyMap.begin(); dutyIt!=dutyMap.end(); dutyIt++ )
        {
            got << "Location: " << dutyIt->first << ", Subsystems: ";
            for ( ssIt=dutyIt->second.begin(); ssIt!=dutyIt->second.end(); ssIt++ )
            {
                got << *ssIt << " ";
            }
            got << std::endl;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Wanted locations and subsystems:\n%s", wanted.str().c_str() );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Duty:\n%s", duty.str().c_str() );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Intersection:\n%s", got.str().c_str() );

		// add the subsystem state information to the duty map
		for( i=0; i<subsystemStateSeq->length(); i++)
		{
			DutyMap::iterator regionIt = dutyMap.find( subsystemStateSeq[i].regionKey );
			if( regionIt != dutyMap.end())
			{
				ssIt=regionIt->second.begin(); 
				while( ssIt!=regionIt->second.end() && *ssIt!=subsystemStateSeq[i].subsystemKey)
				{
					ssIt++;
				}
				if( ssIt!=regionIt->second.end() )
				{
					TA_Base_Core::CombinedEventAccessFactory::FullDutyMap::iterator regIt = finalDutyMap.find( subsystemStateSeq[i].regionKey );
					TA_Base_Core::CombinedEventAccessFactory::PhysicalSubsystemInfo physicalSubsysInfo;
					physicalSubsysInfo.physicalSubsystemId = subsystemStateSeq[i].subsystemKey;
					physicalSubsysInfo.physicalSubsystemState = subsystemStateSeq[i].currState;
					if( regIt == finalDutyMap.end())
					{
						TA_Base_Core::CombinedEventAccessFactory::SubSysList subSysList;
						subSysList.push_back(physicalSubsysInfo);
						
						// Create an entry for this location						
						finalDutyMap.insert( TA_Base_Core::CombinedEventAccessFactory::FullDutyMap::value_type( subsystemStateSeq[i].regionKey, subSysList ) );
					}
					else{
						regIt->second.push_back(physicalSubsysInfo);
					}					
				}
			}
		}
    }
    catch ( const TA_Base_Core::AccessDeniedException& ex )
    {
        std::string reason( "AccessDeniedException: "+std::string(ex.reason.in()) );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch ( const TA_Base_Core::BadParameterException& ex )
    {
        std::string reason( "BadParameterException: "+std::string(ex.reason.in()) );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch ( const TA_Base_Core::DatabaseErrorException& ex )
    {
        std::string reason( "DatabaseErrorException: "+std::string(ex.reason.in()) );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch ( const TA_Base_Core::OperationModeException& ex )
    {
        std::string reason( "OperationModeException: "+std::string(ex.reason.in()) );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch ( TA_Base_Core::ObjectResolutionException& ex )
    {
        std::string reason( "ObjectResolutionException: "+std::string(ex.what()) );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::string reason( "TransactiveException: "+std::string(ex.what()) );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch ( TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException& )
    {
        std::string reason( "Unhandled exception in duty agent." );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch (const CORBA::Exception& ex)
    {
        std::string reason( "CORBA::Exception: "+TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch ( ... )
    {
        std::string reason( "An unknown exception was caught while determining duty." );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }


	std::stringstream finalgot;
	TA_Base_Core::CombinedEventAccessFactory::FullDutyMap::iterator finalIt;
	TA_Base_Core::CombinedEventAccessFactory::SubSysList::iterator  finalSubSysIt;
	for ( finalIt=finalDutyMap.begin(); finalIt!=finalDutyMap.end(); finalIt++ )
	{
		finalgot << "Location: " << finalIt->first << ", Subsystems: ";
        for ( finalSubSysIt=finalIt->second.begin(); finalSubSysIt!=finalIt->second.end(); finalSubSysIt++ )
        {
            finalgot << (*finalSubSysIt).physicalSubsystemId << " ";
        }
        finalgot << std::endl;
    }

	LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Final Intersection:\n%s", finalgot.str().c_str() );
    FUNCTION_EXIT;
    return finalDutyMap;
}
DutyAdapter::DutyMap DutyAdapter::getDutyMap( const KeyList& locationKeys, const KeyList& physicalSubsystemKeys )
{
    FUNCTION_ENTRY("getDutyMap");

    DutyMap dutyMap;

    // Make sure the provided map is in a known state.
    dutyMap.clear();

    try
    {
        // Get the current duty from the agent.
        DutyMap actualDuty = queryDutyAgent();

        if ( !locationKeys.empty() )
        {
            // The user specified a list of locations, so check the duty for each.
            KeyList::const_iterator locationIt;
            for ( locationIt=locationKeys.begin(); locationIt!=locationKeys.end(); locationIt++ )
            {
                DutyMap::iterator dutyIt = actualDuty.find( *locationIt );
                if ( dutyIt != actualDuty.end() )
                {
                    // The user has some duty at this location so check the subsystems.
                    KeyList subsystems = createSubsystemList( physicalSubsystemKeys, dutyIt->second );
                    if ( !subsystems.empty() )
                    {
                        dutyMap.insert( DutyMap::value_type( *locationIt, subsystems ) );
                    }
                }
            }
        }
        else
        {
            // The user wants to see all locations, so limit based on duty.
            DutyMap::iterator dutyIt;
            for ( dutyIt=actualDuty.begin(); dutyIt!=actualDuty.end(); dutyIt++ )
            {
                KeyList subsystems = createSubsystemList( physicalSubsystemKeys, dutyIt->second );
                if ( !subsystems.empty() )
                {
                    dutyMap.insert( DutyMap::value_type( dutyIt->first, subsystems ) );
                }
            }
        }

        // Dump debug information regarding the duty.

        // List what the user wants.
        std::stringstream wanted;
        wanted << "Locations: ";
        KeyList::const_iterator locIt;
        for ( locIt=locationKeys.begin(); locIt!=locationKeys.end(); locIt++ )
        {
            wanted << *locIt << " ";
        }
        wanted << ", Subsystems: ";
        KeyList::const_iterator ssIt;
        for ( ssIt=physicalSubsystemKeys.begin(); ssIt!=physicalSubsystemKeys.end(); ssIt++ )
        {
            wanted << *ssIt << " ";
        }

        // List what duty the user has.
        std::stringstream duty;
        DutyMap::iterator dutyIt;
        for ( dutyIt=actualDuty.begin(); dutyIt!=actualDuty.end(); dutyIt++ )
        {
            duty << "Location: " << dutyIt->first << ", Subsystems: ";
            for ( ssIt=dutyIt->second.begin(); ssIt!=dutyIt->second.end(); ssIt++ )
            {
                duty << *ssIt << " ";
            }
            duty << std::endl;
        }

        // List what the user will get.
        std::stringstream got;
        for ( dutyIt=dutyMap.begin(); dutyIt!=dutyMap.end(); dutyIt++ )
        {
            got << "Location: " << dutyIt->first << ", Subsystems: ";
            for ( ssIt=dutyIt->second.begin(); ssIt!=dutyIt->second.end(); ssIt++ )
            {
                got << *ssIt << " ";
            }
            got << std::endl;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Wanted locations and subsystems:\n%s", wanted.str().c_str() );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Duty:\n%s", duty.str().c_str() );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Intersection:\n%s", got.str().c_str() );
    }
    catch ( const TA_Base_Core::AccessDeniedException& ex )
    {
        std::string reason( "AccessDeniedException: "+std::string(ex.reason.in()) );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch ( const TA_Base_Core::BadParameterException& ex )
    {
        std::string reason( "BadParameterException: "+std::string(ex.reason.in()) );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch ( const TA_Base_Core::DatabaseErrorException& ex )
    {
        std::string reason( "DatabaseErrorException: "+std::string(ex.reason.in()) );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch ( const TA_Base_Core::OperationModeException& ex )
    {
        std::string reason( "OperationModeException: "+std::string(ex.reason.in()) );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch ( TA_Base_Core::ObjectResolutionException& ex )
    {
        std::string reason( "ObjectResolutionException: "+std::string(ex.what()) );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::string reason( "TransactiveException: "+std::string(ex.what()) );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch ( TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException& )
    {
        std::string reason( "Unhandled exception in duty agent." );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch (const CORBA::Exception& ex)
    {
        std::string reason( "CORBA::Exception: "+TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    catch ( ... )
    {
        std::string reason( "An unknown exception was caught while determining duty." );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }


    FUNCTION_EXIT;
    return dutyMap;
}


DutyAdapter::DutyMap DutyAdapter::queryDutyAgent()
{
    FUNCTION_ENTRY("queryDutyAgent");

    // The variables for holding the duty information.
	TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence_var subsystemStateSeq;
	TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence_var subsystemDutySeq;
	TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence_var regionDutySeq;

	// Get the current session ID.
	std::string sessionId( TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ) );

		try
		{
			// Query the duty agent.
	        CORBA_CALL( m_dutyAgent,
	            mgrGetDutyState,
	            ( sessionId.c_str(), subsystemStateSeq.out(), subsystemDutySeq.out(), regionDutySeq.out() ) );
    }
    catch (...) 
    {
        std::string reason( "An unknown exception was caught while DutyAdapter::queryDutyAgent()." );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s", reason.c_str() );
        throw TA_Base_Core::DutyAgentException( reason );
    }
    
    // Now iterate through all of the results and pull out the ones for our session.
    DutyMap duty;
    for ( unsigned int i=0; i<subsystemDutySeq->length(); i++ )
    {
        if ( sessionId == std::string( subsystemDutySeq[i].session ) )
        {
            DutyMap::iterator regionIt = duty.find( subsystemDutySeq[i].regionKey );
            if ( regionIt == duty.end() )
            {
                // Create an entry for this region.
                std::pair<DutyMap::iterator,bool> result = duty.insert(
                    DutyMap::value_type( subsystemDutySeq[i].regionKey, KeyList() ) );

                // Take note of the iterator.
                regionIt = result.first;
            }

            // Now add the subsystem in the correct position.
            KeyList::iterator subsystemInsertPt = std::find_if( regionIt->second.begin(), regionIt->second.end(),
                std::bind2nd( std::greater<unsigned long>(), subsystemDutySeq[i].subsystemKey ) );
            regionIt->second.insert( subsystemInsertPt, subsystemDutySeq[i].subsystemKey );
        }
    }

    FUNCTION_EXIT;
    return duty;
}


DutyAdapter::KeyList DutyAdapter::createSubsystemList( const KeyList& physicalSubsystemKeys, const KeyList& subsystemDuty )
{
    FUNCTION_ENTRY("createSubsystemList");

    KeyList keyList;

    if ( 0 < physicalSubsystemKeys.size() )
    {
        // If the user wants a select group of subsystems only allow those
        // that also have duty.

        // Create an insert iterator for the new subsystem list.
        std::insert_iterator< KeyList > keyInsertIt( keyList, keyList.begin() );

        // Now find the intersection of the two subsystem lists.
        std::set_intersection( physicalSubsystemKeys.begin(), physicalSubsystemKeys.end(), subsystemDuty.begin(),
                               subsystemDuty.end(), keyInsertIt );
    }
    else
    {
        // If the user wants all subsystems, only allow those with duty.
        keyList = subsystemDuty;
    }

    FUNCTION_EXIT;
    return keyList;
}
