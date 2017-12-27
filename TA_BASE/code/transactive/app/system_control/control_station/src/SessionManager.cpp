/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Karen Graham
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This class manages all session related tasks such as logging in, logging out
  * and communicating with the System Controller.
  */

#pragma warning(disable:4786)

#include "app\system_control\control_station\src\StdAfx.h"
#include "app\system_control\control_station\src\SessionManager.h"
#include "app\system_control\control_station\src\GraphWorxManager.h"
#include "app\system_control\control_station\src\ControlStationCorbaImpl.h"
#include "app\system_control\control_station\src\IGraphWorxManager.h"
#include "app\system_control\control_station\src\ControlStationRootDialog.h"
#include "app\system_control\control_station\src\ProcessManager.h"
#include "app\system_control\control_station\src\WinkeysMgr.h"
#include "core\data_access_interface\src\DatabaseKey.h"
#include "core\data_access_interface\src\LocationAccessFactory.h"

#include <algorithm> // for transform
#include <cctype> // for tolower
#include <sstream>
#include <winsock2.h>  // Used by getHostName

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/database_status_monitor/DsmCorbaDef/IDL/src/DatabaseStatusCorbaDef.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/data_access_interface/entity_access/src/ControlStation.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/SimpleDbDatabase.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/EntityTypeException.h"
#include "core/exceptions/src/SessionException.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/exceptions/src/GraphWorxException.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/RightsException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/Hostname.h"
#include "core/message/types/SystemControllerComms_MessageTypes.h"
#include "core/message/types/ControlStationAudit_MessageTypes.h"
#include "core/message/types/ControlStationComms_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/TransactiveMessageSubscription.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/PublicationManager.h"
#include "core/message/src/ConnectionChecker.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/process_management/IDL/src/IControlStationCorbaDef.h"
#include "core/process_management/IDL/src/IManagedApplicationCorbaDef.h"
#include "core/database/src/DBException.h"
//TD16491++
#include "core/data_access_interface/alarm_rule/src/AlarmRuleAccessFactory.h"
//++TD16491

#include "core/threads/src/IWorkItem.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"

using namespace TA_Base_App::TA_ControlStation;
using TA_Base_Bus::AuthenticationLibrary;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::OnlineUpdateListener;
using TA_Base_Core::RunParams;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::MessageSubscriptionManager;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::EntityTypeException;
using TA_Base_Core::GraphWorxException;
using TA_Base_Core::SessionException;
using TA_Base_Core::ApplicationException;
using TA_Base_Core::RightsException;
using TA_Base_Core::AuthenticationSecurityException;
using TA_Base_Core::AuthenticationAgentException;
using TA_Base_Core::TransactiveException;


SessionManager* SessionManager::s_instance = NULL;
bool SessionManager::m_dbUserPwdValid = true;

TA_Base_Core::NonReEntrantThreadLockable SessionManager::m_singletonLock;
const std::string SessionManager::CONSOLE_NAME("ConsoleName");
const std::string SessionManager::PROFILE_NAME("ProfileName");
const std::string SessionManager::USER_NAME("Username");
const std::string SessionManager::GROUP_ONLINE = "GroupOnline";
const std::string SessionManager::LOGIC_LOCATION("LogicLoc");
bool SessionManager::s_exitNeedsRestart = false;

SessionManager::SessionManager()
    : m_isLoggedIn(false),
      m_resourceKey(-1),
      m_sCGroupOnline(true),
      m_hostName(""),
      m_systemControllerName(""),
      m_graphWorxHelp(""),
      m_controlStationEntity(NULL),
      m_systemControllerEntity(NULL),
      m_controlStationImpl(NULL),
      m_processManager(NULL),
      m_graphWorxManager(NULL),
      m_authenticationMgr(NULL),
      m_rightsMgr(NULL),
      m_dutyManager(NULL),
      m_dllHostEntity(NULL)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "run" );

    m_hostName = TA_Base_Core::Hostname::getHostname();
    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "This console is: %s",m_hostName.c_str());

    IF_NOT_LOCAL_TEST
    {
        locateSystemController();

        retrieveRunParams(m_hostName);
    }

    // Load the Control Station entity and retrieve all the data we need
    std::string entityName = RunParams::getInstance().get(RPARAM_ENTITYNAME);

    try
    {
        m_controlStationEntity = dynamic_cast<TA_Base_Core::ControlStation*>(EntityAccessFactory::getInstance().getEntity( entityName ));
    }
    catch ( const DataException&)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Converting into a SessionException");
        TA_THROW( SessionException("Could not load the ControlStation entity. Unable to continue") );
    }
    catch ( const DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Converting into a SessionException");
        TA_THROW( SessionException("Could not load the ControlStation entity. Unable to continue") );
    }
	//TD17205,jianghp
	catch(const DBException &e)
	{
		if (e.getNativeCode() == TA_Base_Core::INVALID_USERNAME_PASSWORD)
		{
			m_dbUserPwdValid = false;
		}
	}
	//TD17205,jianghp
}


SessionManager& SessionManager::getInstance()
{
    // protect the instance check / creation
    ThreadGuard guard(m_singletonLock);

    if( s_instance == NULL)
    {
        s_instance = new SessionManager();
    }

	return *s_instance;
}


