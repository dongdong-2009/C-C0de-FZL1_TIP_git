/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/authentication_library/src/KeepSessionAliveThreadManager.cpp $
  * @author:   Anita Lee
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/02/05 18:34:52 $
  * Last modified by:  $Author: liwei.gong $
  *
  */

#ifdef _MSC_VER
#pragma warning(disable : 4250 4503)
#endif

#include <time.h>

#include "bus/security/authentication_library/src/KeepSessionAliveThreadManager.h"
#include "bus/security/authentication_library/src/KeepSessionAliveThread.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/FunctionEntry.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/AuthenticationAgentEntityData.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/naming/src/INamedObject.h"


#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/AuthenticationAgentEntityData.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ControlStation.h"	// CL-20822++

namespace TA_Base_Bus
{
    const unsigned long HEARTBEAT_DEFAULT = 30;
	const unsigned long CENTRL_PROFILE_LOCATION_ID = 0;
	// response time delay for corba call to AuthenticationAgent, equivalent to RemoateDutyPeer::TIME_OF_FLIGHT
	const unsigned short RESPONSE_TIME( 20 );	// CL-20822++
	const std::string ACCESS_CONTROL_POPUP_TIMEOUT("AccessControlPopupTimeout");	// CL-20822++
    //
    // Initialise statics
    //
    TA_Base_Core::NonReEntrantThreadLockable KeepSessionAliveThreadManager::m_singletonLock;
    KeepSessionAliveThreadManager* KeepSessionAliveThreadManager::m_theManager = NULL;


    KeepSessionAliveThreadManager& KeepSessionAliveThreadManager::getInstance()
    {
        TA_Base_Core::ThreadGuard guard(m_singletonLock);

        if (m_theManager == NULL)
        {
            m_theManager = new KeepSessionAliveThreadManager();
        }

        return *m_theManager;
    }

	void KeepSessionAliveThreadManager::releaseInstance()
	{
		if (m_theManager != NULL)
		{
			TA_Base_Core::ThreadGuard guard(m_singletonLock);
			if(m_theManager != NULL)
			{
				delete m_theManager;
				m_theManager = NULL;
			}
		}
	}

    KeepSessionAliveThreadManager::KeepSessionAliveThreadManager()
    {        
        FUNCTION_ENTRY("KeepSessionAliveThreadManager");

        unsigned long heartbeat = HEARTBEAT_DEFAULT;

        // Get the entity name based on the locationKey
        m_localLocationId = strtoul( TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY ).c_str(), NULL, 10 );

        try
        {
            // Retrieve the heartbeat value
            // std::vector<TA_Base_Core::IEntityData*> entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(TA_Base_Core::AuthenticationAgentEntityData::getStaticType());
			// Get the entity name based on the locationKey
			TA_Assert( m_localLocationId > 0 );
			std::vector<TA_Base_Core::IEntityData*> entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::AuthenticationAgentEntityData::getStaticType(), m_localLocationId );

            TA_ASSERT(entityData.size() > 0, "Authentication Entity cannot be retrieved");

            TA_Base_Core::AuthenticationAgentEntityData* agentData = dynamic_cast <TA_Base_Core::AuthenticationAgentEntityData*> (entityData[0]);

            heartbeat = agentData->getKeepSessionAliveHeartbeat();

            std::vector<TA_Base_Core::IEntityData*>::iterator iter;
            for ( iter=entityData.begin(); iter!=entityData.end(); ++iter )
            {
                if(*iter != NULL)
                {
                    delete *iter;
                    *iter = NULL;
                }
            }

