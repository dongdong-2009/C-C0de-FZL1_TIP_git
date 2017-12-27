/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/NSALocator.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/12/17 15:24:44 $
 * Last modified by:  $Author: shurui.liu $
 * 
 */
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "app/notification_service/src/NSALocator.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TA_String.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/NotificationAgentData.h"

// For LOCAL_LOCATION and ANY_LOCATION
#include "core/message/src/SubscriptionManager.h"

using TA_Base_Core::NotificationAgentData;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::ILocation;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::TransactiveException;

namespace TA_Base_App
{
    // The host:port that notification services at different locations run on is stored in 
    // the database, but in a rather indirect fashion, because the ENTITY table doesn't indicate
    // which host a given NotificationServiceAgent runs on, only the port (which is stored in
    // the ADDRESS field.  The SN_MANAGED_PROCESS and SN_SYSTEM_CONTROLLER tables must be queried 
    // to determine the hostnames.  The relationship between location, hostname and port is as 
    // follows (where 1-m is a one-to-many relationship, and 1-1 is a one-to-one relationship):
    //
    //          1   m            1   1                          1   m
    // LOCATION ----- NSA ENTITY ----- PARENT MONITORED PROCESS ----- SYSTEM CONTROLLER
    //                (port)                                          (hostname)
    //
    // Each location can have multiple NSA entities associated with it, each with its
    // own port number.
    // Each NSA entity can be running on multiple hosts (but with the same port number on
    // each host).

    // For example:
    // LOCATION 1:
    //  PKEY 101: NS_9991 can be running on callisto and lyra.  ie. (callisto, 9991), (lyra, 9991)
    //  PKEY 102: NS_9992 can be running on callisto.  ie. (callisto, 9992)
    // LOCATION 2:
    //  PKEY 201: NS_9993 can be running on lyra.  ie. (lyra, 9993)

	NSALocator::NSALocator()
	{
		initNSALocator();
	}

	NSALocator::~NSALocator()
	{
		cleanup();
	}

