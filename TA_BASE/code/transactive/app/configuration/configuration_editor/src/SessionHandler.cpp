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
  * This is a replacement for GenericGUI. If the Control Station is not running
  * then this class performs similar functionality to GenericGUI (minus the
  * requirement on the Control Station).
  */
#include "app/configuration/configuration_editor/src/StdAfx.h"
#include "app/configuration/configuration_editor/src/RightsChecker.h"
#include "app/configuration/configuration_editor/src/DatabaseCache.h"
#include "app/configuration/configuration_editor/src/SessionHandler.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/SystemControllerDlg.h"
#include "app/configuration/configuration_editor/src/TransActiveLoginDlg.h"
#include "app/configuration/configuration_editor/src/DatabaseLoginDlg.h"
#include "bus/generic_gui/src/IGUIApplication.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/GuiAccessFactory.h"
#include "core/data_access_interface/src/IGui.h"
#include "core/message/src/TransactiveMessageSubscription.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/PublicationManager.h"
#include "core/message/src/ConnectionChecker.h"
#include "core/exceptions/src/CannotUseSysControllerException.h"
#include "core/exceptions/src/UserCancelException.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/EntityTypeException.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/Hostname.h"

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::GenericGUIException;

namespace TA_Base_App
{
    SessionHandler::SessionHandler(TA_Base_Bus::IGUIApplication& gui)
        : m_guiApplication(gui),
          m_guiEntity(NULL),
          m_authentication(NULL)
    {
        try
        {
            checkCommandLine();

            // Register to be notified of changes to the Debug parameters.
            // It is possible these could change when Managed Process is set up.
            //
            RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGFILE);
            RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGLEVEL);
            RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGFILEMAXSIZE);
            RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGMAXFILES);
            RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGPIDFILENAMES);
            RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGLEVELOFF);
            RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGLEVELFILE);

            // We must set the OperationMode to be Control so that the rebinding of names works.
            RunParams::getInstance().set(RPARAM_OPERATIONMODE, RPARAM_CONTROL);
        }
        catch (...)
        {
            RunParams::getInstance().deregisterRunParamUser(this);
            throw;
        }
    }

    SessionHandler::~SessionHandler()
    {
        try
        {
            RunParams::getInstance().deregisterRunParamUser(this);

            if (m_guiEntity != NULL)
            {
                delete m_guiEntity;
                m_guiEntity = NULL;
            }

            if (m_authentication != NULL && !isControlStationRunning())
            {
                try
                {
                    m_authentication->endSession(RunParams::getInstance().get(RPARAM_SESSIONID));
                }
                catch (const TA_Base_Core::AuthenticationSecurityException&)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationSecurityException", "Could not end the Configuration Editor session");
                }
                catch (const TA_Base_Core::AuthenticationAgentException&)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationAgentException", "Could not end the Configuration Editor session");
                }
                catch (const TA_Base_Core::ObjectResolutionException&)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not end the Configuration Editor session");
                }

                delete m_authentication;
                m_authentication = NULL;
            }

            TA_Base_Core::Naming::cleanUp();
            TA_Base_Core::gDestroySubscriptionManager();
            TA_Base_Core::gTerminateAndDeleteStructuredEventSuppliers();
            TA_Base_Core::ChannelLocatorConnectionMgr::removeInstance();
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught in destructor");
        }
    }

    void SessionHandler::checkCommandLine()
    {
        FUNCTION_ENTRY("checkCommandLine");

        //
        // Check the command line passed was correct. We will check entity separately so
        // this is just for any extra params. This may throw an exception. This should be
        // thrown straight up the the calling class as we can't deal with it.
        //
        m_guiApplication.checkCommandLine();

        FUNCTION_EXIT;
    }

    void SessionHandler::setUpGUIEntity()
    {
        FUNCTION_ENTRY("setUpGUIEntity");

        //
        // Retrieve the entity name from RunParams to ensure it was on the command line.
        //
        std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);

        if (entityName.empty())
        {
            TA_THROW(GenericGUIException("No entity specified on command line (ie --entity-name not found)", GenericGUIException::COMMAND_LINE_INVALID));
        }

        //
        // Load the entity and check the configuration is correct
        //
        try
        {
            //m_guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);
            m_guiEntity = DatabaseCache::getInstance().getGuiEntity();
            m_guiApplication.checkEntity(m_guiEntity);
        }
        catch (const TA_Base_Core::DatabaseException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Will change this exception into a GenericGUIException. We don't care what happened just that we can't connect to the database");
            TA_THROW(GenericGUIException(GenericGUIException::NO_DATABASE_CONNECTION));
        }
        catch (const TA_Base_Core::EntityTypeException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "EntityTypeException", "Will change this exception into a GenericGUIException.");
            TA_THROW(GenericGUIException(GenericGUIException::ENTITY_CONFIGURATION_INVALID));
        }
        catch (const TA_Base_Core::DataException& ex)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Will change this exception into a GenericGUIException.");

            switch (ex.getFailType())
            {
                using TA_Base_Core::GenericGUIException;

            case (TA_Base_Core::DataException::WRONG_TYPE):
                TA_THROW(GenericGUIException(GenericGUIException::ENTITY_CONFIGURATION_INVALID));
                break;

            case (TA_Base_Core::DataException::NO_VALUE):
                TA_THROW(GenericGUIException(GenericGUIException::ENTITY_DOES_NOT_EXIST));
                break;

            case (TA_Base_Core::DataException::NOT_UNIQUE):
                TA_THROW(GenericGUIException(GenericGUIException::ENTITY_NOT_UNIQUE));
                break;

            default:
                TA_ASSERT(false, "Do not recognise the DataException type");
            }
        }

        FUNCTION_EXIT;
    }

    bool SessionHandler::run()
    {
        FUNCTION_ENTRY("run");

        try
        {
            try
            {
                connectToSystemController();
            }
            catch (const TA_Base_Core::CannotUseSysControllerException& ex)
            {
                // This exception is thrown if we could not connect using the system controller for some reason.
                // what() holds why we couldn't connect so that it can be displayed to the user
                // Any other exceptions are un-recoverable errors and we cannot handle them
                LOG_EXCEPTION_CATCH(SourceInfo, "CannotUseSysControllerException", "Cannot connect to system controller. We will attempt to connect to the database instead");
                connectToDatabase(ex.what());
            }
        }
        catch (const TA_Base_Core::UserCancelException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "UserCancelException", "User has cancelled the running of the Configuration Editor");
            throw;
        }

        // Ok we've connected successfully if we get to here

        // Check if our help file is set. If it is not then we need to retrieve it ourselves.
        configureHelpFile();

        bool shouldStartMessagePump = m_guiApplication.displayApplication();

        FUNCTION_EXIT;
        return shouldStartMessagePump;
    }

    void SessionHandler::connectToDatabase(const std::string& reasonMessage)
    {
        FUNCTION_ENTRY("connectToDatabase");

        DatabaseLoginDlg dlg(reasonMessage.c_str());

        if (dlg.DoModal() == IDCANCEL)
        {
            TA_THROW(TA_Base_Core::UserCancelException());
        }

        // Now we have a database connection we can set up the Configuration Editor's entity
        setUpGUIEntity();

        FUNCTION_EXIT;
    }

    void SessionHandler::connectToSystemController()
    {
        FUNCTION_ENTRY("connectToSystemController");

        std::string hostName = RunParams::getInstance().get(TA_Configuration::RPARAM_HOSTNAME.c_str());

        if (hostName.empty())  // If the port is empty we just use the default port. This is what the user wants most of the time anyway
        {
            SystemControllerDlg dlg;

            if (dlg.DoModal() == IDCANCEL)
            {
                TA_THROW(TA_Base_Core::UserCancelException());
            }
        }

        // Attempt to connect to system controller
        locateSystemController();

        retrieveRunParams(); // from the System Controller

        if (RunParams::getInstance().get(RPARAM_NOTIFYHOSTS).empty())
        {
            TA_THROW(TA_Base_Core::CannotUseSysControllerException("The System Controller specified cannot be used from this workstation"));
        }

        setUpGUIEntity();  // Now we have a database connection we can set up the Configuration Editor's entity

        // Load the authentication agent library
        try
        {
            m_authentication = new TA_Base_Bus::AuthenticationLibrary;
        }
        catch (const TA_Base_Core::DataException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Cannot create the authentication library. This is because the authentication agent configuration has not yet been set up so we go straight to the database login instead");
            TA_THROW(TA_Base_Core::CannotUseSysControllerException("No entity is configured for the Authentication Agent"));
        }
        catch (const TA_Base_Core::DatabaseException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Cannot create the authentication library. This is because the authentication agent configuration has not yet been set up so we go straight to the database login instead");
            TA_THROW(TA_Base_Core::CannotUseSysControllerException("No entity is configured for the Authentication Agent"));
        }

        // Do login dialog

        TransActiveLoginDlg dlg(*this);

        if (dlg.DoModal() == IDCANCEL)
        {
            TA_THROW(TA_Base_Core::UserCancelException());
        }

        // Ok if we get to here the user is authenticated and has permission to run this app we are done

        FUNCTION_EXIT;
    }

    void SessionHandler::locateSystemController()
    {
        using TA_Base_Core::RunParams;
        using TA_Base_Core::CorbaUtil;

        FUNCTION_ENTRY("locateSystemController");

        //
        // Convert the portnumber into an unsigned long
        //
        unsigned long portNumber = TA_Base_Core::DEFAULT_MGR_PORT;

        std::string portNumberString = RunParams::getInstance().get(TA_Configuration::RPARAM_PORTNUMBER.c_str());

        if (!portNumberString.empty())
        {
            if (std::string::npos == portNumberString.find_first_not_of("0123456789"))
            {
                //all valid characters
                std::istringstream portNumberStream(portNumberString);
                portNumberStream >> portNumber;

                if (0 == portNumber)
                {
                    portNumber = TA_Base_Core::DEFAULT_MGR_PORT;
                }
            }
        }

        std::string hostName = RunParams::getInstance().get(TA_Configuration::RPARAM_HOSTNAME.c_str());

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Host Name = %s, Port Number = %lu", hostName.c_str(), portNumber);

        CString procManString;
        CORBA::Object_var obj;
        procManString.Format("corbaloc::%s:%lu/ProcessManager", hostName.c_str(), portNumber);

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Process Manager IOR = %s", std::string(procManString).c_str());

        try
        {
            // Attempt to get Process Manager
            obj = CorbaUtil::getInstance().stringToObject(procManString.GetBuffer(1));

            if (CORBA::is_nil(obj))
            {
                TA_THROW(TA_Base_Core::CannotUseSysControllerException("System Controller cannot be located"));
            }

            m_systemController = TA_Base_Core::IProcessManagerCorbaDef::_narrow(obj);

            if (CORBA::is_nil(m_systemController))
            {
                TA_THROW(TA_Base_Core::CannotUseSysControllerException("System Controller cannot be located"));
            }
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Cannot find system controller. Converting into a SystemController exception");
            TA_THROW(TA_Base_Core::CannotUseSysControllerException("System Controller cannot be located"));
        }

        FUNCTION_EXIT;
    }

    void SessionHandler::retrieveRunParams()
    {
        FUNCTION_ENTRY("retrieveRunParams");

        try
        {
            std::string hostname = TA_Base_Core::Hostname::getHostname();
            TA_Base_Core::RunParamSeq_var tmpParams = m_systemController->getParams(0, hostname.c_str());

            // For each RunParam we have retrieved. If it is not already set then set it.
            for (unsigned long i = 0; i < tmpParams->length(); ++i)
            {
                if (!RunParams::getInstance().isSet(tmpParams[i].name))
                {
                    RunParams::getInstance().set(tmpParams[i].name, tmpParams[i].value);
                }
            }
        }
        catch (const CORBA::Exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "");
            TA_THROW(GenericGUIException(GenericGUIException::INITIALISATION_ERRORS));
        }

        FUNCTION_EXIT;
    }

    void SessionHandler::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY("onRunParamChange");

        // New debug file name
        if (0 == name.compare(RPARAM_DEBUGFILE))
        {
            DebugUtil::getInstance().setFile(value.c_str());
        }
        // New debug level
        else if (0 == name.compare(RPARAM_DEBUGLEVEL))
        {
            DebugUtil::getInstance().setLevel(DebugUtil::getDebugLevelFromString(value.c_str()));
        }
        // New max file size
        else if (0 == name.compare(RPARAM_DEBUGFILEMAXSIZE))
        {
            std::istringstream strValue(value);
            int maxSize;
            strValue >> maxSize;
            DebugUtil::getInstance().setMaxSize(maxSize);
        }
        // New max number of debug files
        else if (0 == name.compare(RPARAM_DEBUGMAXFILES))
        {
            std::istringstream strValue(value);
            int maxFiles;
            strValue >> maxFiles;
            DebugUtil::getInstance().setMaxFiles(maxFiles);
        }
        // Use pid in filenames
        else if (0 == name.compare(RPARAM_DEBUGPIDFILENAMES))
        {
            DebugUtil::getInstance().encryptPidInFilenames(value);
        }
        // Turn off logging for these debug levels
        else if (0 == name.compare(RPARAM_DEBUGLEVELOFF))
        {
            DebugUtil::getInstance().decodeDisabledLevels(value);
        }
        // Filenames for debug levels
        else if (0 == name.compare(RPARAM_DEBUGLEVELFILE))
        {
            DebugUtil::getInstance().decodeLevelFilenames(value);
        }

        // There is no way the session id can change for the user if they have logged in using this
        // class so we do not need to worry about listening for updates.

        FUNCTION_EXIT;
    }

    void SessionHandler::authenticate(const ISessionChecker::SessionDetails& sessionDetails)
    {
        FUNCTION_ENTRY("authenticate");

        m_sessionId = m_authentication->requestSession(sessionDetails.operatorKey,
                              sessionDetails.profileKey,
                              sessionDetails.locationKey,
                              sessionDetails.consoleKey,
                              sessionDetails.password);
        RunParams::getInstance().set(RPARAM_SESSIONID, m_sessionId.c_str());

        FUNCTION_EXIT;
    }

    void SessionHandler::checkUserPermissions()
    {
        FUNCTION_ENTRY("checkUserPermissions");

        if (!RightsChecker::getInstance().canPerformAction(TA_Base_Bus::aca_RUN_APPLICATION))
        {
            TA_THROW(GenericGUIException(GenericGUIException::ACCESS_DENIED));
        }

        FUNCTION_EXIT;
    }

    void SessionHandler::configureHelpFile()
    {
        FUNCTION_ENTRY("configureHelpFile");

        if (!RunParams::getInstance().get(RPARAM_HELPFILE).empty())
        {
            // Already set
            FUNCTION_EXIT;
            return;
        }

        TA_Base_Core::IGui* gui = NULL;

        try
        {
            gui = TA_Base_Core::GuiAccessFactory::getInstance().getGui(m_guiApplication.getApplicationType());
            std::string helpFile = gui->getHelpFilePath();

            if (!helpFile.empty())
            {
                RunParams::getInstance().set(RPARAM_HELPFILE, helpFile.c_str());
            }
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not set the help file for this application. No help will be available");
        }

        if (gui != NULL)
        {
            delete gui;
            gui = NULL;
        }

        FUNCTION_EXIT;
    }

    unsigned long SessionHandler::getEntityKey()
    {
        FUNCTION_ENTRY("getEntityKey");

        if (m_guiEntity != NULL)
        {
            return m_guiEntity->getKey();
        }
        else
        {
            return 0;
        }

        FUNCTION_EXIT;
    }

    bool SessionHandler::isControlStationRunning()
    {
        locateProcessManager();

        if (CORBA::is_nil(m_ProcessManager))
        {
            return false;
        }

        try
        {
            m_ProcessManager->poll();
            return true;
        }
        catch (...)
        {
        }

        return false;
    }

    void SessionHandler::locateProcessManager()
    {
        try
        {
            try
            {
                if (!CORBA::is_nil(m_ProcessManager))
                {
                    m_ProcessManager->poll();
                    return;
                }
            }
            catch (...)
            {
            }

            m_ProcessManager = TA_Base_Core::IProcessManagerCorbaDef::_nil();
            unsigned long port = TA_Base_Core::getRunParamValue(RPARAM_MGRPORT, TA_Base_Core::DEFAULT_MGR_PORT);
            std::stringstream ior;
            ior << "corbaloc::localhost:" << port << "/ProcessManager";
            TA_Base_Core::RunParams::getInstance().set("ProcessManagerIor", ior.str().c_str());
            CORBA::Object_var obj = TA_Base_Core::CorbaUtil::getInstance().stringToObject(ior.str());

            if (!CORBA::is_nil(obj))
            {
                m_ProcessManager = TA_Base_Core::IProcessManagerCorbaDef::_narrow(obj);
            }
        }
        catch (...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Cannot locate ProcessManager");
        }
    }
}
