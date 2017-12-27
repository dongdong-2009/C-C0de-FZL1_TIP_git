/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/pa/pa_agent_proxy/src/PaAgentAccessFactory.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/23 16:48:56 $
  * Last modified by:  $Author: qi.huang $
  *
  * This is used to get access to the pa agent.
  */
#include <sstream>

#include "core/synchronisation/src/ThreadGuard.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/StationPAAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/MasterPAAgentEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

#include "bus/pa/pa_agent_proxy/src/PaAgentAccessFactory.h"

namespace TA_Base_Bus
{
    PaAgentAccessFactory* PaAgentAccessFactory::m_theClassInstance = 0;
    TA_Base_Core::ReEntrantThreadLockable PaAgentAccessFactory::m_singletonLock;

    PaAgentAccessFactory& PaAgentAccessFactory::getInstance()
    {
		if ( 0 == m_theClassInstance )
        {
            TA_Base_Core::ThreadGuard guard( m_singletonLock );

            if ( 0 == m_theClassInstance )
            {
                // Create the one & only object
                m_theClassInstance = new PaAgentAccessFactory();
            }
		}
		
		return *m_theClassInstance;
    }

    void PaAgentAccessFactory::delInstance()
    {
        if ( 0 != m_theClassInstance )
        {
            TA_Base_Core::ThreadGuard guard( m_singletonLock );

            if ( 0 != m_theClassInstance )
            {
                // Create the one & only object
                delete m_theClassInstance;
                m_theClassInstance = 0;
            }
        }
    }

    PaAgentAccessFactory::PaAgentAccessFactory() : 
    m_mapPaNamedObjMap(),
    m_pwOccLocationKey()
    {
        FUNCTION_ENTRY( "PaAgentAccessFactory" );

        FUNCTION_EXIT;
    }

    PaAgentAccessFactory::~PaAgentAccessFactory()
    {
	    FUNCTION_ENTRY( "~PaAgentAccessFactory" );

        for (PaNamedObjMap::iterator itLoop = m_mapPaNamedObjMap.begin(); itLoop != m_mapPaNamedObjMap.end(); itLoop++)
        {
            delete itLoop->second;
            itLoop->second = 0;
        }

        m_mapPaNamedObjMap.clear();

        FUNCTION_EXIT;
    }

    void PaAgentAccessFactory::getOccLocationkey() 
    {
        unsigned long ulOccKey = TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey();
        m_pwOccLocationKey.setValue( ulOccKey );
    }

    PaNamedObj& PaAgentAccessFactory::getMasterPaAgentOfOcc()
    {
	    FUNCTION_ENTRY( "getMasterPaAgentAtOcc" );

        static PaNamedObj* pOccPaAgent = 0;
        if ( 0 == pOccPaAgent )
        {
            if ( !m_pwOccLocationKey.hasBeenSet() )
            {
                getOccLocationkey();
            }

            pOccPaAgent = getMasterPaAgent( m_pwOccLocationKey.getValue() );
        }

        FUNCTION_EXIT;
        return (*pOccPaAgent);
    }

    PaNamedObj& PaAgentAccessFactory::getPaAgentByLocationKey( unsigned long ulLocationKey )
    {
	    FUNCTION_ENTRY( "getPaAgentByLocationKey" );

        PaNamedObjMap::iterator itFound = m_mapPaNamedObjMap.find( ulLocationKey );
        if (( m_mapPaNamedObjMap.end() != itFound ) && ( 0 != itFound->second ))
        {
            return *(itFound->second);
        }

        if ( !m_pwOccLocationKey.hasBeenSet() )
        {
            getOccLocationkey();
        }

        const std::string EntityType = ( m_pwOccLocationKey.getValue() == ulLocationKey ) ? 
            TA_Base_Core::MasterPAAgentEntityData::getStaticType() : TA_Base_Core::StationPAAgentEntityData::getStaticType();

        TA_Base_Core::CorbaNameList vecPaNames = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(
             EntityType, ulLocationKey, false);

        // no pa agent
        if ( vecPaNames.size() < 1 )
        {
            std::stringstream stmErrorMsg;
            stmErrorMsg << "No station pa agent at the location: " << ulLocationKey;
            TA_THROW( TA_Base_Core::DataException( stmErrorMsg.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }
        // multiple pa agents
        else if ( vecPaNames.size() > 1 )
        {
            std::stringstream stmErrorMsg;
            stmErrorMsg << "Multiple station pa agents at the location: " << ulLocationKey;
            TA_THROW( TA_Base_Core::DataException( stmErrorMsg.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE, "" ));
        }

        // exactly one TIS agent. create the named object and put it into the map
        PaNamedObj* newNamedObj = new PaNamedObj( vecPaNames[0] );

        m_mapPaNamedObjMap[ulLocationKey] = newNamedObj;

        FUNCTION_EXIT;
        return *newNamedObj;
    }

    PaNamedObj* PaAgentAccessFactory::getMasterPaAgent( unsigned long ulLocationKey )
    {
        FUNCTION_ENTRY( "getMasterPaAgent" );

        PaNamedObjMap::iterator itFound = m_mapPaNamedObjMap.find( ulLocationKey );
        if (( m_mapPaNamedObjMap.end() != itFound ) && ( 0 != itFound->second ))
        {
            return itFound->second;
        }

        TA_Base_Core::CorbaNameList vecPaNames = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(
            TA_Base_Core::MasterPAAgentEntityData::getStaticType(), ulLocationKey, false );

        // no pa agent
        if ( vecPaNames.size() < 1 )
        {
            std::stringstream stmErrorMsg;
            stmErrorMsg << "No master pa agent at the location: " << ulLocationKey;
            TA_THROW( TA_Base_Core::DataException( stmErrorMsg.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }
        // multiple pa agents
        else if ( vecPaNames.size() > 1 )
        {
            std::stringstream stmErrorMsg;
            stmErrorMsg << "Multiple master pa agents at the location: " << ulLocationKey;
            TA_THROW( TA_Base_Core::DataException( stmErrorMsg.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE, "" ));
        }

        // exactly one TIS agent. create the named object and put it into the map
        PaNamedObj* newNamedObj = new PaNamedObj( vecPaNames[0] );

        m_mapPaNamedObjMap[ulLocationKey] = newNamedObj;

        FUNCTION_EXIT;
        return newNamedObj;
    }
} // end namespace TA_Base_Bus