SessionManager::~SessionManager()
{
    FUNCTION_ENTRY("~SessionManager");
	
    try
    {
        // This takes a long time to shutdown, so do this first, hopefully all of them
        // would have shutdown by the time process manager shuts down its external processes
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Going to delete control station impl");
        bool isCorbaShutdownRequired = false;
        try
        {
            if (m_controlStationImpl != NULL)
            {
                m_controlStationImpl->deactivateServant();
                isCorbaShutdownRequired = true;
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Successfully deactivated control station servant");
            }
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Failed to deactivate control station");
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Going to delete GraphWorx Manager");
        if (m_graphWorxManager !=  NULL)
        {
            delete m_graphWorxManager;
            m_graphWorxManager = NULL;
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Successfully deleted GraphWorx Manager");
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Going to delete banner entities");
        for (unsigned int i = 0; i < m_bannerEntities.size(); i++)
        {
            delete m_bannerEntities[i];
            m_bannerEntities[i] = NULL;
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Successfully deleted banner entities");
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Going to delete dllhost entity");
        delete m_dllHostEntity;
        m_dllHostEntity = NULL;
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Successfully deleted dllhost entity");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Going to delete control station entity");
        if (m_controlStationEntity != NULL)
        {
            delete m_controlStationEntity;
            m_controlStationEntity = NULL;
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Successfully deleted control station entity");
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Going to delete system controller entity");
        if (m_systemControllerEntity != NULL)
        {
            delete m_systemControllerEntity;
            m_systemControllerEntity = NULL;
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Successfully deleted system controller entity");
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Going to delete authentication manager");
        if (m_authenticationMgr != NULL)
        {
            std::string id = m_sessionDetails.getSessionId();
            if (!id.empty())
            {
                try
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Still got a session, going to terminate the session first");
                    // make sure we end the session
                    m_authenticationMgr->endSession(id);
                }
                catch (...)  
                {
                    // catch all exceptions since we want to continue deleting the rest of the stuff
                    // of the stuff even if some exception occurred
                }
            }
            delete m_authenticationMgr;
            m_authenticationMgr = NULL;
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Successfully deleted authentication Manager");
        }

/* 	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Going to delete rights manager");
        if (m_rightsMgr != NULL)
        {
            delete m_rightsMgr;
            m_rightsMgr = NULL;
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Successfully deleted rights Manager");
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Going to delete duty manager");
        if (m_dutyManager != NULL)
        {
            delete m_dutyManager; // TD11312, TD11486 - Clean up duty manager registration
            m_dutyManager = NULL;
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Successfully deleted duty Manager");
        }
*/
        

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Going to delete process manager");
        try
        {
            if (m_processManager != NULL)
            {
                m_processManager->terminateAllApplications(TA_Base_App::TA_ControlStation::EXIT);
                m_processManager->deactivateServant();
                isCorbaShutdownRequired = true;
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Successfully deleted process Manager");
            }
        }
        catch (std::exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Error terminating/deactivating process manager", ex.what());
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Failed to deactivate process manager");
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Going to clean up online update and message registrations");
        OnlineUpdateListener::cleanUp(); // TD1399 - Clean up all registrations to online updates so that we can shutdown properly
        // Unsubscribe the subscriber
        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Going to delete process manager");
        if (isCorbaShutdownRequired)
        {
		    // Clean up the alarm subsystem
		    TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();	

            // Clean up named objects
            TA_Base_Core::Naming::cleanUp();

            TA_Base_Core::gDestroySubscriptionManager();
            TA_Base_Core::gTerminateAndDeleteStructuredEventSuppliers();
			TA_Base_Core::ChannelLocatorConnectionMgr::removeInstance();

            // Shutdown corba
            TA_Base_Core::CorbaUtil::getInstance().shutdown();
            TA_Base_Core::CorbaUtil::cleanup();
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Successfully shutdown corba");
        }
    }
    catch ( ... )
    {
        // Catch all exceptions so we don't get errors filtering out
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor of SessionManager");
    }
    FUNCTION_EXIT;
}


void SessionManager::cleanUp()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "cleanUp" );

    // protect the instance check / creation
    ThreadGuard guard(m_singletonLock);
	//
	// stop the background task if the flag indicates that the thread has been
	// started
	//
	
    if (s_instance != NULL)
    {	
		if ( THREAD_STATE_RUNNING == s_instance->getCurrentState() )
		{
			s_instance->terminateAndWait();
		}
        delete s_instance;
        s_instance = NULL;
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "cleanUp" );
}


void SessionManager::run()
{
    using TA_Base_Core::IResource;
    using TA_Base_Core::ResourceAccessFactory;
    bool wasSuccessfulInitialisation = false;

    try
    {
        /////////////////////////////////////////
        // Initialise all the necessary run params
        /////////////////////////////////////////

        RunParams::getInstance().set( TA_Base_App::TA_ControlStation::DEBUG_LOG_DIR.c_str(),
                                      m_controlStationEntity->getDebugLogDir().c_str() );

        if (RunParams::getInstance().get(RPARAM_DEBUGFILE).empty())
        {
            std::string debugFile = RunParams::getInstance().get(TA_Base_App::TA_ControlStation::DEBUG_LOG_DIR.c_str());
            if (!debugFile.empty())
            {
                if ( (debugFile[debugFile.length() - 1] != '/') &&
                     (debugFile[ debugFile.length() - 1] != '\\') )
                {
                    debugFile+= "\\";
                }
                debugFile += TA_Base_App::TA_ControlStation::LOG_PRE_STRING;
                debugFile += m_controlStationEntity->getName();
                debugFile += TA_Base_App::TA_ControlStation::LOG_POST_STRING;
                DebugUtil::getInstance().setFile(debugFile.c_str());
            }
        }

        if (true == m_controlStationEntity->getUserPreferences())
        {
            RunParams::getInstance().set(RPARAM_USERPREFERENCES,"on");
        }

        RunParams::getInstance().set(ACCESS_CONTROL_POPUP_TIMEOUT.c_str(), SessionDetails::convertULtoString(m_controlStationEntity->getAccessControlPopupTimeout()).c_str());

        //TD16984 - the runparam string should not be sent if value is empty 
       	if (!(m_controlStationEntity->getMmsSubmitterType().empty()))
		{
             //TD16491++
             RunParams::getInstance().set(TA_Base_Core::AlarmRuleAccessFactory::RPARAM_MMS_SUBMITTER_TYPE.c_str(), m_controlStationEntity->getMmsSubmitterType().c_str());
             //++TD16491
		}		


        /////////////////////////////////////////
        // Initialise member variables
        /////////////////////////////////////////
        m_sessionDetails.initialise(m_hostName);

        m_graphWorxHelp = m_controlStationEntity->getGraphWorxHelpfile();

        // load banners that are child entities of this control station entity
        m_bannerEntities = EntityAccessFactory::getInstance().getChildEntitiesOfEntityOfType(m_controlStationEntity->getName(),
                                                                                             TA_Base_App::TA_ControlStation::BANNER_ENTITY_TYPE);
        try
        {
            m_dllHostEntity = EntityAccessFactory::getInstance().getEntity(TA_Base_App::TA_ControlStation::DLLHOST_ENTITY_NAME);
        }
        catch(const TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
        }
        catch(const TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
        }

        // We can now initialise the Screen positioning object since we have a ControlStation object.
        m_screenPositions.loadScreenLayout(*m_controlStationEntity,m_bannerEntities);

        // Start the process manager
        m_processManager = new ProcessManager();

        if (m_processManager == NULL)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new on ProcessManager returned NULL");
            TA_THROW( SessionException("Could not create a ProcessManager") );
        }
        TA_Base_Core::CorbaUtil::getInstance().setServantKey(m_processManager,"ProcessManager");
        m_processManager->activateServant();

        // Set the list of applications that are to be terminated on exit 
        m_processManager->setApplicationsToTerminate(m_controlStationEntity->getApplicationsToTerminate());
        m_processManager->setApplicationsToTerminateOnLogout(m_controlStationEntity->getApplicationsToTerminateOnLogout());
        // get rid of all the zombie external processes first before we proceed.  Need to
        // do this before we start the graphworx processes so the the new processes that
        // we start off won't be killed 
        m_processManager->terminateExternalApplications();

        // Start GraphWorx loading as soon as possible as this takes a long time to load
        m_graphWorxManager = new GraphWorxManager( m_controlStationEntity->getBackgroundDisplay(), m_controlStationEntity->getStartupDisplay(), m_screenPositions );

        if (m_graphWorxManager == NULL)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new on GraphWorxManager returned NULL");
            TA_THROW( SessionException("Could not start the GraphWorX instances. Unable to continue") );
        }

        IResource* resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(m_controlStationEntity->getKey());
        if (resource != NULL)
        {
            m_resourceKey = resource->getKey();
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "IResource key = %d", m_resourceKey);
            delete resource;
        }
        else
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Cannot find the resource key for control station");
        }

        // This member has to be created after we've set up the messaging stuff
        m_dutyManager = new DutyManager();

        if (m_dutyManager == NULL)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new on DutyManager returned NULL");
            TA_THROW( SessionException("Could not create a DutyManager") );
        }

        // Initialising authentication library
        m_authenticationMgr = new AuthenticationLibrary();        

        TA_Base_Bus::RightsLibraryFactory rightsFactory;
        m_rightsMgr = rightsFactory.buildRightsLibrary();

        m_controlStationImpl = new ControlStationCorbaImpl(m_sessionDetails.getConsoleName());
        if (m_controlStationImpl == NULL)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new on ControlStsationCorbaImpl returned NULL");
            TA_THROW( SessionException("Could not create a ControlStationCorbaImpl") );
        }

        IF_NOT_LOCAL_TEST
        {
            TA_Base_Core::IEntityData* systemControllerEntity = EntityAccessFactory::getInstance().getEntity(m_systemControllerName);
            // now subscribe to the system controller online and offline comms messages
            MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::SystemControllerComms::SystemControllerGroupOnline, this, systemControllerEntity->getKey(), systemControllerEntity->getSubsystem(), systemControllerEntity->getLocation(), NULL);
            MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::SystemControllerComms::SystemControllerGroupOffline, this, systemControllerEntity->getKey(), systemControllerEntity->getSubsystem(), systemControllerEntity->getLocation(), NULL);
            MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::ControlStationComms::TAToGXMessageNotification, this, m_sessionDetails.getConsoleId(), 0, atoi((TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY)).c_str()), NULL);
		    //lizettejl++ (DP-changes)
		    MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
			    TA_Base_Core::ControlStationComms::GXToTAMessageNotification, this,  0, 0, atoi((TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY)).c_str()), NULL );

		    MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
			    TA_Base_Core::ControlStationComms::LaunchApplication, this, NULL);

		    //++lizettejl
            delete systemControllerEntity;
            systemControllerEntity = NULL;

            // Register interest for online updates
            OnlineUpdateListener::getInstance().registerInterest(TA_Base_Core::ENTITY_OWNER, *this, m_controlStationEntity->getKey());
            OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::SECURITY, *this);
            OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::ACTION_GROUP, *this);
        }

        wasSuccessfulInitialisation = true;
    }
    catch ( const EntityTypeException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "EntityTypeException","Converting into a SessionException");
    }
    catch ( const GraphWorxException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "GraphWorxException","Converting into a SessionException");
    }
    catch ( const DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Converting into a SessionException");
    }
    catch ( const DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Converting into a SessionException");
    }
    catch (const RightsException&)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "RightsException","Converting into a SessionException");
    }
    catch (...)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown exception","Converting into a SessionException");
    }

    if (wasSuccessfulInitialisation)
    {
        // Submit an audit with the name of the console on which this application is running
        unsigned long entityKey = 0;
        try
        {
            if (m_controlStationEntity != NULL)
            {
                entityKey = m_controlStationEntity->getKey();
            }
        }
        catch(...)
        {
            // If there is any type of exception thrown, then go with the default value. Log the failure
            LOG_EXCEPTION_CATCH(SourceInfo, "General exception", 
                "An exception was caught while attempting to retrieve the Control Station entity key. Proceeding with default value");
        }

        std::string consoleName = m_sessionDetails.getConsoleName();

        // Get the audit message sender with which to send the message
        TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().
            getAuditMessageSender(TA_Base_Core::ControlStationAudit::Context);

        TA_Base_Core::DescriptionParameters params;
        TA_Base_Core::NameValuePair param1(CONSOLE_NAME,consoleName);
        params.push_back(&param1);

        sender->sendAuditMessage( TA_Base_Core::ControlStationAudit::ControlStationStart, // Message Type
                                 entityKey,     // Entity key
                                 params,        // Message parameters
                                 "N/A",         // Additional message details
                                 "",            // Session ID - null because we're not logged in
                                 "",            // Alarm ID - null because there's no alarm
                                 "",            // Incident ID - null because there's no incident
                                 "" );          // Parent event ID - null because there's no parent event

        delete sender;
        sender = NULL;

		while ( ! ControlStationRootDialog::getInstance().hasInitDialog() )
		{
			TRACE("sleep\n");
			sleep( 2000 );
		}

        // if the entity contains login information, then automatically login
        if (m_controlStationEntity->canLoginAutomatically())
        {
            try
            {
                IF_NOT_LOCAL_TEST
                {
                    // TD14005
                    m_graphWorxManager->waitUntilReady();
                }

                login(m_controlStationEntity->getLoginOperator(),
                    m_controlStationEntity->getLoginProfile(),
                    m_controlStationEntity->getLoginLocation(),
                    CString(m_controlStationEntity->getLoginPassword().c_str()));
            }
            catch (...)
            {
                wasSuccessfulInitialisation = false;
            }
        }
    }

    // Tell root dialog whether we managed to initialise properly
    ControlStationRootDialog::getInstance().initialisationComplete(wasSuccessfulInitialisation);
    
    if (wasSuccessfulInitialisation)
    {
		// TD6972 if the group offline run param is set send an update message. The group is online we
        // don't need to send anything
		std::string param = TA_Base_Core::RunParams::getInstance().get(RPARAM_GROUPSOFFLINE);
		if( (!param.empty()) && (param.compare(GROUP_ONLINE) != 0) )
		{
		    m_sCGroupOnline = false;
		    ControlStationRootDialog::getInstance().PostMessage(WM_SYSTEMCONTROLLER_STATE_CHANGE, (LPARAM)false);
		}
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "run" );
}


