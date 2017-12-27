/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */


#include "bus/alarm/alarm_list_control/src/TrainAgentAccess.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataBaseException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_Bus
{

    TrainAgentAccess::TrainAgentAccess()
        : m_cctvTrainAgent( NULL ),
          m_informationTrainAgent( NULL ),
          m_locationKey( 0 ),
          m_consoleKey( 0 )
    {
        FUNCTION_ENTRY( "TrainAgentAccess" );
        FUNCTION_EXIT;
    }


    TrainAgentAccess::~TrainAgentAccess()
    {
        FUNCTION_ENTRY( "~TrainAgentAccess" );
        
        delete m_cctvTrainAgent;
        m_cctvTrainAgent = NULL;

        delete m_informationTrainAgent;
        m_informationTrainAgent = NULL;
        
        FUNCTION_EXIT;
    }


    TrainAgentAccess& TrainAgentAccess::getInstance()
    {
        FUNCTION_ENTRY( "getInstance" );

		FUNCTION_EXIT;
        return *( TrainAgentAccessSingleton::instance() );
    }


    TrainAgentAccess::TrainCctvNamedObject& TrainAgentAccess::getTrainCctvAgent()
    {
        FUNCTION_ENTRY( "getTrainCctvAgent" );

        // initialise and return the named object
        initialise();

        FUNCTION_EXIT;
        return *m_cctvTrainAgent;
    }


    TrainAgentAccess::TrainInformationNamedObject& TrainAgentAccess::getTrainInformationAgent()
    {
        FUNCTION_ENTRY( "getTrainInformationAgent" );

        // initialise and return the named object
        initialise();

        FUNCTION_EXIT;
        return *m_informationTrainAgent;
    }


    unsigned long TrainAgentAccess::getLocationKey()
    {
        FUNCTION_ENTRY( "getLocationKey" );

        // initialise and return the location key
        initialise();

        FUNCTION_EXIT;
        return m_locationKey;
    }


    unsigned long TrainAgentAccess::getConsoleKey()
    {
        FUNCTION_ENTRY( "getConsoleKey" );

        // initialise and return the console key
        initialise();

        FUNCTION_EXIT;
        return m_consoleKey;
    }


    void TrainAgentAccess::initialise()
    {
        FUNCTION_ENTRY( "initialise" );

        // If the data is already initialised, return.
        // use m_informationTrainAgent, because this is the last thing set below
        if ( NULL != m_informationTrainAgent )
        {
            FUNCTION_EXIT;
            return;
        }
        
        // Get the Corba name of the agent.
        
        try
        {
            // Determine the local agent for the physical Location of this Session.
            
            // get the session id from runparams
            // get the session information using the AuthenticaionLibrary
            std::string sessionID = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );
        
            // get session info from the authentication agent
            AuthenticationLibrary authenticationLibrary;
            SessionInfo sessionInfo = authenticationLibrary.getSessionInfo( sessionID, sessionID );
        
            // populate m_consoleKey and m_locationKey from the session information
            m_consoleKey = sessionInfo.WorkstationId;
            m_locationKey = sessionInfo.LocationId;
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
            throw TA_Base_Core::TransactiveException( "Unable to get session information." );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "Unable to get session information." );
            throw TA_Base_Core::TransactiveException( "Unable to get session information." );
        }

        if ( 0 == m_locationKey )
        {
            std::string strLocationKey = (const std::string&)TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
			
			std::istringstream strStream(strLocationKey);
			strStream >> m_locationKey;
        }
        
        try
        {
            // load train agent for this location from the database
            TA_Base_Core::CorbaNameList agents =
                TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( 
                    TA_Base_Core::TrainAgentEntityData::getStaticType(), m_locationKey,false );
        
            if ( 1 != agents.size() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "The Corba name for the Train Agent could not be uniquely determined, because there were %d names returned.",
                    agents.size() );

                throw TA_Base_Core::TransactiveException( "The Corba name for the Train Agent could not be determined." );
            }
        
            m_cctvTrainAgent = new TrainCctvNamedObject( agents[ 0 ].getAgentName(), TrainAgentObjectNames::TrainCctvObjectName );
            m_informationTrainAgent = new TrainInformationNamedObject( agents[ 0 ].getAgentName(), TrainAgentObjectNames::TrainInformationObjectName );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            throw TA_Base_Core::TransactiveException( "The Corba name for the Train Agent could not be determined." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            throw TA_Base_Core::TransactiveException( "The Corba name for the Train Agent could not be determined." );
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
            throw TA_Base_Core::TransactiveException( "The Corba name for the Train Agent could not be determined." );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "The Corba name for the Train Agent could not be determined." );
            throw TA_Base_Core::TransactiveException( "The Corba name for the Train Agent could not be determined." );
        }

        FUNCTION_EXIT;
    }

}