			initAuthAgentMap();
        }
        catch(const TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
             LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Error retrieving configuration parameters. Setting to heartbeat to default");
        }
        catch(const TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
             LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Error retrieving configuration parameters. Setting to heartbeat to default");
        }
        catch(...)
        {
             LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Error retrieving configuration parameters. Setting to heartbeat to default");
        }

         LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Using a keep alive heartbeat of %d seconds.", heartbeat );

        // Set the heart beat interval of the thread.
        m_keepAliveThread.setHeartbeat( heartbeat );

        // start the keep alive thread
        m_keepAliveThread.start();

        FUNCTION_EXIT;
    }


    KeepSessionAliveThreadManager::~KeepSessionAliveThreadManager()
    {
        FUNCTION_ENTRY("~KeepSessionAliveThreadManager");

        // Remove all sessions from the keep alive thread.
        m_keepAliveThread.removeAllSessions();

        // now stop the thread
        m_keepAliveThread.terminateAndWait();

		std::map<unsigned long, AuthenticationAgentNamedObj*>::iterator it;
		for( it=m_authAgentMap.begin(); it!=m_authAgentMap.end(); it++)
		{
			AuthenticationAgentNamedObj* pAuthAgent = it->second;
			TA_Assert( pAuthAgent != NULL );
			delete pAuthAgent;
			pAuthAgent = NULL;
		}
		m_authAgentMap.clear();

        FUNCTION_EXIT;
    }


    bool KeepSessionAliveThreadManager::canFindSessionWithId( const std::string& sessionId, SessionInfo& session )
    {
		return m_keepAliveThread.canFindMatchingSession( sessionId, session );
    }


    void KeepSessionAliveThreadManager::startKeepingSessionAlive( const SessionInfo& session )
    {
        FUNCTION_ENTRY("startKeepingSessionAlive");

        // Add the session to the keep alive thread.
        m_keepAliveThread.addSession( session );

        FUNCTION_EXIT;
    }


    void KeepSessionAliveThreadManager::stopKeepingSessionAlive( const std::string& sessionId )
    {
        FUNCTION_ENTRY("stopKeepingSessionAlive");

        // Remove the session from the keep alive thread.
        m_keepAliveThread.removeSession( sessionId );

        FUNCTION_EXIT;
    }

	//TD18707 xinsong ++ 
	void KeepSessionAliveThreadManager::initAuthAgentMap()
	{
		std::string agentType = TA_Base_Core::AuthenticationAgentEntityData::getStaticType();

		TA_Base_Core::CorbaNameList authAgentNameList = 
			TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtAllLocation( agentType,true );
		if( authAgentNameList.size() <= 0 )
		{
			LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "No Authentication Agent was found" );
			return;
		}
		
		// ly++ for CL-20822
		// set corba call timeout to *2 to AuthenticationAgent call timeout (*4 for dutyAgent call)
		// each time we add RESPONSE_TIME seconds for routine response
		// actually, AuthLibrary will not use corba macro retry repository and will return before AuthAgent corba call timeout
		// Because: 
		//	Location DutyAgent will return to AuthAgent and do not cause AuthAgent timeout even some PeerDutyAgent Hang
		// 	AuthAgent will return before AuthLib raise TRANISENT_CallTimedOut. Only for some of Locations Duty allocate failed.
		unsigned short usTimeout;
		if ( TA_Base_Core::RunParams::getInstance().get( ACCESS_CONTROL_POPUP_TIMEOUT.c_str() ).empty() )
		{
			std::auto_ptr<TA_Base_Core::ControlStation> cs( dynamic_cast<TA_Base_Core::ControlStation*>
				( TA_Base_Core::EntityAccessFactory::getInstance().getEntity( "ControlStation") ) );
			unsigned long ulDutyAgentTimeOut = cs->getAccessControlPopupTimeout();
			
			usTimeout = ( ( (unsigned short)ulDutyAgentTimeOut + RESPONSE_TIME ) * 2 + RESPONSE_TIME ) * 2 + RESPONSE_TIME; // +10 for response
		}
		else
		{
			usTimeout = ( ( (unsigned short)atoi( TA_Base_Core::RunParams::getInstance().get( ACCESS_CONTROL_POPUP_TIMEOUT.c_str() ).c_str() ) + RESPONSE_TIME ) * 2 + RESPONSE_TIME ) * 2 + RESPONSE_TIME; // +10 for response 
		}
		 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"[CL-20822] Set AuthenticationAgent requestSession call timeout to %u seconds",
			usTimeout );
		// ++ly

		for(size_t i=0; i<authAgentNameList.size(); i++)
		{
			TA_Base_Core::CorbaName & corbaName = authAgentNameList[i];
			unsigned long locationId = corbaName.getLocationKey();
			TA_ASSERT( locationId >= 0, "LocationId should be larger than 0" );
			AuthenticationAgentNamedObj * authAgent = new AuthenticationAgentNamedObj(corbaName);
			
			// ly++ CL-20822
			authAgent->setObjectTimeout( usTimeout );
			// ++ly

			m_authAgentMap[locationId] = authAgent;			
		}
	}



	AuthenticationAgentNamedObj & KeepSessionAliveThreadManager::getAuthCorbaNameObject( unsigned long locationID )
	{
		if ( locationID == CENTRL_PROFILE_LOCATION_ID )
		{
			 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "central profile use authentication object" );
			locationID = m_localLocationId;
		}
		else if ( m_authAgentMap.find(locationID) == m_authAgentMap.end() )
		{
			std::ostringstream ostr;
			ostr << "could not find authtication agent at location " << locationID;
			TA_THROW( TA_Base_Core::AuthenticationAgentException(ostr.str().c_str()) );
		}

		return *(m_authAgentMap[locationID]);
	}

	AuthenticationAgentNamedObj & KeepSessionAliveThreadManager::getAuthCorbaNameObject( const std::string & sessionId )
	{
		if ( sessionId.size() == 0 )
		{
			return getAuthCorbaNameObject(m_localLocationId);
		}
		if ( TA_Base_Core::SessionAccessFactory::isSupperSessionId( sessionId ) )
		{
			return getAuthCorbaNameObject(m_localLocationId);
		}
		if ( TA_Base_Core::SessionAccessFactory::isObsoleteSessionIdFormat( sessionId ) )
		{
			return getAuthCorbaNameObject(m_localLocationId);
		}
		if ( !TA_Base_Core::SessionAccessFactory::isCurrentSessionIdFormat( sessionId ) )
		{
			std::ostringstream ostr;
			ostr << "could not recognize the location for session " << sessionId;
			TA_THROW( TA_Base_Core::AuthenticationAgentException(ostr.str().c_str()) );
		}
		unsigned long locationID = TA_Base_Core::SessionAccessFactory::getLocIDFromSessionId( sessionId );
		return getAuthCorbaNameObject(locationID);
	}

	bool KeepSessionAliveThreadManager::isSameLocationId( const std::string & sessionId, unsigned long locationId )
	{
		unsigned long sessionLocationId = 0;
		if ( TA_Base_Core::SessionAccessFactory::isSupperSessionId( sessionId ) )
		{
			sessionLocationId = m_localLocationId;
		}
		else if ( TA_Base_Core::SessionAccessFactory::isObsoleteSessionIdFormat( sessionId ) )
		{
			sessionLocationId = m_localLocationId;
		}
		else if ( !TA_Base_Core::SessionAccessFactory::isCurrentSessionIdFormat( sessionId ) )
		{
			TA_ASSERT( false, "error session format");
			LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error session format %s", sessionId.c_str() );
		}
		else
		{
			sessionLocationId = TA_Base_Core::SessionAccessFactory::getLocIDFromSessionId( sessionId );
		}
		
		if ( locationId == CENTRL_PROFILE_LOCATION_ID )
		{
			return sessionLocationId == m_localLocationId;
		}

		return sessionLocationId == locationId;
	}

	bool KeepSessionAliveThreadManager::isLocalLocationId(unsigned long locationId )
	{
		//central profile
		if ( locationId == CENTRL_PROFILE_LOCATION_ID )
		{
			return true;
		}
		return m_localLocationId == locationId;
	}

};