void SessionManager::terminate()
{
    // Don't need to do anything here as the run method will always finish.
}


void SessionManager::login(const ProfileLoginDlg& loginDlg)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "login" );

    TA_Base_Core::IOperator* theOperator = NULL;
    TA_Base_Core::IProfile*  theProfile  = NULL;
    TA_Base_Core::ILocation*  theLocation  = NULL;
    CString password;
    loginDlg.getSelectedDetails(theOperator,theProfile,theLocation,password);
    login(theOperator,theProfile,theLocation,password);

    LOG( SourceInfo, DebugUtil::FunctionExit, "login" );
}


void SessionManager::login(TA_Base_Core::IOperator* theOperator,
                          TA_Base_Core::IProfile* theProfile,
                          TA_Base_Core::ILocation* theLocation,
                          CString& password)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "login" );

    // Try to request for a session
    requestSession(theOperator,theProfile,theLocation,password);

    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Operator logged in");

    // Indicate that we have logged in so when GraphWorx starts up and calls back we are ready
    // to answer it.
    m_isLoggedIn = true;
    ControlStationRootDialog::getInstance().PostMessage(WM_LOGIN_DETAILS_CHANGED);

    try
    {
        m_graphWorxManager->launchGraphWorx();
    }
    catch( const GraphWorxException& )
    {
        // Login has failed so undo everything we've done so far
        m_isLoggedIn = false;
		m_authenticationMgr->endSession(m_sessionDetails.getSessionId());
        m_sessionDetails.sessionLogout();
        m_graphWorxManager->shutdownGraphWorx();
        m_processManager->terminateAllApplications();
        m_processManager->terminateExternalApplications();
        throw;
    }
    // TD10353: Load the banner AFTER GraphWorx has been successfully launched.
    //          Otherwise it just gets killed straight away.
    loadBanners();
    
    loadDllHost();
    // Submit an audit with the name of the console on which this application is running
    unsigned long entityKey = 0;
    try
    {
        if (m_controlStationEntity != NULL)
        {
            entityKey = m_controlStationEntity->getKey();
        }
    }
    catch(...)
    {
        // If there is any type of exception thrown, then go with the default value. Log the failure
        LOG_EXCEPTION_CATCH(SourceInfo, "General exception", 
            "An exception was caught while attempting to retrieve the Control Station entity key. Proceeding with default value");
    }
    
    std::string consoleName = m_sessionDetails.getConsoleName();
    std::string userName = m_sessionDetails.getOperatorName();
	std::string profileName = m_sessionDetails.getProfileName();
	std::string logicLocation = m_sessionDetails.getLocationName();
    // Get the audit message sender with which to send the message
    TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().
        getAuditMessageSender(TA_Base_Core::ControlStationAudit::Context);

    TA_Base_Core::DescriptionParameters params; //td 15530
    TA_Base_Core::NameValuePair param1(CONSOLE_NAME,consoleName);
    TA_Base_Core::NameValuePair param2(PROFILE_NAME,profileName);
	TA_Base_Core::NameValuePair param3(LOGIC_LOCATION,logicLocation);
	TA_Base_Core::NameValuePair param4(USER_NAME,userName);
    params.push_back(&param1);
    params.push_back(&param2);
	params.push_back(&param3);
    params.push_back(&param4);
    sender->sendAuditMessage( TA_Base_Core::ControlStationAudit::ControlStationLogin, // Message Type
                             entityKey,                                     // Entity key
                             params,                                        // Message parameters
                             "N/A",                                         // Additional message details
                             m_sessionDetails.getSessionId(),               // Session ID
                             "",                                            // Alarm ID - null because there's no alarm
                             "",                                            // Incident ID - null because there's no incident
                             "" );                                          // Parent event ID - null because there's no parent event

    delete sender;
    sender = NULL;

    LOG( SourceInfo, DebugUtil::FunctionExit, "login" );
}


void SessionManager::changeProfile(const ProfileLoginDlg& loginDlg)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "changeProfile" );

    // if the session is no longer valid then just throw an exception to prevent user
    // from continuing on

    if (!m_sessionDetails.isSessionValid())
    {
        TA_THROW(SessionException(SessionException::INVALID_SESSION_CONFIGURATION, "Trying to change profile when the session configuration has become invalid"));
    }

    TA_Base_Core::IOperator* theOperator = NULL;
    TA_Base_Core::IProfile*  theProfile  = NULL;
    TA_Base_Core::ILocation*  theLocation  = NULL;
    CString password;

    loginDlg.getSelectedDetails(theOperator,theProfile,theLocation,password);

    try
    {
        if ( theProfile == NULL )
        {
            TA_THROW( SessionException("Profile retrieved from the login dialogue was NULL. Cannot continue to log the user in") );
        }

        // the operator should still be same one
        if (theOperator->getName() != m_sessionDetails.getOperatorName())
        {
            TA_THROW( SessionException("Attempting to change the profile of operator who is not logged in") );
        }

        unsigned int locationKey = 0;

        // if we're offline, we'll always just use the current location
        if (!m_sCGroupOnline)
        {
            locationKey = getSessionDetails().getServerLocation();
        }
        else if (theLocation != NULL)
        {
            locationKey = theLocation->getKey();
        }

        // we get the new session id when we change profile
        std::string sessionId = m_authenticationMgr->changeProfile(
                                m_sessionDetails.getSessionId(), 
                                theProfile->getKey(), 
                                locationKey, 
                                std::string(password));
        m_sessionDetails.profileChanged(sessionId, theProfile,theLocation);
        // Tell all running applications that the session Id has changed
        m_processManager->updateSessionId(sessionId);

		// Submit an audit message for the new profile
        unsigned long entityKey = 0;
        try
        {
            if (m_controlStationEntity != NULL)
            {
                entityKey = m_controlStationEntity->getKey();
            }
        }
        catch(...)
        {
            // If there is any type of exception thrown, then go with the default value. Log the failure
            LOG_EXCEPTION_CATCH(SourceInfo, "General exception", 
                "An exception was caught while attempting to retrieve the Control Station entity key. Proceeding with default value");
        }

        std::string consoleName = m_sessionDetails.getConsoleName();
		std::string profileName = m_sessionDetails.getProfileName();
        std::string userName = m_sessionDetails.getOperatorName();

        // Get the audit message sender with which to send the message
        TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().
            getAuditMessageSender(TA_Base_Core::ControlStationAudit::Context);

        TA_Base_Core::DescriptionParameters params;
        TA_Base_Core::NameValuePair param1(CONSOLE_NAME,consoleName);
		TA_Base_Core::NameValuePair param2(PROFILE_NAME,profileName);
        TA_Base_Core::NameValuePair param3(USER_NAME,userName);
        params.push_back(&param1);
        params.push_back(&param2);
        params.push_back(&param3);

        sender->sendAuditMessage( TA_Base_Core::ControlStationAudit::ControlStationChangeProfile, // Message Type
                                 entityKey,                                     // Entity key
                                 params,                                        // Message parameters
                                 "N/A",                                         // Additional message details
                                 m_sessionDetails.getSessionId(),               // Session ID
                                 "",                                            // Alarm ID - null because there's no alarm
                                 "",                                            // Incident ID - null because there's no incident
                                 "" );                                          // Parent event ID - null because there's no parent event

    }
    catch (const AuthenticationSecurityException&)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, 
            "AuthenticationSecurityException", "Converting into a SessionException");
        TA_THROW(SessionException(SessionException::AUTHENTICATION_FAILED, 
            "Failed to authenticate to change profile"));
    }
    catch (const AuthenticationAgentException&)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, 
            "AuthenticationAgentException", "Converting into a SessionException");
        TA_THROW(SessionException(SessionException::GENERAL_ERROR, 
            "Authentication Agent failed when trying to change profile"));
    }
    catch (const SessionException&)
    {
        throw;
    }
    // TD14617
    catch (const TransactiveException&)
    {
        CString actionName;
        TA_VERIFY(actionName.LoadString(IDS_CHANGE_PROFILE)!=0, "Unable to load IDS_CHANGE_PROFILE");
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        userMsg.showMsgBox(IDS_UE_010018);
        // TransactiveException need to log operator off cos we don't have a valid session.
        ControlStationRootDialog::getInstance().PostMessage(WM_FORCE_LOGOUT);
    }
    catch (...) // whatever exception that we got, just make it SessionException
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown Exception", 
            "An unknown exception is caught during login authentication.  Converting into a SessionException");
        TA_THROW(SessionException(SessionException::GENERAL_ERROR, 
            "Unknown error when attempting to login"));
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "changeProfile" );
}

