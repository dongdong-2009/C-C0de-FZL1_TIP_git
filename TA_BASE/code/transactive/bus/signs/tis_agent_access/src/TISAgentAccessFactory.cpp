/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/signs/tis_agent_access/src/TISAgentAccessFactory.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This is used to get access to the TIS agent.
  */

#include "TISAgentAccessFactory.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/entity_access/src/TTISEntityData.h"
#include "core/data_access_interface/entity_access/src/STISEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::TTISEntityData;
using TA_Base_Core::STISEntityData;
using TA_Base_Core::LocationAccessFactory;


namespace TA_Base_Bus
{
    const std::string TISAgentAccessFactory::OCC_LOCATION_NAME = "OCC";
    
    TISAgentAccessFactory* TISAgentAccessFactory::m_theClassInstance;
    TA_Base_Core::ReEntrantThreadLockable TISAgentAccessFactory::m_singletonLock;


    TISAgentAccessFactory& TISAgentAccessFactory::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( m_theClassInstance == NULL )
		{
			// Create the one & only object
            m_theClassInstance = new TISAgentAccessFactory();
		}
		
		return *m_theClassInstance;
    }


    TISAgentAccessFactory::TISAgentAccessFactory()
        : m_ttisNamedObj(NULL),
          m_stisNamedObj(NULL),
          m_occTtisNamedObj(NULL),
          m_occStisNamedObj(NULL)
    {}


    TISAgentAccessFactory::~TISAgentAccessFactory()
    {
	    FUNCTION_ENTRY( "TISAgentAccessFactory" );
        for (StisNamedObjMap::iterator stisNameObj = m_stisNamedObjMap.begin(); stisNameObj != m_stisNamedObjMap.end(); stisNameObj++)
        {
            delete stisNameObj->second;
            stisNameObj->second = NULL;
        }
        m_stisNamedObjMap.clear();

        for (TtisNamedObjMap::iterator ttisNameObj = m_ttisNamedObjMap.begin(); ttisNameObj != m_ttisNamedObjMap.end(); ttisNameObj++)
        {
            delete ttisNameObj->second;
            ttisNameObj->second = NULL;
        }
        m_ttisNamedObjMap.clear();

        // no need to delete the other pointers cos they were just pointing to the same objects that the map 
        // was pointing to, so deleting the maps should clean them up.
        FUNCTION_EXIT;
    }


    TISAgentAccessFactory::TtisNamedObj& TISAgentAccessFactory::getTTISAgent()
    {
	    FUNCTION_ENTRY( "getTTISAgent" );
        // if it hasnt been created, create it.
        if (m_ttisNamedObj == NULL)
        {
            m_ttisNamedObj = &getTTISAgentAtLocation(getLocationName());
        }

        FUNCTION_EXIT;
        return (*m_ttisNamedObj);
    }


    TISAgentAccessFactory::StisNamedObj& TISAgentAccessFactory::getSTISAgent()
    {
	    FUNCTION_ENTRY( "getSTISAgent" );
        // if it hasnt been created, create it.
        if (m_stisNamedObj == NULL)
        {
            m_stisNamedObj = &getSTISAgentAtLocation(getLocationName());
        }

        FUNCTION_EXIT;
        return (*m_stisNamedObj);
    }


    TISAgentAccessFactory::TtisNamedObj& TISAgentAccessFactory::getOccTTISAgent()
    {
	    FUNCTION_ENTRY( "getOccTTISAgent" );
        // if it hasnt been created, create it.
        if (m_occTtisNamedObj == NULL)
        {
            m_occTtisNamedObj = &getTTISAgentAtLocation(OCC_LOCATION_NAME);
        }

        FUNCTION_EXIT;
        return (*m_occTtisNamedObj);
    }


    TISAgentAccessFactory::StisNamedObj& TISAgentAccessFactory::getOccSTISAgent()
    {
	    FUNCTION_ENTRY( "getOccSTISAgent" );
        // if it hasnt been created, create it.
        if (m_occStisNamedObj == NULL)
        {
            m_occStisNamedObj = &getSTISAgentAtLocation(OCC_LOCATION_NAME);
        }

        FUNCTION_EXIT;
        return (*m_occStisNamedObj);
    }

    
    std::string TISAgentAccessFactory::getLocationName()
    {
	    FUNCTION_ENTRY( "getLocationName" );
        // lock
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        // if the value hasnt been retrieved yet
        if (m_locationName.empty())
        {
            // get the location key and convert it to a number
            TA_ASSERT( TA_Base_Core::RunParams::getInstance().isSet(RPARAM_LOCATIONKEY),
                       "Location Key is not set");
    
            // extract the location key
            std::stringstream locationKeyStream;
            locationKeyStream << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
            unsigned long locationKey = 0;
            locationKeyStream >> locationKey;

            TA_ASSERT( locationKey != 0,
                       "Location Key could not be converted to a number, or it was 0");

            TA_Base_Core::ILocation* theLocation = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationKey);
            m_locationName = theLocation->getName();
        }

        FUNCTION_EXIT;
        return m_locationName;
    }


    TISAgentAccessFactory::StisNamedObj& TISAgentAccessFactory::getSTISAgentAtLocation(const std::string& location)
    {
	    FUNCTION_ENTRY( "getSTISAgentAtLocation" );
        StisNamedObjMap::iterator namedObj = m_stisNamedObjMap.find(location);
        if ((namedObj != m_stisNamedObjMap.end()) && (namedObj->second != NULL))
        {
            return (*(namedObj->second));
        }

        // if the stis agent is not in our map, then we haven't resolved it yet
        TA_Base_Core::ILocation* theLocation = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByName(location);

        TA_Base_Core::CorbaNameList sTisName = 
            TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(
                                                STISEntityData::getStaticType(),
                                                theLocation->getKey(),false);
        // no TIS agent
        if ( sTisName.size() < 1 )
        {
            TA_THROW(DataException("No TIS agent at the Occ", DataException::NO_VALUE, "") );
        }
        // multiple TIS agents
        else if ( sTisName.size() > 1 )
        {
            TA_THROW(DataException("Multiple TIS agents at the Occ", DataException::NOT_UNIQUE, "") );
        }

        // exactly one TIS agent. create the named object and put it into the map
        StisNamedObj* newNamedObj = new StisNamedObj(sTisName[0]);
        m_stisNamedObjMap.insert(StisNamedObjMap::value_type(location, newNamedObj));
        FUNCTION_EXIT;
        return *newNamedObj;
    }


    TISAgentAccessFactory::TtisNamedObj& TISAgentAccessFactory::getTTISAgentAtLocation(const std::string& location)
    {
	    FUNCTION_ENTRY( "getTTISAgentAtLocation" );
        TtisNamedObjMap::iterator namedObj = m_ttisNamedObjMap.find(location);
        if ((namedObj != m_ttisNamedObjMap.end()) && (namedObj->second != NULL))
        {
            return (*(namedObj->second));
        }

        // if the ttis agent is not in our map, then we haven't resolved it yet
        TA_Base_Core::ILocation* theLocation = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByName(location);

        TA_Base_Core::CorbaNameList tTisName = 
            TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(
                                                TTISEntityData::getStaticType(),
                                                theLocation->getKey(),false);
        // no TIS agent
        if ( tTisName.size() < 1 )
        {
            TA_THROW(DataException("No TIS agent at the Occ", DataException::NO_VALUE, "") );
        }
        // multiple TIS agents
        else if ( tTisName.size() > 1 )
        {
            TA_THROW(DataException("Multiple TIS agents at the Occ", DataException::NOT_UNIQUE, "") );
        }

        // exactly one TIS agent. create the named object and put it into the map
        TtisNamedObj* newNamedObj = new TtisNamedObj(tTisName[0]);
        
        //
        // Temporary for VTT
        // Set the corba timeout longer for train operations
        //
        if (TA_Base_Core::RunParams::getInstance().isSet("TrainCorbaTimeout"))
        {
            std::stringstream timeoutStr;
            timeoutStr << TA_Base_Core::RunParams::getInstance().get("TrainCorbaTimeout");
            unsigned short timeout = 0;
            timeoutStr >> timeout;

            if (timeout > 0)
            {
                newNamedObj->setObjectTimeout(timeout);
            }
        }

        // end Temporary for VTT

        m_ttisNamedObjMap.insert(TtisNamedObjMap::value_type(location, newNamedObj));
        FUNCTION_EXIT;
        return *newNamedObj;
    }
} // end namespace TA_Base_Bus