	void NSALocator::initNSALocator()
	{
		std::vector< ILocation* > locations;
		try
		{
			// First retrieve all the locations from the database
			locations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
		}
		catch (DatabaseException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException in NSALocator::getAllLocations()", e.what() );
		}
		catch (DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException in NSALocator::getAllLocations()", e.what() );
		}
		catch (TransactiveException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException in NSALocator::getAllLocations()", e.what() );
		}
		catch(...)
		{
			LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception");
		}

		for (std::vector< ILocation* >::iterator iter = locations.begin(); iter != locations.end(); ++iter)
		{
			if (*iter != NULL)
			{
				try
				{
					unsigned long key = (*iter)->getKey();
					TA_Base_Core::IEntityDataList entities;
					
					if (key == TA_Base_Core::ANY_LOCATION)
					{
						entities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(
							NotificationAgentData::getStaticType(), false, false);
					}
					else
					{
						entities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
							NotificationAgentData::getStaticType(), key, false);
					}

                    TA_THREADGUARD( m_remoteNSAsLock );
					m_locationToEntityMap[ key ] = entities;
				}
				catch (TransactiveException& e)
				{
					// Make sure we catch any exceptions from the database code
					LOG_EXCEPTION_CATCH( SourceInfo, "NSALocator::initNSALocator() Exception occured while getting NotificationAgentData", e.what() );
				}
				catch (...)
				{
					LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
						"NSALocator::initNSALocator(): unkown exception occurs when constructing m_locationToEntityMap");
				}

				// Now we're finished with this iterator so we might as well delete what it contains so we
				// don't have to do it later.
				delete *iter;
				*iter = NULL;
			}
		}

		try
		{
			// initialize the managed process and hostnames
            TA_THREADGUARD( m_remoteNSAsLock );
			TA_Base_Core::ManagedProcessAccessFactory::getInstance().getAllPeersOfManagedProcess(m_processToHostnameMap);
		}
		catch (TransactiveException& e)
		{
			// Make sure we catch any exceptions from the database code
			LOG_EXCEPTION_CATCH( SourceInfo, "Exception in NSALocator::initNSALocator() while getAllPeersOfManagedProcess", e.what() );
		}
		catch(...)
		{
			LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"NSALocator::initNSALocator(): unkown exception occurs when constructing m_locationToEntityMap");
		}
	}

	void NSALocator::cleanup()
	{
		FUNCTION_ENTRY("NSALocator::cleanup");

		// clean up the location and notification agent map
		LocationToEntityMap::iterator locationToEntityPtr = m_locationToEntityMap.begin();
		while( locationToEntityPtr != m_locationToEntityMap.end() )
		{
			std::vector<TA_Base_Core::IEntityData*> entityDataListPtr = locationToEntityPtr->second;
			std::vector<TA_Base_Core::IEntityData*>::iterator entityDataPtr = entityDataListPtr.begin();
			
			while( entityDataPtr != entityDataListPtr.end() )
			{
				if( *entityDataPtr != NULL )
				{
					delete *entityDataPtr;
					*entityDataPtr = NULL;
				}
				entityDataPtr++;
			}

			entityDataListPtr.clear();
			locationToEntityPtr++;
		}

		m_locationToEntityMap.clear();
		m_processToHostnameMap.clear();

		FUNCTION_EXIT;
	}

	void NSALocator::findNSAsAtLocation( unsigned long locationKey, NSALocator::NSADetailVec& nsaDetails ) const
    {
		FUNCTION_ENTRY("NSALocator::findNSAsAtLocation");

        TA_Assert(locationKey != TA_Base_Core::LOCAL_LOCATION);

		// See if this location has been looked up before (ie. has been cached).
        // Note that the special location ANY_LOCATION can be cached just like other locations.
		{
            TA_THREADGUARD( m_remoteNSAsLock );

			LocationToNSADetailMap::iterator it = m_locationToNSADetailMap.find( locationKey );
			if ( it != m_locationToNSADetailMap.end() )
			{
                nsaDetails = it->second;

				LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                    "NSALocator::findNSAsAtLocation(): NSAs in cache=%s", 
                    TA_Base_Core::toString(nsaDetails.begin(), nsaDetails.end()).c_str() );

				FUNCTION_EXIT;
                return;
			}

			// We will get here if we don't find the entry in the cache
			try
			{
				TA_Base_Core::IEntityDataList nsaEntities;
				getNSAEntitiesAtLocation(locationKey, nsaEntities);

				LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, 
					"NSALocator::findNSAsAtLocation(): Found %d NotificationServiceAgent entities for locationKey %d", 
					nsaEntities.size(), locationKey );

				TA_Base_Core::IEntityDataList::iterator it = nsaEntities.begin();
				for ( ; it != nsaEntities.end(); it++ )
				{
					NotificationAgentData* nad = (dynamic_cast<NotificationAgentData*>(*it));
					TA_Assert( nad != NULL );

					LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, 
						"NSALocator::findNSAsAtLocation(): NotificationAgentData entity '%s', broadcast=%d, locationKey=%d", 
						nad->getName().c_str(), nad->isBroadcast(), nad->getLocation() );

					std::vector<std::string> hostnames;
					getHostnamesForNSAParent(nad->getParent(), nad->getParentName(), hostnames);

					// If there are no hostnames (eg. because SN_MANAGED_PROCESS doesn't contain an entry
					// for the NSA), NO NSAs will be returned.
					for ( unsigned int i = 0; i < hostnames.size(); i++ )
					{
						// locationKey can be ANY_LOCATION.  We use the actual locationKey of the NSA entity.
						nsaDetails.push_back( NSADetail(nad->getLocation(), hostnames[i], atoi(nad->getAddress().c_str()), nad->isBroadcast()) );
					}
				}

				// Now that we are here we need to cache the result, even if there are no entries!
				if (nsaDetails.size() > 0)
				{
					m_locationToNSADetailMap[ locationKey ] = nsaDetails;
				}
				else
				{
					LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
						"NSALocator::findNSAsAtLocation(): nsaDetails is empty for location: %lu. Will not addFederator.", locationKey);
				}
			}
			catch (TA_Base_Core::TransactiveException& e)
			{
				// Make sure we catch any exceptions from the database code
				LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException in NSALocator::findNSAsAtLocation()", e.what() );

				// Do not cache or return a partial result
				nsaDetails.clear();
			}
		}

		LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "NSALocator::findNSAsAtLocation(): found NSAs=%s", 
            TA_Base_Core::toString(nsaDetails.begin(), nsaDetails.end()).c_str() );

		FUNCTION_EXIT;
	}

    void NSALocator::getNSAEntitiesAtLocation(unsigned long locationKey, TA_Base_Core::IEntityDataList& entities) const
    {
		FUNCTION_ENTRY("NSALocator::getNSAEntitiesAtLocation");

		LocationToEntityMap::iterator it = m_locationToEntityMap.find( locationKey );
		if ( it != m_locationToEntityMap.end() )
		{
            entities = it->second;
		}
		else
		{
			// NotificationAgent entities not configured for this location.
			entities.clear();
			LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"NSALocator::getNSAEntitiesAtLocation(): can't find locationkey = %lu in m_locationToEntityMap", locationKey);
		}
		FUNCTION_EXIT;
    }

    void NSALocator::getHostnamesForNSAParent(const unsigned long parentKey, const std::string& parentName, std::vector<std::string>& hostnames) const
    {
		// Get the list of hosts this parent entity is running on.
        // getPeersOfManagedProcess() uses the SN_SYSTEM_CONTROLLER and SN_MANAGED_PROCESS 
        // tables.  For example:
        // SELECT name FROM sn_system_controller WHERE snscon_id IN 
        //  (SELECT snscon_id FROM sn_managed_process WHERE managed_process_pkey = 
        //   (SELECT pkey FROM entity WHERE name = 'OccNotificationServiceMonitoredProcess'))
        //
        // ENTITY                      SN_MANAGED_PROCESS              SN_SYSTEM_CONTROLLER
        //   PKEY -1------------------m- MANAGED_PROCESS_PKEY -1------1- SNS_CON_ID
        //   TYPE ManagedProcess         SNS_CON_ID                      NAME (hostname)

        // This query finds the pkey corresponding to the parent entity name, locates the
        // appropriate records in SN_MANAGED_PROCESS, then finds all system controllers whose
        // snscon_id corresponds to one of these records.
        // TODO LPP: It seems a bit wasteful for getPeersOfManagedProcess() to use the parent
        // entity name rather than the parent entity key.  What do other clients need?
		
		FUNCTION_ENTRY("NSALocator::getHostnamesForNSAParent");

		TA_Base_Core::ProcessToHostnameMap::iterator it = m_processToHostnameMap.find( parentKey );
		if ( it != m_processToHostnameMap.end() )
		{
            hostnames = it->second;
		}
		else
		{
			LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"NSALocator::getHostnamesForNSAParent(): can't get parentKey=%lu from m_processToHostnameMap",
				parentKey);
		}

		LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, 
			"NSALocator::getHostnamesForNSAParent(): parentName=%s, hostnames=%s",
			parentName.c_str(), TA_Base_Core::toString(hostnames.begin(), hostnames.end()).c_str() );

		FUNCTION_EXIT;
	}

}