TA_Base_Bus::SessionInfo SessionManager::getSessionInfo(const std::string & sessionId)
{
	return this->m_authenticationMgr->getSessionInfo(sessionId,"");
}
void SessionManager::changeOperator(const ProfileLoginDlg& loginDlg)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "changeOperator" );

    // if the session is no longer valid then just throw an exception to prevent user
    // from continuing on
    if (!m_sessionDetails.isSessionValid())
    {
        TA_THROW(SessionException(SessionException::INVALID_SESSION_CONFIGURATION, "Trying to change operator when the session configuration has become invalid"));
    }

    TA_Base_Core::IOperator* theOperator = NULL;
    TA_Base_Core::IProfile*  theProfile  = NULL;
    TA_Base_Core::ILocation*  theLocation  = NULL;
    CString password;

    loginDlg.getSelectedDetails(theOperator,theProfile,theLocation,password);

    try
    {
        if ( (theOperator == NULL) || (theProfile == NULL) )
        {
            TA_THROW( SessionException("Operator or profile retrieved from the login dialogue was NULL. Cannot continue to log the user in") );
        }

        // the operator shouldn't be same one
        if (theOperator->getName() == m_sessionDetails.getOperatorName())
        {
            TA_THROW( SessionException("Attempting to change into the same operator") );
        }

        unsigned int locationKey = 0;

        // if we're offline, we'll always just use the current location
        if (!m_sCGroupOnline)
        {
            locationKey = getSessionDetails().getConsoleId();
        }
        else if (theLocation != NULL)
        {
            locationKey = theLocation->getKey();
        }

        // get the old session Id
        std::string oldSessionId = m_sessionDetails.getSessionId();
        std::string oldUserName = m_sessionDetails.getOperatorName();

        // Try to request for a new session
        requestSession(theOperator,theProfile,theLocation,password);

        
        // Submit an audit message for the new login
        unsigned long entityKey = 0;
        try
        {
            if (m_controlStationEntity != NULL)
            {
                entityKey = m_controlStationEntity->getKey();
            }
        }
        catch(...)
        {
            // If there is any type of exception thrown, then go with the default value. Log the failure
            LOG_EXCEPTION_CATCH(SourceInfo, "General exception", 
                "An exception was caught while attempting to retrieve the Control Station entity key. Proceeding with default value");
        }

        std::string consoleName = m_sessionDetails.getConsoleName();
        std::string userName = m_sessionDetails.getOperatorName();

        // Get the audit message sender with which to send the message
        TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().
            getAuditMessageSender(TA_Base_Core::ControlStationAudit::Context);

        TA_Base_Core::DescriptionParameters params;
        TA_Base_Core::NameValuePair param1(CONSOLE_NAME,consoleName);
        TA_Base_Core::NameValuePair param2(USER_NAME,userName);
        params.push_back(&param1);
        params.push_back(&param2);

        sender->sendAuditMessage( TA_Base_Core::ControlStationAudit::ControlStationLogin, // Message Type
                                 entityKey,                                     // Entity key
                                 params,                                        // Message parameters
                                 "N/A",                                         // Additional message details
                                 m_sessionDetails.getSessionId(),               // Session ID
                                 "",                                            // Alarm ID - null because there's no alarm
                                 "",                                            // Incident ID - null because there's no incident
                                 "" );                                          // Parent event ID - null because there's no parent event


        // logout the old session
        try
        {
            // make sure we remove the operator override before logging off
            if (m_sessionDetails.isOperatorOverridden())
            {
                m_authenticationMgr->endOverride(oldSessionId);
            }
            m_authenticationMgr->endSession(oldSessionId);
        }
        catch (...)
        {
            // If the session manager somehow couldn't logout, just ignore
            // we already got the new session
        }

        // And an audit message for the logout
        try
        {
            if (m_controlStationEntity != NULL)
            {
                entityKey = m_controlStationEntity->getKey();
            }
        }
        catch(...)
        {
            // If there is any type of exception thrown, then go with the default value. Log the failure
            LOG_EXCEPTION_CATCH(SourceInfo, "General exception", 
                "An exception was caught while attempting to retrieve the Control Station entity key. Proceeding with default value");
        }    

        TA_Base_Core::NameValuePair param3(USER_NAME,oldUserName);
        params.clear();
        params.push_back(&param1);
        params.push_back(&param3); // WP 4619M50980010 TD 9195 - changed to use correct parameter when sending audit message

        sender->sendAuditMessage( TA_Base_Core::ControlStationAudit::ControlStationLogout, // Message Type
                                 entityKey,                                     // Entity key
                                 params,                                        // Message parameters
                                 "N/A",                                         // Additional message details
                                 oldSessionId,               // Session ID
                                 "",                                            // Alarm ID - null because there's no alarm
                                 "",                                            // Incident ID - null because there's no incident
                                 "" );                                          // Parent event ID - null because there's no parent event

        delete sender;
        sender = NULL;
    }
    catch (const AuthenticationSecurityException&)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, 
            "AuthenticationSecurityException", "Converting into a SessionException");
        TA_THROW(SessionException(SessionException::AUTHENTICATION_FAILED, 
            "Failed to authenticate to change operator"));
    }
    catch (const AuthenticationAgentException&)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, 
            "AuthenticationAgentException", "Converting into a SessionException");
        TA_THROW(SessionException(SessionException::GENERAL_ERROR, 
            "Authentication Agent failed when trying to change operator"));
    }
    catch (const SessionException&)
    {
        throw;
    }
    catch (...) // whatever exception that we got, just make it SessionException
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown Exception", 
            "An unknown exception is caught during login authentication.  Converting into a SessionException");
        TA_THROW(SessionException(SessionException::GENERAL_ERROR, 
            "Unknown error when attempting to login"));
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "changeOperator" );
}


void SessionManager::operatorOverride(const ProfileLoginDlg& loginDlg)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "operatorOverride" );

    // if the session is no longer valid then just throw an exception to prevent user
    // from continuing on
    if (!m_sessionDetails.isSessionValid())
    {
        TA_THROW(SessionException(SessionException::INVALID_SESSION_CONFIGURATION, "Trying to override when the session configuration has become invalid"));
    }

    TA_Base_Core::IOperator* theOperator = NULL;
    TA_Base_Core::IProfile*  theProfile  = NULL;
    TA_Base_Core::ILocation*  theLocation  = NULL;
    CString password;

    loginDlg.getSelectedDetails(theOperator,theProfile,theLocation,password);

    try
    {
        if ( (theOperator == NULL) || (theProfile == NULL) )
        {
            TA_THROW( SessionException("Operator or profile retrieved from the login dialogue was NULL. Cannot continue to log the user in") );
        }

        std::string locationName = "";
        unsigned int locationKey = 0;
        try
        {
            // if we're offline, we'll always just use the current location
            if (!m_sCGroupOnline)
            {
                locationName = getSessionDetails().getConsoleName(); 
                locationKey = getSessionDetails().getConsoleId();
            }
            else if (theLocation != NULL)
            {
                locationName = theLocation->getName();
                locationKey = theLocation->getKey();
            }
        }
        catch ( const DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to retrieve profile, console or operator name for audit message");
            return;
        }
        catch ( const DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to retrieve profile, console or operator name for audit message");
            return;
        }            
        // Check if the two profiles are the same AND
        // The location names are the same
        if ( (m_sessionDetails.getProfileName() == theProfile->getName()) &&
             (m_sessionDetails.getLocationName() == locationName )
           )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "The Details selected for override are the same as the current details. Ignoring.");
            LOG( SourceInfo, DebugUtil::FunctionExit, "operatorOverride" );
            return;
        }

        m_authenticationMgr->beginOverride(m_sessionDetails.getSessionId(), 
                                           theOperator->getKey(), 
                                           theProfile->getKey(), 
                                           locationKey, 
                                           std::string(password));
        m_sessionDetails.operatorOverride(theOperator,theProfile,theLocation);

		// yanrong: reset rights cache, rights library will call rights agent to check permission.
		m_rightsMgr->resetCache();

        // Tell all running applications that the session Id has changed
        m_processManager->updateSessionId(m_sessionDetails.getSessionId());
    }
    catch (const AuthenticationSecurityException&)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, 
            "AuthenticationSecurityException", "Converting into a SessionException");
        TA_THROW(SessionException(SessionException::AUTHENTICATION_FAILED, 
            "Failed to authenticate for operator override"));
    }
    catch (const AuthenticationAgentException&)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, 
            "AuthenticationAgentException", "Converting into a SessionException");
        TA_THROW(SessionException(SessionException::GENERAL_ERROR, 
            "Authentication Agent failed when trying to do operator override"));
    }
    catch ( const SessionException& )
    {
        throw;
    }
    catch (...) // whatever exception that we got, just make it SessionException
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown Exception", 
            "An unknown exception is caught during login authentication.  Converting into a SessionException");
        TA_THROW(SessionException(SessionException::GENERAL_ERROR, 
            "Unknown error when attempting to do operator override"));
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "operatorOverride" );
}


