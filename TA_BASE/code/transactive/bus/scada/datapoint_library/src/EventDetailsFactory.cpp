/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/EventDetailsFactory.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/01/19 17:43:23 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *	This class provides a single point to get the Asset Name associated with the specified entity key.
  * It maintains a list of the proxies that provide the asset names and a map between the proxy pkey
  * and the asset name.  This class also handles the callbacks from the proxies when the asset names
  * are changed and updates the map accordingly.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <algorithm>

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "bus/scada/datapoint_library/src/EventDetailsFactory.h"


namespace TA_Base_Bus
{

	EventDetailsFactory *						EventDetailsFactory::m_eventDetailsFactory = 0;
	unsigned int								EventDetailsFactory::m_refCounter = 0;
	TA_Base_Core::ReEntrantThreadLockable		EventDetailsFactory::m_lock;
    std::vector<std::string>                    EventDetailsFactory::m_agentNameMap;
	TA_Base_Core::PkeyEventDetailsMap			EventDetailsFactory::m_pkeyEventDetailsMap;
	TA_Base_Core::DataPointEventDetails 		EventDetailsFactory::m_defaultEventDetails;

	EventDetailsFactory::EventDetailsFactory()
	{
		m_pkeyEventDetailsMap.clear();
		m_defaultEventDetails.parentKey = 0;
		m_defaultEventDetails.assetName = "ASSET NAME UNDEFINED";
		m_defaultEventDetails.eventMessage = "EVENT MESSAGE UNDEFINED";
	};

	EventDetailsFactory::~EventDetailsFactory()
	{
		m_pkeyEventDetailsMap.clear();
	}


	EventDetailsFactory * EventDetailsFactory::getInstance()
	{
		if ( 0 == m_eventDetailsFactory )
		{
			// prevent multiple threads creating multiple instances
			// simultaneously
			TA_Base_Core::ThreadGuard guard( m_lock );

			// if the EventDetailsFactory has not been created yet
			if ( 0 == m_eventDetailsFactory )
			{
				// create one and only one instance
				m_eventDetailsFactory = new EventDetailsFactory();
			}
		}

		// increment reference counter and return the single instance of
		// EventDetailsFactory
		m_refCounter++;
		return m_eventDetailsFactory;
	}


	void EventDetailsFactory::removeInstance ( EventDetailsFactory * instance )
	{
		// prevent multiple threads attempting to delete/create
		// simultaneously
		TA_Base_Core::ThreadGuard guard( m_lock );

		// decrement reference counter
		m_refCounter--;

		// if not referenced anywhere
		if ( 0 == m_refCounter )
		{
			// destroy the only instance
			delete m_eventDetailsFactory;
			m_eventDetailsFactory = 0;
            
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Singleton object of EventDetailsFactory is removed.");
        }
	}

    void EventDetailsFactory::populateEventDetails (const std::string agentName)
    {
        TA_Base_Core::ThreadGuard guard( m_mapLock );
        std::vector<std::string>::iterator i = std::find(m_agentNameMap.begin(),
            m_agentNameMap.end(), agentName);
        if( i == m_agentNameMap.end())
        {
            m_agentNameMap.push_back(agentName);
            TA_Base_Core::DataPointAccessFactory::getAllDpEventDetails ( agentName, m_pkeyEventDetailsMap );
        }
    }

    const TA_Base_Core::DataPointEventDetails& EventDetailsFactory::getEventDetails ( unsigned long entityKey, const std::string& agentName /*=""*/)
	{
		//
		// PW3726: asset names to be loaded from database, not from DataNodeProxy
		//

        // TD18575, to avoid multiple thread access event details map
        TA_Base_Core::ThreadGuard guard( m_mapLock );
        
		// if the list of asset names is not loaded
		if ( true == m_pkeyEventDetailsMap.empty() )
		{
			try
			{
                if (agentName.empty())
                {
                // use agent name to load event messages, instead of using location key
                TA_Base_Core::CorbaName dpCorbaName = 
                    TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity(entityKey,false);

                    populateEventDetails( dpCorbaName.getAgentName() );

                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "EventDetailMap is built for agent [%s]", dpCorbaName.getAgentName().c_str() );
                }
                else
                {
                    populateEventDetails( agentName );

                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "EventDetailMap is built for agent [%s]", agentName.c_str() );
                }

            }
			catch ( const TA_Base_Core::DatabaseException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
			}
			catch ( const TA_Base_Core::DataException& e )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
			}
		}

		// find if the specified entity key already in the map
		TA_Base_Core::PkeyEventDetailsMap::iterator itr = m_pkeyEventDetailsMap.find ( entityKey );

		// if found one
		if ( m_pkeyEventDetailsMap.end() != itr )
		{
			// return the asset name associated with the key
			return m_pkeyEventDetailsMap [ entityKey ];
		}

		// if found none
		else
		{
			// Maybe this DP is at a different location

			try
			{
                if (agentName.empty())
                {
                    // use agent name to load event messages, instead of using location key
				    TA_Base_Core::CorbaName dpCorbaName = 
					TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity(entityKey,false);

				    // try to get all asset names from database that are associated with 
				    // the specified location key
                    populateEventDetails( dpCorbaName.getAgentName() );
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "EventDetailMap is re-built for agent [%s]", dpCorbaName.getAgentName().c_str() );
                }
                else
                {
                    populateEventDetails( agentName );
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "EventDetailMap is re-built for agent [%s]", agentName.c_str() );
                }

            }
			catch ( const TA_Base_Core::DatabaseException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
			}
			catch ( const TA_Base_Core::DataException& e )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
			}

			// second try: find if the specified entity key already in the map
			TA_Base_Core::PkeyEventDetailsMap::iterator itr = m_pkeyEventDetailsMap.find ( entityKey );

			// if found one
			if ( m_pkeyEventDetailsMap.end() != itr )
			{
				// return the asset name associated with the key
				return m_pkeyEventDetailsMap [ entityKey ];
			}

			// if still not find one
			else
			{
                m_pkeyEventDetailsMap [ entityKey ] = m_defaultEventDetails;

				// return the default asset name
				return m_defaultEventDetails;
			}
		}
	}
}