void SessionManager::changePassword(const CPasswordDlg& passwordDlg)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "changePassword" );

    TA_Base_Core::IOperator* theOperator = NULL;
    CString oldPassword, newPassword1, newPassword2;

    passwordDlg.getSelectedDetails(theOperator, oldPassword, newPassword1, newPassword2);

    // if the session is no longer valid then just throw an exception to prevent user
    // from continuing on
    if (!m_sessionDetails.isSessionValid())
    {
        TA_THROW(SessionException(SessionException::INVALID_SESSION_CONFIGURATION, "Trying to change password when the session configuration has become invalid"));
    }

    try
    {
        // if both new passwords are not empty and the same
        if ((newPassword1 != "") && (newPassword1 == newPassword2))
        {
            // if new passwords are shorter than what's allowed
            if ((newPassword1.GetLength() < MIN_PASSWORD_LENGTH) || (newPassword1.GetLength() > MAX_PASSWORD_LENGTH))
            {
                TA_THROW(SessionException(SessionException::INVALID_PASSWORD_LENGTH, 
                    "Password is shorter than minimum length"));
            }
            m_authenticationMgr->changePassword(m_sessionDetails.getSessionId(),
                std::string(oldPassword), std::string(newPassword1));
        }
        else  // new passwords are different
        {
            TA_THROW(SessionException(SessionException::PASSWORD_MISMATCH, 
                "New passwords are different"));
        }
    }
    catch (const SessionException&)
    {
        throw;
    }
    catch (const AuthenticationSecurityException& ex)
    {
        if (ex.getReason() == AuthenticationSecurityException::INVALID_PASSWORD_LENGTH)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "AuthenticationSecurityException::INVALID_PASSWORD", "Converting into a SessionException::CHANGE_PASSWORD_FAILED");
            // We shouldn't really get this since we've checked the length of the password in CPasswordDlg, but
            // if for some reason it wasn't caught there, 
            TA_THROW(SessionException(SessionException::INVALID_PASSWORD_LENGTH, 
                "Failed to change password"));
        }
        else  // Any other exceptions, just throw it as a general AUTHENTICATION_FAILED exception
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "AuthenticationSecurityException", "Converting into a SessionException");
            TA_THROW(SessionException(SessionException::AUTHENTICATION_FAILED, 
                "Failed to authenticate to change password"));
        }
    }
    catch (const AuthenticationAgentException&) 
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "AuthenticationAgentException", "Converting into a SessionException");
        TA_THROW(SessionException(SessionException::GENERAL_ERROR, 
            "Authentication Agent failed when trying to change password"));
    }
    catch (...) // whatever exception that we got, just make it SessionException
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown Exception", 
            "Some unknown exception has occurred while attempting to change password. Converting into a SessionException");
        TA_THROW(SessionException(SessionException::GENERAL_ERROR, 
            "Unknown error encountered when attempting to change password"));
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "changePassword" );
}


void SessionManager::removeOperatorOverride()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "removeOperatorOverride" );

    try
    {
        m_authenticationMgr->endOverride(m_sessionDetails.getSessionId());

		// yanrong: reset rights cache, rights library will call rights agent to check permission.
		m_rightsMgr->resetCache();

        // Tell all running applications that the session Id has changed
        m_processManager->updateSessionId(m_sessionDetails.getSessionId());
    }
    catch (...)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown exception", 
            "Some unknown exception has occurred while attempting to remove operator override");
    }

    m_sessionDetails.removeOperatorOverride();

    LOG( SourceInfo, DebugUtil::FunctionExit, "removeOperatorOverride" );
}


void SessionManager::logout()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "logout" );

    TA_ASSERT(m_isLoggedIn, "Trying to log out though not logged in");


    std::string sessionId = m_sessionDetails.getSessionId();
    // TD9195 GT: Moved these above the sessionLogout call so that the details passed 
    //         into the audit message are valid.
    std::string consoleName = m_sessionDetails.getConsoleName();
    std::string userName = m_sessionDetails.getOperatorName();

    try
    {
        // make sure we remove the operator override before logging off
        if (m_sessionDetails.isOperatorOverridden())
        {
            m_authenticationMgr->endOverride(m_sessionDetails.getSessionId());
        }
        m_authenticationMgr->endSession(m_sessionDetails.getSessionId());
    }
    catch (...)
    {
        // If the session manager somehow couldn't logout, we still want to
        // continue with the rest of the logout process
    }
    m_isLoggedIn = false;

    m_sessionDetails.sessionLogout();

    // Shutdown GraphWorx if it is currently running.
    m_graphWorxManager->shutdownGraphWorx();

    ControlStationRootDialog::getInstance().SendMessage(WM_HIDE_GRAPHWORX);

    // Tell the processManager to terminate all running applications.
    if (m_processManager != NULL)
    {
        m_processManager->terminateAllApplications();
    }

    
    
    // Submit an audit with the name of the console on which this application is running
    unsigned long entityKey = 0;
    try
    {
        if (m_controlStationEntity != NULL)
        {
            entityKey = m_controlStationEntity->getKey();
        }
    }
    catch(...)
    {
        // If there is any type of exception thrown, then go with the default value. Log the failure
        LOG_EXCEPTION_CATCH(SourceInfo, "General exception", 
            "An exception was caught while attempting to retrieve the Control Station entity key. Proceeding with default value");
    }
    

    // Get the audit message sender with which to send the message
    TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().
        getAuditMessageSender(TA_Base_Core::ControlStationAudit::Context);

    TA_Base_Core::DescriptionParameters params;
    TA_Base_Core::NameValuePair param1(CONSOLE_NAME,consoleName);
    TA_Base_Core::NameValuePair param2(USER_NAME,userName);
    params.push_back(&param1);
    params.push_back(&param2);

    sender->sendAuditMessage( TA_Base_Core::ControlStationAudit::ControlStationLogout, // Message Type
                             entityKey,                                     // Entity key
                             params,                                        // Message parameters
                             "N/A",                                         // Additional message details
                             sessionId,                                     // Session ID
                             "",                                            // Alarm ID - null because there's no alarm
                             "",                                            // Incident ID - null because there's no incident
                             "" );                                          // Parent event ID - null because there's no parent event

    delete sender;
    sender = NULL;

    LOG( SourceInfo, DebugUtil::FunctionExit, "logout" );
}


EPermission SessionManager::isShutdownPermitted(const ProfileLoginDlg& loginDlg)
{
    bool isExit = (loginDlg.getTerminateCode() == TA_Base_App::TA_ControlStation::EXIT);

    // if it is not access controlled, then it'll always be permitted to do this
    if ((isExit && !isExitAccessControlled()) ||
        (!isExit && !isRestartAccessControlled()))
    {
        return TA_Base_App::TA_ControlStation::PERMITTED;
    }

    TA_Base_Core::IOperator* theOperator = NULL;
    TA_Base_Core::IProfile*  theProfile  = NULL;
    TA_Base_Core::ILocation*  theLocation  = NULL;
    CString password;

    loginDlg.getSelectedDetails(theOperator,theProfile,theLocation,password);
    requestSession(theOperator,theProfile,theLocation,password);

    unsigned int action = isExit?TA_Base_Bus::aca_CONTROL_STATION_EXIT:TA_Base_Bus::aca_CONTROL_STATION_RESTART;

    // login details are correct, so see if operator has rights to restart
    return isControlPermitted(action);
}


IProcessManager* SessionManager::getProcessManager()
{
    IF_LOCAL_TEST
    {
        return m_processManager;
    }

    if( !m_isLoggedIn )
    {
        return NULL;
    }
    return m_processManager;
}


IGraphWorxManager* SessionManager::getGraphWorxManager()
{
    return m_graphWorxManager;
}


void SessionManager::locateSystemController()
{
    using TA_Base_Core::RunParams;
    using TA_Base_Core::CorbaUtil;

    LOG( SourceInfo, DebugUtil::FunctionEntry, "locateSystemController" );

    std::string hosts = RunParams::getInstance().get(TA_Base_App::TA_ControlStation::HOST_NAMES.c_str());

    //
    // Convert the portnumber into an unsigned long
    //
    unsigned long portNumber = TA_Base_Core::DEFAULT_MGR_PORT;

    std::string portNumberString = RunParams::getInstance().get(TA_Base_App::TA_ControlStation::PORT_NUMBER.c_str()).c_str();
    if ( !portNumberString.empty() )
    {
		if (std::string::npos == portNumberString.find_first_not_of("0123456789") )
        {
            //all valid characters
            portNumber = atol( portNumberString.c_str() );
            if (0 == portNumber)
            {
                portNumber = TA_Base_Core::DEFAULT_MGR_PORT;
            }
        }
    }

    // Generate a list of hostnames from the command line argument
    std::vector<std::string> hostNames;
    std::string::size_type position = hosts.find_first_of(",",0);
	while (position != std::string::npos)
    {
        hostNames.push_back( hosts.substr(0,position) );
        position = hosts.find_first_of(",",position+1);
    }

    // Get the last name that is not followed by a comma
    position = hosts.find_last_of(",");
    hostNames.push_back( hosts.substr(position+1, hosts.length() - (position+1)) );

    bool haveFoundSystemController = false;
    std::vector<std::string>::iterator iter = hostNames.begin();
    while( (iter != hostNames.end()) && (!haveFoundSystemController) )
    {
        CString procManString;
        CORBA::Object_var obj;
        procManString.Format("corbaloc::%s:%lu/ProcessManager", iter->c_str(), portNumber);

        try
        {
            // Attempt to get Process Manager
            obj = CorbaUtil::getInstance().stringToObject(procManString.GetBuffer(1));
            if (!CORBA::is_nil(obj))
            {
                m_systemController = TA_Base_Core::IProcessManagerCorbaDef::_narrow(obj);
                if (!CORBA::is_nil(m_systemController))
                {
                    haveFoundSystemController = true;
                    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Found system controller, %s", iter->c_str());
                    m_systemControllerName = *iter;

                    // TD 6950: Set the system controller name as a run param so it can be retrieved by
                    // applications that require subscribing to System Controller messages to be notified
                    // when the system is in degraded mode or recovered from degraded mode.
                    RunParams::getInstance().set(RPARAM_SYSTEMCONTROLLERNAME, m_systemControllerName.c_str());
                }
            }   
        }
        catch ( ... )
        {
            // Nothing to do as we just move onto the next host
        }
        ++iter;
    }

    if (!haveFoundSystemController)
    {
        std::string errorMsg = "No system controller was located on any of the hosts specifed: ";
        errorMsg += hosts;
        TA_THROW( SessionException(errorMsg.c_str()) );
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "locateSystemController" );
}


void SessionManager::retrieveRunParams(const std::string& hostName)
{
    using TA_Base_Core::RunParams;

    LOG( SourceInfo, DebugUtil::FunctionEntry, "retrieveRunParams" );

    try
    {
        TA_Base_Core::RunParamSeq_var tmpParams = m_systemController->getParams(0,hostName.c_str());

        // For each RunParam we have retrieved. If it is not already set then
        // set it.
        for (unsigned long i = 0; i < tmpParams->length(); ++i)
        {
            if( !RunParams::getInstance().isSet(tmpParams[i].name) )
            {
                RunParams::getInstance().set(tmpParams[i].name, tmpParams[i].value);
            }
        }
    }
    catch (const CORBA::Exception& ex)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),"");
        TA_THROW( SessionException("Could not retrieve a list of RunParams from the System Controller. Since we won't have most of the information we need then there is no point continuing") );
    }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
            "Unknown","Caught some unknown exception when trying to retrive run params");
        TA_THROW( SessionException("Could not retrieve a list of RunParams from the System Controller. Since we won't have most of the information we need then there is no point continuing") );
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "retrieveRunParams" );
}


void SessionManager::loadBanners()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "loadBanners" );

    try
    {
        // Get a debug path we can use to create a log file for each banner
        std::string debugFilePath = TA_Base_Core::RunParams::getInstance().get(TA_Base_App::TA_ControlStation::DEBUG_LOG_DIR.c_str());
        if (!debugFilePath.empty())
        {
            if ( (debugFilePath[debugFilePath.length() - 1] != '/') &&
                 (debugFilePath[ debugFilePath.length() - 1] != '\\') )
            {
                debugFilePath+= "\\";
            }
        }

        try
        {
			/*
            unsigned int numMonitors = m_screenPositions.getNumberOfMonitors();
            unsigned int numBanners = m_bannerEntities.size();

            // TD1381 - Only launch a banner if there's a corresponding monitor
            if (numMonitors < numBanners)
            {
                numBanners = numMonitors;
            }

            for (unsigned int i = 0; i < numBanners; i++)
            {
                OnlineUpdateListener::getInstance().registerInterest(TA_Base_Core::ENTITY_OWNER, *this, m_bannerEntities[i]->getKey());
                std::ostringstream commandLine;
                commandLine << "--entity-name=";
                commandLine << m_bannerEntities[i]->getName();

                RECT boundaryRect = SessionManager::getInstance().getScreenPositioning().getRect(
                    TA_Base_Bus::TA_GenericGui::SCREEN_SPECIFIC, 
                    TA_Base_Bus::TA_GenericGui::AREA_BANNER,
                    i+1);

                m_processManager->launchApplication(
                    BANNER_GUI_APPTYPE, 
                    commandLine.str(), 
                    TA_Base_Bus::TA_GenericGui::POS_BOUNDED, 
                    TA_Base_Bus::TA_GenericGui::ALIGN_FIT, 
                    TA_Base_Bus::TA_GenericGui::DEFAULT_RECT, 
                    boundaryRect, 
                    false);  // false indicates this application is not available as a running application
                
            }
			*/

            unsigned int numBanners = m_bannerEntities.size();

			std::vector<bool> bannerVisibility = SessionManager::getInstance().getScreenPositioning().getBannerVisibility();

			for (unsigned int i = 0; i < numBanners; i++)
            {
				if( bannerVisibility[i] == false )
				{
					continue;
				}
                OnlineUpdateListener::getInstance().registerInterest(TA_Base_Core::ENTITY_OWNER, *this, m_bannerEntities[i]->getKey());
                std::ostringstream commandLine;
                commandLine << "--entity-name=";
                commandLine << m_bannerEntities[i]->getName();

                RECT boundaryRect = SessionManager::getInstance().getScreenPositioning().getRect(
                    TA_Base_Bus::TA_GenericGui::SCREEN_SPECIFIC, 
                    TA_Base_Bus::TA_GenericGui::AREA_BANNER,
                    i+1);

                m_processManager->launchApplication(
                    BANNER_GUI_APPTYPE, 
                    commandLine.str(), 
                    TA_Base_Bus::TA_GenericGui::POS_BOUNDED, 
                    TA_Base_Bus::TA_GenericGui::ALIGN_FIT, 
                    TA_Base_Bus::TA_GenericGui::DEFAULT_RECT, 
                    boundaryRect, 
                    false);  // false indicates this application is not available as a running application
                
            }

        }
        catch ( const DataException& )
        {
        }
        catch ( const DatabaseException& )
        {
        }
    }
    catch (ApplicationException& )
    {
        // TD1484, TD2207 - display error message when it couldn't launch one or more banners
        CString banner;
        TA_VERIFY(banner.LoadString(IDS_BANNER)!=0, "Unable to load IDS_DISPLAY");
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << banner;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_010002);
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "ApplicationException", "Could not launch one or more of the banners");
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "loadBanners" );
}


void SessionManager::loadDllHost()
{
    FUNCTION_ENTRY( "loadDllHost" );

    if ( NULL == m_dllHostEntity )
    {
        FUNCTION_EXIT;
        return;
    }

    try
    {
        try
        {
            std::ostringstream commandLine;
            commandLine << " --" << RPARAM_CORBA_NAMING_PORT << "=" << m_dllHostEntity->getAddress();

            m_processManager->launchApplication(
                DLL_HOST_GUI_APPTYPE,
                commandLine.str(),
                TA_Base_Bus::TA_GenericGui::POS_BOUNDED,
                TA_Base_Bus::TA_GenericGui::ALIGN_FIT,
                TA_Base_Bus::TA_GenericGui::DEFAULT_RECT,
                TA_Base_Bus::TA_GenericGui::DEFAULT_RECT, 
                false);  // false indicates this application is not available as a running application
        }
        catch ( const DataException& )
        {
        }
        catch ( const DatabaseException& )
        {
        }
    }
    catch (ApplicationException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "ApplicationException", "Could not launch DllHost");
    }

    FUNCTION_EXIT;
}


void SessionManager::requestSession(TA_Base_Core::IOperator*& theOperator,
                          TA_Base_Core::IProfile*& theProfile,
                          TA_Base_Core::ILocation*& theLocation,
                          CString& password)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "requestSession" );
    try
    {
        if ( (theOperator == NULL) || (theProfile == NULL) )
        {
            TA_THROW( SessionException("Operator or profile retrieved from the login dialogue was NULL. Cannot continue to log the user in") );
        }

        unsigned int locationKey = 0;

        // if we're offline, we'll always just use the current location
        if (!m_sCGroupOnline)
        {
            // TD10804: Use the key for the consoles location, not the key for the console itself.
            locationKey = m_sessionDetails.getConsoleLocationId();
        }
        else if (theLocation != NULL)
        {
            locationKey = theLocation->getKey();
        }

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugNorm, 
            "Attempting to request a session with OperatorKey = %d, ProfileKey = %d, LocationKey = %d, ConsoleKey = %d",
            theOperator->getKey(), 
            theProfile->getKey(),
            locationKey,
            m_sessionDetails.getConsoleId());

        std::string sessionId = m_authenticationMgr->requestSession(
                                theOperator->getKey(), 
                                theProfile->getKey(),
                                locationKey,
                                m_sessionDetails.getConsoleId(),
                                std::string(password));

        m_sessionDetails.sessionLogin(sessionId, theOperator,theProfile,theLocation);

        // Tell all running applications that the session Id has changed
        m_processManager->updateSessionId(sessionId);
    }
    catch (const AuthenticationSecurityException&)
    {
        TA_THROW(SessionException(SessionException::AUTHENTICATION_FAILED, 
            "Fail to request a session"));
    }
    catch (const AuthenticationAgentException&)
    {
        TA_THROW(SessionException(SessionException::GENERAL_ERROR, 
            "Authentication agent throws an exception when requesting for a session"));
    }
    catch(TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "ObjectResolutionException", ex.what());
        
        TA_THROW(SessionException(SessionException::GENERAL_ERROR, 
            "Unknown login authentication error"));
    }
    catch (const SessionException&)
    {
        throw;
    }
    catch (...) // whatever exception that we got, just make it SessionException
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown Exception", 
            "An unknown exception is caught during login authentication");
        
        TA_THROW(SessionException(SessionException::GENERAL_ERROR, 
            "Unknown login authentication error"));
    }
    LOG( SourceInfo, DebugUtil::FunctionExit, "requestSession" );
}


EPermission SessionManager::isActionPermitted(unsigned long action, unsigned long resourceKey)
{
    IF_LOCAL_TEST
    {
        return TA_Base_App::TA_ControlStation::PERMITTED;
    }

    EPermission permission;
    try
    {
        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;

        bool allowed = m_rightsMgr->isActionPermittedOnResource(
            m_sessionDetails.getSessionId(), resourceKey, action, reason, decisionModule);

        if (allowed)
        {
            permission = TA_Base_App::TA_ControlStation::PERMITTED;
        }
        else
        {
            permission = TA_Base_App::TA_ControlStation::DENIED;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "Not permitted to perform action=%d on control station as %s", 
                action, reason.c_str());
        }
    }
    catch (const RightsException&)
    {
        // The library throws this exception when the right is either
        // indeterminate or not applicable
        permission = TA_Base_App::TA_ControlStation::UNKNOWN;
    }
    catch (...)
    {
        // The library does not catch any exception, so any exception 
        // other than the RightsException is error
        permission = TA_Base_App::TA_ControlStation::GENERAL_ERROR;
    }
    return permission;
}


EPermission SessionManager::isControlPermitted(unsigned long action)
{
    IF_LOCAL_TEST
    {
        return TA_Base_App::TA_ControlStation::PERMITTED;
    }

    TA_ASSERT(m_resourceKey != -1, "Resource key is not set");
	// adding some retry in case of corba failures.
	// set a runparam to set retry.
    EPermission isPermitted = isActionPermitted(action, m_resourceKey);
	if ((isPermitted != TA_Base_App::TA_ControlStation::PERMITTED) && 
        (isPermitted != TA_Base_App::TA_ControlStation::DENIED))
	{
		// retry to check if it's due to croba timeout or agent temporarily not available
		unsigned int retries = m_controlStationEntity->getCorbaRetries();
		for (unsigned int i=0; i<retries; i++ )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                "could not determine operator rights with sessionId=%s. retrying %d times",
                m_sessionDetails.getSessionId().c_str(), i+1);
			isPermitted = isActionPermitted(action, m_resourceKey);
			if ((isPermitted != TA_Base_App::TA_ControlStation::PERMITTED) &&
                (isPermitted != TA_Base_App::TA_ControlStation::DENIED))
			{
				// Give some sleep. But how long should we wait for retry?
				Thread::sleep(1000);
			}
			else
			{
				break;
			}
		}
	}
    return isPermitted;
}


void SessionManager::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "processUpdate" );

    switch (updateEvent.getType())
    {
    case TA_Base_Core::SECURITY:
    case TA_Base_Core::ACTION_GROUP:
        m_rightsMgr->resetCache();
        break;
    case TA_Base_Core::ENTITY_OWNER:
        {
            if (updateEvent.getKey() != m_controlStationEntity->getKey())
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                    "This is not control station update, ignore");
                LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
                return;
            }
            if ( updateEvent.getModifications() == TA_Base_Core::Create )
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, 
                    "Got 'Create' update, ignore");
                LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
                return;
            }
            if ( updateEvent.getModifications() == TA_Base_Core::Delete )
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, 
                    "Received a 'Delete' update the control station" );
                LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UW_020003);
                LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
                return;
            }
            const std::vector< std::string> changes = updateEvent.getChangedParams();
            if ( changes.empty() )
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                    "Got no update changes, ignore");
                LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
                return;
            }

            // update our control station database entity
            m_controlStationEntity->invalidate();

            using TA_Base_App::TA_ControlStation::DEBUG_LOG_DIR;
            using TA_Base_App::TA_ControlStation::USER_PREFERENCES;
            for (unsigned int i = 0; i < changes.size(); i++)
            {
                if (DEBUG_LOG_DIR.compare(changes[i]) == STRINGS_EQUIVALENT)
                {
                    RunParams::getInstance().set(DEBUG_LOG_DIR.c_str(),
                        m_controlStationEntity->getDebugLogDir().c_str());
                    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                        "DEBUG_LOG_DIR has been changed to %s", m_controlStationEntity->getDebugLogDir().c_str());
                }
                else if (USER_PREFERENCES.compare(changes[i]) == STRINGS_EQUIVALENT)
                {
                    if (true == m_controlStationEntity->getUserPreferences())
                    {
                        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                            "USER_PREFERENCES has been changed to on");
                        RunParams::getInstance().set(RPARAM_USERPREFERENCES,"on");
                    }
                    else
                    {
                        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                            "RPARAM_USERPREFERENCES has been changed to off");
                        RunParams::getInstance().set(RPARAM_USERPREFERENCES,"");
                    }
                }
                else if (ACCESS_CONTROL_POPUP_TIMEOUT.compare(changes[i]) == STRINGS_EQUIVALENT)
                {
                    RunParams::getInstance().set(ACCESS_CONTROL_POPUP_TIMEOUT.c_str(), 
                        SessionDetails::convertULtoString(m_controlStationEntity->getAccessControlPopupTimeout()).c_str());
                    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                        "AccessControlPopupTimeout has been changed to %d", m_controlStationEntity->getAccessControlPopupTimeout());
                }
            }
        }
        break;
    }
    
    // Make the control station update its displays to reflect any changes.
    ControlStationRootDialog::getInstance().PostMessage(WM_LOGIN_DETAILS_CHANGED);
	
	//Haipeng Jiang added the codes below to update the current CAD keys status
	WinkeysMgr::instance()->onRuntimeParams();

    LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
}


bool SessionManager::isRestartAccessControlled() const
{
    return m_controlStationEntity->isRestartAccessControlled();
}


bool SessionManager::isExitAccessControlled() const
{
    return m_controlStationEntity->isExitAccessControlled();
}


void SessionManager::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    try
    {
        //
        // messages from the system controller
        //
        std::string mesgTypeKey = message.messageTypeKey;
        // if we got an online message and we're already know that, then just ignore, otherwise notify
        if (0 == mesgTypeKey.compare(TA_Base_Core::SystemControllerComms::SystemControllerGroupOnline.getTypeKey()))
        {
            if (!m_sCGroupOnline)
            {
                char const* groupString = "";
                if((message.messageState >>= groupString) == 0)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve the group string for SystemControllerGroupOnline message");
                    groupString = "Unknown";
                }

                m_sCGroupOnline = true;
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                    "Received a SystemControllerGroupOnline message");
                // tell the control station root dialog to display notification
                // update the session id of all 
                std::string* group = new std::string(groupString);
                ControlStationRootDialog::getInstance().PostMessage(WM_SYSTEMCONTROLLER_STATE_CHANGE, (WPARAM)true, (LPARAM)group);
            }
        }
    
        // if we got an offline message and we're already know that, then just ignore, otherwise notify
        if (0 == mesgTypeKey.compare(TA_Base_Core::SystemControllerComms::SystemControllerGroupOffline.getTypeKey()))
        {
            if (m_sCGroupOnline)
            {
                char const* groupString = "";
                if((message.messageState >>= groupString) == 0)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve the group string for SystemControllerGroupOffline message");
                    groupString = "Unknown";
                }

                m_sCGroupOnline = false;
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                    "Received a SystemControllerGroupOffline message");
                // tell the control station root dialog to display notification
                // update the session id of all 
                std::string* group = new std::string(groupString);
                ControlStationRootDialog::getInstance().PostMessage(WM_SYSTEMCONTROLLER_STATE_CHANGE, (WPARAM)false, (LPARAM)group);
            }
        }

        // GX targeted message
        if (0 == mesgTypeKey.compare(TA_Base_Core::ControlStationComms::TAToGXMessageNotification.getTypeKey()))
        {
		    processTAToGxMessage(message);
        }

		//lizettejl++ (DP-changes) 
		// TA targeted message
        if (0 == mesgTypeKey.compare(TA_Base_Core::ControlStationComms::GXToTAMessageNotification.getTypeKey()))
        {
		    processGxToTAMessage(message);
        }
		//++lizettejl

		if (0 == mesgTypeKey.compare(TA_Base_Core::ControlStationComms::LaunchApplication.getTypeKey()))
		{
			processLaunchApplicationMessage(message);
			return;
		}
        // Added for TD 6950: Some GUIs need to know when the system is in degraded mode.
        // Refresh the run params as the group online value would have changed
        retrieveRunParams(m_hostName);
    }
    catch (...) 
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught some error when processing specialised message");
    }
}

void SessionManager::processTAToGxMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    TA_Base_Core::IControlStationCorbaDef::TAToGXMessage* data;

    // can't extract to TAToGXMessage, ignore
	if ( ( message.messageState >>= data ) == 0)
	{
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Received a TAToGXMessageNotification but couldn't extract TAToGXMessage.  Ignore");
        return;
    }

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
        "Received a TAToGXMessageNotification message");
	// Tell the control station root dialog to fire the message to Graphworx
	TAToGxMessageInformation *pGxMsg = new TAToGxMessageInformation();
	pGxMsg->targetSessionID = data->targetSessionID;
	pGxMsg->targetSchematicID = data->targetSchematicID;
	pGxMsg->sourceEntityName = data->sourceEntityName;
	pGxMsg->message = data->message;
    ControlStationRootDialog::getInstance().PostMessage(WM_TA_TO_GX_MESSAGE, (WPARAM)pGxMsg);
}

void SessionManager::processGxToTAMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
	try
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
			"Received a GXToTAMessageNotification message");
		TA_Base_Core::IManagedApplicationCorbaDef::GXToTAMessage* event;
		
		if ((message.messageState >>= event) == 0)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
				"Failed to interpret message");
			return;
		}
		else
		{				
			if (event->entityType == TA_Base_Core::IManagedApplicationCorbaDef::ControlStation)
			{
				// Create message copy on the heap to make sure
				// the handler for WM_GX_TO_TA_MESSAGE has a valid
				// object to work with.
				TA_Base_Core::IManagedApplicationCorbaDef::GXToTAMessage *copyTAMessage = 
					new struct TA_Base_Core::IManagedApplicationCorbaDef::GXToTAMessage();
				
				copyTAMessage->sessionId = event->sessionId;
				copyTAMessage->sourceSchematicID = event->sourceSchematicID;
				copyTAMessage->entityType = event->entityType;
				copyTAMessage->message = event->message;					
				ControlStationRootDialog::getInstance().PostMessage(WM_GX_TO_TA_MESSAGE, 
					reinterpret_cast<WPARAM>(copyTAMessage));
			}
		}
	}	
	catch (...) 
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught some error when processing specialized message");
	}	
}

void SessionManager::processLaunchApplicationMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Receive a LaunchApplication message");
	unsigned long locationKey = 0;
	unsigned long appType = 0;

	try
	{
		IProcessManager* mgr = getProcessManager();
		if (NULL != mgr)
		{
			TA_Base_Core::IControlStationCorbaDef::LaunchApplicationEvent* event;
			if((message.messageState >>= event) == 0)
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
					"Received a LaunchApplication but couldn't extract LaunchApplicationEvent.  Ignore");
				return;
			}
			else
			{
				locationKey = event->locationKey;
				appType = event->applicationType;
				std::string strCommandLine = event->runParameter.in();
				static unsigned long occLocationKey = TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey();

				if (BAS_EMERGENCY_MODE_MANAGER == appType)
				{
					static unsigned long BasManagerResuouceKey = TA_Base_Core::DatabaseKey::getInvalidKey();
					if ( TA_Base_Core::DatabaseKey::isInvalidKey( BasManagerResuouceKey ))
					{
                        try
                        {
                            std::auto_ptr<TA_Base_Core::IEntityData> ptrData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity("BasModeManager"));
                            BasManagerResuouceKey = ptrData->getKey();
                        }
                        catch (...)
                        {
                        }
                        if ( TA_Base_Core::DatabaseKey::isInvalidKey( BasManagerResuouceKey ))
                        {
                            BasManagerResuouceKey = m_resourceKey;
                        }
					}
					if (DENIED == isActionPermitted(TA_Base_Bus::aca_SET_MODE_ECS, BasManagerResuouceKey))
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "This user don't have the right to control set mode. ignore.");
						return;
					}

					if (getSessionDetails().getLocationKey() == locationKey ||
						getSessionDetails().getLocationKey() == occLocationKey)
					{
						RECT boundaryRect = SessionManager::getInstance().getScreenPositioning().getRect(
							TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, 
							TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC);

						mgr->launchApplication(appType,strCommandLine, 
							TA_Base_Bus::TA_GenericGui::POS_BOUNDED, 
							TA_Base_Bus::TA_GenericGui::ALIGN_FIT, 
							TA_Base_Bus::TA_GenericGui::DEFAULT_RECT, boundaryRect);
						sendLaunchApplicationACK(locationKey, appType, true);
					}
					else
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "this launch application message not match this MFT. MessageLocationKey[%u]", event->locationKey);
					}
				}
				else // if you want add some app to lauch, please modify this.
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "do not support this application launch.");
				}
				
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "User not login, so we will NOT process this message.");
		}
	}
	catch(TA_Base_Core::TransactiveException& err)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
		sendLaunchApplicationACK(locationKey, appType, false);
	}
	catch(...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when processLaunchApplicationMessage");
		sendLaunchApplicationACK(locationKey, appType, false);
	}
}

void SessionManager::sendLaunchApplicationACK(unsigned long locationKey, unsigned long appType, bool bSuccess)
{
	TA_Base_Core::IControlStationCorbaDef::LaunchApplicationACKEvent event;
	CORBA::Any data;
	
	event.locationKey = locationKey;
	event.applicationType = appType;
	event.bSuccess = bSuccess;

	data <<= event;

	TA_Base_Core::CommsMessageSender* commsMessageSender =
		TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::ControlStationComms::LaunchApplicationACK );

	commsMessageSender->sendBroadcastCommsMessage(TA_Base_Core::ControlStationComms::LaunchApplicationACK,
		getControlStationEntity().getKey(),	data, NULL);
}

void SessionManager::exit(const bool isRestart /*=false*/)
{
    FUNCTION_ENTRY("exit");

    s_exitNeedsRestart = isRestart;

    // Submit an audit with the name of the console on which this application is running
    unsigned long entityKey = 0;
    try
    {
        if (m_controlStationEntity != NULL)
        {
            entityKey = m_controlStationEntity->getKey();
        }
    }
    catch(...)
    {
        // If there is any type of exception thrown, then go with the default value. Log the failure
        LOG_EXCEPTION_CATCH(SourceInfo, "General exception", 
            "An exception was caught while attempting to retrieve the Control Station entity key. Proceeding with default value");
    }

    if ( RunParams::getInstance().isSet(RPARAM_LOCATIONKEY) 
        && !RunParams::getInstance().get(RPARAM_NOTIFYHOSTS).empty()
        && RunParams::getInstance().get(RPARAM_LOCATIONKEY) != "0")
    {
        std::string consoleName = m_sessionDetails.getConsoleName();

        // Get the audit message sender with which to send the message
        TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().
            getAuditMessageSender(TA_Base_Core::ControlStationAudit::Context);

        TA_Base_Core::DescriptionParameters params;

        // TODO: This is just a dodgy hack at the moment so that I don't have to update the message corba
        // register to use ConsoleName or update the audit message sender to use case insensitive comparison
        // when comparing the placeholder names.
        TA_Base_Core::NameValuePair param1("consolename",consoleName);
        params.push_back(&param1);

        sender->sendAuditMessage( isRestart ? TA_Base_Core::ControlStationAudit::MFTRestart : TA_Base_Core::ControlStationAudit::ControlStationExit,
                                 entityKey,
                                 params,
                                 "N/A",
                                 m_sessionDetails.getSessionId(),
                                 "",
                                 "",
                                 "" );

        delete sender;
        sender = NULL;
    }

    FUNCTION_EXIT;
}


void SessionManager::renewSession()
{
    FUNCTION_ENTRY("renewSession");

    if (RunParams::getInstance().get(RPARAM_SESSIONID).empty())
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "No operator is logged in, so no need to renew session");
        return;
    }

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "An operator is logged in, so need to renew session");
    try
    {
        SessionDetails::SessionInfo sessionInfo = m_sessionDetails.getSessionInfo();
        std::string sessionId = m_authenticationMgr->requestSession(sessionInfo.operatorId, sessionInfo.profileId, sessionInfo.locationId, sessionInfo.consoleId, sessionInfo.password);

        // if the session Id returned by the Authentication Agent is still the same as the existing one, 
        // it means the Authentication Agent still regard the session as active.  So don't need to 
        // perform override and update the session ID then.
        if (sessionId == m_sessionDetails.getSessionId())
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Session remains unchanged.  No need to check for override and update session details");
            return;
        }

    	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Requested a new session from the Authentication Agent.");
        // if the session is overriden, we need to perform an override to make sure the session has the same privilege
        // as before the control station lost connection to the network
        if (sessionInfo.isOverriden)
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Session is overriden, so need to override this new session");
            m_authenticationMgr->beginOverride(sessionId, sessionInfo.overrideOperatorId, sessionInfo.overrideProfileId, sessionInfo.locationId, sessionInfo.overridePassword);
        }

        // update the session details
        m_sessionDetails.renewSession(sessionId);

        // Tell all running applications that the session Id has changed
        m_processManager->updateSessionId(sessionId);
    }
    catch (const AuthenticationSecurityException&)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, 
            "AuthenticationSecurityException", "re-authentication failed.  No need to retry");
    }
    catch (const AuthenticationAgentException&)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, 
            "AuthenticationAgentException", "re-authentication failed.  No need to retry");
    }
    catch (...) // whatever exception that we got, just make it SessionException
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown Exception", 
            "An unknown exception is caught during re-authentication.  No need to retry");
    }

    FUNCTION_EXIT;
}

void SessionManager::areActionsPermitted(unsigned long action, ResourceList& resourceKeyList,ResourceList& permittedResourceKeyList)
{
	permittedResourceKeyList.clear();

	try
	{
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
		std::string reason;
		m_rightsMgr->isActionPermittedOnResources(
			m_sessionDetails.getSessionId(), resourceKeyList,permittedResourceKeyList, action, reason, decisionModule);

	}
	catch (const RightsException&)
	{
		// The library throws this exception when the right is either
		// indeterminate or not applicable
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Failed to get the permitted resource list!");
	}
	catch (...)
	{
		// The library does not catch any exception, so any exception 
		// other than the RightsException is error
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Failed to get the permitted resource list!");
	}

}
